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

int Settings::GetDataRate()
{
  return 0;
}
