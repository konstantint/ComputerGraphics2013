#version 140

smooth out vec4 glVertex;
smooth out vec4 position;
smooth out vec3 normal;
smooth out vec4 color;

void main(void) {
    glVertex = gl_Vertex;
    position = gl_ModelViewMatrix * gl_Vertex;
    normal = gl_NormalMatrix * gl_Normal;
    color = gl_Color;
    gl_Position = ftransform();
}

