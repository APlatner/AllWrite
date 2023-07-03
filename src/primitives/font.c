#include "font.h"

#include <logger.h>

#include <GL/glew.h>
#include <stdlib.h>
#include <string.h>

// ivec2_t viewport = (ivec2_t){{800, 600}};

int font_load(font_t *font, long cursor_position, const char *font_filepath,
    const char *string, mat4_t projection) {
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
	render_object_set_uniform_mat4(&font->object, "projection", projection.data);

	font_update(font, cursor_position, string, 0.0f);

	return 0;
}

void font_update(font_t *font, long cursor_position, const char *string,
    float vertical_offset) {
	if (string == NULL) {
		font->object.vertices = 0;
		return;
	}
	int cursor = 0;
	if (cursor_position >= 0) {
		cursor = 1;
	}

	// debug("string: %s, %i", string, strlen(string));
	render_object_load_data(
	    &font->object, (strlen(string) + cursor) * 48 * sizeof(float), NULL);
	vec2_t current_position =
	    (vec2_t){{font->position.x, font->position.y + vertical_offset}};
	long advance = font->characters[(int)' '].advance.x >> 6;

	for (int i = 0; i < strlen(string); ++i) {
		char_glyph_t character = font->characters[(int)string[i]];
		// debug("current char: %c; current character advance: %ld", string[i],
		// character.advance.x >> 6);
		if (i == cursor_position && cursor) {
			char_glyph_t cursor = font->characters[(int)'|'];
			float temp_vertices[6][8] = {
			    {current_position.x, current_position.y + font->font_size,
			        cursor.start.x, cursor.end.y, font->color.r, font->color.g,
			        font->color.b, font->color.a},

			    {current_position.x + 4, current_position.y + font->font_size,
			        cursor.end.x, cursor.end.y, font->color.r, font->color.g,
			        font->color.b, font->color.a},

			    {current_position.x, current_position.y, cursor.start.x,
			        cursor.start.y, font->color.r, font->color.g, font->color.b,
			        font->color.a},

			    {current_position.x + 4, current_position.y, cursor.end.x,
			        cursor.start.y, font->color.r, font->color.g, font->color.b,
			        font->color.a},

			    {current_position.x, current_position.y, cursor.start.x,
			        cursor.start.y, font->color.r, font->color.g, font->color.b,
			        font->color.a},

			    {current_position.x + 4, current_position.y + font->font_size,
			        cursor.end.x, cursor.end.y, font->color.r, font->color.g,
			        font->color.b, font->color.a},
			};
			// debug("current_position %f, %f", current_position.x,
			// current_position.y);
			render_object_load_sub_data(
			    &font->object, sizeof(temp_vertices), 0, temp_vertices);
		}
		if (string[i] == '\n') {
			current_position.x = font->position.x;
			current_position.y += font->font_size;
		} else if (string[i] == '\t') {
			current_position.x += (advance)*2;
		}
		if (current_position.x + advance < font->position.x + font->size.x &&
		    current_position.y + font->font_size <
		        font->position.y + font->size.y) {
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
			// debug("current_position %f, %f", current_position.x,
			// current_position.y);
			render_object_load_sub_data(&font->object, sizeof(temp_vertices),
			    sizeof(temp_vertices) * (i + cursor), temp_vertices);
		} else {
			float temp_vertices[6][8] = {
			    {current_position.x + character.bearing.x,
			        current_position.y + character.size.y - character.bearing.y +
			            font->font_size,
			        0.0f, 0.0f, font->color.r, font->color.g, font->color.b,
			        font->color.a},

			    {current_position.x + character.bearing.x + character.size.x,
			        current_position.y + character.size.y - character.bearing.y +
			            font->font_size,
			        0.0f, 0.0f, font->color.r, font->color.g, font->color.b,
			        font->color.a},

			    {current_position.x + character.bearing.x,
			        current_position.y - character.bearing.y + font->font_size, 0.0f,
			        0.0f, font->color.r, font->color.g, font->color.b, font->color.a},

			    {current_position.x + character.bearing.x + character.size.x,
			        current_position.y - character.bearing.y + font->font_size, 0.0f,
			        0.0f, font->color.r, font->color.g, font->color.b, font->color.a},

			    {current_position.x + character.bearing.x,
			        current_position.y - character.bearing.y + font->font_size, 0.0f,
			        0.0f, font->color.r, font->color.g, font->color.b, font->color.a},

			    {current_position.x + character.bearing.x + character.size.x,
			        current_position.y + character.size.y - character.bearing.y +
			            font->font_size,
			        0.0f, 0.0f, font->color.r, font->color.g, font->color.b,
			        font->color.a},

			};
			current_position.x += character.advance.x >> 6;
			// debug("current_position %f, %f", current_position.x,
			// current_position.y);
			render_object_load_sub_data(&font->object, sizeof(temp_vertices),
			    sizeof(temp_vertices) * (i + cursor), temp_vertices);
		}
	}

	if (strlen(string) == cursor_position && cursor) {
		char_glyph_t cursor = font->characters[(int)'|'];
		float temp_vertices[6][8] = {
		    {current_position.x, current_position.y + font->font_size,
		        cursor.start.x, cursor.end.y, font->color.r, font->color.g,
		        font->color.b, font->color.a},

		    {current_position.x + 4, current_position.y + font->font_size,
		        cursor.end.x, cursor.end.y, font->color.r, font->color.g,
		        font->color.b, font->color.a},

		    {current_position.x, current_position.y, cursor.start.x, cursor.start.y,
		        font->color.r, font->color.g, font->color.b, font->color.a},

		    {current_position.x + 4, current_position.y, cursor.end.x,
		        cursor.start.y, font->color.r, font->color.g, font->color.b,
		        font->color.a},

		    {current_position.x, current_position.y, cursor.start.x, cursor.start.y,
		        font->color.r, font->color.g, font->color.b, font->color.a},

		    {current_position.x + 4, current_position.y + font->font_size,
		        cursor.end.x, cursor.end.y, font->color.r, font->color.g,
		        font->color.b, font->color.a},
		};
		// debug("current_position %f, %f", current_position.x, current_position.y);
		render_object_load_sub_data(
		    &font->object, sizeof(temp_vertices), 0, temp_vertices);
	}
}

void font_destroy(font_t *font) { render_object_delete(&font->object); }
