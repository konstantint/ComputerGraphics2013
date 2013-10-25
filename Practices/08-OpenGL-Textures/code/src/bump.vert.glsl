#version 120
varying vec3 normal;
varying vec4 vertexPos;

void main(void) {
    gl_Position = ftransform();

    // Send the normal and vertex positions down to the fragment shader
    normal = gl_NormalMatrix * gl_Normal;
    vertexPos = gl_ModelViewMatrix * gl_Vertex;

    // Send texture coordinates to the fragment shader
    gl_TexCoord[0] = gl_Vertex;
}
