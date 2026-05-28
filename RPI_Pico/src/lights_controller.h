#ifndef __LIGHTS_CONTROLLER_H
#define __LIGHTS_CONTROLLER_H

#define ON true
#define OFF false

//void lights_controller_task(void *pvParams);
void turn_on_lights(uint8_t);
void init_lights_controller(void);
void send_command_to_detection_zone_leds(bool status);

#endif