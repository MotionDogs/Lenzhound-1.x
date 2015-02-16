#ifndef lenzhound_constants_h
#define lenzhound_constants_h

const char kBitShift = 15;
const int kSerialBaud = 9600;

// ISR constants
const long kSecondsInMicroseconds = 1000000L;
const long kIsrFrequency          = 6000L;
const long kPeriod                = kSecondsInMicroseconds/kIsrFrequency;

// Motor constants
const long kSleepThreshold        = kIsrFrequency * 5; // five seconds

enum {
  FREE_MODE,
  PLAYBACK_MODE,
  Z_MODE
};

#endif // lenzhound_constants_h
