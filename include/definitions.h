#ifndef DEFINITIONS_H
#define DEFINITIONS_H

//SERIAL-MONITOR
#ifndef BAUD_RATE
#define BAUD_RATE 115200
#endif

//DEBUG
#ifndef DEBUG_SENSORS
#define DEBUG_SENSORS false
#endif
#ifndef DEBUG_PIR_SKIP_INIT
#define DEBUG_PIR_SKIP_INIT false
#endif

//SENSORS
#ifndef SENSOR_LIGHT_TRIGGER
#define SENSOR_LIGHT_TRIGGER 5
#endif
#ifndef SENSOR_DISTANCE_TRIGGER
#define SENSOR_DISTANCE_TRIGGER 170
#endif
#ifndef SENSOR_MOTION_TIMEOUT
#define SENSOR_MOTION_TIMEOUT 30000
#endif
#ifndef SENSORS_SECURITY
#define SENSORS_SECURITY 5000
#endif

//LED
#ifndef LED_TOGGLE
#define LED_TOGGLE 1500
#endif
#ifndef LED_AUTO_TIMEOUT
#define LED_AUTO_TIMEOUT 60000
#endif
#ifndef LED_FADE_DELAY
#define LED_FADE_DELAY 20
#endif
#ifndef LED_FADE_AMOUNT
#define LED_FADE_AMOUNT 5
#endif
#ifndef LED_MAX_BRIGHTNESS
#define LED_MAX_BRIGHTNESS 125
#endif
#ifndef LED_MIN_BRIGHTNESS
#define LED_MIN_BRIGHTNESS 0
#endif

#endif
