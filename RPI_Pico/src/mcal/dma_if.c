/*
    * dma_if.c
    *
    *  Created on: 8 May 2024
    *  MCU Abstraction Layer for Direct Memory Access (DMA) controller of the Raspberry Pi Pico. [..]
    * [..] This module provides functions to initialize and manage DMA channels for efficient data transfer 
    * [..] between peripherals and memory without CPU intervention. 
    * In this case, the ADC module will use the DMA to save it's converstion results.
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "board_config.h"
#include "dma_if.h"

static uint8_t x_dma_disable_peripheral_reset(void){
    uint8_t retVal = RET_NOK;

    /* Disable the reset for DMA peripheral */
    xHW_REG32(x_RESET_REGISTER_BASE_ADDRESS) &= ~(1 << DMA_RESET_BIT);

    /* Waits for reset done flag to be set */
    while((xHW_REG32(x_RESET_DONE_REG) & (1 << DMA_RESET_BIT)) == 0);

    retVal = RET_OK;
    return retVal;
}

uint8_t x_dma_init(uint8_t channel_number){
    uint8_t retVal = RET_NOK;

    /* Set all registers to 0. */
    if(channel_number < DMA_NUM_OF_CHANNELS){
        if(x_dma_disable_peripheral_reset() == RET_OK){
            xHW_REG32(x_DMA_CH0_READ_ADDR + (channel_number * x_DMA_CHANNEL_REGS_OFFSET)) = 0;
            xHW_REG32(x_DMA_CH0_WRITE_ADDR + (channel_number * x_DMA_CHANNEL_REGS_OFFSET)) = 0;
            xHW_REG32(x_DMA_CH0_TRANS_COUNT + (channel_number * x_DMA_CHANNEL_REGS_OFFSET)) = 0;

            retVal = RET_OK;
        }
    } else{
        /* Invalid channel number*/
    }

    return retVal;
}

uint8_t x_dma_set_read_addr(uint32_t addr, uint8_t channel_number){
    uint8_t retVal = RET_NOK;

    if(channel_number < DMA_NUM_OF_CHANNELS){
        /* Set the source address for DMA channel with number "channel_number"*/
        xHW_REG32(x_DMA_CH0_READ_ADDR + (channel_number * x_DMA_CHANNEL_REGS_OFFSET)) = addr;
        
        retVal = RET_OK;
    } else {
        /* Invalid channel number*/
    }

    return retVal;    
}

uint8_t x_dma_set_write_addr(uint32_t addr, uint8_t channel_number){
    uint8_t retVal = RET_NOK;

    if(channel_number < DMA_NUM_OF_CHANNELS){
        /* Set the destination address for DMA channel with number "channel_number" */
        xHW_REG32(x_DMA_CH0_WRITE_ADDR + (channel_number * x_DMA_CHANNEL_REGS_OFFSET)) = addr;

        retVal = RET_OK;
    }
    
    return retVal;
}

uint8_t x_dma_set_trans_count_and_data_size(uint32_t trans_count, uint8_t data_size, uint8_t channel_number){
    uint8_t retVal = RET_NOK;

    if((channel_number < DMA_NUM_OF_CHANNELS) && ((data_size >= DMA_DATA_SIZE_1B) || (data_size <= DMA_DATA_SIZE_4B))){
        /* Set the number of transfers for DMA channel with number "channel_number"*/
        xHW_REG32(x_DMA_CH0_TRANS_COUNT + (channel_number * x_DMA_CHANNEL_REGS_OFFSET)) = trans_count;

        /* Set the data size for this transfer */
        xHW_REG32(x_DMA_CH0_CTRL_REG + (channel_number * x_DMA_CHANNEL_REGS_OFFSET)) |= (data_size << x_DMA_CH0_CTRL_DATA_SIZE_OFFSET);

        retVal = RET_OK;
    }
    
    return retVal;
}

uint8_t x_dma_set_trigger_source(uint32_t trig_source, uint8_t channel_number){
    uint8_t retVal = RET_NOK;

    if(channel_number < DMA_NUM_OF_CHANNELS){
        /* Set the DMA transfer trigger source */
        xHW_REG32(x_DMA_CH0_CTRL_REG + (channel_number * x_DMA_CHANNEL_REGS_OFFSET)) |= (trig_source << x_DMA_CH0_CTRL_TREQ_OFFSET);

        retVal = RET_OK;
    }

    return retVal;
}

uint8_t x_dma_enable_channel(uint8_t channel_number){
    uint8_t retVal = RET_NOK;

    if(channel_number < DMA_NUM_OF_CHANNELS){
        /* Set the enable bit */
        xHW_REG32(x_DMA_CH0_CTRL_REG + (channel_number * x_DMA_CHANNEL_REGS_OFFSET)) |= (1 << x_DMA_CH0_CTRL_EN_BIT);

        retVal = RET_OK;
    }

    return retVal;
}

uint8_t x_dma_set_chain_to(uint8_t chain_to_channel_number, uint8_t channel_number){
    uint8_t retVal = RET_NOK;

    if((channel_number < DMA_NUM_OF_CHANNELS) && (chain_to_channel_number < DMA_NUM_OF_CHANNELS)){
        xHW_REG32(x_DMA_CH0_CTRL_REG) |= (channel_number << x_DMA_CH0_CTRL_CHAIN_TO_OFFSET);
        
        retVal = RET_OK;
    }
    return retVal;
}