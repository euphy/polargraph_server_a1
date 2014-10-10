/**
*  Polargraph Server. - IMPLEMENTATION
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_a1

Implementation of executeCommand for UNO-sized boards 
without "store" features. Doesn't actually do anything except
pass the command to the real executeCommand.  */
void impl_processCommand(String com)
{
  impl_executeCommand(com);
}

void impl_runBackgroundProcesses()
{
  long motorCutoffTime = millis() - lastOperationTime;
  if ((automaticPowerDown) && (powerIsOn) && (motorCutoffTime > motorIdleTimeBeforePowerDown))
  {
    Serial.print(MSG_I_STR);
    Serial.println(F("Powering down."));
    impl_releaseMotors();
  }
}

void impl_loadMachineSpecFromEeprom()
{}

void impl_executeCommand(String &com)
{
  if (exec_executeBasicCommand(com))
  {
    // that's nice, it worked
  }
  else
  {
    comms_unrecognisedCommand(com);
    comms_ready();
  }
}

void impl_engageMotors()
{
  motorA.enableOutputs();
  motorB.enableOutputs();
  powerIsOn = true;
  motorA.runToNewPosition(motorA.currentPosition()+4);
  motorB.runToNewPosition(motorB.currentPosition()+4);
  motorA.runToNewPosition(motorA.currentPosition()-4);
  motorB.runToNewPosition(motorB.currentPosition()-4);
}

void impl_releaseMotors()
{
  motorA.disableOutputs();
  motorB.disableOutputs();  
#ifdef ADAFRUIT_MOTORSHIELD_V2
  afMotorA->release();
  afMotorB->release();
#endif
#ifdef PENLIFT 
  penlift_penUp();
#endif
  powerIsOn = false;  
}

