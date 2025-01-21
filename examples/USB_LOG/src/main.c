#include "CH59x_common.h"
#include "ch_log.h"
#include "usb.h"

#define TAG "APP/USB"

int main(void){

	SetSysClock(CLK_SOURCE_PLL_60MHz);
  app_usb_init();
  ch_set_log_level(CH_LOG_INFO);
  ch_set_log_mode(CH_LOG_USB);

  static volatile int count =0;

	while(1)
  {
    CH_LOGI(TAG,"USB PRINTF TEST %d", count++);
  }
}
