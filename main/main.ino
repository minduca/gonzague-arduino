#include "Gonzague.cpp"

Gonzague gonzague;

class CommandResult {
  public:
    virtual String getDescription() {
      return "a";
    }
};

class MessageResult: public CommandResult {
  private:
    const String message;

  public:
    MessageResult(String msg): message(msg) { }

    String getDescription() {
      return this->message;
    }
};

class Command {
  public:
    virtual CommandResult execute(Gonzague gonzague) {
      (void)gonzague; // Suppress warning
      return CommandResult();
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
    ~MovementResult() {}

    String getDescription() {
      String description = opDescription;
      description += " ";
      description += value;
      description += unit;
      return description;
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
    ~MoveCommand() {}

    CommandResult execute(Gonzague gonzague) {
      String op = "";
      if (this->direction == Forward) {
        op = "Move forward";
        gonzague.moveForward(distance);
      }
      else {
        op = "Move backward";
        gonzague.moveBackward(distance);
      }
      return MovementResult(op, "cm", this->distance);
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
    ~TurnCommand() { }

    CommandResult execute(Gonzague gonzague) {
      String op = "";
      if (this->direction == Left) {
        op = "Move left";
        gonzague.turnLeft(angle);
      }
      else {
        op = "Move right";
        gonzague.turnRight(angle);
      }
      return MovementResult(op, "degrees", this->angle);
    }

    void free() {
      delete this;
    }
};

class UnknownOpCommand: public Command {
  public:
    CommandResult execute(Gonzague gonzague) {
      (void)gonzague; // Suppress warning
      return MessageResult("Unknown operation");
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

    bool hasIncommingMessage() {
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

SerialCommandQueue serialQueue;

void setup() {
  Serial.begin(9600); // opens serial port and sets data rate to 9600 bps
  gonzague.setup();
  Serial.println("Gonzague Started");
}

void loop() {
  if (serialQueue.hasIncommingMessage()) {
    Serial.println("--------");
    Serial.println("Wake Up");
    gonzague.wakeUp();

    while (serialQueue.hasIncommingMessage()) {
      Command* command = serialQueue.dequeCommand();
      if (command) {
        CommandResult result = command->execute(gonzague);
        Serial.println(result.getDescription());
        command->free();
      }
    }

    gonzague.sleep();
    Serial.println("Sleep");
  }
}
