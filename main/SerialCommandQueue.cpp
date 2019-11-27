#ifndef __SERIAL_COMMAND_QUEUE_INCLUDED__
#define __SERIAL_COMMAND_QUEUE_INCLUDED__

#include "Command.cpp"

#define OP_END ';'
#define OP_FORWARD 'F'
#define OP_BACKWARD 'B'
#define OP_LEFT 'L'
#define OP_RIGHT 'R'

class SerialCommandQueue {
  private:
    const int defaultCmDistance;
    const int defaultDegreeAngle;
  
  public:
    SerialCommandQueue(int defaultCm, int defaultAngle): defaultCmDistance(defaultCm), defaultDegreeAngle(defaultAngle) { }

    void setup(int dataRate) {
      // opens serial port and sets data rate
      Serial.begin(dataRate);
    }

    bool hasNext() {
      return Serial.available() > 0;
    }

    Command *dequeCommand() {

      char op = Serial.read();
      int arg = Serial.parseInt(SKIP_NONE);

      Command *command = NULL;

      switch (op) {
        case OP_FORWARD:
          command = new MoveCommand(this->getCmOrDefault(arg), Forward);
          break;
        case OP_BACKWARD:
          command = new MoveCommand(this->getCmOrDefault(arg), Backward);
          break;
        case OP_LEFT:
          command = new TurnCommand(this->getAngleOrDefault(arg), Left);
          break;
        case OP_RIGHT:
          command = new TurnCommand(this->getAngleOrDefault(arg), Right);
          break;
        default:
          if (!isOpEnd(op)) {
            command = new UnknownOpCommand();
          }
          break;
      }

      return command;
    }

  private:
    int getCmOrDefault(int cmDistance) {
      return (cmDistance > 0) ? cmDistance : this->defaultCmDistance;
    }

    int getAngleOrDefault(int angle) {
      return (angle > 0) ? angle : this->defaultDegreeAngle;
    }

    bool isOpEnd(char opData) {
      return opData == OP_END || opData == '\n' || opData == '\r';
    }
};

#endif
