#ifndef PROJECT_PATH_ROLLING_AVERAGER_H_
#define PROJECT_PATH_ROLLING_AVERAGER_H_

namespace rollingaveragernamespace {
  
class RollingAverager {
private:
  static const unsigned char kBufferSize = 4;
  char buffer_index_;
  long buffer_[kBufferSize];
  long sum;
  bool started;
public:
  void Reset(long start_value);
  long Roll(long next_input);
};

} // namespace rollingaveragernamespace
#endif // PROJECT_PATH_ROLLING_AVERAGER_H_
