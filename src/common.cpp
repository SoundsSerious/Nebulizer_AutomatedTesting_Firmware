#include "common.h"


unsigned long one_second = 1000000; //microseconds
unsigned long one_minute = one_second*5; //microseconds

bool TEST_RUNNING = false;

bool AUTO_MODE = false;
bool BOOST_ON = false ;
bool OUT_ON = false;
bool HEAT_ON = false;
bool CLEAR_FAN_ON = false;
bool VAC_FAN_ON = false;


//Times in ms
 int RUN_TIME = 3000;
 int CLEAR_TIME = 1000;
 int VAC_TIME = 10000;
 int BST_TIME = 1000;

//Thermal Duty
 int TEMP_MAX_DUTY = 25;
 
//Boost Duty
 int BST_DUTY_MAX_ON = 93;
 int BST_DUTY_MAX_OFF = 83;
 int BST_DUTY_MIN_ON = 60;
 int BST_DUTY_MIN_OFF = 0;

//Other Duty
 int OUT_DUTY = 8;
 int CLR_FAN_DUTY = 100;
 int VAC_FAN_DUTY = 255;

 int BOOST_FREQUENCY = 500700;
 int OUT_FREQUENCY = 134000;

 double V_BOOST_SET = 50;
double PROCESS_TEMP_SET = 0;


//calc variables
double heat_duty= 0;

float boost_error= 0;

float estimated_boost_power= 0;
float estimated_output_power= 0;
float estimated_system_efficiency= 0;
float measured_vin= 0;
float temp_error= 0;

float last_typical_particle_size= 0;
float last_massflow= 0;
float inst_typical_particle_size= 0;
float inst_massflow= 0;

bool thermally_ready= 0;
