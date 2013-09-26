/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 4: Introduction to OpenGL.
 */
#include <iostream>
using namespace std;

#include <GL/freeglut.h>
#define GLUT_KEY_ESCAPE 27  // The keycode for the escape key.
#define GLUT_KEY_ENTER 13   // The keycode for "enter" key.


// --------------- Forward declarations ------------- //
int main(int argc, char* argv[]);
void keyboard(unsigned char key, int x, int y);
void display();

// Complete this routine
void draw_cube();

// ----------------------------------------------- //
/**
 * Program entry point
 */
int main(int argc, char* argv[]) {
    // Initialize GLUT and create a window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(30,30);
    glutCreateWindow("Introduction to OpenGL");

    // Register handlers
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc((void(*)())glutPostRedisplay);

    // Enable depth desting
    glEnable(GL_DEPTH_TEST);

    // Enable basic lighting (to be discussed in later lectures, so far take this for granted)
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    // Position the light source
    GLfloat light_position[]= { 0.0f, 0.0f, 10.0f, 1.0f };  // Note the homogeneous representation
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_COLOR_MATERIAL); // This is a cheat, that will let us use "glColor" to specify "colors" of objects.
    // As you will learn from later lectures, the concept of "color" for vertices
    // is meaningless when we are using lighting models.

    // Set up perspective projection
    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60, 1, 0.5, 1000);
    glMatrixMode(GL_MODELVIEW);

    // Run main application loop
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
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Start by setting up the view transformation
    glLoadIdentity();

    // We'll be looking from point (0, -250, 100)  (by the way, the scale is in millions of kilometers).
    // onto point (0, 0, 0) with our "top" pointing towards (0, 0, 1);
    gluLookAt(0, -250, 100, 0, 0, 0, 0, 0, 1);

    // Rotate
    float time = glutGet(GLUT_ELAPSED_TIME);
    // Have the earth make a full circle along its orbit in 10 seconds.
    //glRotatef(time/100, 0.0, 1.0, 1.0);

    // Draw planets
    // The Sun
    glColor3f(1.0, 0.7, 0.0);
    glutSolidSphere(5, 10, 10); // Actual radius of the sun is ~0.5M km. We are drawing it 10 times larger to see it.

    // The following are potential orbits for earth, venus, mercury, etc.
    // You can delete them if they spoil the picture for you
    glColor3f(1, 1, 1);
    glutWireTorus(0.01,  57.909, 2, 100);  // Mercury orbit
    glutWireTorus(0.01, 108.208, 2, 100);  // Venus orbit
    glutWireTorus(0.01, 149.598, 2, 100);  // Earth orbit

    // Your task: add Mercury, Venus, Earth and Moon.
    // You do not have to make the model to scale (but you are welcome to try!).
    // What concerns planet radii, you probably do not want to keep them
    // to scale, as those will be too tiny to see.
    // Please, make it so that the planets *are* visible.
    // I suggest a magnification of around a 500 to 1000 (i.e. the radius of the sphere representing mercury could be 1.2 to 2.4)
    // You will also need to increase the orbit of the moon to fit.
    //
    // Here's some useful information, if you want to make use of it.
    //  Mercury: R=2439km, orbit= 57.909Mkm, orbital period=87.969 days
    //  Venus:   R=6058km, orbit=108.208Mkm, orbital period=224.701 days
    //  Earth:   R=6371km, orbit=149.598Mkm, orbital period=365.256 days
    //  Moon:    R=1737km, orbit= 384399km,  orbital period=27.322 days

    // ... your code here ...

    // Flush picture to screen
    glutSwapBuffers();
}
