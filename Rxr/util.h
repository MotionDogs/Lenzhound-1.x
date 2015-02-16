#ifndef lenzhound_util_h
#define lenzhound_util_h
#include "constants.h"

namespace util {
inline long Abs(long a) {
  return (a < 0) ? -a : a;
}

inline long Max(long a, long b) {
  return (a > b) ? a : b;
}

inline long Min(long a, long b) {
  return (a < b) ? a : b;
}

inline long MakeFixed(long a) {
  return a << kBitShift;
}

inline long FixedMultiply(long a, long b) {
  return (a * b) >> kBitShift;
}

inline long FixedDivide(long a, long b) {
  return (a << kBitShift) / b;  
}

const long kFixedOne = MakeFixed(1L);
}

#endif // lenzhound_util_h
