#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "board_config.h"
#include "queues_header.h"
#include "uart_handler.h"

void uart_handler(void* pvParams){
    (void) pvParams;

    bool sensorStateReceived = false;

    while(1){
        xQueueReceive(xQueue_Entry_Req, &sensorStateReceived, 0);

        if(sensorStateReceived == true || sensorStateReceived == false){
            printf("Received in the uart_handler task -> %d\n", sensorStateReceived);
        }

        vTaskDelay(pdMS_TO_TICKS(TASK_DELAY));
    }
}