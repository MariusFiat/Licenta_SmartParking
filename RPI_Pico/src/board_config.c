#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "board_config.h"
#include "lights_controller.h"
#include "brightness_module.h"

static void common_hardware_init(){
    init_brightness_submodules(); /* Initialize the brightness submodules! DMA + ADC*/
    init_lights_controller();
}

static void init_board_wh(){
    /* Infineon chip initialization */
    cyw43_arch_init();

    /* Common part with the normal board version*/
    common_hardware_init();
}

static void init_board_normal(){
    /* Specific hardware initialization. */

    /* Common hardware with the wifi version */
    common_hardware_init();
}

void init_board(){
    stdio_init_all();

    #if USE_PICO_WH == 1
        init_board_wh();
    #else
        init_board_normal();
    #endif
    //printf("%s", "The board was initialized!\n");
}

#if USE_PICO_WH == 0

void blink_built_in_led(void){
    static bool init = false;
    static uint8_t val = 0;

    if(init == false){
        init = true;
        gpio_init(25);
        gpio_set_dir(25, true); /* Set GPIO25 (the onboard LED) as output. */
    } else{
        gpio_put(25, val);
        val = !val;
    }
}

#endif

void send_log_message(char* msg, uint32_t val){
    printf("[RP2040]: %s -> %d\n", msg, val);
}