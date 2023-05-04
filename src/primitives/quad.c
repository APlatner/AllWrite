#include "quad.h"

#include <GL/glew.h>
#include <math/matrix.h>

void quad_load(quad_t *quad) {
  buffer_element_t position_element = {GL_FLOAT, 2, GL_FALSE};
  buffer_element_t color_element = {GL_FLOAT, 4, GL_FALSE};
  buffer_layout_t layout = {0};
  buffer_layout_load(&layout, position_element);
  buffer_layout_load(&layout, color_element);

  render_object_create_vao(&quad->object, &layout);

  float vertices[6][6] = {
      {quad->position.x, quad->position.y, quad->color.r, quad->color.g,
       quad->color.b, quad->color.a},

      {quad->position.x + quad->size.x, quad->position.y + quad->size.y,
       quad->color.r, quad->color.g, quad->color.b, quad->color.a},

      {quad->position.x, quad->position.y + quad->size.y, quad->color.r,
       quad->color.g, quad->color.b, quad->color.a},

      {quad->position.x + quad->size.x, quad->position.y, quad->color.r,
       quad->color.g, quad->color.b, quad->color.a},

      {quad->position.x + quad->size.x, quad->position.y + quad->size.y,
       quad->color.r, quad->color.g, quad->color.b, quad->color.a},

      {quad->position.x, quad->position.y, quad->color.r, quad->color.g,
       quad->color.b, quad->color.a},
  };

  render_object_load_data(&quad->object, sizeof(vertices), vertices);
  render_object_load_shaders(&quad->object, "res/shaders/quad.vert",
                             "res/shaders/quad.frag");

  mat4_t projection = mat4_ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
  render_object_set_uniform_mat4(&quad->object, "projection", projection.data);
}

void quad_update(quad_t *quad) {
  float vertices[6][6] = {
      {quad->position.x, quad->position.y + quad->size.y, quad->color.r,
       quad->color.g, quad->color.b, quad->color.a},

      {quad->position.x, quad->position.y, quad->color.r, quad->color.g,
       quad->color.b, quad->color.a},

      {quad->position.x + quad->size.x, quad->position.y + quad->size.y,
       quad->color.r, quad->color.g, quad->color.b, quad->color.a},

      {quad->position.x + quad->size.x, quad->position.y, quad->color.r,
       quad->color.g, quad->color.b, quad->color.a},

      {quad->position.x + quad->size.x, quad->position.y + quad->size.y,
       quad->color.r, quad->color.g, quad->color.b, quad->color.a},

      {quad->position.x, quad->position.y, quad->color.r, quad->color.g,
       quad->color.b, quad->color.a},
  };

  render_object_load_data(&quad->object, sizeof(vertices), vertices);
}

void quad_destroy(quad_t *quad) { render_object_delete(&quad->object); }
