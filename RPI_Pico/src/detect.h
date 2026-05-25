#ifndef __DETECT_H
#define __DETECT_H

typedef enum{
    IDLE = 0,
    ACTIVE,
}Detect_State_t;

void detect_entry(void* params);
void detect_exit(void* params);

uint8_t get_detectEntryState(void);
uint8_t get_detectExitState(void);
void set_detectEntryState(uint8_t state);
void set_detectExitState(uint8_t state);

#endif