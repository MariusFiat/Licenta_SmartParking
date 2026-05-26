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

        if (is_car_still_there) {
            /* Do nothing */

            xSemaphoreGive(xSemaphore_Barrier_Safety_Entry);
        } else {
            ServoMessage_t closeMsg = {true, false};
            xQueueSend(xQueue_Servo_Safety_Entry, &closeMsg, 0);

            xQueueReset(xQueue_Entry_Req);
            send_command_to_detection_zone_leds(OFF); /* Turn OFF the detection zone leds. */
            set_detectEntryState(IDLE);         /* Open detection on entry side. */
            xSemaphoreGive(xSemaphore_Entry_Res);
        }
    }
}

void checkExit(){
    if(xSemaphoreTake(xSemaphore_Barrier_Safety_Exit, 0) == pdTRUE){
         bool is_car_still_there = !gpio_get(EXIT_SENSOR) || !gpio_get(EXIT_SAFETY_SENSOR);

        if(is_car_still_there){
            /* The car is still in the barrier area. Do nothing. */
            
            xSemaphoreGive(xSemaphore_Barrier_Safety_Exit);
        }else{
            ServoMessage_t closeMsg = {true, false};
            xQueueReset(xQueue_Exit_Req);
            xQueueSend(xQueue_Servo_Safety_Exit, &closeMsg, 0);
            
            xQueueReset(xQueue_Exit_Req);
            send_command_to_detection_zone_leds(OFF); /* Turn on the detection zone leds. */
            set_detectExitState(IDLE);              /* Open the detection on exit side */
            xSemaphoreGive(xSemaphore_Exit_Res);
        }
    }

}