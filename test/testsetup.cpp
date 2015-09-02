#include "stdio.h"
#include "testsetup.h"

static void (*pulse_motor_func)();
static void (*set_motor_dir_forward_func)();
static void (*set_motor_dir_backward_func)();
static void (*sleep_motor_func)();
static void (*wake_motor_func)();

void lh_test::set_pulse_motor_func(callback func) {
  pulse_motor_func = func;
}

void lh_test::set_set_motor_dir_forward_func(callback func) {
  set_motor_dir_forward_func = func;
}

void lh_test::set_set_motor_dir_backward_func(callback func) {
  set_motor_dir_backward_func = func;
}

void lh_test::set_sleep_motor_func(callback func) {
  sleep_motor_func = func;
}

void lh_test::set_wake_motor_func(callback func) {
  wake_motor_func = func;
}

void lh_test::pulse_motor_default() {}
void lh_test::set_motor_dir_forward_default() {}
void lh_test::set_motor_dir_backward_default() {}
void lh_test::sleep_motor_default() {}
void lh_test::wake_motor_default() {}

void lh_test::reset_motor_funcs() {
  pulse_motor_func = &pulse_motor_default;
  set_motor_dir_forward_func = &set_motor_dir_forward_default;
  set_motor_dir_backward_func = &set_motor_dir_backward_default;
  sleep_motor_func = &sleep_motor_default;
  wake_motor_func = &wake_motor_default;
}

void lh::pulse_motor() {pulse_motor_func();}
void lh::set_motor_dir_forward() {set_motor_dir_forward_func();}
void lh::set_motor_dir_backward() {set_motor_dir_backward_func();}
void lh::sleep_motor() {sleep_motor_func();}
void lh::wake_motor() {wake_motor_func();}

