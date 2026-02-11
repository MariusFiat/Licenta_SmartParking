#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"      
#include "hardware/pwm.h"

#include "board_config.h"
#include "detect.h"
#include "shared_resources.h"

#define SERVO_STEP 5
#define SERVO_MIN 1000
#define SERVO_MAX 2000

#define BARRIER_SPEED 5

static void setMillis(int servoPin, float millis);
static void setServo(int servoPin1, int servoPin2, float startMillis);

static void setMillis(int servoPin, float millis){
    pwm_set_gpio_level(servoPin, (millis/20000.f)*39062.f);
}

static void setServo(int servoPin1, int servoPin2, float startMillis){
    gpio_set_function(servoPin1, GPIO_FUNC_PWM);
    gpio_set_function(servoPin2, GPIO_FUNC_PWM);

    uint slice_entry = pwm_gpio_to_slice_num(servoPin1);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 64.f);
    pwm_config_set_wrap(&config, 39062.f);
    pwm_init(slice_entry, &config, true);

    uint slice_exit = pwm_gpio_to_slice_num(servoPin2);
    if(slice_exit != slice_entry) {
        pwm_init(slice_exit, &config, true);
    }

    // Poziția inițială (Bariere jos)
    setMillis(servoPin1, SERVO_MIN);
    setMillis(servoPin2, SERVO_MIN);
}

// Barrier's states
typedef enum {
    BARRIER_IDLE = 0,
    BARRIER_MOVING
} BarrierState_t;

static BarrierState_t entryState = BARRIER_IDLE;
static int currentEntryPos = SERVO_MIN;
static bool dirEntry;

static BarrierState_t exitState = BARRIER_IDLE;
static int currentExitPos = SERVO_MIN;
static bool dirExit;
static ServoMessage_t entryServo = {false, false};
static ServoMessage_t exitServo = {false, false};

void servo_task(void* pvParams) {
    (void) pvParams;
    setServo(PWM_BARRIER_ENTRY, PWM_BARRIER_EXIT, SERVO_MIN);

    while(true) {

        xQueueReceive(xQueue_Servo_Entry, &entryServo, 0);
        xQueueReceive(xQueue_Servo_Exit, &exitServo, 0);

        entryState = entryServo.state;
        dirEntry = entryServo.dir;
        exitState = exitServo.state;
        dirExit = exitServo.dir;

        //printf("Am primit in servo din Queue_Servo_Entry -> %d %d\n", entryState, dirEntry);
        //printf("Am primit in servo din Queue_Servo_Exit -> %d %d\n", exitState, dirExit);

        // Entry barrier
        if (entryState == BARRIER_MOVING) {
            if (dirEntry) {
                currentEntryPos += SERVO_STEP;
                if (currentEntryPos >= SERVO_MAX) {
                    currentEntryPos = SERVO_MAX;
                    entryState = BARRIER_IDLE;
                }
            } else {
                currentEntryPos -= SERVO_STEP;
                if (currentEntryPos <= SERVO_MIN) {
                    currentEntryPos = SERVO_MIN;
                    entryState = BARRIER_IDLE;
                }
            }
            setMillis(PWM_BARRIER_ENTRY, currentEntryPos);
        }

        //Exit barrier
        if(exitState == BARRIER_MOVING){
            if(dirExit){
                currentExitPos += SERVO_STEP;
                if(currentExitPos >= SERVO_MAX){
                    currentExitPos = SERVO_MAX;
                    exitState = BARRIER_IDLE;
                }
            }else{
                currentExitPos -= SERVO_STEP;
                if(currentExitPos <= SERVO_MIN){
                    currentExitPos = SERVO_MIN;
                    exitState = BARRIER_IDLE;
                }
            }
            setMillis(PWM_BARRIER_EXIT, currentExitPos);
        }


        vTaskDelay(pdMS_TO_TICKS(10)); // Barrier speed
    }
}