/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 5: Projections
 */

#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
using namespace std;

#include "glu_invert_matrix.h"

// Define some constants we shall need later
#define GLUT_KEY_ESCAPE 27  // The keycode for the escape key.
#define GLUT_KEY_ENTER 13   // The keycode for "enter" key.

const bool WIREFRAME = 0; // Set this to 1 to see a wireframe scene.

/**
 * To those of you who do not know C, the following line defines a datatype
 * "(pointer to a) function with no arguments".
 * That is, after this definition you may write
 *    function_t x;
 * This will define a variable (named x) to which you can assign functions
 * with no arguments (in our case those are most of the above), e.g.
 *    x = scene_orthogonal_projection;
 * (note the lack of () after function name:
 * We are not invoking the function and taking its result, we are assigning the
 * function itself to the variable)
 * Once assigned, you can invoke the function:
 *    x();
 */
typedef void (*function_t)();


// --------------- Forward declarations ------------- //
int main(int argc, char* argv[]);
void keyboard(unsigned char key, int x, int y);

void display();                             // <--- Exercise 3
void scene_orthogonal_projection();
void scene_perspective_projection();        // <--- Exercise 1
void scene_inverse_perspective_projection();// <--- Exercise 2
void scene_oblique_projection();

void draw_scene();
void draw_clip_volume(function_t projection_function);
void draw_label(function_t projection_function);

int current_scene_id = 0;


// ----------------------------------------------- //
/**
 * Program entry point
 */
int main(int argc, char* argv[]) {
    // Initialize GLUT
    glutInit(&argc, argv);
    // Request a double-buffered, RGB display mode.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    // Specify size and position of a window and create it
    glutInitWindowSize(800, 600);
    glutCreateWindow("Clip Volume Viewer");

    // Register handlers
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc((function_t)glutPostRedisplay);

    // Lighting and depth testing for non-wireframe scene
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LINE_SMOOTH);   // Line antialiasing
    glEnable(GL_POLYGON_SMOOTH);// Polygon antialiasing
    glEnable(GL_BLEND);         // Antialiasing works only with blending enabled
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Run the event loop
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
        current_scene_id = (current_scene_id + 1) % 4;
    }
}

// ----------------------------------------------- //
/**
 * This function is invoked every time the screen needs to be redrawn.
 * For example, every time the window is moved, covered by other windows,
 * or when we explicitly invoke glutPostRedisplay
 */
void display() {
    // Clear screen
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Choose current projection function
    function_t projection_function;
    if (current_scene_id == 0)      projection_function = scene_orthogonal_projection;
    else if (current_scene_id == 1) projection_function = scene_perspective_projection;
    else if (current_scene_id == 2) projection_function = scene_oblique_projection;
    else projection_function = scene_inverse_perspective_projection;

    // We split the screen into four parts and draw in each part separately.
    // We can use glViewport for that.

    // Top left -------------------------------------------
    // Scene itself
    glViewport(0, 300, 400, 300);
    // We want to have a different clear color for that part, this can be done using glScissor
    glEnable(GL_SCISSOR_TEST);
        glScissor(0, 300, 400, 300);
        glClearColor(0.3, 0.3, 0.3, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        projection_function();
    glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        draw_scene();

    // Top right -------------------------------------------
    // Draw the scene as above (including the scissored glClear to set the correct background),
    // but with the camera rotated slightly around (0, 0, -5)
    //
    //   YOUR CODE HERE

    // Bottom left -------------------------------------------
    // Clip volume before transformation
    glViewport(0, 0, 400, 300);

    // Set up the projection used to view the clip space
    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(30, 4.0/3, 0.5, 100);
    glMatrixMode(GL_MODELVIEW);

    // We know that the point (0, 0, -10) is at the center of the clip box
    // so we look at that point
    glLoadIdentity();
    gluLookAt(0, 13, -5, 0, 0, -5, 0, 0, -1);

    // Next render the scene, rotated around x axis.
    glTranslatef(0, 0, -5);
    glRotatef(glutGet(GLUT_ELAPSED_TIME)*0.03, -1, 0, 0);
    glTranslatef(0, 0, 5);
    draw_scene();
    draw_clip_volume(projection_function);  // Add clip volume to the drawing

    // Bottom right -------------------------------------------
    // Clip volume after projection
    glViewport(400, 0, 400, 300);

    // We know that the point (0, 0, 0) is at the center of the clip space
    // so we look at that point
    glLoadIdentity();
    gluLookAt(0, 13, 0, 0, 0, 0, 0, 0, -1);

    // Rotate around X axis.
    glRotatef(glutGet(GLUT_ELAPSED_TIME)*0.03, -1, 0, 0);

    // Next render the scene with projection transform included into the pipeline
    // Flip the z axis after projection (otherwise the result is somewhat unintuitive)
    glScalef(1, 1, -1);
    projection_function();
    draw_scene();
    draw_clip_volume(projection_function);

    // Write the name of the projection in the corner
    draw_label(projection_function);

    // Swap the front and back buffers (the usual double-buffering stuff)
    glutSwapBuffers();
}


/**
 * Render a simplistic scene in the camera frame.
 * All objects in the scene should be within [-2, 2], [-2, 2], [-3, -7] (wrt the camera frame)
 *
 * NB: You should not modify this function in any of the exercises!
 */
 void draw_scene() {
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT); // Save current state variables (color and lighting state)
    glPushMatrix();               // Save current matrix
        if (WIREFRAME) glDisable(GL_LIGHTING);
        // Define some scene - let this be a sphere and a cylinder that we are used to from the lectures
        // Position the light source just above, to the left, and a bit in front of the camera
        float light_position[] = {-5, 5, 0.1, 1};
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);

        // Set the view transform to look at the origin from a distance of 15.
        gluLookAt(0, -5, 0, 0, 0, 0, 0, 0, 1);
        // Further down the code "z" points to the top

        // Render a sphere with a cylinder at the origin with Z as the top.
        glColor3f(0, 1, 0);   // Green "pseudo-man" (sphere with a cylinder)
        glPushMatrix();
            glTranslatef(-1, -0.5, 0);
            glScalef(0.6, 0.6, 0.6);
            WIREFRAME ? glutWireSphere(1, 10, 10) : glutSolidSphere(1, 10, 10);
            glTranslatef(0, 0, -1-sqrt(1-0.7*0.7));
            WIREFRAME ? glutWireCylinder(0.7, 1, 20, 3) : glutSolidCylinder(0.7, 1, 20, 3);
        glPopMatrix();

        // Add a stretched cube (to see perspective and oblique effects)
        glPushMatrix();
            glColor3f(0, 0, 1);
            glTranslatef(1, 0, 1);
            glScalef(1, 3.95, 1);
            WIREFRAME ? glutWireCube(1) : glutSolidCube(1);
        glPopMatrix();

        // And a red sphere too, why not, everybody loves red spheres!
        glPushMatrix();
            glColor3f(1, 0, 0);
            glTranslatef(0, -1, 0);
            WIREFRAME ? glutWireSphere(0.5, 4, 4) : glutSolidSphere(0.5, 4, 4);
        glPopMatrix();

    glPopMatrix();
    glPopAttrib();
}

