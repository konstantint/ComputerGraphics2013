/**
 * glimg-based texture loading utility function.
 *
 * Copyright 2013, Konstantin Tretyakov
 */


/**
 * Loads given image file into a given target-texture.
 * Builds mipmaps using gluBuildMipmaps.
 * Returns the OpenGL handle of the resulting texture object
 * (free it using glDeleteTexture eventually)
 *
 * Example:
 *    GLuint texHandle = load_texture(GL_TEXTURE_2D, GL_SRGB, "some_file.png");
 */
GLuint load_texture(GLenum target, GLint internalFormat, const char* filename);
