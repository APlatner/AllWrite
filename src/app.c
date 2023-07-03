#include "app.h"
#define NDEBUG
#include "file_manager.h"
#include "logger.h"
#include "math/matrix.h"
#include "primitives/font.h"
#include "primitives/quad.h"
#include "primitives/texture.h"
#include "split_buffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _POSIX_C_SOURCE 199309L
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define CLOCK_MONOTONIC 1

enum input_context_t {
	NO_CONTEXT = 0,
	CONTROL_INPUT_CONTEXT,
	TEXT_INPUT_CONTEXT,
	FILE_INPUT_CONTEXT,
};

int old_input_context;

typedef struct app_state_t {
	split_buffer_t buffer;
	char file_manager_text[256];
	char filename[256];
	int input_context;
	long cursor_position;
	float vertical_offset;
} app_state_t;

typedef struct app_t {
	GLFWwindow *window;
	app_state_t state;
} app_t;

static app_t app;

void check_for_state_change(app_t previous_state);
void renderer_debug_callback(uint32_t source, uint32_t type, uint32_t id,
    uint32_t severity, int32_t length, const char *message,
    const void *user_param);
void change_input_context(int new_context);
void key_callback(
    GLFWwindow *window, int key, int scancode, int action, int mods);
void file_input_callback(int key, int scancode, int action, int mods);
void control_input_callback(int key, int scancode, int action, int mods);
void text_input_callback(int key, int scancode, int action, int mods);

result_t app_startup(void) {
	trace("app starting...");
	// rename text-editor-log.txt to text-editor.log
	result_t res = logger_startup("text-editor.log");
	if (res != NO_ERROR) {
		return res;
	}

	res = file_manager_startup();
	if (res != NO_ERROR) {
		app_shutdown();
		return res;
	}

	if (!glfwInit()) {
		fatal("failed to initialize GLFW!");
		app_shutdown();
		return WINDOWING_ERROR;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	app.window = glfwCreateWindow(800, 600, "text-editor", NULL, NULL);
	if (!app.window) {
		fatal("failed to create window!");
		app_shutdown();
		return WINDOWING_ERROR;
	}
	glfwMakeContextCurrent(app.window);
	glfwSetKeyCallback(app.window, key_callback);

	if (glewInit() != GLEW_OK) {
		fatal("failed to initialize GLEW!");
		app_shutdown();
		return OPENGL_ERROR;
	}

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(renderer_debug_callback, app.window);

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);

	app.state.filename[0] = '\0';
	app.state.file_manager_text[0] = '\0';
	app.state.cursor_position = 0;
	split_buffer_create(&app.state.buffer, "");
	app.state.vertical_offset = 0.0f;
	app.state.input_context = NO_CONTEXT;

	return NO_ERROR;
}

void app_shutdown(void) {
	info("app shutting.");
	glfwTerminate();
	file_manager_shutdown();
	logger_shutdown();
}

double app_get_time(void) {
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	return now.tv_sec + now.tv_nsec * 0.000000001;
}

void app_sleep(int64_t ms) {
	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000 * 1000;
	nanosleep(&ts, 0);
}

