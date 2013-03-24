/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  http://code.google.com/p/polargraph/

Penlift.

This is one of the core files for the polargraph server program.  
This file contains the servo calls that raise or lower the pen from
the page.

*/
void penlift_penUp()
{
  if (inNoOfParams > 1)
  {
    upPosition = asInt(inParam1);
  }
  if (isPenUp == false)
  {
    penlift_movePenUp();
  }
}

void penlift_movePenUp()
{
  penHeight.attach(PEN_HEIGHT_SERVO_PIN);
  for (int i=downPosition; i<upPosition; i++) {
    penHeight.write(i);
    delay(penLiftSpeed);
  }
  penHeight.detach();
  isPenUp = true;
}  


void penlift_penDown()
{
  // check to see if this is a multi-action command (if there's a
  // parameter then this sets the "down" motor position too).
  if (inNoOfParams > 1)
  {
    downPosition = asInt(inParam1);
  }
  if (isPenUp == true)
  {
    penlift_movePenDown();
  }
}
void penlift_movePenDown()
{
  penHeight.attach(PEN_HEIGHT_SERVO_PIN);
  for (int i=upPosition; i>downPosition; i--) {
//    Serial.println(i);
    penHeight.write(i);
    delay(penLiftSpeed);
  }
  penHeight.detach();
  isPenUp = false;
}

void penlift_testServoRange()
{
  penHeight.attach(PEN_HEIGHT_SERVO_PIN);
  for (int i=0; i<200; i++) {
    Serial.println(i);
    penHeight.write(i);
    delay(15);
  }
  penHeight.detach();
  
}
