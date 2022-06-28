#ifndef WIFI_HANDLER_HPP
#define WIFI_HANDLER_HPP

#include <Arduino.h>
#include <WiFi.h>
#include <string>
#include "definitions.h"
#include "StateTimer.hpp"

class WiFiHandler {
private:
  std::string _SSID;
  std::string _password;
  StateTimer  _isConnecting;
public:
  WiFiHandler() 
    :_isConnecting(1000)
  {}

  ~WiFiHandler() {}

  void setSSID(std::string const value) {
    if (!value.length() || value.length() > WIFI_SSID_LENGHT)
      return;
    this->_SSID = value;
  }

  void setPassword(std::string const value) {
    if (!value.length() || value.length() > WIFI_PASS_LENGHT)
      return;
    this->_password = value;
  }

  void run() {
    if (!this->_SSID.length() || !this->_password.length())
      return;

    //LOGGING
    if (WiFi.status() == WL_CONNECTED && !this->_isConnecting.status())
      return;
    if (WiFi.status() == WL_CONNECTED && this->_isConnecting.status()) {
      this->_isConnecting.setState(false);
      Serial.println("WiFi connected!");
      return;
    }

    //SPAM PREVENTION
    if (WiFi.status() == WL_CONNECTED || !this->_isConnecting.timeOut())
      return;

    static bool firstConnection = true;
    if (!this->_isConnecting.status()) {
      if (firstConnection) {
        WiFi.begin(this->_SSID.c_str(), this->_password.c_str());
        firstConnection = false;
      }
      else
        WiFi.reconnect();
      this->_isConnecting.setState(true);
      Serial.println("WiFi connecting...");
    }

    this->_isConnecting.timerUpdate();
  }
};

#endif
