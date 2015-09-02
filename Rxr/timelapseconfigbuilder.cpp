#include "timelapseconfigbuilder.h"
#include "string.h"
#include "stdio.h"

char* find_char(char* input, char c) {
  while (*input && (*input != c)) {
    input++;
  }

  return input;
}

// space requirements: count * 8 - 4
lh::CubicSpline2 lh::TimelapseConfigBuilder::interpolate_cubic_spline(
  f32* xs, f32* ys, i16 n, void* free_space) {

  f32* a = ys; // f32[n+1]
  f32* b = (f32*)free_space; // f32[n]
  f32* c = b + n;            // f32[n+1]
  f32* d = c + n + 1;        // f32[n]
  f32* h = d + n;            // f32[n]
  f32* m = h + n;            // f32[n]
  f32* l = m + n;            // f32[n+1]
  f32* o = l + n + 1;        // f32[n+1]
  f32* z = o + n + 1;        // f32[n+1]

  for (int i = 0; i < n; ++i) {
    h[i] = xs[i+1] - xs[i];
  }

  for (int i = 1; i < n; ++i) {
    m[i] = 3.0f / h[i] * (a[i+1] - a[i]) -  3.0f / h[i-1] * (a[i] - a[i-1]);
  }

  l[0] = 1;
  o[0] = z[0] = 0;

  for (int i = 1; i < n; ++i) {
    l[i] = 2.0f * (xs[i+1] - xs[i-1]) - h[i-1] * o[i-1];
    o[i] = h[i] / l[i];
    z[i] = (m[i] - h[i-1] * z[i-1]) / l[i];
  }

  l[n] = 1;
  z[n] = c[n] = 0;

  for (int i = n-1; i >= 0; --i) {
    c[i] = z[i] - o[i] * c[i+1];
    b[i] = (a[i+1] - a[i]) / h[i] - h[i] * (c[i+1] + 2.0f * c[i]) / 3.0f;
    d[i] =  (c[i+1] - c[i]) / (3.0f * h[i]);
  }

  CubicSpline2 result;
  result.as = a;
  result.bs = b;
  result.cs = c;
  result.ds = d;
  result.xs = xs;
  result.n = n;

  return result;
}

i32 lh::TimelapseConfigBuilder::build_configuration(
  TimelapseConfiguration* result,
  void* memory, i32 max_size,
  char* str_input) {

  i32 max_count = ((max_size + 4) / (sizeof(f32) * 10));

  int scan_result = sscanf(str_input, "^i:%d,d:%d,%*s",
    &result->interval_in_millis, &result->delay_in_millis);

  if (scan_result < 0) {
    return -1;
  }

  i32 count = 0;
  f32* xs = (f32*)memory;
  char* comma = find_char(find_char(str_input, ',') + 1, ','); // skip two commas
  while (comma && comma[1] == 'x') {
    i32 tmp = 0;
    scan_result = sscanf(comma + 1, "x:%d%*s", &tmp);
    xs[count] = (f32)tmp;
    if (scan_result < 0 || count > max_count) {
      return -1;
    }
    ++count;
    comma = find_char(comma + 1, ',');
  }
  f32* ys = xs + count;
  for (int i = 0; i < count; ++i) {
    if (!comma || comma[1] != 'y') {
      return -1;
    }
    i32 tmp = 0;
    scan_result = sscanf(comma + 1, "y:%d%*s", &tmp);
    ys[i] = (f32)tmp;
    if (scan_result < 0) {
      return -1;
    }
    comma = find_char(comma + 1, ',');
  }

  // f32* as = ys + count;
  // f32* bs = as + count - 1;
  // void* free_space = (void*)(bs + count - 1);
  // result->spline.xs = xs;
  // result->spline.ys = ys;
  // result->spline.as = as;
  // result->spline.bzs = bzs;

  // ::lh::interpolate_cubic_spline(&result->spline, free_space);
  
  void* free_space = (void*)(ys + count);

  result->spline = interpolate_cubic_spline(xs, ys, count - 1, free_space);

  return 0;
}