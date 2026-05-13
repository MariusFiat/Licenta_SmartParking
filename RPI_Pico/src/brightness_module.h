#ifndef __BRIGHTNESS_MODULE_H__
#define __BRIGHTNESS_MODULE_H__

#include "board_config.h"

uint16_t x_read_ambient_light_once(void);
uint32_t get_brightness(void);
void init_brightness_submodules(void);

#endif /* __BRIGHTNESS_MODULE_H__ */