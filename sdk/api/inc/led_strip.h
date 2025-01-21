#ifndef _LED_STRIP_H_
#define _LED_STRIP_H_

#ifdef __cplusplus
 extern "C" {
#endif

void led_strip_set_pixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void led_strip_spi_device_init();
void led_strip_refresh();
void led_strip_clear();
void led_strip_delete();

#ifdef __cplusplus
}
#endif

#endif
