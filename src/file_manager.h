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
#include "split_buffer.h"

result_t file_manager_startup(void);
void file_manager_shutdown(void);

result_t file_manager_open(split_buffer_t *buffer, const char *filepath);
void file_manager_close(void);

void file_manager_delete(const char *filepath);
result_t file_manager_save(split_buffer_t *buffer);

char *read_file(FILE *file);