/**
 * Sets the projection transform matrix for the scene.
 */
void scene_orthogonal_projection() {
    glOrtho(-2, 2, -1.5, 1.5, 3, 7);
}

void scene_perspective_projection() {
    gluPerspective(45, 4.0/3, 2, 7);
}

void scene_inverse_perspective_projection() {
    // Implement *inverted* perspective frustum
    // Hint: the center of the scene (in camera frame) is at (0, 0, -5)
    // Hint: You have to wrap the following call with some additional calls before and after it.
    gluPerspective(45, 4.0/3, 2, 7);
}

void scene_oblique_projection() {
    glOrtho(-2, 2, -1.5, 1.5, -2, 2);   // NB: We translate the scene to z=0 further down the code.
    float a = 3.14/2-0.3;
    float b = 3.14/2-0.2;
    float H[] = { 1, 0, 0, 0,
                  0, 1, 0, 0,
                 1.0/tan(a), 1.0/tan(b), 1, 0,
                  0, 0, 0, 1};
    glMultMatrixf(H);
    // Need to translate the sceen to be center at 0,
    // for appropriate oblique skewing.
    glTranslatef(0, 0, 5);
}


/**
 * Given a function that sets up a projection for the scene,
 * renders the clip volume into the camera frame.
 */
void draw_clip_volume(function_t projection_setup_function) {
    // Extract the projection matrix
    glPushMatrix();                     // Save current modelview matrix
        glLoadIdentity();               // Reset modelview matrix
        projection_setup_function();    // The projection_setup_function has output the required matrix into GL_MODELVIEW_MATRIX
        GLdouble projection_matrix[16]; // We simply read this matrix out
        glGetDoublev(GL_MODELVIEW_MATRIX, projection_matrix);
    glPopMatrix();                      // Restore previous state

    // Now we invert the projection matrix and output a (-1 +1) cube
    // transformed by this inverse.
    GLdouble inverse_projection_matrix[16];
    if (!gluInvertMatrix(projection_matrix, inverse_projection_matrix)) {
        cout << "Failed to invert projection matrix" << endl;
    }

    // Add to the scene the bounds of the clip space
    // Those can be obtained if we draw the normalized clip space transformed by the inverse of the projection matrix.
    glPushMatrix();
    glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);
        glMultMatrixd(inverse_projection_matrix);
        glutWireCube(2);
    glPopAttrib();
    glPopMatrix();
}

/**
 * Draws the name of the projection in the corner.
 */
void draw_label(function_t projection_function) {
    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glRasterPos2f(0.1, -0.9);
    glColor3f(1, 1, 1);
    const char* name =
        projection_function == scene_orthogonal_projection ? "Orthogonal" :
        projection_function == scene_oblique_projection    ? "Oblique" :
        projection_function == scene_perspective_projection? "Perspective" :
        projection_function == scene_inverse_perspective_projection ? "Inverse perspective" :
        "Unknown";
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)name);
    glEnable(GL_LIGHTING);
}

// ----------------------------------------------- //
