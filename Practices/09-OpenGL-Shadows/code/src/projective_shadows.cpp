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
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
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
 * Exercise 1. Projective shadows.
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

    // --------------------------- Draw walls & light positions ---------------------------
    draw_walls();
    shader.uniform1i("no_lighting", 1); // Switch off lighting when rendering light source positions  (NB: this is a variable defined in the shader).
    draw_lights(lights);
    shader.uniform1i("no_lighting", 0);

    // --------------------------- Draw objects ---------------------------
    for (int i = 0; i < objects.size(); i++) {
        objects[i].draw();
    }

    // --------------------------- Draw shadows ---------------------------
    // YOUR CODE BELOW: Implement projective shadows
    // The example below constructs a shadow projection of one object onto the ground plane (z = 0)
    // with the first light source as the projection center.
    // Your task is to update this code to project all objects from all light sources to all three planes.
    //
    shader.uniform1i("no_lighting", 1);  // Switch off lighting computations
    glDepthMask(GL_FALSE);               // Disable writing to the depth buffer (it results in weird artefacts)

    glColor4f(0, 0, 0, 0.3);             // Shadow color, note the alpha component
    float plane1[] = {0, 0, 1, 0};       // Plane equation z = 0 represented in the form A*x + B*y + C*z + D = 0
                                         // Note that we want the normal of the plane (A, B, C) to point towards the light
                                         // (otherwise we shall be offsetting the shadow in the wrong direction)

    // Now we apply the shadow projection transformation to an object
    // and then render it
    glPushMatrix();
        shadowMatrix(lights[0], plane1, 0.001);
        objects[0].draw();
    glPopMatrix();

    glDepthMask(GL_TRUE); // Turn depth buffer writing and lighting back on
    shader.uniform1i("no_lighting", 0);

    glutSwapBuffers();
}
