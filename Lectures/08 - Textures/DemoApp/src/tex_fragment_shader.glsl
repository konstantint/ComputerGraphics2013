#version 140

noperspective in vec2 texCoord; // gl_TexCoord[0], but without perspective correction
uniform sampler2D myTexture;
uniform int mode;
uniform int gamma;

void main (void) {
    vec4 col;
    if (mode == 0) {
        col  = texture2D(myTexture, texCoord);
    }
    else {
        col  = texture2D(myTexture, vec2(gl_TexCoord[0]));
    }
    if (gamma == 1) {
        col = pow(col, vec4(1/2.2));
    }
    gl_FragColor = col;
}
