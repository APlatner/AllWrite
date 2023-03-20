
#pragma once

#include "result.h"

typedef struct {
  const char *name;
} app_t;

result_t app_startup(app_t *app);
void app_shutdown(app_t *app);

result_t app_run(app_t *app);
