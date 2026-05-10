#ifndef __ADC_IF_H__
#define __ADC_IF_H__

/* ADC peripheral's registers */
#define x_ADC_RESET_BIT                       (0x0) /* Bit to reset the ADC peripheral */

#define x_ADC_BASE                            (0x4004C000) /* Base address for ADC */
#define x_ADC_CS_OFFSET                       (0x00) 
#define x_ADC_CS_REG                          (x_ADC_BASE + x_ADC_CS_OFFSET) /* Control and Status register for ADC */
#define x_ADC_RESULT_OFFSET                   (0x04) /* Result register offset for ADC */
#define x_ADC_RESULT_REG                      (x_ADC_BASE + x_ADC_RESULT_OFFSET) /* Result register for ADC */
#define x_AINSEL                              12  /* Channel select bits*/
#define x_ADC_EN_BIT                          0  /* Power on ADC module bit */
#define x_ADC_READY_BIT                       8  /* Ready flag for ADC module */
#define x_ADC_START_ONCE_BIT                  2  /* Start conversion bit */

#define x_CLOCK_BASE_ADDR                     (0x40008000) /* The base address for CLOCK registers */
#define x_CLOCK_ADC_OFFSET                    (0x60) 
#define x_CLOCK_ADC_REG                       (x_CLOCK_BASE_ADDR + x_CLOCK_ADC_OFFSET) /* Clock control register for ADC peripheral */
#define x_ENABLE_CLOCK_BIT                    11
#define x_CLK_USB_SOURCE                      0 
#define x_AUXSRC_CLK_USB_MASK                 0x7 << 5 /* Mask for selecting the USB clock source */

//* Functions definition
uint16_t x_read_ambient_light_once();
void x_adc_init(void);
uint8_t x_adc_set_channels(uint8_t channel_mask);
uint8_t x_adc_enable(void);

#endif /* __ADC_IF_H__ */