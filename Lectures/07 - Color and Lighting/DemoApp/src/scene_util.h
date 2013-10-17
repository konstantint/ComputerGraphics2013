/**
 * MTAT 03.015 Computer Graphics.
 * Helper illustrations for Lecture 7.
 *
 * Utility functions.
 *
 * Copyright 2013, Konstantin Tretyakov.
 */
#include <GL/freeglut.h>
#include <vector>
#define M_PI           3.14159265358979323846
using std::vector;

// Commenting and organizing code is for the weak!

typedef void (*function_t)();

struct vector3f {
    float x, y, z;
    vector3f (): x(0), y(0), z(0) {};
    vector3f (float x, float y): x(x), y(y), z(0) {};
    vector3f (float x, float y, float z): x(x), y(y), z(z) {};
    vector3f operator+ (vector3f v) {
        return vector3f (x + v.x, y + v.y, z + v.z);
    }
    vector3f operator+= (vector3f v) {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
    vector3f operator-= (vector3f v) {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }
    vector3f operator- (vector3f v) {
        return vector3f (x - v.x, y - v.y, z - v.z);
    }
    vector3f operator* (float f) {
        return vector3f (x * f, y * f, z * f);
    }
    vector3f operator/ (float f) {
        return vector3f (x / f, y / f, z / f);
    }
    void apply(float* matrix4x4, float w=1) { // Transform the vector in-place using a homogeneous matrix
        float result[3];
        for (int i = 0; i < 3; i++) {
            result[i] = x*matrix4x4[i] + y*matrix4x4[i+4] + z*matrix4x4[i+8] + w*matrix4x4[i+12];
        }
        x = result[0]; y = result[1]; z = result[2];
    }
    friend ostream& operator<< (ostream& out, vector3f& v) {
        out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return out;
    }
};

// Draws a "camera" at center of coordinates watching towards negative z.
void draw_camera() {
    glPushAttrib(GL_CURRENT_BIT);
    glPushMatrix();
            glScalef(0.2, 0.2, 0.2);
            glTranslatef(0, 0, 5);
    glPushMatrix();
        glColor3f(0.05, 0.05, 0.1);
        glScalef(1, 2, 3);
        glutSolidCube(2);
    glPopMatrix();
    glPushMatrix();
        glColor3f(0.13, 0.1, 0.1);
        glTranslatef(0, 0, -2-3);
        glutSolidCylinder(1.5, 2, 20, 5);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0, 1.8, -1.3);
        glRotatef(90, 0, 1, 0);
        glTranslatef(0, 0, -1.5);
        glutSolidCylinder(1.3, 3, 20, 5);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0, 1.8, 1.3);
        glRotatef(90, 0, 1, 0);
        glTranslatef(0, 0, -1.5);
        glutSolidCylinder(1.3, 3, 20, 5);
    glPopMatrix();

    glPopMatrix();
    glPopAttrib();
}

// The projection used to view the scene
void setup_scene_projection() {
    gluPerspective(DOF_Y, ASPECT_RATIO, 3, 7);
}

// This is what we demonstrate in the visualization.
// It is a lie, but a tiny one.
void setup_scene_projection_flipped() {
    glScalef(1, 1, -1);
    setup_scene_projection();
}

// Render the clip volume for the given projection matrix.
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

// Interpolates between two matrices
void interpolated_matrix_v(const double* first, const double* second, float slider) {
    GLdouble intermediate[16];
    for (int i = 0; i < 16; i++) intermediate[i] = first[i]*(1-slider) + second[i]*slider;
    glMultMatrixd(intermediate);
}

// Same as above, but matrices are given via functions
void interpolated_matrix(function_t setup_first_matrix, function_t setup_second_matrix, float slider) {
    GLdouble first[16];
    GLdouble second[16];
    glPushMatrix();
        glLoadIdentity();
        setup_first_matrix();
        glGetDoublev(GL_MODELVIEW_MATRIX, first);
    glPopMatrix();
    glPushMatrix();
        glLoadIdentity();
        setup_second_matrix();
        glGetDoublev(GL_MODELVIEW_MATRIX, second);
    glPopMatrix();
    interpolated_matrix_v(first, second, slider);
}

// Draws short string at the upper right corner.
void draw_label(float x, float y, const char* text) {
    glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
    glDisable(GL_LIGHTING);
    glRasterPos2f(x, y);
    glColor3f(1, 1, 1);
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)text);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
        glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
