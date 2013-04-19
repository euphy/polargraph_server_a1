/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  http://code.google.com/p/polargraph/

Util.

This is one of the core files for the polargraph server program.  

This has all the methods that let the rest actually work, including
the geometry routines that convert from the different coordinates
systems, and do transformations.

*/
long multiplier(int in)
{
  return multiplier((long) in);
}
long multiplier(long in)
{
  return in * stepMultiplier;
}
float multiplier(float in)
{
  return in * stepMultiplier;
}
long divider(long in)
{
  return in / stepMultiplier;
}


void transform(float &tA, float &tB)
{
//  Serial.print("In tA:");
//  Serial.println(tA);
//  Serial.print("In tB:");
//  Serial.println(tB);

  tA = tA * scaleX;
  tB = tB * scaleY;

  // rotate
  
  if (rotateTransform >45 && rotateTransform <= 135) // 90 degree rotation
  {
    float temp = tA;
    tA = -tB;
    tB = temp;
  }
  else if (rotateTransform > 135 && rotateTransform <= 225) // 180 degree rotation
  {
    tA = -tA;
    tB = -tB;
  }
  else if (rotateTransform > 225 && rotateTransform <= 315) // 270 degree rotation
  {
    float temp = tA;
    tA = tB;
    tB = -temp;
  }
  else // no rotation 
  {
    
  }
  
  tA = tA + translateX;
  tB = tB + translateY;

//  Serial.print("Out tA:");
//  Serial.println(tA);
//  Serial.print("Out tB:");
//  Serial.println(tB);
  
}

void changeLength(long tA, long tB)
{
  changeLength((float)tA, (float)tB);
}

void changeLength(float tA, float tB)
{
//  Serial.println("changeLenth-float");
  lastOperationTime = millis();

  transform(tA,tB);
  
  float currSpeedA = motorA.speed();
  float currSpeedB = motorB.speed();
  
//  Serial.print("A pos: ");
//  Serial.print(motorA.currentPosition());
//  Serial.print(", A target: ");
//  Serial.println(tA);
//  Serial.print("B pos: ");
//  Serial.print(motorB.currentPosition());
//  Serial.print(", B target: ");
//  Serial.println(tB);
  
  
  motorA.setSpeed(0.0);
  motorB.setSpeed(0.0);
  motorA.moveTo(tA);
  motorB.moveTo(tB);
  
  
  if (!usingAcceleration)
  {
    // The moveTo() function changes the speed in order to do a proper
    // acceleration. This counteracts it. Ha.
    
    if (motorA.speed() < 0)
      currSpeedA = -currSpeedA;
    if (motorB.speed() < 0)
      currSpeedB = -currSpeedB;

//    Serial.print("Setting A speed ");
//    Serial.print(motorA.speed());
//    Serial.print(" back to ");
//    Serial.println(currSpeedA);
//    Serial.print("Setting B speed ");
//    Serial.print(motorB.speed());
//    Serial.print(" back to ");
//    Serial.println(currSpeedB);
      
    motorA.setSpeed(currSpeedA);
    motorB.setSpeed(currSpeedB);
  }
  
  
  while (motorA.distanceToGo() != 0 || motorB.distanceToGo() != 0)
  {
//    Serial.print("dA:");
//    Serial.print(motorA.distanceToGo());
//    Serial.print(", dB:");
//    Serial.println(motorB.distanceToGo());
    impl_runBackgroundProcesses();
    if (currentlyRunning)
    {
      if (usingAcceleration)
      {
        motorA.run();
        motorB.run();
      }
      else
      {
//        Serial.print("Run speed..");
//        Serial.println(motorA.speed());
        motorA.runSpeedToPosition();
        motorB.runSpeedToPosition();
      }
    }
  }
  
  reportPosition();
}

void changeLengthRelative(float tA, float tB)
{
  changeLengthRelative((long) tA, (long)tB);
}
void changeLengthRelative(long tA, long tB)
{
  lastOperationTime = millis();
  motorA.move(tA);
  motorB.move(tB);
  
  while (motorA.distanceToGo() != 0 || motorB.distanceToGo() != 0)
  {
    //impl_runBackgroundProcesses();
    if (currentlyRunning)
    {
      if (usingAcceleration)
      {
        motorA.run();
        motorB.run();
      }
      else
      {
        motorA.runSpeedToPosition();
        motorB.runSpeedToPosition();
      }
    }
  }
  
  reportPosition();
}

