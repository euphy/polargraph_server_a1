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
#if MICROCONTROLLER == MC_UNO
  impl_executeCommand(com);
#endif
#if MICROCONTROLLER == MC_MEGA
  // check for change mode commands
  if (com.startsWith(CMD_MODE_STORE_COMMANDS)
  || com.startsWith(CMD_MODE_LIVE))
  {
    Serial.println("Changing mode.");
    impl_executeCommand(com);
  }
  // else execute / store the command
  else if (storeCommands)
  {
    Serial.print(F("Storing command:"));
    Serial.println(com);
    sd_storeCommand(com);
  }
  else
  {
    impl_executeCommand(com);
  }
#endif
}

void impl_executeCommand(String &com)
{
  if (exec_executeBasicCommand(com))
  {
    // that's nice, it worked
#ifdef DEBUG
    Serial.println("Basic Command executed.");
#endif
  }
#if MICROCONTROLLER == MC_MEGA
  else if (com.startsWith(CMD_DRAWCIRCLEPIXEL))
    curves_pixel_drawCircularPixel();
//  else if (com.startsWith(CMD_TESTPATTERN))
//    testPattern();
//  else if (com.startsWith(CMD_TESTPENWIDTHSCRIBBLE))
//    impl_pixel_testPenWidthScribble();
//  else if (com.startsWith(CMD_DRAWSAWPIXEL))
//    impl_pixel_drawSawtoothPixel();
//  else if (com.startsWith(CMD_DRAWDIRECTIONTEST))
//    impl_exec_drawTestDirectionSquare();
  else if (com.startsWith(CMD_MODE_STORE_COMMANDS))
    impl_exec_changeToStoreCommandMode();
  else if (com.startsWith(CMD_MODE_LIVE))
    impl_exec_changeToLiveCommandMode();
  else if (com.startsWith(CMD_MODE_EXEC_FROM_STORE))
    impl_exec_execFromStore();
//  else if (com.startsWith(CMD_RANDOM_DRAW))
//    drawRandom();
  else if (com.startsWith(CMD_SET_ROVE_AREA))
    rove_setRoveArea();
  else if (com.startsWith(CMD_START_TEXT))
    rove_startText();
  else if (com.startsWith(CMD_DRAW_SPRITE))
    sprite_drawSprite();
  else if (com.startsWith(CMD_DRAW_RANDOM_SPRITE))
    sprite_drawRandomPositionedSprite();
  else if (com.startsWith(CMD_CHANGELENGTH_RELATIVE))
    exec_changeLength();
//  else if (com.startsWith(CMD_SWIRLING))
//    rove_controlSwirling();
  else if (com.startsWith(CMD_DRAW_NORWEGIAN))
    rove_drawNorwegianFromFile();
  else if (com.startsWith(CMD_DRAW_NORWEGIAN_OUTLINE))
    rove_drawRoveAreaFittedToImage();
#endif
  else
  {
    comms_unrecognisedCommand(com);
    comms_ready();
  }
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

#if MICROCONTROLLER == MC_MEGA
void impl_exec_execFromStore()
{
  String fileToExec = inParam1;
  if (fileToExec != "")
  {
    currentlyDrawingFromFile = true;
    Serial.print("Filename to read from: ");
    Serial.println(fileToExec);
    commandFilename = fileToExec;
    impl_exec_execFromStore(commandFilename);
    currentlyDrawingFromFile = true;
  }
  else
  {
    Serial.println("No filename supplied to read from.");
  }
  
}

void impl_exec_execFromStore(String inFilename)
{
//  if (inFilename != "")
//  {
//    String noBlanks = "";
//    // remove blanks
//    for (int i = 0; i<inFilename.length(); i++)
//    {
//      if (inFilename[i] != ' ')
//        noBlanks = noBlanks + inFilename[i];
//    }
//    
//    char filename[noBlanks.length()+1];
//    noBlanks.toCharArray(filename, noBlanks.length()+1);
//#ifdef DEBUG_SD    
//    Serial.print("Array to read from: ");
//    Serial.println(filename);
//#endif
//    File readFile = SD.open(filename, FILE_READ);
//    if (readFile)
//    {
//      Serial.print("Opened file:");
//      Serial.println(noBlanks);
//      String command = "";
//      while (readFile.available() && currentlyDrawingFromFile)
//      {
//#ifdef DEBUG_SD        
//        Serial.println("Reading...");
//        // poll for input
//#endif
//        char ch = readFile.read();
//#ifdef DEBUG_SD        
//        Serial.print(".");
//        Serial.print(ch);
//        Serial.print("-");
//#endif
//        if (ch == 13 || ch == 10)
//        {
//#ifdef DEBUG_SD        
//          Serial.println("New line");
//#endif
//          // execute the line
//          command.trim();
//          boolean commandParsed = comms_parseCommand(command);
//          if (commandParsed)
//          {
//#ifdef DEBUG_SD        
//            Serial.println("Stored command parsed.");
//#endif
//            Serial.print(F("Executing command:"));
//            Serial.println(command);
//            if (echoingStoredCommands) lcd_echoLastCommandToDisplay(command, inFilename+": ");
//            impl_executeCommand(command);
//          }
//#ifdef DEBUG_SD        
//          else Serial.println("Stored command WAS NOT parsed.");
//#endif            
//          command = "";
//          lcd_checkForInput();
//        }
//        else
//          command += ch;
//
//#ifdef DEBUG_SD        
//        Serial.print("Command building:");
//        Serial.println(command);
//#endif
//      }
//      Serial.println("Finished with the file.");
//      currentlyDrawingFromFile = false;
//      readFile.close();
//    }
//    else
//    {
//      Serial.println("Couldn't find that file, btw.");
//      currentlyDrawingFromFile = false;
//    }
//  }
//  else
//  {
//    Serial.println("No filename supplied to read from.");
//    currentlyDrawingFromFile = false;
//  }
//  
}

void impl_exec_changeToStoreCommandMode()
{
  String newfilename = inParam1;
  String newFile = inParam2;
  if (newfilename != "")
  {
    Serial.print("Filename for command store: ");
    Serial.println(newfilename);
    storeCommands = true;
    commandFilename = newfilename;
    if (newFile.equals("R"))
    {
      // delete file if it exists
      char filename[newfilename.length()+1];
      newfilename.toCharArray(filename, newfilename.length()+1);
      
      if (SD.exists(filename))
      {
        // file exists
        Serial.println(F("File already exists."));
        boolean removed = SD.remove(filename);
        if (removed)
          Serial.println(F("File removed."));
        
      }
    }
  }
  else
  {
    Serial.println("No filename supplied to write to.");
  }
}

void impl_exec_changeToLiveCommandMode()
{
  Serial.println(F("Changing back to live mode."));
  storeCommands = false;
}
#endif

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


