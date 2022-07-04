#ifndef STATE_TIMER_HPP
#define STATE_TIMER_HPP

#include <Arduino.h>

class StateTimer {
private:
  ulong _start;
  ulong _delay;
  bool  _state;
public:
  StateTimer(ulong const delay, bool const state = false)
    :_start(0)
    ,_delay(delay)
    ,_state(state)
  {}

  ~StateTimer() {}

  bool status() const {
    return this->_state;
  }

  bool timeOut() const {
    return ((millis() - this->_start) > this->_delay);
  }

  /**
   * Sets internal timer delay for timeOut
   */
  void setDelay(ulong const delay) {
    this->_delay = delay;
  }

  /**
   * Sets internal timer to current time
   */ 
  void timerUpdate() {
    this->_start = millis();
  }

  /**
   * Sets internal timer to 0
   */
  void timerReset() {
    this->_start = 0;
  }


  void setState(bool const state) {
    this->_state = state;
  }

  void switchState() {
    this->_state = !this->_state;
  }
};

#endif
