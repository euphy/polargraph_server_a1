polargraph_server_a1
====================

Polargraph Server for ATMEGA328 based arduino boards using Adafruit motorshields.
It is called *_a1* because it is the version for the first arduino (ie Uno, or Duemilanove).  
Bit obtuse that, I know.

There is too much code to fit onto an arduino all at once, so it is arranged into a couple of "feature" sets:

For convenience, I have pre-compiled and included two hex files:

1. **polargraph_server_a1_adafruit_v1.cpp.hex** - Is for motorshield v1.
2. **polargraph_server_a1_adafruit_v2.cpp.hex** - Is for the new motorshield v2.

When you compile from source yourself, the features that get compiled and loaded are controlled by editing the #define lines in the main file (polargraph_server_a1.ino). Comment out a line to remove that feature.

``` C++
//#define PIXEL_DRAWING
#define PENLIFT
#define VECTOR_LINES
```

Features: 
---------

1. **PIXEL_DRAWING** - Drawing using a shading algorithm, like on https://www.flickr.com/photos/euphy/6320268680/in/set-72157626497662024. Necessary for "render pixel" functions.
1. **PENLIFT** - Control a little servo motor that can lift and drop the pen from the drawing surface.
1. **VECTOR_LINES** - Draw lines that are straight. Necessary for vector drawing.

I know this chopping and recompiling is a bit of a pain, but something has to give.

Motor driver:
-------------

This firmware works for Adafruit Motorshield v1 that uses AFMotor as it's software driver, and also Adafruit Motorshield v2 that uses Adafruit_MotorShield as it's software driver.

To switch between the two, comment out one of the two lines in polargraph_server_a1.ino

``` C++
#define ADAFRUIT_MOTORSHIELD_V2
//#define ADAFRUIT_MOTORSHIELD_V1
```

AND then, you also need to comment out the irrelevant #include statements in configuration.ino too.

``` C++
// Using Adafruit Motorshield V2? Comment out this one line underneath.
//#include <AFMotor.h>

// Using Adafruit Motorshield V1? Comment out the three INCLUDE lines below.
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
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
