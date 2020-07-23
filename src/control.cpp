#include "control.h"


double boost_duty = 0;

PID boostPID(&v_boost, &boost_duty, &V_BOOST_SET, Kp_boost, Ki_boost, Kd_boost, DIRECT);

bool hv_protection = false;
bool hv_manual_off = false;

double Kp_boost = 1.0,Ki_boost=1.0,Kd_boost=0.0;
double Kp_therm = 0.1,Ki_therm=0.0001,Kd_therm=0.0;

int boost_max=0, boost_min=0;

portMUX_TYPE controlMutex = portMUX_INITIALIZER_UNLOCKED;


void turn_output_off(){
    portENTER_CRITICAL(&controlMutex);
    boost_min = max(BST_DUTY_MIN_OFF*64/100,0);
    boost_max = min(BST_DUTY_MAX_OFF*64/100,MAX_BOOST_DUTY);
    set_duty_output(0);
    portEXIT_CRITICAL(&controlMutex);
}
void turn_output_on(){
    portENTER_CRITICAL(&controlMutex);
    boost_min = max(BST_DUTY_MIN_ON*64/100,0);
    boost_max = min(BST_DUTY_MAX_ON*64/100,MAX_BOOST_DUTY);
    set_duty_output(min(OUT_DUTY,8));
    portEXIT_CRITICAL(&controlMutex);
}

void boost_voltage_control(void *parameter){

    const TickType_t xDelay = boost_controller_internval_ms / portTICK_PERIOD_MS;


    boostPID.SetTunings(Kp_boost, Ki_boost, Kd_boost);
    boostPID.SetOutputLimits(0,MAX_BOOST_DUTY);
    boostPID.SetSampleTime(voltage_feedback_interval_ms);

    for (;;)
    {   

        boost_error = V_BOOST_SET - v_boost;
        if (BOOST_ON){
            boostPID.SetMode(AUTOMATIC);

            boostPID.SetOutputLimits(boost_min,boost_max);

            

            if (v_boost > V_BOOST_SET+2.5){
                //This should make it go down faster!
                boostPID.SetTunings(Kp_boost*20.0, Ki_boost*5.0, Kd_boost); 
                hv_protection = true;
            }
            else if(hv_protection){
                boostPID.SetTunings(Kp_boost, Ki_boost, Kd_boost);
                hv_protection = false;            
            }

            boostPID.Compute();
            set_duty_boost((int)boost_duty);
        }
        else{ //BOOST_OFF
            boostPID.SetMode(MANUAL);
            set_duty_boost((int)0);
        }

        vTaskDelay(xDelay);
    }
    vTaskDelete(NULL);
}

void out_voltage_control(void *parameter){

    const TickType_t xDelay = out_controller_internval_ms / portTICK_PERIOD_MS;

    for (;;)
    {   
        if (OUT_ON){
            set_duty_output(min(OUT_DUTY,8));
        }
        else{
            set_duty_output(0);
        }

        vTaskDelay(xDelay);
    }
    vTaskDelete(NULL);
}

void thermal_control(void *parameter){

    const TickType_t xDelay = thermal_controller_internval_ms / portTICK_PERIOD_MS;

    for (;;)
    {   
        temp_error = PROCESS_TEMP_SET - thermocouple_temp;
        if (HEAT_ON){
            set_duty_clear_fan(0);
        }
        else{
            set_duty_clear_fan(0);
        }

        vTaskDelay(xDelay);
    }
    vTaskDelete(NULL);
}




void vacfan_control(void *parameter){

    const TickType_t xDelay = vacfan_controller_internval_ms / portTICK_PERIOD_MS;

    for (;;)
    {   
        
        if (VAC_FAN_ON){
            set_duty_vac_fans(VAC_FAN_DUTY);
        }
        else{
            set_duty_vac_fans(0);
        }

        vTaskDelay(xDelay);
    }
    vTaskDelete(NULL);
}


void clrfan_control(void *parameter){

    const TickType_t xDelay = clrfan_controller_internval_ms / portTICK_PERIOD_MS;

    for (;;)
    {   
        if (CLEAR_FAN_ON){
            set_duty_clear_fan(CLR_FAN_DUTY);
        }
        else{
            set_duty_clear_fan(0);
        }

        vTaskDelay(xDelay);
    }
    vTaskDelete(NULL);
}



