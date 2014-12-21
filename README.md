polargraph_server_a1
====================

Polargraph Server for ATMEGA328 based arduino boards using Adafruit motorshields.
It is called *_a1* because it is the version for the first arduino (ie Uno, or Duemilanove).  
Bit obtuse that, I know.

For convenience, I have pre-compiled and included two hex files:

1. **polargraph_server_a1_adafruit_v1.cpp.hex** - Is for motorshield v1.
2. **polargraph_server_a1_adafruit_v2.cpp.hex** - Is for the new motorshield v2.

Motor driver:
-------------

This firmware works for:
1. Adafruit Motorshield v1 that uses AFMotor as it's software driver
2. Adafruit Motorshield v2 that uses Adafruit_MotorShield as it's software driver
3. Generic serial stepper drivers, eg Stepsticks or Easy Drivers
4. Four-wire signal amplifier, eg UNL2003

To switch between the different drivers, comment out the other lines in polargraph_server_a1.ino

``` C++
#define ADAFRUIT_MOTORSHIELD_V1
//#define ADAFRUIT_MOTORSHIELD_V2

// Using discrete stepper drivers? (eg EasyDriver, stepstick, Pololu gear),
// choose SERIAL_STEPPER_DRIVERS and define your pins at the bottom of 'configuration.ino'.
//#define SERIAL_STEPPER_DRIVERS 

// Using a signal amplifier like a UNL2003? 
//#define UNL2003_DRIVER
```

AND then, you also need to comment out the irrelevant #include statements in configuration.ino too.

``` C++
// Using Adafruit Motorshield V2? Comment out this one line underneath.
#include <AFMotor.h>

// Using Adafruit Motorshield V1? Comment out the three INCLUDE lines below.
//#include <Wire.h>
//#include <Adafruit_MotorShield.h>
//#include "utility/Adafruit_PWMServoDriver.h"
```


The program has a core part that consists of the following files that are common to all Polargraph Server versions:

- comms.ino
- configuration.ino
- eeprom.ino
- exec.ino
- penlift.ino
- pixel.ino
- util.ino

and 
- polargraph_server_a1.ino

which is named for the project.

The only other file worth highlighing is **impl_uno.ino** which has the uno-only implementations of the server
features.  This file is the one that is modified for different platforms / processors.

Written by Sandy Noble

Released under GNU License version 3.

http://www.polargraph.co.uk

https://github.com/euphy/polargraph_server_a1
