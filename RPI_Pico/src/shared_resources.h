#ifndef __QUEUES_HEADER_H
#define __QUEUES_HEADER_H

#include "../lib/FreeRTOS-Kernel/include/queue.h"
#include "../lib/FreeRTOS-Kernel/include/semphr.h"
#include "../lib/FreeRTOS-Kernel/include/timers.h"

#define QUEUE_ENTRY_REQ_LENGTH 1
#define QUEUE_EXIT_REQ_LENGTH 1


typedef struct{
    bool state;
    bool dir;
}ServoMessage_t;

#define QUEUE_SERVO_LENGTH 1

/* Queues declarations */
extern QueueHandle_t xQueue_Entry_Req;
extern QueueHandle_t xQueue_Exit_Req;
extern QueueHandle_t xQueue_Servo_Entry;
extern QueueHandle_t xQueue_Servo_Exit;
extern QueueHandle_t xQueue_Servo_Safety_Entry;
extern QueueHandle_t xQueue_Servo_Safety_Exit;

/* Synchronization variables */
extern SemaphoreHandle_t xSemaphore_Entry_Res;
extern SemaphoreHandle_t xSemaphore_Exit_Res;
extern SemaphoreHandle_t xSemaphore_Entry_Timer;
extern SemaphoreHandle_t xSemaphore_Exit_Timer;
extern SemaphoreHandle_t xSemaphore_Barrier_Safety_Entry;
extern SemaphoreHandle_t xSemaphore_Barrier_Safety_Exit;

/* Timers */
extern TimerHandle_t xTimer_Barrier_Entry;
extern TimerHandle_t xTimer_Barrier_Exit;

#endif