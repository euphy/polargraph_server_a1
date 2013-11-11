polargraph_server_a1
====================

Polargraph Server for ATMEGA328 based arduino boards using AFMotor (Adafruit Motorshield v1).
It is called *_a1* because it is the version for the first arduino (ie Uno, or Duemilanove).  
Bit obtuse that, I know.


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

The only other file worth highlighing is *impl_uno.ino* which has the uno-only implementations of the server
features.  This file is the one that is modified for different platforms / processors.

Written by Sandy Noble

Released under GNU License version 3.

http://www.polargraph.co.uk

http://code.google.com/p/polargraph/
