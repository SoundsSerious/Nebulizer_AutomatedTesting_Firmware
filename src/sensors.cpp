#include "sensors.h"


//Temp sensor initalization
bool temp_last_sensor_status = false;
bool temp_sensor_is_nominal = false;

double thermocouple_temp;
double sensor_ic_temp;
double raw_temp;

SimpleKalmanFilter mesh_temp_filter = SimpleKalmanFilter(1, 1, 0.5);
Adafruit_MAX31855 thermocouple = Adafruit_MAX31855(MAXCLK, MAXCS, MAXDO);

unsigned long fail_time_allowed = 1 * one_second; //seconds
unsigned long fail_time_begin;                    // = 0.0 //seconds
bool heat_allowed;                                     // = false

String thermocouple_failure_message = "";


//PM SEnsor initializatoin
String pmsensor_failure_message = "";

s16 ret;
u8 auto_clean_days = 1;
u32 auto_clean;
struct sps30_measurement m;
char serial[SPS_MAX_SERIAL_LEN];
u16 data_ready;
int sps_fail_count = 0;
s16 init_ret;

//Amplification Reading
SimpleKalmanFilter v_boost_filter = SimpleKalmanFilter(2, 2, 0.01);
SimpleKalmanFilter v_out_filter = SimpleKalmanFilter(2, 2, 0.025);
SimpleKalmanFilter vpp_out_filter = SimpleKalmanFilter(2, 2, 0.05);

double v_boost;
float v_boost_raw;
float v_out_raw;
float v_out,v_out_pp;
int acd_int_vboost;
int acd_int_vout;

int v_out_baseline;
int vrms_out_int;
int v_out_raw_int;

float nc_bin_005,nc_bin_010,nc_bin_025,nc_bin_040,nc_bin_100;
float log_nc_bin_005,log_nc_bin_010,log_nc_bin_025,log_nc_bin_040,log_nc_bin_100;

float mc_bin_010,mc_bin_025,mc_bin_040,mc_bin_100;
float log_mc_bin_010,log_mc_bin_025,log_mc_bin_040,log_mc_bin_100;


float mass_average_particle_size;

//ADC Timer
portMUX_TYPE DRAM_ATTR timerMux = portMUX_INITIALIZER_UNLOCKED; 
TaskHandle_t complexHandlerTask;
hw_timer_t * adcTimer = NULL; // our timer

bool pm_sensor_is_nominal = false;

void initalize_particle_sensor()
{   init_ret = sps30_probe();
    while ( init_ret != 0)
    {
        pmsensor_failure_message = "SPS sensor initialize failed " + String(init_ret);
        Serial.println(pmsensor_failure_message);
        delay(500);
    }

    Serial.print("SPS sensor probing successful\n");

    ret = sps30_set_fan_auto_cleaning_interval_days(auto_clean_days);
    if (ret)
    {
        Serial.print("error setting the auto-clean interval: ");
        Serial.println(ret);
    }

    ret = sps30_start_measurement();
    if (ret < 0)
    { pm_sensor_is_nominal = false;
        Serial.print("error starting measurement\n");
    }
    else{
      pm_sensor_is_nominal = true;
    }

    Serial.print("measurements started\n");
    
}

