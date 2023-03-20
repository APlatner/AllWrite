#pragma once

#include <stdio.h>

typedef struct {
  enum {
    NO_ERROR = 0,
    APP_ERROR,
    LOGGER_ERROR,
    FILE_MANAGER_ERROR,
  } code;
  char *message;
} result_t;

void print_result(result_t result);
