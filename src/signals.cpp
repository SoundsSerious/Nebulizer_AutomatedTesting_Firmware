#include <signals.h>




void setup_control_channels()
{
    periph_module_enable(PERIPH_LEDC_MODULE);

    pinMode(BOOST_PIN, OUTPUT);
    pinMode(MESHOUT_PIN, OUTPUT);

    //Low level ESPIDF
    ledc_timer_config_t ledc_output_timer;
    ledc_output_timer.speed_mode = LEDC_HIGH_SPEED_MODE;     // timer mode
    ledc_output_timer.duty_resolution = (ledc_timer_bit_t)4; // resolution of PWM duty
    ledc_output_timer.timer_num = LEDC_TIMER_1;              // timer index
    ledc_output_timer.freq_hz = 5000000;                     // root signal frequency of PWM signal,

    ledc_timer_config_t ledc_boost_timer;
    ledc_boost_timer.speed_mode = LEDC_HIGH_SPEED_MODE;     // timer mode
    ledc_boost_timer.duty_resolution = (ledc_timer_bit_t)6; // resolution of PWM duty
    ledc_boost_timer.timer_num = LEDC_TIMER_0;              // timer index
    ledc_boost_timer.freq_hz = 1250000;                     // root signal frequency of PWM signal,

    ledc_channel_config_t ledc_output_channel = {
        gpio_num : MESHOUT_PIN,
        speed_mode : LEDC_HIGH_SPEED_MODE,
        channel : LEDC_CHANNEL_4,
        intr_type : LEDC_INTR_DISABLE,
        timer_sel : LEDC_TIMER_1,
        duty : 1,
        hpoint : 0
    };

    ledc_channel_config_t ledc_boost_channel = {
        gpio_num : BOOST_PIN,
        speed_mode : LEDC_HIGH_SPEED_MODE,
        channel : LEDC_CHANNEL_0,
        intr_type : LEDC_INTR_DISABLE,
        timer_sel : LEDC_TIMER_0,
        duty : 1,
        hpoint : 0
    };

    // Set configuration of timer0 for high speed channels
    esp_err_t resulto = ledc_timer_config(&ledc_output_timer);
    if (resulto == ESP_OK)
        Serial.printf("init output frequency: %d", ledc_get_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_1));
    Serial.println();

    // Set LED Controller with previously prepared configuration
    ledc_channel_config(&ledc_output_channel);

    // Set configuration of timer0 for high speed channels
    esp_err_t resultb = ledc_timer_config(&ledc_boost_timer);
    if (resultb == ESP_OK)
        Serial.printf("init boost frequency: %d", ledc_get_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0));
    Serial.println();

    // Set LED Controller with previously prepared configuration
    ledc_channel_config(&ledc_boost_channel);

    //Low Level Arduino
    ledcAttachPin(HEATER_PIN, HEATER_CH);
    ledcSetup(HEATER_CH, 1000, 8);

    ledcAttachPin(FANS_PIN, FANS_CH);
    ledcSetup(FANS_CH, 1000, 8);

    ledcAttachPin(CLEARFAN_PIN, CLEARFAN_CH);
    ledcSetup(CLEARFAN_CH, 1000, 8);

    esp_err_t resultso = ledc_set_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_1, 202500);
    if (resultso == ESP_OK)
        Serial.printf("output frequency: %d", ledc_get_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_1));
    Serial.println();

    esp_err_t resultsb = ledc_set_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0, 500700);
    if (resultsb == ESP_OK)
        Serial.printf("boost frequency: %d", ledc_get_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0));
    Serial.println();
}


void set_control_channel_duty()
{

    //Low Level Arduino
    ledcWrite(HEATER_CH, 127);
    ledcWrite(FANS_CH, 127);
    ledcWrite(CLEARFAN_CH, 127);

    //Low Level ESP IDF
    esp_err_t resultsdo = ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4, 7); //50%
    if (resultsdo == ESP_OK)
    {
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4);
    }
    esp_err_t resultsdb = ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 31); //50%
    if (resultsdb == ESP_OK)
    {
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
    }
}