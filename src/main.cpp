#include <Arduino.h>
#include <HCSR04.h>
#include <BH1750.h>
#include <WiFiManager.h>
#include "definitions.h"
#include "Led.hpp"
#include "HCSR501.hpp"
#include "Smoothing.hpp"
#include "StateTimer.hpp"
#include "utils.h"

WiFiManager wm;
BH1750 lightSensor;
UltraSonicDistanceSensor distanceSensor(23, 22);
PIR motionSensor(16);
Led led(26);
bool security = false;

void setup () {
  Serial.begin(BAUD_RATE);
  initSensors(led, lightSensor);
  WiFi.mode(WIFI_STA);
  wm.setConfigPortalBlocking(false);
  wm.setConfigPortalTimeout(180);
  if (wm.autoConnect("SmartLights", "esp32test"))
    Serial.println("WiFi connected");
  else
    Serial.println("WiFi configportal running");
  Serial.println("ESP32 running");
}

byte touchSettingsTimer(ulong const timer) {
  ulong timeDiff = (millis() - timer);
  if (timeDiff < 100)
    return 0;
  else if (timeDiff < SENSORS_SECURITY)
    return 1;
  else if (timeDiff >= SENSORS_SECURITY && timeDiff < 10000)
    return 2;
  else
    return 3;
}

void touch() {
  static ulong timer;
  static bool state, prevState, settings;

  state = (touchRead(13) < 20);
  if (state != prevState) {
    if (state) {
      settings = true;
      timer = millis();
    }
    else {
      settings = false;
      switch (touchSettingsTimer(timer)) {
        case 1: led.fadeSwitch(); break;
        case 2: security = !security; break;
        default: break;
      }
    }
  }
  if (settings && state && state == prevState) {
    switch (touchSettingsTimer(timer)) {
      case 2: led.blinkBlocking(100); break;
      case 3: manualReset(led, wm); break;
      default: break;
    }
  }
  prevState = state;
}

void sensors() {
  static Smoothing<float> lightData(0, SENSOR_LIGHT_TRIGGER, 10);
  static Smoothing<float> distanceData(0, SENSOR_DISTANCE_TRIGGER, 10);

  if (security)
    return;

  lightData.addData(lightSensor.readLightLevel());
  distanceData.addData(distanceSensor.measureDistanceCm());

  if (lightData.getIndex() < 9)
    return;

  motionSensor.update();
  if (lightData.isInRange()
      && motionSensor.status()
      && distanceData.isInRange())
    led.fadeIn(true);

  if (DEBUG_SENSORS)
    debugSensors(lightData.getAverage(),
      distanceData.getAverage(),
      motionSensor.status());
}

void loop () {
  wm.process();
  touch();
  sensors();
  led.run();
  delay(10);
}
