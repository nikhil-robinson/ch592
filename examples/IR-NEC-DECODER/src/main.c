#include "CH59x_common.h"
#include "ring_buffer.h"

#define LSE_32K_CRYSTAL_REMOVED         1

__attribute__((aligned(4))) uint32_t cap_buf[64];
struct ring_buffer cap_ring_buf;
ring_buffer_t p_cap_ring_buf;


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

/*********************************************************************
 * @fn      TMR1_CapInit
 *
 * @brief   外部信号捕捉功能初始化
 *
 * @param   cap     - select capture mode, refer to CapModeTypeDef
 *
 * @return  none
 */
void TMR1_CaptureInit(CapModeTypeDef cap)
{
    R8_TMR1_CTRL_MOD |= RB_TMR_COUNT_EN | RB_TMR_MODE_IN | (cap << 6);
}

uint32_t ticksToMicroseconds(uint32_t ticks)
{
    // Assuming Fsys = 60 MHz
    return (ticks * 16 + 8) / 1000; // Optimized rounding to avoid floating-point
    /*
    *  return (ticks / 60000); //Is the value calculated by the above formula the lighting effect?
    */
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
    uint32_t dat_buf[32];
    PRINT("Processing IR Signal:\n");

    rb_read_data_without_update_len(p_cap_ring_buf, dat_buf, 32);
    PFIC_DisableIRQ(TMR1_IRQn);
    rb_read_update_len(p_cap_ring_buf, 32);
    PFIC_EnableIRQ(TMR1_IRQn);
    for(i=0; i<32; ++i) 
    {
        PRINT("%x | %d\n", ((dat_buf[i]>>25) & 0x01), dat_buf[i] & 0x1ffffff); // 26bit, 最高位表示 高电平还是低电平
    }
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
    p_cap_ring_buf = &cap_ring_buf;
    rb_param_init(p_cap_ring_buf, cap_buf, sizeof(cap_buf)/sizeof(uint32_t));

    // Timer 1 capture setup
#if LSE_32K_CRYSTAL_REMOVED
    /* When using the timer capture function of PA10 or PA11, 
    * the externally connected 32K crystal must be removed.
    */
    PWR_UnitModCfg(DISABLE, UNIT_SYS_LSE);
    GPIOA_ModeCfg(GPIO_Pin_10, GPIO_ModeIN_PU); //PA10 for capture
#else 
    GPIOPinRemap(ENABLE, RB_PIN_TMR1);
    GPIOB_ModeCfg(GPIO_Pin_10, GPIO_ModeIN_PU); //PB10 for capture
#endif

    R8_TMR1_CTRL_MOD = RB_TMR_ALL_CLEAR;
    TMR1_CaptureInit(FallEdge_To_FallEdge); //rewrite function
    /* Because only the lower 25 bits of R32_TMRx_FIFO are valid in capture mode, 
    * R32_TMR1_CNT_END can only be set to a 25-bit value.
    */
    TMR1_CAPTimeoutCfg(30000000); //set the capture timeout,500ms
    TMR1_ITCfg(ENABLE, TMR0_3_IT_FIFO_HF);
    PFIC_EnableIRQ(TMR1_IRQn);
    R8_TMR1_CTRL_MOD &= ~RB_TMR_ALL_CLEAR;

    while (1)
    {
        if (p_cap_ring_buf->dat_len > 32)
        {
            processIRSignal();
        }
        __nop();
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
    volatile uint8_t fifo_count;

    if (TMR1_GetITFlag(TMR0_3_IT_FIFO_HF))
    {
        TMR1_ClearITFlag(TMR0_3_IT_FIFO_HF);    // ����жϱ�־
        fifo_count = R8_TMR1_FIFO_COUNT;
        while (fifo_count)
        {
            rb_write_byte(p_cap_ring_buf, R32_TMR1_FIFO);
            --fifo_count;
        }
    }
}
