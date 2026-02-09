#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "board_config.h"

static void init_board_wh(){
    cyw43_arch_init();
}

static void init_board_normal(){
    //Board settings
}

void init_board(){
    stdio_init_all();

    #if USE_PICO_WH == 1
        init_board_wh();
    #else
        init_board_normal();
    #endif
    printf("%s", "The board was initialized!\n");
}