result_t app_run(void) {
	info("app running");

	mat4_t projection = mat4_ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);

	texture_t texture;
	texture.position = (vec2_t){{0.0f, 0.0f}};
	texture.size = (vec2_t){{800.0f, 600.0f}};
	texture.color = (vec4_t){{0.1f, 0.2f, 0.3f, 1.0f}};
	texture_load(&texture);

	quad_t quad;
	quad.position = (vec2_t){{0.0f, 0.0f}};
	quad.size = (vec2_t){{800.0f, 30.0f}};
	quad.color = (vec4_t){{0.2f, 0.2f, 0.2f, 1.0f}};
	quad_load(&quad);

	font_t filename_display;
	filename_display.position = (vec2_t){{0.0f, 0.0f}};
	filename_display.size = (vec2_t){{600.0f, 30.0f}};
	filename_display.color = (vec4_t){{0.8f, 0.8f, 0.9f, 1.0f}};
	filename_display.font_size = 24;
	font_load(&filename_display, -1, "res/fonts/NotoSans-Regular.ttf", NULL,
	    projection);

	font_t file_manager_hint;
	file_manager_hint.position = (vec2_t){{600.0f, 0.0f}};
	file_manager_hint.size = (vec2_t){{200.0f, 30.0f}};
	file_manager_hint.color = (vec4_t){{0.8f, 0.8f, 0.9f, 1.0f}};
	file_manager_hint.font_size = 24;
	font_load(&file_manager_hint, -1, "res/fonts/NotoSans-Regular.ttf", NULL,
	    projection);

	font_t font;
	font.position = (vec2_t){{0.0f, 30.0f}};
	font.size = (vec2_t){{800.0f, 600.0f}};
	font.color = (vec4_t){{0.8f, 0.8f, 0.9f, 1.0f}};
	font.font_size = 24;
	font_load(&font, -1, "res/fonts/Noto Mono Nerd Font Complete.ttf", NULL,
	    projection);

	app_state_t previous_state = app.state;

	while (!glfwWindowShouldClose(app.window)) {
		double start = app_get_time();
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		render_object_draw(&texture.object);
		render_object_draw(&font.object);
		render_object_draw(&quad.object);
		render_object_draw(&filename_display.object);
		render_object_draw(&file_manager_hint.object);

		glfwSwapBuffers(app.window);
		glfwPollEvents();

		// update application state if the two states dont match
		if (memcmp(&previous_state, &app.state, sizeof(app_state_t))) {
			trace("changed state");
			// update file content display
			if (app.state.buffer.current_size != previous_state.buffer.current_size) {
				char *string = split_buffer_to_string(&app.state.buffer);
				font_update(&font, app.state.buffer.pre_cursor_index, string,
				    app.state.vertical_offset);
				free(string);
			}
			// update filename display
			if (strcmp(app.state.filename, previous_state.filename)) {
				font_update(&filename_display, -1, app.state.filename, 0.0f);
			}
			// update file manager state display
			if (strcmp(
			        app.state.file_manager_text, previous_state.file_manager_text)) {
				font_update(&file_manager_hint, -1, app.state.file_manager_text, 0.0f);
			}
			// update cursor position
			if (app.state.buffer.pre_cursor_index !=
			    previous_state.buffer.pre_cursor_index) {
				char *string = split_buffer_to_string(&app.state.buffer);
				font_update(&font, app.state.buffer.pre_cursor_index, string,
				    app.state.vertical_offset);
				free(string);
			}
			// update projection matrix
			if (app.state.vertical_offset != previous_state.vertical_offset) {
				char *string = split_buffer_to_string(&app.state.buffer);
				font_update(&font, app.state.buffer.pre_cursor_index, string,
				    app.state.vertical_offset);
				free(string);
			}
			previous_state = app.state;
		}
		double current = app_get_time();
		if (current - start < ((1.0 / 60.0) * 1000000000)) {
			app_sleep((int64_t)((1.0 / 60.0) * 1000) - (current - start));
		}
	}

	quad_destroy(&quad);
	texture_destroy(&texture);
	font_destroy(&font);

	return NO_ERROR;
}

void check_for_state_change(app_t previous_state) {
	if (!memcmp(&previous_state, &app, sizeof(app_t)))
		return;
}

void change_input_context(int new_context) {
	if (new_context == app.state.input_context) {
		return;
	}

	const char *context_strings[] = {
	    "No Context",
	    "Control Context",
	    "Text Context",
	    "File Context",
	};

	debug("switching context from %s to %s",
	    context_strings[app.state.input_context], context_strings[new_context]);
	old_input_context = app.state.input_context;
	app.state.input_context = new_context;
}

