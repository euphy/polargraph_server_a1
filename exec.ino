/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_a1

Exec.

This is one of the core files for the polargraph server program.  
Purposes are getting a little more blurred here.  This file contains
the basic decision tree that branches based on command.

It has a set of the most general-purpose drawing commands, but only
methods that are directly called - none of the geometry or conversion
routines are here.

*/
/**  This method looks only for the basic command set
*/
boolean exec_executeBasicCommand(String &com)
{
  boolean executed = true;
  if (com.startsWith(CMD_CHANGELENGTH))
    exec_changeLength();
#ifdef VECTOR_LINES
  else if (com.startsWith(CMD_CHANGELENGTHDIRECT))
    exec_changeLengthDirect();
#endif
  else if (com.startsWith(CMD_CHANGEPENWIDTH))
    exec_changePenWidth();
  else if (com.startsWith(CMD_SETMOTORSPEED))
    exec_setMotorSpeed();
  else if (com.startsWith(CMD_SETMOTORACCEL))
    exec_setMotorAcceleration();
#ifdef PIXEL_DRAWING
  else if (com.startsWith(CMD_DRAWPIXEL))
    pixel_drawSquarePixel();
  else if (com.startsWith(CMD_DRAWSCRIBBLEPIXEL))
    pixel_drawScribblePixel();
  else if (com.startsWith(CMD_CHANGEDRAWINGDIRECTION))
    pixel_changeDrawingDirection();
  else if (com.startsWith(CMD_TESTPENWIDTHSQUARE))
    pixel_testPenWidth();
#endif
  else if (com.startsWith(CMD_SETPOSITION))
    exec_setPosition();
#ifdef PENLIFT
  else if (com.startsWith(CMD_PENDOWN))
    penlift_penDown();
  else if (com.startsWith(CMD_PENUP))
    penlift_penUp();
  else if (com.startsWith(CMD_SETPENLIFTRANGE))
    exec_setPenLiftRange();
#endif
  else if (com.startsWith(CMD_SETMACHINESIZE))
    exec_setMachineSizeFromCommand();
  else if (com.startsWith(CMD_SETMACHINEMMPERREV))
    exec_setMachineMmPerRevFromCommand();
  else if (com.startsWith(CMD_SETMACHINESTEPSPERREV))
    exec_setMachineStepsPerRevFromCommand();
  else if (com.startsWith(CMD_SETMACHINESTEPMULTIPLIER))
    exec_setMachineStepMultiplierFromCommand();
  else if (com.startsWith(CMD_GETMACHINEDETAILS))
    exec_reportMachineSpec();
  else if (com.startsWith(CMD_RESETEEPROM))
    eeprom_resetEeprom();
  else
    executed = false;

  return executed;
}
void exec_reportMachineSpec()
{
  eeprom_dumpEeprom();
  Serial.print(F("PGSIZE,"));
  Serial.print(machineWidth);
  Serial.print(COMMA);
  Serial.print(machineHeight);
  Serial.println(CMD_END);

  Serial.print(F("PGMMPERREV,"));
  Serial.print(mmPerRev);
  Serial.println(CMD_END);

  Serial.print(F("PGSTEPSPERREV,"));
  Serial.print(motorStepsPerRev);
  Serial.println(CMD_END);
  
  Serial.print(F("PGSTEPMULTIPLIER,"));
  Serial.print(stepMultiplier);
  Serial.println(CMD_END);

  Serial.print(F("PGLIFT,"));
  Serial.print(downPosition);
  Serial.print(COMMA);
  Serial.print(upPosition);
  Serial.println(CMD_END);

  Serial.print(F("PGSPEED,"));
  Serial.print(currentMaxSpeed);
  Serial.print(COMMA);
  Serial.print(currentAcceleration);
  Serial.println(CMD_END);

}

void exec_setMachineSizeFromCommand()
{
  int width = atoi(inParam1);
  int height = atoi(inParam2);

  // load to get current settings
  int currentValue = width;
  EEPROM_readAnything(EEPROM_MACHINE_WIDTH, currentValue);  
  if (currentValue != width)
    if (width > 10)
    {
      EEPROM_writeAnything(EEPROM_MACHINE_WIDTH, width);
    }
  
  EEPROM_readAnything(EEPROM_MACHINE_HEIGHT, currentValue);
  if (currentValue != height)
    if (height > 10)
    {
      EEPROM_writeAnything(EEPROM_MACHINE_HEIGHT, height);
    }

  // reload 
  eeprom_loadMachineSize();
}

