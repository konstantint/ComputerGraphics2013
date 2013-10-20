#version 120
varying vec3 normal;
varying vec4 vertex_pos;
varying vec4 vertex_color_if_lighting_disabled;
uniform int disable_lighting;

void main(void) {
    if (disable_lighting == 1) {
        gl_FragColor = vertex_color_if_lighting_disabled;
        return;
    }

    // This is the fragment shader. It's task is to output gl_FragColor - the color of the current fragment.
    // In this exercise you have to replicate the lighting computations you did
    // in the vertex shader in the last exercise here.

    gl_FragColor = vec4(0, 0, 0, 0);
}
