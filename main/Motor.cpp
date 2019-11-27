#ifndef __MOTOR_INCLUDED__
#define __MOTOR_INCLUDED__

#include <Arduino.h>

class Motor {
  private:
    const int directionPin;
    const int stepPin;

  public:
    Motor(int dir, int stp): directionPin(dir), stepPin(stp) { }

    void setupPins() {
      pinMode(this->directionPin, OUTPUT);
      pinMode(this->stepPin, OUTPUT);
    }

    void setDirection(bool clockwise) {
      digitalWrite(this->directionPin, clockwise);
    }

    void move() {
      digitalWrite(this->stepPin, HIGH);
      digitalWrite(this->stepPin, LOW);
    }

    int getDirectionPin() {
      return directionPin;
    }

    int getStepPin() {
      return stepPin;
    }
};

#endif
