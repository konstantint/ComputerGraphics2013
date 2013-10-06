/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 5: Vertex shaders
 */
#include <iostream>
using namespace std;

#include <GL/glew.h>
#include <GL/freeglut.h>
#define GLUT_KEY_ESCAPE 27  // The keycode for the escape key.
#define GLUT_KEY_ENTER 13   // The keycode for "enter" key.

#include "shader_util.h"


// --------------- Forward declarations ------------- //
int main(int argc, char* argv[]);
void keyboard(unsigned char key, int x, int y);
void display();

// ----------------------------------------------- //
/**
 * Program entry point
 */
int main(int argc, char* argv[]) {
    // Initialize GLUT
    glutInit(&argc, argv);
    // Request a double-buffered, RGB display mode with a depth buffer (will be discussed later)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    // Specify size and position of a window and create it
    glutInitWindowSize(400, 400);
    glutCreateWindow("Wave");
    // Initialize GL extensions
    glewInit();
    if (!GLEW_VERSION_2_0) {
        cerr << "OpenGL 2.0 not available\n";
        return 1;
    }

    // Register handlers
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc((void(*)())glutPostRedisplay);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Run the main event loop. To leave it, call glutLeaveMainLoop from a handler.
    glutMainLoop();
    return 0;
}

// ----------------------------------------------- //
/**
 * Keyboard handler
 */
void keyboard(unsigned char key, int x, int y) {
    if (key == GLUT_KEY_ESCAPE || key == ' ') glutLeaveMainLoop();
}

// ----------------------------------------------- //
/**
 * Repaint routine (the drawing routine for our only scene)
 */
void display() {
    // Clear screen and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Load and use the vertex shader
    shader_prog shader("../src/wave_vertex_shader.glsl", NULL);
    shader.use();

    // Send current time in seconds as a float to the shader
    float time = (float)glutGet(GLUT_ELAPSED_TIME)/1000;
    GLint variable_location = glGetUniformLocation(shader, "time");
    glUniform1f(variable_location, time);

    // Draw a sphere
    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60, 1, 1, 10);
    glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(2, 2, 2, 0, 0, 0, 0, 0, 1);
    glutWireSphere(1.5, 40, 40);

    // Free the vertex shader
    shader.free();

    // Flush picture
    glutSwapBuffers();
}
