#ifndef _APP_USB_H_
#define _APP_USB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "CH59x_common.h"


extern void app_usb_init(void);
extern void usb_write_buffer(const char *str, size_t len);
extern void USB_IRQProcessHandler(void);



#ifdef __cplusplus
}
#endif

#endif /*_APP_USB_H_ */
