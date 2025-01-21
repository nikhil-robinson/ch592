#include "ch_pwm.h"
#include "ch_utily.h"
#include "ch_gpio.h"

#define TAG "PWM_API"

#define SOC_PWM_CHAN_NUMBER 11
#define SOC_TMR_CHAN_NUMBER 4

static const int pwm8_channel_map[SOC_PWM_CHAN_NUMBER] = {
    CH_PWM_NC,
    CH_PWM_NC,
    CH_PWM_NC,
    CH_PWM4,
    CH_PWM5,
    CH_PWM6,
    CH_PWM7,
    CH_PWM8,
    CH_PWM9,
    CH_PWM10,
    CH_PWM11,
};

static const int pwm16_channel_map[SOC_PWM_CHAN_NUMBER] = {
    CH_PWM_NC,
    CH_PWM_NC,
    CH_PWM_NC,
    CH_PWM4,
    CH_PWM5,
    CH_PWM6,
    CH_PWM7,
    CH_PWM8,
    CH_PWM9,
    CH_PWM_NC,
    CH_PWM_NC,
};

static const int pwm8_channel_io_map[SOC_PWM_CHAN_NUMBER] = {
    PWM8_CH1_GPIO,
    PWM8_CH2_GPIO,
    PWM8_CH3_GPIO,
    PWM8_CH4_GPIO,
    PWM8_CH5_GPIO,
    PWM8_CH6_GPIO,
    PWM8_CH7_GPIO,
    PWM8_CH8_GPIO,
    PWM8_CH9_GPIO,
    PWM8_CH10_GPIO,
    PWM8_CH11_GPIO};

static const int pwm16_channel_io_map[SOC_PWM_CHAN_NUMBER] = {
    PWM16_CH1_GPIO,
    PWM16_CH2_GPIO,
    PWM16_CH3_GPIO,
    PWM16_CH4_GPIO,
    PWM16_CH5_GPIO,
    PWM16_CH6_GPIO,
    PWM16_CH7_GPIO,
    PWM16_CH8_GPIO,
    PWM16_CH9_GPIO,
    PWM16_CH10_GPIO,
    PWM16_CH11_GPIO};

static const int timer_channel_io_map[SOC_TMR_CHAN_NUMBER] = {
    TMR_CH0_GPIO,
    TMR_CH1_GPIO,
    TMR_CH2_GPIO,
    TMR_CH3_GPIO};


#define MAP_32(x,in_min,in_max,out_min,out_max) (long)(((long)x - (long)in_min) * ((long)out_max - (long)out_min) / ((long)in_max - (long)in_min) + (long)out_min)

