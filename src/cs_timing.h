#ifndef CS_TIMING_H
#define CS_TIMING_H


extern unsigned long control_loop_ms;
extern unsigned long pm_sensor_interval_ms;
extern unsigned long temp_sensor_interval_ms;
extern unsigned long wifi_interval_ms;

extern unsigned long voltage_feedback_interval_ms;

extern unsigned long boost_controller_internval_ms;
extern unsigned long out_controller_internval_ms;

extern unsigned long vacfan_controller_internval_ms ;
extern unsigned long clrfan_controller_internval_ms;
extern unsigned long thermal_controller_internval_ms;

#endif