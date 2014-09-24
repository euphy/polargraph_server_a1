/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_a1

Penlift.

This is one of the core files for the polargraph server program.  
This file contains the servo calls that raise or lower the pen from
the page.

The behaviour of the pen lift is this:

If a simple "pen up", or "pen lift" command is received ("C14,END"), then the machine will
not try to lift the pen if it thinks it is already up.  It checks the value of the 
global boolean variable "isPenUp" to decide this.

If a qualified "pen up" is received, that is one that includes a pen position (eg "C14,150,END"),
then the global "up" position variable is updated, and the servo is moved to that position,
even if it already is "up".  Because naturally, if the up position has changed, even if it
is already up, there's a good chance it won't be up enough.

The same goes for the 

*/
#ifdef PENLIFT
void penlift_movePen(int start, int end, int delay_ms)
{
  penHeight.attach(PEN_HEIGHT_SERVO_PIN);
  if(start < end)
  {
    for (int i=start; i<=end; i++) 
    {
      penHeight.write(i);
      delay(delay_ms);
#ifdef DEBUG_PENLIFT
      Serial.println(i);
#endif
    }
  }
  else
  {
    for (int i=start; i>=end; i--) 
    {
      penHeight.write(i);
      delay(delay_ms);
#ifdef DEBUG_PENLIFT
      Serial.println(i);
#endif
    }
  }
  penHeight.detach();
}

void penlift_penUp()
{
  if (inNoOfParams > 1)
  {
#ifdef DEBUG_PENLIFT
    Serial.print("Penup with params");
#endif
    int positionToMoveFrom = isPenUp ? upPosition : downPosition;
    upPosition = atoi(inParam1);
    penlift_movePen(positionToMoveFrom, upPosition, penLiftSpeed);
  }
  else
  {
    if (isPenUp == false)
    {
      penlift_movePen(downPosition, upPosition, penLiftSpeed);
    }
  }
  isPenUp = true;
}

void penlift_penDown()
{
  // check to see if this is a multi-action command (if there's a
  // parameter then this sets the "down" motor position too).
  if (inNoOfParams > 1)
  {
    int positionToMoveFrom = isPenUp ? upPosition : downPosition;
    downPosition = atoi(inParam1);
    penlift_movePen(positionToMoveFrom, downPosition, penLiftSpeed);
  }
  else
  {
    if (isPenUp == true)
    {
      penlift_movePen(upPosition, downPosition, penLiftSpeed);
    }
  }
  isPenUp = false;
}
#endif
