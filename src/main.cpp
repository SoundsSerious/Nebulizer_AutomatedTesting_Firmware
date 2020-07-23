#include "Arduino.h"
#include <WiFi.h>

#include <FreeRTOS.h>

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
                  2,                /* Priority of the task. */
                  NULL);            /* Task handle. */  
 
 //Start Particle Sensor Task 
  delay( 100);
  xTaskCreate(
              particle_sensor_task,          /* Task function. */
              "Read Particle Senosr",        /* String with name of task. */
              10000,            /* Stack size in bytes. */
              NULL,             /* Parameter passed as input of the task */
              2,                /* Priority of the task. */
              NULL);            /* Task handle. */    


  ///Start Boost & Outpub Feedback Voltage Task / Timer
  setup_boost_out_adc();


  //Initalize Control Channels
  setup_control_channels();
  set_control_channel_initial( 0 );
  //Initalize Heater Control

  //Initalize Boost Control
  delay( 100);
  xTaskCreate(
              boost_voltage_control,          /* Task function. */
              "Controls Boost Voltage",        /* String with name of task. */
              10000,            /* Stack size in bytes. */
              NULL,             /* Parameter passed as input of the task */
              20,               /* Priority of the task. */
              NULL);            /* Task handle. */   
                   
  //Initalize Mesh Control
  delay( 100);
  xTaskCreate(
              out_voltage_control,          /* Task function. */
              "Controls Output",        /* String with name of task. */
              1024,            /* Stack size in bytes. */
              NULL,             /* Parameter passed as input of the task */
              5,               /* Priority of the task. */
              NULL);            /* Task handle. */ 

  //Initalize Vacum Control
  xTaskCreate(
              vacfan_control,          /* Task function. */
              "Vac Fan Control",        /* String with name of task. */
              1024,            /* Stack size in bytes. */
              NULL,             /* Parameter passed as input of the task */
              3,               /* Priority of the task. */
              NULL);            /* Task handle. */    

  //Initalize Clear Fan Control
  xTaskCreate(
              clrfan_control,          /* Task function. */
              "Vac Fan Control",        /* String with name of task. */
              1024,            /* Stack size in bytes. */
              NULL,             /* Parameter passed as input of the task */
              3,               /* Priority of the task. */
              NULL);            /* Task handle. */                 
           

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
  if (AUTO_MODE){
    set_duty_vac_fans( 10 );

    set_duty_output(0);
    vTaskDelay(5000);  
    set_duty_output(8);
    vTaskDelay(2);
    set_duty_output(6);
    vTaskDelay(5000); 

    set_duty_output(0);
    set_duty_vac_fans( 255 );

    vTaskDelay(10000); 

    
    vTaskDelay(5000);  
    set_duty_output(8);
    vTaskDelay(2);
    set_duty_output(6);
    vTaskDelay(5000); 

    set_duty_output(0);
    set_duty_vac_fans( 255 );

    vTaskDelay(10000); 
  }
  else{
    vTaskDelay(5000); 
  }
  Serial.println("VboostRaw Vboost Vpp Duty N05 N10 N25 N40 N10");
}



  // delay( 100);
  // xTaskCreate(
  //             boost_feedback_voltage_task,          /* Task function. */
  //             "Read Feedback Voltages",        /* String with name of task. */
  //             10000,            /* Stack size in bytes. */
  //             NULL,             /* Parameter passed as input of the task */
  //             16,                /* Priority of the task. */
  //             NULL);            /* Task handle. */          

  // xTaskCreate(
  //             out_feedback_voltage_task,          /* Task function. */
  //             "Read Feedback Voltages",        /* String with name of task. */
  //             10000,            /* Stack size in bytes. */
  //             NULL,             /* Parameter passed as input of the task */
  //             16,                /* Priority of the task. */
  //             NULL);            /* Task handle. */  


  // set_duty_output(0);
  // vTaskDelay(5000);
  // set_duty_output(1); //nothin
  // vTaskDelay(5000);

  // set_duty_output(0);
  // vTaskDelay(5000);
  // set_duty_output(2);//nothing
  // vTaskDelay(5000);

  // set_duty_output(0);
  // vTaskDelay(5000);
  // set_duty_output(3);
  // vTaskDelay(5000);

  // set_duty_output(0);
  // vTaskDelay(5000);
  // set_duty_output(4);
  // vTaskDelay(5000);

  // set_duty_output(0);
  // vTaskDelay(5000);
  // set_duty_output(5);
  // vTaskDelay(5000);

  // set_duty_output(0);
  // vTaskDelay(5000);  
  // set_duty_output(6);
  // vTaskDelay(5000);

  // set_duty_output(0);
  // vTaskDelay(5000);  
  // set_duty_output(7);
  // vTaskDelay(5000);

  // set_duty_output(0);
  // vTaskDelay(5000);  
  // set_duty_output(8);
  // vTaskDelay(5000); 

  // set_duty_output(0);
  // vTaskDelay(5000);  
  // set_duty_output(8);
  // vTaskDelay(10);
  // set_duty_output(4);
  // vTaskDelay(5000); 

  // set_duty_output(0);
  // vTaskDelay(5000);  
  // set_duty_output(6);
  // vTaskDelay(10);
  // set_duty_output(2);
  // vTaskDelay(5000);  