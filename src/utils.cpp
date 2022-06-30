#include "utils.h"

void initSensors(Led & led, BH1750 & lightSensor) {
  if (!DEBUG_PIR_SKIP_INIT) {
    Serial.println("PIR Sensor init:");
    for (byte i = 0; i < 30; i++) {
      Serial.print("."); led.blinkBlocking(50); delay(1000);
    } Serial.println(" done!");
  }
  Serial.print("Light Sensor init:");
  Wire.begin(32, 33);
  while (!lightSensor.begin()) {
    Serial.print("."); led.blinkBlocking(50);
  } Serial.println(" done!");
}

void debugSensors(float const lux, float const distance, bool const motion) {
  Serial.print("SENSORS: ");
  Serial.print(lux); Serial.print(" lx");
  Serial.print(" | ");
  Serial.print(distance); Serial.print(" cm");
  Serial.print(" | ");
  Serial.print("motion "); Serial.print(motion);
  Serial.println();
}

void manualReset(Led & led, WiFiManager & wm) {
  Serial.println("RESET");
  led.toggleOn(); delay(2500);
  wm.resetSettings();
  ESP.restart();
}

// void bluetoothHandler() {
//   if (!SerialBT.available())
//     return;

//   static std::string message;
//   byte byteRead = SerialBT.read();
//   if (byteRead != '\n') {
//     message += byteRead;
//     return;
//   }
//   if (message.length() < 2) {
//     message.clear();
//     return;
//   }

//   switch (message[0]) {
//     case BT_MSG_LED:  manualLightControl(); break;
//     default:          Serial.println("ERROR: Bluetooth: BAD DATA"); break;
//   }

//   message.clear();
// }