void read_particle_sensor()
{

    ret = sps30_read_data_ready(&data_ready);

    ret = sps30_read_measurement(&m);
    if (ret < 0)
    {   
        sps_fail_count += 1;
        //if (sps_fail_count > 10){
        // Serial.print("error reading measurement\n");
        //  sps_fail_count = 0;
        //}
        pm_sensor_is_nominal = false;
        
        if ( (sps_fail_count > 10) & (sps_fail_count%2 ==0)){
          s16 init_ret = sps30_probe();
          pmsensor_failure_message = "error initalizing pm sensor "+String(init_ret);
        }
        else{
          pmsensor_failure_message = "error reading pm sensor "+String(ret);
        }
        
    }
    else{
        pm_sensor_is_nominal = true;
        pmsensor_failure_message = "PM Sensor Working Normally";

        sps_fail_count = 0;

        mc_bin_010 = m.mc_1p0;
        mc_bin_025 = m.mc_2p5 - m.mc_1p0;
        mc_bin_040 = m.mc_4p0 - m.mc_2p5;
        mc_bin_100 = m.mc_10p0 - m.mc_4p0;

        mass_average_particle_size = ( 0.5 * mc_bin_010 + 1.75 * mc_bin_025 +
                                     3.25 * mc_bin_040 + 7.0 *mc_bin_100 ) / 
                                     ( mc_bin_010 + mc_bin_025 + mc_bin_040 + mc_bin_100);

        nc_bin_005 = m.nc_0p5;
        nc_bin_010 = m.nc_1p0 - m.nc_0p5;
        nc_bin_025 = m.nc_2p5 - m.nc_1p0;
        nc_bin_040 = m.nc_4p0 - m.nc_2p5;
        nc_bin_100 = m.nc_10p0 - m.nc_4p0;

        log_nc_bin_005 = 10 * log10f(nc_bin_005+1);
        log_nc_bin_010 = 10 * log10f(nc_bin_010+1);
        log_nc_bin_025 = 10 * log10f(nc_bin_025+1);
        log_nc_bin_040 = 10 * log10f(nc_bin_040+1);
        log_nc_bin_100 = 10 * log10f(nc_bin_100+1);

        
        
    }
}

void read_thermocouple()
{
    raw_temp = (float)thermocouple.readCelsius();
    sensor_ic_temp = (float)thermocouple.readInternal();

    if (isnan(raw_temp))
    {
        temp_sensor_is_nominal = false;
        int error_message = thermocouple.readError();
        switch (error_message)
        {
        case 1:
            thermocouple_failure_message = "ThermoCouple Open Circuit";
            break;
        case 2:
            thermocouple_failure_message = "ThermoCouple Short To Ground";
            break;
        case 4:
            thermocouple_failure_message = "ThermoCouple Short To VCC";
            break;
        default:
            thermocouple_failure_message = "Got Unexpected Error: " + String(error_message);
        }
    }
    else if (sensor_ic_temp <= 0 || raw_temp <= 0)
    {
        temp_sensor_is_nominal = false;
        thermocouple_failure_message = "ThermoCouple Reads Zero";
    }
    else
    {
        temp_sensor_is_nominal = true;
        thermocouple_failure_message = "Thermocouple Working Normally";
        thermocouple_temp = mesh_temp_filter.updateEstimate(raw_temp);
    }

    //Failure Tolerance Loop
    if (temp_sensor_is_nominal)
    {
        heat_allowed = true;
        fail_time_begin = micros(); //Keep updating
    }
    else
    {
        if (micros() - fail_time_begin < fail_time_allowed)
        {
            heat_allowed = true;
        }
        else
        {
            heat_allowed = false;
        }
    }
}

//Tasks
void thermocouple_task(void *parameter)
{

    const TickType_t xDelay = temp_sensor_interval_ms / portTICK_PERIOD_MS;

    thermocouple.begin();
    vTaskDelay(xDelay);

    for (;;)
    { // Always Run
        read_thermocouple();
        vTaskDelay(xDelay);
    }
    vTaskDelete(NULL);
}

void particle_sensor_task(void *parameter)
{

    const TickType_t xDelay = pm_sensor_interval_ms / portTICK_PERIOD_MS;

    initalize_particle_sensor();

    vTaskDelay(xDelay);

    for (;;)
    { // Always Run
        read_particle_sensor();
        vTaskDelay(xDelay);
    }
    vTaskDelete(NULL);
}



void setup_boost_out_adc(){ 
    pinMode(V_BOOST_FB_PIN, INPUT);
    pinMode(V_OUT_FB_PIN, INPUT);
    analogReadResolution(12);

  xTaskCreate(filter_update, "Filter Update", 8192, NULL, 1, &complexHandlerTask);
  adcTimer = timerBegin(3, 80, true); // 80 MHz / 80 = 1 MHz hardware clock for easy figuring
  timerAttachInterrupt(adcTimer, &onTimer, true); // Attaches the handler function to the timer 
  timerAlarmWrite(adcTimer,80, true); // Interrupts when counter == 45, i.e. 22.222 times a second
  timerAlarmEnable(adcTimer);


}

