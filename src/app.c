#include "app.h"

#include "file_manager.h"
#include "logger.h"
#include "split_buffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

static GLFWwindow *window = NULL;
static split_buffer_t buffer;

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods);

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
    return;
  }

  window = glfwCreateWindow(800, 600, "text-editor", NULL, NULL);
  if (!window) {
    fatal("failed to create window!");
    app_shutdown(app);
    return;
  }
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);

  if (glewInit() != GLEW_OK) {
    fatal("failed to initialize GLEW!");
    app_shutdown(app);
  }

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

  split_buffer_create(&buffer, "Hello,_World!");

  result_t res = file_manager_open("test2.txt");

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  file_manager_close();
  return res;
}

void buffer_test() {
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

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
    case GLFW_KEY_A:
      split_buffer_append(&buffer, 'a');
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
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, GLFW_TRUE);
      break;

    default:
      break;
    }
    split_buffer_print(&buffer);
  }
}