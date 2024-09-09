/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description        : ��ʱ������
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH59x_common.h"


/*********************************************************************
 * @fn      DebugInit
 *
 * @brief   ���Գ�ʼ��
 *
 * @return  none
 */
void DebugInit(void)
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

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
    DebugInit();

    PRINT("Start @ChipID=%02X\n", R8_CHIP_ID);
    GPIOB_ResetBits(GPIO_Pin_22);
    GPIOB_ModeCfg(GPIO_Pin_22, GPIO_ModeOut_PP_5mA);
    TMR3_PWMInit(High_Level, PWM_Times_1);
    TMR3_PWMCycleCfg(60 * 100 * 200); // ���� 100us  ���67108864
    TMR3_PWMActDataWidth(300000); // ռ�ձ� 50%, �޸�ռ�ձȱ�����ʱ�رն�ʱ��
    TMR3_PWMEnable();
    TMR3_Enable();

    while(1);
}