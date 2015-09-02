#include <vector>
#include "gtest/gtest.h"
#include "motorcontroller.h"
#include "util.h"
#include "constants.h"
#include "testsetup.h"

namespace {

struct MotorPulse { long run_count, direction; };
struct MotorContext {
  long boundary_, position_, direction_, run_count_;
  std::vector<MotorPulse> pulses_;
};
static MotorContext context = {};

void reset_context() {
  context.boundary_ = 0;
  context.position_ = 0;
  context.run_count_ = 0;
  context.direction_ = 1;
  context.pulses_.clear();
}

void test_pulse_motor() {
  context.position_ += context.direction_;
  ASSERT_LE(context.position_, context.boundary_);
  MotorPulse pulse = { context.run_count_, context.direction_ };
  context.pulses_.push_back(pulse);
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

TEST(MotorController, HitsItsTarget) {
  reset_context();
  setup_motor_funcs();
  lh::MotorController controller = lh::MotorController();
  long target = 5000;

  controller.configure(50, 6000, 50, 6000);
  controller.set_max_velocity(100, 0);
  controller.set_observed_position(util::MakeFixed(target));

  // make sure we don't go over target
  context.boundary_ = target;
  for (int i = 0; i < 35000; ++i) {
    controller.run();
  }
  EXPECT_EQ(context.position_, target);
}

TEST(MotorController, HandlesSlowSpeeds) {
  setup_motor_funcs();
  reset_context();
  lh::MotorController controller = lh::MotorController();
  long target = 300;

  controller.configure(50, 1, 50, 1);
  controller.set_max_velocity(100, 0);
  controller.set_observed_position(util::MakeFixed(target));

  // make sure we don't go over target
  context.boundary_ = target;
  for (int i = 0; i < 10000000; ++i) {
    controller.run();
  }
  EXPECT_EQ(context.position_, target);
}

TEST(MotorController, DoesNotChangeConcavity) {
  setup_motor_funcs();
  reset_context();
  lh::MotorController controller = lh::MotorController();
  long target = 6000;

  controller.configure(32, 48000, 32, 48000);
  controller.set_max_velocity(100, FREE_MODE);
  controller.set_accel(100, FREE_MODE);
  controller.set_observed_position(util::MakeFixed(target));

  context.boundary_ = target;

  for (int i = 0; i < 60000; ++i) {
    controller.run();
    ++context.run_count_;
  }

  EXPECT_EQ(context.position_, target);

  std::vector<MotorPulse> pulses = context.pulses_;
  long prev_delta = pulses[1].run_count - pulses[0].run_count;
  bool decel_begun = false;
  for (int i = 2; i < pulses.size(); ++i) {
    long next_delta = pulses[i].run_count - pulses[i - 1].run_count;
    // std::cout << next_delta << "\n"; // can chart using vanilla csv graphing tools

    if (!decel_begun) {
      // allow for the previous delta to be a little bit bigger because
      // of the digital/stepwise nature of the algorithm
      decel_begun = next_delta > prev_delta + 1;
    } else {
      // we only allow for one change of concavity - i.e. once we start
      // decelerating, we shouldn't start accelerating again
      ASSERT_GE(next_delta, prev_delta - 1);
    }

    prev_delta = next_delta;
  }
}

}
