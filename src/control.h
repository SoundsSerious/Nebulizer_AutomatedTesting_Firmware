#ifndef CONTROL_H
#define CONTROL_H

#include <PID_v1.h>

#include "common.h"

#include <signals.h>
#include <sensors.h>
#include <cs_timing.h>

//Define Variables we'll be connecting to
extern double DUTY_IN;
extern double boost_duty;

extern double therm_duty;

//Define the aggressive and conservative Tuning Parameters
extern double Kp_boost,Ki_boost,Kd_boost;
extern double Kp_therm,Ki_therm,Kd_therm;

extern int boost_max, boost_min;

extern PID boostPID; 
extern PID thermPID;

extern bool hv_protection;
extern bool hv_manual_off;

// void turn_boost_off();
// void turn_boost_on();

extern void turn_output_off();
extern void turn_output_on();

// void control_heat_off();
// void control_heat_on();
// void update_thermal_control();

// void clear_fans_on();
// void clear_fans_off();
// void update_clearfans();

// void vac_fans_on();
// void vac_fans_off();
// void update_vac_fans();

void boost_voltage_control(void *parameter);
void out_voltage_control(void *parameter);
void vacfan_control(void *parameter);
void clrfan_control(void *parameter);
void thermal_control(void *parameter);

#endif