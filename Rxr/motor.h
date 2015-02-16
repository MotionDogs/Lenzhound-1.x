#ifndef lenzhound_motor_h
#define lenzhound_motor_h

namespace lh {

class Motor {
public:
  virtual ~Motor() {}
  virtual void Pulse() = 0;
  virtual void SetDirForward() = 0;
  virtual void SetDirBackward() = 0;
  virtual void Sleep() = 0;
  virtual void WakeUp() = 0;
};

}

#endif //lenzhound_motor_h
