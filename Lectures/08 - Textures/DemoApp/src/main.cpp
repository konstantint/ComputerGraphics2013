/**
 * MTAT.03.015 Computer Graphics.
 * Lecture 8 visualization app.
 *
 * Copyright 2013, Konstantin Tretyakov
 */

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
using namespace std;

float ASPECT_RATIO = 1.0; // Ew, don't ask why it's here before #include "scenes.h"
float DOF_Y = 45;
#include "scenes.h"

// Define some constants we shall need later
#define GLUT_KEY_ESCAPE 27  // The keycode for the escape key.
#define GLUT_KEY_ENTER 13   // The keycode for "enter" key.
#define GLUT_KEY_BACKSPACE 0x08

// --------------- Forward declarations ------------- //
int main(int argc, char* argv[]);
void keyboard(unsigned char key, int x, int y);
void display();
void idle();

SceneSet scenes;

// ----------------------------------------------- //
int main(int argc, char* argv[]) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitContextVersion(2, 1);
    // Request a double-buffered, RGB display mode.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    // Specify size and position of a window and create it
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("Visualization");
    glutFullScreen();
    ASPECT_RATIO = (float)glutGet(GLUT_WINDOW_WIDTH)/glutGet(GLUT_WINDOW_HEIGHT);

    // Initialize GL extensions
    glewInit();
    if (!GLEW_VERSION_2_0) {
        cerr << "OpenGL 2.0 not available\n";
        return 1;
    }

    // Register handlers
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    // Lighting and depth testing for non-wireframe scene
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);   // Line antialiasing
    glEnable(GL_POLYGON_SMOOTH);// Polygon antialiasing
    glEnable(GL_BLEND);         // Antialiasing works only with blending enabled
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.1);

    scenes.push(new EmptyScene());
    scenes.push(new TorusScene(0, "Phong"));
    scenes.push(new TorusScene(1, "Blinn"));
    scenes.push(new TorusScene(2, "Gauss"));
    scenes.push(new TorusScene(3, "Linear fog"));
    scenes.push(new TorusScene(4, "Exponential fog"));
    scenes.push(new TorusScene(5, "Exponential quadratic fog"));
    scenes.push(new TorusScene(6, "Exponential quadratic fog"));
    scenes.push(new TextureScene(0, "Incorrect interpolation"));
    scenes.push(new TextureScene(1, "Perspective-correct interpolation"));
    scenes.push(new TextureCheckerboardScene(GL_NEAREST, "GL_NEAREST filtering"));
    scenes.push(new TextureCheckerboardScene(GL_LINEAR, "GL_LINEAR filtering"));
    scenes.push(new TextureCheckerboardScene(GL_NEAREST_MIPMAP_LINEAR, "GL_LINEAR_MIPMAP_NEAREST filtering"));
    scenes.push(new TextureCheckerboardScene(GL_LINEAR_MIPMAP_LINEAR, "GL_LINEAR_MIPMAP_LINEAR filtering"));
    scenes.push(new TextureCheckerboardScene(GL_TEXTURE_MAX_ANISOTROPY_EXT, "GL_TEXTURE_MAX_ANISOTROPY_EXT=16"));
    scenes.push(new TextureCheckerboardScene(GAMMA_CORRECT, "Anisotropic mipmaps + Gamma correction"));
    scenes.push(new TorusScene(7, "Bump-mapping"));
    scenes.push(new SphereMapScene());
    scenes.push(new ShadowScene());

    scenes.current()->start();

    // Run the event loop
    glutMainLoop();

    for (unsigned int i = 0; i < scenes.size(); i++) delete scenes[i];
    return 0;
}

// ----------------------------------------------- //
void keyboard(unsigned char key, int x, int y) {
    if (key == GLUT_KEY_ESCAPE)
        glutLeaveMainLoop();
    else if (key == GLUT_KEY_ENTER || key == ' ')
        scenes.next();
    else if (key == GLUT_KEY_BACKSPACE)
        scenes.prev();
    else scenes.current()->start();

}

// ----------------------------------------------- //
void display() {
    scenes.current()->draw();
    glutSwapBuffers();
}

// ----------------------------------------------- //
void idle() {
    scenes.current()->idle();
}
