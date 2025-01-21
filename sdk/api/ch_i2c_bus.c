#include "ch_i2c_bus.h"
#include "CH59x_common.h"
#include "ch_err.h"

#define MASTER_ADDR 0x52

#define TAG "I2C_BUS"

typedef struct {
  int i2c_port;
  bool is_init;             /*if bus is initialized*/
  i2c_config_t conf_active; /*!<I2C active configuration */
  int32_t ref_counter;      /*reference count*/
} i2c_bus_t;

typedef struct {
  uint8_t dev_addr;   /*device address*/
  i2c_config_t conf;  /*!<I2C active configuration */
  i2c_bus_t *i2c_bus; /*!<I2C bus*/
} i2c_bus_device_t;

static i2c_bus_t s_i2c_bus[I2C_NUM_MAX];

#define I2C_BUS_CHECK(a, str, ret) \
  if (!(a)) { \
    CH_LOGI(TAG,"%s:%d (%s): %s", __FILE__, __LINE__, __FUNCTION__, str); \
    return (ret); \
  }

#define I2C_BUS_CHECK_GOTO(a, str, label) \
  if (!(a)) { \
    CH_LOGI(TAG,"%s:%d (%s): %s", __FILE__, __LINE__, __FUNCTION__, str); \
    goto label; \
  }

#define I2C_BUS_INIT_CHECK(is_init, ret) \
  if (!is_init) { \
    CH_LOGI(TAG,"%s:%d (%s): i2c_bus has not been initialized", __FILE__, __LINE__, __FUNCTION__); \
    return (ret); \
  }

static int i2c_driver_reinit(i2c_port_t port, const i2c_config_t *conf);
static int i2c_driver_deinit(i2c_port_t port);
static int i2c_bus_write_reg8(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, size_t data_len, const uint8_t *data);
static int i2c_bus_read_reg8(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, size_t data_len, uint8_t *data);
inline static bool i2c_config_compare(i2c_port_t port, const i2c_config_t *conf);
/**************************************** Public Functions (Application level)*********************************************/

i2c_bus_handle_t i2c_bus_create(i2c_port_t port, const i2c_config_t *conf)
{
  I2C_BUS_CHECK(port < I2C_NUM_MAX, "I2C port error", NULL);
  I2C_BUS_CHECK(conf != NULL, "pointer = NULL error", NULL);
  I2C_BUS_CHECK(conf->mode == I2C_MODE_MASTER, "i2c_bus only supports master mode", NULL);

  if(s_i2c_bus[port].is_init) {
    /**if i2c_bus has been inited and configs not changed, return the handle directly**/
    if(i2c_config_compare(port, conf)) {
      CH_LOGI(TAG,"i2c%d has been inited, return handle directly, ref_counter=%"PRIu32" ", port, s_i2c_bus[port].ref_counter);
      return (i2c_bus_handle_t)&s_i2c_bus[port];
    }
  }
  else {
    s_i2c_bus[port].ref_counter = 0;
  }

  uint8_t ret = i2c_driver_reinit(port, conf);
  I2C_BUS_CHECK(ret == CH_OK, "init error", NULL);
  s_i2c_bus[port].conf_active = *conf;
  s_i2c_bus[port].i2c_port = port;
  return (i2c_bus_handle_t)&s_i2c_bus[port];
  
}

int i2c_bus_delete(i2c_bus_handle_t *p_bus)
{
  I2C_BUS_CHECK(p_bus != NULL && *p_bus != NULL, "pointer = NULL error", CH_ERR_INVALID_ARG);
  i2c_bus_t *i2c_bus = (i2c_bus_t *)(*p_bus);
  I2C_BUS_INIT_CHECK(i2c_bus->is_init, CH_FAIL);

  /** if ref_counter == 0, de-init the bus**/
  if ((i2c_bus->ref_counter) > 0) {
    CH_LOGI(TAG,"i2c%d is also handled by others ref_counter=%"PRIu32", won't be de-inited", i2c_bus->i2c_port, i2c_bus->ref_counter);
    return CH_OK;
  }

  int ret = i2c_driver_deinit(i2c_bus->i2c_port);
  I2C_BUS_CHECK(ret == CH_OK, "deinit error", ret);
  *p_bus = NULL;
  return CH_OK;
}

