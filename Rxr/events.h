#ifndef rxr_events_h
#define rxr_events_h
class events {
public:
  static bool dirty();
  static void set_dirty(bool val);
private:
  static bool dirty_;
};
#endif //rxr_events_h