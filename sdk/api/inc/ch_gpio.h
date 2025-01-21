#pragma once

#include "CH59x_common.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "ch_log.h"
#include "ch_err.h"
#include "ch_check.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GPIOA_MASK 0
#define GPIOB_MASK 16

#define GPIO_MAP_A(number)  number - GPIOA_MASK
#define GPIO_MAP_B(number)  number - GPIOB_MASK

#define GET_GPIO_MAP(number)  1ULL << number

typedef void (*gpio_isr_t)(void *arg);


typedef enum {
    GPIO_NUM_NC  = -1,     /*!< Use to signal not connected to S/W */
    GPIOA_NUM_4  =  4,     /*!< PA4, input and output */
    GPIOA_NUM_5  =  5,     /*!< PA5, input and output */
    GPIOA_NUM_6  =  6,     /*!< PA6, input and output */
    GPIOA_NUM_7  =  7,     /*!< PA7, input and output */
    GPIOA_NUM_8  =  8,     /*!< PA8, input and output */
    GPIOA_NUM_9  =  9,     /*!< PA9, input and output */
    GPIOA_NUM_10 = 10,     /*!< PA10, input and output */
    GPIOA_NUM_11 = 11,     /*!< PA11, input and output */
    GPIOA_NUM_12 = 12,     /*!< PA12, input and output */
    GPIOA_NUM_13 = 13,     /*!< PA13, input and output */
    GPIOA_NUM_14 = 14,     /*!< PA14, input and output */
    GPIOA_NUM_15 = 15,     /*!< PA15, input and output */
    GPIOB_NUM_0  = 16,     /*!< PB0, input and output */
    GPIOB_NUM_4  = 20,     /*!< PB4, input and output */
    GPIOB_NUM_6  = 22,     /*!< PB6, input and output */
    GPIOB_NUM_7  = 23,     /*!< PB7, input and output */
    GPIOB_NUM_10 = 26,     /*!< PB10, input and output */
    GPIOB_NUM_11 = 27,     /*!< PB11, input and output */
    GPIOB_NUM_12 = 28,     /*!< PB12, input and output */
    GPIOB_NUM_13 = 29,     /*!< PB13, input and output */
    GPIOB_NUM_14 = 30,     /*!< PB14, input and output */
    GPIOB_NUM_15 = 31,     /*!< PB15, input and output */
    GPIOB_NUM_22 = 38,     /*!< PB22, input and output */
    GPIOB_NUM_23 = 39,     /*!< PB23, input and output */
    GPIO_NUM_MAX,
} gpio_num_t;

typedef enum {
    GPIO_MODE_DISABLE,         /*!< GPIO mode : disable input and output             */
    GPIO_MODE_INPUT,           /*!< GPIO mode : input only                           */
    GPIO_MODE_OUTPUT,          /*!< GPIO mode : output only mode                     */
    GPIO_MODE_OUTPUT_OD,       /*!< GPIO mode : output only with open-drain mode     */
    GPIO_MODE_INPUT_OUTPUT_OD, /*!< GPIO mode : output and input with open-drain mode*/
    GPIO_MODE_INPUT_OUTPUT,    /*!< GPIO mode : output and input mode                */
} gpio_mode_t;

typedef enum {
    GPIO_DRV_5mA,         /*!< GPIO drive current : 5mA  */
    GPIO_DRV_20mA,        /*!< GPIO drive current : 20mA */
} gpio_drv_t;

typedef enum {
    GPIO_INTR_DISABLE = 0,     /*!< Disable GPIO interrupt                             */
    GPIO_INTR_POSEDGE = 1,     /*!< GPIO interrupt type : rising edge                  */
    GPIO_INTR_NEGEDGE = 2,     /*!< GPIO interrupt type : falling edge                 */
    GPIO_INTR_LOW_LEVEL = 3,   /*!< GPIO interrupt type : input low level trigger      */
    GPIO_INTR_HIGH_LEVEL = 4,  /*!< GPIO interrupt type : input high level trigger     */
    GPIO_INTR_MAX,
} gpio_int_type_t;

typedef enum {
    GPIO_PULLUP_DISABLE = 0x0,     /*!< Disable GPIO pull-up resistor */
    GPIO_PULLUP_ENABLE = 0x1,      /*!< Enable GPIO pull-up resistor */
} gpio_pullup_t;

typedef enum {
    GPIO_PULLDOWN_DISABLE = 0x0,   /*!< Disable GPIO pull-down resistor */
    GPIO_PULLDOWN_ENABLE = 0x1,    /*!< Enable GPIO pull-down resistor  */
} gpio_pulldown_t;

typedef enum {
    GPIO_PULLUP_ONLY,               /*!< Pad pull up            */
    GPIO_PULLDOWN_ONLY,             /*!< Pad pull down          */
    GPIO_PULLUP_PULLDOWN,           /*!< Pad pull up + pull down*/
    GPIO_FLOATING,                  /*!< Pad floating           */
} gpio_pull_mode_t;

typedef struct {
    uint64_t pin_bit_mask;          /*!< GPIO pin: set with bit mask, each bit maps to a GPIO */
    gpio_mode_t mode;               /*!< GPIO mode: set input/output mode                     */
    gpio_pullup_t pull_up_en;       /*!< GPIO pull-up                                         */
    gpio_pulldown_t pull_down_en;   /*!< GPIO pull-down                                       */
    gpio_int_type_t intr_type;      /*!< GPIO interrupt type                                  */
} gpio_config_t;

#define GPIO_IS_VALID_GPIO(gpio_num)     ((gpio_num > GPIO_NUM_NC) && (gpio_num < GPIO_NUM_MAX))

ch_err_t gpio_set_direction(gpio_num_t gpio_num, gpio_mode_t mode);

ch_err_t gpio_set_level(gpio_num_t gpio_num, uint32_t level);

int gpio_get_level(gpio_num_t gpio_num);

ch_err_t gpio_pullup_en(gpio_num_t gpio_num);

ch_err_t gpio_pullup_dis(gpio_num_t gpio_num);

ch_err_t gpio_pulldown_en(gpio_num_t gpio_num);

ch_err_t gpio_pulldown_dis(gpio_num_t gpio_num);

ch_err_t gpio_intr_enable(gpio_num_t gpio_num, gpio_int_type_t intr_type);

ch_err_t gpio_intr_disable(gpio_num_t gpio_num);

ch_err_t gpio_reset_pin(gpio_num_t gpio_num);

#ifdef __cplusplus
}
#endif