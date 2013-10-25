/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 8: Textures
 */

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
using namespace std;

#include "texture_util.h"

// --------------- Forward declarations ------------- //
int main(int argc, char* argv[]);
void keyboard(unsigned char key, int x, int y);
void reshape(int w, int h);
void display();

// ----------------------------------------------- //
/**
 * Program entry point
 */
int main(int argc, char* argv[]) {
    // Initialize GLUT & GLEW
    glutInit(&argc, argv);
    glutInitContextVersion(2, 1);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Textures");
    if (glewInit() != GLEW_OK) {
        cout << "Glew initialization failed" << endl;
        return 1;
    }

    // Register handlers
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc((void(*)())glutPostRedisplay);
    reshape(800, 600);

    // Configure OpenGL
    glEnable(GL_DEPTH_TEST);

    // -------------- Your code here ------------- //

    // ------------------------------------------- //

    // Run the event loop
    glutMainLoop();

    // Be polite, don't forget to free texture memory in the end.
    // ...
    return 0;
}

// ----------------------------------------------- //
void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45, (float)w/h, 0.5, 100);
    glMatrixMode(GL_MODELVIEW);
}

// ----------------------------------------------- //
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) glutLeaveMainLoop();
}


// ----------------------------------------------- //
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0, 0, 4, 0, 0, 0, 0, 1, 0);
    glRotatef(glutGet(GLUT_ELAPSED_TIME)*0.02, 1, 1, 0);
    glutSolidTeapot(1);
    glutSwapBuffers();
}
