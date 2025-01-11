#include "CH59x_common.h"

__attribute__((aligned(4))) uint32_t CapBuf[34];

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

int main()
{
    uint8_t i;

    SetSysClock(CLK_SOURCE_PLL_60MHz);

    /* ���ô��ڵ��� */
    DebugInit();
    PRINT("Start @ChipID=%02X\n", R8_CHIP_ID);

    PWR_UnitModCfg(DISABLE, UNIT_SYS_LSE); // ע���������LSE�������ţ�Ҫ��֤�رղ���ʹ����������
    GPIOA_ResetBits(GPIO_Pin_10);          // ����PWM�� PA10
    GPIOA_ModeCfg(GPIO_Pin_10, GPIO_ModeIN_PU);

    TMR1_CapInit(FallEdge_To_FallEdge);
    TMR1_CAPTimeoutCfg(0xFFFFFFFF); // ���ò�׽��ʱʱ��
    TMR1_DMACfg(ENABLE, (uint16_t)(uint32_t)&CapBuf[0], (uint16_t)(uint32_t)&CapBuf[34], Mode_LOOP);
    TMR1_ITCfg(ENABLE, TMR1_2_IT_DMA_END); // ����DMA����ж�
    PFIC_EnableIRQ(TMR1_IRQn);


    while (1)
    {
        while(capFlag == 0);
        capFlag = 0;
        for(i = 0; i < 34; i++)
        {
            PRINT("%08ld ", CapBuf[i] & 0x1ffffff); // 26bit, ���λ��ʾ �ߵ�ƽ���ǵ͵�ƽ
        }
        PRINT("\n");
    }

}


__INTERRUPT
__HIGH_CODE
void TMR1_IRQHandler(void) // TMR1 ��ʱ�ж�
{
    if(TMR1_GetITFlag(TMR1_2_IT_DMA_END))
    {
        // TMR1_ITCfg(DISABLE, TMR1_2_IT_DMA_END); // ʹ�õ���DMA����+�жϣ�ע����ɺ�رմ��ж�ʹ�ܣ������һֱ�ϱ��жϡ�
        TMR1_ClearITFlag(TMR1_2_IT_DMA_END);    // ����жϱ�־
        capFlag = 1;
    }
}