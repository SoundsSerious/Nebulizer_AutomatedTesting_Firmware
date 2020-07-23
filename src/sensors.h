#ifndef SENSORS_H
#define SENSORS_H

#include <config.h>
#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include <sps30.h>
#include "SimpleKalmanFilter.h"
#include <cs_timing.h>
#include <common.h>

//ADC Stuff
#include <soc/sens_reg.h>
#include <soc/sens_struct.h>
#include <driver/adc.h>

extern SimpleKalmanFilter mesh_temp_filter;

//Thermocouple Initialization
extern bool temp_last_sensor_status;
extern bool temp_sensor_is_nominal;

extern String thermocouple_failure_message;


extern unsigned long fail_time_allowed;// = 5 * one_second;//seconds
extern unsigned long fail_time_begin;// = 0.0 //seconds
extern bool heat_allowed; // = false

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
extern String pmsensor_failure_message;
extern bool pm_sensor_is_nominal;

//Voltage Feedback
extern SimpleKalmanFilter v_boost_filter;
extern SimpleKalmanFilter v_out_filter;
extern SimpleKalmanFilter vpp_out_filter;

extern int acd_int_vboost;
extern int acd_int_vout;
extern float v_boost_raw;
extern double v_boost;
extern float v_out_raw;
extern float v_out,v_out_pp;

extern int v_out_baseline;
extern int vrms_out_int;
extern int v_out_raw_int;

extern float nc_bin_005,nc_bin_010,nc_bin_025,nc_bin_040,nc_bin_100;
extern float log_nc_bin_005,log_nc_bin_010,log_nc_bin_025,log_nc_bin_040,log_nc_bin_100;

extern float mc_bin_010,mc_bin_025,mc_bin_040,mc_bin_100;

extern float mass_average_particle_size;

//ADC Timer
extern portMUX_TYPE DRAM_ATTR timerMux;
extern TaskHandle_t complexHandlerTask;
extern hw_timer_t * adcTimer;

void initalize_particle_sensor();
void read_particle_sensor();
void particle_sensor_task(void * parameter);

void read_thermocouple();
void thermocouple_task(void * parameter);


void setup_boost_out_adc();

void filter_update(void *param);
void IRAM_ATTR onTimer();

int IRAM_ATTR local_adc1_read(int channel);

//Task Based Data Reading
// void boost_feedback_voltage_task(void *parameter);
// void out_feedback_voltage_task(void *parameter);



#endif // SENSORS_H
