#include "render_object.h"
#include "logger.h"

#include <GL/glew.h>
#include <stdio.h>

int get_type_size(int type) {
  switch (type) {
  case GL_FLOAT:
    return sizeof(GLfloat);
    break;
  case GL_UNSIGNED_INT:
    return sizeof(GLuint);
    break;
  case GL_UNSIGNED_BYTE:
    return sizeof(GLubyte);
    break;
  default:
    return 0;
    break;
  }
}

void buffer_layout_create(buffer_layout_t *layout) {
  layout->num_elements = 0;
  layout->stride = 0;
}

void buffer_layout_load(buffer_layout_t *layout, buffer_element_t element) {
  debug("Element count: %d, Element type: %d", element.count,
        get_type_size(element.type));
  layout->elements[layout->num_elements] = element;
  layout->num_elements += 1;
  layout->stride += element.count * get_type_size(element.type);
  debug("Layout stride: %d", layout->stride);
}

void render_object_create_vao(render_object_t *object,
                              buffer_layout_t *layout) {
  glGenVertexArrays(1, &object->vao);
  glBindVertexArray(object->vao);
  glCreateBuffers(1, &object->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
  intptr_t offset = 0;
  for (int i = 0; i < layout->num_elements; ++i) {
    glVertexAttribPointer(
        i, layout->elements[i].count, layout->elements[i].type,
        layout->elements[i].normalized, layout->stride, (void *)offset);
    glEnableVertexAttribArray(i);
    offset +=
        layout->elements[i].count * get_type_size(layout->elements[i].type);
  }
}

void render_object_load_data(render_object_t *object, long size,
                             const void *data) {
  int stride = 0;
  glBindVertexArray(object->vao);
  glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride);
  object->vertices = size / stride;
  glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

void render_object_load_shaders(render_object_t *object,
                                const char *vertex_shader_filepath,
                                const char *fragment_shader_filepath) {
  uint32_t vert_shader;
  char *vert_shader_code = "\
#version 330 core\n\
\n\
layout(location = 0) in vec2 position;\n\
\n\
void main() { gl_Position = vec4(position.x, position.y, 0.0, 1.0); }\n\
\0";
  int success;
  char info_log[512];
  vert_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert_shader, 1, (const char *const *)&vert_shader_code, NULL);
  glCompileShader(vert_shader);
  glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vert_shader, 512, NULL, info_log);
    error("Failed to compile vertex shader");
    debug("shader error log: %s", info_log);
    return;
  }

  uint32_t frag_shader;
  char *frag_shader_code = "\
#version 330 core\n\
\n\
out vec4 frag_color;\n\
\n\
void main() { frag_color = vec4(0.8, 0.8, 0.8, 1.0); }\n\
\0";

  frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag_shader, 1, (const char *const *)&frag_shader_code, NULL);
  glCompileShader(frag_shader);
  glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(frag_shader, 512, NULL, info_log);
    error("Failed to compile fragment shader");
    debug("shader error log: %s", info_log);
    return;
  }

  object->shader_id = glCreateProgram();
  glAttachShader(object->shader_id, vert_shader);
  glAttachShader(object->shader_id, frag_shader);
  glLinkProgram(object->shader_id);

  glGetProgramiv(object->shader_id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(object->shader_id, 512, NULL, info_log);
    error("Failed to link shaders");
    debug("shader error log: %s", info_log);
    return;
  }

  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);
  return;
}

void render_object_draw(render_object_t *object) {
  glUseProgram(object->shader_id);
  glBindVertexArray(object->vao);
  glDrawArrays(GL_TRIANGLES, 0, object->vertices);
}

void render_object_delete(render_object_t *object) {
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &object->vao);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &object->vbo);
}
