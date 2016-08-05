#ifndef Radio_h
#define Radio_h

class Radio {
public:
  Radio(int packetSize);
  void ReloadSettings();
  void SendPacket(byte* message);
  bool IsAlive();  // confirms we're still communicating with Txcvr
  void PowerDown();
  void PowerUp();  
  bool IsPowerUp();
private:
  void LoadSettings();
  bool mIsPowerUp;
};

#endif //Radio_h
