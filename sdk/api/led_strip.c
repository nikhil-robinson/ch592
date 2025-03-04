#include "CH59x_common.h"
#include "led_strip.h"
#include "config.h"


#define Pixel_NUM        (RGB_NUMS)

/** spi mode
 * When you using spi mode, you should make sure your spi work at 6M clock
 */

#define Pixel_PRE_LEN (12u)
#define Pixel_RESET_LEN (25u)
#define COLOR_BUFFER_LEN (((Pixel_NUM)*Pixel_PRE_LEN)+Pixel_RESET_LEN)

uint8_t color_buf[COLOR_BUFFER_LEN] = {0};


/*********************************************************************
 * @fn      convToBit
 *
 * @brief   Convert hex to spi bit
 *
 * @param   res  - the result
 *          input - input data
 *
 * @return  none
 */
static void convToBit(uint8_t *res, uint8_t input)
{
  uint8_t mask = 0x80;
  for (int i = 0; i < 4; i++) {
    uint8_t result = (input & mask) ? 0xE : 0x8;
    result <<= 4;
    mask >>= 1;
    result |= (input & mask) ? 0xE : 0x8;
    mask >>= 1;
    res[i] = result;
  }
}


/*********************************************************************
 * @fn      colorToBit
 *
 * @brief   Convert color to spi bit
 *
 * @param   buf  - the result
 *          r  - red channel
 *          g  - green channel
 *          b  - blue channel
 *
 * @return  none
 */
static void colorToBit(uint8_t *buf, uint8_t r, uint8_t g, uint8_t b)
{
  uint8_t *res = buf;
  convToBit(res, g);
  convToBit(&(res[4]), r);
  convToBit(&(res[8]), b);
}


/*********************************************************************
 * @fn      setPixelColor
 *
 * @brief   Set the pixel color of an LED
 *
 * @param   index - index of LED
 *          r  - red channel
 *          g  - green channel
 *          b  - blue channel
 *
 *
 * @return  none
 */
void led_strip_set_pixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b)
{
  uint8_t *buf = &(color_buf[index * Pixel_PRE_LEN]);
  colorToBit(buf,r,g,b);
}


void led_strip_spi_device_init()
{
  GPIOA_ModeCfg(GPIO_Pin_14, GPIO_ModeOut_PP_20mA);
  SPI0_MasterDefInit();
}

void led_strip_delete()
{
  led_strip_clear();
  SPI0_MasterDefDeInit();
}

void led_strip_refresh()
{
  SPI0_MasterDMATrans(color_buf, COLOR_BUFFER_LEN);
}

void led_strip_clear()
{
  for(int i = 0; i < Pixel_NUM; i++){
    led_strip_set_pixel(i , 0,0,0);
  }
  led_strip_refresh();
}


