#pragma once

#include <stddef.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 4096

typedef struct split_buffer_t {
  long pre_cursor_index;
  long post_cursor_index;
  long current_size;
  char buffer[MAX_BUFFER_SIZE];
} split_buffer_t;

void split_buffer_create(split_buffer_t *split_buffer, const char *string);

void split_buffer_move(split_buffer_t *split_buffer, long distance);

void split_buffer_append(split_buffer_t *split_buffer, char c);
char split_buffer_remove(split_buffer_t *split_buffer);

void split_buffer_print(split_buffer_t *split_buffer);
