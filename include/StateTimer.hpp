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
    void setDelay(bool const delay);
    void timerUpdate();
    void timerReset();
    void setState(bool const state);
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
  return ((millis() - this->_start) > this->_delay);
}

void StateTimer::setState(bool const state) {
  this->_state = state;
}

/**
 * Sets internal timer delay for timeOut
 */
void StateTimer::setDelay(bool const delay) {
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

#endif
