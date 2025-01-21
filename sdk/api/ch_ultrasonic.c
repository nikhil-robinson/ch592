#include "ch_ultrasonic.h"
#include "sys/time.h"

#define TAG "ULTRASONIC"

#define TRIGGER_LOW_DELAY 4
#define TRIGGER_HIGH_DELAY 10
#define PING_TIMEOUT 6000
#define ROUNDTRIP 58

static inline uint32_t get_time_us()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_usec;
}

#define timeout_expired(start, len) ((uint32_t)(get_time_us() - (start)) >= (len))

void ultrasonic_init(const ultrasonic_sensor_t *dev)
{
  if(dev->echo_gpio_port == PORTA)
  {
    // GPIOA_ResetBits(dev->echo_pin);
    GPIOA_ModeCfg(dev->echo_pin, GPIO_ModeIN_PU);
  }
  else if (dev->echo_gpio_port == PORTB)
  {
    // GPIOB_ResetBits(dev->echo_pin);
    GPIOB_ModeCfg(dev->echo_pin, GPIO_ModeIN_PU);
  }

  if(dev->trigger_gpio_port == PORTA)
  {
    GPIOA_ResetBits(dev->trigger_pin);
    GPIOA_ModeCfg(dev->trigger_pin, GPIO_ModeOut_PP_5mA);
    GPIOA_ResetBits(dev->trigger_pin);
  }
  else if(dev->trigger_gpio_port == PORTB)
  {
    GPIOB_ResetBits(dev->trigger_pin);
    GPIOB_ModeCfg(dev->trigger_pin, GPIO_ModeOut_PP_5mA);
    GPIOB_ResetBits(dev->trigger_pin);    
  }
}

uint8_t ultrasonic_measure_cm(const ultrasonic_sensor_t *dev, uint32_t max_distance, uint32_t *distance)
{
  if(!distance)
    return ESP_ERR_INVALID_ARG;
  
  CH_LOGI(TAG,"print 1");
  if(dev->trigger_gpio_port == PORTA)
  {
    CH_LOGI(TAG,"print 2");
    GPIOA_ResetBits(dev->trigger_pin);
    DelayUs(TRIGGER_LOW_DELAY);
    GPIOA_SetBits(dev->trigger_pin);
    DelayUs(TRIGGER_HIGH_DELAY);
    GPIOA_ResetBits(dev->trigger_pin);
  }
  else if(dev->trigger_gpio_port == PORTB)
  {
    CH_LOGI(TAG,"print 3");
    GPIOB_ResetBits(dev->trigger_pin);
    DelayUs(TRIGGER_LOW_DELAY);
    GPIOB_SetBits(dev->trigger_pin);
    DelayUs(TRIGGER_HIGH_DELAY);
    GPIOB_ResetBits(dev->trigger_pin);
  }
  CH_LOGI(TAG,"print 4");
  if(dev->echo_gpio_port == PORTA)
  {
    CH_LOGI(TAG,"print 5");
    if(GPIOA_ReadPortPin(dev->echo_pin)) {
      return CH_ERR_ULTRASONIC_PING;
    }
  }
  else if(dev->echo_gpio_port == PORTB)
  {
    CH_LOGI(TAG,"print 6");
    if(GPIOB_ReadPortPin(dev->echo_pin)) {
      return CH_ERR_ULTRASONIC_PING;
    }
  }

  uint32_t start = get_time_us();
  CH_LOGI(TAG,"print 7");
  if(dev->echo_gpio_port == PORTA)
  {
    CH_LOGI(TAG,"print 8");
    while(!GPIOA_ReadPortPin(dev->echo_pin))
    {
      CH_LOGI(TAG,"print 9");
      if(timeout_expired(start, PING_TIMEOUT)) {
        return CH_ERR_ULTRASONIC_PING_TIMEOUT;
      }
    }
  }
  else if(dev->echo_gpio_port == PORTB)
  {
    CH_LOGI(TAG,"print 10");
    while(!GPIOB_ReadPortPin(dev->echo_pin))
    {
      CH_LOGI(TAG,"print 11");
      if(timeout_expired(start, PING_TIMEOUT))
        return CH_ERR_ULTRASONIC_PING_TIMEOUT;
    }
    CH_LOGI(TAG,"print 12");
  }

  uint32_t echo_start =  get_time_us();
  uint32_t time = echo_start;
  uint32_t meas_timeout = echo_start + max_distance * ROUNDTRIP;
  CH_LOGI(TAG,"print 13");
  if(dev->echo_gpio_port == PORTA)
  {
    CH_LOGI(TAG,"print 14");
    while(GPIOA_ReadPortPin(dev->echo_pin))
    {
      CH_LOGI(TAG,"print 15");
      time = get_time_us();
      if(timeout_expired(echo_start, meas_timeout))
        return CH_ERR_ULTRASONIC_ECHO_TIMEOUT;
    }
  }
  else if(dev->echo_gpio_port == PORTB)
  {
    CH_LOGI(TAG,"print 16");
    while(GPIOB_ReadPortPin(dev->echo_pin))
    {
      CH_LOGI(TAG,"print 17");
      time = get_time_us();
      if(timeout_expired(echo_start, meas_timeout))
        return CH_ERR_ULTRASONIC_ECHO_TIMEOUT;
    }
  }
  CH_LOGI(TAG,"print 18");
  *distance = (time - echo_start)/ROUNDTRIP;
  CH_LOGI(TAG,"print 19");
  return CH_OK;
}