uint8_t i2c_bus_scan(i2c_bus_handle_t bus_handle ,uint8_t *buf , uint8_t num)
{
  I2C_BUS_CHECK(bus_handle != NULL, "Handle error", 0);
  i2c_bus_t *i2c_bus = (i2c_bus_t *)bus_handle;
  I2C_BUS_INIT_CHECK(i2c_bus->is_init, 0);

  uint8_t device_count = 0;
  for(uint8_t dev_address = 1; dev_address < 127 ; dev_address++)
  {
  
    while(I2C_GetFlagStatus(I2C_FLAG_BUSY) != RESET);
    I2C_GenerateSTART(ENABLE);

    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(dev_address << 1, I2C_Direction_Transmitter);
    DelayMs(10);
    if(I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) 
    {
      CH_LOGI(TAG,"found i2c device address  = 0x%02x ", dev_address );
      if (buf != NULL && device_count < num) {
        *(buf + device_count) = dev_address;
      }
      device_count++;
    }
    I2C_GenerateSTOP(ENABLE);
  }
  return device_count;
}

uint32_t i2c_bus_get_current_clk_speed(i2c_bus_handle_t bus_handle)
{
  I2C_BUS_CHECK(bus_handle != NULL, "Null Bus Handle", 0);
  i2c_bus_t *i2c_bus = (i2c_bus_t *)bus_handle;
  I2C_BUS_INIT_CHECK(i2c_bus->is_init, 0);
  return i2c_bus->conf_active.master.clk_speed;
}

uint8_t i2c_bus_get_created_device_num(i2c_bus_handle_t bus_handle)
{
  I2C_BUS_CHECK(bus_handle != NULL, "Null Bus Handle", 0);
  i2c_bus_t *i2c_bus = (i2c_bus_t *)bus_handle;
  I2C_BUS_INIT_CHECK(i2c_bus->is_init, 0);
  return i2c_bus->ref_counter;
}

i2c_bus_device_handle_t i2c_bus_device_create(i2c_bus_handle_t bus_handle, uint8_t dev_addr, uint32_t clk_speed)
{
  I2C_BUS_CHECK(bus_handle != NULL, "Null Bus Handle", NULL);
  I2C_BUS_CHECK(clk_speed <= 400000, "clk_speed must <= 400000", NULL);
  i2c_bus_t *i2c_bus = (i2c_bus_t *)bus_handle;
  I2C_BUS_INIT_CHECK(i2c_bus->is_init, NULL);
  i2c_bus_device_t *i2c_device = calloc(1, sizeof(i2c_bus_device_t));
  I2C_BUS_CHECK(i2c_device != NULL, "calloc memory failed", NULL);
  i2c_device->dev_addr = dev_addr;
  i2c_device->conf = i2c_bus->conf_active;

  /*if clk_speed == 0, current active clock speed will be used, else set a specified value*/
  if (clk_speed != 0) {
    i2c_device->conf.master.clk_speed = clk_speed;
  }

  i2c_device->i2c_bus = i2c_bus;
  i2c_bus->ref_counter++;
  return (i2c_bus_device_handle_t)i2c_device;
}

int i2c_bus_device_delete(i2c_bus_device_handle_t *p_dev_handle)
{
  I2C_BUS_CHECK(p_dev_handle != NULL && *p_dev_handle != NULL, "Null Device Handle", CH_ERR_INVALID_ARG);
  i2c_bus_device_t *i2c_device = (i2c_bus_device_t *)(*p_dev_handle);
  i2c_device->i2c_bus->ref_counter--;
  free(i2c_device);
  *p_dev_handle = NULL;
  return CH_OK;
}

uint8_t i2c_bus_device_get_address(i2c_bus_device_handle_t dev_handle)
{
  I2C_BUS_CHECK(dev_handle != NULL, "device handle error", NULL_I2C_DEV_ADDR);
  i2c_bus_device_t *i2c_device = (i2c_bus_device_t *)dev_handle;
  return i2c_device->dev_addr;
}

int i2c_bus_read_bytes(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, size_t data_len, uint8_t *data)
{
  return i2c_bus_read_reg8(dev_handle, mem_address, data_len, data);
}

int i2c_bus_read_byte(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, uint8_t *data)
{
  return i2c_bus_read_reg8(dev_handle, mem_address, 1, data);
}

