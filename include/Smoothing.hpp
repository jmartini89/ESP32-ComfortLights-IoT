#include <Arduino.h>

class Smoothing {
  private:
    byte  _index;
    byte  _limits[2];
    float _readings[10];
  public:
    Smoothing(byte const min, byte const max);
    ~Smoothing();
    void addData(float const data);
    float getAverage() const;
    bool isInRange() const;
};

Smoothing::Smoothing(byte const minRange, byte const maxRange) {
  this->_index = 0;
  this->_limits[0] = minRange;
  this->_limits[1] = maxRange;
  bzero(this->_readings, sizeof(*this->_readings) * 10);
}

Smoothing::~Smoothing() {}

void Smoothing::addData(float const data) {
  if (this->_index > 9)
    this->_index = 0;
  this->_readings[this->_index] = data;
  this->_index++;
}

float Smoothing::getAverage() const {
  float total = 0;
  for (byte i = 0; i <= this->_index; i++)
    total += this->_readings[i];
  return (total / (this->_index + 1));
}

bool Smoothing::isInRange() const {
  float data = this->getAverage();
  return (data >= this->_limits[0] && data <= this->_limits[1]);
}
