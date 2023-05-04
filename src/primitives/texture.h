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
void texture_destroy(texture_t *texture);
