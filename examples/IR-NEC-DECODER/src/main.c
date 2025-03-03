#include "CH59x_common.h"
#include "ch_log.h"

#define LSE_32K_CRYSTAL_REMOVED         1

#define IR_REC_IGNORE_VALUE             300         //300*1000000us/60MHz = 5us, waveforms within 5us are considered interference
#define IR_REC_CNT_VALUE                5000        //5000*1000000us/60MHz = 83us, interval of more than 83us is considered a signal switch
#define IR_REC_RAW_DATA_LEN             66


#define EXAMPLE_IR_NEC_DECODE_MARGIN    12000       //12000*1000000us/60MHz = 200us, Tolerance for parsing RMT symbols into bit stream
/**
 * @brief NEC timing spec
 */
#define NEC_LEADING_CODE_DURATION_0     540000      //540000*1000000us/60MHz = 9000us
#define NEC_LEADING_CODE_DURATION_1     270000      //270000*1000000us/60MHz = 4500us
#define NEC_PAYLOAD_ZERO_DURATION_0     33600       //33600*1000000us/60MHz = 560us
#define NEC_PAYLOAD_ZERO_DURATION_1     33600       //33600*1000000us/60MHz = 560us
#define NEC_PAYLOAD_ONE_DURATION_0      33600       //33600*1000000us/60MHz = 560us
#define NEC_PAYLOAD_ONE_DURATION_1      101400      //101400*1000000us/60MHz = 1690us
#define NEC_REPEAT_CODE_DURATION_0      540000      //540000*1000000us/60MHz = 9000us
#define NEC_REPEAT_CODE_DURATION_1      135000      //135000*1000000us/60MHz = 9000us

#define USB_DEBUG 1

#define TAG "NEC"


typedef struct _ir_rec_controller_struct
{
    uint32_t data[IR_REC_RAW_DATA_LEN];
    uint16_t data_index;
    uint8_t state;
    uint8_t last_level;
} ir_rec_ctr_t;

typedef struct _rmt_symbol_word
{
    uint32_t duration0;
    uint32_t duration1;
}rmt_symbol_word_t;

enum
{
    IR_REC_STATE_FREE = 0,
    IR_REC_STATE_WAIT_START1, //9ms low level
    IR_REC_STATE_WAIT_START2, //4.5ms high level
    IR_REC_STATE_WAIT_DATA,
    IR_REC_STATE_OK,
    IR_REC_STATE_ERR,
};


volatile ir_rec_ctr_t g_ir_rec_ctr;

/*********************************************************************
 * @fn      DebugInit
 * @brief   Initialize UART for debugging.
 * @return  none
 */
void DebugInit(void)
{

#if USB_DEBUG

    app_usb_init();
    ch_set_log_level(CH_LOG_INFO);
    ch_set_log_mode(CH_LOG_USB);

#else
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
#endif
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

/**
 * @brief Check whether a duration is within expected range
 */
static inline BOOL nec_check_in_range(uint32_t signal_duration, uint32_t spec_duration)
{
    return (signal_duration < (spec_duration + EXAMPLE_IR_NEC_DECODE_MARGIN)) &&
           (signal_duration > (spec_duration - EXAMPLE_IR_NEC_DECODE_MARGIN));
}

/**
 * @brief Check whether a RMT symbol represents NEC logic zero
 */
static BOOL nec_parse_logic0(rmt_symbol_word_t *rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_PAYLOAD_ZERO_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_PAYLOAD_ZERO_DURATION_1);
}

/**
 * @brief Check whether a RMT symbol represents NEC logic one
 */
static BOOL nec_parse_logic1(rmt_symbol_word_t *rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_PAYLOAD_ONE_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_PAYLOAD_ONE_DURATION_1);
}

/*********************************************************************
 * @fn      processIRSignal
 * @brief   Process captured IR signal to decode the data.
 * @return  none
 */
