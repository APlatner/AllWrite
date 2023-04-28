#include "app.h"

#include "file_manager.h"
#include "logger.h"
#include "split_buffer.h"
#include "widgets/quad.h"
#include "widgets/texture.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

enum input_context_t {

  CONTROL_INPUT_CONTEXT = 0,
  TEXT_INPUT_CONTEXT,
  FILE_INPUT_CONTEXT,
};

static GLFWwindow *window = NULL;
static split_buffer_t buffer;
static int input_context = FILE_INPUT_CONTEXT;

void renderer_debug_callback(uint32_t source, uint32_t type, uint32_t id,
                             uint32_t severity, int32_t length,
                             const char *message, const void *user_param);
void change_input_context(int new_context);
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods);
void control_input_callback(int key, int scancode, int action, int mods);
void text_input_callback(int key, int scancode, int action, int mods);

result_t app_startup(app_t *app) {
  trace("app starting...");
  // rename text-editor-log.txt to text-editor.log
  result_t res = logger_startup("text-editor.log");
  if (res != NO_ERROR) {
    return res;
  }

  res = file_manager_startup();
  if (res != NO_ERROR) {
    app_shutdown(app);
    return res;
  }

  if (!glfwInit()) {
    fatal("failed to initialize GLFW!");
    app_shutdown(app);
    return WINDOWING_ERROR;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  window = glfwCreateWindow(800, 600, "text-editor", NULL, NULL);
  if (!window) {
    fatal("failed to create window!");
    app_shutdown(app);
    return WINDOWING_ERROR;
  }
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);

  if (glewInit() != GLEW_OK) {
    fatal("failed to initialize GLEW!");
    app_shutdown(app);
    return OPENGL_ERROR;
  }

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(renderer_debug_callback, window);

  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glActiveTexture(GL_TEXTURE0);

  return NO_ERROR;
}

void app_shutdown(app_t *app) {
  info("app shutting.");
  glfwTerminate();
  file_manager_shutdown();
  logger_shutdown();
}

result_t app_run(app_t *app) {
  info("app running");

  quad_t quad;
  quad.position = (vec2_t){{-0.5, -0.5}};
  quad.size = (vec2_t){{1.0, 1.0}};
  quad.color = (vec4_t){{0.5f, 0.8f, 0.9f, 1.0f}};
  quad_load(&quad);
  texture_t texture;
  texture.position = (vec2_t){{-0.5, -0.5}};
  texture.size = (vec2_t){{1.0, 1.0}};
  texture.color = (vec4_t){{0.0f, 0.8f, 0.9f, 1.0f}};
  texture_load(&texture);

  while (!glfwWindowShouldClose(window)) {
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
      quad.color = (vec4_t){{0.8f, 0.8f, 0.8f, 1.0f}};
      quad_update(&quad);
    }
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    render_object_draw(&texture.object);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // file_manager_close();
  return NO_ERROR;
}

void buffer_test(void) {
  split_buffer_t buffer;
  split_buffer_create(&buffer, "Hello,_World!");
  split_buffer_print(&buffer);
  split_buffer_move(&buffer, -4);
  split_buffer_print(&buffer);
  split_buffer_append(&buffer, 'c');
  split_buffer_print(&buffer);
  split_buffer_move(&buffer, 2);
  split_buffer_print(&buffer);
  split_buffer_remove(&buffer);
  split_buffer_print(&buffer);
}

void change_input_context(int new_context) {
  const char *context_strings[] = {
      "Control Context",
      "Text Context",
      "File Context",
  };

  debug("switching context from %s to %s", context_strings[input_context],
        context_strings[new_context]);
  // printf("switching context from %s to %s\n", context_strings[input_context],
  //        context_strings[new_context]);
  input_context = new_context;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
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

    switch (input_context) {
    case CONTROL_INPUT_CONTEXT:
      control_input_callback(key, scancode, action, mods);
      break;
    case TEXT_INPUT_CONTEXT:
      text_input_callback(key, scancode, action, mods);
    }
  }
}

void control_input_callback(int key, int scancode, int action, int mods) {
  result_t res;
  switch (key) {
  case GLFW_KEY_S:
    // TODO: save file
    res = file_manager_save(&buffer);
    if (res != NO_ERROR) {
      return;
    }
    change_input_context(TEXT_INPUT_CONTEXT);
    break;
  case GLFW_KEY_Q:
    // TODO: close file
    file_manager_close();
    split_buffer_destroy(&buffer);
    change_input_context(FILE_INPUT_CONTEXT);
    break;
  case GLFW_KEY_O:
    // TODO: open file
    res = file_manager_open(&buffer, "test2.txt");
    if (res != NO_ERROR) {
      return;
    }
    change_input_context(TEXT_INPUT_CONTEXT);
    break;

  default:
    break;
  }
}

void text_input_callback(int key, int scancode, int action, int mods) {
  switch (key) {
  case GLFW_KEY_0:
  case GLFW_KEY_1:
  case GLFW_KEY_2:
  case GLFW_KEY_3:
  case GLFW_KEY_4:
  case GLFW_KEY_5:
  case GLFW_KEY_6:
  case GLFW_KEY_7:
  case GLFW_KEY_8:
  case GLFW_KEY_9:
    split_buffer_append(&buffer, (char)key);
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
    if (mods & GLFW_MOD_SHIFT) {
      split_buffer_append(&buffer, (char)key);
    } else {
      split_buffer_append(&buffer, (char)(key + 32));
    }
    break;
  case GLFW_KEY_BACKSPACE:
    split_buffer_remove(&buffer);
    break;
  case GLFW_KEY_LEFT:
    split_buffer_move(&buffer, -1);
    break;
  case GLFW_KEY_RIGHT:
    split_buffer_move(&buffer, 1);
    break;
  default:
    break;
  }
}

void renderer_debug_callback(uint32_t source, uint32_t type, uint32_t id,
                             uint32_t severity, int32_t length,
                             const char *message, const void *user_param) {
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
