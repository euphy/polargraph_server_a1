/**
*  Polargraph Server for ATMEGA328-based arduino boards.
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  http://code.google.com/p/polargraph/

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



*/

#include <AccelStepper.h>
#include <Servo.h>
#include <EEPROM.h>
#include "EEPROMAnything.h"

/*  ===========================================================  
    These variables are common to all polargraph server builds
=========================================================== */    

const String FIRMWARE_VERSION_NO = "1.66";

// for working out CRCs
static PROGMEM prog_uint32_t crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

static boolean usingCrc = false;

//  EEPROM addresses
const byte EEPROM_MACHINE_WIDTH = 0;
const byte EEPROM_MACHINE_HEIGHT = 2;
const byte EEPROM_MACHINE_NAME = 4;
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
byte const PEN_HEIGHT_SERVO_PIN = 9;
boolean isPenUp = false;

int motorStepsPerRev = 800;
float mmPerRev = 95;
byte stepMultiplier = 1;

static float translateX = 0.0;
static float translateY = 0.0;
static float scaleX = 1.0;
static float scaleY = 1.0;
static int rotateTransform = 0;

static int machineWidth = 650;
static int machineHeight = 800;

static int defaultMachineWidth = 650;
static int defaultMachineHeight = 650;
static int defaultMmPerRev = 95;
static int defaultStepsPerRev = 800;
static int defaultStepMultiplier = 1;

String machineName = "";
const String DEFAULT_MACHINE_NAME = "PG01    ";

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

//const char DIRECTION_STRING_LTR = 'L';
//static int SRAM_SIZE = 2048;
const String FREE_MEMORY_STRING = "MEMORY,";
int availMem = 0;

static float penWidth = 0.8; // line width in mm

boolean reportingPosition = true;
boolean acceleration = true;

extern AccelStepper motorA;
extern AccelStepper motorB;

boolean currentlyRunning = true;

static String inCmd = "                                                  ";
static String inParam1 = "              ";
static String inParam2 = "              ";
static String inParam3 = "              ";
static String inParam4 = "              ";

byte inNoOfParams;

int rebroadcastReadyInterval = 5000L;
long lastOperationTime = 0L;
long motorIdleTimeBeforePowerDown = 600000L;
boolean automaticPowerDown = false;

long lastInteractionTime = 0L;

static boolean lastWaveWasTop = true;
//static boolean lastMotorBiasWasA = true;
//static boolean drawingLeftToRight = true;

//  Drawing direction
const static byte DIR_NE = 1;
const static byte DIR_SE = 2;
const static byte DIR_SW = 3;
const static byte DIR_NW = 4;

const static byte DIR_N = 5;
const static byte DIR_E = 6;
const static byte DIR_S = 7;
const static byte DIR_W = 8;
static int globalDrawDirection = DIR_NW;

const static byte DIR_MODE_AUTO = 1;
const static byte DIR_MODE_PRESET = 2;
const static byte DIR_MODE_RANDOM = 3;
static byte globalDrawDirectionMode = DIR_MODE_AUTO;


//static int currentRow = 0;

const String READY = "READY";
const String RESEND = "RESEND";
const String DRAWING = "DRAWING";
const String OUT_CMD_CARTESIAN = "CARTESIAN,";
const String OUT_CMD_SYNC = "SYNC,";

static String readyString = READY;

String lastCommand = "";
boolean commandConfirmed = false;

const static String COMMA = ",";
const static String CMD_END = ",END";
const static String CMD_CHANGELENGTH = "C01";
const static String CMD_CHANGEPENWIDTH = "C02";
const static String CMD_CHANGEMOTORSPEED = "C03";
const static String CMD_CHANGEMOTORACCEL = "C04";
const static String CMD_DRAWPIXEL = "C05";
const static String CMD_DRAWSCRIBBLEPIXEL = "C06";
//const static String CMD_DRAWRECT = "C07";
const static String CMD_CHANGEDRAWINGDIRECTION = "C08";
const static String CMD_SETPOSITION = "C09";
const static String CMD_TESTPATTERN = "C10";
const static String CMD_TESTPENWIDTHSQUARE = "C11";
const static String CMD_PENDOWN = "C13";
const static String CMD_PENUP = "C14";
const static String CMD_CHANGELENGTHDIRECT = "C17";
const static String CMD_SETMACHINESIZE = "C24";
const static String CMD_SETMACHINENAME = "C25";
const static String CMD_GETMACHINEDETAILS = "C26";
const static String CMD_RESETEEPROM = "C27";
const static String CMD_SETMACHINEMMPERREV = "C29";
const static String CMD_SETMACHINESTEPSPERREV = "C30";
const static String CMD_SETMOTORSPEED = "C31";
const static String CMD_SETMOTORACCEL = "C32";
const static String CMD_SETMACHINESTEPMULTIPLIER = "C37";
const static String CMD_SETPENLIFTRANGE = "C45";

void setup() 
{
  Serial.begin(57600);           // set up Serial library at 57600 bps
  Serial.print(F("POLARGRAPH ON!"));
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
  readyString = READY;
  comms_establishContact();

  penlift_penUp();
  delay(500);
  outputAvailableMemory();
}

void loop()
{
  lastCommand = comms_waitForNextCommand();
  comms_parseAndExecuteCommand(lastCommand);
}




