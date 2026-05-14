/*
    * brightness_module.c
    *
    *  Created on: 17 Apr 2024
    *   This module is responsible for checking the status of the parking lot lights based on the ambient light conditions. It uses an ADC to read the ambient light level 
    * [..] decide if the lights should be turned on or off. The module also includes a timer to periodically check the ambient light level and adjust the brightness accordingly.
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"   
#include "hardware/i2c.h"

#include "board_config.h"
#include "brightness_module.h"
#include "mcal/adc_if.h"
#include "mcal/dma_if.h"

#define BRIGHNESS_SENSOR_CHANNEL_0 0 /* ADC channel for GPIO26 */
#define BRIGHNESS_SENSOR_CHANNEL_1 1 /* ADC channel for GPIO27 */
#define ENABLED_CHANNELS BRIGHNESS_SENSOR_CHANNEL_0

#define DMA_CHANNEL_SENSOR1_RAM 0 /* Channel that moves the sensor1 measured value into RAM*/
#define DMA_RE_ARM_CHANNEL_SENSOR1 1 /* Channel that re-arm the DMA channel that reads the measured value for sensor 1*/
#define TRANS_COUNT_VALUE 0xFFFFFFFF

static uint32_t trans_count = TRANS_COUNT_VALUE;
static uint32_t measured_value_sensor1 = 0;

void init_brightness_submodules(void){
    /* This function will initialize and configuree the ADC and DMA modules */

    /* Init and configure DMA */
    /* Init the channel that moves the measured values into the RAM*/
    x_dma_init(DMA_CHANNEL_SENSOR1_RAM);
    x_dma_set_read_addr((uint32_t)(x_ADC_BASE + x_ADC_FIFO_REG_OFFSET), DMA_CHANNEL_SENSOR1_RAM);
    x_dma_set_write_addr((uint32_t)&measured_value_sensor1, DMA_CHANNEL_SENSOR1_RAM);
    x_dma_set_trans_count_and_data_size(trans_count, DMA_DATA_SIZE_2B, DMA_CHANNEL_SENSOR1_RAM);
    x_dma_set_trigger_source(x_DMA_ADC_DREQ, DMA_CHANNEL_SENSOR1_RAM);
    x_dma_set_chain_to(DMA_RE_ARM_CHANNEL_SENSOR1, DMA_CHANNEL_SENSOR1_RAM);

    /* Init the re_arm channel for first dma_adc_ram channel */
    x_dma_init(DMA_RE_ARM_CHANNEL_SENSOR1);
    x_dma_set_trans_count_and_data_size(1, DMA_DATA_SIZE_4B, DMA_RE_ARM_CHANNEL_SENSOR1);
    x_dma_set_read_addr((uint32_t)&trans_count, DMA_RE_ARM_CHANNEL_SENSOR1);
    x_dma_set_write_addr((uint32_t)(x_DMA_CH0_ALIAS1_TRIGGER_REG), DMA_RE_ARM_CHANNEL_SENSOR1);

    /* Start DMA channels */
    x_dma_enable_channel(DMA_CHANNEL_SENSOR1_RAM);

    /* Init and configure ADC */
    x_adc_init();
    x_adc_set_channels(ENABLED_CHANNELS);
    x_adc_enable_dreq();
    x_adc_enable_fifo();
    x_adc_set_divider(x_ADC_MAX_DIVIDER_VALUE);
    x_adc_enable();

    /* Start adc many conversions */
    x_adc_start_many();
}

uint32_t get_brightness(void){
    return measured_value_sensor1 & 0xFFF; 
}