#version 140

in vec4 position;
in vec3 normal;

const vec3 l = vec3(1, 0, 0);
const vec3 v = vec3(0, 0, 1);
const float ambient = 0.02;
void main(void) {
    vec3 n = normalize(normal);
    vec3 h = l + v;
    h = h/2;
    h = normalize(h);

    float lambert = clamp(dot(n, l), 0, 1);
    vec3 r = n*2*dot(n, l) - l;
    float blinn = clamp(dot(h, n), 0, 1);
    blinn = pow(blinn, 5);

    float c = ambient + lambert + blinn;
    c = c/1.9;
    c = pow(c, 1.0/2.2);

    gl_FragColor = vec4(c, c, c, 1);
}
