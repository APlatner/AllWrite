#include <stdio.h>

#include "app.h"

int main() {
  app_t app = {"text-editor"};
  result_t res = app_startup(&app);
  print_result(res);
  res = app_run(&app);
  print_result(res);
  app_shutdown(&app);

  return 0;
}