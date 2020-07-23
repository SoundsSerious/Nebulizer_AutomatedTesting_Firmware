#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include <math.h> 




extern unsigned long one_second;// = 1000000; //microseconds
extern unsigned long one_minute;// = one_second*5; //microseconds

extern bool TEST_RUNNING;

extern bool AUTO_MODE;
extern bool BOOST_ON;
extern bool OUT_ON;
extern bool HEAT_ON;
extern bool CLEAR_FAN_ON;
extern bool VAC_FAN_ON;

extern int RUN_TIME;
extern int CLEAR_TIME;
extern int VAC_TIME;
extern int BST_TIME;

extern int TEMP_MAX_DUTY;
extern int BST_DUTY_MAX_ON;
extern int BST_DUTY_MAX_OFF;
extern int BST_DUTY_MIN_ON;
extern int BST_DUTY_MIN_OFF;
extern int OUT_DUTY;
extern int CLR_FAN_DUTY;
extern int VAC_FAN_DUTY;

extern int BOOST_FREQUENCY;
extern int OUT_FREQUENCY;

extern double V_BOOST_SET;
extern double PROCESS_TEMP_SET;


//calc variables
extern double heat_duty;

extern float boost_error;
extern float temp_error;

extern float estimated_boost_power;
extern float estimated_output_power;
extern float estimated_system_efficiency;
extern float measured_vin;
extern float temp_error;

extern float last_typical_particle_size;
extern float last_massflow;
extern float inst_typical_particle_size;
extern float inst_massflow;

extern bool thermally_ready;



#endif
