#version 120
varying vec3 normal;
varying vec4 vertexPos;
varying vec4 color;

void main(void) {
    gl_Position = ftransform();
    normal = gl_NormalMatrix * gl_Normal;
    vertexPos = gl_ModelViewMatrix * gl_Vertex;
    color = gl_Color;
}