void filter_update(void *param){

    float out_convert = (3.3 / 4095.0) *  (430 / 6.8);
    float out_inv = 1.0 / out_convert;

    float boost_convert =  (3.3 / 4095.0) * (230 / 10);

    const TickType_t xDelay = voltage_feedback_interval_ms / portTICK_PERIOD_MS;


    for (;;) {
      // Sleep until the ISR gives us something to do, or for 1 second
      //uint32_t tcount = ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(voltage_feedback_interval_ms));  

      //Transform To Voltage Space
      v_out_raw = ((float) acd_int_vout ) *out_convert;        
      v_boost_raw = ((float) acd_int_vboost ) * boost_convert;
      
      //Update Kalman Filters
      v_boost = v_boost_filter.updateEstimate( v_boost_raw );
      v_out = v_out_filter.updateEstimate( v_out_raw );

      //Convert Back to Int Space
      v_out_baseline = (int) (v_out * out_inv);

      //Create RMS Estimate
      vrms_out_int = vrms_out_int * 0.9;
      v_out_pp = 2 * out_convert *  vpp_out_filter.updateEstimate( vrms_out_int );

      vTaskDelay(xDelay);
    }   

}

void IRAM_ATTR onTimer() {
  // A mutex protects the handler from reentry (which shouldn't happen, but just in case)
  portENTER_CRITICAL_ISR(&timerMux);

  v_out_raw_int = analogRead(V_OUT_FB_PIN);

  //Use fast lowpass 50% pass filter update data 
  acd_int_vboost = (acd_int_vboost>>1) + (analogRead(V_BOOST_FB_PIN)>>1);
  acd_int_vout = (acd_int_vout>>1) + (v_out_raw_int>>1);

  vrms_out_int =  max(vrms_out_int,abs(v_out_raw_int - v_out_baseline));

  portEXIT_CRITICAL_ISR(&timerMux);
}







//   if (buffer_overflow) { 
//     // Notify complexHandlerTask that the buffer is full.
//     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//     vTaskNotifyGiveFromISR(complexHandlerTask, &xHigherPriorityTaskWoken);
//     if (xHigherPriorityTaskWoken) {
//       portYIELD_FROM_ISR();
//     }
//   }


//Task Based Data Reading
// void boost_feedback_voltage_task(void *parameter)
// {

//     const TickType_t xDelay = voltage_feedback_interval_ms / portTICK_PERIOD_MS;



//     vTaskDelay(xDelay);

//     for (;;)
//     {   
//         // Always Run
//         acd_int_vboost = analogRead(V_BOOST_FB_PIN); //3.3 * analogRead(V_BOOST_FB_PIN) * (190 / 10) / 4096;
        
//         v_boost_raw = (((float) acd_int_vboost ) * 3.3 / 4095.0) * (230 / 10);
//         v_boost = v_boost_filter.updateEstimate( v_boost_raw );

//         // v_out_raw = (((float) acd_int_vout ) * 3.3 / 4095.0) *  (1010 / 10);
//         // v_out = v_out_filter.updateEstimate( v_out_raw );
//         vTaskDelay(xDelay);
//     }
//     vTaskDelete(NULL);
// }

// void out_feedback_voltage_task(void *parameter)
// {

//     pinMode(V_BOOST_FB_PIN, INPUT);
//     pinMode(V_OUT_FB_PIN, INPUT);
//     analogReadResolution(12);

//     taskYIELD();

//     for (;;)
//     {   
//         // Always Run
//          //3.3 *  * (1010 / 10) / 4096;
//         taskYIELD();
//         v_out_raw = (((float) acd_int_vout ) * 3.3 / 4095.0) *  (1010 / 10);
//         taskYIELD();
//         v_out = v_out_filter.updateEstimate( v_out_raw );
//         taskYIELD();
//     }
//     vTaskDelete(NULL);
// }