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

#endif