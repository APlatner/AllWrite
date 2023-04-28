#version 330 core

in vec4 texture_color;
in vec2 tex_coords;

out vec4 frag_color;

uniform sampler2D tex;

void main() {
    frag_color = texture(tex, tex_coords) * texture_color;
}