/*
    * adc_if.c
    *
    *  Created on: 8 May 2024
    *  MCU Abstraction Layer for ADC peripheral. 
    *  This file provide the necessary interfaces for ADC usage.
*/

#include <stdio.h>
#include <stdint.h>
#include "board_config.h"

#include "adc_if.h"

#define ADC_ALL_CHANNELS_ENABLED                0x0F

uint16_t x_read_ambient_light_once(){
    /* Start one conversion */
    xHW_REG32(x_ADC_CS_REG) |= (1 << x_ADC_START_ONCE_BIT);

    /* Wait for conversion to be performed */
    while((xHW_REG32(x_ADC_CS_REG) & (1 << x_ADC_READY_BIT)) == 0);

    /* Read and return the result */
    return xHW_REG32(x_ADC_RESULT_REG) & 0xFFF;
}

void x_adc_init(void){
    /* Disable the digital functionality of GPIO26 and GPIO27 to use them as ADC inputs. */
    xHW_REG32(x_GPIO26_PAD_CTRL_REG) &= ~(1 << x_IE_BIT);
    xHW_REG32(x_GPIO26_PAD_CTRL_REG) |= (1 << x_OD_BIT);
    xHW_REG32(x_GPIO27_PAD_CTRL_REG) &= ~(1 << x_IE_BIT);
    xHW_REG32(x_GPIO27_PAD_CTRL_REG) |= (1 << x_OD_BIT);

    /* 1. Enable the clock source for the ADC module to provide it with the necessary clock signal. */
    xHW_REG32(x_CLOCK_ADC_REG) &= ~(x_AUXSRC_CLK_USB_MASK); /* Select the USB clock source */
    xHW_REG32(x_CLOCK_ADC_REG) |= (1 << x_ENABLE_CLOCK_BIT); /* Enable the clock for ADC */

    /* 2. Reset the ADC peripheral to ensure it's in a known state. */
    xHW_REG32(x_RESET_REGISTER_BASE_ADDRESS) &= ~(1 << x_ADC_RESET_BIT); /* Deassert the reset for ADC */
    while((xHW_REG32(x_RESET_DONE_REG) & (1 << x_ADC_RESET_BIT)) == 0); /* Wait for the reset to complete */

    /* At this moment, the adc is ready to be configured. */
}

uint8_t x_adc_set_channels(uint8_t channel_mask){
    /* Configure the ADC to read from the specified channels based on the provided mask. */
    uint8_t retVal = RET_NOK;

    if(channel_mask <= 0xF){
        xHW_REG32(x_ADC_CS_REG) &= ~(0x7 << x_AINSEL); /* Clear the AINSEL field. */
        xHW_REG32(x_ADC_CS_REG) |= (channel_mask << x_AINSEL);
    }
    else{
        /* Invalid channel */
    }

    return retVal;
}

uint8_t x_adc_enable(void){
    uint8_t retVal = RET_NOK;
    
    xHW_REG32(x_ADC_CS_REG) |= (1 << x_ADC_EN_BIT);
    while((xHW_REG32(x_ADC_CS_REG) & (1 << x_ADC_READY_BIT)) == 0);

    retVal = RET_OK;
    return retVal;
}