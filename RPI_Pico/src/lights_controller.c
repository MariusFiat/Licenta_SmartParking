#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"   
#include "hardware/i2c.h"

#include "board_config.h"
#include "lights_controller.h"
#include "shared_resources.h"

void test_io_Expander(void);

void lights_controller_task(void* pvParams){

    // Inițializare I2C1
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(I2C_SDA_LIGHTS, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_LIGHTS, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_LIGHTS);
    gpio_pull_up(I2C_SCL_LIGHTS);
    
    while(true){
        test_io_Expander();

        vTaskDelay(pdMS_TO_TICKS(TASK_DELAY));
    }
}

void turn_on_lights(uint8_t slot){
    /* 
        This task wants to turn on a list of light from the pakring entry to the assigned parking slot. 
        This must be aceived without disturbind the existing commands.
    */
}

void turn_off_lights(uint8_t slot){

}

void test_io_Expander(void){
    uint8_t val_on = 0x0; 
    uint8_t val_off = 0xFF;

    printf("Start test blink P0 la 100ms...\n");

    while(true) {
        i2c_write_blocking(I2C_PORT, PCF_0_ADDR, &val_on, 1, false);

        vTaskDelay(pdMS_TO_TICKS(500));

        i2c_write_blocking(I2C_PORT, PCF_0_ADDR, &val_off, 1, false);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}