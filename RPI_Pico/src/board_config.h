#ifndef __BOARD_CONFIG_H
#define __BOARD_CONFIG_H

#define USE_PICO_WH 1 //Board type, pico wifi or not.
#define TASK_DELAY 10

/* Pins declarations */
#define ENTRY_SENSOR 0  /* IR sensor for vehicle detection at the parking entry. */
#define EXIT_SENSOR 1 /* IR sensor for vehicle detection at the parking exit. */

#define ENTRY_SAFETY_SENSOR 2
#define EXIT_SAFETY_SENSOR 3

#define PWM_BARRIER_ENTRY 16 
#define PWM_BARRIER_EXIT 17

#define I2C_PORT i2c1
#define I2C_SDA_LIGHTS 14
#define I2C_SCL_LIGHTS 15
#define PCF_0_ADDR 0x38
#define PCF_1_ADDR 0x39

/* Messages lengths and format. 
    ENAXXX = Entry allowed slot number XXX
    END000 = Entry denied
    EXA000 = Exit allowed 
    EXD000 = Exit denied (Maybe you forgot to pay your slot.)
*/
#define COMMAND_LENGTH 6

/*Board hardware init functions*/
void init_board();

/* Board hardware registers and addresses */
#define x_IO_BANK0_BASE                       (0x40014000) /* Base address for IO Bank 0 */

/* Reset register that enables the ADC peripheral */
#define x_RESET_REGISTER_BASE_ADDRESS         (0x4000C000)
#define x_RESET_DONE_OFFSET                   (0x08) /* Offset for the reset done register */
#define x_RESET_DONE_REG                      (x_RESET_REGISTER_BASE_ADDRESS + x_RESET_DONE_OFFSET) /* Reset done register */
#define x_ADC_RESET_BIT                       (0x0) /* Bit to reset the ADC peripheral */

#define x_GPIO26_STATUS_REG_OFFSET            (0x0D0) /* Status register offset for GPIO26 */
#define x_GPIO26_CTRL_REG_OFFSET              (0x0D4) /* Control register offset for GPIO26 */

#define x_GPIO27_STATUS_REG_OFFSET            (0x0D8) /* Status register offset for GPIO27 */
#define x_GPIO27_CTRL_REG_OFFSET              (0x0DC) /* Control register offset for GPIO27 */

#define x_PADS_BANK0_BASE                     (0x4001C000) /* Base address for Pads Bank 0 */
#define x_GPIO26_PAD_CTRL_OFFSET              (0x6C) 
#define x_GPIO27_PAD_CTRL_OFFSET              (0x70) 
#define x_GPIO26_PAD_CTRL_REG                 (x_PADS_BANK0_BASE + x_GPIO26_PAD_CTRL_OFFSET)  /* Pad control register offset for GPIO26 */
#define x_GPIO27_PAD_CTRL_REG                 (x_PADS_BANK0_BASE + x_GPIO27_PAD_CTRL_OFFSET)  /* Pad control register offset for GPIO27 */
#define x_IE_BIT                              6 
#define x_OD_BIT                              7

/* ADC registers and bits mapping */
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
#endif