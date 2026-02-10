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

static bool sensorStateReceived = false;
static bool lastEntryRead = false;
static int received_Response;
static bool readEntryResponseDone = false;

static char command_Entry[COMMAND_LENGTH + 1];
static int i = 0;

void uart_handler(void* pvParams){
    (void) pvParams;

    while(1){
        //printf("%s\n", "The uart task is running!");
        checkEntryRequest();

        vTaskDelay(pdMS_TO_TICKS(TASK_DELAY));
    }
}

static void checkEntryRequest(){

        if(lastEntryRead == true){
            /* Send signal to the python scrip */

            /* If the response is received, than, open/not the barrier*/
            readEntryResponseDone = read_user_data(command_Entry);
            
            if(readEntryResponseDone){
                if(strstr(command_Entry, "EA") != NULL){
                        
                    printf("Entry access allowed!\n");
                }else{
                    printf("Entry access denied!\n");
                }

                lastEntryRead = false;
                xSemaphoreGive(xSemaphore_Entry_Res); /* Wake-up the detect entry task. */
                vTaskDelay(pdMS_TO_TICKS(100));
            }
        }
        else if(lastEntryRead == false){ /* If there was not detected any obstacle, than run normally. */
            
            xQueueReceive(xQueue_Entry_Req, &sensorStateReceived, 0);

            if(sensorStateReceived == false){
                printf("Received in the uart_handler task -> %d\n", sensorStateReceived);

            } else if(sensorStateReceived == true){
                printf("%s\n", "The detect entry task is blocked");
                lastEntryRead = true;
                xSemaphoreTake(xSemaphore_Entry_Res, 0); /* Call the semaphore to block the detect_entry task. Now, when the detect_entry task will call SemaporeTake, it will be blocked. */
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