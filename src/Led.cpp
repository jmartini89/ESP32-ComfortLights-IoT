#include "Led.hpp"

Led::Led(byte const pin)
  :_pin(pin)
  ,_manual(false)
  ,_brightCurr(0)
  ,_brightTarget(0)
  ,_STAsync(LED_FADE_DELAY)
  ,_STAutomatic(LED_AUTO_TIMEOUT)
{
  pinMode(this->_pin, OUTPUT);
}

Led::~Led() {}

uint Led::getBrightnessTarget() const {
  return this->_brightTarget;
}

void Led::setBrightnessTarget(uint brightness) {
  if (brightness > LED_MAX_BRIGHTNESS)
    brightness = LED_MAX_BRIGHTNESS / LED_FADE_AMOUNT * LED_FADE_AMOUNT;
  if (brightness < LED_FADE_AMOUNT)
    brightness = LED_FADE_AMOUNT;
  this->_brightTarget = brightness / LED_FADE_AMOUNT * LED_FADE_AMOUNT;
}

void Led::instantOn() {
  this->_brightCurr = 255;
  analogWrite(this->_pin, this->_brightCurr);
}

void Led::instantOff() {
  this->_brightCurr = 0;
  analogWrite(this->_pin, this->_brightCurr);
}

void Led::blinkBlocking(uint const time) {
  this->instantOn();
  delay(time);
  this->instantOff();
  delay(time);
}

void Led::fadeSwitch() {
  if (this->_brightTarget)
    this->setBrightnessTarget(0);
  else // to be removed
    this->setBrightnessTarget(LED_MAX_BRIGHTNESS);
  this->fadeQueue();
}

void Led::fadeQueue(bool const automatic) {
  this->_STAsync.setState(true);
  this->_STAsync.timerUpdate();

  if (automatic && !this->_manual) {
    this->_STAutomatic.setState(true);
    this->_STAutomatic.timerUpdate();
    this->_manual = false;
    return;
  }

  this->_STAutomatic.setState(false);
  this->_manual = true;
}

void Led::run() {
  if (this->_STAutomatic.status() && this->_STAutomatic.timeOut()) {
    this->setBrightnessTarget(0);
    this->fadeQueue();
  }

  if (!this->_STAsync.status() || !this->_STAsync.timeOut())
    return;

  if (this->_brightCurr < this->_brightTarget)
    this->_brightCurr += LED_FADE_AMOUNT;
  else if (this->_brightCurr > this->_brightTarget)
    this->_brightCurr -= LED_FADE_AMOUNT;

  if (this->_brightCurr == this->_brightTarget) {
    this->_STAsync.setState(false);
    this->_manual = false;
  }

  this->_STAsync.timerUpdate();
  analogWrite(this->_pin, this->_brightCurr);
}
