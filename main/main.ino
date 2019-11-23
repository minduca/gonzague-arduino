#include "Gonzague.cpp"

class CommandResult {
  public:
    virtual String getDescription() {
      return "";
    }
    virtual ~CommandResult() { }

    virtual void free() = 0;
};

class MessageResult: public CommandResult {
  private:
    const String message;

  public:
    MessageResult(String msg): message(msg) { }

    String getDescription() {
      return this->message;
    }

    void free() {
      delete this;
    }
};

class Command {
  public:
    virtual CommandResult *execute(Gonzague gonzague) {
      (void)gonzague; // Suppress warning
      return NULL;
    }
    virtual ~Command() { }

    virtual void free() = 0;
};

class MovementResult: public CommandResult {
  private:
    const String opDescription;
    const String unit;
    const int value;

  public:
    MovementResult(String op, String unt, int val): opDescription(op), unit(unt), value(val) { }

    String getDescription() {
      String description = opDescription;
      description += " ";
      description += value;
      description += unit;
      return description;
    }

    void free() {
      delete this;
    }
};

enum MoveDirection { Forward, Backward };
enum TurnDirection { Left, Right };

class MoveCommand: public Command {
  private:
    const int distance;
    const MoveDirection direction;

  public:
    MoveCommand(int dist, MoveDirection dir): distance(dist), direction(dir) { }

    CommandResult *execute(Gonzague gonzague) {
      String op = "";
      if (this->direction == Forward) {
        op = "Move forward";
        gonzague.moveForward(distance);
      }
      else {
        op = "Move backward";
        gonzague.moveBackward(distance);
      }
      return new MovementResult(op, "cm", this->distance);
    }

    void free() {
      delete this;
    }
};

class TurnCommand: public Command {
  private:
    const int angle;
    const TurnDirection direction;

  public:
    TurnCommand(int ang, TurnDirection dir): angle(ang), direction(dir) { }

    CommandResult *execute(Gonzague gonzague) {
      String op = "";
      if (this->direction == Left) {
        op = "Move left";
        gonzague.turnLeft(angle);
      }
      else {
        op = "Move right";
        gonzague.turnRight(angle);
      }
      return new MovementResult(op, "degrees", this->angle);
    }

    void free() {
      delete this;
    }
};

class UnknownOpCommand: public Command {
  public:
    CommandResult *execute(Gonzague gonzague) {
      (void)gonzague; // Suppress warning
      return new MessageResult("Unknown operation");
    }

    void free() {
      delete this;
    }
};

#define OP_END ';'
#define OP_FORWARD 'F'
#define OP_BACKWARD 'B'
#define OP_LEFT 'L'
#define OP_RIGHT 'R'

#define DEFAULT_CM_DISTANCE 5
#define DEFAULT_DEGREE_ANGLE 15

class SerialCommandQueue {
  public:
    SerialCommandQueue() { }

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
      return (cmDistance > 0) ? cmDistance : DEFAULT_CM_DISTANCE;
    }

    int getAngleOrDefault(int angle) {
      return (angle > 0) ? angle : DEFAULT_DEGREE_ANGLE;
    }

    bool isOpEnd(char opData) {
      return opData == OP_END || opData == '\n' || opData == '\r';
    }
};



#include <IRremote.h>

/*
  HX1838 VS1838 IR Remote
*/
#define IR_UP_HEX 0xFF629D
#define IR_DOWN_HEX 0xFFA857
#define IR_LEFT_HEX 0xFF22DD
#define IR_RIGHT_HEX 0xFFC23D
#define IR_REPEAT_HEX 0xFFFFFFFF

const int IR_RECEIVER_PIN = 4; // Originally Step Pulse Z

class InfraRedRemoteCommandQueue {
  private:
    IRrecv irrecv;
    decode_results results;
    long unsigned lastActionCode;

  public:
    InfraRedRemoteCommandQueue(): irrecv(IR_RECEIVER_PIN), lastActionCode(0) { }

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
          command = new MoveCommand(DEFAULT_CM_DISTANCE, Forward);
          break;
        case IR_DOWN_HEX:
          command = new MoveCommand(DEFAULT_CM_DISTANCE, Backward);
          break;
        case IR_LEFT_HEX:
          command = new TurnCommand(DEFAULT_DEGREE_ANGLE, Left);
          break;
        case IR_RIGHT_HEX:
          command = new TurnCommand(DEFAULT_DEGREE_ANGLE, Right);
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

Gonzague gonzague;
SerialCommandQueue serialQueue;
InfraRedRemoteCommandQueue irQueue;

void setup() {
  irQueue.setup();
  serialQueue.setup(9600);
  gonzague.setup();
  Serial.println("Gonzague Started");
}

void loop() {

  Command* command = irQueue.tryDequeCommand();

  if (command || serialQueue.hasNext()) {
    Serial.println("--------");
    Serial.println("Wake Up");
    gonzague.wakeUp();

    while (command) {
      handleCommand(command);
      command = irQueue.tryDequeCommand();
    }

    while (serialQueue.hasNext()) {
      handleCommand(serialQueue.dequeCommand());
    }

    gonzague.sleep();
    Serial.println("Sleep");
  }
}

void handleCommand(Command* command) {
  if (command) {
    CommandResult* result = command->execute(gonzague);
    command->free();
    Serial.println(result->getDescription());
    result->free();
  }
}
