#include "font.h"

#include <logger.h>
#include <math/matrix.h>

#include <GL/glew.h>
#include <stdlib.h>
#include <string.h>

// ivec2_t viewport = (ivec2_t){{800, 600}};

int font_load(font_t *font, const char *font_filepath, const char *string) {
	buffer_element_t position_element = {GL_FLOAT, 2, GL_FALSE};
	buffer_element_t coordinate_element = {GL_FLOAT, 2, GL_FALSE};
	buffer_element_t color_element = {GL_FLOAT, 4, GL_FALSE};
	buffer_layout_t layout = {0};
	buffer_layout_load(&layout, position_element);
	buffer_layout_load(&layout, coordinate_element);
	buffer_layout_load(&layout, color_element);
	render_object_create_vao(&font->object, &layout);

	render_object_load_font(
	    &font->object, font->characters, font_filepath, font->font_size);

	render_object_load_shaders(
	    &font->object, "res/shaders/font.vert", "res/shaders/font.frag");
	mat4_t projection = mat4_ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
	render_object_set_uniform_mat4(&font->object, "projection", projection.data);

	font_update(font, string);

	return 0;
}

void font_update(font_t *font, const char *string) {
	if (string == NULL) {
		font->object.vertices = 0;
		return;
	}
	debug("string: %s, %i", string, strlen(string));
	render_object_load_data(
	    &font->object, strlen(string) * 6 * 8 * sizeof(float), NULL);
	vec2_t current_position = font->position;
	long advance = font->characters[(int)' '].advance.x >> 6;

	for (int i = 0; string[i] != '\0'; ++i) {
		char_glyph_t character = font->characters[(int)string[i]];
		debug("current char: %c; current character advance: %ld", string[i],
		    character.advance.x >> 6);
		if (string[i] == '\n') {
			current_position.x = font->position.x;
			current_position.y += font->font_size;
		} else if (string[i] == '\t') {
			current_position.x += (advance)*2;
		}
		if (current_position.x + advance < font->position.x + font->size.x) {
			float temp_vertices[6][8] = {
			    {current_position.x + character.bearing.x,
			        current_position.y + character.size.y - character.bearing.y +
			            font->font_size,
			        character.start.x, character.end.y, font->color.r, font->color.g,
			        font->color.b, font->color.a},

			    {current_position.x + character.bearing.x + character.size.x,
			        current_position.y + character.size.y - character.bearing.y +
			            font->font_size,
			        character.end.x, character.end.y, font->color.r, font->color.g,
			        font->color.b, font->color.a},

			    {current_position.x + character.bearing.x,
			        current_position.y - character.bearing.y + font->font_size,
			        character.start.x, character.start.y, font->color.r,
			        font->color.g, font->color.b, font->color.a},

			    {current_position.x + character.bearing.x + character.size.x,
			        current_position.y - character.bearing.y + font->font_size,
			        character.end.x, character.start.y, font->color.r, font->color.g,
			        font->color.b, font->color.a},

			    {current_position.x + character.bearing.x,
			        current_position.y - character.bearing.y + font->font_size,
			        character.start.x, character.start.y, font->color.r,
			        font->color.g, font->color.b, font->color.a},

			    {current_position.x + character.bearing.x + character.size.x,
			        current_position.y + character.size.y - character.bearing.y +
			            font->font_size,
			        character.end.x, character.end.y, font->color.r, font->color.g,
			        font->color.b, font->color.a},

			};
			current_position.x += character.advance.x >> 6;
			debug("current_position %f, %f", current_position.x, current_position.y);
			render_object_load_sub_data(&font->object, sizeof(temp_vertices),
			    sizeof(temp_vertices) * i, temp_vertices);
		}
	}
}

void font_destroy(font_t *font) { render_object_delete(&font->object); }
