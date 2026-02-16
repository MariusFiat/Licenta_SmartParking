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
static void route_message_from_rpi5();

static bool readEntryResponseDone = false;
static char command_Entry[COMMAND_LENGTH + 1];
static bool readExitResponseDone = false;
static char command_Exit[COMMAND_LENGTH + 1];
static ServoMessage_t entryServo = {false, false};
static ServoMessage_t exitServo = {false, false};

void uart_handler(void* pvParams){
    (void) pvParams;

    while(1){
        //printf("%s\n", "The uart task is running!");
        checkEntryRequest();
        checkExitRequest();
        route_message_from_rpi5();

        vTaskDelay(pdMS_TO_TICKS(TASK_DELAY));
    }
}

static void checkEntryRequest(){

    static bool sensorStateReceived = false;
    static bool lastEntryRead = false;
    static int received_Response;
    static bool requestSent = false;

    static int i = 0;

        if(lastEntryRead == true){
            /* Send signal to the python scrip */

            /* If the response is received, then, open/not the barrier*/
            
            if(readEntryResponseDone){
                if(strstr(command_Entry, "ENA") != NULL){
                    //printf("Entry access allowed!\n");
                    
                    /* Write in the Servo_Queue. Send the open command for the entry. */
                    entryServo.state = true;
                    entryServo.dir = true;
                    xQueueSend(xQueue_Servo_Entry, &entryServo, 0);

                    /* Unlock the safety task semaphore for entry*/
                    xSemaphoreGive(xSemaphore_Barrier_Safety_Entry);

                }else{
                    /* The barrier remains closed.*/
                    xSemaphoreGive(xSemaphore_Entry_Res); /* Wake-up the detect entry task. */
                }
                command_Entry[0] = 0;
                readEntryResponseDone = false;

                requestSent = false;
                lastEntryRead = false;
                xQueueReset(xQueue_Entry_Req);
                vTaskDelay(pdMS_TO_TICKS(100));
            }
        }
        else if(lastEntryRead == false){ /* If there was not detected any obstacle, than run normally. */
            
            if(xQueueReceive(xQueue_Entry_Req, &sensorStateReceived, 0) == pdTRUE){ /* Read the entry sensor state, check if there are any detections in the queue. If not, do nothing*/

                /* Read the state of the entry sensor. */
                if(sensorStateReceived == true && requestSent == false){
                    //printf("%s\n", "The detect entry task is blocked");
                    lastEntryRead = true;
                    requestSent = true;
                    xSemaphoreTake(xSemaphore_Entry_Res, 0); /* Call the semaphore to block the detect_entry task. Now, when the detect_entry task will call SemaporeTake, it will be blocked. */

                    /* Send the command on uart to the RPI5. The RPI5 will start the car plate detection stage and will replay with the result. */
                    printf("A");
                }else{
                    /* Do nothing. */
                }
            }
        }
}

static void checkExitRequest(){
    static bool sensorStateReceived = false;
    static bool lastExitRead = false;
    static bool requestSent = false;

    if(lastExitRead == true){
        /* Send signal to the python script */

        /* If the response is received, then, open/not the exit barrier */

        if(readExitResponseDone){
            if(strstr(command_Exit, "EXA") != NULL){
                //printf("Exit access allowed!\n");

                /* Write in the Servo_Queue */
                exitServo.state = true;
                exitServo.dir = true;
                xQueueSend(xQueue_Servo_Exit, &exitServo, 0);

                /* Release the semaphore for the barrier safety task. I want to be able to check if the car is still in the barrier's area. */
                xSemaphoreGive(xSemaphore_Barrier_Safety_Exit);
            }
            else{
                //printf("Exit access denied!\n");
                /* The barrier remains closed. */
                xSemaphoreGive(xSemaphore_Exit_Res); /* Wake-up the detect exit task. */
            }
            command_Exit[0] = 0;
            readExitResponseDone = false;

            requestSent = false;
            lastExitRead = false;
            xQueueReset(xQueue_Exit_Req);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    } else if(lastExitRead == false){
        if(xQueueReceive(xQueue_Exit_Req, &sensorStateReceived, 0) == pdTRUE){
            /* Check if there are any sensors activated in the queue for exit side.*/

            if(sensorStateReceived == true && requestSent == false){
                lastExitRead = true;
                requestSent = true;
                xSemaphoreTake(xSemaphore_Exit_Res, 0);

                /* Send cmd to the RPI5 for processing the exit. */
                printf("B");
            }else{
                /* Do nothing */
            }
        }
    }
}

static bool read_user_data(char* out_buffer) {
    static char internal_buffer[COMMAND_LENGTH + 1];
    static int current_idx = 0;

    int c = getchar_timeout_us(0);

    while (c != PICO_ERROR_TIMEOUT) {
        if (c == '\r' || c == '\n') {
            if (current_idx > 0) {
                internal_buffer[current_idx] = '\0';
                strcpy(out_buffer, internal_buffer);
                current_idx = 0;
                return true; 
            }
        }
        else if (c >= 32 && c <= 126 && current_idx < COMMAND_LENGTH) {
            internal_buffer[current_idx++] = (char)c;
        }
        
        c = getchar_timeout_us(50);
    }
    return false;
}

static void route_message_from_rpi5(){
    static char command[COMMAND_LENGTH + 1];
    static bool readStatus = false;
    
        readStatus = read_user_data(command);

    if(readStatus){
        if(strstr(command, "EN")){
            strcpy(command_Entry, command);
            readEntryResponseDone = readStatus;
        }
        else if(strstr(command, "EX")){
            strcpy(command_Exit, command);
            readExitResponseDone = readStatus;
        }
    }
    memset(command, 0, sizeof(command));
}