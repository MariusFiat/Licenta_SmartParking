#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "board_config.h"

static void common_hardware_init(){
    /* To be implemented. */
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