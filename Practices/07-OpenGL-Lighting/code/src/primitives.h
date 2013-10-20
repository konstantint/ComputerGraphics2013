/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 7: Lighting
 *
 * Primitive drawing routines.
 * Do not change them.
 */

// From the point of view of C programming it is of course more conventional
// to have the declarations in a .h file and definitions in .cpp
// However, to simplify things we'll put the full definitions in the .h
//
// It is OK if you do not include the header file anywhere more than once.


void draw_cube(float t) {
    glPushMatrix();
        glRotatef(t*0.1, 0, 0, 1);
        glRotatef(atan2(1, sqrt(2))*180/M_PI, 0, 1, 0);
        glRotatef(45, 1, 0, 0);

        glutSolidCube(0.4);

        // For the fun of it, let's also draw a "halo" around the cube
        // by blending half-transparent larger versions of the cube on top

        // Save current blending, back-face culling, lighting & depth buffer setup
        glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_CULL_FACE); // Must cull back faces to avoid double-drawing of some pixels
        glDepthMask(GL_FALSE); // Don't touch the depth buffer
        glDisable(GL_LIGHTING); // Draw fixed-color
        int cur_program;
        glGetIntegerv(GL_CURRENT_PROGRAM, &cur_program);
        if (glGetUniformLocation(cur_program, "disable_lighting") != -1) {
            glUniform1i(glGetUniformLocation(cur_program, "disable_lighting"), 1);
        }
        glBlendFunc(GL_ONE, GL_ONE); // Set to simple additive blending
        glColor3f(0.1, 0, 0);

        // Cover the current cube pixels with a stencil
        glEnable(GL_STENCIL_TEST);
        glClear(GL_STENCIL_BUFFER_BIT);
        glStencilFunc(GL_ALWAYS, 1, 1);
        glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glutSolidCube(0.45);
        glStencilFunc(GL_NOTEQUAL, 1, 1);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        // Draw several copies of the larger cube one on top of the other
        for (int i = 1; i < 9; i++) {
            glutSolidCube(0.5 + (float)i/20);
        }
        // Restore settings
        glPopAttrib();
        if (glGetUniformLocation(cur_program, "disable_lighting") != -1) {
            glUniform1i(glGetUniformLocation(cur_program, "disable_lighting"), 0);
        }
        // If you actually cared read the code to this point and understood how the halo
        // effect was implemented, write the code "STENCILBONUS" in the comment field of
        // the homework submission and get 0.5 bonus points! Yay!
    glPopMatrix();
}

void draw_sphere(float t) {
    glPushMatrix();
        glTranslatef(0, 0, 0.2*sin(t*0.01));
        glutSolidSphere(0.5, 20, 20);
    glPopMatrix();
}


#include "random_surface.h"
RandomSurface ground(15, -15, 15, -15, 15, 0, 3);
void draw_ground(float t) {
    ground.draw();
}