void key_callback(
    GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_LEFT_CONTROL:
		case GLFW_KEY_RIGHT_CONTROL:
			change_input_context(CONTROL_INPUT_CONTEXT);
			break;
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;

		default:
			break;
		}
	}
	if (action != GLFW_RELEASE) {
		switch (app.state.input_context) {
		case CONTROL_INPUT_CONTEXT:
			control_input_callback(key, scancode, action, mods);
			break;
		case TEXT_INPUT_CONTEXT:
			text_input_callback(key, scancode, action, mods);
			break;
		case FILE_INPUT_CONTEXT:
			file_input_callback(key, scancode, action, mods);
		}
	}
	if (action == GLFW_RELEASE) {
		switch (key) {
		case GLFW_KEY_LEFT_CONTROL:
		case GLFW_KEY_RIGHT_CONTROL:
			change_input_context(old_input_context);
			break;
		default:
			break;
		}
	}
}

void control_input_callback(int key, int scancode, int action, int mods) {
	result_t res;
	switch (key) {
	case GLFW_KEY_S:
		res = file_manager_save(&app.state.buffer);
		if (res != NO_ERROR) {
			return;
		}
		sprintf(app.state.file_manager_text, "saved %s", app.state.filename);
		change_input_context(TEXT_INPUT_CONTEXT);
		break;
	case GLFW_KEY_Q:
		sprintf(app.state.file_manager_text, "closed %s", app.state.filename);
		file_manager_close();
		split_buffer_destroy(&app.state.buffer);
		app.state.filename[0] = '\0';
		old_input_context = NO_CONTEXT;
		break;
	case GLFW_KEY_O: {
		strcpy(app.state.file_manager_text, "opening file");
		old_input_context = FILE_INPUT_CONTEXT;
	} break;
	case GLFW_KEY_UP: {
		app.state.vertical_offset += 14.0f;
	} break;
	case GLFW_KEY_DOWN: {
		app.state.vertical_offset -= 14.0f;
	} break;

	default:
		break;
	}
}

void filename_append(char c) {
	long length = strlen(app.state.filename);
	if (length >= 255) {
		return;
	}
	app.state.filename[length] = c;
	app.state.filename[length + 1] = '\0';
}

char string_pop(char *string) {
	long length = strlen(string);
	if (length <= 0) {
		return '\0';
	}
	char c = string[length - 1];
	string[length - 1] = '\0';

	return c;
}

void file_input_callback(int key, int scancode, int action, int mods) {
	int shift = mods & GLFW_MOD_SHIFT;
	switch (key) {
	case GLFW_KEY_0:
		filename_append((char)(key - shift * 7));
		break;
	case GLFW_KEY_1:
		filename_append((char)(key - shift * 16));
		break;
	case GLFW_KEY_2:
		filename_append((char)(key + shift * 14));
		break;
	case GLFW_KEY_3:
	case GLFW_KEY_4:
	case GLFW_KEY_5:
		filename_append((char)(key - shift * 16));
		break;
	case GLFW_KEY_6:
		filename_append((char)(key + shift * 40));
		break;
	case GLFW_KEY_7:
		filename_append((char)(key - shift * 17));
		break;
	case GLFW_KEY_8:
		filename_append((char)(key - shift * 14));
		break;
	case GLFW_KEY_9:
		filename_append((char)(key - shift * 17));
		break;
	case GLFW_KEY_A:
	case GLFW_KEY_B:
	case GLFW_KEY_C:
	case GLFW_KEY_D:
	case GLFW_KEY_E:
	case GLFW_KEY_F:
	case GLFW_KEY_G:
	case GLFW_KEY_H:
	case GLFW_KEY_I:
	case GLFW_KEY_J:
	case GLFW_KEY_K:
	case GLFW_KEY_L:
	case GLFW_KEY_M:
	case GLFW_KEY_N:
	case GLFW_KEY_O:
	case GLFW_KEY_P:
	case GLFW_KEY_Q:
	case GLFW_KEY_R:
	case GLFW_KEY_S:
	case GLFW_KEY_T:
	case GLFW_KEY_U:
	case GLFW_KEY_V:
	case GLFW_KEY_W:
	case GLFW_KEY_X:
	case GLFW_KEY_Y:
	case GLFW_KEY_Z:
		filename_append((char)(key + (!shift) * 32));
		break;
	case GLFW_KEY_SPACE:
		filename_append((char)key);
		break;
	case GLFW_KEY_COMMA:
		filename_append((char)(key + shift * 16));
		break;
	case GLFW_KEY_SEMICOLON:
		filename_append((char)(key - shift * 1));
		break;
	case GLFW_KEY_PERIOD:
		filename_append((char)(key + shift * 16));
		break;
	case GLFW_KEY_SLASH:
		filename_append((char)(key + shift * 16));
		break;
	case GLFW_KEY_GRAVE_ACCENT:
		filename_append((char)(key + shift * 30));
		break;
	case GLFW_KEY_LEFT_BRACKET:
	case GLFW_KEY_RIGHT_BRACKET:
	case GLFW_KEY_BACKSLASH:
		filename_append((char)(key + shift * 32));
		break;
	case GLFW_KEY_APOSTROPHE:
		filename_append((char)(key - shift * 5));
		break;
	case GLFW_KEY_EQUAL:
		filename_append((char)(key - shift * 18));
		break;
	case GLFW_KEY_MINUS:
		filename_append((char)(key + shift * 50));
		break;
	case GLFW_KEY_ENTER: {
		result_t res = file_manager_open(&app.state.buffer, app.state.filename);
		if (res != NO_ERROR) {
			error("error opening file!");
			return;
		}
		change_input_context(TEXT_INPUT_CONTEXT);
	} break;
	case GLFW_KEY_BACKSPACE:
		string_pop(app.state.filename);
		break;
	default:
		break;
	}
}

