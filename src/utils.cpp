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

void initwm(WiFiManager & wm, wmParameters & wmParams, void (*f)(void)) {
  WiFi.mode(WIFI_STA);
  wm.addParameter(&wmParams.address);
  wm.addParameter(&wmParams.id);
  wm.addParameter(&wmParams.user);
  wm.addParameter(&wmParams.pass);
  wm.addParameter(&wmParams.topic);
  wm.setSaveParamsCallback(f);
  wm.setConfigPortalBlocking(false);
  wm.setConfigPortalTimeout(180);
  wm.autoConnect(WM_CONF_NAME, WM_CONF_PASS);
}

void manualReset(Led & led, WiFiManager & wm, Preferences & preferences) {
  Serial.println("RESET");
  led.instantOn(); delay(2500);
  preferences.clear();
  preferences.end();
  wm.resetSettings();
  ESP.restart();
}

void mqttConnect(MQTTClient & mqtt, Preferences & preferences, std::string & mqttTopicIdStatus) {
  static ulong lastMillis = 0;

  if (mqtt.connected() || ((millis() - lastMillis) < 5000)
    || !preferences.isKey("address") || preferences.getString("address", "").isEmpty())
    return;
  lastMillis = millis();

  if (!WiFi.isConnected())
    return;

  mqtt.setHost(preferences.getString("address", "").c_str());

  Serial.println("MQTT connecting...");
  if (!mqtt.connect(
    preferences.getString("id", "").c_str(),
    preferences.getString("user", "").c_str(),
    preferences.getString("pass", "").c_str()))
    return;

  Serial.println("MQTT connected!");

  std::string topicId;
  topicId = preferences.getString("topic", "").c_str();
  topicId += "/";
  topicId += preferences.getString("id", "").c_str();
  mqttTopicIdStatus = topicId + "/status";

  Serial.printf("Topic:%s\n", preferences.getString("topic", "").c_str());
  Serial.printf("TopicId:%s\n", topicId.c_str());
  Serial.printf("TopicIdStatus:%s\n", mqttTopicIdStatus.c_str());

  mqtt.subscribe(preferences.getString("topic", "").c_str());
  mqtt.subscribe(topicId.c_str());
}

void debugSensors(float const lux, float const distance, bool const motion) {
  if (!DEBUG_SENSORS)
    return;
  Serial.print("SENSORS: ");
  Serial.print(lux); Serial.print(" lx");
  Serial.print(" | ");
  Serial.print(distance); Serial.print(" cm");
  Serial.print(" | ");
  Serial.print("motion "); Serial.print(motion);
  Serial.println();
}
