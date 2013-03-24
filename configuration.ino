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

#include <AFMotor.h>
const int stepType = INTERLEAVE;

AF_Stepper afMotorA(motorStepsPerRev, 1);
AF_Stepper afMotorB(motorStepsPerRev, 2);

void forwarda() { afMotorA.onestep(FORWARD, stepType); }
void backwarda() { afMotorA.onestep(BACKWARD, stepType); }
AccelStepper motorA(forwarda, backwarda);

void forwardb() { afMotorB.onestep(FORWARD, stepType); }
void backwardb() { afMotorB.onestep(BACKWARD, stepType); }
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
  delay(500);
}
// end of Adafruit motorshield definition
// =================================================================



