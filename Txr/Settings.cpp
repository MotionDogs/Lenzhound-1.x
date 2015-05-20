#include "Settings.h"
#include "eepromImpl.h"

Settings::Settings() {
  // if values are saved might to load them up here  
}

void Settings::SetChannel(int val)
{
  eeprom::WriteInt16(CHANNEL_LOC, val); 
}

int Settings::GetChannel()
{
  int val;
  eeprom::ReadInt16(CHANNEL_LOC, &val);
  return val;
}

void Settings::SetPALevel(int val)
{
  eeprom::WriteInt16(PA_LEVEL_LOC, val); 
}

int Settings::GetPALevel()
{
  int val;
  eeprom::ReadInt16(PA_LEVEL_LOC, &val);
  return val;
}

void Settings::SetCalPos1(int val)
{
  eeprom::WriteInt16(CAL_POS_1_LOC, val);
}

int Settings::GetCalPos1()
{
  int val;
  eeprom::ReadInt16(CAL_POS_1_LOC, &val);
  return val;
}

void Settings::SetCalPos2(int val)
{
  eeprom::WriteInt16(CAL_POS_2_LOC, val);
}

int Settings::GetCalPos2()
{
  int val;
  eeprom::ReadInt16(CAL_POS_2_LOC, &val);
  return val;
}

void Settings::SetSavedPos(int val, int index)
{
  eeprom::WriteInt16(SAVED_POS_ARR_LOC + index*2, val);
}

int Settings::GetSavedPos(int index){
  int val;
  eeprom::ReadInt16(SAVED_POS_ARR_LOC + index*2, &val);
  return val;
}

void Settings::SetStartInCal(int startInCal)
{
  eeprom::WriteInt16(START_IN_CAL_LOC, startInCal);
}

int Settings::GetStartInCal()
{
  return false;//this will be changed when we have an options menu
  int val;
  eeprom::ReadInt16(START_IN_CAL_LOC, &val);
  return val;
}

int Settings::GetDataRate()
{
  return 0;
}
