#ifndef SMOOTHING_HPP
#define SMOOTHING_HPP

#include <Arduino.h>

template <class Data>
class Smoothing {
private:
  byte  _index;
  byte  _indexSize;
  int   _limits[2];
  Data* _readings;
public:
  Smoothing(int const minRange, int const maxRange, byte const indexCount) {
    this->_index = 0;
    this->_indexSize = indexCount;
    this->_limits[0] = minRange;
    this->_limits[1] = maxRange;
    this->_readings = new Data[indexCount];
    bzero(this->_readings, sizeof(*this->_readings) * indexCount);
  }

  Smoothing(Smoothing const & src) {
    *this = src;
  }

  ~Smoothing() {
    delete [] this->_readings;
  }

  Smoothing<Data> & operator=(Smoothing<Data> const & rhs) {
    if (this != &rhs) {
      this->_index = rhs._index;
      this->_indexSize = rhs._indexSize;
      this->_limits[0] = rhs._limits[0];
      this->_limits[1] = rhs._limits[1];
      this->_readings = new Data[this->_indexSize];
      for (byte i = 0; i < this->_indexSize; i++) {
        this->_readings[i] = rhs._readings[i];
      }
    }
    return *this;
  }

  void addData(Data const data) {
    if (this->_index > (this->_indexSize - 1))
      this->_index = 0;
    this->_readings[this->_index] = data;
    ++this->_index;
  }

  Data getAverage() const {
    Data total = 0;
    for (byte i = 0; i <= this->_index; i++)
      total += this->_readings[i];
    return (total / (this->_index + 1));
  }

  bool isInRange() const {
    Data data = this->getAverage();
    return (data >= this->_limits[0] && data <= this->_limits[1]);
  }

  byte getIndex() const {
    return this->_index;
  }
};

#endif
