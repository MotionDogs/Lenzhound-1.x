#ifndef rxr_motor_h
#define rxr_motor_h
#include "motor.h"

class MotorImpl: public lh::Motor {
public:
  void Pulse();
  void SetDirForward();
  void SetDirBackward();
  void Sleep();
  void WakeUp();
};
#endif //rxr_motor_h
