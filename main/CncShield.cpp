#include <Arduino.h>
#include "Motor.cpp"

/*
   CNC shield V3 + A4988 drivers + Nema motors
   GRBL Pin Layout
*/
class CncShield {
  private:
    const int delayMicrosecondsPerStep = 200;
    const int stepperEnabledPin = 8; //Stepper Enable
    Motor motorX;
    Motor motorY;

  public:
    CncShield(): motorX(5, 2), motorY(6, 3) { }

    void setupPins() {
      pinMode(this->stepperEnabledPin, OUTPUT);

      this->disable();
      this->motorX.setupPins();
      this->motorY.setupPins();
    }

    void enable() {
      digitalWrite(this->stepperEnabledPin, LOW);
    }

    void disable() {
      digitalWrite(this->stepperEnabledPin, HIGH);
    }

    void step(boolean clockwiseX, boolean clockwiseY, int numberOfSteps) {
      this->motorX.setDirection(clockwiseX);
      this->motorY.setDirection(clockwiseY);

      for (float i = 0; i < numberOfSteps; i++)
      {
        digitalWrite(this->motorX.getStepPin(), HIGH);
        digitalWrite(this->motorY.getStepPin(), HIGH);
        delayMicroseconds(delayMicrosecondsPerStep);
        digitalWrite(this->motorX.getStepPin(), LOW);
        digitalWrite(this->motorY.getStepPin(), LOW);
        delayMicroseconds(delayMicrosecondsPerStep);
      }
    }

    void step(boolean clockwiseX, int numberOfStepsX, boolean clockwiseY, int numberOfStepsY) {
      this->motorX.setDirection(clockwiseX);
      this->motorY.setDirection(clockwiseY);

      float totalSteps = (numberOfStepsX > numberOfStepsY) ? numberOfStepsX : numberOfStepsY;

      if (totalSteps > 0) {
        float ratioX = ((float)numberOfStepsX / totalSteps);
        float ratioY = ((float)numberOfStepsY / totalSteps);

        float countToRatioX = 0;
        float countToRatioY = 0;

        for (float i = 0; i < totalSteps; i++)
        {
          countToRatioX++;
          countToRatioY++;

          delayMicroseconds(delayMicrosecondsPerStep);

          if (countToRatioX * ratioX >= 1) {
            this->motorX.move();
            countToRatioX = 0;
          }

          if (countToRatioY * ratioY >= 1) {
            this->motorY.move();
            countToRatioY = 0;
          }
        }
      }
    }
};
