/**
 * MTAT.03.015 Computer Graphics.
 * Lecture 5 visualization app.
 *
 * Copyright 2013, Konstantin Tretyakov
 */

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
using namespace std;

#include "glu_invert_matrix.h"

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
    // Request a double-buffered, RGB display mode.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    // Specify size and position of a window and create it
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("Visualization");
    glutFullScreen();
    ASPECT_RATIO = (float)glutGet(GLUT_SCREEN_WIDTH)/glutGet(GLUT_SCREEN_HEIGHT);

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
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LINE_SMOOTH);   // Line antialiasing
    glEnable(GL_POLYGON_SMOOTH);// Polygon antialiasing
    glEnable(GL_BLEND);         // Antialiasing works only with blending enabled
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.1);

    scenes.push(new EmptyScene());
    scenes.push(new ConstructObjectScene());
    scenes.push(new ModelViewTransformScene());
    scenes.push(new FrustumScene());
    scenes.push(new ProjectionTransformScene());
    scenes.push(new MoveClipSpaceIntoViewScene());
    scenes.push(new PolygonFillScene("Filling triangles with solid color", false, false));
    scenes.push(new LambertScene(false));
    scenes.push(new LambertScene(true));
    scenes.push(new ShaderScene("../src/diffuse_vertex_shader.glsl", "../src/base_fragment_shader.glsl", "Lambert diffusion with gamma correction"));
    scenes.push(new ShaderScene("../src/ambientdiffuse_vertex_shader.glsl", "../src/base_fragment_shader.glsl", "Ambient + Diffuse terms"));
    scenes.push(new PhongScene());
    scenes.push(new ShaderScene("../src/phong_vertex_shader.glsl", "../src/phong_fragment_shader.glsl", "Phong lighting (per-fragment, no gamma correction)"));
    scenes.push(new ShaderScene("../src/phong_vertex_shader.glsl", "../src/phong_fragment_shader_gc.glsl", "Phong lighting (per-fragment, gamma-corrected)"));
    scenes.push(new ShaderScene("../src/phong_vertex_shader.glsl", "../src/phong_fragment_shader_hdr.glsl", "Phong lighting (per-fragment, gamma-corrected, HDR)"));
    scenes.push(new ShaderScene("../src/phong_vertex_shader.glsl", "../src/blinn_fragment_shader_hdr.glsl", "Blinn lighting (per-fragment, gamma-corrected, HDR)"));
    scenes.push(new SpotScene());

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
