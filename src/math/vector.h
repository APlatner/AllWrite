#pragma once

#include <stdint.h>

typedef union vec2_t {
  float data[2];
  struct {
    float x;
    float y;
  };
} vec2_t;

typedef union vec3_t {
  float data[3];
  struct {
    float x;
    float y;
    float z;
  };
	struct {
    float r;
    float g;
    float b;
  };
} vec3_t;

typedef union vec4_t {
  float data[4];
  struct {
    float x;
    float y;
    float z;
    float w;
  };
	struct {
    float r;
    float g;
    float b;
		float a;
  };
} vec4_t;

vec2_t vec2_scale(vec2_t vec2, float scaler);
vec2_t vec2_normalize(vec2_t vec2);
