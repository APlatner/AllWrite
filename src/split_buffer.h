#pragma once

#include <stddef.h>

#define MAX_BUFFER_SIZE 256

typedef struct split_buffer_t {
  size_t pre_cursor_index;
  size_t post_cursor_index;
  size_t current_size;
  size_t max_size;
  char buffer[MAX_BUFFER_SIZE];
} split_buffer_t;

void split_buffer_create(split_buffer_t *split_buffer, const char *string);

void split_buffer_advance(split_buffer_t *split_buffer, long distance);

void split_buffer_append(split_buffer_t *split_buffer, char c);
char split_buffer_remove(split_buffer_t *split_buffer);
