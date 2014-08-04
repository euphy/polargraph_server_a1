/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  http://code.google.com/p/polargraph/

Configuration.

This is one of the core files for the polargraph server program.  
It sets up the motor objects (AccelSteppers), and has default
values for the motor, sprocket and microstepping combinations used
by polargraphs so far.

Comment out the blocks of code you don't need.

*/
// motor configurations for the various electrical schemes

// =================================================================
// 1. Adafruit motorshield

// Using Adafruit Motorshield V2 or UNL2003? Comment out this one line underneath.
//#include <AFMotor.h>

// Using Adafruit Motorshield V1 or UNL2003? Comment out the three INCLUDE lines below.
//#include <Wire.h>
//#include <Adafruit_MotorShield.h>
//#include "utility/Adafruit_PWMServoDriver.h"

#ifdef ADAFRUIT_MOTORSHIELD_V1
const int stepType = INTERLEAVE;
AF_Stepper afMotorA(motorStepsPerRev, 1);
AF_Stepper afMotorB(motorStepsPerRev, 2);

void forwarda() { afMotorA.onestep(FORWARD, stepType); }
void backwarda() { afMotorA.onestep(BACKWARD, stepType); }
void forwardb() { afMotorB.onestep(FORWARD, stepType); }
void backwardb() { afMotorB.onestep(BACKWARD, stepType); }
#endif

#ifdef ADAFRUIT_MOTORSHIELD_V2
const int stepType = MICROSTEP;

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_StepperMotor *afMotorA = AFMS.getStepper(motorStepsPerRev, 1);
Adafruit_StepperMotor *afMotorB = AFMS.getStepper(motorStepsPerRev, 2);

void forwarda() { afMotorA->onestep(FORWARD, stepType); }
void backwarda() { afMotorA->onestep(BACKWARD, stepType); }
void forwardb() { afMotorB->onestep(FORWARD, stepType); }
void backwardb() { afMotorB->onestep(BACKWARD, stepType); }
#endif

#ifdef UNL2003
void forwarda() { motorA.move(FORWARD);motorA.run(); }
void backwarda() { motorA.move(BACKWARD);motorA.run(); }
void forwardb() { motorB.move(FORWARD);motorB.run(); }
void backwardb() { motorB.move(BACKWARD);motorB.run(); }
#endif

AccelStepper motorA(forwarda, backwarda);
AccelStepper motorB(forwardb, backwardb);

void configuration_motorSetup()
{
  // no initial setup for these kinds of motor drivers
}
void configuration_setup()
{
  defaultMachineWidth = 650;
  defaultMachineHeight = 650;
  defaultMmPerRev = 95;
  defaultStepsPerRev = 400;
  defaultStepMultiplier = 1;
#ifdef ADAFRUIT_MOTORSHIELD_V2
  AFMS.begin();  // create with the default frequency 1.6KHz
#endif
  delay(500);
  
}
// end of Adafruit motorshield definition
// =================================================================