void processIRSignal(void)
{
    uint16_t i;
    rmt_symbol_word_t *cur = (rmt_symbol_word_t *)g_ir_rec_ctr.data;
    uint16_t address = 0;
    uint16_t command = 0;
    CH_LOGI(TAG,"Processing IR Signal:\n");

    CH_LOGI(TAG,"%x %x\n", cur->duration0, cur->duration1);
    BOOL valid_leading_code = nec_check_in_range(cur->duration0, NEC_LEADING_CODE_DURATION_0) &&
                              nec_check_in_range(cur->duration1, NEC_LEADING_CODE_DURATION_1);
    
    if (!valid_leading_code)
    {
        CH_LOGI(TAG,"Frame Error\n");
        return;
    }

    cur++;
    for (i = 0; i < 16; i++)
    {
        CH_LOGI(TAG,"[%d] %d %d\n", i, cur->duration0/60, cur->duration1/60);
        if (nec_parse_logic1(cur))
        {
            address |= 1 << i;
        }
        else if (nec_parse_logic0(cur))
        {
            address &= ~(1 << i);
        }
        else
        {
            CH_LOGI(TAG,"Frame Error\n");
            return;
        }
        cur++;
    }

    for (i = 0; i < 16; i++)
    {
        CH_LOGI(TAG,"[%d] %d %d\n", i, cur->duration0/60, cur->duration1/60);
        if (nec_parse_logic1(cur))
        {
            command |= 1 << i;
        }
        else if (nec_parse_logic0(cur))
        {
            command &= ~(1 << i);
        }
        else
        {
            CH_LOGI(TAG,"Frame Error\n");
            return;
        }
        cur++;
    }

    CH_LOGI(TAG,"address:%x command:%x\n", address, command);
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

    CH_LOGI(TAG,"Starting IR Decoder @ChipID=%02X\n", R8_CHIP_ID);
    // p_cap_ring_buf = &cap_ring_buf;
    // rb_param_init(p_cap_ring_buf, cap_buf, sizeof(cap_buf)/sizeof(uint32_t));

    // g_ir_rec_ctr.data_len = 0;
    g_ir_rec_ctr.state = IR_REC_STATE_WAIT_START1;
    g_ir_rec_ctr.data_index = 0;
    g_ir_rec_ctr.last_level = 1;

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

    /* Tim1 init, used to capture signal */
    R8_TMR1_CTRL_MOD = RB_TMR_ALL_CLEAR; 
    TMR1_CaptureInit(Edge_To_Edge); //rewrite function
    /* Because only the lower 25 bits of R32_TMRx_FIFO are valid in capture mode, 
    * R32_TMR1_CNT_END can only be set to a 25-bit value.
    */
    TMR1_CAPTimeoutCfg(30000000); //set the capture timeout,500ms
    TMR1_ITCfg(ENABLE, TMR0_3_IT_FIFO_HF);
    PFIC_EnableIRQ(TMR1_IRQn);
    R8_TMR1_CTRL_MOD &= ~RB_TMR_ALL_CLEAR;

    /* Tim2 init, used to receive signal timeout */
    R8_TMR2_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R32_TMR2_CNT_END = FREQ_SYS / 5; //Set the timing time to 200ms, the longest frame of data will not exceed 200ms
    R8_TMR2_CTRL_MOD |= RB_TMR_COUNT_EN;
    TMR2_ITCfg(ENABLE, TMR0_3_IT_CYC_END);
    PFIC_EnableIRQ(TMR2_IRQn);
    // R8_TMR2_CTRL_MOD &= ~RB_TMR_ALL_CLEAR;
    
    while (1)
    {
        if (g_ir_rec_ctr.data_index == IR_REC_RAW_DATA_LEN)
        {
            R8_TMR2_CTRL_MOD |= RB_TMR_ALL_CLEAR;
            // for(uint16_t i = 0; i < IR_REC_RAW_DATA_LEN; ++i) 
            // {
            //     CH_LOGI(TAG,"%x\n", g_ir_rec_ctr.data[i]);
            // }
            processIRSignal();
            g_ir_rec_ctr.state = IR_REC_STATE_WAIT_START1;
            g_ir_rec_ctr.data_index = 0;
        }
        
        DelayMs(10);
    }
}

/*********************************************************************
 * @fn      TMR1_IRQHandler
 * @brief   Timer 1 interrupt handler.
 * @return  none
 */
