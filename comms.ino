/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_a1

Comms.

This is one of the core files for the polargraph server program.  
Comms can mean "communications" or "commands", either will do, since
it contains methods for reading commands from the serial port.

*/

char *comms_waitForNextCommand()
{
  // send ready
  // wait for instruction
  int idleTime = millis();
  char buf[INLENGTH+1];
  int bufPos = 0;

  // loop while there's there isn't a terminated command.
  // (Note this might mean characters ARE arriving, but just
  //  that the command hasn't been finished yet.)
  boolean terminated = false;
  while (!terminated)
  {
    // idle time is mostly spent in this loop.
    impl_runBackgroundProcesses();
    int timeSince = millis() - idleTime;
    if (timeSince > rebroadcastReadyInterval)
    {
      // issue a READY every 5000ms of idling
      comms_ready();
      idleTime = millis();
    }
    
    // And now read the command if one exists.
    if (Serial.available() > 0)
    {
      // Get the char
      char ch = Serial.read();
      
      // look at it, if it's a terminator, then lets terminate the string
      if (ch == INTERMINATOR) {
        buf[bufPos] = 0; // null terminate the string
        terminated = true;
      } else {
        // otherwise, just add it into the buffer
        buf[bufPos] = ch;
        bufPos++;
      }
    }
  }

  // if it's using the CRC check, then confirmation is easy
  // CRC was ok, or we aren't using one
  idleTime = millis();
  lastOperationTime = millis();
  lastInteractionTime = lastOperationTime;
  return buf;
}

boolean comms_parseCommand(char* inS)
{
  char* sub = strstr(inS, CMD_END);
  if (strcmp(sub, CMD_END) == 0) 
  {
    comms_extractParams(inS);
    return true;
  }
  else
    return false;
}  


void comms_parseAndExecuteCommand(char* inS)
{
  boolean commandParsed = comms_parseCommand(inS);
  if (commandParsed)
  {
    impl_processCommand(lastCommand);
    inS = "";
    commandConfirmed = false;
    comms_ready();
  }
  else
  {
    Serial.print(F("MSG_E_STRComm ("));
    Serial.print(inS);
    Serial.println(F(") not parsed."));
  }
  inNoOfParams = 0;
  
}


void comms_extractParams(char* inS) 
{
  
  char * in;
  strcpy(in, inS);
  char * param;
  
  int paramNumber = 0;
  param = strtok(in, COMMA);
  while (param != NULL) 
  {
      switch(paramNumber) 
      {
        case 0:
          strcpy(inCmd, param);
//          inCmd = param;
          break;
        case 1:
          strcpy(inParam1, param);
//          inParam1 = param;
          break;
        case 2:
          strcpy(inParam2, param);
//          inParam2 = param;
          break;
        case 3:
          strcpy(inParam3, param);
//          inParam3 = param;
          break;
        case 4:
          strcpy(inParam4, param);
//          inParam4 = param;
          break;
        default:
          break;
      }
      param = strtok(NULL, COMMA);
      paramNumber++;
  }

  inNoOfParams = paramNumber;
  
//    Serial.print(F("Command:"));
//    Serial.print(inCmd);
//    Serial.print(F(", p1:"));
//    Serial.print(inParam1);
//    Serial.print(F(", p2:"));
//    Serial.print(inParam2);
//    Serial.print(F(", p3:"));
//    Serial.print(inParam3);
//    Serial.print(F(", p4:"));
//    Serial.println(inParam4);
}


long asLong(String inParam)
{
  char paramChar[inParam.length() + 1];
  inParam.toCharArray(paramChar, inParam.length() + 1);
  return atol(paramChar);
}
int asInt(String inParam)
{
  char paramChar[inParam.length() + 1];
  inParam.toCharArray(paramChar, inParam.length() + 1);
  return atoi(paramChar);
}
byte asByte(String inParam)
{
  int i = asInt(inParam);
  return (byte) i;
}
float asFloat(String inParam)
{
  char paramChar[inParam.length() + 1];
  inParam.toCharArray(paramChar, inParam.length() + 1);
  return atof(paramChar);
}

void comms_ready()
{
  Serial.println(F(READY_STR));
}
void comms_drawing()
{
  Serial.println(F(DRAWING_STR));
}
void comms_requestResend()
{
  Serial.println(F(RESEND_STR));
}
void comms_unrecognisedCommand(String &com)
{
  Serial.print(F("MSG_E_STR"));
  Serial.print(com);
  Serial.println(F(" not recognised."));
}  


