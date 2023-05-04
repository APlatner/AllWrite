#include "render_object.h"
#include "logger.h"

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
	shader_code[len] = '\0'; // forgot to null terminate the string
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
