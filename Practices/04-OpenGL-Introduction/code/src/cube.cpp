/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 4: Introduction to OpenGL.
 */
#include <iostream>
using namespace std;

#include <GL/freeglut.h>
#define GLUT_KEY_ESCAPE 27  // The keycode for the escape key.
#define GLUT_KEY_ENTER 13   // The keycode for "enter" key.
// In some VirtualBox VMs program crashes on glutCreateWindow. Invoking this beforehand magically fixes the problem.
// http://stackoverflow.com/questions/9759898/opengl-glut-on-virtualbox-ubuntu-11-10-segmentation-fault
#define GLUT_VIRTUAL_BOX_FIX glutGet(GLUT_DISPLAY_MODE_POSSIBLE);


// --------------- Forward declarations ------------- //
int main(int argc, char* argv[]);
void keyboard(unsigned char key, int x, int y);
void mouse_drag(int x, int y);
void display();

// Complete this routine
void draw_cube();

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
    glutInitWindowPosition(30,30);
    GLUT_VIRTUAL_BOX_FIX;
    glutCreateWindow("The Cube");

    // Register handlers
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(mouse_drag); // Handler for the "mouse drag" event

    // Instead of providing our own "idle" function, we shall
    // simply ask GLUT to invoke glutPostRedisplay all the time
    glutIdleFunc((void(*)())glutPostRedisplay);
    // The (void(*)()) part in front is a type cast which prevents some compiler warnings, don't be confused by it
    // for most compilers you could simply write glutIdleFunc(glutPostRedisplay);

    // Enable depth test. You should uncomment this line after taking a look how things work without it.
    // glEnable(GL_DEPTH_TEST);
    // We shall discuss depth testing (the z-buffer algorithm) in two weeks.

    // Set up perspective projection matrix
    // We shall discuss how this works in the next lecture, for now just ignore.
    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60, 1, 0.5, 100);
    glMatrixMode(GL_MODELVIEW);

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
 * Mouse drag handler
 */
void mouse_drag(int x, int y) {
    cout << "mouse_drag(" << x << ", " << y << ")" << endl;
}

// ----------------------------------------------- //
/**
 * Repaint routine (the drawing routine for our only scene)
 */
void display() {
    // Clear screen and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Start with a clear modelview matrix
    glLoadIdentity();

    // Set up the view transformation.
    // We'll be looking from point (0, 0, 5)
    // onto point (0, 0, 0) with our "top" pointing towards (0, 1, 0);
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);
    // Note that with such a set up the up-down direction on screen corresponds to y axis,
    // the left-right is the x axis, and the "out of the screen" direction is the z axis.

    // Rotate
    float time = glutGet(GLUT_ELAPSED_TIME);
    glRotatef(time/100, 0.0, 1.0, 1.0);

    // Draw the cube
    draw_cube();

    // Flush picture
    glutSwapBuffers();
}


// ----------------------------------------------- //
/**
 * Cube drawing
 */
void draw_cube() {
    glTranslatef(-0.5, -0.5, -0.5);
    glBegin(GL_QUADS);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 1);
        glVertex3f(0, 1, 1);
        glVertex3f(0, 1, 0);

        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(1, 0, 0);
        glVertex3f(1, 0, 1);
        glVertex3f(1, 1, 1);
        glVertex3f(1, 1, 0);

        // ... complete me ...

    glEnd();
}

// ----------------------------------------------- //



