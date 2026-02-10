#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "board_config.h"
#include "shared_resources.h"
#include "uart_handler.h"

static void checkEntryRequest();
static void checkExitRequest();
static bool read_user_data(char*);


void uart_handler(void* pvParams){
    (void) pvParams;

    while(1){
        printf("%s\n", "The uart task is running!");
        checkEntryRequest();
        checkExitRequest();

        vTaskDelay(pdMS_TO_TICKS(TASK_DELAY));
    }
}

static void checkEntryRequest(){

    static bool sensorStateReceived = false;
    static bool lastEntryRead = false;
    static int received_Response;
    static bool readEntryResponseDone = false;

    static char command_Entry[COMMAND_LENGTH + 1];
    static int i = 0;

        if(lastEntryRead == true){
            /* Send signal to the python scrip */

            /* If the response is received, then, open/not the barrier*/
            taskENTER_CRITICAL();
            readEntryResponseDone = read_user_data(command_Entry);
            taskEXIT_CRITICAL();
            
            if(readEntryResponseDone){
                if(strstr(command_Entry, "ENA") != NULL){
                    printf("Entry access allowed!\n");
                    
                    /* Write in the Servo_Queue. Send the open command for the entry. */

                }else{
                    printf("Entry access denied!\n");
                    /* The barrier remain closed. The detect_entry task will be resumed. */
                }

                lastEntryRead = false;
                xSemaphoreGive(xSemaphore_Entry_Res); /* Wake-up the detect entry task. */
                vTaskDelay(pdMS_TO_TICKS(100));
            }
        }
        else if(lastEntryRead == false){ /* If there was not detected any obstacle, than run normally. */
            
            xQueueReceive(xQueue_Entry_Req, &sensorStateReceived, 0); /* Read the state of the entry sensor. */

            if(sensorStateReceived == false){
                printf("Received in the uart_handler task from entry -> %d\n", sensorStateReceived);

            } else if(sensorStateReceived == true){
                printf("%s\n", "The detect entry task is blocked");
                lastEntryRead = true;
                xSemaphoreTake(xSemaphore_Entry_Res, 0); /* Call the semaphore to block the detect_entry task. Now, when the detect_entry task will call SemaporeTake, it will be blocked. */

                /* Send the command on uart to the RPI5. The RPI5 will start the car plate detection stage and will replay with the result. */

            }
        }
}

static void checkExitRequest(){
    static bool sensorStateReceived = false;
    static bool lastExitRead = false;
    static bool readExitResponseDone = false;

    static char command_Exit[COMMAND_LENGTH + 1];

    if(lastExitRead == true){
        /* Send signal to the python script */

        /* If the response is received, then, open/not the exit barrier */
        taskENTER_CRITICAL();
        readExitResponseDone = read_user_data(command_Exit);
        taskEXIT_CRITICAL();

        if(readExitResponseDone){
            if(strstr(command_Exit, "EXA") != NULL){
                printf("Exit access allowed!\n");

                /* Write in the Servo_Queue */
            }
            else{
                printf("Exit access denied!\n");
            }

            lastExitRead = false;
            xSemaphoreGive(xSemaphore_Exit_Res);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    } else if(lastExitRead == false){
        xQueueReceive(xQueue_Exit_Req, &sensorStateReceived, 0);

        if(sensorStateReceived == false){
            printf("Received in the uart_handler task from exit -> %d\n", sensorStateReceived);
        }
        else if(sensorStateReceived == true){
            printf("The detect exit task is blocked!\n");
            lastExitRead = true;
            xSemaphoreTake(xSemaphore_Exit_Res, 0);

            /* Send cmd to the RPI5 for processing the exit. */
        }else{
            /* Do nothing */
        }
    }
}

static bool read_user_data(char *out_buffer) {
    static char internal_buffer[COMMAND_LENGTH + 1];
    static int current_idx = 0;

    int c = getchar_timeout_us(0);

    if (c != PICO_ERROR_TIMEOUT) {
        if (c >= 32 && c <= 126) { 
            internal_buffer[current_idx++] = (char)c;
        }

        if (current_idx == COMMAND_LENGTH) {
            internal_buffer[COMMAND_LENGTH] = '\0';
            
            for(int i = 0; i <= COMMAND_LENGTH; i++) {
                out_buffer[i] = internal_buffer[i];
            }

            current_idx = 0;
            printf("\n[System] RPI5 send: %s\n", out_buffer);
            return true; 
        }
    }

    return false;
}