/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_a1

Configuration.

This is one of the core files for the polargraph server program.  
It sets up the motor objects (AccelSteppers), and has default
values for the motor, sprocket and microstepping combinations used
by polargraphs so far.

Comment out the #include lines for the electrical drivers you DON'T need.

So, if you are using Adafruit Motorshield v1: Make sure the #include lines 
under V2 are preceded with //.

Equally, if you are using Adafruit Motorshield v2, make sure the #include
lines under V1 are precede with //.

If you are using generic serial stepper drivers (eg EasyDrivers, 
stepsticks, Pololu gear), then look
*/

// =================================================================
// Configuring Adafruit motorshield version 1.
// Using Adafruit Motorshield V1? Make sure the #include line below is NOT commented-out.
// Using Adafruit Motorshield V2? Comment out the line below.
#include <AFMotor.h>

// =================================================================
// Configuring Adafruit motorshield version 2.
// Using Adafruit Motorshield V1? Comment out the three INCLUDE lines below.
// Using Adafruit Motorshield V2? Make sure these three lines are NOT commented-out.
//#include <Wire.h>
//#include <Adafruit_MotorShield.h>
//#include "utility/Adafruit_PWMServoDriver.h"


// Adafruiters, you don't need to worry about anything more in this file.

#ifdef ADAFRUIT_MOTORSHIELD_V1
const int stepType = INTERLEAVE;
AF_Stepper afMotorA(motorStepsPerRev, 1);
AF_Stepper afMotorB(motorStepsPerRev, 2);

void forwarda() { afMotorA.onestep(FORWARD, stepType); }
void backwarda() { afMotorA.onestep(BACKWARD, stepType); }
void forwardb() { afMotorB.onestep(FORWARD, stepType); }
void backwardb() { afMotorB.onestep(BACKWARD, stepType); }
AccelStepper motorA(forwarda, backwarda);
AccelStepper motorB(forwardb, backwardb);
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
AccelStepper motorA(forwarda, backwarda);
AccelStepper motorB(forwardb, backwardb);
#endif

/* Motor setup if you are using serial stepper drivers 
(EasyDrivers, stepsticks, Pololu etc).

If you are wiring up yourself, just put the pin numbers in here.

Note that the pen lift servo usually lives on pin 9, so avoid 
that if you can. If you can't, then you know how to change it.
*/
#ifdef SERIAL_STEPPER_DRIVERS
#define MOTOR_A_ENABLE_PIN 3
#define MOTOR_A_STEP_PIN 4
#define MOTOR_A_DIR_PIN 5
  
#define MOTOR_B_ENABLE_PIN 6
#define MOTOR_B_STEP_PIN 7
#define MOTOR_B_DIR_PIN 8
AccelStepper motorA(1,MOTOR_A_STEP_PIN, MOTOR_A_DIR_PIN); 
AccelStepper motorB(1,MOTOR_B_STEP_PIN, MOTOR_B_DIR_PIN); 

#endif

void configuration_motorSetup()
{
#ifdef SERIAL_STEPPER_DRIVERS
  pinMode(MOTOR_A_ENABLE_PIN, OUTPUT);
  digitalWrite(MOTOR_A_ENABLE_PIN, HIGH);
  pinMode(MOTOR_B_ENABLE_PIN, OUTPUT);
  digitalWrite(MOTOR_B_ENABLE_PIN, HIGH);
  motorA.setEnablePin(MOTOR_A_ENABLE_PIN);
  motorA.setPinsInverted(false, false, true);
  motorB.setEnablePin(MOTOR_B_ENABLE_PIN);
  motorB.setPinsInverted(true, false, true); // this one turns the opposite direction to A, hence inverted.
#endif
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



