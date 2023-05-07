#include "texture.h"

#include <math/matrix.h>

#include <GL/glew.h>

void texture_load(texture_t *texture) {
	buffer_element_t position_element = {GL_FLOAT, 2, GL_FALSE};
	buffer_element_t coordinate_element = {GL_FLOAT, 2, GL_FALSE};
	buffer_element_t color_element = {GL_FLOAT, 4, GL_FALSE};
	buffer_layout_t layout = {0};
	buffer_layout_load(&layout, position_element);
	buffer_layout_load(&layout, coordinate_element);
	buffer_layout_load(&layout, color_element);

	render_object_create_vao(&texture->object, &layout);

	float vertices[6][8] = {
	    // upper left
	    {texture->position.x, texture->position.y, 0.0f, 0.0f, texture->color.r,
	        texture->color.g, texture->color.b, texture->color.a},
	    // lower right
	    {texture->position.x + texture->size.x,
	        texture->position.y + texture->size.y, 1.0f, 1.0f, texture->color.r,
	        texture->color.g, texture->color.b, texture->color.a},
	    // upper right
	    {texture->position.x + texture->size.x, texture->position.y, 1.0f, 0.0f,
	        texture->color.r, texture->color.g, texture->color.b,
	        texture->color.a},
	    // upper left
	    {texture->position.x, texture->position.y, 0.0f, 0.0f, texture->color.r,
	        texture->color.g, texture->color.b, texture->color.a},
	    // lower left
	    {texture->position.x, texture->position.y + texture->size.y, 0.0f, 1.0f,
	        texture->color.r, texture->color.g, texture->color.b,
	        texture->color.a},
	    // lower right
	    {texture->position.x + texture->size.x,
	        texture->position.y + texture->size.y, 1.0f, 1.0f, texture->color.r,
	        texture->color.g, texture->color.b, texture->color.a},

	};

	render_object_load_data(&texture->object, sizeof(vertices), vertices);
	render_object_load_texture(
	    &texture->object, "res/textures/SentientCloudLogo.png");
	render_object_load_shaders(
	    &texture->object, "res/shaders/texture.vert", "res/shaders/texture.frag");
	mat4_t projection = mat4_ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
	render_object_set_uniform_mat4(
	    &texture->object, "projection", projection.data);
}

void texture_update(texture_t *texture) {
	float vertices[6][8] = {

	    {texture->position.x, texture->position.y, 0.0f, 1.0f, texture->color.r,
	        texture->color.g, texture->color.b, texture->color.a},

	    {texture->position.x, texture->position.y + texture->size.y, 0.0f, 0.0f,
	        texture->color.r, texture->color.g, texture->color.b,
	        texture->color.a},

	    {texture->position.x + texture->size.x,
	        texture->position.y + texture->size.y, 1.0f, 0.0f, texture->color.r,
	        texture->color.g, texture->color.b, texture->color.a},

	    {texture->position.x + texture->size.x, texture->position.y, 1.0f, 1.0f,
	        texture->color.r, texture->color.g, texture->color.b,
	        texture->color.a},

	    {texture->position.x, texture->position.y, 0.0f, 1.0f, texture->color.r,
	        texture->color.g, texture->color.b, texture->color.a},

	    {texture->position.x + texture->size.x,
	        texture->position.y + texture->size.y, 1.0f, 0.0f, texture->color.r,
	        texture->color.g, texture->color.b, texture->color.a},
	};

	render_object_load_data(&texture->object, sizeof(vertices), vertices);
}

void texture_destroy(texture_t *texture) {
	render_object_delete(&texture->object);
}
