#version 330

// This is the output variable of the fragment shader
// We configure this to be "output 0" for our fragment shader
// in the main program using glBindFragDataLocation
out vec4 fragColor;

void main(void) {
    fragColor = vec4(1, 0, 0, 1);
}
