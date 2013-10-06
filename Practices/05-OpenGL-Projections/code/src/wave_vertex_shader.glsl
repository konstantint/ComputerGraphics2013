#version 130
out vec4 vertex_color;
uniform float time;

void main(void) {
    // Your code below
    gl_Position = ftransform();

    // Leave this as is
    vertex_color = gl_Color;
}
