/**
*  Polargraph Server for ATMEGA328-based arduino boards.
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_a1

The program has a core part that consists of the following files:

- comms.ino
- configuration.ino
- eeprom.ino
- exec.ino
- penlift.ino
- pixel.ino
- util.ino

and the first portion of the main file, probably called
something like polargraph_server_a1.ino.

CONFIGURATION!! Read this!
==========================

Kung fu is like a game of chess. You must think first! Before you move.

This is a unified codebase for a few different versions of Polargraph Server.

You can control how it is compiled by changing the #define lines below.

Comment the lines below in or out to control what gets compiled.
*/

// Turn on some debugging code
// ===========================
//#define DEBUG
//#define DEBUG_COMMS

// Program features
// ================
#define PIXEL_DRAWING
#define PENLIFT
#define VECTOR_LINES

// Specify what kind of motor driver you are using
// ===============================================
// Make sure the version of motorshield you have is listed below WITHOUT "//" on the front.
// REMEMBER!!!  You need to comment out the matching library imports in the 'configuration.ino' tab too.
#define ADAFRUIT_MOTORSHIELD_V2
//#define ADAFRUIT_MOTORSHIELD_V1
//#define UNL2003_DRIVER

#include <AccelStepper.h>
#include <Servo.h>
#include <EEPROM.h>
#include "EEPROMAnything.h"

/*  ===========================================================  
    These variables are common to all polargraph server builds
=========================================================== */    

const String FIRMWARE_VERSION_NO = "1.10.2";

//  EEPROM addresses
const byte EEPROM_MACHINE_WIDTH = 0;
const byte EEPROM_MACHINE_HEIGHT = 2;
const byte EEPROM_MACHINE_MM_PER_REV = 14; // 4 bytes (float)
const byte EEPROM_MACHINE_STEPS_PER_REV = 18;
const byte EEPROM_MACHINE_STEP_MULTIPLIER = 20;

const byte EEPROM_MACHINE_MOTOR_SPEED = 22; // 4 bytes float
const byte EEPROM_MACHINE_MOTOR_ACCEL = 26; // 4 bytes float
const byte EEPROM_MACHINE_PEN_WIDTH = 30; // 4 bytes float

const byte EEPROM_MACHINE_HOME_A = 34; // 4 bytes
const byte EEPROM_MACHINE_HOME_B = 38; // 4 bytes

const byte EEPROM_PENLIFT_DOWN = 42; // 2 bytes
const byte EEPROM_PENLIFT_UP = 44; // 2 bytes

// Pen raising servo
Servo penHeight;
const int DEFAULT_DOWN_POSITION = 90;
const int DEFAULT_UP_POSITION = 180;
static int upPosition = DEFAULT_UP_POSITION; // defaults
static int downPosition = DEFAULT_DOWN_POSITION;
static int penLiftSpeed = 3; // ms between steps of moving motor
byte const PEN_HEIGHT_SERVO_PIN = 9; //UNL2003 driver uses pin 9
boolean isPenUp = false;

int motorStepsPerRev = 800;//UNL2003 = 4076
float mmPerRev = 95;//UNL2003 = 25
byte stepMultiplier = 1;

static int machineWidth = 650;
static int machineHeight = 800;

static int defaultMachineWidth = 650;
static int defaultMachineHeight = 650;
static int defaultMmPerRev = 95;//UNL2003 = 25
static int defaultStepsPerRev = 800;//UNL2003 = 4076
static int defaultStepMultiplier = 1;

float currentMaxSpeed = 800.0;
float currentAcceleration = 400.0;
boolean usingAcceleration = true;

int startLengthMM = 800;

float mmPerStep = mmPerRev / multiplier(motorStepsPerRev);
float stepsPerMM = multiplier(motorStepsPerRev) / mmPerRev;

long pageWidth = machineWidth * stepsPerMM;
long pageHeight = machineHeight * stepsPerMM;
long maxLength = 0;

//static char rowAxis = 'A';
const int INLENGTH = 50;
const char INTERMINATOR = 10;

static float penWidth = 0.8; // line width in mm

boolean reportingPosition = true;
boolean acceleration = true;

#ifdef UNL2003_DRIVER
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
// defaultStepsPerRev = 4076;
// defaultMmPerRev = 63;

static AccelStepper motorA(8, 6,8,7,9);
static AccelStepper motorB(8, 2,4,3,5);
#else
extern AccelStepper motorA;
extern AccelStepper motorB;
#endif

