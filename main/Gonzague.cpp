#include <Arduino.h>
#include "CncShield.cpp"

class Gonzague {
  private:
    CncShield cnc;
    
    const int oneCmSteps = 300;
    const int oneDegreeSteps = 88;

  public:
    Gonzague(): cnc() { }

    void setup() {
      this->cnc.setupPins();
    }

    void wakeUp() {
      this->cnc.enable();
    }

    void sleep() {
      this->cnc.disable();
    }

    void moveForward(int cmDistance) {
      int steps = this->toCmSteps(cmDistance);
      this->cnc.step(true, true, steps);
    }

    void moveBack(int cmDistance) {
      int steps = this->toCmSteps(cmDistance);
      this->cnc.step(false, false, steps);
    }

    void turnLeft(int angle) {
      int steps = toAngleSteps(angle);
      this->cnc.step(false, true, steps);
    }

    void turnRight(int angle) {
      int steps = toAngleSteps(angle);
      this->cnc.step(true, false, steps);
    }

  private:
    int toCmSteps(int cmDistance) {
      return cmDistance * oneCmSteps;
    }

    int toAngleSteps(int angle) {
      return angle * oneDegreeSteps;
    }
};
