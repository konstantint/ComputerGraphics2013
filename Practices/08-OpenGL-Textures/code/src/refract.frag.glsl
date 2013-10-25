#version 120
varying vec3 normal;
varying vec4 vertexPos;
uniform sampler2D textureImage;  // The only sampler2D uniform is auto-bound to the only texture

// Gamma encoding
vec4 srgb(vec4 color) {
    return pow(color, vec4(1/2.2));
}

void main(void) {
    vec3 n = normalize(normal);
    // Modulate by the texture value
    vec4 c = texture2D(textureImage, gl_TexCoord[0].xy);
    // Encode & output
    gl_FragColor = srgb(c);
}
