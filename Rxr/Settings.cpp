#include "Settings.h"
#include "eepromImpl.h"
#include "events.h"

Settings::Settings() {
  // if values are saved might to load them up here  
}

void Settings::SetMaxVelocity(long val)
{
  eeprom::WriteInt32(MAX_VEL_LOC, val); 
  events::set_dirty(true);
}

long Settings::GetMaxVelocity()
{
  long val;
  eeprom::ReadInt32(MAX_VEL_LOC, &val);
  return val;
}

void Settings::SetAcceleration(long val)
{
  eeprom::WriteInt32(ACCEL_LOC, val); 
  events::set_dirty(true);
}

long Settings::GetAcceleration()
{
  long val;
  eeprom::ReadInt32(ACCEL_LOC, &val);
  return val;
}

void Settings::SetMicrosteps(int val)
{
  eeprom::WriteInt16(MICROSTEPS_LOC, val); 
  events::set_dirty(true);
}

int Settings::GetMicrosteps()
{
  int val;
  eeprom::ReadInt16(MICROSTEPS_LOC, &val);
  return val;
}

void Settings::SetAntenna(int val)
{
  eeprom::WriteInt16(ANTENNA_LOC, val); 
  events::set_dirty(true);
}

int Settings::GetAntenna()
{
  int val;
  eeprom::ReadInt16(ANTENNA_LOC, &val);
  return val;
}

void Settings::SetChannel(int val)
{
  eeprom::WriteInt16(CHANNEL_LOC, val); 
  events::set_dirty(true);
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
  events::set_dirty(true);
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

void Settings::SetZModeMaxVelocity(long val)
{
  eeprom::WriteInt32(ZMODE_MAX_VEL_LOC, val); 
  events::set_dirty(true);
}

long Settings::GetZModeMaxVelocity()
{
  long val;
  eeprom::ReadInt32(ZMODE_MAX_VEL_LOC, &val);
  return val;
}

void Settings::SetZModeAcceleration(long val)
{
  eeprom::WriteInt32(ZMODE_ACCEL_LOC, val); 
  events::set_dirty(true);
}

long Settings::GetZModeAcceleration()
{
  long val;
  eeprom::ReadInt32(ZMODE_ACCEL_LOC, &val);
  return val;
}
