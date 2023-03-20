
#include "file_manager.h"

#include "logger.h"

#include <stdio.h>
#include <string.h>

static FILE *active_file = NULL;

result_t file_manager_startup() {
  info("file manager started.");
  return (result_t){NO_ERROR, NULL};
}

void file_manager_shutdown() {
  if (active_file != NULL) {
    warn("attempting to shutdown file manager without closing active file.");
    file_manager_close();
  }

  info("file manager shutdown.");
}

result_t file_manager_open(const char *filepath) {
  if (active_file != NULL) {
    warn("attempting to open a new file without closing the previous one.");
    file_manager_close();
  }

  active_file = fopen(filepath, "r+");
  if (active_file == NULL)
    return (result_t){FILE_MANAGER_ERROR, "failed to open file!"};

  return (result_t){NO_ERROR, NULL};
}

void file_manager_close() {
  if (active_file == NULL) {
    warn("attempting to close an unopened file.");
    return;
  }

  fclose(active_file);
  active_file = NULL;
}