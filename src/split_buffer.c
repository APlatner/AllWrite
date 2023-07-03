#include "split_buffer.h"
#define NDEBUG
#include "logger.h"

#include <stdlib.h>
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

void split_buffer_destroy(split_buffer_t *split_buffer) {
	split_buffer->pre_cursor_index = 0;
	split_buffer->post_cursor_index = MAX_BUFFER_SIZE - 1;
	split_buffer->current_size = 0;
	split_buffer->buffer[0] = '\0';
}

result_t split_buffer_move(split_buffer_t *split_buffer, long distance) {
	if (!distance) {
		error("distance must be non zero!");
		return TEXT_BUFFER_ERROR;
	}
	if (split_buffer->pre_cursor_index + distance > split_buffer->current_size ||
	    split_buffer->pre_cursor_index + distance < 0) {
		error("distance must be within the bounds of the buffer's current size!");
		return TEXT_BUFFER_ERROR;
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
	debug("moved %ld", distance);

	return NO_ERROR;
}

result_t split_buffer_ascend(split_buffer_t *split_buffer) {
	if (split_buffer->pre_cursor_index == 0) {
		trace("end of buffer");
		return NO_ERROR;
	}
	for (int i = split_buffer->pre_cursor_index - 1; i > 0; i--) {
		char c = split_buffer->buffer[i];
		if (c == '\n') {
			split_buffer_move(split_buffer, i - split_buffer->pre_cursor_index);
			break;
		}
	}

	return NO_ERROR;
}

result_t split_buffer_descend(split_buffer_t *split_buffer) {
	if (split_buffer->pre_cursor_index == split_buffer->current_size) {
		trace("end of buffer");
		return NO_ERROR;
	}
	for (int i = split_buffer->post_cursor_index + 1; i < MAX_BUFFER_SIZE; i++) {
		char c = split_buffer->buffer[i];
		if (c == '\n') {
			split_buffer_move(split_buffer, i - split_buffer->post_cursor_index);
			break;
		}
	}

	return NO_ERROR;
}

result_t split_buffer_append(split_buffer_t *split_buffer, char c) {
	if (!c) {
		error("character must non null!");
		return TEXT_BUFFER_ERROR;
	}

	if (split_buffer->current_size == MAX_BUFFER_SIZE) {
		error("buffer is full!");
		return TEXT_BUFFER_ERROR;
	}

	split_buffer->current_size++;
	split_buffer->buffer[split_buffer->pre_cursor_index] = c;
	split_buffer->pre_cursor_index++;
	debug("pre cursor index: %ld, post cursor index: %ld, current size: %ld",
	    split_buffer->pre_cursor_index, split_buffer->post_cursor_index,
	    split_buffer->current_size);
	debug("character appended: %c", c);

	return NO_ERROR;
}

result_t split_buffer_remove(split_buffer_t *split_buffer) {
	if (split_buffer->current_size == 0) {
		error("buffer is empty!");
		return TEXT_BUFFER_ERROR;
	}

	split_buffer->current_size--;
	split_buffer->pre_cursor_index--;
	debug("pre cursor index: %ld, post cursor index: %ld, current size: %ld",
	    split_buffer->pre_cursor_index, split_buffer->post_cursor_index,
	    split_buffer->current_size);
	return NO_ERROR;
}

char *split_buffer_to_string(split_buffer_t *split_buffer) {
	if (split_buffer->current_size == 0) {
		return NULL;
	}
	char *string = malloc(split_buffer->current_size + 1);
	if (string == NULL) {
		return NULL;
	}
	strncpy(string, split_buffer->buffer, split_buffer->pre_cursor_index);
	string[split_buffer->pre_cursor_index] = '\0';
	debug("pre cursor buffer: %s; current_size: %ld", string,
	    split_buffer->current_size);
	strncat(string, &split_buffer->buffer[split_buffer->post_cursor_index],
	    MAX_BUFFER_SIZE - split_buffer->post_cursor_index);
	debug("post cursor buffer: %s", &string[split_buffer->pre_cursor_index]);

	return string;
}
