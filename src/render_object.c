#include "render_object.h"
#include "logger.h"
#include "util.h"

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
  trace("loading vertex buffer...");
  int stride = 0;
  glBindVertexArray(object->vao);
  glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride);
  object->vertices = size / stride;
  glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

void render_object_load_texture(render_object_t *object,
                                const char *texture_filepath) {
  glGenTextures(1, &object->texture_id);
  glBindTexture(GL_TEXTURE_2D, object->texture_id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width = 0, height = 0, channels = 0;
  uint8_t *data =
      stbi_load(texture_filepath, &width, &height, &channels, STBI_rgb_alpha);
  if (!data) {
    error("failed to load texture!\n");
    stbi_image_free(data);
    return;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
}

void render_object_load_shaders(render_object_t *object,
                                const char *vertex_shader_filepath,
                                const char *fragment_shader_filepath) {
  trace("loading shaders...");
  uint32_t vert_shader;
  FILE *vertex_file = fopen(vertex_shader_filepath, "r");
  char *vert_shader_code;
  if (vertex_file == NULL) {
    debug("filename: %s", vertex_shader_filepath);
    error("failed to load vertex shader!");
    vert_shader_code = "#version 330 core in vec2 position; in vec4 color; out "
                       "vec4 quadColor; void main() { quadColor = color; "
                       "gl_Position = vec4(position.x, position.y, 0.0, 1.0);}";
  } else {
    vert_shader_code = read_file(vertex_file);
  }
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
  FILE *fragment_file = fopen(fragment_shader_filepath, "r");
  char *frag_shader_code;
  if (fragment_file == NULL) {
    error("failed to load fragment shader!");
    debug("filename: %s", fragment_shader_filepath);
    frag_shader_code = "#version 330 core out vec4 fragColor; in vec4 "
                       "quadColor; void main() { fragColor = vec4(quadColor.x, "
                       "quadColor.y, quadColor.z, quadColor.w);}";
  } else {
    frag_shader_code = read_file(fragment_file);
  }

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
  if (glIsTexture(object->texture_id) == GL_TRUE) {
    glBindTexture(GL_TEXTURE_2D, object->texture_id);
  }
  glDrawArrays(GL_TRIANGLES, 0, object->vertices);
}

void render_object_delete(render_object_t *object) {
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &object->vao);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &object->vbo);
}
