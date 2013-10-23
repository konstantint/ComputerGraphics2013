/**
 * MTAT 03.015 Computer Graphics.
 * Utility functions.
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

// Draws short string at the upper right corner.
void draw_label(float x, float y, const char* text) {
    glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glRasterPos2f(x, y);
    glColor3f(1, 1, 1);
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)text);
    glPopAttrib();
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
        glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
