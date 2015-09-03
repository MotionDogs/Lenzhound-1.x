#include "gtest/gtest.h"
#include "timelapsecontroller.h"
#include "timelapseconfigbuilder.h"
#include "testsetup.h"
#include "util.h"

namespace {

struct MotorPulse { long run_count, direction; };
struct MotorContext {
  long boundary_, position_, direction_, run_count_;
};
static MotorContext context = {};

void reset_context() {
  context.boundary_ = 0;
  context.position_ = 0;
  context.run_count_ = 0;
  context.direction_ = 1;
}

void test_pulse_motor() {
  context.position_ += context.direction_;
}
void test_set_motor_dir_forward() {
  context.direction_ = 1;
}
void test_set_motor_dir_backward() {
  context.direction_ = -1;
}

void setup_motor_funcs() {
  lh_test::reset_motor_funcs();
  lh_test::set_pulse_motor_func(&test_pulse_motor);
  lh_test::set_set_motor_dir_forward_func(&test_set_motor_dir_forward);
  lh_test::set_set_motor_dir_backward_func(&test_set_motor_dir_backward);
}

TEST(TimelapseController, HappyPath) {
  reset_context();
  setup_motor_funcs();

  lh::MotorController motor = lh::MotorController();
  motor.configure(1000, 6000, 1000, 6000);
  motor.set_max_velocity(10000, 0);

  const i32 free_space_size = 512;
  char free_space[free_space_size] = {0};
  lh::TimelapseState state = {0};
  lh::TimelapseConfigBuilder timelapse_builder;
  i32 err = timelapse_builder.build_configuration(&state.config,
    free_space, free_space_size, (char*)
    "^i:1000,d:100,n:3,t:1000,s:50,t:10000,s:500,t:20000,s:1000$");
  state.motor_controller = &motor;
  state.delay_remaining = state.config.delay_in_millis;

  const i16 n = 4;
  f32 xs[n] = {0.0f, 1000.0f, 10000.0f, 20000.0f};
  f32 ys[n] = {0.0f, 50.0f, 500.0f, 1000.0f};

  lh::TimelapseController timelapse;

  ASSERT_EQ(err, 0);
  i32 j = 0;
  for (int i = 0; i < n; ++i) {
    for (; j <= ((i32)xs[i] * 6); ++j) {
      timelapse.run(&state);
    }
    EXPECT_TRUE(abs((i32)ys[i] - state.position) <= 1);
  }

  EXPECT_EQ(context.position_, (i32)ys[n-1] * 8);
}

TEST(TimelapseController, RunsTwoTimelapsesConsecutively) {
  reset_context();
  setup_motor_funcs();

  lh::MotorController motor = lh::MotorController();
  motor.configure(1000, 6000, 1000, 6000);
  motor.set_max_velocity(10000, 0);

  const i32 free_space_size = 512;
  char free_space[free_space_size] = {0};
  lh::TimelapseState state = {0};
  state.motor_controller = &motor;

  lh::TimelapseConfigBuilder timelapse_builder;
  lh::TimelapseController timelapse;

  motor.set_motor_position(0);
  i32 err = timelapse_builder.build_configuration(&state.config,
    free_space, free_space_size, (char*)
    "^i:1000,d:10,n:1,t:10000,s:200$");

  ASSERT_EQ(err, 0);

  timelapse.init_state(&state);

  for (int i = 0; i < 10000 * 6; ++i) {
    timelapse.run(&state);
  }

  EXPECT_EQ(state.position, 200); 
  EXPECT_EQ(context.position_, 200 * 8);

  motor.set_motor_position(0);
  timelapse_builder.build_configuration(&state.config,
    free_space, free_space_size, (char*)
    "^i:1000,d:10,n:1,t:5000,s:1000$");
  timelapse.init_state(&state);

  for (int i = 0; i < 5000 * 6; ++i) {
    timelapse.run(&state);
  }

  EXPECT_EQ(state.position, 1000); 
  EXPECT_EQ(context.position_, (200 + 1000) * 8);
}

TEST(TimelapseController, InterpolateHandlesWikipediaCase) {
  i16 n = 2;
  f32 xs[3] = {-100, 0, 300};
  f32 ys[3] = {50, 0, 300};
  i32 free[512] = {0};
  lh::TimelapseConfigBuilder builder;
  lh::CubicSpline spline = builder.interpolate_cubic_spline(xs, ys, n, free);

  EXPECT_EQ(50, eval_spline(-100.0f, &spline));
}

}