#include "CH59x_common.h"

__attribute__((aligned(4))) uint32_t CapBuf[32];
volatile uint8_t capFlag = 0;

/*********************************************************************
 * @fn      DebugInit
 * @brief   Initialize UART for debugging.
 * @return  none
 */
void DebugInit(void)
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

uint32_t ticksToMicroseconds(uint32_t ticks)
{
    // Assuming Fsys = 60 MHz
    return (ticks * 16 + 8) / 1000; // Optimized rounding to avoid floating-point
}

/*********************************************************************
 * @fn      processIRSignal
 * @brief   Process captured IR signal to decode the data.
 * @return  none
 */
void processIRSignal(void)
{
    uint8_t i;
    static uint8_t bit_count = 0;
    static uint32_t command = 0;
    PRINT("Processing IR Signal:\n");

    for (i = 0; i < 32; i++)
    {
        uint32_t pulseWidth = CapBuf[i] & 0x1FFFFFF;
        uint32_t pulseWidthUs = ticksToMicroseconds(pulseWidth);

        if (pulseWidthUs > 1000 && pulseWidthUs < 1300)
        {
            command = (command << 1);
            bit_count++;
        }
        else if (pulseWidthUs > 2100 && pulseWidthUs < 2300)
        {
            command = (command << 1) | 1;
            bit_count++;
        }
        else
        {
            PRINT("? ");
        }

        if (bit_count == 32)
        {
            PRINT("Received Command: 0x%08lX\n", command);
            bit_count = 0;
        }
    }
    TMR1_ITCfg(ENABLE, TMR1_2_IT_DMA_END);
    PRINT("\n");
}

/*********************************************************************
 * @fn      main
 * @brief   Main program entry point.
 * @return  none
 */
int main(void)
{
    SetSysClock(CLK_SOURCE_PLL_60MHz);
    DebugInit();

    PRINT("Starting IR Decoder @ChipID=%02X\n", R8_CHIP_ID);

    // Timer 1 capture setup
    PWR_UnitModCfg(DISABLE, UNIT_SYS_LSE);
    GPIOA_ModeCfg(GPIO_Pin_10, GPIO_ModeIN_PU);

    TMR1_CapInit(FallEdge_To_FallEdge);
    TMR1_CAPTimeoutCfg(0xFFFFFFFF);
    TMR1_DMACfg(ENABLE, (uint16_t)(uint32_t)&CapBuf[0], (uint16_t)(uint32_t)&CapBuf[32], Mode_LOOP);
    TMR1_ITCfg(ENABLE, TMR1_2_IT_DMA_END);
    PFIC_EnableIRQ(TMR1_IRQn);

    while (1)
    {
        if (capFlag)
        {
            capFlag = 0;
            processIRSignal();
        }
    }
}

/*********************************************************************
 * @fn      TMR1_IRQHandler
 * @brief   Timer 1 interrupt handler.
 * @return  none
 */
__INTERRUPT
__HIGH_CODE
void TMR1_IRQHandler(void) // TMR1 ��ʱ�ж�
{
    if (TMR1_GetITFlag(TMR1_2_IT_DMA_END))
    {
        TMR1_ITCfg(DISABLE, TMR1_2_IT_DMA_END); // ʹ�õ���DMA����+�жϣ�ע����ɺ�رմ��ж�ʹ�ܣ������һֱ�ϱ��жϡ�
        TMR1_ClearITFlag(TMR1_2_IT_DMA_END);    // ����жϱ�־
        capFlag = 1;
    }
}
