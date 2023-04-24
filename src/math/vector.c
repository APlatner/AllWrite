#include "vector.h"

vec2_t vec2_scale(vec2_t vec2, float scaler) {
  vec2_t scaled_vec2 = {vec2.x * scaler, vec2.y * scaler};
  return scaled_vec2;
}

vec2_t vec2_normalize(vec2_t vec2) {
  // vec2_t normal_vec2 = vec2.x * vec2.x + vec2.y * vec2.y;
  return vec2;
}
