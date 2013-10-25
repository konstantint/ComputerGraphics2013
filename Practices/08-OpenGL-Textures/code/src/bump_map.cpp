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
#include "shader_util.h"

// --------------- Forward declarations ------------- //
int main(int argc, char* argv[]);
void keyboard(unsigned char key, int x, int y);
void mouseMove(int mx, int my);
void reshape(int w, int h);
void display();

// ----------------------------------------------- //
shader_prog bump_shader("../src/bump.vert.glsl", "../src/bump.frag.glsl");
GLuint textureHandle;

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
    glutPassiveMotionFunc(mouseMove);
    reshape(800, 600);

    // Set basic config
    glEnable(GL_DEPTH_TEST);

    // ---------- Set lighting ------------
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    float ambient[] = { 0.02, 0.02, 0.02, 1.0 };
    float strong_white[] = {1.5, 1.5, 1.5, 1};
    float white[] = {1.0, 1.0, 1.0, 1};
    float red[] = {1.0, 0.05, 0.05, 1};
    float black[] = {0, 0, 0, 1};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, black );
    glLightfv(GL_LIGHT0, GL_AMBIENT, black);
    glLightfv(GL_LIGHT0, GL_SPECULAR, strong_white);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, strong_white);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0);
    glLightfv(GL_LIGHT0, GL_POSITION, black);
    glMaterialfv(GL_FRONT, GL_AMBIENT, red);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
    glMaterialfv(GL_FRONT, GL_AMBIENT, white);

    // -------------- Load texture ------------- //
    textureHandle = load_texture(GL_TEXTURE_2D, GL_SRGB, "../data/ut.png");
    bump_shader.use();

    // Run the event loop
    glutMainLoop();

    // Free memory
    glDeleteTextures(1, &textureHandle);
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
void mouseMove(int mx, int my) {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    float x = 2.0*mx/w - 1.0;
    float y = 1.0 - 2.0*my/h;
    x *= 4*tan(45.0/2)*w/h;
    y *= 4*tan(45.0/2);
    glLoadIdentity();
    float position[] = {x, y, -4, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glutPostRedisplay();
}


// ----------------------------------------------- //
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);
    glScalef(2, 2, 1);
    glBegin(GL_QUADS);
        glVertex2f(-1, -1);
        glVertex2f(1, -1);
        glVertex2f(1, 1);
        glVertex2f(-1, 1);
    glEnd();
    glutSwapBuffers();
}
