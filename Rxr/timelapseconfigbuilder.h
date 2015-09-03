#ifndef lenzhound_timelapse_config_builder_h
#define lenzhound_timelapse_config_builder_h

#include "typedefs.h"
#include "timelapsecontroller.h"

namespace lh {

class TimelapseConfigBuilder {

public:
  i32 build_configuration(
    TimelapseConfiguration* config,
    void* memory, i32 max_size,
    char* str_input); // assume null-terminated string
  CubicSpline interpolate_cubic_spline(
    f32* xs, f32* ys, i16 n, void* free_space);
};

}
#endif //lenzhound_timelapse_config_builder_h

