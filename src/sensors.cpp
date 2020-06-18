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
bool heat_on;                                     // = false

String failure_message = "";


//PM SEnsor initializatoin
s16 ret;
u8 auto_clean_days = 1;
u32 auto_clean;
struct sps30_measurement m;
char serial[SPS_MAX_SERIAL_LEN];
u16 data_ready;

float V_Boost;
float V_Out;

void initalize_particle_sensor()
{
    while (sps30_probe() != 0)
    {
        Serial.print("SPS sensor probing failed\n");
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
    {
        Serial.print("error starting measurement\n");
    }

    Serial.print("measurements started\n");
}

void read_particle_sensor()
{

    ret = sps30_read_data_ready(&data_ready);

    ret = sps30_read_measurement(&m);
    if (ret < 0)
    {
        Serial.print("error reading measurement\n");
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
            failure_message = "ThermoCouple Open Circuit";
            break;
        case 2:
            failure_message = "ThermoCouple Short To Ground";
            break;
        case 4:
            failure_message = "ThermoCouple Short To VCC";
            break;
        default:
            failure_message = "Got Unexpected Error: " + String(error_message);
        }
    }
    else if (sensor_ic_temp <= 0 || raw_temp <= 0)
    {
        temp_sensor_is_nominal = false;
        failure_message = "ThermoCouple Reads Zero";
    }
    else
    {
        temp_sensor_is_nominal = true;
        failure_message = "";
        thermocouple_temp = mesh_temp_filter.updateEstimate(raw_temp);
    }

    //Failure Tolerance Loop
    if (temp_sensor_is_nominal)
    {
        heat_on = true;
        fail_time_begin = micros(); //Keep updating
    }
    else
    {
        if (micros() - fail_time_begin < fail_time_allowed)
        {
            heat_on = true;
        }
        else
        {
            heat_on = false;
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

void feedback_voltage_task(void *parameter)
{

    const TickType_t xDelay = voltage_feedback_interval_ms / portTICK_PERIOD_MS;

    pinMode(V_BOOST_FB_PIN, INPUT);
    pinMode(V_OUT_FB_PIN, INPUT);

    vTaskDelay(xDelay);

    for (;;)
    { // Always Run
        V_Boost = 3.3 * analogRead(V_BOOST_FB_PIN) * (190 / 10) / 4096;
        V_Out = 3.3 * analogRead(V_OUT_FB_PIN) * (1010 / 10) / 4096;
        vTaskDelay(xDelay);
    }
    vTaskDelete(NULL);
}
