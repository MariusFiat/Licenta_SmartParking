#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h" // Necesar pentru Pico WH

#define USE_PICO_WH 1 // Schimbă în 0 pentru Pico simplu

void led_task(void* params)
{   
    (void) params;

#if USE_PICO_WH
    //The build-in LED is controlled by the wifi chip on the Raspberry Pi Pico WH, 
    //[..] firstly I have to initialise that chip in order to be able to control it.
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        vTaskDelete(NULL);
    }
#else
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
#endif

    while (true) {
#if USE_PICO_WH
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
#else
        gpio_put(LED_PIN, 1);
#endif
        vTaskDelay(pdMS_TO_TICKS(500)); // 500ms

#if USE_PICO_WH
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
#else
        gpio_put(LED_PIN, 0);
#endif
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main()
{
    stdio_init_all();

    xTaskCreate(led_task, "LED_Task", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    while(1){};
}