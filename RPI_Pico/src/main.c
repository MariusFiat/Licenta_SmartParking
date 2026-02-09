#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h" // Raspberry PICO WH includes

#include "test_led.h"
#include "detect.h"
#include "board_config.h"

int main()
{
    init_board();

    //xTaskCreate(led_task, "LED_Task", 256, NULL, 1, NULL);
    xTaskCreate(detect_entry, "Detetect_Entry", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    while(1){};
}