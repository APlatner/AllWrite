#pragma once

#include <stdio.h>

typedef enum result_t {
  NO_ERROR = 0,
  APP_ERROR,
  LOGGER_ERROR,
  FILE_MANAGER_ERROR,
} result_t;

void print_result(result_t result);
