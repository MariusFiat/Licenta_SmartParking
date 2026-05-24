#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"  // Obligatoriu pentru vTaskDelay
#include "task.h"      // Obligatoriu pentru vTaskDelay

#include "board_config.h"
#include "detect.h"
#include "shared_resources.h"

/* System states between detection and barrier_safety tasks.*/
static Detect_State_t detectEntryState = IDLE;
static Detect_State_t detectExitState = IDLE;

uint8_t get_detectEntryState(void){
    return detectEntryState;
}

void set_detectEntryState(uint8_t state){
    detectEntryState = state;
}

uint8_t get_detectExitState(void){
    return detectExitState;
}

void set_detectExitState(uint8_t state){
    detectExitState = state;
}

void detect_entry(void* params){
    (void) params;
    
    gpio_init(ENTRY_SENSOR);
    gpio_set_dir(ENTRY_SENSOR, GPIO_IN);

    while(true) {
        if(xSemaphoreTake(xSemaphore_Entry_Res, 0) == pdFALSE){  /* This semaphore is used to wait the init signal and for features blocking in the future mentenance mode */
            vTaskDelay(pdMS_TO_TICKS(TASK_DELAY)); 
            continue;
        }

        bool object_detected = !gpio_get(ENTRY_SENSOR); 

        if(object_detected && (detectEntryState == IDLE)){
            detectEntryState = ACTIVE;

            #if USE_PICO_WH == 1
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            #endif

            /* Send the entry request to the UART_Handler task. */
            xQueueSend(xQueue_Entry_Req, &object_detected, 0);
            vTaskDelay(pdMS_TO_TICKS(500));

        } else {
            #if USE_PICO_WH == 1
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            #endif
        }

        xSemaphoreGive(xSemaphore_Entry_Res);
        
        vTaskDelay(pdMS_TO_TICKS(TASK_DELAY)); 
    }
}

void detect_exit(void* params){
    (void) params;

    gpio_init(EXIT_SENSOR);
    gpio_set_dir(EXIT_SENSOR, GPIO_IN);
    //printf("%s\n", "Run the detect method for exit!\n");

    while(true){
        if(xSemaphoreTake(xSemaphore_Exit_Res, 0) == pdFALSE){ /* This semaphore is used to wait the init signal and for features blocking in the future mentenance mode */
            vTaskDelay(pdMS_TO_TICKS(TASK_DELAY)); 
            continue;
        }

        bool object_detected = !gpio_get(EXIT_SENSOR);

        if(object_detected && (detectExitState == IDLE)){  /* Check if the detection is in IDLE or not */
            detectExitState = ACTIVE;
            //Save the signal in the communication Queue.
            xQueueSend(xQueue_Exit_Req, &object_detected, 0);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        else{
            /* Do nothing. */
        }
        
        xSemaphoreGive(xSemaphore_Exit_Res);

        vTaskDelay(pdMS_TO_TICKS(TASK_DELAY)); 
    }
}