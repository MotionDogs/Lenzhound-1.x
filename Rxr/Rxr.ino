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
#include "receiver.h"
#include "events.h"


Console console;
Settings settings;
Receiver receiver;

lh::MotorController motor_controller = lh::MotorController();

void TimerISR() {
  motor_controller.Run();
}

void DirtyCheckSettings() {
  long accel = settings.GetAcceleration();
  long max_velocity = settings.GetMaxVelocity();
  long z_max_velocity = settings.GetZModeMaxVelocity();
  long z_max_accel = settings.GetZModeAcceleration();
  motor_controller.Configure(accel, max_velocity, z_max_accel, z_max_velocity);
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

  // drv8880
  SET_MODE(SLEEP2_PIN, OUT);
  SET_MODE(STEP2_PIN, OUT);
  SET_MODE(DIR2_PIN, OUT);
  SET_MODE(TRQ1_PIN, OUT); 


  console.Init();
  DirtyCheckSettings();

  delay(250);
  
  while(receiver.Position()==SENTINEL_VALUE){ //Wait until the motor gets a signal before setting starting position
    receiver.GetData();
    console.Run();
  }
  motor_controller.set_motor_position(receiver.Position());
  
  Timer1.initialize();
  Timer1.attachInterrupt(TimerISR, kPeriod);
}
 
void loop() {
  receiver.GetData();
  motor_controller.set_observed_position(receiver.Position());
  motor_controller.set_max_velocity(receiver.Velocity(), receiver.Mode());
  motor_controller.set_accel(receiver.Acceleration(), receiver.Mode());
  console.Run();
  if (events::dirty()) {
    events::set_dirty(false);
    DirtyCheckSettings();
  }
}