uint8_t pwm_create(pwm_config_t config, pwm_handle_t *ret_pwm)
{

  if (config.bit_width == PWM_8_BIT && config.freq == 5000)
  {
    ret_pwm->bit_width = PWM_8_BIT;
    if (config.gpio_num == GPIO_Pin_12)
    {
      GPIOA_ModeCfg(config.gpio_num, GPIO_ModeOut_PP_5mA);
      ret_pwm->pwm_channel = CH_PWM4;
    }
    else if (config.gpio_num == GPIO_Pin_13)
    {
      GPIOA_ModeCfg(config.gpio_num, GPIO_ModeOut_PP_5mA);
      ret_pwm->pwm_channel = CH_PWM5;
    }
    else if (config.gpio_num == GPIO_Pin_0)
    {
      GPIOB_ModeCfg(GPIO_Pin_0, GPIO_ModeOut_PP_5mA);
      ret_pwm->pwm_channel = CH_PWM6;
    }
    else if (config.gpio_num == GPIO_Pin_4)
    {
      GPIOB_ModeCfg(config.gpio_num, GPIO_ModeOut_PP_5mA);
      ret_pwm->pwm_channel = CH_PWM7;
    }
    else if (config.gpio_num == GPIO_Pin_6)
    {
      GPIOB_ModeCfg(config.gpio_num, GPIO_ModeOut_PP_5mA);
      ret_pwm->pwm_channel = CH_PWM8;
    }
    else if (config.gpio_num == GPIO_Pin_7)
    {
      GPIOB_ModeCfg(config.gpio_num, GPIO_ModeOut_PP_5mA);
      ret_pwm->pwm_channel = CH_PWM9;
    }
    else if (config.gpio_num == GPIO_Pin_14)
    {
      GPIOB_ModeCfg(config.gpio_num, GPIO_ModeOut_PP_5mA);
      ret_pwm->pwm_channel = CH_PWM10;
    }
    else if (config.gpio_num == GPIO_Pin_23)
    {
      GPIOB_ModeCfg(config.gpio_num, GPIO_ModeOut_PP_5mA);
      ret_pwm->pwm_channel = CH_PWM11;
    }
    else
    {
      return CH_FAIL;
    }

    PWMX_CLKCfg(46);
    PWMX_CycleCfg(PWMX_Cycle_256);
  }

  else if (config.bit_width == PWM_16_BIT && config.freq == 50)
  {
    ret_pwm->bit_width = PWM_16_BIT;
    if (config.gpio_num == GPIO_Pin_12)
    {
      GPIOA_ModeCfg(config.gpio_num, GPIO_ModeOut_PP_5mA);
      ret_pwm->pwm_channel = CH_PWM4;
    }
    else if (config.gpio_num == GPIO_Pin_13)
    {
      GPIOA_ModeCfg(config.gpio_num, GPIO_ModeOut_PP_5mA);
      ret_pwm->pwm_channel = CH_PWM5;
    }
    else if (config.gpio_num == GPIO_Pin_0)
    {
      GPIOB_ModeCfg(config.gpio_num, GPIO_ModeOut_PP_5mA);
      ret_pwm->pwm_channel = CH_PWM6;
    }
    else if (config.gpio_num == GPIO_Pin_4)
    {
      GPIOB_ModeCfg(config.gpio_num, GPIO_ModeOut_PP_5mA);
      ret_pwm->pwm_channel = CH_PWM7;
    }
    else if (config.gpio_num == GPIO_Pin_6)
    {
      GPIOB_ModeCfg(config.gpio_num, GPIO_ModeOut_PP_5mA);
      ret_pwm->pwm_channel = CH_PWM8;
    }
    else if (config.gpio_num == GPIO_Pin_7)
    {
      GPIOB_ModeCfg(config.gpio_num, GPIO_ModeOut_PP_5mA);
      ret_pwm->pwm_channel = CH_PWM9;
    }
    else
    {
      return CH_FAIL;
    }

    PWMX_CLKCfg(20);
    PWMX_16bit_CycleCfg(60000);
  }
  else
  {
    return CH_FAIL;
  }
  return CH_OK;
}

uint8_t pwm_set_duty(pwm_handle_t handle, uint16_t duty)
{
  if (handle.bit_width == PWM_8_BIT)
  {
    PWMX_ACTOUT(handle.pwm_channel, (uint8_t)duty, High_Level, ENABLE);
    return CH_OK;
  }
  else if (handle.bit_width == PWM_16_BIT)
  {
    PWMX_16bit_ACTOUT(handle.pwm_channel, duty, High_Level, ENABLE);
    return CH_OK;
  }
  return CH_FAIL;
}

uint8_t pwm_delete(pwm_handle_t handle)
{
  if (handle.bit_width == PWM_8_BIT)
  {
    PWMX_ACTOUT(handle.pwm_channel, 0, High_Level, DISABLE);
    return CH_OK;
  }
  else if (handle.bit_width == PWM_16_BIT)
  {
    PWMX_16bit_ACTOUT(handle.pwm_channel, 0, High_Level, DISABLE);
    return CH_OK;
  }
  return CH_FAIL;
}

ch_err_t pwm8_io_to_channel(gpio_num_t io_num, int *channel)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(io_num), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");
  for (int j = 0; j < SOC_PWM_CHAN_NUMBER; j++)
  {
    if (pwm8_channel_io_map[j] == io_num)
    {
      if (CH_PWM_NC != pwm8_channel_map[j])
      {

        *channel = pwm8_channel_map[j];
        return CH_OK;
      }
    }
  }
  *channel = CH_PWM_NC;
  return CH_ERR_NOT_FOUND;
}

