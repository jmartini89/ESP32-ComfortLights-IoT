#include <Arduino.h>
#include <HCSR04.h>
#include <BH1750.h>
#include <WiFiManager.h>
#include <MQTT.h>
#include <Preferences.h>
#include "definitions.h"
#include "Led.hpp"
#include "HCSR501.hpp"
#include "Smoothing.hpp"
#include "StateTimer.hpp"
#include "utils.h"
#include "wmParameters.h"

BH1750 lightSensor;
UltraSonicDistanceSensor distanceSensor(23, 22);
PIR motionSensor(16);
Led led(26);

Preferences preferences;
WiFiManager wm;
WiFiClient net;
MQTTClient mqtt;
wmParameters wmParams;

bool sensorsSecurity = false;

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

  state = (touchRead(13) < 30);
  if (state != prevState) {
    if (state) {
      settings = true;
      timer = millis();
    }
    else {
      settings = false;
      switch (touchSettingsTimer(timer)) {
        case 1: led.fadeSwitch(); break;
        case 2: sensorsSecurity = !sensorsSecurity; break;
        default: break;
      }
    }
  }
  if (settings && state && state == prevState) {
    switch (touchSettingsTimer(timer)) {
      case 2: led.blinkBlocking(100); break;
      case 3: manualReset(led, wm, preferences); break;
      default: break;
    }
  }
  prevState = state;
}

void sensors() {
  static Smoothing<float> lightData(0, SENSOR_LIGHT_TRIGGER, 10);
  static Smoothing<float> distanceData(0, SENSOR_DISTANCE_TRIGGER, 10);

  if (sensorsSecurity)
    return;

  motionSensor.update();
  if (!motionSensor.status())
    return;

  lightData.addData(lightSensor.readLightLevel());
  distanceData.addData(distanceSensor.measureDistanceCm());

  if (lightData.getIndex() < 9)
    return;

  if (lightData.isInRange() && distanceData.isInRange())
    led.fadeIn(true);

  if (DEBUG_SENSORS)
    debugSensors(lightData.getAverage(),
      distanceData.getAverage(),
      motionSensor.status());
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}

void wmSaveParamsCallback() {
  preferences.putString("address", wmParams.address.getValue());
  preferences.putString("id", wmParams.id.getValue());
  preferences.putString("user", wmParams.user.getValue());
  preferences.putString("pass", wmParams.pass.getValue());
  preferences.putString("topic", wmParams.topic.getValue());
}

void setup () {
  preferences.begin("mqtt");
  Serial.begin(BAUD_RATE);
  initSensors(led, lightSensor);
  initwm(wm, wmParams, wmSaveParamsCallback);
  mqtt.begin(net);
  mqtt.onMessage(messageReceived);
  Serial.println("ESP32 running");
}

void loop () {
  touch();
  sensors();
  wm.process();
  mqttConnect(mqtt, preferences);
  mqtt.loop();
  led.run();
  delay(10);
}
