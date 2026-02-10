#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h" // Raspberry PICO WH includes

#include "test_led.h"
#include "detect.h"
#include "board_config.h"
#include "shared_resources.h"
#include "uart_handler.c"

/* Init queues */
QueueHandle_t xQueue_Entry_Req = NULL; /* Queue declaration, in the same way in detect.c and uart.c*/
QueueHandle_t xQueue_Exit_Req = NULL;

/* Init semaphores */
SemaphoreHandle_t xSemaphore_Entry_Res = NULL;
SemaphoreHandle_t xSemaphore_Exit_Res = NULL;

int main()
{
    init_board();

    xQueue_Entry_Req = xQueueCreate(QUEUE_ENTRY_REQ_LENGTH, sizeof(bool));
    xQueue_Exit_Req = xQueueCreate(QUEUE_EXIT_REQ_LENGTH, sizeof(bool));

    xSemaphore_Entry_Res = xSemaphoreCreateBinary();
    xSemaphore_Exit_Res = xSemaphoreCreateBinary();

    //xTaskCreate(led_task, "LED_Task", 256, NULL, 1, NULL);
    xTaskCreate(detect_entry, "Detetect_Entry", 256, NULL, 1, NULL);
    xTaskCreate(detect_exit, "Detect_Exit", 256, NULL, 1, NULL);
    xTaskCreate(uart_handler, "UART_handler", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    printf("%s", "FreeRTOS has run out of RAM memory!");
    while(1){};
}