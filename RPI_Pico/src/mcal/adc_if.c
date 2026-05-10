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


    /* 3. Configure the ADC to read from the appropriate channels (GPIO26 and GPIO27), channel 0 and 1. */
    //TODO: Here I have to configure both channel for measurement and the ROUNDROBBIN func.
    xHW_REG32(x_ADC_CS_REG) &= ~(0x7 << x_AINSEL); //? Now just the channel 0 (GPIO26) is selected.

    /* 4. Power on the ADC and wait for it to be ready before starting any conversions. */
    xHW_REG32(x_ADC_CS_REG) |= (1 << x_ADC_EN_BIT);
    while((xHW_REG32(x_ADC_CS_REG) & (1 << x_ADC_READY_BIT)) == 0);

    /* Now the ADC is ready for use. To start a new conversion, write to the ADC control register, the ADC_START bit. */
}