ch_err_t pwm16_io_to_channel(gpio_num_t io_num, int *channel)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(io_num), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");

  for (int j = 0; j < SOC_PWM_CHAN_NUMBER; j++)
  {
    if (pwm16_channel_io_map[j] == io_num)
    {
      if (CH_PWM_NC != pwm16_channel_map[j])
      {
        *channel = pwm16_channel_map[j];
        return CH_OK;
      }
    }
  }
  *channel = CH_PWM_NC;
  return CH_ERR_NOT_FOUND;
}

ch_err_t pwm_servo_enable(gpio_num_t pin)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(pin), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");
  gpio_set_direction(pin, GPIO_MODE_OUTPUT);
  gpio_pulldown_dis(pin);
  PWMX_CLKCfg(20);
  PWMX_16bit_CycleCfg(60000);
  int channel;
  return pwm16_io_to_channel(pin, &channel);
}

ch_err_t pwm_ledc_enable(gpio_num_t pin)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(pin), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");
  gpio_set_direction(pin, GPIO_MODE_OUTPUT);
  gpio_pulldown_dis(pin);
  PWMX_CLKCfg(4);
  PWMX_CycleCfg(PWMX_Cycle_64);
  int channel;
  return pwm8_io_to_channel(pin, &channel);
}

ch_err_t pwm_servo_set_duty(gpio_num_t pin, uint16_t duty)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(pin), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");
  int channel = CH_PWM_NC;
  if (CH_OK == pwm16_io_to_channel(pin, &channel))
  {
    if (channel != CH_PWM_NC)
    {
      PWMX_16bit_ACTOUT(channel, duty, High_Level, ENABLE);
      return CH_OK;
    }
  }
  return CH_ERR_NOT_FOUND;
}

ch_err_t pwm_servo_set_angle(gpio_num_t pin, uint16_t angle)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(pin), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");
  int duty = MAP_32(angle, 0, 180, 1500, 7500);
  return pwm_servo_set_duty(pin, duty);
}

ch_err_t pwm_servo_set_speed(gpio_num_t pin, uint16_t speed)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(pin), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");
  int rotate = 0;
  if (speed == 100)
  {
    rotate = 0;
  }
  else if (speed < 100)
  {
    rotate = MAP_32(speed, 0, 99, 7500, 3000);
  }
  else if (speed > 100)
  {
    rotate = MAP_32(speed, 101, 200, 3000, 1500);
  }
  return pwm_servo_set_duty(pin, rotate);
}

ch_err_t pwm_ledc_set_duty(gpio_num_t pin, uint16_t _duty)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(pin), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");
  int channel = CH_PWM_NC;
  int duty = 64 * _duty / 100;
  if (CH_OK == pwm8_io_to_channel(pin, &channel))
  {
    if (channel != CH_PWM_NC)
    {
      PWMX_ACTOUT(channel, duty, High_Level, ENABLE);
      return CH_OK;
    }
  }
  return CH_ERR_NOT_FOUND;
}

ch_err_t pwm_servo_disable(gpio_num_t pin)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(pin), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");
  int channel = CH_PWM_NC;
  int duty = 0;
  if (CH_OK == pwm16_io_to_channel(pin, &channel))
  {
    if (channel != CH_PWM_NC)
    {
      PWMX_16bit_ACTOUT(channel, duty, High_Level, DISABLE);
      return CH_OK;
    }
  }
  return CH_ERR_NOT_FOUND;
}

ch_err_t pwm_ledc_disable(gpio_num_t pin)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(pin), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");
  int channel = CH_PWM_NC;
  int duty = 0;
  if (CH_OK == pwm8_io_to_channel(pin, &channel))
  {
    if (channel != CH_PWM_NC)
    {
      PWMX_ACTOUT(channel, duty, High_Level, DISABLE);
      return CH_OK;
    }
  }
  return CH_ERR_NOT_FOUND;
}

