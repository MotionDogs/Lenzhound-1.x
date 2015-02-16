
#ifndef Settings_h
#define Settings_h

// EEPROM locations for parameters
#define CHANNEL_LOC       12 // int
#define PA_LEVEL_LOC      14 // int
#define DATA_RATE_LOC     16 // int


class Settings
{
public:
  Settings();
  void SetChannel(int val);
  int GetChannel();
  void SetPALevel(int val);
  int GetPALevel();
  int GetDataRate();

private:
  // any reason to save the state of the settings?
  // or is it a one time read on boot up?

};

#endif  // bsp_h
