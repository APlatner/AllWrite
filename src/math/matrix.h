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

#include "vector.h"
#include <stdint.h>

typedef union  mat4_t {
    float data[16];
    struct {
      float x1, y1, z1, w1;
      float x2, y2, z2, w2;
      float x3, y3, z3, w3;
      float x4, y4, z4, w4;
    };
} mat4_t;

mat4_t mat4_ortho(float left, float right, float top, float bottom, float near,
                float far);
void mat4_print(mat4_t matrix);
vec4_t mat4_vec4_multiply(mat4_t matrix, vec4_t vec4);
