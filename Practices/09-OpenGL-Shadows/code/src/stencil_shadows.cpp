/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 9: Shadows
 */

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
#include <vector>
using namespace std;

#include "shader_util.h"
#include "scene_primitives.h"

// --------------- Forward declarations ------------- //
int main(int argc, char* argv[]);
void keyboard(unsigned char key, int x, int y);
void reshape(int w, int h);
void display();

// ----------------------------------------------- //
shader_prog shader("../src/default.vert.glsl", "../src/default.frag.glsl");

// ----------------------------------------------- //
/**
 * Program entry point
 */
int main(int argc, char* argv[]) {
    // ---------- Initialize GLUT & GLEW ----------
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Shadows");
    if (glewInit() != GLEW_OK) {
        cout << "Glew initialization failed" << endl;
        return 1;
    }

    // ---------- Register handlers ----------
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc((void (*)())glutPostRedisplay);
    reshape(800, 600);

    // ---------- Set basic config ----------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ---------- Set light source parameters ------------
    configure_lighting();

    // ---------- Do per-fragment lighting ----------
    shader.use();

    // ---------- Run the event loop ----------
    glutMainLoop();
    return 0;
}

// ----------------------------------------------- //
void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective (45.0, (GLdouble)w/(GLdouble)h, 0.5, 100);
    glMatrixMode(GL_MODELVIEW);
}

// ----------------------------------------------- //
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) glutLeaveMainLoop();
}

// ----------------------------------------------- //
/**
 * Exercise 2. Stencil shadows.
 */
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float t = glutGet(GLUT_ELAPSED_TIME)*0.001;

    // --------------------------- View transform ---------------------------
    glLoadIdentity();
    gluLookAt(0, -10, 6,
              0, -1, 3,
              0, 0, 1);

    // --------------------------------------------------------
    // Prepare positions of two light sources and two "Cube" objects
    vector<float*> lights = set_light_positions(t);
    vector<Cube> objects = create_objects(t);

    // --------------------------- Draw scene geometry only ambient lit ---------------------------
    shader.uniform1i("no_light0", 1);
    shader.uniform1i("no_light1", 1);
    draw_walls();
    for (int i = 0; i < objects.size(); i++) {
        objects[i].draw();
    }
    shader.uniform1i("no_light0", 0);
    shader.uniform1i("no_light1", 0);

    // --------------------------- Now goes the stencil shadow algorithm logic ---------------------------
    // YOUR CODE BELOW: Implement the "Depth Pass" stencil shadow algorithm
    glDepthFunc(GL_LEQUAL); // This is a multipass algorithm, so we need <= depth checks.

    // 1. Turn off writing to the depth and color buffers


    // 2. Enable stencil test, clear stencil buffer, set stencil test to always pass,
    // set stencil op to increase stencil buffer value whenever depth test passes


    // 3. Enable backface culling and render all shadow volumes


    // 4. Now set the stencil op to decrease stencil buffer value whenever depth test passes


    // 5. Enable front-face culling and render all the shadow volumes again


    // 6. You have prepared the stencil buffer. Now set stencil test to pass whenever stencil value is equal to 0,
    // and set StencilOp to always KEEP.


    // 7. Finally, return to normal rendering settings (writing to depth and color buffer, back-face culling),
    // and re-render the scene with full lighting (except that stencil is in effect now).


    // 8. Turn off stencil testing
    glDisable(GL_STENCIL_TEST);

    // ---------------------- End of your code ---------------------- //
    // Draw light positions
    shader.uniform1i("no_lighting", 1);
    draw_lights(lights);
    shader.uniform1i("no_lighting", 0);

    glutSwapBuffers();
}
