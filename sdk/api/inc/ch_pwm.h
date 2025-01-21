#ifndef _CH_PWM_H
#define _CH_PWM_H

#include "CH59x_common.h"
#include "ch_err.h"
#include "ch_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PWM_8_BIT = 0,
  PWM_16_BIT 
}pwm_bit_width_t;

typedef enum {
  PWM_BASE = 0,
  TIMER_BASE,
  BASE_NONE
}pwm_base_t;

typedef struct {
  uint32_t gpio_num;
  pwm_bit_width_t bit_width;
  uint16_t freq;
}pwm_config_t;

typedef struct {
  uint32_t pwm_channel;
  pwm_bit_width_t bit_width;
}pwm_handle_t;

typedef struct {
  int channel;
  PWMX_PolarTypeDef pr;
  PWM_RepeatTsTypeDef ts;
  uint32_t cycle;
  uint32_t data_width;
}timer_config_t;

#define CH_PWM_NC -1


#define PWM8_CH1_GPIO  GPIO_NUM_NC
#define PWM8_CH2_GPIO  GPIO_NUM_NC
#define PWM8_CH3_GPIO  GPIO_NUM_NC
#define PWM8_CH4_GPIO  GPIOA_NUM_12
#define PWM8_CH5_GPIO  GPIOA_NUM_13
#define PWM8_CH6_GPIO  GPIOB_NUM_0
#define PWM8_CH7_GPIO  GPIOB_NUM_4
#define PWM8_CH8_GPIO  GPIOB_NUM_6
#define PWM8_CH9_GPIO  GPIOB_NUM_7
#define PWM8_CH10_GPIO GPIOB_NUM_14
#define PWM8_CH11_GPIO GPIOB_NUM_23

#define PWM16_CH1_GPIO  GPIO_NUM_NC
#define PWM16_CH2_GPIO  GPIO_NUM_NC
#define PWM16_CH3_GPIO  GPIO_NUM_NC
#define PWM16_CH4_GPIO  GPIOA_NUM_12
#define PWM16_CH5_GPIO  GPIOA_NUM_13
#define PWM16_CH6_GPIO  GPIOB_NUM_0
#define PWM16_CH7_GPIO  GPIOB_NUM_4
#define PWM16_CH8_GPIO  GPIOB_NUM_6
#define PWM16_CH9_GPIO  GPIOB_NUM_7
#define PWM16_CH10_GPIO GPIO_NUM_NC
#define PWM16_CH11_GPIO GPIO_NUM_NC

#define TMR_CH0_GPIO  GPIOA_NUM_9
#define TMR_CH1_GPIO  GPIOA_NUM_10
#define TMR_CH2_GPIO  GPIOA_NUM_11
#define TMR_CH3_GPIO  GPIOB_NUM_22




ch_err_t pwm8_io_to_channel(gpio_num_t io_num,  int *channel);
ch_err_t pwm16_io_to_channel(gpio_num_t io_num,  int *channel);
ch_err_t pwm_servo_enable(gpio_num_t pin);
ch_err_t pwm_ledc_enable(gpio_num_t pin);
ch_err_t pwm_servo_set_angle(gpio_num_t pin, uint16_t angle);
ch_err_t pwm_servo_set_duty(gpio_num_t pin, uint16_t duty);
ch_err_t pwm_servo_set_speed(gpio_num_t pin, uint16_t speed);
ch_err_t pwm_ledc_set_duty(gpio_num_t pin, uint16_t _duty);
ch_err_t pwm_servo_disable(gpio_num_t pin);
ch_err_t pwm_ledc_disable(gpio_num_t pin);


ch_err_t timer_io_to_channel(gpio_num_t io_num,  int *channel);
ch_err_t timer_servo_enable(gpio_num_t pin);
ch_err_t timer_ledc_enable(gpio_num_t pin);
ch_err_t timer_servo_set_angle(gpio_num_t pin, uint32_t angle);
ch_err_t timer_servo_set_duty(gpio_num_t pin, uint32_t duty);
ch_err_t timer_servo_set_speed(gpio_num_t pin, uint32_t speed);
ch_err_t timer_ledc_set_duty(gpio_num_t pin, uint32_t _duty);
ch_err_t timer_servo_disable(gpio_num_t pin);
ch_err_t timer_ledc_disable(gpio_num_t pin);


pwm_base_t get_io_pwm_base(gpio_num_t pin);


extern uint8_t pwm_create(pwm_config_t config , pwm_handle_t *ret_pwm);
extern uint8_t pwm_set_duty(pwm_handle_t handle , uint16_t duty);
extern uint8_t pwm_delete(pwm_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif