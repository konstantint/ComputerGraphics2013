/**
 * MTAT.03.015 Computer Graphics.
 * Shader configuration utility routines.
 * Copyright 2013, Konstantin Tretyakov
 */
#include "GL/glew.h"
#include "shader_util.h"
#include <stdexcept>
#include <cerrno>
#include <iostream>
#include <fstream>

// -------- Utility functions --------------
/**
 * Reads file contents into a string.
 */
std::string get_file_contents(const char *filename) {
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in) return std::string((std::istreambuf_iterator<char>(in)),
                                std::istreambuf_iterator<char>());
    else throw(errno);
}

/**
 * Allocates and compiles given shader in OpenGL
 */
GLuint compile(GLuint type, std::string source) {
    GLuint shader = glCreateShader(type);
    const GLchar* code[] = { source.c_str() };
    glShaderSource(shader, 1, code, NULL);
    glCompileShader(shader);
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::string log(length, ' ');
        glGetShaderInfoLog(shader, length, &length, &log[0]);
        std::cout << "Shader compilation error: " << log << std::endl;
        throw std::logic_error(log);
        return false;
    }
    return shader;
}

/**
 * Default shaders.
 */
const GLchar *default_vertex_shader =
    "#version 120\n"
    "varying vec4 vertex_color\n;"
    "void main(void) {\n"
    "    gl_Position = ftransform();\n"
    "    vertex_color = gl_Color;\n"
    "}";

const GLchar *default_fragment_shader = // Only supports coloring. Textures require custom stuff.
    "#version 120\n"
    "varying vec4 vertex_color;\n"
    "void main() {\n"
    "    gl_FragColor = vertex_color;\n"
    "}";

shader_prog::shader_prog(const char* vertex_shader_filename, const char* fragment_shader_filename) {
    v_source = vertex_shader_filename == NULL ? std::string((const char*)default_vertex_shader) : get_file_contents(vertex_shader_filename);
    f_source = fragment_shader_filename == NULL ? std::string((const char*)default_fragment_shader) : get_file_contents(fragment_shader_filename);
}

void shader_prog::use() {
    vertex_shader = compile(GL_VERTEX_SHADER, v_source);
    fragment_shader = compile(GL_FRAGMENT_SHADER, f_source);
    prog = glCreateProgram();
    glAttachShader(prog, vertex_shader);
    glAttachShader(prog, fragment_shader);
    glLinkProgram(prog);
    glUseProgram(prog);
}

void shader_prog::free() {
    glDeleteProgram(prog);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glUseProgram(0);
}

shader_prog::operator GLuint() {
    return prog;
}

void shader_prog::uniform1f(const char* name, float f) {
    GLint loc = glGetUniformLocation(prog, name);
    glUniform1f(loc, f);
}
void shader_prog::uniform1i(const char* name, int f) {
    GLint loc = glGetUniformLocation(prog, name);
    glUniform1i(loc, f);
}
void shader_prog::uniform3f(const char* name, float x, float y, float z) {
    GLint loc = glGetUniformLocation(prog, name);
    glUniform3f(prog, x, y, z);
}