int i2c_bus_read_bit(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, uint8_t bit_num, uint8_t *data)
{
  uint8_t byte = 0;
  int ret = i2c_bus_read_reg8(dev_handle, mem_address, 1, &byte);
  *data = byte & (1 << bit_num);
  *data = (*data != 0) ? 1 : 0;
  return ret;
}

int i2c_bus_read_bits(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, uint8_t bit_start, uint8_t length, uint8_t *data)
{
  uint8_t byte = 0;
  int ret = i2c_bus_read_byte(dev_handle, mem_address, &byte);

  if (ret != CH_OK) {
    return ret;
  }

  uint8_t mask = ((1 << length) - 1) << (bit_start - length + 1);
  byte &= mask;
  byte >>= (bit_start - length + 1);
  *data = byte;
  return ret;
}

int i2c_bus_write_byte(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, uint8_t data)
{
  return i2c_bus_write_reg8(dev_handle, mem_address, 1, &data);
}

int i2c_bus_write_bytes(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, size_t data_len, const uint8_t *data)
{
  return i2c_bus_write_reg8(dev_handle, mem_address, data_len, data);
}


int i2c_bus_write_bit(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, uint8_t bit_num, uint8_t data)
{
  uint8_t byte = 0;
  int ret = i2c_bus_read_byte(dev_handle, mem_address, &byte);

  if (ret != CH_OK) {
    return ret;
  }

  byte = (data != 0) ? (byte | (1 << bit_num)) : (byte & ~(1 << bit_num));
  return i2c_bus_write_byte(dev_handle, mem_address, byte);
}

int i2c_bus_write_bits(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, uint8_t bit_start, uint8_t length, uint8_t data)
{
  uint8_t byte = 0;
  int ret = i2c_bus_read_byte(dev_handle, mem_address, &byte);

  if (ret != CH_OK) {
    return ret;
  }

  uint8_t mask = ((1 << length) - 1) << (bit_start - length + 1);
  data <<= (bit_start - length + 1); // shift data into correct position
  data &= mask;                     // zero all non-important bits in data
  byte &= ~(mask);                  // zero all important bits in existing byte
  byte |= data;                     // combine data with existing byte
  return i2c_bus_write_byte(dev_handle, mem_address, byte);
}

/**************************************** Public Functions (Low level)*********************************************/

