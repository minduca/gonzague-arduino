#include "Gonzague.cpp"
#include "Command.cpp"
#include "SerialCommandQueue.cpp"
#include "InfraRedRemoteCommandQueue.cpp"

#define DEFAULT_CM_DISTANCE 5
#define DEFAULT_DEGREE_ANGLE 15

Gonzague gonzague;
SerialCommandQueue serialQueue(DEFAULT_CM_DISTANCE, DEFAULT_DEGREE_ANGLE);
InfraRedRemoteCommandQueue irQueue(DEFAULT_CM_DISTANCE, DEFAULT_DEGREE_ANGLE);

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
