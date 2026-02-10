#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "board_config.h"
#include "shared_resources.h"
#include "uart_handler.h"

void uart_handler(void* pvParams){
    (void) pvParams;

    bool sensorStateReceived = false;
    static bool lastEntryRead = false;

    int received_Response;

    while(1){
        received_Response = getchar_timeout_us(0);

        if(lastEntryRead == true && received_Response != PICO_ERROR_TIMEOUT){
            while(getchar_timeout_us(0) != PICO_ERROR_TIMEOUT); /* Clear the buffer. Delete the unused characters like '\n'*/
            /* Send signal to the python scrip */

            /* If the response is received, than, open/not the barrier*/
            
            lastEntryRead = false;
            xSemaphoreGive(xSemaphore_Entry_Res); /* Wake-up the detect entry task. */
        }
        else if(lastEntryRead == false){ /* If there was not detected any obstacle, than run normally. */
            
            xQueueReceive(xQueue_Entry_Req, &sensorStateReceived, 0);

            if(sensorStateReceived == false){
                printf("Received in the uart_handler task -> %d\n", sensorStateReceived);

            } else if(sensorStateReceived == true){
                printf("%s\n", "The detect entry task is blocked");
                lastEntryRead = true;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(TASK_DELAY));
    }
}