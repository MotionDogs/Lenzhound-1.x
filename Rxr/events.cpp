#include "events.h"

bool events::dirty_ = false;

bool events::dirty() {
  return dirty_;
}

void events::set_dirty(bool val) {
  dirty_ = val;
}