#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"  // Obligatoriu pentru vTaskDelay
#include "task.h"      // Obligatoriu pentru vTaskDelay

#include "board_config.h"
#include "detect.h"
#include "shared_resources.h"

void detect_entry(void* params){
    (void) params;
    
    vTaskDelay(pdMS_TO_TICKS(3000)); //This init time must be moved in the init task. 
    
    gpio_init(ENTRY_SENSOR);
    gpio_set_dir(ENTRY_SENSOR, GPIO_IN);
    //printf("%s\n", "Run the detect method for entry!\n");

    while(true) {
        bool object_detected = !gpio_get(ENTRY_SENSOR); 

        //printf("%s - %d\n", "Detect entry! -> ", object_detected);

        if(object_detected){
            #if USE_PICO_WH == 1
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            #endif
            // Here I will send some data to the pico - rpi5 communication task. Those data will be written in the specific queue.

            /* 
                Semaphore that blocks the task in this point until the barrier is opend and than closed. In this way, I will send just a signal per car. 
                This semaphore must be shared with the UART task. When a car was detected, the message is pushed into the UART_Req_Queue and the semaphore is decremented.
                When the detection phase was done and the car access was allowed or denied, the UART task will increment this semaphore and this Detect_Task will be able to run again.

            */
            xQueueSend(xQueue_Entry_Req, &object_detected, 0);
            xSemaphoreTake(xSemaphore_Entry_Res, portMAX_DELAY); /* Block and wait the signal from the uart handler task. */

            /* Delay to simulate that the barrier waits */
            vTaskDelay(pdMS_TO_TICKS(3000)); /* Give 3 second before barrier close. */

            ServoMessage_t closeMessage = {true, false};
            xQueueSend(xQueue_Servo_Entry, &closeMessage, 0);

        } else {
            #if USE_PICO_WH == 1
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            #endif
            /* Do nothing. */
            xQueueSend(xQueue_Entry_Req, &object_detected, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(TASK_DELAY)); 
    }
}

void detect_exit(void* params){
    (void) params;

    gpio_init(EXIT_SENSOR);
    gpio_set_dir(EXIT_SENSOR, GPIO_IN);
    //printf("%s\n", "Run the detect method for exit!\n");

    while(true){
        bool object_detected = !gpio_get(EXIT_SENSOR);

        //printf("%s - %d\n", "Detect exit! -> ", object_detected);

        if(object_detected){
            //Save the signal in the communication Queue.
            xQueueSend(xQueue_Exit_Req, &object_detected, 0);
            xSemaphoreTake(xSemaphore_Exit_Res, portMAX_DELAY);

            /* Delay to simulate that the barrier waits */
            vTaskDelay(pdMS_TO_TICKS(3000)); /* Give 3 second before barrier close. */

            ServoMessage_t closeMessage = {true, false};
            xQueueSend(xQueue_Servo_Exit, &closeMessage, 0);
        }
        else{
            /* Do nothing. */
            xQueueSend(xQueue_Exit_Req, &object_detected, 0);
        }
        
        vTaskDelay(pdMS_TO_TICKS(TASK_DELAY)); 
    }
}