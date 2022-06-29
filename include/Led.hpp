/*
 Generic led library with async-like (non-blocking) timer implementation for PWM control
 Optional automatic mode for timer-driven toggle
*/

#ifndef LED_HPP
#define LED_HPP

#include <Arduino.h>
#include "StateTimer.hpp"
#include "definitions.h"

class Led {
  private:
    byte const  _pin;
    int         _brightness;
    StateTimer  _automatic;
    StateTimer  _async;
    bool        _fadeInOut;
    void        _setFadeAsync(bool const inOut, int const delay);
    void        _runFadeAsync();
  public:
    Led(byte const pin);
    ~Led();
    void on() const;
    void off() const;
    /**
     * Non-blocking PWM fade with optional execution timer
     * @param automatic enables automatic toggle based on LED_AUTO_TIMEOUT timer delay
     * @param delay timer for each BRIGHTNESS step
     */
    void fadeIn(bool automatic = false, int const delay = LED_FADE_DELAY);
    void fadeOut(int const delay = LED_FADE_DELAY);
    bool autoStatus() const;
    void run();
};

#endif
