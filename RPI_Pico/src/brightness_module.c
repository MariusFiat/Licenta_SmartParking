/*
    * brightness_module.c
    *
    *  Created on: 17 Apr 2024
    *   This module is responsible for checking the status of the parking lot lights based on the ambient light conditions. It uses an ADC to read the ambient light level 
    * [..] decide if the lights should be turned on or off. The module also includes a timer to periodically check the ambient light level and adjust the brightness accordingly.
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"   
#include "hardware/i2c.h"

#include "board_config.h"
#include "brightness_module.h"

#define BRIGHNESS_SENSOR_CHANNEL_0 0 /* ADC channel for GPIO26 */
#define BRIGHNESS_SENSOR_CHANNEL_1 1 /* ADC channel for GPIO27 */
#define ENABLED_CHANNELS BRIGHNESS_SENSOR_CHANNEL_0
