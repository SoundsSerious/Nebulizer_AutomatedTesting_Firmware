#ifndef SIGNALS_H
#define SIGNALS_H

#include "Arduino.h"
#include "config.h"
#include "common.h"
#include "driver/ledc.h"
#include "analogWrite.h"


#define BOOST_CH LEDC_CHANNEL_0
#define MESHOUT_CH LEDC_CHANNEL_4

#define BOOST_ROOT_FREQUENCY 1250000
#define MESH_ROOT_FREQUENCY 5000000


#define MAX_BOOST_DUTY 56 //No Load 5-22V, 10-55V, 15-62V+
#define MAX_OUTPUT_DUTY 7 //or less than 16/2


void setup_control_channels();
void set_control_channel_initial(int intial_duty);

void set_boost_frequency(int frequency);
void set_output_frequency(int frequency);

void set_duty_heater( uint8_t duty);
void set_duty_vac_fans( uint8_t duty);
void set_duty_clear_fan( uint8_t duty);

void set_duty_boost(int duty);
void set_duty_output(int duty);

#endif 