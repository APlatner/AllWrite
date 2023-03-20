
#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static FILE *log_file = NULL;

result_t logger_startup(const char *filepath) {
  if (filepath == NULL) {
    return (result_t){LOGGER_ERROR, "provided log file path is NULL!"};
  }
  log_file = fopen(filepath, "w+");
  if (log_file == NULL) {
    return (result_t){LOGGER_ERROR, "failed to open log file!"};
  }

  info("logger started.");

  return (result_t){NO_ERROR, NULL};
}

void logger_shutdown() {
  if (log_file == NULL) {
    return;
  }

  info("logger shutdown");
  fclose(log_file);
}

void fatal(char *message) {
  if (log_file == NULL) {
    printf("[FATAL]|%s\n", message);
  } else {
    fprintf(log_file, "[FATAL]|%s\n", message);
  }
}

void error(char *message) {
  if (log_file == NULL) {
    printf("[ERROR]|%s\n", message);
  } else {
    fprintf(log_file, "[ERROR]|%s\n", message);
  }
}

void warn(char *message) {
  if (log_file == NULL) {
    printf("[WARN] |%s\n", message);
  } else {
    fprintf(log_file, "[WARN] |%s\n", message);
  }
}

void info(char *message) {
  if (log_file == NULL) {
    printf("[INFO] |%s\n", message);
  } else {
    fprintf(log_file, "[INFO] |%s\n", message);
  }
}

void _debug(char *filepath, int line, char *message, ...) {
  char buffer[256];
  va_list va;
  va_start(va, message);
  vsprintf(buffer, message, va);
  va_end(va);
  if (log_file == NULL) {
    printf("[DEBUG]|FILE:%s, LINE:%i, %s\n", filepath, line, buffer);
  } else {
    fprintf(log_file, "[DEBUG]|FILE:%s, LINE:%i, %s\n", filepath, line, buffer);
  }
}

void _trace(char *filepath, int line, char *message) {
  if (log_file == NULL) {
    printf("[TRACE]|LINE:%i, %s\n", line, message);
  } else {
    fprintf(log_file, "[TRACE]|LINE:%i, %s\n", line, message);
  }
}