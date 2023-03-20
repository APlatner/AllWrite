#include "app.h"

#include "file_manager.h"
#include "logger.h"

result_t app_startup(app_t *app) {
  result_t res = logger_startup("text-editor-log.txt");
  if (res.code != NO_ERROR) {
    return res;
  }

  res = file_manager_startup();
  if (res.code != NO_ERROR) {
    return res;
  }

  info("app started.");

  return (result_t){NO_ERROR, NULL};
}

void app_shutdown(app_t *app) {
  file_manager_shutdown();
  info("app shutdown.");
  logger_shutdown();
}

result_t app_run(app_t *app) {
  info("app running");
  result_t res = file_manager_open("test.txt");

  char c = 'a';
  while (c != 'q') {
    printf("> ");
    c = fgetc(stdin);
    debug("character entered: %c", c);
  }

  file_manager_close("test.txt");
  return res;
}
