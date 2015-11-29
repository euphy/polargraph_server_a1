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
*/

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

#ifdef UNL2003_DRIVER
// Contributed by @j0nson
// Initialize ULN2003 stepper driver
// first number is type of stepper motor, 4 for a normal 4 wire step motor, 8 for a halfstepped normal 4 wire motor
//Connection Directions
// MotorA
//ULN2003  Arduino  AcceStepper Init
//IN1      2        2
//IN2      3        4
//IN3      4        3
//IN4      5        5
// MotorB
//ULN2003  Arduino  AcceStepper Init
//IN1      6        6
//IN2      7        8
//IN3      8        7
//IN4      9        9

//for a 28YBJ-48 Stepper, change these parameters above
//Step angle (8-step) 5.625deg, 64 steps per rev
//Step angle (4-step) 11.25deg, 32 steps per rev
//gear reduction ratio 1/63.68395

// motorStepsPerRev = 32 * 63.68395 = 2038; //for 4 step sequence
// motorStepsPerRev = 64 * 63.68395 = 4076; //for 8 step sequence

// motorStepsPerRev = 4076;
// mmPerRev = 63;
// DEFAULT_STEPS_PER_REV = 4076;
// DEFAULT_MM_PER_REV = 63;

AccelStepper motorA(8, 6,8,7,9);
AccelStepper motorB(8, 2,4,3,5);
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
  mmPerStep = mmPerRev / multiplier(motorStepsPerRev);
  stepsPerMM = multiplier(motorStepsPerRev) / mmPerRev;
  
#ifdef ADAFRUIT_MOTORSHIELD_V2
  AFMS.begin();  // create with the default frequency 1.6KHz
#endif
  delay(500);
  
}
// end of Adafruit motorshield definition
// =================================================================



