#include "split_buffer.h"

#include <string.h>

void split_buffer_create(split_buffer_t *split_buffer, const char *string) {
  strncpy(split_buffer->buffer, string, MAX_BUFFER_SIZE - 1);
  size_t length = strlen(string);
  if (strlen(string) > MAX_BUFFER_SIZE - 1) {
    length = MAX_BUFFER_SIZE - 1;
  }

  split_buffer->pre_cursor_index = length;
  split_buffer->post_cursor_index = MAX_BUFFER_SIZE - 1;
  split_buffer->current_size = length;
  split_buffer->max_size = MAX_BUFFER_SIZE - 1;
}

void split_buffer_advance(split_buffer_t *split_buffer, long distance) {
  if (distance > 0) {

  } else if (distance < 0) {
    memcpy(split_buffer->buffer + split_buffer->post_cursor_index + distance,
           split_buffer->buffer + split_buffer->pre_cursor_index + distance,
           distance * (-1));
  }

  split_buffer->pre_cursor_index += distance;
  split_buffer->post_cursor_index += distance;
}

void split_buffer_append(split_buffer_t *split_buffer, char c);
char split_buffer_remove(split_buffer_t *split_buffer);