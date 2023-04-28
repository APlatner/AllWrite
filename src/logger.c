
#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static FILE *log_file = NULL;

void print_result(result_t result) {
  switch (result) {
  case APP_ERROR:
    fatal("application error");
    break;
  case LOGGER_ERROR:
    fatal("logger error");
    break;
  case FILE_MANAGER_ERROR:
    fatal("file manager error");
    break;
  default:
    break;
  }
}

result_t logger_startup(const char *filepath) {
  trace("logger starting.");
  if (filepath == NULL) {
    fatal("provided log file path is NULL!");
    return LOGGER_ERROR;
  }
  log_file = fopen(filepath, "w+");
  if (log_file == NULL) {
    fatal("failed to open log file!");
    return LOGGER_ERROR;
  }

  return NO_ERROR;
}

void logger_shutdown(void) {
  if (log_file == NULL) {
    return;
  }
  fclose(log_file);
  log_file = NULL;
}

void fatal(char *message) {
  if (log_file == NULL) {
    printf("[FATAL]|%s\n", message);
  } else {
    fprintf(log_file, "[FATAL]|%s\n", message);
    fflush(log_file);
  }
}

void error(char *message) {
  if (log_file == NULL) {
    printf("[ERROR]|%s\n", message);
  } else {
    fprintf(log_file, "[ERROR]|%s\n", message);
    fflush(log_file);
  }
}

void warn(char *message) {
  if (log_file == NULL) {
    printf("[WARN] |%s\n", message);
  } else {
    fprintf(log_file, "[WARN] |%s\n", message);
    fflush(log_file);
  }
}

void info(char *message) {
  if (log_file == NULL) {
    printf("[INFO] |%s\n", message);
  } else {
    fprintf(log_file, "[INFO] |%s\n", message);
    fflush(log_file);
  }
}

void _debug(char *file, int line, char *message, ...) {
  char buffer[256];
  va_list va;
  va_start(va, message);
  vsprintf(buffer, message, va);
  va_end(va);
  if (log_file == NULL) {
    printf("[DEBUG]|[FILE:%s][LINE:%i] %s\n", file, line, buffer);
  } else {
    fprintf(log_file, "[DEBUG]|[FILE:%s][LINE:%i] %s\n", file, line, buffer);
    fflush(log_file);
  }
}

void _trace(char *file, int line, char *message) {
  if (log_file == NULL) {
    printf("[TRACE]|[FILE:%s][LINE:%i] %s\n", file, line, message);
  } else {
    fprintf(log_file, "[TRACE]|[FILE:%s][LINE:%i] %s\n", file, line, message);
    fflush(log_file);
  }
}
