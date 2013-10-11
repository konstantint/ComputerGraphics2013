/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 6: Blending
 */

#include <GL/glew.h>    // glBlendColor is an extension function, so use GLEW.
#include <GL/freeglut.h>

// --------------- Forward declarations ------------- //
int main(int argc, char* argv[]);
void keyboard(unsigned char key, int x, int y);
void display();
void draw_triangle();

// Modify the functions below
void draw_triangles();      // Exercise 1
void draw_motionblur();     // Exercise 2

int current_scene_id = 0;

// ----------------------------------------------- //
/**
 * Program entry point
 */
int main(int argc, char* argv[]) {
    // Initialize GLUT
    glutInit(&argc, argv);
    // Request a double-buffered, RGB display mode.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA |GLUT_DEPTH);
    // Specify size and position of a window and create it
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(30,30);
    glutCreateWindow("Blending");
    glewInit();

    // Register handlers
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc((void(*)())glutPostRedisplay);

    // Run the main event loop. To leave it, call glutLeaveMainLoop from a handler.
    glutMainLoop();
    return 0;
}

// ----------------------------------------------- //
void keyboard(unsigned char key, int x, int y) {
    if (key == 13 || key == ' ') current_scene_id = 1-current_scene_id;
    else if (key == 27) glutLeaveMainLoop();
}

// ----------------------------------------------- //
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    if(current_scene_id == 0) draw_triangles();
    else draw_motionblur();
    glutSwapBuffers();
}

void draw_triangles() {
    // Draw three triangles one on top of each other, rotating with arbitrary speeds
    float t = glutGet(GLUT_ELAPSED_TIME);

    glLoadIdentity();
    glRotatef(t*0.01, 0, 0, 1);
    glColor3f(1, 0, 0);
    draw_triangle();

    glLoadIdentity();
    glRotatef(t*0.03, 0, 0, 1);
    glColor3f(0, 1, 0);
    draw_triangle();

    glLoadIdentity();
    glRotatef(t*0.05, 0, 0, 1);
    glColor3f(0, 0, 1);
    // This triangle is written out explicitly here so that you could
    // see what happens if you assign different alpha values to different
    // vertices
    glBegin(GL_TRIANGLES);
        glVertex2f(0, 0.5);
        glVertex2f(0.5, 0);
        glVertex2f(-0.5, -0.5);
    glEnd();
}

void draw_motionblur() {
    // Use blending to blend together 20 different copies of the same triangle
    // rendered with small shifts in position.
    // Each copy has to be weighed by 1/20.
    // Hint: you will need to use glBlendColor
    glLoadIdentity();
    glColor3f(1, 0, 0);
    for (int i = 0; i < 20; i++) {
        glTranslatef(-0.01, 0, 0);
        draw_triangle();
    }
}

void draw_triangle() {
    glBegin(GL_TRIANGLES);
        glVertex2f(0, 0.5);
        glVertex2f(0.5, 0);
        glVertex2f(-0.5, -0.5);
    glEnd();
}
