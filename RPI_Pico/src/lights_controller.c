#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"   
#include "hardware/i2c.h"

#include "board_config.h"
#include "lights_controller.h"
#include "shared_resources.h"
#include "brightness_module.h"

#define NUMBER_OF_LIGHTS 13
/* Leds that are connected to the PCF with slave address 0x38 */
#define LED0 0
#define LED1 1
#define LED2 2
#define LED3 3
#define LED4 4
#define LED5 5
#define LED6 6
#define LED7 7

/* Leds that are connected to the PCF with slave address 0x39 */
#define LED8 0
#define LED9 1
#define LED10 2
#define LED11 3
#define LED12 4
#define LED13 5
#define LED14 6 // Will be used for detection
#define LED15 7 // Will be used for detection

#define ALL_OFF 0xFF
#define ALL_ON 0xFF
#define FIRST_TREE_ALWAYS_ON 2
#define TURN_ON(data,pos) data &= ~(1<<pos)
#define TURN_OFF(data,pos) data |= 1 << pos
#define ADD_SLOT_OFFSET(slot) slot = slot + FIRST_TREE_ALWAYS_ON
#define TURN_OFF_DELAY 20000

/* Brightness threshold */
#define BRIGHTNESS_THRESHOLD 2000 /* The actual value read by the ADC module*/

/* Local variables */
static uint8_t leds_status[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0}; /* This will be an array that will contain the number of references that needs each led. */

/* Local functions */
static void test_io_Expander(void);
void vTimerTask_turn_off_lights(TimerHandle_t xTimer);
static void update_lights_status(uint8_t, uint8_t*, uint8_t*, int8_t);

void init_lights_controller(){
        // Inițializare I2C1
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(I2C_SDA_LIGHTS, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_LIGHTS, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_LIGHTS);
    gpio_pull_up(I2C_SCL_LIGHTS);
}

void lights_controller_task(void* pvParams){
    while(true){
        test_io_Expander();

        vTaskDelay(pdMS_TO_TICKS(TASK_DELAY));
    }
}

static void update_lights_status(uint8_t slot, uint8_t* data_slave_0, uint8_t* data_slave_1, int8_t value){
    uint8_t initial_slot = slot;
    ADD_SLOT_OFFSET(slot); /* First three reds will be on for every slot that is selected. */

    for(int i = 0; i <= slot; i++){ /* This section needs a mutex. */
        leds_status[i] += value; /* Mark this led as on. */
    }

    /* This check must update the lights with the position greater than 5 (number of slots available. */
    if(initial_slot == 3){
        /* Turn on the led that is in the opposite side of slot 3. */
        leds_status[13] += value;
    } else if(initial_slot == 4){
        leds_status[12] += value;
        leds_status[13] += value;
    } else if(initial_slot == 5){
        for(int i = slot; i < 14; i++){
            leds_status[i] += value;
        }
    }

    /* Check all leds that needs to be on. */
    for(int i = 0; i <= 13; i++){
        if(leds_status[i] > 0){
            if(i > 7){
                TURN_ON(*data_slave_1, i-8);
            }
            else{
                TURN_ON(*data_slave_0, i);
            }
        }
    }
}

void turn_on_lights(uint8_t slot){
    /* 
        This task wants to turn on a list of light from the pakring entry to the assigned parking slot. 
        This must be aceived without disturbind the existing commands.
    */

    uint32_t brightness = get_brightness();
    send_log_message("Turn_on_lights brightness value", brightness);

    /* Check the abmient brightness. If it's under the threshold, we won't turn on the lights. */
    if(brightness < BRIGHTNESS_THRESHOLD){
        return; /* Do nothing. Ignore the request. */
    }

    uint8_t data_slave_0 = ALL_OFF, data_slave_1 = ALL_OFF;

    update_lights_status(slot,&data_slave_0, &data_slave_1, 1);

    /* Send the turn on lights signal on the i2c bus. */
    i2c_write_blocking(I2C_PORT, PCF_0_ADDR, &data_slave_0, 1, false); /* IC slave 0 */
    i2c_write_blocking(I2C_PORT, PCF_1_ADDR, &data_slave_1, 1, false); /* IC slave 1 */

    int voidCastSlot = slot;
    /* Create a new timerTask that will be triggered after 20 secods and executes the turn off function.*/
    TimerHandle_t xTimerSlot = xTimerCreate(
        "TurnOffLightTimer",
        pdMS_TO_TICKS(TURN_OFF_DELAY),
        pdFALSE,
        (void*) voidCastSlot,
        vTimerTask_turn_off_lights
    );

    if(xTimerSlot != NULL){
        xTimerStart(xTimerSlot, 0);
    }
}

