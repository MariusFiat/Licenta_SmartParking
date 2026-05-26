#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "board_config.h"
#include "shared_resources.h"
#include "barrier_safety.h"
#include "detect.h"
#include "lights_controller.h"

void checkEntry();
void checkExit();

void init_function(void){
    gpio_init(ENTRY_SAFETY_SENSOR);
    gpio_set_dir(ENTRY_SAFETY_SENSOR, GPIO_IN);
    gpio_init(EXIT_SAFETY_SENSOR);
    gpio_set_dir(EXIT_SAFETY_SENSOR, GPIO_IN);
}

void barrier_safety_check(void* pvParams){
    (void) pvParams;

    init_function();

    while(1){
        checkEntry();
        checkExit();
        vTaskDelay(pdMS_TO_TICKS(TASK_DELAY)); 
    }
}

void checkEntry() {
    if (xSemaphoreTake(xSemaphore_Barrier_Safety_Entry, 0) == pdTRUE) {
        
        bool is_car_still_there = !gpio_get(ENTRY_SENSOR) || !gpio_get(ENTRY_SAFETY_SENSOR);
        static uint8_t counter = 0;

        if (is_car_still_there) {
            /* The car is still there  */
            counter = 0;

            xSemaphoreGive(xSemaphore_Barrier_Safety_Entry);
        } else {
            counter++;
            
            if(counter >= 4){
                send_log_message("Send the closing message to the entry barrier!", 0);

                counter = 0;
                ServoMessage_t closeMsg = {true, false};
                xQueueSend(xQueue_Servo_Safety_Entry, &closeMsg, 0);

                xQueueReset(xQueue_Entry_Req);
                send_command_to_detection_zone_leds(OFF); 
                set_detectEntryState(IDLE);         
                
                xSemaphoreGive(xSemaphore_Entry_Res); 
            } else {
                xSemaphoreGive(xSemaphore_Barrier_Safety_Entry);
            }
        }
    }
}

void checkExit(){
    if(xSemaphoreTake(xSemaphore_Barrier_Safety_Exit, 0) == pdTRUE){
        bool is_car_still_there = !gpio_get(EXIT_SENSOR) || !gpio_get(EXIT_SAFETY_SENSOR);
        static uint8_t counter = 0;
        
        if(is_car_still_there){
            /* The car is still there. */
            counter = 0;

            xSemaphoreGive(xSemaphore_Barrier_Safety_Exit);
        }else{
            counter++;

            if(counter >= 4){
                counter = 0;

                send_log_message("Send the closing message to the exit barrier!", 0);

                ServoMessage_t closeMsg = {true, false};
                
                xQueueSend(xQueue_Servo_Safety_Exit, &closeMsg, 0);
                xQueueReset(xQueue_Exit_Req);
                
                send_command_to_detection_zone_leds(OFF); 
                set_detectExitState(IDLE);              
                
                xSemaphoreGive(xSemaphore_Exit_Res);
            } else {
                xSemaphoreGive(xSemaphore_Barrier_Safety_Exit);
            }
        }
    }
}