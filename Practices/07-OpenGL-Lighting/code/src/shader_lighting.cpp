/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 7: Lighting
 */

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
using namespace std;

// Define some constants we shall need later
#define GLUT_KEY_ESCAPE 27  // The keycode for the escape key.
#define GLUT_KEY_ENTER 13   // The keycode for "enter" key.

#include "primitives.h"
#include "shader_util.h"

// --------------- Forward declarations ------------- //
int main(int argc, char* argv[]);
void keyboard(unsigned char key, int x, int y);
void reshape(int w, int h);
void display();

shader_prog per_vertex_lighting("../src/lighting1_vertex.glsl", "../src/lighting1_fragment.glsl");

// ----------------------------------------------- //
/**
 * Program entry point
 */
int main(int argc, char* argv[]) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Lighting");
    glewInit();

    // Register handlers
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc((void (*)())glutPostRedisplay);
    reshape(800, 600);

    // Z-buffer
    glEnable(GL_DEPTH_TEST);

    // Back-face culling
    glEnable(GL_CULL_FACE);

    // Normal auto-normalization
    glEnable(GL_RESCALE_NORMAL);

    // Antialiasing & alpha-blending
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Reset default lighting parameters to zero
    float zeros[] = { 0, 0, 0, 0 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, zeros);
    glLightfv(GL_LIGHT0, GL_SPECULAR, zeros);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, zeros);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, zeros);

    // Switch on the custom shader
    per_vertex_lighting.use();

    // Run the event loop
    glutMainLoop();

    // Switch off custom shader
    per_vertex_lighting.free();
    return 0;
}

// ----------------------------------------------- //
void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    // Perspective projection
    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective (45.0, (GLdouble)w/(GLdouble)h, 0.5, 100);
    glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
}

// ----------------------------------------------- //
void keyboard(unsigned char key, int x, int y) {
    if (key == GLUT_KEY_ESCAPE) {
        glutLeaveMainLoop();
    }
}


// ----------------------------------------------- //
void display() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float t = glutGet(GLUT_ELAPSED_TIME);


    // Set up lighting here

    // NB: You must set up ALL the lights *before*
    // you start emitting triangles. This includes the light
    // that will move together with the cube.

    // Render the scene
    glLoadIdentity();
    gluLookAt(0, -15, 4, 0, 1, 1, 0, 0, 1);

    // Ground
    glColor3f(0.1, 1, 0.1);
    draw_ground(t); // This is implemented in primitives.h, but you shouldn't change anything there. Just assume this as given.

    // Magic cube, that shines light
    glColor3f(1, 0, 0);
    glPushMatrix();
        glTranslatef(2 + 2*sin(t*0.001), -9 + 2*cos(t*0.001), 3 + 0.3*cos(t*0.002));
        draw_cube(t);
    glPopMatrix();

    // Blue sphere
    glColor3f(0, 0, 1);
    glPushMatrix();
        glTranslatef(-1.5, -9, 2.5);
        draw_sphere(t);
    glPopMatrix();

    // Swap the front and back buffers (the usual double-buffering stuff)
    glutSwapBuffers();
}

