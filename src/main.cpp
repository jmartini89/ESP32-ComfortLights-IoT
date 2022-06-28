#include "Definitions.h"

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <string>
#include <HCSR04.h>
#include <BH1750.h>
#include "Led.hpp"
#include "HCSR501.hpp"
#include "Smoothing.hpp"
#include "StateTimer.hpp"
#include "WiFiHandler.hpp"

BluetoothSerial SerialBT;
WiFiHandler wifiHandler;

BH1750 lightSensor;
UltraSonicDistanceSensor distanceSensor(23, 22);
PIR motionSensor(16);
Led led(26);

StateTimer manual(0);

void initSensors() {
  if (!PIR_SKIP_INIT) {
    Serial.println("PIR Sensor init:");
    for (byte i = 0; i < 30; i++) {
      Serial.print("."); led.on(); delay(50); led.off(); delay(1000);
    } Serial.println(" done!");
  }
  Serial.print("Light Sensor init:");
  Wire.begin(32, 33);
  while (!lightSensor.begin()) {
    Serial.print("."); led.on(); delay(50); led.off(); delay(50);
  } Serial.println(" done!");
}

void setup () {
  Serial.begin(BAUD_RATE);
  initSensors();
  SerialBT.begin("ESP32test");
  Serial.println("ESP32 running");
}

void manualLightControl() {
  if (!manual.status())
    led.fadeIn();
  else
    led.fadeOut();
  manual.switchState();
}

void touch() {
  static StateTimer touch(500);
  static Smoothing<touch_value_t> touchData(0, 20, 10);
  touchData.addData(touchRead(13));

  if (touchData.getIndex() < 9)
    return;

  if (touchData.isInRange() && touch.timeOut()) {
    manualLightControl();
    touch.timerUpdate();
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

  if (SENSORS_DEBUG) {
    float lux = lightData.getAverage();
    float distance = distanceData.getAverage();
    Serial.print("SENSORS: ");
    Serial.print(distance); Serial.print(" cm");
    Serial.print(" | ");
    Serial.print(lux); Serial.print(" lx");
    Serial.print(" | ");
    Serial.print("motion "); Serial.print(motionSensor.status());
    Serial.println();
  }
}

void bluetoothHandler() {
  if (!SerialBT.available())
    return;

  static std::string message;
  byte byteRead = SerialBT.read();
  if (byteRead != '\n') {
    message += byteRead;
    return;
  }
  if (message.length() < 2) {
    message.clear();
    return;
  }

  switch (message[0]) {
    case BT_MSG_LED:        manualLightControl(); break;
    case BT_MSG_WIFI_SSID:  wifiHandler.setSSID(message.substr(1)); break;
    case BT_MSG_WIFI_PASS:  wifiHandler.setPassword(message.substr(1)); break;
    default:                Serial.println("ERROR: Bluetooth: BAD DATA"); break;
  }

  message.clear();
}

void loop () {
  touch();
  sensors();
  bluetoothHandler();
  wifiHandler.run();
  led.run();

  delay(10);
}
