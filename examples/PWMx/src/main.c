/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description 		   : PWM4-11������ʾ
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH59x_common.h"

#define  PWM8     1
#define  PWM16    0

/*********************************************************************
 * @fn      main
 *
 * @brief   ������
 *
 * @return  none
 */
int main()
{
    SetSysClock(CLK_SOURCE_PLL_60MHz);

#if PWM8

    /* ����GPIO */
    GPIOA_ModeCfg(GPIO_Pin_12, GPIO_ModeOut_PP_5mA); // PA12 - PWM4
    GPIOA_ModeCfg(GPIO_Pin_13, GPIO_ModeOut_PP_5mA); // PA13 - PWM5
    GPIOB_ModeCfg(GPIO_Pin_0, GPIO_ModeOut_PP_5mA);  // PB0 - PWM6
    GPIOB_ModeCfg(GPIO_Pin_4, GPIO_ModeOut_PP_5mA);  // PB4 - PWM7
    GPIOB_ModeCfg(GPIO_Pin_6, GPIO_ModeOut_PP_5mA);  // PB6 - PWM8
    GPIOB_ModeCfg(GPIO_Pin_7, GPIO_ModeOut_PP_5mA);  // PB7 - PWM9
    GPIOB_ModeCfg(GPIO_Pin_14, GPIO_ModeOut_PP_5mA); // PB14 - PWM10
                                                     //  GPIOB_ModeCfg(GPIO_Pin_23, GPIO_ModeOut_PP_5mA); // PB23 - PWM11 �˽Ÿ���Ϊ�ⲿ��λ�ţ���Ҫ�رմ˹��ܲ��ܿ���PWM����

    PWMX_CLKCfg(4);                                   // cycle = 4/Fsys
    PWMX_CycleCfg(PWMX_Cycle_64);                     // ���� = 64*cycle
    PWMX_ACTOUT(CH_PWM4, 64 / 4, Low_Level, ENABLE);  // 25% ռ�ձ�
    PWMX_ACTOUT(CH_PWM5, 64 / 4, Low_Level, ENABLE);  // 25% ռ�ձ�
    PWMX_ACTOUT(CH_PWM6, 64 / 4, Low_Level, ENABLE);  // 25% ռ�ձ�
    PWMX_ACTOUT(CH_PWM7, 64 / 4, Low_Level, ENABLE);  // 25% ռ�ձ�
    PWMX_ACTOUT(CH_PWM8, 64 / 4, Low_Level, ENABLE);  // 25% ռ�ձ�
    PWMX_ACTOUT(CH_PWM9, 64 / 4, Low_Level, ENABLE);  // 25% ռ�ձ�
    PWMX_ACTOUT(CH_PWM10, 64 / 4, Low_Level, ENABLE); // 25% ռ�ձ�

#endif

#if PWM16

    /* ����GPIO */
    GPIOA_ModeCfg(GPIO_Pin_12, GPIO_ModeOut_PP_5mA); // PA12 - PWM4
    GPIOA_ModeCfg(GPIO_Pin_13, GPIO_ModeOut_PP_5mA); // PA13 - PWM5
    GPIOB_ModeCfg(GPIO_Pin_0, GPIO_ModeOut_PP_5mA);  // PB0 - PWM6
    GPIOB_ModeCfg(GPIO_Pin_4, GPIO_ModeOut_PP_5mA);  // PB4 - PWM7
    GPIOB_ModeCfg(GPIO_Pin_6, GPIO_ModeOut_PP_5mA);  // PB6 - PWM8
    GPIOB_ModeCfg(GPIO_Pin_7, GPIO_ModeOut_PP_5mA);  // PB7 - PWM9

    PWMX_CLKCfg(4);                                   // cycle = 4/Fsys
    PWMX_16bit_CycleCfg(60000);                       // 16 ���ݿ���ʱ��PWMʱ������
    PWMX_16bit_ACTOUT(CH_PWM4, 30000, Low_Level, ENABLE);  // 50%ռ�ձ�
    PWMX_16bit_ACTOUT(CH_PWM5, 15000, Low_Level, ENABLE);  // 25%ռ�ձ�
    PWMX_16bit_ACTOUT(CH_PWM6, 45000, Low_Level, ENABLE);  // 75%ռ�ձ�
    PWMX_16bit_ACTOUT(CH_PWM7, 30000, High_Level, ENABLE); // 50%ռ�ձ�
    PWMX_16bit_ACTOUT(CH_PWM8, 15000, High_Level, ENABLE); // 25%ռ�ձ�
    PWMX_16bit_ACTOUT(CH_PWM9, 45000, High_Level, ENABLE); // 75%ռ�ձ�

#endif


    while(1);
}