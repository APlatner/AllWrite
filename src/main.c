#include <stdio.h>

#include "app.h"
#include "logger.h"

void print_result(result_t result) {
  switch (result.code) {
  case APP_ERROR:
    error("application error\n");
    error(result.message);
    break;
  case LOGGER_ERROR:
    error("logger error\n");
    error(result.message);
    break;
  case FILE_MANAGER_ERROR:
    error("file manager error\n");
    error(result.message);
    break;
  default:
    break;
  }
}

int main() {
  app_t app = {"text-editor"};
  result_t res = app_startup(&app);
  print_result(res);
  res = app_run(&app);
  print_result(res);
  app_shutdown(&app);

  return 0;
}