#version 330 core

in vec4 font_color;
in vec2 tex_coords;

out vec4 frag_color;

uniform sampler2D tex;

void main() {
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(tex, tex_coords).r);
    frag_color = font_color * sampled;
}