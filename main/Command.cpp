#ifndef __COMMAND_INCLUDED__
#define __COMMAND_INCLUDED__

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

#endif