ch_err_t timer_io_to_channel(gpio_num_t io_num, int *channel)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(io_num), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");
  for (int j = 0; j < SOC_TMR_CHAN_NUMBER; j++)
  {
    if (timer_channel_io_map[j] == io_num)
    {
      *channel = j;
      return CH_OK;
    }
  }
  *channel = -1;
  return CH_ERR_NOT_FOUND;
}

ch_err_t timer_init(timer_config_t *cfg)
{
  CH_RETURN_ON_FALSE(cfg, CH_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
  CH_RETURN_ON_FALSE( (cfg->channel < 4) && (cfg->channel > -1), CH_ERR_INVALID_ARG, TAG, "invalid Timer Channel");

  CH_LOGI(TAG,"ENABLED TIMER_%d WITH CYCLE [%d] Data width [%ld]",cfg->channel,cfg->cycle,cfg->data_width);

  switch (cfg->channel)
  {
  case 0:
  {
    TMR0_PWMInit(cfg->pr, cfg->ts);
    TMR0_PWMCycleCfg(cfg->cycle);
    TMR0_PWMActDataWidth(cfg->data_width);
    TMR0_PWMEnable();
    TMR0_Enable();
    return CH_OK;
    break;
  }
  case 1:
  {
    TMR1_PWMInit(cfg->pr, cfg->ts);
    TMR1_PWMCycleCfg(cfg->cycle);
    TMR1_PWMActDataWidth(cfg->data_width);
    TMR1_PWMEnable();
    TMR1_Enable();
    return CH_OK;
    break;
  }
  case 2:
  {
    TMR2_PWMInit(cfg->pr, cfg->ts);
    TMR2_PWMCycleCfg(cfg->cycle);
    TMR2_PWMActDataWidth(cfg->data_width);
    TMR2_PWMEnable();
    TMR2_Enable();
    return CH_OK;
    break;
  }
  case 3:
  {
    TMR3_PWMInit(cfg->pr, cfg->ts);
    TMR3_PWMCycleCfg(cfg->cycle);
    TMR3_PWMActDataWidth(cfg->data_width);
    TMR3_PWMEnable();
    TMR3_Enable();
    return CH_OK;
    break;
  }
  default:
    return CH_FAIL;
    break;
  }
}

ch_err_t timer_deinit(int channel)
{
  CH_RETURN_ON_FALSE((channel < 4) , CH_ERR_INVALID_ARG, TAG, "invalid Timer Channel");

  switch (channel)
  {
  case 0:
  {
    TMR0_PWMActDataWidth(0);
    TMR0_PWMDisable();
    TMR0_Disable();
    return CH_OK;
    break;
  }
  case 1:
  {
    TMR1_PWMActDataWidth(0);
    TMR1_PWMDisable();
    TMR1_Disable();
    return CH_OK;
    break;
  }
  case 2:
  {
    TMR2_PWMActDataWidth(0);
    TMR2_PWMDisable();
    TMR2_Disable();
    return CH_OK;
    break;
  }
  case 3:
  {
    TMR3_PWMActDataWidth(0);
    TMR3_PWMDisable();
    TMR3_Disable();
    return CH_OK;
    break;
  }
  default:
    return CH_FAIL;
    break;
  }
}

ch_err_t timer_set_data(int channel, uint32_t data)
{
  CH_RETURN_ON_FALSE(channel < 4, CH_ERR_INVALID_ARG, TAG, "invalid Timer Channel");

  switch (channel)
  {
  case 0:
  {
    TMR0_PWMActDataWidth(data);
    break;
  }
  case 1:
  {
    TMR1_PWMActDataWidth(data);
    break;
  }
  case 2:
  {
    TMR2_PWMActDataWidth(data);
    break;
  }
  case 3:
  {
    TMR3_PWMActDataWidth(data);
    break;
  }
  default:
    break;
  }

  return CH_OK;
}

ch_err_t timer_servo_enable(gpio_num_t pin)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(pin), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");
  gpio_set_direction(pin, GPIO_MODE_OUTPUT);
  gpio_pullup_dis(pin);

  int chan =-1;
  if(timer_io_to_channel(pin,&chan) != CH_OK)
  {
    return CH_FAIL;
  }

  timer_config_t cfg =
  {
    .channel = chan,
    .cycle = 60 * 100 * 200,
    .data_width = 0,
    .pr =High_Level,
    .ts = PWM_Times_1,
  };

  return timer_init(&cfg);
}
ch_err_t timer_ledc_enable(gpio_num_t pin)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(pin), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");
  gpio_set_direction(pin, GPIO_MODE_OUTPUT);
  gpio_pullup_dis(pin);

  int chan =-1;
  if(timer_io_to_channel(pin,&chan) != CH_OK)
  {
    return CH_FAIL;
  }

  timer_config_t cfg =
  {
    .channel = chan,
    .cycle = 100,
    .data_width = 0,
    .pr =High_Level,
    .ts = PWM_Times_1,
  };

  return timer_init(&cfg);

}


