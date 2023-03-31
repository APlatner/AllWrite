#pragma once

#include "result.h"

result_t file_manager_startup();
void file_manager_shutdown();

result_t file_manager_open(const char *filepath);
void file_manager_close();

void file_manager_delete(const char *filepath);
void file_manager_save();
