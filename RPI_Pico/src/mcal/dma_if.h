#ifndef __DMA_H
#define __DMA_H

//* DMA Details */
#define DMA_NUM_OF_CHANNELS                     12
#define DMA_DATA_SIZE_1B                        0x0
#define DMA_DATA_SIZE_2B                        0x1
#define DMA_DATA_SIZE_4B                        0x2

//* List of registers */
#define x_DMA_BASE_ADDRESS                      0x50000000                          /* The base address for DMA pheripheral*/
#define x_DMA_CHANNEL_REGS_OFFSET               0x040      
#define x_DMA_CH0_READ_ADDR                     (x_DMA_BASE_ADDRESS + 0x000)        /* The read address register for DMA channel 0 */
#define x_DMA_CH0_WRITE_ADDR                    (x_DMA_BASE_ADDRESS + 0x004)        /* The write address register for DMA channel 0 */
#define x_DMA_CH0_TRANS_COUNT                   (x_DMA_BASE_ADDRESS + 0x008)        /* The register that contains the number of transfers for each DMA transfer sequence */
#define x_DMA_CH0_CTRL_REG                      (x_DMA_BASE_ADDRESS + 0x00C)        /* Control and status register for DMA channel 0 */

#define x_DMA_CH0_CTRL_DATA_SIZE_OFFSET         2                                   /* DATA_SIZE offset in CTRL reg*/
#define x_DMA_CH0_CTRL_EN_BIT                   0                                   /* Enable bit offset in CTRL reg. Enables the channel that will start a new trasnfer when a TREQ is enabled. */
#define x_DMA_CH0_CTRL_TREQ_OFFSET              15                                  /* TREQ offset in CTRL reg. This field sets the REQ source for a DMA transfer */
#define DMA_RESET_BIT                           2
#define x_DMA_CH0_CTRL_CHAIN_TO_OFFSET          11

#define x_DMA_ADC_DREQ                          0x24                                /* TREQ value for ADC DMA request*/

//This DMA module contains 12 independent channels. Each chanel has 64b reserved for it's registers
//[..] and bcs of that, I can configure every channel with the same function + channel offset.

//* Public functions */
uint8_t x_dma_init(uint8_t channel_number);                                         /* Disable the reset and set registers to 0*/
uint8_t x_dma_set_read_addr(uint32_t addr, uint8_t channel_number);
uint8_t x_dma_set_write_addr(uint32_t addr, uint8_t channel_number);
uint8_t x_dma_set_trans_count_and_data_size(uint32_t trans_count, uint8_t data_size, uint8_t channel_number);
uint8_t x_dma_set_trigger_source(uint32_t trig_source, uint8_t channel_number);
uint8_t x_dma_enable_channel(uint8_t channel_number);
uint8_t x_dma_set_chain_to(uint8_t chain_to_channel_number, uint8_t channel_number);


#endif /* __DMA_H */