/**
 * MTAT 03.015 Computer Graphics.
 * Helper illustrations for Lecture 7.
 *
 * Creates the scene as an array of triangles.
 *
 * Copyright 2013, Konstantin Tretyakov.
 */

// Just a bunch of copy-pasting with changing the kind of outputs.
#include <GL/freeglut.h>
#include <vector>
#include <iostream>
using namespace std;

struct Triangle {
    vector3f a, b, c;
    vector3f na, nb, nc;
    Triangle(vector3f a, vector3f b, vector3f c, vector3f normal): a(a), b(b), c(c), na(normal), nb(normal), nc(normal) {
        init();
    }
    Triangle(vector3f a, vector3f b, vector3f c, vector3f na, vector3f nb, vector3f nc): a(a), b(b), c(c), na(na), nb(nb), nc(nc) {
        init();
    }
    void init() {
        float mv[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, mv);
        a.apply(mv, 1); b.apply(mv, 1); c.apply(mv, 1); na.apply(mv, 0); nb.apply(mv, 0); nc.apply(mv, 0);
    }
    void render() {
        glNormal3f(na.x, na.y, na.z);
        glVertex3f(a.x, a.y, a.z);
        glNormal3f(nb.x, nb.y, nb.z);
        glVertex3f(b.x, b.y, b.z);
        glNormal3f(nc.x, nc.y, nc.z);
        glVertex3f(c.x, c.y, c.z);
    }
    friend ostream& operator<< (ostream& out, Triangle& t) {
        out << "Triangle {\n";
        out << "  A:" << t.a << " NA:" << t.na << "\n";
        out << "  B:" << t.b << " NB:" << t.nb << "\n";
        out << "  C:" << t.c << " NC:" << t.nc << "\n";
        out << "}";
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
vector<Triangle> FGAPIENTRY makeSolidSphere(GLdouble radius, GLint slices, GLint stacks)
{
    vector<Triangle> result;
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

    //glBegin(GL_TRIANGLE_FAN);

        vector3f na(0,0,1);
        vector3f a(0,0,radius);
        vector3f nb(cost1[slices]*r1,        sint1[slices]*r1,        z1       );
        vector3f b(cost1[slices]*r1*radius, sint1[slices]*r1*radius, z1*radius);

        for (j=slices-1; j>=0; j--)
        {
            vector3f nc(cost1[j]*r1,        sint1[j]*r1,        z1       );
            vector3f c(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);
            result.push_back(Triangle(a,b,c,na,nb,nc));
            b = c; nb = nc;
        }
    //glEnd();

    /* Cover each stack with a quad strip, except the top and bottom stacks */

    for( i=1; i<stacks-1; i++ )
    {
        z0 = z1; z1 = cost2[i+1];
        r0 = r1; r1 = sint2[i+1];

        //glBegin(GL_QUAD_STRIP);
            j = 0;
            vector3f na(cost1[j]*r1,        sint1[j]*r1,        z1       );
            vector3f a(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);
            vector3f nb(cost1[j]*r0,        sint1[j]*r0,        z0       );
            vector3f b(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);

            for(j=1; j<=slices; j++)
            {
                vector3f nc(cost1[j]*r1,        sint1[j]*r1,        z1       );
                vector3f c(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);
                vector3f nd(cost1[j]*r0,        sint1[j]*r0,        z0       );
                vector3f d(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);
                result.push_back(Triangle(a,b,c,na,nb,nc));
                result.push_back(Triangle(b,d,c,nb,nd,nc));
                a = c; b = d; na = nc; nb = nd;
            }

        //glEnd();
    }

    /* The bottom stack is covered with a triangle fan */

    z0 = z1;
    r0 = r1;
    //glBegin(GL_TRIANGLE_FAN);
    {
        vector3f na(0,0,-1);
        vector3f a(0,0,-radius);
        vector3f nb(cost1[j]*r0,        sint1[j]*r0,        z0       );
        vector3f b(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);

        for (j=0; j<=slices; j++)
        {
            vector3f nc(cost1[j]*r0,        sint1[j]*r0,        z0       );
            vector3f c(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);
            result.push_back(Triangle(a,b,c,na,nb,nc));
            b = c; nb = nc;
        }
    }
    //glEnd();

    /* Release sin and cos tables */

    free(sint1);
    free(cost1);
    free(sint2);
    free(cost2);
    return result;
}


// From FreeGLUT, modified
/*
 * Generates a set of triangles describing a solid cylinder
 */
vector<Triangle> makeSolidCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks)
{
    vector<Triangle> result;
    int i,j;

    /* Step in z and radius as stacks are drawn. */

    double z0,z1;
    const double zStep = height / ( ( stacks > 0 ) ? stacks : 1 );

    /* Pre-computed circle */

    double *sint,*cost;

    fghCircleTable(&sint,&cost,-slices);

    /* Cover the base */
    //glBegin(GL_TRIANGLE_FAN);
    {
        vector3f normal(0, 0, -1);
        vector3f a(0, 0, 0);
        vector3f b(cost[0]*radius, sint[0]*radius, 0.0);
        for (j=1; j<=slices; j++) {
            vector3f c(cost[j]*radius, sint[j]*radius, 0.0);
            result.push_back(Triangle(a,b,c,normal));
            b = c;
        }
    }
    //glEnd();

    /* Do the stacks */

    z0 = 0.0;
    z1 = zStep;

    for (i=1; i<=stacks; i++)
    {
        if (i==stacks)
            z1 = height;

        //glBegin(GL_QUAD_STRIP);
            vector3f nprev(cost[0],        sint[0],        0.0 );
            vector3f a(cost[0]*radius, sint[0]*radius, z0  );
            vector3f b(cost[0]*radius, sint[0]*radius, z1  );
            for (j=1; j<=slices; j++ )
            {
                vector3f nnew(cost[j],        sint[j],        0.0 );
                vector3f c(cost[j]*radius, sint[j]*radius, z0  );
                vector3f d(cost[j]*radius, sint[j]*radius, z1  );
                result.push_back(Triangle(a,b,c,nprev,nprev,nnew));
                result.push_back(Triangle(c,b,d,nnew,nprev,nnew));
                a = c; b = d; nprev = nnew;
            }
        //glEnd();

        z0 = z1; z1 += zStep;
    }

    /* Cover the top */
    //glBegin(GL_TRIANGLE_FAN);
    {
        vector3f normal(0, 0, 1);
        vector3f a(0, 0, height);
        vector3f b(cost[slices]*radius, sint[slices]*radius, height);
        for (j=slices-1; j>=0; j--) {
            vector3f c(cost[j]*radius, sint[j]*radius, height);
            result.push_back(Triangle(a,b,c,normal));
            b = c;
        }
    }
    //glEnd();

    /* Release sin and cos tables */

    free(sint);
    free(cost);

    return result;
}

vector<Triangle> make_scene() {
    vector<Triangle> result;
    glPushMatrix();
        glLoadIdentity();
        gluLookAt(0, -5, 0, 0, 0, 0, 0, 0, 1);
        result = makeSolidSphere(1, 10, 5);
        glPushMatrix();
            glTranslatef(0, 0, -sqrt(1-0.7*0.7));
            glRotatef(180, 1, 0, 0);
            vector<Triangle> cyl = makeSolidCylinder(0.7, 1, 10, 2);
            result.insert(result.end(), cyl.begin(), cyl.end());
        glPopMatrix();
    glPopMatrix();
    return result;
}

void draw_triangles(vector<Triangle>& triangles, int num = -1, bool wireframe = false) {
    if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (wireframe) glDisable(GL_LIGHTING);
    glBegin(GL_TRIANGLES);
    for (vector<Triangle>::iterator i = triangles.begin(); i != triangles.end(); i++) {
        i->render();
        if(--num == 0) break;
    }
    glEnd();
    if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (wireframe) glEnable(GL_LIGHTING);
}
