#include "utils.h"

void initSensors(Led const & led, BH1750 & lightSensor) {
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

void debugSensors(float const lux, float const distance, bool const motion) {
  Serial.print("SENSORS: ");
  Serial.print(lux); Serial.print(" lx");
  Serial.print(" | ");
  Serial.print(distance); Serial.print(" cm");
  Serial.print(" | ");
  Serial.print("motion "); Serial.print(motion);
  Serial.println();
}
