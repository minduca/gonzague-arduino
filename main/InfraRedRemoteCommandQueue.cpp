#ifndef __INFRA_RED_REMOTE_COMMAND_QUEUE_INCLUDED__
#define __INFRA_RED_REMOTE_COMMAND_QUEUE_INCLUDED__

#include <IRremote.h>
#include "Command.cpp"
#include "Pins.cpp"

/*
  HX1838 VS1838 IR Remote
*/
#define IR_UP_HEX 0xFF629D
#define IR_DOWN_HEX 0xFFA857
#define IR_LEFT_HEX 0xFF22DD
#define IR_RIGHT_HEX 0xFFC23D
#define IR_REPEAT_HEX 0xFFFFFFFF

class InfraRedRemoteCommandQueue {
  private:
    const int defaultCmDistance;
    const int defaultDegreeAngle;
    IRrecv irrecv;
    decode_results results;
    long unsigned lastActionCode;

  public:
    InfraRedRemoteCommandQueue(int defaultCm, int defaultAngle): defaultCmDistance(defaultCm), defaultDegreeAngle(defaultAngle), irrecv(IR_RECEIVER_PIN), lastActionCode(0) { }

    void setup() {
      irrecv.enableIRIn();
      irrecv.blink13(true);
    }

    Command *tryDequeCommand() {
      Command *command = NULL;
      if (irrecv.decode(&results)) {
        unsigned long actionCode = results.value;
        this->print(actionCode);
        command = this->tryDequeCommand(actionCode);

        if (actionCode != IR_REPEAT_HEX) {
          this->lastActionCode = command ? actionCode : 0;
        }

        irrecv.resume();
      }
      return command;
    }

  private:
    void print(unsigned long actionCode) {
      Serial.print(actionCode, HEX);
      Serial.print(" latest:");
      Serial.println(this->lastActionCode);
    }

    Command *tryDequeCommand(unsigned long actionCode) {
      Command *command = NULL;
      switch (actionCode) {
        case IR_UP_HEX:
          command = new MoveCommand(this->defaultCmDistance, Forward);
          break;
        case IR_DOWN_HEX:
          command = new MoveCommand(this->defaultCmDistance, Backward);
          break;
        case IR_LEFT_HEX:
          command = new TurnCommand(this->defaultDegreeAngle, Left);
          break;
        case IR_RIGHT_HEX:
          command = new TurnCommand(this->defaultDegreeAngle, Right);
          break;
        case IR_REPEAT_HEX:
          if (this->lastActionCode > 0) {
            command = this->tryDequeCommand(this->lastActionCode);
          }
          break;
      }
      return command;
    }
};

#endif
