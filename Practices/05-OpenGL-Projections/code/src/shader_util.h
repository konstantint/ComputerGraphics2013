/**
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
};
