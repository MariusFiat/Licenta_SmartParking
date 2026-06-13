/* 
    * pwm_if.c
    *
    *  Created on: 31 May 2026
    *  MCU Abstraction Layer for PWM module
    *  This module will provide the necessary interfaces that will generate the Servo's PWM signals
*/

#include <stdio.h>
#include <stdint.h>
#include "board_config.h"

#include "pwm_if.h"

void x_pwm_init(const uint8_t channel)
{
    /* Clear the registers for this slice*/
    x_HWREG32(x_PWM_CSR + (channel * x_PWM_CHANNEL_x_OFFSET)) = 0;
    x_HWREG32(x_PWM_DIV + (channel * x_PWM_CHANNEL_x_OFFSET)) = 1;
    x_HWREG32(x_PWM_CTR + (channel * x_PWM_CHANNEL_x_OFFSET)) = 0;
    x_HWREG32(x_PWM_CCR + (channel * x_PWM_CHANNEL_x_OFFSET)) = 0;
    x_HWREG32(x_PWM_TOP + (channel * x_PWM_CHANNEL_x_OFFSET)) = 0;
}

void x_pwm_set_div(const uint8_t channel, const uint16_t div_int, const uint8_t div_frac)
{
    uint32_t div_value = (div_int << x_PWM_DIV_INT_OFFSET) | (div_frac << x_PWM_DIV_FRAC_OFFSET);
    x_HWREG32(x_PWM_DIV + (channel * x_PWM_CHANNEL_x_OFFSET)) = div_value;
}

void x_pwm_set_wrap(const uint8_t channel, const uint16_t wrap)
{
    x_HWREG32(x_PWM_TOP + (channel * x_PWM_CHANNEL_x_OFFSET)) = wrap;
}

