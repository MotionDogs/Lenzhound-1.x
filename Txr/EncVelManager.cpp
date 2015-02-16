// 
// Class used to track encoder position as it relates to setting
// velocity in PlayBack mode
// 

#include "EncVelManager.h"
#include "Arduino.h"
#include "bsp.h"

// These are percentages
#define MAX_VELOCITY  100
#define MID_VELOCITY  50
#define MIN_VELOCITY  1

#define ENC_CNTS_TO_VEL_PERCENT  2


void EncVelManager::Init(int startPercentage)
{
  // start at mid-speed
  mVelocityPercent = startPercentage;
  mPrevVelocityPercent = startPercentage;
  mPrevEncPos = BSP_GetEncoder();
  SetLEDs(true);
}

int EncVelManager::GetVelocityPercent()
{
  long curEncPos = BSP_GetEncoder();
  mPrevVelocityPercent = mVelocityPercent;
  mVelocityPercent += (curEncPos - mPrevEncPos) / ENC_CNTS_TO_VEL_PERCENT;
  mPrevEncPos = curEncPos;
  mVelocityPercent = min(mVelocityPercent, MAX_VELOCITY);
  mVelocityPercent = max(mVelocityPercent, MIN_VELOCITY);
  return mVelocityPercent;
}

void EncVelManager::SetLEDs(char force)
{
  // if it hasn't changed exit immediately
  if (mVelocityPercent == mPrevVelocityPercent && !force) {
    return;
  }
  // if we've left one of the boundaries, turn off boundary lights
  else if (mPrevVelocityPercent == MIN_VELOCITY || 
           mPrevVelocityPercent == MAX_VELOCITY ||
           mPrevVelocityPercent == MID_VELOCITY ||
           force) {
    RED_LED_OFF();
    GREEN_LED_OFF();
    ENC_GREEN_LED_OFF();
    ENC_RED_LED_OFF();
  }
  // if we've crossed the midway point make sure lights are all the way off
  else if (mPrevVelocityPercent < MID_VELOCITY ^ mVelocityPercent < MID_VELOCITY) {
    SetAllLEDsOff();
  }
  
  if (mVelocityPercent == MID_VELOCITY) {
    ENC_GREEN_LED_ON();
    ENC_RED_LED_ON();
  }
  else if (mVelocityPercent > MID_VELOCITY) {
    int dim = map(mVelocityPercent, MID_VELOCITY + 1, MAX_VELOCITY, 1, 250);
    analogWrite(SPEED_LED3, dim);
    if (mVelocityPercent == MAX_VELOCITY) {
      GREEN_LED_ON();
    }
  }
  else if (mVelocityPercent < MID_VELOCITY) {
    int dim = map(mVelocityPercent, MIN_VELOCITY, MID_VELOCITY - 1, 250, 1);
    analogWrite(SPEED_LED2, dim);
    if (mVelocityPercent == MIN_VELOCITY) {
      RED_LED_ON();
    }
  }
  
}

void EncVelManager::SetAllLEDsOff()
{
  RED_LED_OFF();
  BSP_TurnOffSpeedLED(1);
  BSP_TurnOffSpeedLED(2);
  GREEN_LED_OFF();
}