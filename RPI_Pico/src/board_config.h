#ifndef __BOARD_CONFIG_H
#define __BOARD_CONFIG_H

#define USE_PICO_WH 1 //Board type, pico wifi or not.
#define TASK_DELAY 1000

/* Pins declarations */
#define ENTRY_SENSOR 0  /* IR sensor for vehicle detection at the parking entry. */
#define EXIT_SENSOR 1 /* IR sensor for vehicle detection at the parking exit. */

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