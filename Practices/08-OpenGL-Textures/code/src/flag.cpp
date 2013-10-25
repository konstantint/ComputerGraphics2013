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

// --------------- Forward declarations ------------- //
int main(int argc, char* argv[]);
void keyboard(unsigned char key, int x, int y);
void reshape(int w, int h);
void display();

void draw_mesh(int n, float t);

// ----------------------------------------------- //
GLuint textureHandle;

shader_prog texture_shader("../src/flag.vert.glsl", "../src/flag.frag.glsl");

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
    glutIdleFunc((void (*)())glutPostRedisplay);
    reshape(800, 600);

    // Set basic config
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_RESCALE_NORMAL);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ---------- Set lighting ------------
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    float ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    float white[] = {7, 7, 7, 1};
    float black[] = {0, 0, 0, 1};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient );
    glLightfv(GL_LIGHT0, GL_AMBIENT, black);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);

    float position[] = {0, 0, 0, 1};
    float direction[] = {0, 0, -1, 0};
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 40);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 10);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.1);

    // -------------- Load texture ------------- //
    textureHandle = load_texture(GL_TEXTURE_2D, GL_SRGB, "../data/flag.png");

    // Uncomment for Exercise 2.
    //texture_shader.use();

    // Run the event loop
    glutMainLoop();

    // Free texture memory
    glDeleteTextures(1, &textureHandle);
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
void display() {
    float t = glutGet(GLUT_ELAPSED_TIME) * 0.001;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);

    // Set up material for the flag
    float black[] = {0, 0, 0, 1};
    float grey[] = {0.8, 0.8, 0.8, 1};
    float white[] = {1, 1, 1, 1};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, white);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, grey);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50);

    // ---------- Your code here: ----------
    //   1. Enable texturing and bind the loaded texture
    //   2. Configure texture filtering mode
    //   3. Set up texture coordinate generation

    // -------------------------------------
    // Draw the flag, slightly rocking around horizontal axis
    glRotatef(20*sin(t), 1, 0, 0);
    glScalef(4, 2, 1);
    draw_mesh(50, t);

    // Swap the front and back buffers (the usual double-buffering stuff)
    glutSwapBuffers();
}

// ----------------------------------------------- //
// Flag surface as a function of (x, y, t)
float flag_surface(float x, float y, float t) {
    // Make the flag wave along X with given frequency
    float wave_amplitude = 0.2; // How much the surface is displaced
    float wave_frequency = 1.0; // Oscillation frequency in Hz.
    float num_waves = 3.5;      // Number of wave peaks at any given time
    float phase_coordinate = -num_waves*2*M_PI*(x+1.0)/2.0;
    float wave_value = sin(2*M_PI*wave_frequency*t + phase_coordinate);
    return wave_amplitude*(wave_value - sin(2*M_PI*wave_frequency*t + 0));
    // Note that from the point of physics this is not a totally correct simulation of a waving manifold.
}

// ----------------------------------------------- //
// Renders a [-1, 1] x [-1, 1] wavy mesh in the xy plane, subdivided into n x n squares.
// t is the time variable.
void draw_mesh(int n, float t) {
    float dx = 2.0/n;
    float dy = 2.0/n;
    float freq = 1.4;
    for (int i = 0; i < n; i++) {
        float top = 1 - i*dy;
        float bottom = top - dy;
        glBegin(GL_TRIANGLE_STRIP);
            glVertex3f(-1, top, flag_surface(-1, top, t));
            glVertex3f(-1, bottom, flag_surface(-1, bottom, t));
            for (int j = 1; j <= n; j++) {
                glVertex3f(-1.0 + dx*j, top, flag_surface(-1.0 + dx*j, top, t));
                glVertex3f(-1.0 + dx*j, bottom, flag_surface(-1.0 + dx*j, bottom, t));
            }
        glEnd();
    }
}
