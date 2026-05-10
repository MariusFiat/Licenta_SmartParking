#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h" // Raspberry PICO WH includes

#include "test_led.h"
#include "detect.h"
#include "board_config.h"
#include "shared_resources.h"
#include "uart_handler.h"
#include "servo.h"
#include "barrier_safety.h"
#include "lights_controller.h"
#include "brightness_module.h"
#include "mcal/adc_if.h"

/* Init queues */
QueueHandle_t xQueue_Entry_Req = NULL; /* Queue declaration, in the same way in detect.c and uart.c*/
QueueHandle_t xQueue_Exit_Req = NULL;
QueueHandle_t xQueue_Servo_Entry = NULL;
QueueHandle_t xQueue_Servo_Exit = NULL;
QueueHandle_t xQueue_Servo_Safety_Entry = NULL;
QueueHandle_t xQueue_Servo_Safety_Exit = NULL;

/* Init semaphores */
SemaphoreHandle_t xSemaphore_Entry_Res = NULL;
SemaphoreHandle_t xSemaphore_Exit_Res = NULL;
SemaphoreHandle_t xSemaphore_Entry_Timer = NULL;
SemaphoreHandle_t xSemaphore_Exit_Timer = NULL;
SemaphoreHandle_t xSemaphore_Barrier_Safety_Entry = NULL;
SemaphoreHandle_t xSemaphore_Barrier_Safety_Exit = NULL;

TimerHandle_t xTimer_Barrier_Entry = NULL;
TimerHandle_t xTimer_Barrier_Exit = NULL;

static void create_resources();
static void block_until_init_signal_was_received();

int main()
{
    init_board();

#if MCU_MODE == MCU_MODE_RUNNING
    create_resources();
    block_until_init_signal_was_received();

    xTaskCreate(barrier_safety_check, "BarriersSafety", 256, NULL, 1, NULL);

    //xTaskCreate(led_task, "LED_Task", 256, NULL, 1, NULL);
    xTaskCreate(detect_entry, "Detetect_Entry", 256, NULL, 1, NULL);
    xTaskCreate(detect_exit, "Detect_Exit", 256, NULL, 1, NULL);
    xTaskCreate(uart_handler, "UART_handler", 256, NULL, 1, NULL);
    xTaskCreate(servo_task, "Servo_Task", 256, NULL, 1, NULL);
    //xTaskCreate(lights_controller_task, "Lights_Controller", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    printf("%s", "FreeRTOS has run out of RAM memory!");
    while(1){};

#elif MCU_MODE == MCU_MODE_TEST

    uint8_t val = 0;
    x_adc_init();
    x_adc_set_channels(0x00);
    x_adc_enable();

    while(1){
        uint16_t measured_value = x_read_ambient_light_once();
        printf("Measured_val = %d\r\n", measured_value);

        val = !val;
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, val); 

        sleep_ms(1000);
    }
#endif
}

static void create_resources(){
    xQueue_Entry_Req = xQueueCreate(QUEUE_ENTRY_REQ_LENGTH, sizeof(bool));
    xQueue_Exit_Req = xQueueCreate(QUEUE_EXIT_REQ_LENGTH, sizeof(bool));
    xQueue_Servo_Entry = xQueueCreate(QUEUE_SERVO_LENGTH, sizeof(ServoMessage_t));
    xQueue_Servo_Exit = xQueueCreate(QUEUE_SERVO_LENGTH, sizeof(ServoMessage_t));
    xQueue_Servo_Safety_Entry = xQueueCreate(QUEUE_SERVO_LENGTH, sizeof(ServoMessage_t));
    xQueue_Servo_Safety_Exit = xQueueCreate(QUEUE_SERVO_LENGTH, sizeof(ServoMessage_t));

    xSemaphore_Entry_Res = xSemaphoreCreateBinary();
    xSemaphore_Exit_Res = xSemaphoreCreateBinary();
    xSemaphore_Entry_Timer = xSemaphoreCreateBinary();
    xSemaphore_Exit_Timer = xSemaphoreCreateBinary();
    xSemaphore_Barrier_Safety_Entry = xSemaphoreCreateBinary();
    xSemaphore_Barrier_Safety_Exit = xSemaphoreCreateBinary();
}

static void block_until_init_signal_was_received(){
    xSemaphoreTake(xSemaphore_Barrier_Safety_Entry, 0);
    xSemaphoreTake(xSemaphore_Barrier_Safety_Exit, 0);
    xSemaphoreTake(xSemaphore_Entry_Res, 0);
    xSemaphoreTake(xSemaphore_Exit_Res, 0);
}