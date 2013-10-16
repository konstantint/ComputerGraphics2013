#version 140

smooth out vec4 position;
smooth out vec3 normal;

void main(void) {
  position = gl_ModelViewMatrix * gl_Vertex;
  normal = gl_NormalMatrix * gl_Normal;
  gl_Position = ftransform();
}

