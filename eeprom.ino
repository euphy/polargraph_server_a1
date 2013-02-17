/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  http://code.google.com/p/polargraph/

EEPROM.

This is one of the core files for the polargraph server program.  
Has a couple of little helper methods for reading and writing
ints and floats to EEPROM using the EEPROM library.

But mostly it contains the loadMachineSpecFromEeprom method, which is
used to retrieve the machines saved values when it restarts, or 
whenever a value is written to the EEPROM.

*/
void eeprom_resetEeprom()
{
  for (int i = 0; i <20; i++)
  {
    EEPROM.write(i, 0);
  }
  eeprom_loadMachineSpecFromEeprom();
}
void eeprom_dumpEeprom()
{
  for (int i = 0; i <20; i++)
  {
    Serial.print(i);
    Serial.print(". ");
    Serial.println(EEPROM.read(i));
  }
}  


void eeprom_loadMachineSpecFromEeprom()
{
  impl_loadMachineSpecFromEeprom();

  machineWidth = eeprom_EEPROMReadInt(EEPROM_MACHINE_WIDTH);
  if (machineWidth < 1)
  {
    machineWidth = defaultMachineWidth;
  }
  Serial.print(F("Loaded machine width:"));
  Serial.println(machineWidth);
  
  machineHeight = eeprom_EEPROMReadInt(EEPROM_MACHINE_HEIGHT);
  if (machineHeight < 1)
  {
    machineHeight = defaultMachineHeight;
  }
  Serial.print(F("Loaded machine height:"));
  Serial.println(machineHeight);

  mmPerRev = eeprom_EEPROMReadInt(EEPROM_MACHINE_MM_PER_REV);
  if (mmPerRev < 1)
  {
    mmPerRev = defaultMmPerRev;
  }
  Serial.print(F("Loaded mm per rev:"));
  Serial.println(mmPerRev);

  motorStepsPerRev = eeprom_EEPROMReadInt(EEPROM_MACHINE_STEPS_PER_REV);
  if (motorStepsPerRev < 1)
  {
    motorStepsPerRev = defaultStepsPerRev;
  }
  Serial.print(F("Loaded motor steps per rev:"));
  Serial.println(motorStepsPerRev);

  String name = "";
  for (int i = 0; i < 8; i++)
  {
    char b = EEPROM.read(EEPROM_MACHINE_NAME+i);
    name = name + b;
  }
  
  if (name[0] == 0)
    name = DEFAULT_MACHINE_NAME;
  maxLength = 0;
  machineName = name;
  Serial.print(F("Loaded machine name:"));
  Serial.println(machineName);
  
  stepMultiplier = eeprom_EEPROMReadInt(EEPROM_MACHINE_STEP_MULTIPLIER);
  if (stepMultiplier < 1)
  {
    stepMultiplier = defaultStepMultiplier;
  }
  Serial.print(F("Loaded motor step multiplier:"));
  Serial.println(stepMultiplier);  

  mmPerStep = mmPerRev / multiplier(motorStepsPerRev);
  stepsPerMM = multiplier(motorStepsPerRev) / mmPerRev;
  
  Serial.print(F("Recalculated mmPerStep ("));
  Serial.print(mmPerStep);
  Serial.print(F(") and stepsPerMM ("));
  Serial.print(stepsPerMM);
  Serial.print(F(")"));
  Serial.println();

  pageWidth = machineWidth * stepsPerMM;
  Serial.print(F("Recalculated pageWidth in steps ("));
  Serial.print(pageWidth);
  Serial.print(F(")"));
  Serial.println();
  pageHeight = machineHeight * stepsPerMM;
  Serial.print(F("Recalculated pageHeight in steps ("));
  Serial.print(pageHeight);
  Serial.print(F(")"));
  Serial.println();

  maxLength = 0;
}

//This function will write a 2 byte integer to the eeprom at the specified address and address + 1
void eeprom_EEPROMWriteInt(int p_address, int p_value)
{
  Serial.print(F("Writing Int "));
  Serial.print(p_value);
  Serial.print(F(" to address "));
  Serial.println(p_address);

  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);
  EEPROM.write(p_address, lowByte);
  EEPROM.write(p_address + 1, highByte);
}

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
unsigned int eeprom_EEPROMReadInt(int p_address)
{
  byte lowByte = EEPROM.read(p_address);
  byte highByte = EEPROM.read(p_address + 1);
  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

