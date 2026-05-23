#ifndef __BOARD_CONFIG_H
#define __BOARD_CONFIG_H

//* ---- MCU MODE ----
#define MCU_MODE_RUNNING 0
#define MCU_MODE_TEST 1
#define MCU_MODE MCU_MODE_RUNNING
//* ---- MCU MODE ----

#define USE_PICO_WH 0 //Board type, pico wifi or not.
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

/* Board test functions */
void blink_built_in_led(void);
void send_log_message(char* msg, uint32_t val);

/* Register access */
#define xHW_REG32(addr) (*(volatile uint32_t *)(addr))

/* Standard return types */
#define RET_OK 0
#define RET_NOK 1

/* Board hardware registers and addresses */
#define x_IO_BANK0_BASE                       (0x40014000) /* Base address for IO Bank 0 */

/* Reset register that enables the ADC peripheral */
#define x_RESET_REGISTER_BASE_ADDRESS         (0x4000C000)
#define x_RESET_DONE_OFFSET                   (0x08) /* Offset for the reset done register */
#define x_RESET_DONE_REG                      (x_RESET_REGISTER_BASE_ADDRESS + x_RESET_DONE_OFFSET) /* Reset done register */

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

#endif