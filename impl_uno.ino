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
{}

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
  motorA.runToNewPosition(motorA.currentPosition()+4);
  motorB.runToNewPosition(motorB.currentPosition()+4);
  motorA.runToNewPosition(motorA.currentPosition()-4);
  motorB.runToNewPosition(motorB.currentPosition()-4);
}

void impl_releaseMotors()
{
  penlift_penUp();
}

void impl_transform(float &tA, float &tB)
{ 
}
