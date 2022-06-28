/*
 Passive Infrared Sensor HC-SR501 library
 Hacky workaround for hardware delays and weird behaviours of the unit:
  keeps a virtual HIGH (motion detected) state for TIMEOUT delay
  to be used as main trigger for further sensors reads (E.g. ultrasonic sensor)
*/

#ifndef HCSR501_HPP
#define HCSR501_HPP

#include <Arduino.h>
#include "Definitions.h"
#include "StateTimer.hpp"

class PIR {
  private:
    byte const  _pin;
    StateTimer  _state;
  public:
    PIR(byte const pin);
    ~PIR();
    /**
     * New sensor read if current state is LOW and TIMEOUT expired
     * Updates internal state and timer
     */
    void update();
    /**
     * Resets internal state and timer
     */
    void reset();
    /**
     * Gets internal state
     */
    bool status() const;
};

PIR::PIR(byte const pin)
  :_pin(pin)
  ,_state(SENSOR_MOTION_TIMEOUT)
{
  pinMode(this->_pin, INPUT);
}

PIR::~PIR() {}

void PIR::update() {
  if (!this->_state.status()) {
    this->_state.setState(digitalRead(this->_pin));
    if (this->_state.status())
      this->_state.timerUpdate();
  }
  if (this->_state.status() && this->_state.timeOut())
    this->_state.setState(false);
}

void PIR::reset() {
  this->_state.setState(false);
  this->_state.timerReset();
}

bool PIR::status() const {
  return this->_state.status();
}

#endif