__INTERRUPT
__HIGH_CODE
void TMR1_IRQHandler(void)
{
    volatile uint8_t fifo_count;
    volatile uint32_t cap_value;

    if (TMR1_GetITFlag(TMR0_3_IT_FIFO_HF))
    {
        TMR1_ClearITFlag(TMR0_3_IT_FIFO_HF);
        fifo_count = R8_TMR1_FIFO_COUNT;
        while (fifo_count)
        {
            cap_value = R32_TMR1_FIFO;
            // CH_LOGI(TAG,"%x\n", cap_value);
            if(cap_value & 0x2000000) //high level is captured
            {
                cap_value &= 0x1FFFFFF;
                if(g_ir_rec_ctr.state == IR_REC_STATE_WAIT_START2)
                {
                    /* 4.5ms high level of leading code */
                    if(nec_check_in_range(cap_value, NEC_LEADING_CODE_DURATION_1))
                    {
                        g_ir_rec_ctr.state = IR_REC_STATE_WAIT_DATA;
                        g_ir_rec_ctr.data[1] = cap_value;
                        g_ir_rec_ctr.data_index = 2;
                        g_ir_rec_ctr.last_level = 1;
                    }
                    else
                    {
                        R8_TMR2_CTRL_MOD |= RB_TMR_ALL_CLEAR;
                        /* Discard error frame */
                        g_ir_rec_ctr.state = IR_REC_STATE_WAIT_START1;
                        g_ir_rec_ctr.data_index = 0;
                        g_ir_rec_ctr.last_level = 1;
                    }
                }
                else if(g_ir_rec_ctr.state == IR_REC_STATE_WAIT_DATA)
                {
                    if(g_ir_rec_ctr.last_level == 0) //The previous capture was low level
                    {
                        if(cap_value > IR_REC_CNT_VALUE) //When the time exceeds the maximum value, the next waveform counting starts
                        {
                            if(g_ir_rec_ctr.data_index < IR_REC_RAW_DATA_LEN)
                            {
                                g_ir_rec_ctr.data[g_ir_rec_ctr.data_index] = cap_value;
                                ++g_ir_rec_ctr.data_index;
                                // CH_LOGI(TAG,"{%d} %d\n", g_ir_rec_ctr.data_index, cap_value/60);
                            }
                            g_ir_rec_ctr.last_level = 1;
                        }
                        else
                        {
                            /* Discard interfering signals */
                        }
                    }
                    else
                    {
                        R8_TMR2_CTRL_MOD |= RB_TMR_ALL_CLEAR;
                        /* Timeout, end capture */
                        g_ir_rec_ctr.state = IR_REC_STATE_WAIT_START1;
                        g_ir_rec_ctr.data_index = 0;
                        g_ir_rec_ctr.last_level = 1;
                    }
                }
            }
            else //low level is captured
            {
                if(g_ir_rec_ctr.state == IR_REC_STATE_WAIT_START1)
                {
                    /* The infrared default is high level, and the low level means the conversion officially starts */
                    /* 9ms low level of leading code */
                    if(nec_check_in_range(cap_value, NEC_LEADING_CODE_DURATION_0))
                    {
                        g_ir_rec_ctr.state = IR_REC_STATE_WAIT_START2;
                        g_ir_rec_ctr.data[0] = cap_value;
                        g_ir_rec_ctr.data_index = 1;
                        g_ir_rec_ctr.last_level = 0;

                        R8_TMR2_CTRL_MOD &= ~RB_TMR_ALL_CLEAR;
                    }
                    else
                    {
                        /* Discard interfering signals */
                    }
                }
                else if (g_ir_rec_ctr.state == IR_REC_STATE_WAIT_START2) 
                {
                    R8_TMR2_CTRL_MOD |= RB_TMR_ALL_CLEAR;
                    /* Discard error frame */
                    g_ir_rec_ctr.state = IR_REC_STATE_WAIT_START1;
                    g_ir_rec_ctr.data_index = 0;
                    g_ir_rec_ctr.last_level = 0;
                }
                else if(g_ir_rec_ctr.state == IR_REC_STATE_WAIT_DATA)
                {
                    if(g_ir_rec_ctr.last_level != 0)
                    {
                        if(cap_value > IR_REC_CNT_VALUE) //When the time exceeds the maximum value, the next waveform counting starts
                        {
                            if(g_ir_rec_ctr.data_index < IR_REC_RAW_DATA_LEN)
                            {
                                g_ir_rec_ctr.data[g_ir_rec_ctr.data_index] = cap_value;
                                ++g_ir_rec_ctr.data_index;
                                // CH_LOGI(TAG,"{%d} %d\n", g_ir_rec_ctr.data_index, cap_value/60);
                            }
                            g_ir_rec_ctr.last_level = 0;
                        }
                        else
                        {
                            /* Discard interfering signals */
                        }
                    }
                    else
                    {
                        R8_TMR2_CTRL_MOD |= RB_TMR_ALL_CLEAR;
                        /* Timeout, end capture */
                        g_ir_rec_ctr.state = IR_REC_STATE_WAIT_START1;
                        g_ir_rec_ctr.data_index = 0;
                        g_ir_rec_ctr.last_level = 0;
                    }
                }

            }
            --fifo_count;
        }
    }
}

/*********************************************************************
 * @fn      TMR2_IRQHandler
 * @brief   Timer 2 interrupt handler.
 * @return  none
 */
__INTERRUPT
__HIGH_CODE
void TMR2_IRQHandler(void)
{
    if (TMR2_GetITFlag(TMR0_3_IT_CYC_END))
    {
        TMR2_ClearITFlag(TMR0_3_IT_CYC_END);
        R8_TMR2_CTRL_MOD |= RB_TMR_ALL_CLEAR;
        CH_LOGI(TAG,"Receive singnal timeout\n");
        for(uint16_t i=0; i<g_ir_rec_ctr.data_index; ++i) 
        {
            CH_LOGI(TAG,"[%d] %d\n", i, g_ir_rec_ctr.data[i]/60);
        }
        g_ir_rec_ctr.state = IR_REC_STATE_WAIT_START1;
        g_ir_rec_ctr.data_index = 0;
    }
}