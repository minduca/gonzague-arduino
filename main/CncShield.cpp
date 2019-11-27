#ifndef __CNC_SHIELD_INCLUDED__
#define __CNC_SHIELD_INCLUDED__

#include <Arduino.h>
#include "Motor.cpp"
#include "Pins.cpp"

class CncShield {
  private:
    const int delayMicrosecondsPerStep = 200;
    Motor motorX;
    Motor motorY;

  public:
    CncShield(): motorX(STEP_DIRECTION_X_PIN, STEP_PULSE_X_PIN), motorY(STEP_DIRECTION_Y_PIN, STEP_PULSE_Y_PIN) { }

    void setupPins() {
      pinMode(STEPPER_ENABLE_PIN, OUTPUT);

      this->disable();
      this->motorX.setupPins();
      this->motorY.setupPins();
    }

    void enable() {
      digitalWrite(STEPPER_ENABLE_PIN, LOW);
    }

    void disable() {
      digitalWrite(STEPPER_ENABLE_PIN, HIGH);
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

#endif
