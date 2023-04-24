#pragma once

#include "../math/vector.h"
#include "../render_object.h"

typedef struct quad_t {
  render_object_t object;

  vec2_t position;
  vec2_t size;
  vec4_t color;
} quad_t;

void quad_load(quad_t *quad);
