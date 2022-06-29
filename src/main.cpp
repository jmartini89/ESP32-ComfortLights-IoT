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
  if(wm.autoConnect("SmartLights", "esp32test"))
    Serial.println("WiFi connected");
  else
    Serial.println("WiFi configportal running");
  Serial.println("ESP32 running");
}

void manualLightControl() {
  static bool manual = false;

  if (led.autoStatus() || manual) {
    led.fadeOut();
    manual = false;
    return;
  }
  led.fadeIn();
  manual = true;
}

void touch() {
  static ulong timer;
  static bool state, prevState;

  state = (touchRead(13) < 20);
  if (state == prevState)
    return;

  // led blink for security or reset
  // if (state && (state == prevState))
  //   ;

  if (state)
    timer = millis();
  else {
    ulong timeDiff = (millis() - timer);
    if (timeDiff >= 100 && timeDiff < LED_TOGGLE)
      manualLightControl();
    else if (timeDiff >= LED_TOGGLE && timeDiff < SENSORS_SECURITY) {
      security = !security;
      led.on(); delay(100); led.fadeOutBlocking();
    }
    else if (timeDiff >= SENSORS_SECURITY) {
      Serial.println("RESET");
      wm.resetSettings();
      for (byte i = 0; i <= 10; i++) {
        led.on(); delay(100); led.off(); delay(100);
      }
      ESP.restart();
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
