
#ifndef _ENCVELMANAGER_h
#define _ENCVELMANAGER_h


class EncVelManager
{
protected:
  long mPrevEncPos;
  int mVelocityPercent;
  int mPrevVelocityPercent;

public:
  void Init(int startPercentage);
  int GetVelocityPercent();
  void SetLEDs(char force);
  void SetAllLEDsOff();
};


#endif

