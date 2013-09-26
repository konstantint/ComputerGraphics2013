/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 4: Introduction to OpenGL.
 */

#include <GL/freeglut.h>

// Define some constants we shall need later
#define GLUT_KEY_ESCAPE 27  // The keycode for the escape key.
#define GLUT_KEY_ENTER 13   // The keycode for "enter" key.


// --------------- Forward declarations ------------- //
int main(int argc, char* argv[]);
void keyboard(unsigned char key, int x, int y);
void idle();
void display();

void draw_triangle_scene();
void draw_tree_scene();
void draw_tree();
void draw_stem();
void draw_leaf();

// --------------- Global state ------------- //
int current_scene_id = 0;

// ----------------------------------------------- //
/**
 * Program entry point
 */
int main(int argc, char* argv[]) {
    // Initialize GLUT
    glutInit(&argc, argv);
    // Request a double-buffered, RGB display mode.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    // Specify size and position of a window and create it
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(30,30);
    glutCreateWindow("Introduction to OpenGL");

    // Register handlers
    glutDisplayFunc(display);   // Every time the screen needs to be redrawn, invoke this
    glutKeyboardFunc(keyboard); // Every time the key is pressed, invoke this
    glutIdleFunc(idle);         // Every time the event loop has nothing to do, this is invoked
    // GLUT lets you specify many other handlers for:
    //   glutReshapeFunc,   to reach on window size changes
    //   glutMouseFunc,     to react on mouse clicks
    //   glutJoystickFunc,  to interact using the joystick
    //   ... and more,      see http://freeglut.sourceforge.net/docs/api.php

    // Run the main event loop. To leave it, call glutLeaveMainLoop from a handler.
    glutMainLoop();
    return 0;
}

// ----------------------------------------------- //
/**
 * This is the keyboard handler.
 */
void keyboard(unsigned char key, int x, int y) {
    if (key == GLUT_KEY_ESCAPE) {
        glutLeaveMainLoop();
    }
    else if (key == GLUT_KEY_ENTER || key == ' ') {
        current_scene_id = (current_scene_id + 1) % 2;
    }
}

// ----------------------------------------------- //
/**
 * This is invoked whenever there are no other things to do in the event loop.
 *
 * Here you could update the physics of your world, i.e. this somewhat corresponds to the "animate"
 * routine from the first practice session.
 */
void idle() {
    // ... Do whatever you want, perhaps depending on the current scene ...

    // If you changed something, you should request to redraw the screen
    //(i.e. ask the event loop to invoke the display() function) as soon as possible.
    glutPostRedisplay();
}

// ----------------------------------------------- //
/**
 * This function is invoked every time the screen needs to be redrawn.
 * For example, every time the window is moved, covered by other windows,
 * or when we explicitly invoke glutPostRedisplay
 */
void display() {
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the current scene
    switch(current_scene_id) {
    case 0:
        draw_triangle_scene();
        break;
    case 1:
        draw_tree_scene();
        break;
    }

    // Swap the front and back buffers (the usual double-buffering stuff)
    glutSwapBuffers();
}

// ----------------------------------------------- //
/**
 * This scene simply draws a colored triangle.
 */
void draw_triangle_scene() {
    // Set up a custom model-view matrix here
    glLoadIdentity();

    glBegin(GL_TRIANGLES);
        glColor3f(1.0, 0.0, 0.0);   // We can associate color with each vertex. The next emitted vertex will be "red".
        glVertex2f(0.0, 0.5);

        glColor3f(0.0, 1.0, 0.0);   // This changes current color. So that the next emitted vertex will be "green".
        glVertex2f(-0.5, -0.5);

        glColor3f(0.0, 0.0, 1.0);   // By default, OpenGL will interpolate colors for all pixels between the vertices.
        glVertex2f(0.5, -0.5);
    glEnd();
}

// ----------------------------------------------- //
/**
 * This scene draws a procedurally generated tree.
 */
void draw_tree_scene() {
    glLoadIdentity();
    glTranslatef(0, -1.0, 0); // The view transform: move the origin down to be at the "ground"
    draw_tree();
}

void draw_tree() {
    // A tree consists of a stem and two leaves
    draw_stem();

    // Each leaf has his own transformation associated with it
    // The left leaf turns to the left
    glPushMatrix();
        glTranslatef(0, 0.6, 0); // Height of the stem is 0.6
        glRotatef(35, 0, 0, 1);

        // Now that we positioned the coordinates as needed, draw the leaf
        draw_leaf();
    glPopMatrix();

    // The right leaf -- to the right wrt to stem
    glPushMatrix();
        glTranslatef(0, 0.6, 0); // Height of the stem is 0.6
        glRotatef(-35, 0, 0, 1);

        // Now that we positioned the coordinates as needed, draw the leaf
        draw_leaf();
    glPopMatrix();
}

// The stem object
void draw_stem() {
    glBegin(GL_TRIANGLES);
        // I am using the "unsigned byte" version of the "Color" function because
        // I googled for "brown", found the RGB values in the 0-255 range, and did not care to
        // transform them to 0.0-1.0 float range.
        glColor3ub(192, 64, 0);
        glVertex2f(0.0, -0.0);
        glVertex2f(-0.05, 0.6);
        glVertex2f(0.05, 0.6);
    glEnd();
}

// The leaf object
void draw_leaf() {
    // This is actually the same triangle as in the "draw stem", but with a different color
    // You are free to make a nicer leaf, of course.
    glBegin(GL_TRIANGLES);
        glColor3ub(50, 177, 65);
        glVertex2f(0.0, 0.0);
        glVertex2f(-0.05, 0.6);
        glVertex2f(0.05, 0.6);
    glEnd();
}
