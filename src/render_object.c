#include "render_object.h"
#define NDEBUG
#include "logger.h"

#include <ft2build.h>
#include FT_FREETYPE_H
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

void render_object_create_vao(
    render_object_t *object, buffer_layout_t *layout) {
	glGenVertexArrays(1, &object->vao);
	glBindVertexArray(object->vao);
	glGenBuffers(1, &object->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	intptr_t offset = 0;
	for (int i = 0; i < layout->num_elements; ++i) {
		glVertexAttribPointer(i, layout->elements[i].count,
		    layout->elements[i].type, layout->elements[i].normalized,
		    layout->stride, (void *)offset);
		glEnableVertexAttribArray(i);
		offset +=
		    layout->elements[i].count * get_type_size(layout->elements[i].type);
	}
}

void render_object_load_data(
    render_object_t *object, long size, const void *data) {
	trace("loading vertex buffer...");
	int stride = 0;
	glBindVertexArray(object->vao);
	glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride);
	object->vertices = size / stride;
	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

void render_object_load_sub_data(
    render_object_t *object, long size, long offset, const void *data) {
	trace("loading vertex sub buffer...");
	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void render_object_load_texture(
    render_object_t *object, const char *texture_filepath) {
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

int render_object_load_font(render_object_t *object, char_glyph_t *characters,
    const char *font_filepath, float font_size) {
	FT_Library lib;
	if (FT_Init_FreeType(&lib)) {
		error("failed to init freetype!");
		return -1;
	}

	FT_Face face;
	if (FT_New_Face(lib, font_filepath, 0, &face)) {
		FT_Done_FreeType(lib);
		error("failed to create font face!");
		return -1;
	}

	FT_Set_Pixel_Sizes(face, 0, font_size);
	uint32_t width = 0, height = 0;
	for (uint8_t c = ' '; c < '~'; ++c) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			error("failed to load character");
			continue;
		}
		if (face->glyph->bitmap.width > width) {
			width = face->glyph->bitmap.width;
		}
		if (face->glyph->bitmap.rows > height) {
			height = face->glyph->bitmap.rows;
		}
	}

	GLubyte *data = malloc(width * height * 100);
	memset(data, 0, width * height * 100);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &object->texture_id);
	glBindTexture(GL_TEXTURE_2D, object->texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width * 10, height * 10, 0, GL_RED,
	    GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	ivec2_t position = {{0}};
	for (uint8_t c = ' '; c <= '~'; ++c) {
		debug("loading character %c", c);
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			error("failed to load character");
			continue;
		}

		glTexSubImage2D(GL_TEXTURE_2D, 0, position.x, position.y,
		    face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED,
		    GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		char_glyph_t glyph;
		glyph.start = (vec2_t){{(float)position.x / ((float)width * 10),
		    (float)position.y / ((float)height * 10)}};
		glyph.end =
		    (vec2_t){{((float)position.x + (float)face->glyph->bitmap.width) /
		                  ((float)width * 10),
		        ((float)position.y + (float)face->glyph->bitmap.rows) /
		            ((float)height * 10)}};
		glyph.size =
		    (ivec2_t){{face->glyph->bitmap.width, face->glyph->bitmap.rows}};
		glyph.bearing =
		    (ivec2_t){{face->glyph->bitmap_left, face->glyph->bitmap_top}};
		glyph.advance = (lvec2_t){{face->glyph->advance.x, face->glyph->advance.y}};

		debug("character glyph start:(%f, %f), end:(%f, %f), size:(%i, %i), "
		      "bearing:(%i, %i)",
		    glyph.start.x, glyph.start.y, glyph.end.x, glyph.end.y, glyph.size.x,
		    glyph.size.y, glyph.bearing.x, glyph.bearing.y);

		characters[c] = glyph;
		if (position.x == width * 9) {
			position.y += height;
			position.x = 0;
		} else {
			position.x += width;
		}
	}

	FT_Done_Face(face);
	FT_Done_FreeType(lib);
	free(data);
	return 0;
}

int compile_shader(
    const char *filepath, int shader_type, uint32_t *shader_object) {
	debug("compiling shader: %s", filepath);
	FILE *file = fopen(filepath, "r");
	if (file == NULL) {
		return 1;
	}
	trace("shader file opened");
	fseek(file, 0, SEEK_END);
	long len = ftell(file);
	fseek(file, 0, SEEK_SET);
	char *shader_code = malloc(len + 1);
	if (shader_code == NULL) {
		return 1;
	}
	size_t bytes_read = fread(shader_code, 1, len, file);
	if (bytes_read != (size_t)len) {
		free(shader_code);
		fclose(file);
		return 1;
	}
	shader_code[len] = '\0';
	trace("shader code copied");

	int success;
	char info_log[512];
	*shader_object = glCreateShader(shader_type);
	glShaderSource(*shader_object, 1, (const char *const *)&shader_code, NULL);
	glCompileShader(*shader_object);
	glGetShaderiv(*shader_object, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(*shader_object, 512, NULL, info_log);
		error("Failed to compile vertex shader");
		debug("shader error log: %s", info_log);
		return 1;
	}

	free(shader_code);
	fclose(file);
	debug("%s compiled", filepath);
	return 0;
}

void render_object_load_shaders(render_object_t *object,
    const char *vertex_shader_filepath, const char *fragment_shader_filepath) {
	trace("loading shaders...");
	uint32_t vert_shader;
	if (compile_shader(vertex_shader_filepath, GL_VERTEX_SHADER, &vert_shader)) {
		return;
	}
	uint32_t frag_shader;
	if (compile_shader(
	        fragment_shader_filepath, GL_FRAGMENT_SHADER, &frag_shader)) {
		return;
	}
	int success;
	char info_log[512];
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

void render_object_set_uniform_mat4(
    render_object_t *object, const char *uniform_name, float *mat4) {
	glUseProgram(object->shader_id);
	GLint location = glGetUniformLocation(object->shader_id, uniform_name);
	glUniformMatrix4fv(location, 1, GL_FALSE, mat4);
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
	if (glIsTexture(object->texture_id) == GL_TRUE) {
		glBindTexture(GL_TEXTURE_2D, object->texture_id);
		glDeleteTextures(1, &object->texture_id);
	}
}

char *read_file(FILE *file) {
	fseek(file, 0, SEEK_END);
	long len = ftell(file);
	fseek(file, 0, SEEK_SET);
	char *buffer = malloc(len + 1);
	if (buffer == NULL) {
	}

	size_t bytes_read = fread(buffer, 1, len, file);
	if (bytes_read != (size_t)len) {
		error("failed to read file contents!");
		free(buffer);
		return NULL;
	}
	buffer[len] = '\0';

	return buffer;
}
