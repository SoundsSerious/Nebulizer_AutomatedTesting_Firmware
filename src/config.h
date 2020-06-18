#ifndef CONFIG_H
#define CONFIG_H

// Default connection is using software SPI, but comment and uncomment one of
// the two examples below to switch between software SPI and hardware SPI:

// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define MAXDO 19
#define MAXCS 16
#define MAXCLK 18

#define HEATER_PIN 15
#define FANS_PIN 5
#define CLEARFAN_PIN 17
#define BOOST_PIN 13
#define MESHOUT_PIN 12

#define V_BOOST_FB_PIN 34
#define V_OUT_FB_PIN 35

#define HEATER_CH 11
#define FANS_CH 12
#define CLEARFAN_CH 13


//HEATER PWM LIMITS
#define PWM_MAX 252
#define PWM_MIN 0


#endif
