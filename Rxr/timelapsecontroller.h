#ifndef lenzhound_timelapse_controller_h
#define lenzhound_timelapse_controller_h

#include "typedefs.h"
#include "motorcontroller.h"

namespace lh {

struct CubicSpline {
  f32* as;
  f32* bs;
  f32* cs;
  f32* ds;
  f32* xs;
  i16 n;
};

struct TimelapseConfiguration {
  CubicSpline spline;
  i32 interval_in_millis, delay_in_millis;
};

struct TimelapseState {
  MotorController* motor_controller;
  TimelapseConfiguration config;
  i32 delay_remaining, position, cur_cycles, next_target;
};

i32 eval_spline(f32 x, CubicSpline* spline);

class TimelapseController {
public:
  void run(TimelapseState* state);
  void init_state(TimelapseState* state);
private:
};

}
#endif //lenzhound_timelapse_controller_h

