/*
 StateTimer library:
  keeps track of state (boolean) with an handy timer for async-like operations
*/

#ifndef UTILS_HPP
#define UTILS_HPP

#include <Arduino.h>

class StateTimer {
  private:
    ulong _start;
    ulong _delay;
    bool  _state;
  public:
    StateTimer(ulong const delay, bool const state = false);
    ~StateTimer();
    bool status() const;
    bool timeOut() const;
    void setDelay(ulong const delay);
    void timerUpdate();
    void timerReset();
    void setState(bool const state);
    void switchState();
};

StateTimer::StateTimer(ulong const delay, bool const state) 
  :_start(0)
  ,_delay(delay)
  ,_state(state)
{}

StateTimer::~StateTimer() {}

bool StateTimer::status() const {
  return this->_state;
}

bool StateTimer::timeOut() const {
  // if (!this->_delay)
  //   return true;
  return ((millis() - this->_start) > this->_delay);
}

/**
 * Sets internal timer delay for timeOut
 */
void StateTimer::setDelay(ulong const delay) {
  this->_delay = delay;
}

/**
 * Sets internal timer to current time
 */ 
void StateTimer::timerUpdate() {
  this->_start = millis();
}

/**
 * Sets internal timer to 0
 */
void StateTimer::timerReset() {
  this->_start = 0;
}

void StateTimer::setState(bool const state) {
  this->_state = state;
}

void StateTimer::switchState() {
  this->_state = !this->_state;
}

#endif
