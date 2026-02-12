#ifndef __BOARD_CONFIG_H
#define __BOARD_CONFIG_H

#define USE_PICO_WH 1 //Board type, pico wifi or not.
#define TASK_DELAY 10

/* Pins declarations */
#define ENTRY_SENSOR 0  /* IR sensor for vehicle detection at the parking entry. */
#define EXIT_SENSOR 1 /* IR sensor for vehicle detection at the parking exit. */

#define PWM_BARRIER_ENTRY 16 
#define PWM_BARRIER_EXIT 17

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