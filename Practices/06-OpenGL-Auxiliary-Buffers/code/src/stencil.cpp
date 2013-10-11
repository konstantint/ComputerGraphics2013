/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 6: Stencil buffer.
 */
#include "glm.h"

// --------------- Global variables --------------------- //
GLMmodel* model = NULL;
GLfloat model_dimensions[3];

// --------------- Forward declarations ------------- //
int main(int argc, char* argv[]);
void display();
void keyboard(unsigned char key, int x, int y);
void init_model();
void draw_man();
void draw_floor();

// ----------------------------------------------- //
int main(int argc, char* argv[]) {

    // Initialize GLUT and create a window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(30, 30);
    glutCreateWindow("Stencil buffer");

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
    float light_dir[] = {1.0, 0.0, 1.0, 0.0}; // Lets' have a directional light for a change
    glLightfv(GL_LIGHT0, GL_POSITION, light_dir);

    // Load model
    init_model();

    // Register handlers
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc((void(*)())glutPostRedisplay);

    // Run main application loop
    glutMainLoop();
    return 0;
}

void keyboard(unsigned char key, int x, int y) {
    if (key <= 27) glutLeaveMainLoop();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(0, 1, 6, 0, 1, 0, 0, 1, 0);
    glRotatef(glutGet(GLUT_ELAPSED_TIME)*0.01, 0, 1, 0);

    draw_floor();
    draw_man();
    glutSwapBuffers();
}


// ----------------------------------------------- //

void draw_man() {
    glPushMatrix();
        glTranslatef(0, model_dimensions[1]/2, 0);
        glmDraw(model, GLM_SMOOTH | GLM_MATERIAL);
    glPopMatrix();
}

void draw_floor() {
    glPushMatrix();
        // Draw the y = 0 plane
        glDisable(GL_LIGHTING); // Lighting computations are not good here anyway
        glColor4f(0.5, 0.5, 0.5, 0.8);
        glBegin(GL_QUADS);
            glVertex3f(3, 0, 3);
            glVertex3f(3, 0, -3);
            glVertex3f(-3, 0, -3);
            glVertex3f(-3, 0, 3);
        glEnd();
        glEnable(GL_LIGHTING);
    glPopMatrix();
}

void init_model() {
    model = glmReadOBJ("../data/al.obj");
    if (!model) exit(0);
    glmUnitize(model);
    glmFacetNormals(model);
    glmVertexNormals(model, 90.0);
    glmDimensions(model, model_dimensions);
}
