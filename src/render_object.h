/*
	Copyright 2023 SentientCloud24

	 Licensed under the Apache License, Version 2.0 (the "License");
	 you may not use this file except in compliance with the License.
	 You may obtain a copy of the License at

			 http://www.apache.org/licenses/LICENSE-2.0

	 Unless required by applicable law or agreed to in writing, software
	 distributed under the License is distributed on an "AS IS" BASIS,
	 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	 See the License for the specific language governing permissions and
	 limitations under the License.
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct buffer_element_t {
  int type;
  int count;
	bool normalized;
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
	uint32_t vertices;
  uint32_t vbo;
  uint32_t vao;
  // uint32_t ebo;
  uint32_t shader_id;
  uint32_t texture_id;
} render_object_t;

void buffer_layout_create(buffer_layout_t *layout);
void buffer_layout_load(buffer_layout_t *layout, buffer_element_t element);

void render_object_create_vao(render_object_t *object, buffer_layout_t *layout);
void render_object_load_data(render_object_t *object, long size,  const void *data);
void render_object_load_texture(render_object_t *object, const char *texture_filepath);
void render_object_load_shaders(render_object_t *object, const char *vertex_shader_filepath, const char *fragment_shader_filepath);

void render_object_set_uniform_mat4(render_object_t *object, const char *uniform_name, float *mat4);

void render_object_draw(render_object_t *object);

void render_object_delete(render_object_t *object);
