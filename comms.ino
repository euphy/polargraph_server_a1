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
  long idleTime = millis();
  char buf[INLENGTH+1];
  int bufPos = 0;
  long lastRxTime = 0L;

  // loop while there's there isn't a terminated command.
  // (Note this might mean characters ARE arriving, but just
  //  that the command hasn't been finished yet.)
  boolean terminated = false;
  while (!terminated)
  {
#ifdef DEBUG_COMMS    
    Serial.print(F("."));
#endif    
    long timeSince = millis() - lastRxTime;
    
    // If the buffer is being filled, but hasn't received a new char in less than 100ms,
    // just cancel it. It's probably just junk.
    if (bufPos != 0 && timeSince > 100)
    {
#ifdef DEBUG_COMMS
      Serial.print(F("Timed out:"));
      Serial.println(timeSince);
#endif
      // Clear the buffer and reset the position if it took too long
      for (int i = 0; i<INLENGTH; i++) {
        buf[i] = 0;
      }
      bufPos = 0;
    }
    
    // idle time is mostly spent in this loop.
    impl_runBackgroundProcesses();
    timeSince = millis() - idleTime;
    if (timeSince > rebroadcastReadyInterval)
    {
      // issue a READY every 5000ms of idling
#ifdef DEBUG_COMMS      
      Serial.println("");
#endif
      comms_ready();
      idleTime = millis();
    }
    
    // And now read the command if one exists.
    if (Serial.available() > 0)
    {
      // Get the char
      char ch = Serial.read();
#ifdef DEBUG_COMMS
      Serial.print(F("ch: "));
      Serial.println(ch);
#endif
      
      // look at it, if it's a terminator, then lets terminate the string
      if (ch == INTERMINATOR) {
        buf[bufPos] = 0; // null terminate the string
        terminated = true;
#ifdef DEBUG_COMMS
        Serial.println(F("Term'd"));
#endif
        for (int i = bufPos; i<INLENGTH-1; i++) {
          buf[i] = 0;
        }
        
      } else {
        // otherwise, just add it into the buffer
        buf[bufPos] = ch;
        bufPos++;
      }
#ifdef DEBUG_COMMS
      Serial.print(F("buf: "));
      Serial.println(buf);
      Serial.print(F("Bufpos: "));
      Serial.println(bufPos);
#endif
      lastRxTime = millis();
    }
  }

  idleTime = millis();
  lastOperationTime = millis();
  lastInteractionTime = lastOperationTime;
  Serial.print("xbuf: ");
  Serial.println(buf);
  return buf;
}

void comms_parseAndExecuteCommand(char* inS)
{
  Serial.print("3inS: ");
  Serial.println(inS);

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
    Serial.print(MSG_E_STR);
    Serial.print(F("Comm ("));
    Serial.print(inS);
    Serial.println(F(") not parsed."));
  }
  inNoOfParams = 0;
  
}

boolean comms_parseCommand(char* inS)
{
  Serial.print("1inS: ");
  Serial.println(inS);
  // strstr returns a pointer to the location of ",END" in the incoming string (inS).
  char* sub = strstr(inS, CMD_END);
  Serial.print("2inS: ");
  Serial.println(inS);
  sub[strlen(CMD_END)] = 0;
  Serial.print("4inS: ");
  Serial.println(inS);

  Serial.print("2Sub: ");
  Serial.println(sub);
  
  
  Serial.println(CMD_END);
  if (strcmp(sub, CMD_END) == 0) 
  {
    comms_extractParams(inS);
    return true;
  }
  else
    return false;
}  

void comms_extractParams(char* inS) 
{
  
  char * in;
  strcpy(in, inS);
  char * param;
  
  Serial.print("In: ");
  Serial.println(in);
  Serial.print("InS: ");
  Serial.println(inS);
  
  int paramNumber = 0;
  param = strtok(in, COMMA);
  Serial.print("Param: ");
  Serial.println(param);
  while (param != NULL) 
  {
      switch(paramNumber) 
      {
        case 0:
          strcpy(inCmd, param);
          break;
        case 1:
          strcpy(inParam1, param);
          break;
        case 2:
          strcpy(inParam2, param);
          break;
        case 3:
          strcpy(inParam3, param);
          break;
        case 4:
          strcpy(inParam4, param);
          break;
        default:
          break;
      }
      param = strtok(NULL, COMMA);
      paramNumber++;
  }

  inNoOfParams = paramNumber;

#ifdef DEBUG_COMMS
    Serial.print(F("Command:"));
    Serial.print(inCmd);
    Serial.print(F(", p1:"));
    Serial.print(inParam1);
    Serial.print(F(", p2:"));
    Serial.print(inParam2);
    Serial.print(F(", p3:"));
    Serial.print(inParam3);
    Serial.print(F(", p4:"));
    Serial.println(inParam4);
#endif
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
  Serial.print(F(MSG_E_STR));
  Serial.print(com);
  Serial.println(F(" not recognised."));
}  


