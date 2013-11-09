#version 330

uniform mat4 modelViewMatrix;

// Here we give a name to attribute 0
// from our vertex attribute array.
layout(location = 0) in vec2 position; // For the sake of doing something unusual, we'll pass in 2d coordinates into the shader

void main(void) {
    gl_Position = modelViewMatrix*vec4(position, 0, 1);
}
