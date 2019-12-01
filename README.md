# Gonzague Arduino

Robot that interprets Serial messages and IR remote instructions to walk around.

To control the robot using an application running on a Raspberry PI, see [gonzague-bot](https://github.com/minduca/gonzague-bot).

## Getting Started

In order to run this project, you will need to assemble some hardware.

### Hardware Requirements

You need an Arduino UNO and following hardwares properly connected to it:

* Cnc Shield v3.0.
* Two stepper motors connected to the X and Y axis of the Cnc Shield.
* Optionally, an HX1838 VS1838 IR Remote receiver.
* Power supply that meets the voltage and amperage requirements of the Cnc Shield and the motors.
* Power supply for the Arduino.
* A frame of your choosing to assembly the hardware. // TODO publish the 3D printable frame.

#### Assembling the Hardware

// TODO.

### Build & Deploy to Arduino

1. Connect the Arduino to the computer via USB port.
2. Open the project on the Arduino IDE.
3. Compile.
4. Upload.

### Send Instructions Through the Serial Monitor

On Arduino IDE, open the Serial Monitor.

The following commands are supported:

* `F`: move forward 5cm.
* `B`: move backward 5cm.
* `L`: turn left 15 degrees.
* `R`: turn right 15 degrees.

You can also specify the distance/degrees explicitly and chain instructions into a batch request:

```bash
F10;B5;L90;R90;B
```

The command above will wake up the robot, then make it move forward 10cm, backward 5cm, turn left 90 degrees, turn right 90 degrees, move backward 5cm (the absence of explicit distance fallbacks to its default value), then sleep.

The separator `;` is optional and is there for readability purposes only.

### Send Instructions Through an Infrared (IR) Remote Control

You can control the robot using an IR remote control.

See the hardware requirements for supported IR remote control types.

Only the directional buttons are implemented (Up, Down, Left and Right buttons).

### License

See [LICENSE](LICENSE.txt) file.

This project uses the [IRremote](https://github.com/z3t0/Arduino-IRremote) library, which is governed by its own separate license.
