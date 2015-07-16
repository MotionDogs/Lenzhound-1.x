#ifndef rxr_receiver_h
#define rxr_receiver_h


static const int SENTINEL_VALUE = -2147483647;

struct Packet {
  Packet() 
  : position(SENTINEL_VALUE), velocity(0), acceleration(0), mode(0) {}
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
