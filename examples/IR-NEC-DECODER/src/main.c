

#include "CH59x_common.h"

#define IR_PIN GPIO_Pin_11        // Connect the IR receiver to PB11
#define TIMER_CHANNEL TMR2        // Using Timer 2 for capture

// NEC Protocol Timings (in microseconds)
#define NEC_HEADER_MIN 9000       // 9 ms header pulse
#define NEC_HEADER_MAX 9500       // 9.5 ms header pulse
#define NEC_BIT_0_MAX 1120        // Logic 0: 560 us + 560 us low
#define NEC_BIT_1_MIN 1680        // Logic 1: 560 us + 1690 us low

// Function prototypes
void TimerCapture_Init(void);
void TMR2_IRQHandler(void);
void Decode_IR(uint32_t pulse_width);

// Global variables
volatile uint32_t last_count = 0;
volatile uint8_t bit_count = 0;
volatile uint32_t command = 0;


void DebugInit(void)
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}


void TimerCapture_Init(void) {
    GPIOPinRemap(ENABLE, RB_PIN_TMR2);       // Remap TMR2 to the correct pin

    // Initialize TMR2 for falling edge to falling edge capture
    TMR2_EXTSingleCounterInit(FallEdge_To_FallEdge);
    TMR2_CountOverflowCfg(10000);            // Set overflow to 10 ms

    // Enable TMR2 interrupt
    TMR2_ClearITFlag(TMR0_3_IT_CYC_END);
    PFIC_EnableIRQ(TMR2_IRQn);
    TMR2_ITCfg(ENABLE, TMR0_3_IT_CYC_END);

    // // Start the timer
    // TMR2_Cmd(ENABLE);
}

void TMR2_IRQHandler(void) {
    uint32_t current_count = TMR2_GetCurrentCount();

    if (TMR2_GetITFlag(TMR0_3_IT_CYC_END)) {
        uint32_t pulse_width = current_count - last_count;
        last_count = current_count;

        // Pass the pulse width to the decoding function
        Decode_IR(pulse_width);

        // Clear the interrupt flag
        TMR2_ClearITFlag(TMR0_3_IT_CYC_END);
    }
}

void Decode_IR(uint32_t pulse_width) {
    // Check for NEC header pulse
    if (pulse_width >= NEC_HEADER_MIN && pulse_width <= NEC_HEADER_MAX) {
        PRINT("NEC Header detected\n");
        bit_count = 0;
        command = 0;
        return;
    }

    // Decode NEC bits
    if (pulse_width < NEC_BIT_0_MAX) {
        command = (command << 1);    // Logic 0
        bit_count++;
    } else if (pulse_width >= NEC_BIT_1_MIN) {
        command = (command << 1) | 1;  // Logic 1
        bit_count++;
    }

    // Check if a full command is received (32 bits)
    if (bit_count == 32) {
        PRINT("Received Command: 0x%08lX\n", command);
        bit_count = 0;
    }
}


int main()
{
    SetSysClock(CLK_SOURCE_PLL_60MHz);
    DebugInit();

    GPIOB_ModeCfg(IR_PIN, GPIO_ModeIN_PD);   // Set IR_PIN as input with pull-down
    TimerCapture_Init();

    while(1);
}