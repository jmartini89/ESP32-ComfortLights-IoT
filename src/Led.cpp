#include "Led.hpp"

Led::Led(byte const pin)
  :_pin(pin)
  ,_brightness(0)
  ,_automatic(LED_AUTO_TIMEOUT)
  ,_async(LED_FADE_DELAY)
{
  pinMode(this->_pin, OUTPUT);
}

Led::~Led() {}

void Led::_setFadeAsync(bool const inOut, int const delay) {
  if (this->_async.status() && inOut == this->_fadeInOut)
    return;
  this->_fadeInOut = inOut;
  this->_async.setState(true);
  this->_async.timerUpdate();
  this->_async.setDelay(delay);
}

void Led::_runFadeAsync() {
  if (!this->_async.status() || !this->_async.timeOut())
    return;

  if (this->_fadeInOut) {
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

void Led::on() {
  this->_brightness = 255;
  analogWrite(this->_pin, this->_brightness);
}

void Led::off() {
  this->_brightness = 0;
  analogWrite(this->_pin, this->_brightness);
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

void Led::fadeIn(bool automatic, int const delay) {
  this->_setFadeAsync(true, delay);
  this->_automatic.setState(automatic);
  if (automatic)
    this->_automatic.timerUpdate();
}

void Led::fadeOut(int const delay) {
  this->_setFadeAsync(false, delay);
  this->_automatic.setState(false);
}

bool Led::autoStatus() const {
  return this->_automatic.status();
}

void Led::run() {
  this->_runFadeAsync();
  if (this->_automatic.status() && this->_automatic.timeOut()) {
    this->fadeOut();
    this->_automatic.setState(false);
  }
}

