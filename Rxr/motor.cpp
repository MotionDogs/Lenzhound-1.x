#include "motor.h"
#include "macros.h"

void lh::pulse_motor() {
  STEP_PIN(SET);
  STEP_PIN(CLR);
}

void lh::set_motor_dir_forward() {
  DIR_PIN(SET);
}

void lh::set_motor_dir_backward() {
  DIR_PIN(CLR);
}

void lh::sleep_motor() {
  SLEEP_PIN(CLR);
}

void lh::wake_motor() {
  SLEEP_PIN(SET);
}
