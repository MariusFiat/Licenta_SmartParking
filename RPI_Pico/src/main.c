#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h" // Raspberry PICO WH includes

#include "test_led.h"
#include "detect.h"
#include "board_config.h"
#include "queues_header.h"
#include "uart_handler.c"

#define QUEUE_ENTRY_REQ_LENGTH 2

QueueHandle_t xQueue_Entry_Req = NULL; /* Queue declaration, in the same way in detect.c and uart.c*/

int main()
{
    init_board();

    xQueue_Entry_Req = xQueueCreate(QUEUE_ENTRY_REQ_LENGTH, sizeof(bool));

    //xTaskCreate(led_task, "LED_Task", 256, NULL, 1, NULL);
    xTaskCreate(detect_entry, "Detetect_Entry", 256, NULL, 1, NULL);
    xTaskCreate(detect_exit, "Detect_Exit", 256, NULL, 1, NULL);
    xTaskCreate(uart_handler, "UART_handler", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    printf("%s", "FreeRTOS has run out of RAM memory!");
    while(1){};
}