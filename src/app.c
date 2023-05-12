#include "app.h"

#include "file_manager.h"
#include "logger.h"
#include "primitives/font.h"
#include "primitives/quad.h"
#include "primitives/texture.h"
#include "split_buffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum input_context_t {
	CONTROL_INPUT_CONTEXT = 0,
	TEXT_INPUT_CONTEXT,
	FILE_INPUT_CONTEXT,
};

typedef struct app_t {
	GLFWwindow *window;
	split_buffer_t buffer;
	char file_manager_text[256];
	char filename[256];
	int filename_index;
	int input_context;
	long cursor_position;
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

	app.filename[0] = '\0';
	app.filename_index = 0;
	app.cursor_position = 0;

	return NO_ERROR;
}

void app_shutdown(void) {
	info("app shutting.");
	glfwTerminate();
	file_manager_shutdown();
	logger_shutdown();
}

result_t app_run(void) {
	info("app running");

	split_buffer_create(&app.buffer, "");

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
	filename_display.cursor_position = -1;
	font_load(&filename_display, "res/fonts/NotoSans-Regular.ttf", app.filename);

	font_t file_manager_hint;
	file_manager_hint.position = (vec2_t){{600.0f, 0.0f}};
	file_manager_hint.size = (vec2_t){{200.0f, 30.0f}};
	file_manager_hint.color = (vec4_t){{0.8f, 0.8f, 0.9f, 1.0f}};
	file_manager_hint.font_size = 24;
	file_manager_hint.cursor_position = -1;
	font_load(&file_manager_hint, "res/fonts/NotoSans-Regular.ttf", "");

	font_t font;
	font.position = (vec2_t){{0.0f, 30.0f}};
	font.size = (vec2_t){{800.0f, 600.0f}};
	font.color = (vec4_t){{0.8f, 0.8f, 0.9f, 1.0f}};
	font.font_size = 24;
	font.cursor_position = 0;
	font_load(&font, "res/fonts/Noto Mono Nerd Font Complete.ttf", NULL);

	app_t previous_state = app;

	while (!glfwWindowShouldClose(app.window)) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// update display
		if (memcmp(&previous_state, &app, sizeof(app_t))) {
			if (previous_state.buffer.current_size != app.buffer.current_size) {
				char *string = split_buffer_to_string(&app.buffer);
				font.cursor_position = app.buffer.pre_cursor_index;
				font_update(&font, string);
				free(string);
				previous_state.buffer.current_size = app.buffer.current_size;
			}
			if (app.filename_index != filename_size) {
				font_update(&filename_display, app.filename);
				filename_size = app.filename_index;
			}
			if (strlen(app.file_manager_text) != file_manager_text_length) {
				font_update(&file_manager_hint, app.file_manager_text);
				file_manager_text_length = strlen(app.file_manager_text);
			}
			if (cursor_position != app.cursor_position) {
				char *string = split_buffer_to_string(&app.buffer);
				font.cursor_position = app.cursor_position;
				font_update(&font, string);
				free(string);
				cursor_position = app.cursor_position;
			}
		}

		render_object_draw(&texture.object);
		render_object_draw(&quad.object);
		render_object_draw(&filename_display.object);
		render_object_draw(&file_manager_hint.object);
		render_object_draw(&font.object);

		glfwSwapBuffers(app.window);
		glfwPollEvents();
	}

	quad_destroy(&quad);
	texture_destroy(&texture);
	font_destroy(&font);

	// file_manager_close();
	return NO_ERROR;
}

// void buffer_test(void) {
// 	split_buffer_t buffer;
// 	split_buffer_create(&buffer, "Hello,_World!");
// 	split_buffer_print(&buffer);
// 	split_buffer_move(&buffer, -4);
// 	split_buffer_print(&buffer);
// 	split_buffer_append(&buffer, 'c');
// 	split_buffer_print(&buffer);
// 	split_buffer_move(&buffer, 2);
// 	split_buffer_print(&buffer);
// 	split_buffer_remove(&buffer);
// 	split_buffer_print(&buffer);
// }

void check_for_state_change(app_t previous_state) {
	if (!memcmp(&previous_state, &app, sizeof(app_t)))
		return;
}

