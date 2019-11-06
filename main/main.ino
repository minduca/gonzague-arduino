#include "Gonzague.cpp"

#define OP_END ';'
#define OP_FORWARD 'F'
#define OP_BACKWARD 'B'
#define OP_LEFT 'L'
#define OP_RIGHT 'R'

#define DEFAULT_CM_DISTANCE 5
#define DEFAULT_DEGREE_ANGLE 15

Gonzague gonzague;

void setup() {
  Serial.begin(9600); // opens serial port and sets data rate to 9600 bps
  gonzague.setup();
  Serial.println("Gonzague Started");
}

void loop() {
  if (hasIncommingMessage()) {
    Serial.println("--------");
    Serial.println("Wake Up");
    gonzague.wakeUp();

    while (hasIncommingMessage()) {
      runNextCommand();
    }

    gonzague.sleep();
    Serial.println("Sleep");
  }
}

void runNextCommand() {
  char op = Serial.read();
  int arg = Serial.parseInt(SKIP_NONE);
  int distanceOrAngle = 0;
  String opDescription = "";
  String unit = "";

  switch (op) {
    case OP_FORWARD:
      distanceOrAngle = getCmOrDefault(arg);
      gonzague.moveForward(distanceOrAngle);
      opDescription = "Move Forward";
      unit = "cm";
      break;
    case OP_BACKWARD:
      distanceOrAngle = getCmOrDefault(arg);
      gonzague.moveBack(getCmOrDefault(distanceOrAngle));
      opDescription = "Move Back";
      unit = "cm";
      break;
    case OP_LEFT:
      distanceOrAngle = getAngleOrDefault(arg);
      gonzague.turnLeft(distanceOrAngle);
      opDescription = "Turn Left";
      unit = " degrees";
      break;
    case OP_RIGHT:
      distanceOrAngle = getAngleOrDefault(arg);
      gonzague.turnRight(distanceOrAngle);
      opDescription = "Turn Right";
      unit = " degrees";
      break;
    default:
      if (!isOpEnd(op)) {
        opDescription = "Unknown operation ";
        opDescription += op;
      }
      break;
  }

  if (unit != "") {
    opDescription += " ";
    opDescription += distanceOrAngle;
    opDescription += unit;
  }

  if (opDescription != "") {
    Serial.println(opDescription);
  }
}

int getCmOrDefault(int cmDistance) {
  return (cmDistance > 0) ? cmDistance : DEFAULT_CM_DISTANCE;
}

int getAngleOrDefault(int angle) {
  return (angle > 0) ? angle : DEFAULT_DEGREE_ANGLE;
}

bool isOpEnd(char opData) {
  return opData == OP_END || opData == '\n' || opData == '\r';
}

bool hasIncommingMessage() {
  return Serial.available() > 0;
}
