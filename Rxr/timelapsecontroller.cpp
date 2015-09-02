#include "timelapsecontroller.h"
#include "stdio.h"
#include "alloca.h"
#include "util.h"

namespace lh {

void solve_tridiagonal(f32* ma, f32* mb, f32* mc, f32* r, i32 count) {
  // for (int i = 0; i < count; ++i) {
  //   for (int j = 0; j < count; ++j) {
  //     if (i == j+1) {
  //       printf("%4.2f\t", ma[i]);
  //     } else if (i == j) {
  //       printf("%4.2f\t", mb[i]);
  //     } else if (i == j-1) {
  //       printf("%4.2f\t", mc[i]);
  //     } else {
  //       printf("0.00\t");
  //     }
  //   }

  //   printf("= %4.2f\n", r[i]);
  //   printf("\n");
  // }
  // printf("\n");

  mc[0] = mc[0] / mb[0];
  r[0] = r[0] / mb[0];

  for (i16 i = 1; i < count; ++i) {
    f32 m = 1.0f / (mb[i] - mc[i-1] * ma[i]);
    mc[i] = mc[i] * m;
    r[i] = (r[i] - r[i - 1] * ma[i]) * m;
  }

  for (i16 i = count-2; i >= 0; --i) {
    r[i] = r[i] - mc[i] * r[i+1];
  }
}

void interpolate_cubic_spline(CubicSpline* spline, void* free_space) {
  // below is an implementation of the algorithm for spline interpolation
  // found on wikipedia, with a tridiagonal matrix solver built in to help with
  // space efficiency. anyone who wants to should feel free to come up with a
  // more space-efficient version - i'm sure i've overlooked a lot.

  f32* xs = spline->xs;
  f32* ys = spline->ys;
  f32* as = spline->as;
  f32* bs = spline->bs;
  i16 count = spline->n;
  f32 dx;

  // this is our tri-diagonal matrix. mb is the main diagonal, ma sub, mc super
  f32* ma = as - 1;
  f32* mc = bs;
  f32* mb = ((f32*)free_space);
  f32* r = mb + count;

  // first row
  dx = xs[1] - xs[0];
  mc[0] = 1.0f / dx;
  mb[0] = 2.0f * mc[0];
  r[0] = 3 * (ys[1] - ys[0]) / (dx * dx);


  for (i16 i = 1; i < count - 1; ++i) {
    f32 dx1 = xs[i] - xs[i-1];
    f32 dx2 = xs[i+1] - xs[i];

    ma[i] = 1.0f / dx1;
    mc[i] = 1.0f / dx2;
    mb[i] = 2.0f * (ma[i] + mc[i]);

    f32 dy1 = ys[i] - ys[i-1];
    f32 dy2 = ys[i+1] - ys[i];
    r[i] = 3.0f * (dy1 / (dx1 * dx1) + dy2 / (dx2 * dx2));
  }

  // last row
  dx = xs[count-1] - xs[count-2];
  f32 dy = ys[count-1] - ys[count-2];
  ma[count-1] = 1.0f / dx;
  mb[count-1] = 2.0f * ma[count-2];
  r[count-1] = 3 * (dy / (dx * dx));

  solve_tridiagonal(ma, mb, mc, r, count);

  for (int i = 1; i < count; i++) {
    f32 dx = xs[i] - xs[i - 1];
    f32 dy = ys[i] - ys[i - 1];
    as[i-1] = r[i-1] * dx - dy; // equation 10 from the article
    bs[i-1] = -r[i] * dx + dy; // equation 11 from the article
  }
}

i32 eval_spline(f32 x, CubicSpline2* spline) {
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

i32 eval_spline(f32 x, CubicSpline* spline) {
  i16 n = spline->n;
  f32* xs = spline->xs;
  i16 j = 0;
  while (j < (n-1) && xs[j+1] < x) {
    j++;
  }

  f32 a = spline->as[j];
  f32 b = spline->bs[j];
  f32* ys = spline->ys;

  f32 dx = xs[j+1] - xs[j];
  f32 t = (x - xs[j]) / dx;
  f32 y = (1-t) * ys[j] + t * ys[j+1] + t * (1-t) * (a * (1-t) + b * t);

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