#include "motor.h"
#include "macros.h"

#define NOP __asm__ __volatile__ ("nop\n\t")

void lh::pulse_motor() {
  STEP_PIN(SET); // easydriver
  STEP_PIN(CLR);

  STEP2_PIN(SET); // drv8880
  NOP;NOP; // this seems to smooth out the steps
  STEP2_PIN(CLR);   
}

void lh::set_motor_dir_forward() {
  DIR_PIN(SET); // easydriver
  DIR2_PIN(SET); // drv8880
}

void lh::set_motor_dir_backward() {
  DIR_PIN(CLR); // easydriver
  DIR2_PIN(CLR); // drv8880
}

void lh::sleep_motor() {
  SLEEP_PIN(CLR); // easydriver
  SLEEP2_PIN(CLR); // drv8880
}

void lh::wake_motor() {
  SLEEP_PIN(SET); // easydriver
  SLEEP2_PIN(SET); // drv8880
}
