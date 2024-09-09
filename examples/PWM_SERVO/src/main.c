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

void Setup_PWM_Servo(void) {
    // 1. Configure PA12 as PWM output (push-pull, 5mA drive strength)
    GPIOA_ModeCfg(GPIO_Pin_12, GPIO_ModeOut_PP_5mA);
    
    // 2. Set PWM clock divider (Fsys / 4)
    PWMX_CLKCfg(4); // Set clock to 60 MHz / 4 = 15 MHz PWM clock
    
    // 3. Configure PWM period using 256 cycle configuration
    PWMX_CycleCfg(PWMX_Cycle_256); // Set PWM cycle to 256 cycles for 58.59 kHz frequency
    
    // 4. Set initial pulse width to 1.5 ms (neutral position, 90° = 87.9 cycles)
    PWMX_ACTOUT(CH_PWM4, 88, Low_Level, ENABLE);  // 7.5% duty cycle (1.5 ms pulse)
}

void Set_Servo_Angle(float angle) {
    // MG90 servo typically has a range from 0° to 180°
    // Pulse width range: 1 ms (58.6 cycles) to 2 ms (117.2 cycles)
    // Convert angle (0° to 180°) to corresponding number of cycles (58.6 to 117.2)
    
    float cycles = 58.6 + (angle / 180.0) * (117.2 - 58.6);  // Linear interpolation
    
    // Update PWM duty cycle based on the calculated value
    PWMX_ACTOUT(CH_PWM4, (uint16_t)cycles, Low_Level, ENABLE);
}

int main()
{
    SetSysClock(CLK_SOURCE_PLL_60MHz);

    Setup_PWM_Servo();
    
    while (1) {
        // Example: Set servo to 0° (58.6 cycles)
        Set_Servo_Angle(0);
        DelayMs(1000);  // Delay 1 second
        
        // Example: Set servo to 90° (87.9 cycles)
        Set_Servo_Angle(90);
        DelayMs(1000);  // Delay 1 second
        
        // Example: Set servo to 180° (117.2 cycles)
        Set_Servo_Angle(180);
        DelayMs(1000);  // Delay 1 second
    }
}