void exec_setMachineMmPerRevFromCommand()
{
  EEPROM_writeAnything(EEPROM_MACHINE_MM_PER_REV, (float)atof(inParam1));
  eeprom_loadMachineSpecFromEeprom();
}
void exec_setMachineStepsPerRevFromCommand()
{
  EEPROM_writeAnything(EEPROM_MACHINE_STEPS_PER_REV, atoi(inParam1));
  eeprom_loadMachineSpecFromEeprom();
}
void exec_setMachineStepMultiplierFromCommand()
{
  EEPROM_writeAnything(EEPROM_MACHINE_STEP_MULTIPLIER, atoi(inParam1));
  eeprom_loadMachineSpecFromEeprom();
}

void exec_setPenLiftRange()
{
  int down = atoi(inParam1);
  int up = atoi(inParam2);

#ifdef DEBUG_PENLIFT
  Serial.print(F("Down: "));
  Serial.println(down);
  Serial.print(F("Up: "));
  Serial.println(up);
#endif 
  if (inNoOfParams == 3) 
  {
    // 3 params (C45,<downpos>,<uppos>,1,END) means save values to EEPROM
    EEPROM_writeAnything(EEPROM_PENLIFT_DOWN, down);
    EEPROM_writeAnything(EEPROM_PENLIFT_UP, up);
    eeprom_loadPenLiftRange();
  }
  else if (inNoOfParams == 2)
  {
    // 2 params (C45,<downpos>,<uppos>,END) means just do a range test
    penlift_movePen(up, down, penLiftSpeed);
    delay(200);
    penlift_movePen(down, up, penLiftSpeed);
    delay(200);
    penlift_movePen(up, down, penLiftSpeed);
    delay(200);
    penlift_movePen(down, up, penLiftSpeed);
    delay(200);
  }
}

/* Single parameter to set max speed, add a second parameter of "1" to make it persist.
*/
void exec_setMotorSpeed()
{
  exec_setMotorSpeed((float)atof(inParam1));
  if (inNoOfParams == 2 && atoi(inParam2) == 1)
    EEPROM_writeAnything(EEPROM_MACHINE_MOTOR_SPEED, currentMaxSpeed);
}

void exec_setMotorSpeed(float speed)
{
  currentMaxSpeed = speed;
  motorA.setMaxSpeed(currentMaxSpeed);
  motorB.setMaxSpeed(currentMaxSpeed);
  Serial.print(F("New max speed: "));
  Serial.println(currentMaxSpeed);
}

/* Single parameter to set acceleration, add a second parameter of "1" to make it persist.
*/
void exec_setMotorAcceleration()
{
  exec_setMotorAcceleration((float)atof(inParam1));
  if (inNoOfParams == 2 && atoi(inParam2) == 1)
    EEPROM_writeAnything(EEPROM_MACHINE_MOTOR_ACCEL, currentAcceleration);
}
void exec_setMotorAcceleration(float accel)
{
  currentAcceleration = accel;
  motorA.setAcceleration(currentAcceleration);
  motorB.setAcceleration(currentAcceleration);
  Serial.print(F("New accel: "));
  Serial.println(currentAcceleration);
}

void exec_changePenWidth()
{
  penWidth = atof(inParam1);
  Serial.print(F("Changed Pen width to "));
  Serial.print(penWidth);
  Serial.println(F("mm"));
}

void exec_setPosition()
{
  long targetA = multiplier(atol(inParam1));
  long targetB = multiplier(atol(inParam2));
  motorA.setCurrentPosition(targetA);
  motorB.setCurrentPosition(targetB);
  
  impl_engageMotors();
  
  reportPosition();
}

void exec_changeLengthRelative()
{
  long lenA = multiplier(atol(inParam1));
  long lenB = multiplier(atol(inParam2));
  
  changeLengthRelative(lenA, lenB);
}  

void exec_changeLength()
{
  float lenA = multiplier((float)atof(inParam1));
  float lenB = multiplier((float)atof(inParam2));
  
  changeLength(lenA, lenB);
}

#ifdef VECTOR_LINES
void exec_changeLengthDirect()
{
  float endA = multiplier((float)atof(inParam1));
  float endB = multiplier((float)atof(inParam2));
  int maxSegmentLength = atoi(inParam3);

  float startA = motorA.currentPosition();
  float startB = motorB.currentPosition();

  if (endA < 20 || endB < 20 || endA > getMaxLength() || endB > getMaxLength())
  {
    Serial.print(MSG_E_STR);
    Serial.println(F("This point falls outside the area of this machine. Skipping it."));
  }
  else
  {
    exec_drawBetweenPoints(startA, startB, endA, endB, maxSegmentLength);
  }
}  

