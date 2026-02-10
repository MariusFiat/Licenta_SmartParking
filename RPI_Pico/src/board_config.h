#ifndef __BOARD_CONFIG_H
#define __BOARD_CONFIG_H

#define USE_PICO_WH 1 //Board type, pico wifi or not.
#define TASK_DELAY 50

/* Pins declarations */
#define ENTRY_SENSOR 0  /* IR sensor for vehicle detection at the parking entry. */
#define EXIT_SENSOR 1 /* IR sensor for vehicle detection at the parking exit. */

/*Board hardware init functions*/
void init_board();

#endif