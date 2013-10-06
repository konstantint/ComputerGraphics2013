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
void draw_triangle();

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
    glutCreateWindow("Hello GLSL");
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
    shader_prog shader("../src/simple_vertex_shader.glsl", NULL);
    shader.use();

    glLoadIdentity();
    glRotatef(glutGet(GLUT_ELAPSED_TIME)*0.1, 0, 0, 1);

    /** // Uncomment this part to study variable binding in the shader
    // Let us extract the current model-view matrix
    float model_view_matrix[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, model_view_matrix);

    // Now let us bind this to the "transformation_matrix" variable in our shader.
    GLint variable_location = glGetUniformLocation(shader, "transformation_matrix");
    glUniformMatrix4fv(variable_location, 1, false, model_view_matrix);
    */

    draw_triangle();

    // Free the vertex shader
    shader.free();

    // Flush picture
    glutSwapBuffers();
}


// ----------------------------------------------- //
/**
 * Classical triangle
 */
void draw_triangle() {
    glBegin(GL_TRIANGLES);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(0, 0.5, 0);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(0.5, -0.5, 0);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(-0.5, -0.5, 0);
    glEnd();
}

// ----------------------------------------------- //



