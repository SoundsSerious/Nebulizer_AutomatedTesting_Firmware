#include "Arduino.h"
#include <WiFi.h>

#include "config.h"
#include "communication.h"
#include "sensors.h"
#include "control.h"
#include "signals.h"

void setup()
{
  begin_communications();

  //Start Sensors
  //Start Thermocuple Task
  delay( 100);
  xTaskCreate(
                  thermocouple_task,          /* Task function. */
                  "Read Thermocouples",        /* String with name of task. */
                  10000,            /* Stack size in bytes. */
                  NULL,             /* Parameter passed as input of the task */
                  10,                /* Priority of the task. */
                  NULL);            /* Task handle. */  
 
 //Start Particle Sensor Task 
  delay( 100);
  xTaskCreate(
              particle_sensor_task,          /* Task function. */
              "Read Particle Senosr",        /* String with name of task. */
              10000,            /* Stack size in bytes. */
              NULL,             /* Parameter passed as input of the task */
              10,                /* Priority of the task. */
              NULL);            /* Task handle. */    

 //Start Particle Sensor Task 
  delay( 100);
  xTaskCreate(
              feedback_voltage_task,          /* Task function. */
              "Read Feedback Voltages",        /* String with name of task. */
              10000,            /* Stack size in bytes. */
              NULL,             /* Parameter passed as input of the task */
              12,                /* Priority of the task. */
              NULL);            /* Task handle. */                 

  //Initalize Control Channels
  setup_control_channels();
  set_control_channel_duty();
  //Initalize Heater Control
  //Initalize Particle Cleaerance Control
  //Initalize Mesh Clearance Control
  //Initalize Boost Control
  //Initalize Mesh Control

  //Communications Tasks
  delay( 100);
  xTaskCreate(     print_status,          /* Task function. */
                    "Serial Status ",        /* String with name of task. */
                    10000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */

  Serial.println("Starting System");
}

void loop()
{
  Serial.println("We're Doign It");

  set_control_channel_duty();
  delay(1000);
}
