#include "motorcontroller.h"
#include "constants.h"
#include "util.h"

#define CONVERSION_FACTOR .01

namespace lh {

MotorController::MotorController(Motor * motor) :
direction_(1),
max_velocity_(0),
current_velocity_cap_(0),
accel_(0),
decel_denominator_(0),
velocity_(0),
calculated_position_(0),
motor_position_(0),
observed_position_(0),
run_count_(0),
sleeping_(false)
{
  motor_ = motor;
}

void MotorController::set_observed_position(long position) {
  if (position != observed_position_) {
    WakeUp();
    run_count_ = 0;
  }
  observed_position_ = position;
}

void MotorController::set_max_velocity(int velocity, int mode) {
  long max = (mode == Z_MODE) ? z_max_velocity_ : max_velocity_;
  current_velocity_cap_ = util::Min(
    util::Max(max * velocity * CONVERSION_FACTOR, 1L),
    util::kFixedOne);
}

void MotorController::set_accel(int accel, int mode){
  if(mode==Z_MODE){
    accel_ = util::Max(z_max_accel_ * accel * CONVERSION_FACTOR, 1L);
    decel_denominator_ = util::FixedMultiply(accel_, util::MakeFixed(2L));
  }
  else{
    accel_ = util::Max(max_accel_ * accel * CONVERSION_FACTOR, 1L);
    decel_denominator_ = util::FixedMultiply(accel_, util::MakeFixed(2L));
  }
}

void MotorController::Configure(
  long max_accel, long max_velocity, long z_accel, long z_velocity) {
  max_accel_ = max_accel;
  max_velocity_ = max_velocity;
  z_max_accel_ = z_accel;
  z_max_velocity_ = z_velocity;
  max_decel_denominator_ = util::FixedMultiply(max_accel_, util::MakeFixed(2L));
  
  set_accel(100, FREE_MODE);
  set_max_velocity(100, FREE_MODE);
}

long MotorController::GetDecelerationThreshold() {
  return util::FixedDivide(
    util::FixedMultiply(velocity_,velocity_),decel_denominator_);
}

void MotorController::Sleep() {
  sleeping_ = true;
  run_count_ = 0;
  motor_->Sleep();
}

void MotorController::WakeUp() {
  sleeping_ = false;
  motor_->WakeUp();
}

bool MotorController::TrySleep() {
  if (motor_position_ != observed_position_) {
    run_count_ = 0;
    return false;
  }
  if (sleeping_) {
    return true;
  }
  if (run_count_ > kSleepThreshold) {
    Sleep();
    return true;
  }
  run_count_++;
  return false;
}

void MotorController::Run() {
  if (TrySleep()) {
    return;
  }
  long steps_to_go = util::Abs(observed_position_ - calculated_position_);
  if(direction_) {
    if((calculated_position_ > observed_position_) ||
      (steps_to_go <= GetDecelerationThreshold())) {
      velocity_ -= accel_;
    } else if (calculated_position_ < observed_position_) {
      velocity_ = util::Min(velocity_+accel_, current_velocity_cap_);
    }
    calculated_position_ += velocity_;
    if((motor_position_ < calculated_position_) &&
      (motor_position_ != observed_position_)) {
      motor_position_ += util::kFixedOne;
      motor_->Pulse();
    }
    if(velocity_ < 0) {
      direction_ = 0;
      motor_->SetDirBackward();
    }
  } else {
    if((calculated_position_ < observed_position_) ||
      (steps_to_go <= GetDecelerationThreshold())){
      velocity_ += accel_;
    } else if (calculated_position_ > observed_position_) {
      velocity_ = util::Max(velocity_-accel_, -current_velocity_cap_);
    }
    calculated_position_ += velocity_;
    if(motor_position_ > calculated_position_ &&
      motor_position_ != observed_position_) {
      motor_position_ -= util::kFixedOne;
      motor_->Pulse();
    }
    if(velocity_ > 0) {
      direction_ = 1;
      motor_->SetDirForward();
    }
  }
}
}

