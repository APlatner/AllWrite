#include "vector.h"

#include <stdio.h>

vec2_t vec2_scale(vec2_t vec2, float scaler) {
  vec2_t scaled_vec2 = {{vec2.x * scaler, vec2.y * scaler}};
  return scaled_vec2;
}

vec2_t vec2_normalize(vec2_t vec2) {
  // vec2_t normal_vec2 = vec2.x * vec2.x + vec2.y * vec2.y;
  return vec2;
}

void vec4_print(vec4_t vec4) {
  printf("%f, %f, %f, %f\n", vec4.data[0], vec4.data[1], vec4.data[2],
         vec4.data[3]);
}
