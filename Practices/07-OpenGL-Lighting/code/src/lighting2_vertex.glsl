#version 120
varying vec3 normal;
varying vec4 vertex_pos;
varying vec4 vertex_color_if_lighting_disabled;

void main(void) {
    gl_Position = ftransform();
    vertex_color_if_lighting_disabled = gl_Color;

    // Send the normal and vertex positions down to the fragment shader
    normal = gl_NormalMatrix * gl_Normal;
    vertex_pos = gl_ModelViewMatrix * gl_Vertex;
}
