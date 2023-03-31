
#include "file_manager.h"

#include "logger.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static FILE *active_file = NULL;
static char *active_filepath = NULL;

result_t file_manager_startup() {
  return (result_t){NO_ERROR, "file manager started."};
}

void file_manager_shutdown() {
  if (active_file != NULL) {
    warn("attempting to shutdown file manager without closing active file.");
    file_manager_close();
  }
}

// create file if it doesn't exist
result_t file_manager_open(const char *filepath) {
  if (active_file != NULL) {
    warn("attempting to open a new file without closing the previous one.");
    file_manager_close();
  }

  if (filepath == NULL) {
    error("improper file path!");
    return (result_t){FILE_MANAGER_ERROR, "file manager!"};
  }

  if (access(filepath, F_OK)) {
    error("specified file does not exist!");
    return (result_t){FILE_MANAGER_ERROR, "file manager!"};
  }

  if (access(filepath, R_OK | W_OK)) {
    error("missing permissions for specified file!");
    return (result_t){FILE_MANAGER_ERROR, "file manager!"};
  }

  if (strncmp(filepath, "text-editor-log.txt", 32) == 0) {
    error("attempting to open protected file!");
    return (result_t){FILE_MANAGER_ERROR, "file manager!"};
  }

  active_filepath = malloc(256);
  strncpy(active_filepath, filepath, 256);

  active_file = fopen(filepath, "r+");
  if (active_file == NULL)
    return (result_t){FILE_MANAGER_ERROR, "failed to open file!"};

  // use memcheck
  char *string = read_file(active_file);
  // split_buffer_t split_buffer;

  return (result_t){NO_ERROR, NULL};
}

void file_manager_close() {
  if (active_file == NULL) {
    warn("attempting to close an unopened file.");
    return;
  }

  free(active_filepath);
  active_filepath = NULL;

  fclose(active_file);
  active_file = NULL;
}

void file_manager_delete(const char *filepath) {
  if (filepath == NULL) {
    error("improper active file path!");
    return;
  }

  if (strncmp(filepath, "text-editor-log.txt", 32) == 0) {
    error("attempting to delete protected file!");
    return;
  }

  debug("deleting %s.", filepath);

  if (remove(filepath)) {
    error("failed to delete specified file!");
  }
}

// void file_manager_save();
