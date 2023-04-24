#include "matrix.h"

#include <stdio.h>

mat4 mat4_ortho(float left, float right, float bottom, float top, float near,
                float far) {

  mat4 matrix = {2 / (right - left),
                 0,
                 0,
                 -(right + left) / (right - left),
                 0,
                 2 / (top - bottom),
                 0,
                 -(top + bottom) / (top - bottom),
                 0,
                 0,
                 -2 / (far - near),
                 -(far + near) / (far - near),
                 0,
                 0,
                 0,
                 1};

  return matrix;
}

void mat4_print(mat4 matrix) {
  printf("%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n",
         matrix.array[0], matrix.array[1], matrix.array[2], matrix.array[3],
         matrix.array[4], matrix.array[5], matrix.array[6], matrix.array[7],
         matrix.array[8], matrix.array[9], matrix.array[10], matrix.array[11],
         matrix.array[12], matrix.array[13], matrix.array[14],
         matrix.array[15]);
}
