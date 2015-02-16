#ifndef rxr_macros_h
#define rxr_macros_h

#include "Arduino.h"
#include "constants.h"

// pin macros
#define CLR(x,y)            ( PORT ## x&=(~(1<<y)) )
#define SET(x,y)            ( PORT ## x|=(1<<y) )
#define IN(x,y)             ( DDR ## x&=(~(1<<y)) )
#define OUT(x,y)            ( DDR ## x|=(1<<y) )
#define SET_MODE(pin,mode)  ( pin(mode) )

// antena pins
#define ANT_CTRL1(verb)     ( verb(F,0) )
#define ANT_CTRL2(verb)     ( verb(F,1) )
 
// easydriver pin macros
#define MS1_PIN(verb)       ( verb(D,2) )
#define MS2_PIN(verb)       ( verb(B,5) )
#define STEP_PIN(verb)      ( verb(D,6) )
#define DIR_PIN(verb)       ( verb(C,7) )
#define SLEEP_PIN(verb)     ( verb(D,3) )
#define ENABLE_PIN(verb)    ( verb(D,7) )
#endif // rxr_macros_h
