#pragma once

#include "CH59x_common.h"
#include "ch_err.h"
#include "ch_gpio.h"

typedef enum {
  ADC_CHANNEL_0 = 0,      ///< ADC channel
  ADC_CHANNEL_1,          ///< ADC channel
  ADC_CHANNEL_2,          ///< ADC channel
  ADC_CHANNEL_3,          ///< ADC channel
  ADC_CHANNEL_4,          ///< ADC channel
  ADC_CHANNEL_5,          ///< ADC channel
  ADC_CHANNEL_6,          ///< ADC channel
  ADC_CHANNEL_7,          ///< ADC channel
  ADC_CHANNEL_8,          ///< ADC channel
  ADC_CHANNEL_9,          ///< ADC channel
  ADC_CHANNEL_10,
  ADC_CHANNEL_11,
  ADC_CHANNEL_12,
  ADC_CHANNEL_13,
  ADC_CHANNEL_MAX
} adc_channel_t;

#define SOC_ADC_PERIPH_NUM 1

#define ADC1_CHANNEL_0_GPIO_NUM GPIOA_NUM_4
#define ADC1_CHANNEL_1_GPIO_NUM GPIOA_NUM_5
#define ADC1_CHANNEL_2_GPIO_NUM GPIOA_NUM_12
#define ADC1_CHANNEL_3_GPIO_NUM GPIOA_NUM_13
#define ADC1_CHANNEL_4_GPIO_NUM GPIOA_NUM_14
#define ADC1_CHANNEL_5_GPIO_NUM GPIOA_NUM_15
#define ADC1_CHANNEL_6_GPIO_NUM GPIO_NUM_NC
#define ADC1_CHANNEL_7_GPIO_NUM GPIO_NUM_NC
#define ADC1_CHANNEL_8_GPIO_NUM GPIOB_NUM_0
#define ADC1_CHANNEL_9_GPIO_NUM GPIOB_NUM_6
#define ADC1_CHANNEL_10_GPIO_NUM GPIOA_NUM_6
#define ADC1_CHANNEL_11_GPIO_NUM GPIOA_NUM_7
#define ADC1_CHANNEL_12_GPIO_NUM GPIOA_NUM_8
#define ADC1_CHANNEL_13_GPIO_NUM GPIOA_NUM_9



typedef struct {
  ADC_SampClkTypeDef sample_freq;
  ADC_SignalPGATypeDef pga;
}adc_oneshot_chan_cfg_t;

#ifdef __cplusplus
extern "C" {
#endif

ch_err_t adc_io_to_channel(int io_num, adc_channel_t * const channel);
int adc_oneshot_config_channel(adc_channel_t channel , const adc_oneshot_chan_cfg_t *config);
void adc_oneshot_read(adc_channel_t chan , int *out_raw);
void adc_cali_raw_to_voltage(int raw, int *voltage);

#ifdef __cplusplus
}
#endif