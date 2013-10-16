#version 140

in vec4 position;
in vec3 normal;

const vec3 l = vec3(1, 0, 0);
const vec3 v = vec3(0, 0, 1);
const float ambient = 0.02;

void main(void) {
    vec3 n = normalize(normal);
    float lambert = clamp(dot(n, l), 0, 1);
    vec3 r = n*2*dot(n, l) - l;
    float phong = clamp(dot(r, v), 0, 1);
    phong = pow(phong, 10);

    float c = ambient + lambert + phong;

    gl_FragColor = vec4(c, c, c, 1);
}
