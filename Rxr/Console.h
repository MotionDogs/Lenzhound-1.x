#ifndef Console_h
#define Console_h

#include "Settings.h"

class String;

class Console
{
public:
  Console();
  void Init();
  void Run();
  int GetConsoleObsPos();
private:
  void AttachCommandCallbacks();
};

// have to make these external to class because can't attach member functions
// to handlers
// todo: look into a better way
void OnSetMaxVel();
void OnGetMaxVel();
void OnUnknownCommand();
void OnCommandList();
void ShowCommands();
void OnSetAccel();
void OnGetAccel();
void OnSetAntenna();
void OnGetAntenna();
void OnSetChannel();
void OnGetChannel();
void OnSetPALevel();
void OnGetPALevel();
void OnSetZModeMaxVel();
void OnGetZModeMaxVel();
void OnSetZModeAccel();
void OnGetZModeAccel();
void OnGetAllValues();
void OnSetObsPos();
int CheckBoundsInclusive(long val, long min, long max);
void PrintSuccess(long val, String param);

#endif
