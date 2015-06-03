#include <vector>
#include "gtest/gtest.h"
#include "motorcontroller.h"
#include "util.h"
#include "constants.h"

struct MotorPulse { long run_count; long direction; };

class MotorMock: public lh::Motor {
public:
  MotorMock() : position_(0), direction_(1) {
  }
  void set_boundary(long value) {
    boundary_ = value;
  }
  long position() {
    return position_;
  }
  void increment_run_count() {
    run_count_++;
  }
  std::vector<MotorPulse> pulses() {
    return pulses_;
  }
  virtual void Pulse() {
    position_ += direction_;
    ASSERT_LE(position_, boundary_);
    MotorPulse pulse = { run_count_, direction_ };
    pulses_.push_back(pulse);
  }
  virtual void SetDirForward() {
    direction_ = 1;
  }
  virtual void SetDirBackward() {
    direction_ = -1;
  }
  virtual void Sleep() {
  }
  virtual void WakeUp() {
  }
private:
  long boundary_;
  long position_;
  long direction_;
  long run_count_;
  std::vector<MotorPulse> pulses_;
};

TEST(MotorController, HitsItsTarget) {
  MotorMock mock;
  lh::MotorController controller = lh::MotorController(&mock);
  long target = 5000;

  controller.Configure(50, 6000, 50, 6000);
  controller.set_max_velocity(100, 0);
  controller.set_observed_position(util::MakeFixed(target));

  // make sure we don't go over target
  mock.set_boundary(target);
  for (int i = 0; i < 35000; ++i) {
    controller.Run();
  }
  EXPECT_EQ(mock.position(),target);
}

TEST(MotorController, HandlesSlowSpeeds) {
  MotorMock mock;
  lh::MotorController controller = lh::MotorController(&mock);
  long target = 300;

  controller.Configure(50, 1, 50, 1);
  controller.set_max_velocity(100, 0);
  controller.set_observed_position(util::MakeFixed(target));

  // make sure we don't go over target
  mock.set_boundary(target);
  for (int i = 0; i < 10000000; ++i) {
    controller.Run();
  }
  EXPECT_EQ(target, mock.position());
}

TEST(MotorController, DoesNotChangeConcavity) {
  MotorMock mock;
  lh::MotorController controller = lh::MotorController(&mock);
  long target = 6000;

  controller.Configure(32, 48000, 32, 48000);
  controller.set_max_velocity(100, FREE_MODE);
  controller.set_accel(100, FREE_MODE);
  controller.set_observed_position(util::MakeFixed(target));

  mock.set_boundary(target);

  for (int i = 0; i < 60000; ++i) {
    controller.Run();
    mock.increment_run_count();
  }
  EXPECT_EQ(target, mock.position());

  std::vector<MotorPulse> pulses = mock.pulses();
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