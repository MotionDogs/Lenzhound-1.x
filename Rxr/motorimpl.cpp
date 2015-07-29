#include "motorimpl.h"
#include "macros.h"

void MotorImpl::Pulse() {
  STEP_PIN(SET);
  STEP_PIN(CLR);
}

void MotorImpl::SetDirForward() {
  DIR_PIN(SET);
}

void MotorImpl::SetDirBackward() {
  DIR_PIN(CLR);
}

void MotorImpl::Sleep() {
  SLEEP_PIN(CLR);
}

void MotorImpl::WakeUp() {
  SLEEP_PIN(SET);
}
