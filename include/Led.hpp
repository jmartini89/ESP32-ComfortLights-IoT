/*
 Generic PWM LED library with async-like (non-blocking) timer implementation for seamless fading
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
    StateTimer  _STAutomatic;
    StateTimer  _STAsync;
    bool        _fadeDirection;
    bool        _manual;
    void        _setFadeAsync(bool const fadeDirection, byte const delay);
    void        _runFadeAsync();
  public:
    Led(byte const pin);
    ~Led();

    void toggleOn();
    void toggleOff();
    void setBrightness(int const brightness);
    void blinkBlocking(int const time);

    /**
     * Non-blocking PWM fade with optional execution timer
     * @param automatic enables automatic toggle based on LED_AUTO_TIMEOUT timer delay
     * @param stepDelay timer for each BRIGHTNESS step
     */
    void fadeIn(bool const automatic = false, int const delay = LED_AUTO_TIMEOUT, int const stepDelay = LED_FADE_DELAY);
    void fadeOut(int const stepDelay = LED_FADE_DELAY);
    void fadeInBlocking();
    void fadeOutBlocking();
    void fadeSwitch();

    void setAutoDelay(int const delay);

    bool getAutoStatus() const;
    bool status() const;

    void run();
};

#endif
