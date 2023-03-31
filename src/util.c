#include "util.h"

#include "logger.h"

char *read_file(FILE *file) {
  fseek(file, 0, SEEK_END);
  long len = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *buffer = malloc(len + 1);

  size_t bytes_read = fread(buffer, 1, len, file);
  if (bytes_read != (size_t)len) {
    error("failed to read file contents!");
    free(buffer);
    return NULL;
  }

  return buffer;
}
