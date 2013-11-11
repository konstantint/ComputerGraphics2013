/**
 * MTAT.03.015 Computer Graphics.
 * Shader configuration utility routines.
 */
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>

/**
 * Modified version of code from:
 *  http://stackoverflow.com/questions/2795044/easy-framework-for-opengl-shaders-in-c-c
 */
class shader_prog {
private:
    GLuint vertex_shader, fragment_shader, prog;
    std::string v_source, f_source;
public:
    shader_prog(const char* vertex_shader_filename, const char* fragment_shader_filename);
    void use();
    void free();
    operator GLuint();

    // Shorthands for glUniform specification
    void uniform1i(const char* name, int i);
    void uniform1f(const char* name, float f);
    void uniform3f(const char* name, float x, float y, float z);
    void uniformMatrix4fv(const char* name, const float* matrix);
};
