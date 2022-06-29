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

void setup () {
  Serial.begin(BAUD_RATE);
  initSensors(led, lightSensor);
  WiFi.mode(WIFI_STA);
  if (CONFIG_RESET)
    wm.resetSettings();
  wm.setConfigPortalBlocking(false);
  if(wm.autoConnect("SmartLights", "esp32test"))
    Serial.println("WiFi connected");
  else
    Serial.println("WiFi configportal running");
  Serial.println("ESP32 running");
}

void manualLightControl() {
  StateTimer manual(0);

  if (led.autoStatus() || manual.status()) {
    led.fadeOut();
    manual.setState(false);
    return;
  }
  led.fadeIn();
  manual.setState(true);
}

void touch() {
  static Smoothing<touch_value_t> touchData(0, 20, 5);
  static StateTimer touch(500);

  touchData.addData(touchRead(13));

  if (touchData.getIndex() < 4)
    return;

  if (touchData.isInRange() && touch.timeOut()) {
    touch.timerUpdate();
    manualLightControl();
  }
}

void sensors() {
  static Smoothing<float> lightData(0, SENSOR_LIGHT_TRIGGER, 10);
  static Smoothing<float> distanceData(0, SENSOR_DISTANCE_TRIGGER, 10);

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
