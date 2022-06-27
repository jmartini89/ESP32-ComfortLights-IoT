#include "Definitions.h"

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <WiFi.h>

#include <string>

#include <HCSR04.h>
#include <BH1750.h>

#include "Led.hpp"
#include "HCSR501.hpp"
#include "Smoothing.hpp"
#include "StateTimer.hpp"

BluetoothSerial SerialBT;
BH1750 lightSensor;
UltraSonicDistanceSensor distanceSensor(23, 22);
PIR motionSensor(16);
Led led(26);

byte indexData = 0; // STATIC IN-FUNCTION?
Smoothing lightData(0, SENSOR_LIGHT_TRIGGER), distanceData(0, SENSOR_DISTANCE_TRIGGER);
StateTimer manual(0), security(SECURITY_TIMEOUT), wifiIsConnecting(1000);

std::string messageBT; // STATIC IN-FUNCTION?
std::string wifiSSID;
std::string wifiPass;

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

void autoLightControl() {
  lightData.addData(lightSensor.readLightLevel());
  distanceData.addData(distanceSensor.measureDistanceCm());

  if (indexData < 9) {
    ++indexData;
    return;
  }

  indexData = 0;
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
    Serial.println();
  }
}

void wifiHandler() {
  if (!wifiSSID.length() || !wifiPass.length())
    return;

  if (WiFi.status() == WL_CONNECTED && !wifiIsConnecting.status())
    return;

  if (WiFi.status() == WL_CONNECTED && wifiIsConnecting.status()) {
    wifiIsConnecting.setState(false);
    Serial.println("WiFi connected!");
    return;
  }

  if (WiFi.status() == WL_CONNECTED || !wifiIsConnecting.timeOut())
    return;

  static bool firstConnection = true;
  if (!wifiIsConnecting.status()) {
    if (firstConnection) {
      WiFi.begin(wifiSSID.c_str(), wifiPass.c_str());
      firstConnection = false;
    }
    else
      WiFi.reconnect();
    wifiIsConnecting.setState(true);
    Serial.println("WiFi connecting...");
  }

  wifiIsConnecting.timerUpdate();
}

void bluetoothHandler() {
  byte byteRead = SerialBT.read();
  if (byteRead != '\n') {
    messageBT += byteRead;
    return;
  }
  if (messageBT.length() < 2)
    return;

  if (messageBT[0] == BT_MSG_LED) {
    if (messageBT[1] == 0x1) {
      manual.setState(true);
      led.fadeIn();
    }
    else if (messageBT[1] == 0x0) {
      if (led.autoStatus()) {
        security.setState(true);
        security.timerUpdate();
        motionSensor.reset();
      }
      manual.setState(false);
      led.fadeOut();        
    }
  }
  else if (messageBT[0] == BT_MSG_WIFI_SSID) {
    wifiSSID = messageBT.substr(1);
    if (wifiSSID.length() > WIFI_SSID_LENGHT)
      wifiSSID.clear();
  }
  else if (messageBT[0] == BT_MSG_WIFI_PASS) {
    wifiPass = messageBT.substr(1);
    if (wifiPass.length() > WIFI_PASS_LENGHT)
      wifiPass.clear();
  }
  messageBT.clear();
}

void loop () {
  if (security.status() && security.timeOut())
    security.setState(false);

  if (!manual.status() && !security.status())
    autoLightControl();

  if (SerialBT.available())
    bluetoothHandler();
  wifiHandler();

  led.run();
  delay(10);
}
