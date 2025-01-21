#include "ch_adc.h"
#include "ch_gpio.h"

#define TAG "ADC"

const int adc_channel_io_map[SOC_ADC_PERIPH_NUM][ADC_CHANNEL_MAX] = {
    /* ADC1 */
    {ADC1_CHANNEL_0_GPIO_NUM, ADC1_CHANNEL_1_GPIO_NUM, ADC1_CHANNEL_2_GPIO_NUM, ADC1_CHANNEL_3_GPIO_NUM, ADC1_CHANNEL_4_GPIO_NUM,
     ADC1_CHANNEL_5_GPIO_NUM, ADC1_CHANNEL_6_GPIO_NUM, ADC1_CHANNEL_7_GPIO_NUM, ADC1_CHANNEL_8_GPIO_NUM,ADC1_CHANNEL_9_GPIO_NUM,
     ADC1_CHANNEL_10_GPIO_NUM, ADC1_CHANNEL_11_GPIO_NUM, ADC1_CHANNEL_12_GPIO_NUM, ADC1_CHANNEL_13_GPIO_NUM}
};

static int adc_channel_init(adc_channel_t channel)
{
  if(channel == ADC_CHANNEL_0)
  {
    GPIOA_ModeCfg(GPIO_Pin_4, GPIO_ModeIN_Floating);
    ADC_ChannelCfg(ADC_CHANNEL_0);
  }
  else if(channel == ADC_CHANNEL_1)
  {
    GPIOA_ModeCfg(GPIO_Pin_5, GPIO_ModeIN_Floating);
    ADC_ChannelCfg(ADC_CHANNEL_1);
  }
  else if(channel == ADC_CHANNEL_2)
  {
    GPIOA_ModeCfg(GPIO_Pin_12, GPIO_ModeIN_Floating);
    ADC_ChannelCfg(ADC_CHANNEL_2);
  }
  else if(channel == ADC_CHANNEL_3)
  {
    GPIOA_ModeCfg(GPIO_Pin_13, GPIO_ModeIN_Floating);
    ADC_ChannelCfg(ADC_CHANNEL_3);
  }
  else if(channel == ADC_CHANNEL_4)
  {
    GPIOA_ModeCfg(GPIO_Pin_14, GPIO_ModeIN_Floating);
    ADC_ChannelCfg(ADC_CHANNEL_4);
  }
  else if(channel == ADC_CHANNEL_5)
  {
    GPIOA_ModeCfg(GPIO_Pin_15, GPIO_ModeIN_Floating);
    ADC_ChannelCfg(ADC_CHANNEL_5);
  }
  else if(channel == ADC_CHANNEL_6)
  {
    return CH_FAIL;
  }
  else if(channel == ADC_CHANNEL_7)
  {
    return CH_FAIL;
  }
  else if(channel == ADC_CHANNEL_8)
  {
    GPIOB_ModeCfg(GPIO_Pin_0, GPIO_ModeIN_Floating);
    ADC_ChannelCfg(ADC_CHANNEL_8);
  }
  else if(channel == ADC_CHANNEL_9)
  {
    GPIOB_ModeCfg(GPIO_Pin_6, GPIO_ModeIN_Floating);
    ADC_ChannelCfg(ADC_CHANNEL_9);
  }
  else if(channel == ADC_CHANNEL_10)
  {
    GPIOA_ModeCfg(GPIO_Pin_10, GPIO_ModeIN_Floating);
    ADC_ChannelCfg(ADC_CHANNEL_10);
  }
  else if(channel == ADC_CHANNEL_11)
  {
    GPIOA_ModeCfg(GPIO_Pin_7, GPIO_ModeIN_Floating);
    ADC_ChannelCfg(ADC_CHANNEL_11);
  }
  else if(channel == ADC_CHANNEL_12)
  {
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_Floating);
    ADC_ChannelCfg(ADC_CHANNEL_12);
  }
  else if(channel == ADC_CHANNEL_13)
  {
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeIN_Floating);
    ADC_ChannelCfg(ADC_CHANNEL_13);
  }

  return CH_OK;  
}

ch_err_t adc_io_to_channel(int io_num, adc_channel_t * const channel)
{
    CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(io_num), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");
    CH_RETURN_ON_FALSE(channel, CH_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    bool found = false;
    for (int i = 0; i < SOC_ADC_PERIPH_NUM; i++) {
        for (int j = 0; j < ADC_CHANNEL_MAX; j++) {
            if (adc_channel_io_map[i][j] == io_num) {
                *channel = j;
                found = true;
            }
        }
    }
    return (found) ? CH_OK : CH_ERR_NOT_FOUND;
}

ch_err_t adc_oneshot_config_channel(adc_channel_t channel , const adc_oneshot_chan_cfg_t *config)
{
  ADC_ExtSingleChSampInit(config->sample_freq , config->pga);
  ADC_ChannelCfg(channel);
  return CH_OK;
}

void adc_oneshot_read(adc_channel_t chan , int *out_raw)
{
  ADC_ChannelCfg(chan);
  *out_raw = ADC_ExcutSingleConver();
}

void adc_cali_raw_to_voltage(int raw, int *voltage)
{

}