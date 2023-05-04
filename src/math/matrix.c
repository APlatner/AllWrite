#include "matrix.h"

#include <stdio.h>

mat4_t mat4_ortho(float left, float right, float bottom, float top, float near,
                  float far) {
  mat4_t matrix = {{2 / (right - left), 0, 0, -(right + left) / (right - left),
                    0, 2 / (top - bottom), 0, -(top + bottom) / (top - bottom),
                    0, 0, -2 / (far - near), -(far + near) / (far - near), 0, 0,
                    0, 1}};

  return matrix;
}

void mat4_print(mat4_t matrix) {
  printf("%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n",
         matrix.data[0], matrix.data[1], matrix.data[2], matrix.data[3],
         matrix.data[4], matrix.data[5], matrix.data[6], matrix.data[7],
         matrix.data[8], matrix.data[9], matrix.data[10], matrix.data[11],
         matrix.data[12], matrix.data[13], matrix.data[14], matrix.data[15]);
}

vec4_t mat4_vec4_multiply(mat4_t matrix, vec4_t vec4) {
  return (vec4_t){{matrix.x1 * vec4.x + matrix.x2 * vec4.x +
                       matrix.x3 * vec4.x + matrix.x4 * vec4.x,
                   matrix.y1 * vec4.y + matrix.y2 * vec4.y +
                       matrix.y3 * vec4.y + matrix.y4 * vec4.y,
                   matrix.z1 * vec4.z + matrix.z2 * vec4.z +
                       matrix.z3 * vec4.z + matrix.z4 * vec4.z,
                   matrix.w1 * vec4.w + matrix.w2 * vec4.w +
                       matrix.w3 * vec4.w + matrix.w4 * vec4.w}};
}