void vTimerTask_turn_off_lights(TimerHandle_t xTimer){ //This will be a timer task that will be triggered after x seconds. It's started by the turn_on_function
    int slot = (int)pvTimerGetTimerID(xTimer); /* The parameter for this task represents the slot number.*/

    uint8_t data_slave_0 = ALL_OFF, data_slave_1 = ALL_OFF;

    update_lights_status(slot,&data_slave_0, &data_slave_1, -1);

    /* Send the turn on lights signal on the i2c bus. */
    i2c_write_blocking(I2C_PORT, PCF_0_ADDR, &data_slave_0, 1, false); /* IC slave 0 */
    i2c_write_blocking(I2C_PORT, PCF_1_ADDR, &data_slave_1, 1, false); /* IC slave 1 */

    xTimerDelete(xTimer, 0);
}

static void test_io_Expander(void){
    uint8_t val_on = ALL_ON; 
    uint8_t val_off = ALL_OFF;

    printf("Start test blink P0 la 100ms...\n");

    uint8_t data = ALL_OFF;

    for(int i = 0; i <= 3; i++) {
        TURN_ON(data,i);
    }
        
    i2c_write_blocking(I2C_PORT, PCF_0_ADDR, &data, 1, false);
    i2c_write_blocking(I2C_PORT, PCF_1_ADDR, &val_on, 1, false);

    vTaskDelay(pdMS_TO_TICKS(500));

    for(int i = 0; i <= 3; i++) {
        TURN_OFF(data,i);
    }

    i2c_write_blocking(I2C_PORT, PCF_0_ADDR, &data, 1, false);
    i2c_write_blocking(I2C_PORT, PCF_1_ADDR, &val_off, 1, false);

    vTaskDelay(pdMS_TO_TICKS(500));
}


/*
    *Arguments: boolean status
    *           True = turns on the detection leds
    *           False = turns off the detection leds
    *Brief: This function controls the detection leds.
*/
void send_command_to_detection_zone_leds(bool status){
    uint8_t data_leds = 0;

    /* Check the abmient brightness. If it's under the threshold, we won't turn on the lights. But the turn off command will be executed. */
    if(((get_brightness() && 0xFFF) < BRIGHTNESS_THRESHOLD) && status == ON){
        return; /* Do nothing. Ignore the request. */
    }

    /* Firstly I have to get the status for the rest of the leds. 8 - 13. Just the last two leds must be changed to ON or OFF depending on status arguments. */
    for(int i = 8; i < 16; i++){
        if(leds_status[i - 8] > 0){
            TURN_ON(data_leds, i - 8);          /* This is the payload for the second IOExpander chip */
        } else if((i == 14) || (i == 15)){
            if(status == ON){
                leds_status[i - 8] += 1;
                TURN_ON(data_leds, i - 8);          /* Turn ON the detection leds  */
            }
            else {
                leds_status[i - 8] -= 1;
                if(leds_status[i - 8] == 0){
                    TURN_OFF(data_leds, i - 8);          /* Turn OFF the detection leds */
                }else {
                    TURN_ON(data_leds, i - 8);          /* Keep the detection leds ON because there is another car in the detection zone. */    
                }
            }
        } else {
            TURN_OFF(data_leds, i - 8);
        }
    }

    i2c_write_blocking(I2C_PORT, PCF_1_ADDR, &data_leds, 1, false); /* Send the configuration to the IOExpander that controls the leds with number 8 - 15. */
}