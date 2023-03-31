
#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static FILE *log_file = NULL;

void print_result(result_t result) {
  switch (result.code) {
  case APP_ERROR:
    error("application error");
    error(result.message);
    break;
  case LOGGER_ERROR:
    error("logger error");
    error(result.message);
    break;
  case FILE_MANAGER_ERROR:
    error("file manager error");
    error(result.message);
    break;
  case NO_ERROR:
    trace(result.message);
    break;
  default:
    break;
  }
}

result_t logger_startup(const char *filepath) {
  trace("logger starting.");
  if (filepath == NULL) {
    return (result_t){LOGGER_ERROR, "provided log file path is NULL!"};
  }
  log_file = fopen(filepath, "w+");
  if (log_file == NULL) {
    return (result_t){LOGGER_ERROR, "failed to open log file!"};
  }

  return (result_t){NO_ERROR, "started logger"};
}

void logger_shutdown() {
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
  }
}

void _trace(char *file, int line, char *message) {
  if (log_file == NULL) {
    printf("[TRACE]|[FILE:%s][LINE:%i] %s\n", file, line, message);
  } else {
    fprintf(log_file, "[TRACE]|[FILE:%s][LINE:%i] %s\n", file, line, message);
  }
}
