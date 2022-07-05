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
std::string mqttTopicIdStatus;

bool sensorsSecurity = false;

/* --------------------------------- SENSORS -------------------------------- */

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

void sensorsRun() {
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

  debugSensors(lightData.getAverage(),
      distanceData.getAverage(),
      motionSensor.status());

  if (!(lightData.isInRange() && distanceData.isInRange()))
    return;

  led.setBrightnessTarget(LED_MAX_BRIGHTNESS);
  led.fadeQueue(true);
}

/* ---------------------------------- MQTT ---------------------------------- */

void mqttMessageReceived(String & topic, String & payload) {
  Serial.println("MQTT incoming: " + topic + " - " + payload);

  if (payload.isEmpty())
    return;

  if (std::isdigit(payload[0]))
    led.setBrightnessTarget(payload.toInt());
  else if (payload == "ON") {
    if (!led.getBrightnessTarget())
      led.setBrightnessTarget(LED_MAX_BRIGHTNESS);
    led.fadeQueue();
  }
  else if (payload == "OFF") {
    led.setBrightnessTarget(0);
    led.fadeQueue();
  }
  else if (payload == "SENSORS:ON")
    sensorsSecurity = false;
  else if (payload == "SENSORS:OFF")
    sensorsSecurity = true;
}

void mqttUpdateStatus() {
  static ulong timer;
  static unsigned int state, prevState;

  if ((millis() - timer) < 1000)
    return;
  timer = millis();

  state = led.getBrightnessTarget();
  if (state != prevState) {
    mqtt.publish(mqttTopicIdStatus.c_str(), std::to_string(state).c_str());
    if (state)
      mqtt.publish(mqttTopicIdStatus.c_str(), "ON");
    else
      mqtt.publish(mqttTopicIdStatus.c_str(), "OFF");
  }
  prevState = state;
}

/* ---------------------------------- SETUP --------------------------------- */

void wmSaveParamsCallback() {
  preferences.putString("address", wmParams.address.getValue());
  preferences.putString("id", wmParams.id.getValue());
  preferences.putString("user", wmParams.user.getValue());
  preferences.putString("pass", wmParams.pass.getValue());
  preferences.putString("topic", wmParams.topic.getValue());
}

void setup() {
  preferences.begin("mqtt");
  Serial.begin(BAUD_RATE);
  initSensors(led, lightSensor);
  initwm(wm, wmParams, wmSaveParamsCallback);
  mqtt.begin(net);
  mqtt.onMessage(mqttMessageReceived);
  Serial.println("ESP32 running");
}

/* ---------------------------------- LOOP ---------------------------------- */

void loop () {
  touch();
  sensorsRun();
  wm.process();
  mqttConnect(mqtt, preferences, mqttTopicIdStatus);
  mqtt.loop();
  mqttUpdateStatus();
  led.run();
  delay(10);
}
