#include <cs_timing.h>

unsigned long control_loop_ms = 100;
unsigned long pm_sensor_interval_ms = 500;
unsigned long temp_sensor_interval_ms = 500;
unsigned long wifi_interval_ms = 1000;

unsigned long voltage_feedback_interval_ms = 1;

unsigned long boost_controller_internval_ms = 2;
unsigned long out_controller_internval_ms = 5;

unsigned long vacfan_controller_internval_ms = 100;
unsigned long clrfan_controller_internval_ms = 100;
unsigned long thermal_controller_internval_ms = 100;