#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <BH1750.h>
#include <WiFiManager.h>
#include <MQTT.h>
#include <Preferences.h>
#include <string>
#include "wmParameters.h"
#include "definitions.h"
#include "Led.hpp"

void initSensors(Led & led, BH1750 & lightSensor);
void initwm(WiFiManager & wm, wmParameters & wmParams, void (*f)(void));
void manualReset(Led & led, WiFiManager & wm, Preferences & preferences);
void mqttConnect(MQTTClient & mqtt, Preferences & preferences, std::string & mqttTopicIdStatus);
void debugSensors(float const lux, float const distance, bool const motion);

#endif
