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

#include "result.h"

#include <stddef.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 4096

typedef struct split_buffer_t {
  long pre_cursor_index;
  long post_cursor_index;
  long current_size;
  char buffer[MAX_BUFFER_SIZE];
} split_buffer_t;

void split_buffer_create(split_buffer_t *split_buffer, const char *string);
void split_buffer_destroy(split_buffer_t *split_buffer);

result_t split_buffer_move(split_buffer_t *split_buffer, long distance);

result_t split_buffer_append(split_buffer_t *split_buffer, char c);
result_t split_buffer_remove(split_buffer_t *split_buffer);

void split_buffer_print(split_buffer_t *split_buffer);
