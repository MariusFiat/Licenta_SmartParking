#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"  // Obligatoriu pentru vTaskDelay
#include "task.h"      // Obligatoriu pentru vTaskDelay
#include "board_config.h"
#include "detect.h"

void detect_entry(void* params){
    (void) params;
    
    vTaskDelay(pdMS_TO_TICKS(3000));
    
    gpio_init(ENTRY_SENSOR);
    gpio_set_dir(ENTRY_SENSOR, GPIO_IN);
    printf("%s\n", "Run the detect method for entry!\n");

    while(true) {
        bool object_detected = !gpio_get(ENTRY_SENSOR); 

        printf("%s - %d\n", "Detect entry! -> ", object_detected);

        if(object_detected){
            #if USE_PICO_WH == 1
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            #endif
            // Here I will send some data to the pico - rpi5 communication task. Those data will be written in the specific queue.

        } else {
            #if USE_PICO_WH == 1
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            #endif
            /* Do nothing. */
        }

        vTaskDelay(pdMS_TO_TICKS(50)); 
    }
}

void detect_exit(void* params){
    (void) params;

    gpio_init(EXIT_SENSOR);
    gpio_set_dir(EXIT_SENSOR, GPIO_IN);
    printf("%s\n", "Run the detect method for exit!\n");

    while(true){
        bool object_detected = !gpio_get(EXIT_SENSOR);

        printf("%s - %d\n", "Detect exit! -> ", object_detected);

        if(object_detected){
            //Save the signal in the communication Queue.

        }
        else{
            /* Do nothing. */
        }
        
        vTaskDelay(pdMS_TO_TICKS(50)); 
    }
}