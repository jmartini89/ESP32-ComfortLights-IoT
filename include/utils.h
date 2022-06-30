#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <BH1750.h>
#include <WiFiManager.h>
#include "definitions.h"
#include "Led.hpp"

void initSensors(Led & led, BH1750 & lightSensor);
void debugSensors(float const lux, float const distance, bool const motion);
void manualReset(Led & led, WiFiManager & wm);

#endif
