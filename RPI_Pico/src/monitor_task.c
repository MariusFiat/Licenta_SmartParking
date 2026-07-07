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

        /* 1. Statistics about Heap Memory */
        printf("\n--- Heap Memory Statistics ---\n");
        printf("Current available Heap memory: %u bytes\n", xPortGetFreeHeapSize());
        
        /* Note: xPortGetMinimumEverFreeHeapSize works with heap_4.c and heap_5.c */
        printf("Minimum ever available Heap memory:   %u bytes\n", xPortGetMinimumEverFreeHeapSize());
        
        
        printf("High Water Mark (Monitor Task):  %lu words\n", uxTaskGetStackHighWaterMark(NULL));

        /* 2. Lista task-urilor și consumul de stivă (High Water Mark global) */
        printf("\n--- Task Status and Stack Usage ---\n");
        printf("Task Name\tState\tPriority\tHigh Water\tCount\n");
        
        vTaskList(stats_buffer);
        printf("%s", stats_buffer);

        /* 3. Cpu Load */
        printf("\n--- Cpu Load (Run Time Stats) ---\n");
        printf("Task Name\tAbsolute Time\tPercentage (%%)\n");
        
        vTaskGetRunTimeStats(stats_buffer);
        printf("%s", stats_buffer);
        
        printf("==================================================\n");

        /* Executa o data la 5 secunde */
        vTaskDelay(pdMS_TO_TICKS(5000)); 
    }
}