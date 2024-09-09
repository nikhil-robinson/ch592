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

__attribute__((aligned(4))) uint32_t CapBuf[100];
__attribute__((aligned(4))) uint32_t PwmBuf[100];


#define TIMER_PWMINIT(NUM,pr,ts)   TMR##NUM _PWMInit(pr, ts);
#define TIMER_CYCLECFG(NUM,cycle)  TMR##NUM  _PWMCycleCfg(cycle);
#define TIMER_DATAWIDTH(NUM,width) TMR##NUM  _PWMActDataWidth(width);
#define TIMER_PWMENABLE(NUM) TMR##NUM  _PWMEnable();
#define TIMER_ENABLE(NUM) TMR##NUM  _Enable();

volatile uint8_t capFlag = 0;

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
    GPIOB_ModeCfg(GPIO_Pin_22, GPIO_ModeOut_PP_20mA);
    TMR3_PWMInit(High_Level, PWM_Times_1);
    TMR3_PWMCycleCfg(60 * 100 * 200); // ���� 100us  ���67108864
    TMR3_PWMActDataWidth(30000); // ռ�ձ� 50%, �޸�ռ�ձȱ�����ʱ�رն�ʱ��
    TMR3_PWMEnable();
    TMR3_Enable();

    while (TRUE)
    {
        TMR3_PWMActDataWidth(30000);
        DelayMs(1000);
        TMR3_PWMActDataWidth(150000);
        DelayMs(1000);
    }
    
}

/*********************************************************************
 * @fn      TMR0_IRQHandler
 *
 * @brief   TMR0�жϺ���
 *
 * @return  none
 */
__INTERRUPT
__HIGH_CODE
void TMR0_IRQHandler(void) // TMR0 ��ʱ�ж�
{
    if(TMR0_GetITFlag(TMR0_3_IT_CYC_END))
    {
        TMR0_ClearITFlag(TMR0_3_IT_CYC_END); // ����жϱ�־
        GPIOB_InverseBits(GPIO_Pin_15);
    }
}

/*********************************************************************
 * @fn      TMR1_IRQHandler
 *
 * @brief   TMR1�жϺ���
 *
 * @return  none
 */
__INTERRUPT
__HIGH_CODE
void TMR1_IRQHandler(void) // TMR1 ��ʱ�ж�
{
    if(TMR1_GetITFlag(TMR1_2_IT_DMA_END))
    {
        TMR1_ITCfg(DISABLE, TMR1_2_IT_DMA_END); // ʹ�õ���DMA����+�жϣ�ע����ɺ�رմ��ж�ʹ�ܣ������һֱ�ϱ��жϡ�
        TMR1_ClearITFlag(TMR1_2_IT_DMA_END);    // ����жϱ�־
        capFlag = 1;
    }
}

/*********************************************************************
 * @fn      TMR2_IRQHandler
 *
 * @brief   TMR2�жϺ���
 *
 * @return  none
 */
__INTERRUPT
__HIGH_CODE
void TMR2_IRQHandler(void)
{
    if(TMR2_GetITFlag(TMR0_3_IT_CYC_END))
    {
        TMR2_ClearITFlag(TMR0_3_IT_CYC_END);
        /* ������������Ӳ���Զ����㣬���¿�ʼ���� */
        /* �û�������������Ҫ�Ĵ��� */
    }
    if(TMR2_GetITFlag(TMR1_2_IT_DMA_END))
    {
        TMR2_ClearITFlag(TMR1_2_IT_DMA_END);
        PRINT("DMA end\n");
        /* DMA ���� */
        /* �û�������������Ҫ�Ĵ��� */
    }
}
