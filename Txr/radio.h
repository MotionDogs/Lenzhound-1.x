#ifndef Radio_h
#define Radio_h

class Radio {
public:
  Radio(int packetSize);
  void ReloadSettings();
  void SendPacket(byte* message);
  int IsAlive();  // confirms we're still communicating with Txcvr
private:
  void LoadSettings();
};

#endif //Radio_h