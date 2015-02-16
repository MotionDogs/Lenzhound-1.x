#include "RollingAverager.h"

namespace rollingaveragernamespace {
  
void RollingAverager::Reset(long start_value) {
  buffer_index_ = 0;
  sum = 0;
  for (int i=0; i<kBufferSize; i++) {
    buffer_[i]=start_value; 
    sum += start_value;
  }
}

long RollingAverager::Roll(long next_input) {
  if(!started) {
    Reset(next_input);
    started = true;
  }
  ++buffer_index_;
  buffer_index_ &= (kBufferSize - 1);
  sum += next_input;
  sum -= buffer_[buffer_index_];
  buffer_[buffer_index_] = next_input;
  return sum / kBufferSize;
}

}
