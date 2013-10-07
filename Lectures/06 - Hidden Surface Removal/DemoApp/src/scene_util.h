/**
 * MTAT 03.015 Computer Graphics.
 * Helper illustrations for Lecture 05.
 *
 * Utility functions. Some taken from FreeGLUT.
 *
 * Copyright 2013, Konstantin Tretyakov.
 */
#include <GL/freeglut.h>

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
    friend ostream& operator<< (ostream& out, vector3f& v) {
        out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return out;
    }
};

// Taken verbatim from FreeGLUT
static void fghCircleTable(double **sint,double **cost,const int n)
{
    int i;

    /* Table size, the sign of n flips the circle direction */

    const int size = abs(n);

    /* Determine the angle between samples */

    const double angle = 2*M_PI/(double)( ( n == 0 ) ? 1 : n );

    /* Allocate memory for n samples, plus duplicate of first entry at the end */

    *sint = (double *) calloc(sizeof(double), size+1);
    *cost = (double *) calloc(sizeof(double), size+1);

    /* Bail out if memory allocation fails, fgError never returns */

    if (!(*sint) || !(*cost))
    {
        free(*sint);
        free(*cost);
        //fgError("Failed to allocate memory in fghCircleTable");
    }

    /* Compute cos and sin around the circle */

    (*sint)[0] = 0.0;
    (*cost)[0] = 1.0;

    for (i=1; i<size; i++)
    {
        (*sint)[i] = sin(angle*i);
        (*cost)[i] = cos(angle*i);
    }

    /* Last sample is duplicate of the first */

    (*sint)[size] = (*sint)[0];
    (*cost)[size] = (*cost)[0];
}


// Taken from FreeGLUT, modified.
/*
 * Draws a solid sphere
 */
void FGAPIENTRY glutSolidSphereX(GLdouble radius, GLint slices, GLint stacks, int num_elements)
{
    int i,j;

    /* Adjust z and radius as stacks are drawn. */

    double z0,z1;
    double r0,r1;

    /* Pre-computed circle */

    double *sint1,*cost1;
    double *sint2,*cost2;


    fghCircleTable(&sint1,&cost1,-slices);
    fghCircleTable(&sint2,&cost2,stacks*2);

    /* The top stack is covered with a triangle fan */

    z0 = 1.0;
    z1 = cost2[(stacks>0)?1:0];
    r0 = 0.0;
    r1 = sint2[(stacks>0)?1:0];

    glBegin(GL_TRIANGLE_FAN);

        glNormal3d(0,0,1);
        glVertex3d(0,0,radius);

        for (j=slices; j>=0; j--)
        {
            if (!num_elements) break;
            glNormal3d(cost1[j]*r1,        sint1[j]*r1,        z1       );
            glVertex3d(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);
            num_elements--;
        }
    glEnd();

    /* Cover each stack with a quad strip, except the top and bottom stacks */

    for( i=1; i<stacks-1; i++ )
    {
        if (!num_elements) break;
        z0 = z1; z1 = cost2[i+1];
        r0 = r1; r1 = sint2[i+1];

        glBegin(GL_QUAD_STRIP);

            for(j=0; j<=slices; j++)
            {
                if (!num_elements) break;
                glNormal3d(cost1[j]*r1,        sint1[j]*r1,        z1       );
                glVertex3d(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);
                glNormal3d(cost1[j]*r0,        sint1[j]*r0,        z0       );
                glVertex3d(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);
                num_elements--;
            }

        glEnd();
    }

    /* The bottom stack is covered with a triangle fan */

    z0 = z1;
    r0 = r1;
    glBegin(GL_TRIANGLE_FAN);

        glNormal3d(0,0,-1);
        glVertex3d(0,0,-radius);

        for (j=0; j<=slices; j++)
        {
            if (!num_elements) break;
            glNormal3d(cost1[j]*r0,        sint1[j]*r0,        z0       );
            glVertex3d(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);
            num_elements--;
        }

    glEnd();

    /* Release sin and cos tables */

    free(sint1);
    free(cost1);
    free(sint2);
    free(cost2);
}


// From FreeGLUT, modified
/*
 * Draws a solid cylinder
 */
void FGAPIENTRY glutSolidCylinderX(GLdouble radius, GLdouble height, GLint slices, GLint stacks, int num_elements)
{
    int i,j;

    /* Step in z and radius as stacks are drawn. */

    double z0,z1;
    const double zStep = height / ( ( stacks > 0 ) ? stacks : 1 );

    /* Pre-computed circle */

    double *sint,*cost;

    fghCircleTable(&sint,&cost,-slices);

    /* Cover the base and top */

    glBegin(GL_TRIANGLE_FAN);
        glNormal3d(0.0, 0.0, -1.0 );
        glVertex3d(0.0, 0.0,  0.0 );
        for (j=0; j<=slices; j++) {
            if (!num_elements) break;
            glVertex3d(cost[j]*radius, sint[j]*radius, 0.0);
            num_elements--;
        }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        glNormal3d(0.0, 0.0, 1.0   );
        glVertex3d(0.0, 0.0, height);
        for (j=slices; j>=0; j--) {
            if (!num_elements) break;
            glVertex3d(cost[j]*radius, sint[j]*radius, height);
            num_elements--;
        }
    glEnd();

    /* Do the stacks */

    z0 = 0.0;
    z1 = zStep;

    for (i=1; i<=stacks; i++)
    {
        if (i==stacks)
            z1 = height;

        glBegin(GL_QUAD_STRIP);
            for (j=0; j<=slices; j++ )
            {
                if (!num_elements) break;
                glNormal3d(cost[j],        sint[j],        0.0 );
                glVertex3d(cost[j]*radius, sint[j]*radius, z0  );
                glVertex3d(cost[j]*radius, sint[j]*radius, z1  );
                num_elements--;
            }
        glEnd();

        z0 = z1; z1 += zStep;
    }

    /* Release sin and cos tables */

    free(sint);
    free(cost);
}

// Draws the scene (in the camera frame)
void draw_scene(int num_elements, bool filled=false) {
    if (!filled) glDisable(GL_LIGHTING);
    if (num_elements < 0) num_elements = 1000;
    glPushMatrix();
        gluLookAt(0, -5, 0, 0, 0, 0, 0, 0, 1);
        if (!filled) glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        glutSolidSphereX(1, 10, 10, num_elements);
        glPushMatrix();
            glTranslatef(0, 0, -1-sqrt(1-0.7*0.7));
            glutSolidCylinderX(0.7, 1, 20, 3, max(0, num_elements-100));
        glPopMatrix();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

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

