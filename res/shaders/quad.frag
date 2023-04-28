#version 330 core

in vec4 quad_color;

out vec4 frag_color;

void main() {
    frag_color = vec4(quad_color.x, quad_color.y, quad_color.z, quad_color.w);
}
