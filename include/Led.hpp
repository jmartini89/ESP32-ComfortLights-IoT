#ifndef LED_HPP
#define LED_HPP

#include <Arduino.h>
#include "StateTimer.hpp"
#include "definitions.h"

class Led {
  private:
    byte const _pin;
    bool _manual;
    int _brightCurr;
    uint _brightTarget;
    StateTimer _STAsync;
    StateTimer _STAutomatic;
  public:
    Led(byte const pin);
    ~Led();

    uint getBrightnessTarget() const;
    void setBrightnessTarget(uint brightness);

    void instantOn();
    void instantOff();
    void blinkBlocking(uint const time);
    void fadeSwitch();
    void fadeQueue(bool const automatic = false);

    void run();
};

#endif
