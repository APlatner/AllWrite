#pragma once

#include <stdint.h>

typedef struct buffer_element_t {
  int type;
  int count;
} buffer_element_t;

/*
[position, color, texture]
*/

typedef struct buffer_layout_t {
  buffer_element_t elements[16];
  int num_elements;
  int stride;
} buffer_layout_t;

typedef struct render_object_t {
  uint32_t vbo;
  uint32_t vao;
  uint32_t ebo;
  uint32_t shader_id;
  uint32_t texture_id;
} render_object_t;

void render_object_create_vao(render_object_t *object, buffer_layout_t layout);