static int i2c_bus_read_reg8(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, size_t data_len, uint8_t *data)
{
  I2C_BUS_CHECK(dev_handle != NULL, "device handle error", CH_ERR_INVALID_ARG);
  I2C_BUS_CHECK(data != NULL, "data pointer error", CH_ERR_INVALID_ARG);
  i2c_bus_device_t *i2c_device = (i2c_bus_device_t *)dev_handle;
  I2C_BUS_INIT_CHECK(i2c_device->i2c_bus->is_init, CH_ERR_INVALID_STATE);

  while (I2C_GetFlagStatus(I2C_FLAG_BUSY) != RESET);

  if (mem_address != NULL_I2C_MEM_ADDR) {
    I2C_GenerateSTART(ENABLE);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress((i2c_device->dev_addr << 1), I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    if (I2C_GetFlagStatus(I2C_FLAG_TXE) != RESET) {
      I2C_SendData(mem_address);
    }
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  }

  I2C_GenerateSTART(ENABLE);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
  I2C_Send7bitAddress((i2c_device->dev_addr << 1), I2C_Direction_Receiver);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  for (size_t i = 0; i < data_len; i++) {
    while (I2C_GetFlagStatus(I2C_FLAG_RXNE) == RESET);
    data[i] = I2C_ReceiveData();
    if (i == data_len - 1) {
      I2C_AcknowledgeConfig(DISABLE);
    }
  }
  I2C_GenerateSTOP(ENABLE);
  return CH_OK;
}

int i2c_bus_read_reg16(i2c_bus_device_handle_t dev_handle, uint16_t mem_address, size_t data_len, uint8_t *data)
{
  I2C_BUS_CHECK(dev_handle != NULL, "device handle error", CH_ERR_INVALID_ARG);
  I2C_BUS_CHECK(data != NULL, "data pointer error", CH_ERR_INVALID_ARG);
  i2c_bus_device_t *i2c_device = (i2c_bus_device_t *)dev_handle;
  I2C_BUS_INIT_CHECK(i2c_device->i2c_bus->is_init, CH_ERR_INVALID_STATE);
  uint8_t memAddress8[2];
  memAddress8[0] = (uint8_t)((mem_address >> 8) & 0x00FF);
  memAddress8[1] = (uint8_t)(mem_address & 0x00FF);

  if (mem_address != NULL_I2C_MEM_ADDR) {
    I2C_GenerateSTART(ENABLE);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress((i2c_device->dev_addr << 1), I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    
    for(size_t i = 0 ; i < 2 ; i++)
    {
      if (I2C_GetFlagStatus(I2C_FLAG_TXE) != RESET) {
        I2C_SendData(memAddress8[i]);
      }
    }
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  }
  
  I2C_GenerateSTART(ENABLE);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
  I2C_Send7bitAddress((i2c_device->dev_addr << 1), I2C_Direction_Receiver);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  for (size_t i = 0; i < data_len; i++) {
    while (I2C_GetFlagStatus(I2C_FLAG_RXNE) == RESET);
    data[i] = I2C_ReceiveData();
    if (i == data_len - 1) {
      I2C_AcknowledgeConfig(DISABLE);
    }
  }
  I2C_GenerateSTOP(ENABLE);
  return CH_OK;
}

static int i2c_bus_write_reg8(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, size_t data_len, const uint8_t *data)
{
  I2C_BUS_CHECK(dev_handle != NULL, "device handle error", CH_ERR_INVALID_ARG);
  I2C_BUS_CHECK(data != NULL, "data pointer error", CH_ERR_INVALID_ARG);
  i2c_bus_device_t *i2c_device = (i2c_bus_device_t *)dev_handle;
  I2C_BUS_INIT_CHECK(i2c_device->i2c_bus->is_init, CH_ERR_INVALID_STATE);

  while (I2C_GetFlagStatus(I2C_FLAG_BUSY) != RESET);
  I2C_GenerateSTART(ENABLE);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
  I2C_Send7bitAddress((i2c_device->dev_addr << 1) , I2C_Direction_Transmitter);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  if (mem_address != NULL_I2C_MEM_ADDR) {
    if (I2C_GetFlagStatus(I2C_FLAG_TXE) != RESET) {
      I2C_SendData(mem_address);
    }
  }
  
  for(size_t i = 0 ; i < data_len ; i++)
  {
    if (I2C_GetFlagStatus(I2C_FLAG_TXE) != RESET) {
      I2C_SendData(data[i]);
    }
  }
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  I2C_GenerateSTOP(ENABLE);  
  return CH_OK;
}

int i2c_bus_write_reg16(i2c_bus_device_handle_t dev_handle, uint16_t mem_address, size_t data_len, const uint8_t *data)
{
  I2C_BUS_CHECK(dev_handle != NULL, "device handle error", CH_ERR_INVALID_ARG);
  I2C_BUS_CHECK(data != NULL, "data pointer error", CH_ERR_INVALID_ARG);
  i2c_bus_device_t *i2c_device = (i2c_bus_device_t *)dev_handle;
  I2C_BUS_INIT_CHECK(i2c_device->i2c_bus->is_init, CH_ERR_INVALID_STATE);
  uint8_t memAddress8[2];
  memAddress8[0] = (uint8_t)((mem_address >> 8) & 0x00FF);
  memAddress8[1] = (uint8_t)(mem_address & 0x00FF);

  if (mem_address != NULL_I2C_MEM_ADDR) {
    I2C_GenerateSTART(ENABLE);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress((i2c_device->dev_addr << 1), I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    
    for(size_t i = 0 ; i < 2 ; i++)
    {
      if (I2C_GetFlagStatus(I2C_FLAG_TXE) != RESET) {
        I2C_SendData(memAddress8[i]);
      }
    }
  }
  
  for(size_t j = 0 ; j < data_len ; j++)
  {
    if (I2C_GetFlagStatus(I2C_FLAG_TXE) != RESET) {
      I2C_SendData(data[j]);
    }
  }
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  I2C_GenerateSTOP(ENABLE);  
  return CH_OK;
}

/**************************************** Private Functions*********************************************/
static int i2c_driver_reinit(i2c_port_t port, const i2c_config_t *conf)
{
  I2C_BUS_CHECK(port < I2C_NUM_MAX, "i2c port error", CH_ERR_INVALID_ARG);
  I2C_BUS_CHECK(conf != NULL, "pointer = NULL error", CH_ERR_INVALID_ARG);

  if (s_i2c_bus[port].is_init) {
    I2C_DeInit();
    s_i2c_bus[port].is_init = false;
    CH_LOGI(TAG,"i2c%d bus deinited ", port);
  }
  GPIOB_ModeCfg(conf->sda_io_num | conf->scl_io_num, GPIO_ModeIN_PU);

  I2C_Init(I2C_Mode_I2C, conf->master.clk_speed, I2C_DutyCycle_16_9, I2C_Ack_Enable,
           I2C_AckAddr_7bit, MASTER_ADDR);
  s_i2c_bus[port].is_init = true;
  CH_LOGI(TAG,"i2c%d bus inited", port);
  return CH_OK;
}

static int i2c_driver_deinit(i2c_port_t port)
{
  I2C_BUS_CHECK(port < I2C_NUM_MAX, "i2c port error", CH_ERR_INVALID_ARG);
  I2C_BUS_CHECK(s_i2c_bus[port].is_init == true, "i2c not inited", CH_ERR_INVALID_STATE);
  I2C_DeInit(); //always return ESP_OK
  s_i2c_bus[port].is_init = false;
  CH_LOGI(TAG,"i2c%d bus deinited", port);
  return CH_OK;
}

/**
 * @brief compare with active i2c_bus configuration
 *
 * @param port choose which i2c_port's configuration will be compared
 * @param conf new configuration
 * @return true new configuration is equal to active configuration
 * @return false new configuration is not equal to active configuration
 */
inline static bool i2c_config_compare(i2c_port_t port, const i2c_config_t *conf)
{
  if (s_i2c_bus[port].conf_active.master.clk_speed == conf->master.clk_speed
      && s_i2c_bus[port].conf_active.sda_io_num == conf->sda_io_num
      && s_i2c_bus[port].conf_active.scl_io_num == conf->scl_io_num
      && s_i2c_bus[port].conf_active.scl_pullup_en == conf->scl_pullup_en
      && s_i2c_bus[port].conf_active.sda_pullup_en == conf->sda_pullup_en) {
    return true;
  }

  return false;
}

void i2c_bus_write_one_byte(uint8_t dev_addr, uint8_t mem_address, uint8_t data) {
  while (I2C_GetFlagStatus(I2C_FLAG_BUSY) != RESET)
    ;

  I2C_GenerateSTART(ENABLE);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
    ;
  I2C_Send7bitAddress((dev_addr << 1), I2C_Direction_Transmitter);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    ;

  if (mem_address != NULL_I2C_MEM_ADDR) {
    if (I2C_GetFlagStatus(I2C_FLAG_TXE) != RESET) {
      I2C_SendData(mem_address);
    }
  }

  if (I2C_GetFlagStatus(I2C_FLAG_TXE) != RESET) {
    I2C_SendData(data);
  }
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    ;
  I2C_GenerateSTOP(ENABLE);
}

void i2c_bus_read_one_byte(uint8_t dev_addr, uint8_t mem_addr, uint8_t *data) {
  while (I2C_GetFlagStatus(I2C_FLAG_BUSY) != RESET)
    ;

  if (mem_addr != NULL_I2C_MEM_ADDR) {
    I2C_GenerateSTART(ENABLE);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
      ;
    I2C_Send7bitAddress((dev_addr << 1), I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
      ;

    if (I2C_GetFlagStatus(I2C_FLAG_TXE) != RESET) {
      I2C_SendData(mem_addr);
    }
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
      ;
  }

  I2C_GenerateSTART(ENABLE);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
    ;
  I2C_Send7bitAddress((dev_addr << 1), I2C_Direction_Receiver);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    ;

  while (I2C_GetFlagStatus(I2C_FLAG_RXNE) == RESET)
    I2C_AcknowledgeConfig(DISABLE);

  *data = I2C_ReceiveData();
  I2C_GenerateSTOP(ENABLE);
}

void i2c_create(int pin_sda, int pin_scl, uint32_t freq) {
  GPIOB_ModeCfg(pin_sda | pin_scl, GPIO_ModeIN_PU);

  I2C_Init(I2C_Mode_I2C, freq, I2C_DutyCycle_16_9, I2C_Ack_Enable,
           I2C_AckAddr_7bit, MASTER_ADDR);
}