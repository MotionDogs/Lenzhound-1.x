#include "motor.h"

#ifndef lenzhound_test_setup_h
#define lenzhound_test_setup_h

namespace lh_test {

void pulse_motor_default();
void set_motor_dir_forward_default();
void set_motor_dir_backward_default();
void sleep_motor_default();
void wake_motor_default();

typedef void (*callback)(void);

void set_pulse_motor_func(callback func);
void set_set_motor_dir_forward_func(callback func);
void set_set_motor_dir_backward_func(callback func);
void set_sleep_motor_func(callback func);
void set_wake_motor_func(callback func);

void reset_motor_funcs();

}

#endif