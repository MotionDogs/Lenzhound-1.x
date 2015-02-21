#ifndef lenzhound_motor_controller_h
#define lenzhound_motor_controller_h
#include "motor.h"
#include "constants.h"

namespace lh {

class MotorController {
public:
  MotorController();
  void Configure(long accel, long max_velocity, long z_accel, long z_velocity);
  void Run();
  void set_observed_position(long position);
  void set_max_velocity(int velocity, int mode);
  void set_accel(int accel, int mode);
private:
  bool direction_;
  long max_velocity_;
  long current_velocity_cap_;
  long accel_;
  long max_accel_;
  long z_max_accel_;
  long decel_denominator_;
  long max_decel_denominator_;
  long velocity_;
  long z_max_velocity_;
  long calculated_position_;
  long motor_position_;
  long observed_position_;
  long run_count_;
  long sleeping_;

  long GetDecelerationThreshold();
  bool TrySleep();
  void Sleep();
  void WakeUp();
};

}
#endif //lenzhound_motor_controller_h

