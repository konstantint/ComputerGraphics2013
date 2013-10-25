/**
 * glimg-based texture loading utility function.
 *
 * Copyright 2013, Konstantin Tretyakov
 */
#include <GL/glew.h>
#include <glimg/glimg.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <iostream>
using namespace std;

GLuint load_texture(GLenum target, GLint internalFormat, const char* filename) {
    GLuint handle;
    glGenTextures (1, &handle);
    glBindTexture (target, handle);
    // See http://glsdk.sourceforge.net/docs/html/group__module__glimg.html
    glimg::ImageSet* image_set = glimg::loaders::stb::LoadFromFile(filename);
    glTexImage2D(target, 0, internalFormat,
                 image_set->GetImage(0).GetDimensions().width, image_set->GetImage(0).GetDimensions().height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, image_set->GetImage(0).GetImageData());
    glGenerateMipmap(target);
    delete image_set;
    return handle;
}
