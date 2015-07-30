#ifndef rxr_receiver_h
#define rxr_receiver_h

#include "constants.h"

#define MAX_VELOCITY 100
#define MAX_ACCELERATION 100

static const int SENTINEL_VALUE = -2147483647;

struct Packet {
  Packet() 
  : position(SENTINEL_VALUE), velocity(MAX_VELOCITY), acceleration(MAX_ACCELERATION), mode(PLAYBACK_MODE) {}
  long position;
  char velocity;
  char acceleration;
  char mode;
};

class Receiver {
public:
  Receiver();
  void ReloadSettings();
  void GetData();
  long Position();
  int Velocity();
  int Acceleration();
  int Mode();
private:
  Packet packet_;
  void LoadSettings();
};

#endif //rxr_receiver_h
