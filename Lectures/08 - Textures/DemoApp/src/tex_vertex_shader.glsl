#version 140

noperspective out vec2 texCoord; // gl_TexCoord[0], but without perspective correction

void main(void) {
    gl_TexCoord[0] = gl_MultiTexCoord0;
    texCoord = vec2(gl_MultiTexCoord0);

    gl_Position = ftransform();
}
