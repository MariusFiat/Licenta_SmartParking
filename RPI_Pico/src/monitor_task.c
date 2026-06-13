#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

char stats_buffer[1024];

void monitor_task(void *pvParameters) {
    (void)pvParameters;

    while (true) {
        printf("\n==================================================\n");
        printf("          ANALIZA DE PERFORMANTA FREERTOS         \n");
        printf("==================================================\n");

        printf("\n--- Stack consumption (Remaining RAM) ---\n");
        printf("Task name\tState\tPriority\tRemaining\tNumber\n");
        vTaskList(stats_buffer);
        printf("%s\n", stats_buffer);

        printf("\n--- CPU Load (Execution Time) ---\n");
        printf("Task name\tAbsolute Time\tPercentage (%%)\n");
        vTaskGetRunTimeStats(stats_buffer);
        printf("%s\n", stats_buffer);
        
        printf("==================================================\n");

        /* Execute every 5 seconds */
        vTaskDelay(pdMS_TO_TICKS(5000)); 
    }
}