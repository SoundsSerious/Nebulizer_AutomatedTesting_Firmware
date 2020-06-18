#ifndef SIGNALS_H
#define SIGNALS_H

#include "Arduino.h"
#include "config.h"

#include "driver/ledc.h"
#include "analogWrite.h"



#define BOOST_CH LEDC_CHANNEL_0
#define MESHOUT_CH LEDC_CHANNEL_4



// void volume_fans_duty(float percent)
// {

    
// }

void setup_control_channels();
void set_control_channel_duty();




#endif 