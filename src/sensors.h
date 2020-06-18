#ifndef SENSORS_H
#define SENSORS_H

#include <config.h>
#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include <sps30.h>
#include "SimpleKalmanFilter.h"
#include <cs_timing.h>
#include <common.h>


extern SimpleKalmanFilter mesh_temp_filter;

//Thermocouple Initialization
extern bool temp_last_sensor_status;
extern bool temp_sensor_is_nominal;

extern String failure_message;

extern unsigned long fail_time_allowed;// = 5 * one_second;//seconds
extern unsigned long fail_time_begin;// = 0.0 //seconds
extern bool heat_on; // = false

extern Adafruit_MAX31855 thermocouple;
extern double thermocouple_temp;
extern double sensor_ic_temp;
extern double raw_temp;


//Particle Sensor Initialization
extern s16 ret;
extern u8 auto_clean_days;
extern u32 auto_clean;
extern struct sps30_measurement m;
extern char serial[SPS_MAX_SERIAL_LEN];
extern u16 data_ready;


//Voltage Feedback
extern float V_Boost;
extern float V_Out;

void initalize_particle_sensor();
void read_particle_sensor();
void particle_sensor_task(void * parameter);

void read_thermocouple();
void thermocouple_task(void * parameter);


void feedback_voltage_task(void *parameter);




#endif // SENSORS_H
