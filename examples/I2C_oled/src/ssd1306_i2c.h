#ifndef _SSD1306_I2C_H
#define _SSD1306_I2C_H

#include <string.h>
#include <stdint.h>

// For the CH32V203, we support remapping, if so,
// #define SSD1306_REMAP_I2C

// SSD1306 I2C address
#define SSD1306_I2C_ADDR 0x3c

// I2C Bus clock rate - must be lower the Logic clock rate
#define SSD1306_I2C_CLKRATE 1000000

// I2C Logic clock rate - must be higher than Bus clock rate
#define SSD1306_I2C_PRERATE 2000000

// uncomment this for high-speed 36% duty cycle, otherwise 33%
#define SSD1306_I2C_DUTY

// I2C Timeout count
#define TIMEOUT_MAX 100000

// uncomment this to enable IRQ-driven operation
//#define SSD1306_I2C_IRQ

#ifdef SSD1306_I2C_IRQ
// some stuff that IRQ mode needs
volatile uint8_t ssd1306_i2c_send_buffer[64], *ssd1306_i2c_send_ptr, ssd1306_i2c_send_sz, ssd1306_i2c_irq_state;

// uncomment this to enable time diags in IRQ
//#define IRQ_DIAG
#endif

/*
 * init just I2C
 */
void ssd1306_i2c_setup(void);


/*
 * error handler
 */
uint8_t ssd1306_i2c_error(uint8_t err);

// event codes we use
#define  SSD1306_I2C_EVENT_MASTER_MODE_SELECT ((uint32_t)0x00030001)  /* BUSY, MSL and SB flag */
#define  SSD1306_I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ((uint32_t)0x00070082)  /* BUSY, MSL, ADDR, TXE and TRA flags */
#define  SSD1306_I2C_EVENT_MASTER_BYTE_TRANSMITTED ((uint32_t)0x00070084)  /* TRA, BUSY, MSL, TXE and BTF flags */

/*
 * check for 32-bit event codes
 */
uint8_t ssd1306_i2c_chk_evt(uint32_t event_mask);


/*
 * low-level packet send for blocking polled operation via i2c
 */
uint8_t ssd1306_i2c_send(uint8_t addr, uint8_t *data, uint8_t sz);


/*
 * high-level packet send for I2C
 */
uint8_t ssd1306_pkt_send(uint8_t *data, uint8_t sz, uint8_t cmd);
/*
 * init I2C and GPIO
 */
uint8_t ssd1306_i2c_init(void);

/*
 * reset is not used for SSD1306 I2C interface
 */
void ssd1306_rst(void);


#endif
