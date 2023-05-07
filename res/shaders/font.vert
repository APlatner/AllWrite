#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texture_coordinates;
layout (location = 2) in vec4 color;

out vec2 tex_coords;
out vec4 font_color;

uniform mat4 projection;

void main() {
    tex_coords = texture_coordinates;
    font_color = color;
    gl_Position = vec4(position.x, position.y, 0.0, 1.0) * projection;
}