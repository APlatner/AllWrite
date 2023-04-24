#pragma once

#include "result.h"
#include "split_buffer.h"

result_t file_manager_startup();
void file_manager_shutdown();

result_t file_manager_open(split_buffer_t *buffer, const char *filepath);
void file_manager_close();

void file_manager_delete(const char *filepath);
result_t file_manager_save(split_buffer_t *buffer);
