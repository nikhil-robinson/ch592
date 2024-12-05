#include "ssd1306_i2c.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "app_i2c.h"
#include "CH59x_common.h"

/*
 * error descriptions
 */
char *errstr[] ={
		"not busy",
		"master mode",
		"transmit mode",
		"tx empty",
		"transmit complete",
};

/*
 * error handler
 */
uint8_t ssd1306_i2c_error(uint8_t err)
{
	// report error
	printf("ssd1306_i2c_error - timeout waiting for %s\n\r", errstr[err]);

	// reset & initialize I2C
	ssd1306_i2c_setup();

	return 1;
}

/*
 * low-level packet send for blocking polled operation via i2c
 */
uint8_t ssd1306_i2c_send(uint8_t addr, uint8_t *data, uint8_t sz)
{
	while (I2C_GetFlagStatus(I2C_FLAG_BUSY) != RESET);
	I2C_GenerateSTART(ENABLE);
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress((SSD1306_I2C_ADDR << 1), I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));



	for (size_t i = 0; i < sz; i++)
	{
		while (I2C_GetFlagStatus(I2C_FLAG_TXE) == RESET);
		I2C_SendData(data[i]);
	}
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTOP(ENABLE);
	return 0;
}

/*
 * high-level packet send for I2C
 */
uint8_t ssd1306_pkt_send(uint8_t *data, uint8_t sz, uint8_t cmd)
{
	uint8_t pkt[33];

	/* build command or data packets */
	if (cmd)
	{
		pkt[0] = 0;
		pkt[1] = *data;
	}
	else
	{
		pkt[0] = 0x40;
		memcpy(&pkt[1], data, sz);
	}
	return ssd1306_i2c_send(SSD1306_I2C_ADDR, pkt, sz + 1);
}

uint8_t ssd1306_I2C_Init()
{
    uint32_t sysClock;
    uint16_t tmpreg;

    I2C_SoftwareResetCmd(ENABLE);
    I2C_SoftwareResetCmd(DISABLE);

    sysClock = GetSysClock();

    R16_I2C_CTRL2 &= ~RB_I2C_FREQ;
    R16_I2C_CTRL2 |= (sysClock /SSD1306_I2C_PRERATE)&RB_I2C_FREQ;

    R16_I2C_CTRL1 &= ~RB_I2C_PE;

    if(SSD1306_I2C_CLKRATE <= 100000)
    {
        tmpreg = (sysClock / (2*SSD1306_I2C_CLKRATE))&RB_I2C_CCR;

        if(tmpreg < 0x04)
            tmpreg = 0x04;

        // R16_I2C_RTR = (((sysClock / 1000000) + 1) > 0x3F) ? 0x3F : ((sysClock / 1000000) + 1);
    }
    else
    {
#ifndef SSD1306_I2C_DUTY
        tmpreg = (sysClock / (I2C_ClockSpeed * 3)) & RB_I2C_CCR;
#else

		tmpreg = (sysClock / (SSD1306_I2C_CLKRATE * 25)) & RB_I2C_CCR;
		tmpreg |= I2C_DutyCycle_16_9;
#endif
        tmpreg |= RB_I2C_F_S;
        // R16_I2C_RTR = (uint16_t)((((sysClock / 1000000) * (uint16_t)300) / (uint16_t)1000) + (uint16_t)1);
    }
    R16_I2C_CKCFGR = tmpreg;

    R16_I2C_CTRL1 |= RB_I2C_PE;

    R16_I2C_CTRL1 |= RB_I2C_ACK;

}

void ssd1306_rst(void)
{
	
}