long getMaxLength()
{
  if (maxLength == 0)
  {
    float length = getMachineA(pageWidth, pageHeight);
    maxLength = long(length+0.5);
    Serial.print("Calculated maxLength: ");
    Serial.println(maxLength);
  }
  return maxLength;
}


float getMachineA(float cX, float cY)
{
  float a = sqrt(sq(cX)+sq(cY));
  return a;
}
float getMachineB(float cX, float cY)
{
  float b = sqrt(sq((pageWidth)-cX)+sq(cY));
  return b;
}

void moveA(int dist)
{
  motorA.move(dist);
  while (motorA.distanceToGo() != 0)
  {
    impl_runBackgroundProcesses();
    if (currentlyRunning)
      motorA.run();
  }
  lastOperationTime = millis();
}

void moveB(int dist)
{
  motorB.move(dist);
  while (motorB.distanceToGo() != 0)
  {
    impl_runBackgroundProcesses();
    if (currentlyRunning)
      motorB.run();
  }
  lastOperationTime = millis();
}

void reportPosition()
{
  if (reportingPosition)
  {
    Serial.print(OUT_CMD_SYNC);
    Serial.print(divider(motorA.currentPosition()));
    Serial.print(COMMA);
    Serial.print(divider(motorB.currentPosition()));
    Serial.println(CMD_END);
    
  //  int cX = getCartesianX();
  //  int cY = getCartesianY(cX, motorA.currentPosition());
  //  Serial.print(OUT_CMD_CARTESIAN);
  //  Serial.print(cX*mmPerStep);
  //  Serial.print(COMMA);
  //  Serial.print(cY*mmPerStep);
  //  Serial.println(CMD_END);
  //
    //outputAvailableMemory();
  }
}




void engageMotors()
{
  impl_engageMotors();
}

void releaseMotors()
{
  impl_releaseMotors();
}


float getCartesianXFP(float aPos, float bPos)
{
  float calcX = (sq(pageWidth) - sq(bPos) + sq(aPos)) / (pageWidth*2);
  return calcX;  
}
float getCartesianYFP(float cX, float aPos) 
{
  float calcY = sqrt(sq(aPos)-sq(cX));
  return calcY;
}

long getCartesianX(float aPos, float bPos)
{
  long calcX = long((pow(pageWidth, 2) - pow(bPos, 2) + pow(aPos, 2)) / (pageWidth*2));
  return calcX;  
}

long getCartesianX() {
  long calcX = getCartesianX(motorA.currentPosition(), motorB.currentPosition());
  return calcX;  
}

long getCartesianY() {
  return getCartesianY(getCartesianX(), motorA.currentPosition());
}
long getCartesianY(long cX, float aPos) {
  long calcY = long(sqrt(pow(aPos,2)-pow(cX,2)));
  return calcY;
}


void outputAvailableMemory()
{
  int avMem = availableMemory();
  if (avMem != availMem)
  {
    availMem = avMem;
    Serial.print(FREE_MEMORY_STRING);
    Serial.print(availMem);
    Serial.println(CMD_END);
  }
}

//from http://www.arduino.cc/playground/Code/AvailableMemory
int availableMemory() {
  uint8_t * heapptr, * stackptr;
  stackptr = (uint8_t *)malloc(4);
  heapptr = stackptr;
  free(stackptr);               
  stackptr = (uint8_t *)(SP);
  return stackptr - heapptr;
} 




/*
Calculating CRCs.  Incoming commands have these appended as a way
to check quality.
http://www.excamera.com/sphinx/article-crc.html
*/
unsigned long crc_update(unsigned long crc, byte data)
{
    byte tbl_idx;
    tbl_idx = crc ^ (data >> (0 * 4));
    crc = pgm_read_dword_near(crc_table + (tbl_idx & 0x0f)) ^ (crc >> 4);
    tbl_idx = crc ^ (data >> (1 * 4));
    crc = pgm_read_dword_near(crc_table + (tbl_idx & 0x0f)) ^ (crc >> 4);
    return crc;
}

unsigned long crc_string(String s)
{
  unsigned long crc = ~0L;
  for (int i = 0; i < s.length(); i++)
  {
    crc = crc_update(crc, s.charAt(i));
  }
  crc = ~crc;
  return crc;
}