void text_input_callback(int key, int scancode, int action, int mods) {
	int shift = mods & GLFW_MOD_SHIFT;
	switch (key) {
	case GLFW_KEY_0:
		split_buffer_append(&app.state.buffer, (char)(key - shift * 7));
		break;
	case GLFW_KEY_1:
		split_buffer_append(&app.state.buffer, (char)(key - shift * 16));
		break;
	case GLFW_KEY_2:
		split_buffer_append(&app.state.buffer, (char)(key + shift * 14));
		break;
	case GLFW_KEY_3:
	case GLFW_KEY_4:
	case GLFW_KEY_5:
		split_buffer_append(&app.state.buffer, (char)(key - shift * 16));
		break;
	case GLFW_KEY_6:
		split_buffer_append(&app.state.buffer, (char)(key + shift * 40));
		break;
	case GLFW_KEY_7:
		split_buffer_append(&app.state.buffer, (char)(key - shift * 17));
		break;
	case GLFW_KEY_8:
		split_buffer_append(&app.state.buffer, (char)(key - shift * 14));
		break;
	case GLFW_KEY_9:
		split_buffer_append(&app.state.buffer, (char)(key - shift * 17));
		break;
	case GLFW_KEY_A:
	case GLFW_KEY_B:
	case GLFW_KEY_C:
	case GLFW_KEY_D:
	case GLFW_KEY_E:
	case GLFW_KEY_F:
	case GLFW_KEY_G:
	case GLFW_KEY_H:
	case GLFW_KEY_I:
	case GLFW_KEY_J:
	case GLFW_KEY_K:
	case GLFW_KEY_L:
	case GLFW_KEY_M:
	case GLFW_KEY_N:
	case GLFW_KEY_O:
	case GLFW_KEY_P:
	case GLFW_KEY_Q:
	case GLFW_KEY_R:
	case GLFW_KEY_S:
	case GLFW_KEY_T:
	case GLFW_KEY_U:
	case GLFW_KEY_V:
	case GLFW_KEY_W:
	case GLFW_KEY_X:
	case GLFW_KEY_Y:
	case GLFW_KEY_Z:
		split_buffer_append(&app.state.buffer, (char)(key + (!shift) * 32));
		break;
	case GLFW_KEY_SPACE:
		split_buffer_append(&app.state.buffer, (char)key);
		break;
	case GLFW_KEY_SEMICOLON:
		split_buffer_append(&app.state.buffer, (char)(key - shift * 1));
		break;
	case GLFW_KEY_COMMA:
		split_buffer_append(&app.state.buffer, (char)(key + shift * 16));
		break;
	case GLFW_KEY_PERIOD:
		split_buffer_append(&app.state.buffer, (char)(key + shift * 16));
		break;
	case GLFW_KEY_SLASH:
		split_buffer_append(&app.state.buffer, (char)(key + shift * 16));
		break;
	case GLFW_KEY_EQUAL:
		split_buffer_append(&app.state.buffer, (char)(key - shift * 18));
		break;
	case GLFW_KEY_MINUS:
		split_buffer_append(&app.state.buffer, (char)(key + shift * 50));
		break;
	case GLFW_KEY_GRAVE_ACCENT:
		split_buffer_append(&app.state.buffer, (char)(key + shift * 30));
		break;
	case GLFW_KEY_LEFT_BRACKET:
	case GLFW_KEY_RIGHT_BRACKET:
	case GLFW_KEY_BACKSLASH:
		split_buffer_append(&app.state.buffer, (char)(key + shift * 32));
		break;
	case GLFW_KEY_APOSTROPHE:
		split_buffer_append(&app.state.buffer, (char)(key - shift * 5));
		break;
	case GLFW_KEY_ENTER:
		split_buffer_append(&app.state.buffer, '\n');
		break;
	case GLFW_KEY_TAB:
		split_buffer_append(&app.state.buffer, '\t');
		break;
	case GLFW_KEY_BACKSPACE:
		split_buffer_remove(&app.state.buffer);
		break;
	case GLFW_KEY_LEFT: {
		result_t res = split_buffer_move(&app.state.buffer, -1);
		if (res != NO_ERROR) {
			return;
		}
	} break;
	case GLFW_KEY_RIGHT: {
		result_t res = split_buffer_move(&app.state.buffer, 1);
		if (res != NO_ERROR) {
			return;
		}
	} break;
	case GLFW_KEY_UP: {

		result_t res = split_buffer_ascend(&app.state.buffer);
		if (res != NO_ERROR) {
			return;
		}
	} break;
	case GLFW_KEY_DOWN: {
		result_t res = split_buffer_descend(&app.state.buffer);
		if (res != NO_ERROR) {
			return;
		}
	}
	default:
		break;
	}
}

