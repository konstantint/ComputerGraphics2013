/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 4: Introduction to OpenGL.
 */
#include "glm.h"

// --------------- Global variables --------------------- //
GLMmodel* model = NULL;
GLfloat model_dimensions[3];

// --------------- Forward declarations ------------- //
int main(int argc, char* argv[]);
void display();
void init_model();

// ----------------------------------------------- //
/**
 * Program entry point
 */
int main(int argc, char* argv[]) {

    // Initialize GLUT and create a window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(30,30);
    glutCreateWindow("Loading models from file");

    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);

    // Set up perspective
    glMatrixMode(GL_PROJECTION);
        gluPerspective(45, 1, 0.5, 100);
    glMatrixMode(GL_MODELVIEW);

    // Enable basic lighting
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    // Load model
    init_model();

    // Register handlers
    glutDisplayFunc(display);

    // Run main application loop
    glutMainLoop();
    return 0;
}

/**
 * This function is invoked every time the screen is being repainted.
 */
void display() {
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up view
    glLoadIdentity();
    gluLookAt(0, 1, 6, 0, 0, 0, 0, 1, 0);

    // Draw the y = 0 plane
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_QUADS);
        glVertex3f(-3, 0, 3);
        glVertex3f(-3, 0, -3);
        glVertex3f(3, 0, -3);
        glVertex3f(3, 0, 3);
    glEnd();

    // Draw the model "standing" on the y=0 plane
    glPushMatrix();
        glTranslatef(0, model_dimensions[1]/2, 0);
        glmDraw(model, GLM_SMOOTH | GLM_MATERIAL);
    glPopMatrix();

    // Flush picture
    glutSwapBuffers();
}

// ----------------------------------------------- //
void init_model() {
    model = glmReadOBJ("../data/al.obj");
    if (!model) exit(0);
    glmUnitize(model);
    glmFacetNormals(model);
    glmVertexNormals(model, 90.0);
    glmDimensions(model, model_dimensions);
}
