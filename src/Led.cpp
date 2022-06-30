#include "Led.hpp"

Led::Led(byte const pin)
  :_pin(pin)
  ,_brightness(0)
  ,_STAutomatic(LED_AUTO_TIMEOUT)
  ,_STAsync(LED_FADE_DELAY)
  ,_manual(false)
{
  pinMode(this->_pin, OUTPUT);
}

Led::~Led() {}

void Led::_setFadeAsync(bool const fadeDirection, byte const delay) {
  if (this->_STAsync.status() && fadeDirection == this->_fadeDirection)
    return;
  this->_fadeDirection = fadeDirection;
  this->_STAsync.setState(true);
  this->_STAsync.timerUpdate();
  this->_STAsync.setDelay(delay);
}

void Led::_runFadeAsync() {
  if (!this->_STAsync.status() || !this->_STAsync.timeOut())
    return;

  if (this->_fadeDirection) {
    if (this->_brightness <= LED_MAX_BRIGHTNESS)
      this->_brightness += LED_FADE_AMOUNT;
  }
  else {
    if (this->_brightness >= LED_MIN_BRIGHTNESS)
      this->_brightness -= LED_FADE_AMOUNT;
  }

  if (this->_brightness < LED_MIN_BRIGHTNESS
      || this->_brightness > LED_MAX_BRIGHTNESS)
    this->_STAsync.setState(false);

  this->_STAsync.timerUpdate();
  analogWrite(this->_pin, this->_brightness);
}

void Led::toggleOn() {
  this->_brightness = 255;
  analogWrite(this->_pin, this->_brightness);
}

void Led::toggleOff() {
  this->_brightness = 0;
  analogWrite(this->_pin, this->_brightness);
}

void Led::blinkBlocking(int const time) {
  this->toggleOn();
  delay(time);
  this->toggleOff();
  delay(time);
}

void Led::fadeIn(bool const automatic, int const delay, int const stepDelay) {
  this->_setFadeAsync(true, stepDelay);
  this->_STAutomatic.setState(automatic);
  if (automatic) {
    this->_STAutomatic.timerUpdate();
    this->_STAutomatic.setDelay(delay);
  }
}

void Led::fadeOut(int const stepDelay) {
  this->_setFadeAsync(false, stepDelay);
  this->_STAutomatic.setState(false);
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

void Led::fadeSwitch() {
  if (this->getAutoStatus() || this->_manual) {
    this->fadeOut();
    this->_manual = false;
    return;
  }
  this->fadeIn();
  this->_manual = true;
}

void Led::setAutoDelay(int const delay) {
  this->_STAutomatic.setDelay(delay);
}

bool Led::getAutoStatus() const {
  return this->_STAutomatic.status();
}

void Led::run() {
  this->_runFadeAsync();
  if (this->_STAutomatic.status() && this->_STAutomatic.timeOut()) {
    this->fadeOut();
    this->_STAutomatic.setState(false);
  }
}

