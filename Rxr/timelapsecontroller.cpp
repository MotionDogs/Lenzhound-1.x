#include "timelapsecontroller.h"
#include "stdio.h"
#include "alloca.h"
#include "util.h"

namespace lh {

i32 eval_spline(f32 x, CubicSpline* spline) {
  i16 n = spline->n;
  f32* xs = spline->xs;
  i16 j = 0;
  while (j < n && xs[j+1] < x) {
    j++;
  }

  f32 a = spline->as[j];
  f32 b = spline->bs[j];
  f32 c = spline->cs[j];
  f32 d = spline->ds[j];

  f32 dx = x - xs[j];
  f32 y = a + b * dx + c * dx * dx + d * dx * dx * dx;

  return (i32)y;
}

void TimelapseController::run(lh::TimelapseState* state) {
  i32 t_millis = (state->cur_cycles) / 6;
  f32 max_t = state->config.spline.xs[state->config.spline.n];
  if (t_millis > max_t) { return; }
  if (state->delay_remaining) {
    --state->delay_remaining;
    if (!state->delay_remaining) {
      state->motor_controller->set_motor_position(0);
    }
    return;
  }

  if (t_millis == state->next_target) {
    state->next_target += state->config.interval_in_millis;

    state->position = eval_spline((f32)state->next_target,&state->config.spline);

    // set observed position in fixed-point form, and assume the motor is using
    // 8th steps (we only want to allow full-steps for timelapses)
    state->motor_controller->set_observed_position(util::MakeFixed(state->position) << 3);
  }

  ++state->cur_cycles;

  state->motor_controller->run();
}

void TimelapseController::init_state(TimelapseState* state) {
  state->position = 0;
  state->cur_cycles = 0;
  state->next_target = 0;
  state->delay_remaining = state->config.delay_in_millis;
}

}