boolean currentlyRunning = true;

static char inCmd[10];
static char inParam1[14];
static char inParam2[14];
static char inParam3[14];
static char inParam4[14];
//static char inParams[4][14];

byte inNoOfParams;

char lastCommand[INLENGTH+1];
boolean commandConfirmed = false;

int rebroadcastReadyInterval = 5000;
long lastOperationTime = 0L;
long motorIdleTimeBeforePowerDown = 600000L;
boolean automaticPowerDown = false;
boolean powerIsOn = false;

long lastInteractionTime = 0L;

#ifdef PIXEL_DRAWING
static boolean lastWaveWasTop = true;

//  Drawing direction
const static byte DIR_NE = 1;
const static byte DIR_SE = 2;
const static byte DIR_SW = 3;
const static byte DIR_NW = 4;

static int globalDrawDirection = DIR_NW;

const static byte DIR_MODE_AUTO = 1;
const static byte DIR_MODE_PRESET = 2;
static byte globalDrawDirectionMode = DIR_MODE_AUTO;
#endif

#define READY_STR "READY"
#define RESEND_STR "RESEND"
#define DRAWING_STR "DRAWING"
#define OUT_CMD_SYNC_STR "SYNC,"

char MSG_E_STR[] = "MSG,E,";
char MSG_I_STR[] = "MSG,I,";
char MSG_D_STR[] = "MSG,D,";

const static char COMMA[] = ",";
const static char CMD_END[] = ",END";
const static String CMD_CHANGELENGTH = "C01";
const static String CMD_CHANGEPENWIDTH = "C02";
//const static String CMD_CHANGEMOTORSPEED = "C03";
//const static String CMD_CHANGEMOTORACCEL = "C04";
#ifdef PIXEL_DRAWING
const static String CMD_DRAWPIXEL = "C05";
const static String CMD_DRAWSCRIBBLEPIXEL = "C06";
//const static String CMD_DRAWRECT = "C07";
const static String CMD_CHANGEDRAWINGDIRECTION = "C08";
//const static String CMD_TESTPATTERN = "C10";
const static String CMD_TESTPENWIDTHSQUARE = "C11";
#endif
const static String CMD_SETPOSITION = "C09";
#ifdef PENLIFT
const static String CMD_PENDOWN = "C13";
const static String CMD_PENUP = "C14";
const static String CMD_SETPENLIFTRANGE = "C45";
#endif
#ifdef VECTOR_LINES
const static String CMD_CHANGELENGTHDIRECT = "C17";
#endif
const static String CMD_SETMACHINESIZE = "C24";
//const static String CMD_SETMACHINENAME = "C25";
const static String CMD_GETMACHINEDETAILS = "C26";
const static String CMD_RESETEEPROM = "C27";
const static String CMD_SETMACHINEMMPERREV = "C29";
const static String CMD_SETMACHINESTEPSPERREV = "C30";
const static String CMD_SETMOTORSPEED = "C31";
const static String CMD_SETMOTORACCEL = "C32";
const static String CMD_SETMACHINESTEPMULTIPLIER = "C37";

void setup() 
{
  Serial.begin(57600);           // set up Serial library at 57600 bps
  Serial.print("POLARGRAPH ON!");
  Serial.println();
  configuration_motorSetup();
  eeprom_loadMachineSpecFromEeprom();
  configuration_setup();

  motorA.setMaxSpeed(currentMaxSpeed);
  motorA.setAcceleration(currentAcceleration);  
  motorB.setMaxSpeed(currentMaxSpeed);
  motorB.setAcceleration(currentAcceleration);
  
  float startLength = ((float) startLengthMM / (float) mmPerRev) * (float) motorStepsPerRev;
  motorA.setCurrentPosition(startLength);
  motorB.setCurrentPosition(startLength);
  for (int i = 0; i<INLENGTH; i++) {
    lastCommand[i] = 0;
  }    
  comms_ready();

#ifdef PENLIFT
  penlift_penUp();
#endif
  delay(500);

}

void loop()
{
  if (comms_waitForNextCommand(lastCommand)) 
  {
#ifdef DEBUG_COMMS    
    Serial.print(F("Last comm: "));
    Serial.print(lastCommand);
    Serial.println(F("..."));
#endif
    comms_parseAndExecuteCommand(lastCommand);
  }
}




