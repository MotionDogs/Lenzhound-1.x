#ifndef rxr_receiver_h
#define rxr_receiver_h

struct Packet {
  Packet() 
  : position(0), velocity(0), acceleration(0), mode(0) {}
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
