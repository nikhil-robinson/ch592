#ifndef _CH_COMMON_H_
#define _CH_COMMON_H_


#define CH_OK                   0x00
#define CH_FAIL                 0x01
#define CH_ERR_INVALID_ARG      0x02
#define CH_ERR_INVALID_STATE    0x03
#define CH_ERR_TIMEOUT          0x04

typedef enum {
  PORTA = 1,
  PORTB
}gpio_port_t;

#endif