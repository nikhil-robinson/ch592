#include "CH59x_common.h"

int main(void){

	SetSysClock(CLK_SOURCE_PLL_60MHz);
	GPIOA_ModeCfg(GPIO_Pin_5, GPIO_ModeOut_PP_20mA);
	GPIOB_ModeCfg(GPIO_Pin_22, GPIO_ModeIN_PU);
    GPIOB_ITModeCfg(GPIO_Pin_22, GPIO_ITMode_FallEdge); // �½��ػ���
    PFIC_EnableIRQ(GPIO_B_IRQn);
	while(1);
}


__INTERRUPT
__HIGH_CODE
void GPIOB_IRQHandler(void)
{
	GPIOA_InverseBits(GPIO_Pin_5);
    GPIOB_ClearITFlagBit(GPIO_Pin_22);
}