void renderer_debug_callback(uint32_t source, uint32_t type, uint32_t id,
    uint32_t severity, int32_t length, const char *message,
    const void *user_param) {
	const char *_source;
	const char *_type;
	const char *_severity;
	switch (source) {
	case GL_DEBUG_SOURCE_API:
		_source = "API";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		_source = "Application";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		_source = "Other";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		_source = "Shader Compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		_source = "Third Party";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		_source = "Window System";
		break;
	default:
		_source = "Unknown";
	}
	switch (type) {
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		_type = "Depreciated Behavior";
		break;
	case GL_DEBUG_TYPE_ERROR:
		_type = "Error";
		break;
	case GL_DEBUG_TYPE_MARKER:
		_type = "Marker";
		break;
	case GL_DEBUG_TYPE_OTHER:
		_type = "Other";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		_type = "Performance";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		_type = "Pop Group";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		_type = "Portability";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		_type = "Push Group";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		_type = "Undefined Behavior";
		break;
	default:
		_type = "Unknown";
	}
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: {
		_severity = "High";
		// glfwSetWindowShouldClose((GLFWwindow *)user_param, GLFW_TRUE);
	} break;
	case GL_DEBUG_SEVERITY_MEDIUM: {
		_severity = "Medium";
		// glfwSetWindowShouldClose((GLFWwindow *)user_param, GLFW_TRUE);
	} break;
	case GL_DEBUG_SEVERITY_LOW: {
		_severity = "Low";
		// glfwSetWindowShouldClose((GLFWwindow *)user_param, GLFW_TRUE);
	} break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		_severity = "Notification";
		break;
	default:
		_severity = "Unknown";
		break;
	}

	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
		debug("Source: %s, Type: %s, ID: %u, Severity: %s, Message: %s", _source,
		    _type, id, _severity, message);
	}
}
