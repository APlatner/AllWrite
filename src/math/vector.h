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

typedef union ivec2_t {
	int data[2];
	struct {
		int x;
		int y;
	};
} ivec2_t;

typedef union uvec2_t {
	uint32_t data[2];
	struct {
		uint32_t x;
		uint32_t y;
	};
} uvec2_t;

typedef union lvec2_t {
	long data[2];
	struct {
		long x;
		long y;
	};
} lvec2_t;

vec2_t vec2_scale(vec2_t vec2, float scaler);
vec2_t vec2_normalize(vec2_t vec2);
void vec4_print(vec4_t vec4);
