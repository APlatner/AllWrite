#include "font.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glew.h>
#include <string.h>

int font_load(font_t *font, const char *font_filepath, const char *string) {
  FT_Library lib;
  if (FT_Init_FreeType(&lib)) {
    printf("failed to init freetype!\n");
    return -1;
  }

  FT_Face face;
  if (FT_New_Face(lib, font_filepath, 0, &face)) {
    printf("failed to create font face!\n");
    return -1;
  }

  FT_Set_Pixel_Sizes(face, 0, 48);
  uint32_t width = 0, height = 0;
  for (uint8_t c = 32; c < 128; ++c) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      printf("failed to load %c", c);
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
  glGenTextures(1, &font->object.texture_id);
  glBindTexture(GL_TEXTURE_2D, font->object.texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width * 10, height * 10, 0, GL_RED,
               GL_UNSIGNED_BYTE, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  for (uint8_t c = 32; c < 128; ++c) {
    vec2_t position = {{0.0}};
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      printf("failed to load %c", c);
      continue;
    }

    glTexSubImage2D(GL_TEXTURE_2D, 0, position.x, position.y,
                    face->glyph->bitmap.width, face->glyph->bitmap.rows, 0,
                    GL_RED, face->glyph->bitmap.buffer);

    font->characters[c] =
        (char_glyph_t){position,
                       {{face->glyph->bitmap.width, face->glyph->bitmap.rows}},
                       {{face->glyph->bitmap_left, face->glyph->bitmap_top}},
                       {{face->glyph->advance.x}}};
    if (position.x == width * 9) {
      position.y += height;
      position.x = 0;
    } else {
      position.x += width;
    }
  }

  free(data);

  size_t length = strlen(string);
  font->string = malloc(length + 1);
  if (font->string == NULL) {
    return 1;
  }
  strcpy(font->string, string);

  return 0;
}

void font_update(font_t *font) {}

void font_destroy(font_t *font) { render_object_delete(&font->object); }