void change_input_context(int new_context) {
	const char *context_strings[] = {
	    "Control Context",
	    "Text Context",
	    "File Context",
	};

	debug("switching context from %s to %s", context_strings[app.input_context],
	    context_strings[new_context]);
	// printf("switching context from %s to %s\n", context_strings[input_context],
	//        context_strings[new_context]);
	app.input_context = new_context;
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

		switch (app.input_context) {
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
}

void control_input_callback(int key, int scancode, int action, int mods) {
	result_t res;
	switch (key) {
	case GLFW_KEY_S:
		res = file_manager_save(&app.buffer);
		if (res != NO_ERROR) {
			return;
		}
		sprintf(app.file_manager_text, "saved %s", app.filename);
		change_input_context(TEXT_INPUT_CONTEXT);
		break;
	case GLFW_KEY_Q:
		sprintf(app.file_manager_text, "closed %s", app.filename);
		file_manager_close();
		split_buffer_destroy(&app.buffer);
		app.filename[0] = '\0';
		app.filename_index = 0;
		change_input_context(FILE_INPUT_CONTEXT);
		break;
	case GLFW_KEY_O: {
		strcpy(app.file_manager_text, "opening file");
		debug(app.file_manager_text);
		change_input_context(FILE_INPUT_CONTEXT);
		app.filename_index = 0;
	} break;

	default:
		break;
	}
}

void filename_append(char c) {
	app.filename[app.filename_index] = c;
	app.filename_index++;
	app.filename[app.filename_index] = '\0';
}

void filename_delete(void) {
	if (app.filename_index == 0) {
		return;
	}
	app.filename_index--;
	app.filename[app.filename_index] = '\0';
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
	case GLFW_KEY_PERIOD:
		filename_append((char)(key + shift * 16));
		break;
	case GLFW_KEY_ENTER: {
		result_t res = file_manager_open(&app.buffer, app.filename);
		if (res != NO_ERROR) {
			return;
		}

		change_input_context(TEXT_INPUT_CONTEXT);
	} break;
	case GLFW_KEY_BACKSPACE:
		filename_delete();
		break;
	default:
		break;
	}
}

void text_input_callback(int key, int scancode, int action, int mods) {
	int shift = mods & GLFW_MOD_SHIFT;
	switch (key) {
	case GLFW_KEY_0:
		split_buffer_append(&app.buffer, (char)(key - shift * 7));
		break;
	case GLFW_KEY_1:
		split_buffer_append(&app.buffer, (char)(key - shift * 16));
		break;
	case GLFW_KEY_2:
		split_buffer_append(&app.buffer, (char)(key + shift * 14));
		break;
	case GLFW_KEY_3:
	case GLFW_KEY_4:
	case GLFW_KEY_5:
		split_buffer_append(&app.buffer, (char)(key - shift * 16));
		break;
	case GLFW_KEY_6:
		split_buffer_append(&app.buffer, (char)(key + shift * 40));
		break;
	case GLFW_KEY_7:
		split_buffer_append(&app.buffer, (char)(key - shift * 17));
		break;
	case GLFW_KEY_8:
		split_buffer_append(&app.buffer, (char)(key - shift * 14));
		break;
	case GLFW_KEY_9:
		split_buffer_append(&app.buffer, (char)(key - shift * 17));
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
		split_buffer_append(&app.buffer, (char)(key + (!shift) * 32));
		break;
	case GLFW_KEY_PERIOD:
		split_buffer_append(&app.buffer, (char)(key - shift * 16));
		break;
	case GLFW_KEY_ENTER:
		split_buffer_append(&app.buffer, '\n');
		break;
	case GLFW_KEY_TAB:
		split_buffer_append(&app.buffer, '\t');
		break;
	case GLFW_KEY_BACKSPACE:
		split_buffer_remove(&app.buffer);
		break;
	case GLFW_KEY_LEFT: {
		result_t res = split_buffer_move(&app.buffer, -1);
		if (res != NO_ERROR) {
			return;
		}
	} break;
	case GLFW_KEY_RIGHT: {
		result_t res = split_buffer_move(&app.buffer, 1);
		if (res != NO_ERROR) {
			return;
		}
	} break;
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
