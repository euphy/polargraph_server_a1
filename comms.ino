/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  http://code.google.com/p/polargraph/

Comms.

This is one of the core files for the polargraph server program.  
Comms can mean "communications" or "commands", either will do, since
it contains methods for reading commands from the serial port.

*/

String comms_waitForNextCommand()
{
  // send ready
  // wait for instruction
  int idleTime = millis();
  
  // do this bit until we get a command confirmed
  // idle
  String inS = "";

  // loop while there's no commands coming in
  while (inS.length() == 0)
  {
    impl_runBackgroundProcesses();
    // idle time is spent in this loop.
    int timeSince = millis() - idleTime;
    if (timeSince > rebroadcastReadyInterval)
    {
      // issue a READY every 5000ms of idling
      comms_ready();
      idleTime = millis();
    }
    
    // and now read the command if one exists
    // this also sets usingCrc AND commandConfirmed
    // to true or false
    inS = comms_readCommand();

    // if it's using the CRC check, then confirmation is easy
    if (usingCrc && !commandConfirmed)
    {
      comms_requestResend();
      inS = "";
    }
  }
  
  
  // CRC was ok, or we aren't using one
  idleTime = millis();
  lastOperationTime = millis();
  lastInteractionTime = lastOperationTime;

  return inS;
}

boolean comms_parseCommand(String inS)
{
  if (inS.endsWith(CMD_END))
  {
    comms_extractParams(inS);
    return true;
  }
  else
    return false;
}  

String comms_readCommand()
{
  // check if data has been sent from the computer:
  char inString[INLENGTH+1];
  int inCount = 0;
  while (Serial.available() > 0)
  {
    char ch = Serial.read();       // get it
    delay(1);
    inString[inCount] = ch;
    if (ch == INTERMINATOR)
    {
      Serial.flush();
      break;
    }
    inCount++;
  }
  inString[inCount] = 0;                     // null terminate the string
  String inS = inString;
  
  // check the CRC for this command
  // and set commandConfirmed true or false
  int colonPos = inS.lastIndexOf(":");
  if (colonPos != -1)
  {
    usingCrc = true;
    String cs = inS.substring(colonPos+1);
    long checksum = asLong(cs);
    inS = inS.substring(0, colonPos);
    
    long calcCrc = crc_string(inS);
    
    if (calcCrc == checksum)
    {
      commandConfirmed = true;
    }
    else
    {
      Serial.print(F("I got "));
      Serial.println(inString);
      Serial.print(F("Checksum not matched!:"));
      Serial.println(calcCrc);
      commandConfirmed = false;
    }

  }
  else
  {
    // then fall back and do the ACK - no action here
    usingCrc = false;
    commandConfirmed = false;
  }

  return inS;
}

void comms_parseAndExecuteCommand(String &in)
{
  boolean commandParsed = comms_parseCommand(in);
  if (commandParsed)
  {
    impl_processCommand(lastCommand);
    in = "";
    commandConfirmed = false;
    comms_ready();
  }
  else
  {
    Serial.print(F("Command ("));
    Serial.print(in);
    Serial.println(F(") not parsed."));
  }
  inNoOfParams = 0;
  
}


void comms_extractParams(String inS) {
  
  // get number of parameters
  // by counting commas
  int length = inS.length();
  
  int startPos = 0;
  int paramNumber = 0;
  for (int i = 0; i < length; i++) {
    if (inS.charAt(i) == ',') {
      String param = inS.substring(startPos, i);
      startPos = i+1;
      
      switch(paramNumber) {
        case 0:
          inCmd = param;
          break;
        case 1:
          inParam1 = param;
          break;
        case 2:
          inParam2 = param;
          break;
        case 3:
          inParam3 = param;
          break;
        case 4:
          inParam4 = param;
          break;
        default:
          break;
      }
      paramNumber++;
    }
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

void comms_establishContact() 
{
  comms_ready();
}
void comms_ready()
{
  Serial.println(READY);
}
void comms_drawing()
{
  Serial.println(DRAWING);
}
void comms_requestResend()
{
  Serial.println(RESEND);
}
void comms_unrecognisedCommand(String &com)
{
  Serial.print(F("Sorry, "));
  Serial.print(com);
  Serial.println(F(" isn't a command I recognise."));
}  


