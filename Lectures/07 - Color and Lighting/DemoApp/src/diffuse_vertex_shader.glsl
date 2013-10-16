#version 140

smooth out vec4 vertex_color;
const vec3 light_dir = vec3(1, 0, 0);

void main(void) {
  float c = clamp(dot(gl_Normal, light_dir), 0, 1);
  c = pow(c, 1.0/2.2);
  vertex_color = vec4(c, c, c, 1);
  gl_Position = ftransform();
}

