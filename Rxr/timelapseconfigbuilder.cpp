#include "timelapseconfigbuilder.h"
#include "string.h"
#include "stdio.h"

char* find_char(char* input, char c) {
  while (*input && (*input != c)) {
    input++;
  }

  return input;
}

void parse_i32(char* input, i32* output) {
  bool negative = 0;
  if (*input == '-') {
    ++input;
    negative = 1;
  }

  *output = 0;
  i32 tmp = 0;
  for (int i = 0; i < 10; ++i) {
    tmp *= 10;
    switch(*input) {
      case '1': tmp+=1; break;
      case '2': tmp+=2; break;
      case '3': tmp+=3; break;
      case '4': tmp+=4; break;
      case '5': tmp+=5; break;
      case '6': tmp+=6; break;
      case '7': tmp+=7; break;
      case '8': tmp+=8; break;
      case '9': tmp+=9; break;
      case '0': break;
      default: return;
    }
    input++;
    *output = tmp;
  }
}

// space requirements: count * 8 - 4
lh::CubicSpline lh::TimelapseConfigBuilder::interpolate_cubic_spline(
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

  CubicSpline result;
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

  if (str_input[0] != '^' || str_input[1] != 'i') {
    return -1;
  }

  parse_i32(str_input + 3, &result->interval_in_millis);
  char* comma = find_char(str_input, ',');

  if (!comma || comma[1] != 'd') {
    return -2;
  }
  parse_i32(comma + 3, &result->delay_in_millis);

  i32 count = 0;
  comma = find_char(comma + 1, ',');
  if (!comma || comma[1] != 'n') {
    return -3;
  }
  parse_i32(comma + 3, &count);

  if (count > max_count) {
    return -4;
  }

  f32* xs = (f32*)memory;
  f32* ys = xs + count + 1;

  xs[0] = 0.0f;
  ys[0] = 0.0f;

  i32 tmp = 0;
  for (int i = 1; i <= count; ++i) {
    comma = find_char(comma + 1, ',');
    if (!comma || comma[1] != 't') {
      return -5;
    }
    parse_i32(comma + 3, &tmp);
    xs[i] = (f32)tmp;

    comma = find_char(comma + 1, ',');
    if (!comma || comma[1] != 's') {
      return -6;
    }
    parse_i32(comma + 3, &tmp);
    ys[i] = (f32)tmp;
  }

  if (!find_char(comma + 1, '$')) {
    return -7;
  }

  void* free_space = (void*)(ys + count + 1);

  result->spline = interpolate_cubic_spline(xs, ys, count, free_space);

  return 0;
  }