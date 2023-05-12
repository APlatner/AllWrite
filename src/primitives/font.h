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

#include <render_object.h>
#include <math/vector.h>

typedef struct font_t {
	render_object_t object;

	vec2_t position;
	vec2_t size;
	vec4_t color;

	char_glyph_t characters[128];
	float font_size;
	long cursor_position;
} font_t;

int font_load(font_t *font, const char *font_filepath, const char *string);
void font_update(font_t *font, const char *string);
void font_destroy(font_t *font);
