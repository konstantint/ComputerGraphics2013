/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 8: Textures
 */

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
using namespace std;

#include "shader_util.h"
#include "texture_util.h"
#include "wave_mesh.h"

// --------------- Forward declarations ------------- //
int main(int argc, char* argv[]);
void keyboard(unsigned char key, int x, int y);
void mouseMove(int x, int y);
void reshape(int w, int h);
void display();
void idle();

// ----------------------------------------------- //
// Global variables
GLuint textureHandle;
WaveMesh mesh(30);
int last_t = 0;
shader_prog refract_shader("../src/refract.vert.glsl", "../src/refract.frag.glsl");

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
    glutIdleFunc(idle);
    glutPassiveMotionFunc(mouseMove);
    reshape(800, 600);

    // -------------- Load texture ------------- //
    textureHandle = load_texture(GL_TEXTURE_2D, GL_SRGB, "../data/serious_cat_is_serious.jpg");
    refract_shader.use();

    // Run the event loop
    glutMainLoop();

    // Free texture memory
    glDeleteTextures(1, &textureHandle);
    return 0;
}

// ----------------------------------------------- //
void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

// ----------------------------------------------- //
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) glutLeaveMainLoop();
}

// ----------------------------------------------- //
void mouseMove(int x, int y) {
    float u = 2.0*x/glutGet(GLUT_WINDOW_WIDTH) - 1;
    float v = 1 - 2.0*y/glutGet(GLUT_WINDOW_HEIGHT);
    mesh.touch(u, v);
}

// ----------------------------------------------- //
void idle() {
    float dt = (glutGet(GLUT_ELAPSED_TIME) - last_t)*0.001;
    last_t = glutGet(GLUT_ELAPSED_TIME);
    mesh.step(dt);
    glutPostRedisplay();
}

// ----------------------------------------------- //
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Comment the next code line out when solving exercise.
    // Rotation of the mesh is only useful to see how ``waviness' affects the z coordinate of the mesh
    // When we actually render, we need to look straight down at the mesh, i.e. no rotation.
    glPolygonMode(GL_FRONT, GL_LINE); glRotatef(20, 1, 0, 0);

    mesh.draw();

    // Swap the front and back buffers (the usual double-buffering stuff)
    glutSwapBuffers();
}
