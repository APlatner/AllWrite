#include "split_buffer.h"

#include "logger.h"

#include <string.h>

void split_buffer_create(split_buffer_t *split_buffer, const char *string) {
  strncpy(split_buffer->buffer, string, MAX_BUFFER_SIZE - 1);
  size_t length = strlen(string);
  if (strlen(string) > MAX_BUFFER_SIZE - 1) {
    length = MAX_BUFFER_SIZE - 1;
  }
  split_buffer->buffer[length] = '\0';

  split_buffer->pre_cursor_index = (long)length;
  split_buffer->post_cursor_index = MAX_BUFFER_SIZE - 1;
  split_buffer->current_size = (long)length;
  debug("pre cursor index: %ld, post cursor index: %ld, current size: %ld",
        split_buffer->pre_cursor_index, split_buffer->post_cursor_index,
        split_buffer->current_size);
}

void split_buffer_move(split_buffer_t *split_buffer, long distance) {
  if (!distance) {
    error("distance must be non zero!");
    return;
  }
  if (split_buffer->pre_cursor_index + distance > split_buffer->current_size ||
      split_buffer->pre_cursor_index < 0) {
    error("distance must be within the bounds of the buffer's current size!");
    return;
  }

  if (distance > 0) {
    memcpy(&split_buffer->buffer[split_buffer->pre_cursor_index],
           &split_buffer->buffer[split_buffer->post_cursor_index], distance);
  } else if (distance < 0) {
    memcpy(&split_buffer->buffer[split_buffer->post_cursor_index + distance],
           &split_buffer->buffer[split_buffer->pre_cursor_index + distance],
           -1 * distance);
  }

  split_buffer->pre_cursor_index += distance;
  split_buffer->post_cursor_index += distance;
  debug("pre cursor index: %ld, post cursor index: %ld, current size: %ld",
        split_buffer->pre_cursor_index, split_buffer->post_cursor_index,
        split_buffer->current_size);
}

void split_buffer_append(split_buffer_t *split_buffer, char c) {
  if (!c) {
    error("character must non null!");
    return;
  }

  if (split_buffer->current_size == MAX_BUFFER_SIZE) {
    error("buffer is full!");
    return;
  }

  split_buffer->current_size++;
  split_buffer->buffer[split_buffer->pre_cursor_index] = c;
  split_buffer->pre_cursor_index++;
  debug("pre cursor index: %ld, post cursor index: %ld, current size: %ld",
        split_buffer->pre_cursor_index, split_buffer->post_cursor_index,
        split_buffer->current_size);
}

char split_buffer_remove(split_buffer_t *split_buffer) {
  if (split_buffer->current_size == 0) {
    error("buffer is empty!");
    return;
  }

  split_buffer->current_size--;
  split_buffer->pre_cursor_index--;
  debug("pre cursor index: %ld, post cursor index: %ld, current size: %ld",
        split_buffer->pre_cursor_index, split_buffer->post_cursor_index,
        split_buffer->current_size);
}

void split_buffer_print(split_buffer_t *split_buffer) {
  char string[MAX_BUFFER_SIZE];
  memcpy(string, split_buffer->buffer, split_buffer->pre_cursor_index);
  string[split_buffer->pre_cursor_index] = '\0';
  debug("pre cursor buffer: %s", string);
  memcpy(string, &split_buffer->buffer[split_buffer->post_cursor_index],
         MAX_BUFFER_SIZE - split_buffer->post_cursor_index);
  string[MAX_BUFFER_SIZE - split_buffer->post_cursor_index] = '\0';
  debug("post cursor buffer: %s", string);
}