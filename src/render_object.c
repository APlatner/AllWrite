#include "render_object.h"

#include <GL/glew.h>

void render_object_create_vao(render_object_t *object, buffer_layout_t layout) {
  glGenVertexArrays(1, &object->vao);
  glBindVertexArray(object->vao);
  glCreateBuffers(1, &object->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
  intptr_t offset = 0;
  for (int i = 0; i < layout.num_elements; ++i) {
    glVertexAttribPointer(i, layout.elements[i].count, layout.elements[i].type,
                          GL_FALSE, layout.stride, (void *)offset);
    glEnableVertexAttribArray(i);
    offset += layout.elements[i].count * layout.elements[i].type;
  }
}
