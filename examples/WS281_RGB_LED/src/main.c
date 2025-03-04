#include "CH59x_common.h"
#include "led_strip.h"

#define Pixel_NUM        (8)
#define LIST_SIZE(list) (sizeof(list)/sizeof(list[0]))
#define hex2rgb(c) (((c)>>16)&0xff),(((c)>>8)&0xff),((c)&0xff)

void DebugInit(void)
{
  GPIOA_SetBits(GPIO_Pin_9);
  GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
  UART1_DefInit();
}

uint32_t color_list[] = {0xff0000,
                         0x00ff00,
                         0x0000ff,
                         0xffff00,
                         0xff00ff,
                         0x00ffff,
                         0xffffff,
                         //0x000000,
                        };

void led_example(void)
{
  for (size_t i = 0; i < Pixel_NUM; i++)
  {
    led_strip_set_pixel(i, 0,255,0);
    led_strip_refresh();
  }
  

}

void led_example_1(void)
{
  uint32_t i = 0;
  uint32_t c = 0x0f0f00;
  while(1) {
    c = color_list[i];
    i++;
    if(i>=LIST_SIZE(color_list)) {
      i=0;
    }
    for (int var = 0; var < Pixel_NUM; ++var) {
      led_strip_set_pixel(var,hex2rgb(c));
    }
    led_strip_refresh();
    DelayMs(100);
  }
}

void led_example_2(void)
{
  uint32_t i = 0;
  uint32_t c = 0x0f0f00;
  while(1) {
    c = color_list[i];
    i++;
    if(i>=LIST_SIZE(color_list)) {
      i=0;
    }

    for (int var = 0; var < Pixel_NUM; var+=1) {
      led_strip_set_pixel(var, hex2rgb(c));
      led_strip_refresh();
      DelayMs(100);
      led_strip_set_pixel(var,0,0,0);
      led_strip_refresh();
    }
  }
}

int main(void)
{
  uint16_t i = 0;
  SetSysClock(CLK_SOURCE_PLL_60MHz);
  DebugInit();

  for (i = 0; i < Pixel_NUM; i++) {
    led_strip_set_pixel(i, 0, 0, 0);
  }
  led_strip_spi_device_init();
  led_strip_refresh();
  led_example();
}