/**
This moves the gondola in a straight line between p1 and p2.  Both input coordinates are in 
the native coordinates system.  

The fidelity of the line is controlled by maxLength - this is the longest size a line segment is 
allowed to be.  1 is finest, slowest.  Use higher values for faster, wobblier.
*/
void exec_drawBetweenPoints(float p1a, float p1b, float p2a, float p2b, int maxSegmentLength)
{
  // ok, we're going to plot some dots between p1 and p2.  Using maths. I know! Brave new world etc.
  
  // First, convert these values to cartesian coordinates
  // We're going to figure out how many segments the line
  // needs chopping into.
  float c1x = getCartesianXFP(p1a, p1b);
  float c1y = getCartesianYFP(c1x, p1a);
  
  float c2x = getCartesianXFP(p2a, p2b);
  float c2y = getCartesianYFP(c2x, p2a);
  
  // test to see if it's on the page
  // AND ALSO TO see if the current position is on the page.
  // Remember, the native system can easily specify points that can't exist,
  // particularly up at the top.
  if (c2x > 20 
    && c2x<pageWidth-20 
    && c2y > 20 
    && c2y <pageHeight-20
    && c1x > 20 
    && c1x<pageWidth-20 
    && c1y > 20 
    && c1y <pageHeight-20 
    )
    {
    reportingPosition = false;
    float deltaX = c2x-c1x;    // distance each must move (signed)
    float deltaY = c2y-c1y;
//    float totalDistance = sqrt(sq(deltaX) + sq(deltaY));

    int linesegs = 1;            // assume at least 1 line segment will get us there.
    if (abs(deltaX) > abs(deltaY))
    {
      // slope <=1 case    
      while ((abs(deltaX)/linesegs) > maxSegmentLength)
      {
        linesegs++;
      }
    }
    else
    {
      // slope >1 case
      while ((abs(deltaY)/linesegs) > maxSegmentLength)
      {
        linesegs++;
      }
    }
    
    // reduce delta to one line segments' worth.
    deltaX = deltaX/linesegs;
    deltaY = deltaY/linesegs;
  
    // render the line in N shorter segments
    long runSpeed = 0;

    usingAcceleration = false;
    while (linesegs > 0)
    {
//      Serial.print("Line segment: " );
//      Serial.println(linesegs);
      // compute next new location
      c1x = c1x + deltaX;
      c1y = c1y + deltaY;
  
      // convert back to machine space
      float pA = getMachineA(c1x, c1y);
      float pB = getMachineB(c1x, c1y);
    
      // do the move
      runSpeed = desiredSpeed(linesegs, runSpeed, currentAcceleration*4);
      
//      Serial.print("Setting speed:");
//      Serial.println(runSpeed);
      
      motorA.setSpeed(runSpeed);
      motorB.setSpeed(runSpeed);
      changeLength(pA, pB);
  
      // one line less to do!
      linesegs--;
    }
    // reset back to "normal" operation
    reportingPosition = true;
    usingAcceleration = true;
    reportPosition();
  }
  else
  {
    Serial.print(MSG_E_STR);
    Serial.println(F("Line is not on the page. Skipping it."));
  }
//  outputAvailableMemory();
}

// Work out and return a new speed.
// Subclasses can override if they want
// Implement acceleration, deceleration and max speed
// Negative speed is anticlockwise
// This is called:
//  after each step
//  after user changes:
//   maxSpeed
//   acceleration
//   target position (relative or absolute)
float desiredSpeed(long distanceTo, float currentSpeed, float acceleration)
{
    float requiredSpeed;

    if (distanceTo == 0)
	return 0.0f; // We're there

    // sqrSpeed is the signed square of currentSpeed.
    float sqrSpeed = sq(currentSpeed);
    if (currentSpeed < 0.0)
      sqrSpeed = -sqrSpeed;
      
    float twoa = 2.0f * acceleration; // 2ag
    
    // if v^^2/2as is the the left of target, we will arrive at 0 speed too far -ve, need to accelerate clockwise
    if ((sqrSpeed / twoa) < distanceTo)
    {
	// Accelerate clockwise
	// Need to accelerate in clockwise direction
	if (currentSpeed == 0.0f)
	    requiredSpeed = sqrt(twoa);
	else
	    requiredSpeed = currentSpeed + fabs(acceleration / currentSpeed);

	if (requiredSpeed > currentMaxSpeed)
	    requiredSpeed = currentMaxSpeed;
    }
    else
    {
	// Decelerate clockwise, accelerate anticlockwise
	// Need to accelerate in clockwise direction
	if (currentSpeed == 0.0f)
	    requiredSpeed = -sqrt(twoa);
	else
	    requiredSpeed = currentSpeed - fabs(acceleration / currentSpeed);
	if (requiredSpeed < -currentMaxSpeed)
	    requiredSpeed = -currentMaxSpeed;
    }
    
    //Serial.println(requiredSpeed);
    return requiredSpeed;
}
#endif
