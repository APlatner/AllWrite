#include "app.h"

#include "file_manager.h"
#include "logger.h"
#include "split_buffer.h"

// add static buffer

result_t app_startup(app_t *app) {
  trace("app starting...");
  // rename text-editor-log.txt to text-editor.log
  result_t res = logger_startup("text-editor-log.txt");
  if (res.code != NO_ERROR) {
    return res;
  }

  res = file_manager_startup();
  if (res.code != NO_ERROR) {
    return res;
  }

  return (result_t){NO_ERROR, "application started."};
}

void app_shutdown(app_t *app) {
  info("app shutting.");
  file_manager_shutdown();
  logger_shutdown();
}

// show buffer being used with key input
void buffer_test() {
  split_buffer_t buffer;
  split_buffer_create(&buffer, "Hello, World!");
  info(buffer.buffer);
}

result_t app_run(app_t *app) {
  info("app running");
  buffer_test();

  result_t res = file_manager_open("test.txt");

  char c = 'a';
  while (c != 'q') {
    printf("> ");
    c = fgetc(stdin);
    debug("character entered: %c", c);
  }

  file_manager_close();
  return res;
}
