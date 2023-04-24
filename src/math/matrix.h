#pragma once

typedef struct mat4 {
  union {
    float array[16];
    struct {
      float x1, y1, z1, w1;
      float x2, y2, z2, w2;
      float x3, y3, z3, w3;
      float x4, y4, z4, w4;
    };
  };
} mat4;

mat4 mat4_ortho(float left, float right, float top, float bottom, float near,
                float far);
void mat4_print(mat4 matrix);
