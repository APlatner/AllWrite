
#include "file_manager.h"

#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static FILE *active_file = NULL;
static char *active_filepath = NULL;

result_t file_manager_startup(void) { return NO_ERROR; }

void file_manager_shutdown(void) {
	if (active_file != NULL) {
		warn("attempting to shutdown file manager without closing active file.");
		file_manager_close();
	}
}

// create file if it doesn't exist
result_t file_manager_open(split_buffer_t *buffer, const char *filepath) {
	if (active_file != NULL) {
		warn("attempting to open a new file without closing the previous one.");
		file_manager_close();
	}

	if (filepath == NULL) {
		error("improper file path!");
		return FILE_MANAGER_ERROR;
	}

	if (access(filepath, F_OK)) {
		active_file = fopen(filepath, "w+");
	} else {
		if (access(filepath, R_OK | W_OK)) {
			error("missing permissions for specified file!");
			return FILE_MANAGER_ERROR;
		}

		if (strncmp(filepath, "text-editor.log", 32) == 0) {
			error("attempting to open protected file!");
			return FILE_MANAGER_ERROR;
		}

		active_file = fopen(filepath, "r+");
	}

	active_filepath = malloc(256);
	strncpy(active_filepath, filepath, 256);

	if (active_file == NULL) {
		error("failed to open file!");
		return FILE_MANAGER_ERROR;
	}

	char file_buffer[4096];
	size_t bytes_read = fread(file_buffer, 1, 4096, active_file);
	file_buffer[bytes_read] = '\0';
	split_buffer_create(buffer, file_buffer);

	return NO_ERROR;
}

void file_manager_close(void) {
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

result_t file_manager_save(split_buffer_t *buffer) {
	if (active_file == NULL || active_filepath == NULL) {
		error("expected active file to not be null!");
		return FILE_MANAGER_ERROR;
	}
	split_buffer_print(buffer);
	freopen(active_filepath, "w", active_file);

	fwrite(buffer->buffer, 1, buffer->pre_cursor_index, active_file);
	fwrite(&buffer->buffer[buffer->post_cursor_index], 1,
			MAX_BUFFER_SIZE - buffer->post_cursor_index - 1, active_file);

	return NO_ERROR;
}
