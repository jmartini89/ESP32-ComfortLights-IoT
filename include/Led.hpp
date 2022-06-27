/*
 Generic led library with async-like timer implementation for PWM control
*/

#ifndef LED_HPP
#define LED_HPP

#include <Arduino.h>
#include "Definitions.h"
#include "StateTimer.hpp"

class Led {
  private:
    byte const  _pin;
    int         _brightness;
    StateTimer  _automatic;
    StateTimer  _async;
    bool        _fadeInOut;
    void        _setFadeAsync(bool const inOut, int const delay);
    void        _fadeAsync();
  public:
    Led(byte const pin);
    ~Led();
    void on() const;
    void off() const;
    void fadeInBlocking();
    void fadeOutBlocking();
    void fadeIn(bool automatic = false, int const delay = LED_FADE_DELAY);
    void fadeOut(int const delay = LED_FADE_DELAY);
    void run();
    bool autoStatus() const;
};

Led::Led(byte const pin)
  :_pin(pin)
  ,_brightness(0)
  ,_automatic(LED_TIMEOUT)
  ,_async(LED_FADE_DELAY)
{
  pinMode(this->_pin, OUTPUT);
}

Led::~Led() {}

void Led::on() const {
  digitalWrite(this->_pin, HIGH);
}

void Led::off() const {
  digitalWrite(this->_pin, LOW);
}

void Led::fadeInBlocking() {
  for (; this->_brightness <= LED_MAX_BRIGHTNESS; this->_brightness += LED_FADE_AMOUNT) {
    analogWrite(this->_pin, this->_brightness);
    delay(LED_FADE_DELAY);
  }
}

void Led::fadeOutBlocking() {
  for (; this->_brightness >= LED_MIN_BRIGHTNESS; this->_brightness -= LED_FADE_AMOUNT) {
    analogWrite(this->_pin, this->_brightness);
    delay(LED_FADE_DELAY);
  }
}

void Led::_setFadeAsync(bool const inOut, int const delay) {
  if (this->_async.status() && inOut == this->_fadeInOut)
    return;
  this->_fadeInOut = inOut;
  this->_async.setState(true);
  this->_async.timerUpdate();
  this->_async.setDelay(delay);
}

void Led::fadeIn(bool automatic, int const delay) {
  this->_setFadeAsync(true, delay);
  this->_automatic.setState(automatic);
  if (automatic)
    this->_automatic.timerUpdate();
}

void Led::fadeOut(int const delay) {
  this->_setFadeAsync(false, delay);
}

void Led::_fadeAsync() {
  if (!this->_async.status() || !this->_async.timeOut())
    return;

  if (_fadeInOut) {
    if (this->_brightness <= LED_MAX_BRIGHTNESS)
      this->_brightness += LED_FADE_AMOUNT;
  }
  else {
    if (this->_brightness >= LED_MIN_BRIGHTNESS)
      this->_brightness -= LED_FADE_AMOUNT;
  }

  if (this->_brightness < LED_MIN_BRIGHTNESS
      || this->_brightness > LED_MAX_BRIGHTNESS)
    this->_async.setState(false);

  this->_async.timerUpdate();
  analogWrite(this->_pin, this->_brightness);
}

void Led::run() {
  this->_fadeAsync();
  if (this->_automatic.status() && this->_automatic.timeOut()) {
    this->fadeOut();
    this->_automatic.setState(false);
  }
}

bool Led::autoStatus() const {
  return this->_automatic.status();
}

#endif
