#include "Console.h"
#include <CmdMessenger.h>
#include "Settings.h"

CmdMessenger cmdMessenger = CmdMessenger(Serial);
static Settings settings;
static const char SUCCESS[] = "SUCCESS";
static const char ERROR[] = "ERROR";

// This is the list of recognized commands.  
// In order to receive, attach a callback function to these events
enum
{
  COMMAND_LIST = 0, 
  SET_MAX_VEL = 1, 
  SET_ACCEL = 2,
  SET_ANTENNA = 3,
  SET_CHANNEL = 4,
  SET_PALEVEL = 5,
  SET_ZMODE_MAX_VEL = 7,
  SET_ZMODE_ACCEL = 8,
};

Console::Console()
{
}

void Console::Init()
{
  // Adds newline to every command
  cmdMessenger.printLfCr();   

  // Attach my application's user-defined callback methods
  AttachCommandCallbacks();
}

void Console::Run()
{
 // Process incoming serial data, and perform callbacks
  cmdMessenger.feedinSerialData();
}

// Show available commands
void ShowCommands() 
{
  Serial.println("Available commands:Dogbone");
  Serial.println(" 0;                      - This command list");
  Serial.println(" 1,<max velocity>;       - Set Max Velocity (10-48000)");
  Serial.println(" 2,<acceleration>;       - Set Acceleration (1-32000)");
  Serial.println(" 3,<antenna>;            - Set Antenna (0=integrated; 1=remote)");
  Serial.println(" 4,<channel>;            - Set Channel Num (1-82)");
  Serial.println(" 5,<PA level>;           - Set Power Amp Level (0=-18; 1=-12; 2=-6; 3=0)[dBm]");
  Serial.println(" 7,<zmode max velocity>; - Set ZMode Max Velocity (10-48000)");
  Serial.println(" 8,<zmode acceleration>; - Set ZMode Acceleration (1-32000)");
  Serial.println("Current values");
  OnGetAllValues();
}

void OnUnknownCommand()
{
  Serial.println("This command is unknown!");
  ShowCommands();
}

void PrintSuccess(long val, String param)
{
  Serial.print(param);
  Serial.print(": ");
  Serial.println(val);
}

void OnGetVersionNumber()
{
  Serial.print(" Version: ");
  Serial.println(CURRENT_VERSION);
}

void OnSetMaxVel()
{
  // todo: what happens if there is no arg?
  long val = cmdMessenger.readInt32Arg();
  if (CheckBoundsInclusive(val, 10, 48000)) {
    settings.SetMaxVelocity(val);
    PrintSuccess(val, "MaxVel");
  }  
}

void OnGetMaxVel()
{
  Serial.print(" Max Vel: ");
  Serial.println(settings.GetMaxVelocity());
}

void OnSetAccel()
{
  // todo: what happens if there is no arg?
  long val = cmdMessenger.readInt32Arg();
  if (CheckBoundsInclusive(val, 1, 32000)) {
    settings.SetAcceleration(val);
    PrintSuccess(val, "Accel");
  }  
}

void OnGetAccel()
{
  Serial.print(" Accel: ");
  Serial.println(settings.GetAcceleration());
}

void OnSetAntenna()
{
  // todo: what happens if there is no arg?
  int val = cmdMessenger.readInt16Arg();
  if (CheckBoundsInclusive(val, 0, 1)) {
    settings.SetAntenna(val);
    PrintSuccess(val, "Antenna");
  }  
}

void OnGetAntenna()
{
  Serial.print(" Antenna: ");
  Serial.println(settings.GetAntenna());
}

void OnSetChannel()
{
  // todo: what happens if there is no arg?
  int val = cmdMessenger.readInt16Arg();
  if (CheckBoundsInclusive(val, 1, 82)) {
    settings.SetChannel(val);
    PrintSuccess(val, "Channel");
  }  
}

void OnGetChannel()
{
  Serial.print(" Channel: ");
  Serial.println(settings.GetChannel());
}

void OnSetPALevel()
{
  // todo: what happens if there is no arg?
  int val = cmdMessenger.readInt16Arg();
  if (CheckBoundsInclusive(val, 0, 3)) {
    settings.SetPALevel(val);
    PrintSuccess(val, "PA Level");
  }  
}

void OnGetPALevel()
{
  Serial.print(" PA Level: ");
  Serial.println(settings.GetPALevel());
}

void OnSetZModeMaxVel()
{
  // todo: what happens if there is no arg?
  long val = cmdMessenger.readInt32Arg();
  if (CheckBoundsInclusive(val, 10, 48000)) {
    settings.SetZModeMaxVelocity(val);
    PrintSuccess(val, "ZMode MaxVel");
  }  
}

void OnGetZModeMaxVel()
{
  Serial.print(" ZMode Max Vel: ");
  Serial.println(settings.GetZModeMaxVelocity());
}

void OnSetZModeAccel()
{
  // todo: what happens if there is no arg?
  long val = cmdMessenger.readInt32Arg();
  if (CheckBoundsInclusive(val, 1, 32000)) {
    settings.SetZModeAcceleration(val);
    PrintSuccess(val, "ZMode Accel");
  }  
}

void OnGetZModeAccel()
{
  Serial.print(" ZMode Accel: ");
  Serial.println(settings.GetZModeAcceleration());
}

// Callback function that shows a list of commands
void OnCommandList()
{
  ShowCommands();
}

void OnGetAllValues()
{
  OnGetVersionNumber();
  OnGetMaxVel();
  OnGetAccel();
  OnGetAntenna();
  OnGetChannel();
  OnGetPALevel();
  OnGetZModeMaxVel();
  OnGetZModeAccel();
}

int CheckBoundsInclusive(long val, long min, long max)
{
  if (val < min || val > max) {
    Serial.println(ERROR);
    return false;
  }
  return true;
}

// Callbacks define on which received commands we take action
void Console::AttachCommandCallbacks()
{
  // Attach callback methods
  cmdMessenger.attach(OnUnknownCommand);
  cmdMessenger.attach(COMMAND_LIST, OnCommandList);
  cmdMessenger.attach(SET_MAX_VEL, OnSetMaxVel);
  cmdMessenger.attach(SET_ACCEL, OnSetAccel);
  cmdMessenger.attach(SET_ANTENNA, OnSetAntenna);
  cmdMessenger.attach(SET_CHANNEL, OnSetChannel);
  cmdMessenger.attach(SET_PALEVEL, OnSetPALevel);
  cmdMessenger.attach(SET_ZMODE_MAX_VEL, OnSetZModeMaxVel);
  cmdMessenger.attach(SET_ZMODE_ACCEL, OnSetZModeAccel);
}
