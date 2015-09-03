//****************************************************************************
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
// for more details.
//****************************************************************************

#define DUINO 1

#include <SPI.h>
#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>
#include <MirfSpiDriver.h>
#include <nRF24L01.h>
#include <EEPROM.h>
#include <CmdMessenger.h>
#include "NewTimerOne.h"
#include "Settings.h"
#include "Console.h"
#include "util.h"
#include "constants.h"
#include "macros.h"
#include "motor.h"
#include "motorcontroller.h"
#include "timelapsecontroller.h"
#include "timelapseconfigbuilder.h"
#include "receiver.h"
#include "events.h"


Console console;
Settings settings;
Receiver receiver;

lh::MotorController motor_controller = lh::MotorController();
lh::TimelapseController timelapse_controller;
lh::TimelapseConfigBuilder timelapse_builder;
lh::TimelapseState timelapse = {0};
const i32 free_space_size = 512;
char free_space[free_space_size] = {0};
char input[80] = {0};
i32 input_index = 0;
bool timelapse_started = 0;
bool capturing_config = 0;

void TimerISR() {
  if (timelapse_started) {
    timelapse_controller.run(&timelapse);
  }
}

void DirtyCheckSettings() {
  long accel = settings.GetAcceleration();
  long max_velocity = settings.GetMaxVelocity();
  long z_max_velocity = settings.GetZModeMaxVelocity();
  long z_max_accel = settings.GetZModeAcceleration();
  motor_controller.configure(accel, max_velocity, z_max_accel, z_max_velocity);
  receiver.ReloadSettings();
}
 
void setup() {
  Serial.begin(kSerialBaud);

  SET_MODE(SLEEP_PIN, OUT);
  SET_MODE(ENABLE_PIN, OUT);
  SET_MODE(MS1_PIN, OUT);
  SET_MODE(MS2_PIN, OUT);
  SET_MODE(STEP_PIN, OUT);
  SET_MODE(DIR_PIN, OUT);
  SET_MODE(ANT_CTRL1, OUT);
  SET_MODE(ANT_CTRL2, OUT);

  SLEEP_PIN(SET);
  ENABLE_PIN(CLR);
  ANT_CTRL1(SET);
  ANT_CTRL1(CLR);
  MS1_PIN(SET);
  MS2_PIN(SET);

  // console.Init();
  DirtyCheckSettings();

  timelapse.motor_controller = &motor_controller;

  // delay(250);
  
  // while(receiver.Position()==SENTINEL_VALUE){ //Wait until the motor gets a signal before setting starting position
  //   receiver.GetData();
  //   console.Run();
  // }
  // motor_controller.set_motor_position(receiver.Position());
  
  Timer1.initialize();
  Timer1.attachInterrupt(TimerISR, kPeriod);
}
 
void loop() {
  i32 bytes_available = Serial.available();
  for (int i = 0; i < bytes_available; ++i) {
    char read = Serial.read();
    if (read == '?') {
      Serial.println(timelapse.position);
    }

    if (read == '^') {
      timelapse_started = 0;
      capturing_config = 1;
      input_index = 0;
    }
    input[input_index++] = read;
    if (read == '$') {
      i32 err = timelapse_builder.build_configuration(
        &timelapse.config, free_space, free_space_size, input);
      motor_controller.set_motor_position(0);
      timelapse_controller.init_state(&timelapse);
      if (!err) {
        timelapse_started = 1;
        Serial.println("OK");
        char buffer[100];
        Serial.print(timelapse.config.spline.xs[0]);
        Serial.print(" ");
        Serial.print(timelapse.config.spline.xs[1]);
        Serial.print(" ");
        Serial.print(timelapse.config.spline.as[0]);
        Serial.print(" ");
        Serial.print(timelapse.config.spline.as[1]);
        Serial.println("");
      } else {
        Serial.println("ERR");
      }
      capturing_config = 0;
    }
  } 
}
