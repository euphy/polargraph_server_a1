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
  if (isPenUp == false)
  {
    penlift_movePenUp();
  }
}

void penlift_movePenUp()
{
  penHeight.attach(PEN_HEIGHT_SERVO_PIN);
  for (int i=DOWN_POSITION; i<UP_POSITION; i++) {
//    Serial.println(i);
    penHeight.write(i);
    delay(10);
  }
  penHeight.detach();
  isPenUp = true;
}  


void penlift_penDown()
{
  if (isPenUp == true)
  {
    penlift_movePenDown();
  }
}
void penlift_movePenDown()
{
  penHeight.attach(PEN_HEIGHT_SERVO_PIN);
  for (int i=UP_POSITION; i>DOWN_POSITION; i--) {
//    Serial.println(i);
    penHeight.write(i);
    delay(5);
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
