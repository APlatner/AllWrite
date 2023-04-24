#pragma once

#include <stdio.h>

typedef enum result_t {
  NO_ERROR = 0,
  APP_ERROR,
  LOGGER_ERROR,
  FILE_MANAGER_ERROR,
  WINDOWING_ERROR,
  OPENGL_ERROR,
  TEXT_BUFFER_ERROR,
} result_t;

/*
typedef struct result_t {
  void *value;
  int code;
}
*/

void print_result(result_t result);