ch_err_t timer_servo_set_duty(gpio_num_t pin, uint32_t duty)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(pin), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");

  int chan =-1;
  if(timer_io_to_channel(pin,&chan) != CH_OK)
  {
    return CH_FAIL;
  }
  CH_RETURN_ON_FALSE((chan > -1), CH_FAIL, TAG, "invalid CHannel");

  return timer_set_data(chan,duty);

}


ch_err_t timer_servo_set_angle(gpio_num_t pin, uint32_t angle)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(pin), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");
  uint32_t duty = MAP_32(angle, 0, 180, 30000, 150000);
  return timer_servo_set_duty(pin,duty);
}


ch_err_t timer_servo_set_speed(gpio_num_t pin, uint32_t speed)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(pin), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");
  long _duty =0;
  if (speed == 100)
  {
    _duty = 0;
  }
  else
  {
    _duty = MAP_32(speed, 0, 200, 150000, 30000);
  }
  return timer_servo_set_duty(pin,_duty);

}
ch_err_t timer_ledc_set_duty(gpio_num_t pin, uint32_t _duty)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(pin), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");
  CH_RETURN_ON_FALSE(_duty < 101, CH_ERR_INVALID_ARG, TAG, "invalid duty precntage");

  int chan =-1;
  if(timer_io_to_channel(pin,&chan) != CH_OK)
  {
    return CH_FAIL;
  }
  CH_RETURN_ON_FALSE((chan > -1), CH_FAIL, TAG, "invalid CHannel");

  return timer_set_data(chan,_duty);
}
ch_err_t timer_servo_disable(gpio_num_t pin)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(pin), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");
  gpio_set_direction(pin, GPIO_MODE_OUTPUT);
  gpio_pulldown_dis(pin);

  int chan =-1;
  if(timer_io_to_channel(pin,&chan) != CH_OK)
  {
    return CH_FAIL;
  }
  CH_RETURN_ON_FALSE((chan > -1), CH_FAIL, TAG, "invalid CHannel");

  return timer_deinit(chan);

}
ch_err_t timer_ledc_disable(gpio_num_t pin)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(pin), CH_ERR_INVALID_ARG, TAG, "invalid gpio number");
  gpio_set_direction(pin, GPIO_MODE_OUTPUT);
  gpio_pulldown_dis(pin);

  int chan =-1;
  if(timer_io_to_channel(pin,&chan) != CH_OK)
  {
    return CH_FAIL;
  }
  CH_RETURN_ON_FALSE((chan > -1), CH_FAIL, TAG, "invalid CHannel");

  return timer_deinit(chan);

}


pwm_base_t get_io_pwm_base(gpio_num_t pin)
{
  CH_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(pin), BASE_NONE, TAG, "invalid gpio number");

  int chan =-1;
  if(timer_io_to_channel(pin,&chan) == CH_OK)
  {
    return TIMER_BASE;
  }
  else if(pwm16_io_to_channel(pin,&chan) == CH_OK)
  {
    return PWM_BASE;
  }
  else if(pwm8_io_to_channel(pin,&chan) == CH_OK)
  {
    return PWM_BASE;
  }
  else
  {
    return BASE_NONE;
  }

}