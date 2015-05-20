
#ifndef Settings_h
#define Settings_h

// EEPROM locations for parameters
#define CHANNEL_LOC       12 // int
#define PA_LEVEL_LOC      14 // int
#define DATA_RATE_LOC     16 // int
#define CAL_POS_1_LOC     18 // int
#define CAL_POS_2_LOC     20 // int
#define SAVED_POS_ARR_LOC 22 // int[4]
#define START_IN_CAL_LOC  30 // boolean

class Settings
{
public:
  Settings();
  void SetChannel(int val);
  int GetChannel();
  void SetPALevel(int val);
  int GetPALevel();
  void SetCalPos1(int val);
  int GetCalPos1();
  void SetCalPos2(int val);
  int GetCalPos2();
  void SetSavedPos(int val, int index);
  int GetSavedPos(int index);
  void SetStartInCal(int calSUp);
  int GetStartInCal();
  int GetDataRate();

private:
  // any reason to save the state of the settings?
  // or is it a one time read on boot up?

};

#endif  // bsp_h
