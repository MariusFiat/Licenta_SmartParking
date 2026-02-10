#ifndef __QUEUES_HEADER_H
#define __QUEUES_HEADER_H

#include "../lib/FreeRTOS-Kernel/include/queue.h"
#include "../lib/FreeRTOS-Kernel/include/semphr.h"

#define QUEUE_ENTRY_REQ_LENGTH 1

/* Queues declarations */
extern QueueHandle_t xQueue_Entry_Req;

/* Synchronization variables */
extern SemaphoreHandle_t xSemaphore_Entry_Res;

#endif