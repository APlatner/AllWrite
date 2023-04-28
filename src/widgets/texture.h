#pragma once

#include <math/vector.h>
#include <render_object.h>

typedef struct texture_t {
  render_object_t object;

  vec2_t position;
  vec2_t size;
  vec4_t color;
} texture_t;

void texture_load(texture_t *texture);
void texture_update(texture_t *texture);
