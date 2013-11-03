/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 9: Shadows
 *
 * Procedures for rendering the constituent parts for the scene.
 */

// You should not change this file.
#include "scene_primitives.h"
#include "GL/glut.h"
#include <glm/glm.hpp>
#include <math.h>
#include <iostream>
using namespace std;

float white[] = {1.0, 1.0, 1.0, 1.0};

// ----------------- Materials used in the scene ----------------- //
void floor_material() {
    float c[] = {1.0, 1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, c);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialf(GL_FRONT, GL_SHININESS, 25.0);
    glColor3fv(c); // For debugging without lighting.
}

void right_wall_material() {
    float c[] = {1.0, 1.0, 0.0, 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, c);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialf(GL_FRONT, GL_SHININESS, 25.0);
    glColor3fv(c);
}

void back_wall_material() {
    float c[] = {1.0, 0.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, c);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialf(GL_FRONT, GL_SHININESS, 25.0);
    glColor3fv(c);
}

void cube_material() {
    float c[] = {0.1, 0.9, 0.3, 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, c);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialf(GL_FRONT, GL_SHININESS, 25.0);
}

// ----------------- Objects used in the scene ----------------- //
// Configures OpenGL light source parameters. Invoked in the initialization only.
void configure_lighting() {
    glEnable(GL_LIGHTING);  // We are using a shader, so we don't care about those
    glEnable(GL_LIGHT0);    // enable calls, but in case we want to debug, this is useful
    glEnable(GL_LIGHT1);
    float ambient[] = { 0.1, 0.1, 0.1, 1.0 };
    float light0[] = {4, 4, 4, 1};
    float light1[] = {8, 8, 8, 1};
    float black[] = {0, 0, 0, 1};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient );
    glLightfv(GL_LIGHT0, GL_AMBIENT, black);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0);
    glLightfv(GL_LIGHT1, GL_AMBIENT, black);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 1.0);
}

// Creates a vector of two light source position vectors, given the current time t.
// Also invokes glLightfv(GL_LIGHTx, GL_POSITION, .) with these values to position the light sources.
float light_pos_1[4];
float light_pos_2[4];
vector<float*> set_light_positions(float t) {
    light_pos_1[0] = 2.0*sin(t);
    light_pos_1[1] = 2.0*cos(t);
    light_pos_1[2] = 3.0;
    light_pos_1[3] = 1.0;
    light_pos_2[0] = -3.0;
    light_pos_2[1] = -3.0;
    light_pos_2[2] = 3.0;
    light_pos_2[3] = 1.0;
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos_1);
    glLightfv(GL_LIGHT1, GL_POSITION, light_pos_2);
    vector<float*> result;
    result.push_back(light_pos_1);
    result.push_back(light_pos_2);
    return result;
}

// Create a vector of two "cube" objects, positioned as needed for current time t.
vector<Cube> create_objects(float t) {
    vector<Cube> objects;
    // The "standing cube"
    objects.push_back(Cube(2, 2, 2, 1));
    // The "jumping cube"
    float j = fmod(t, 1.0);
    j = 3*4*j*(1-j);
    objects.push_back(Cube(0.5, -0.2, -0.2, 0.25 + j));
    return objects;
}

// ------------------- Rendering of scene geometry ----------------- //

// Renders a [-1, 1] x [-1, 1] square in the z=0 axis.
void draw_square() {
    glNormal3f(0, 0, 1);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(-1, -1);
        glVertex2f(1, -1);
        glVertex2f(1, 1);
        glVertex2f(-1, 1);
    glEnd();
}


// Renders small spheres denoting positions of the light sources
void draw_lights(vector<float*> lights) {
    glColor3f(1, 1, 1);
    glPushMatrix();
        glTranslatef(lights[0][0], lights[0][1], lights[0][2]);
        glutSolidSphere(0.1, 5, 5);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(lights[1][0], lights[1][1], lights[1][2]);
        glutSolidSphere(0.1, 5, 5);
    glPopMatrix();
}


// Renders the "walls" in the scene (i.e. two walls and the floor).
void draw_walls() {
    // Floor: [-4, 4] x [-4, 4] rect on the z=0 plane.
    floor_material();
        glPushMatrix();
            glScalef(4, 4, 4);
            draw_square();
        glPopMatrix();

    // Right wall: [-4, 4] x [-4, 4] rect on the x=4 plane
    right_wall_material();
        glPushMatrix();
            glTranslatef(4, 0, 4);
            glRotatef(-90, 0, 1, 0);
            glScalef(4, 4, 4);
            draw_square();
        glPopMatrix();

    // Back wall: [-4, 4] x [-4, 4] rect on the y=4 plane
    back_wall_material();
        glPushMatrix();
            glTranslatef(0, 4, 4);
            glRotatef(90, 1, 0, 0);
            glScalef(4, 4, 4);
            draw_square();
        glPopMatrix();
}


// ----------------- Cube class ----------------- //
#define NUM_CUBE_VERTICES 8
const float CUBE_VERTICES[][3] =
    {{-1, -1, -1},  {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},  // Bottom four
     {-1, -1,  1},  {1, -1,  1}, {1, 1,  1}, {-1, 1,  1}}; // Top four
#define NUM_CUBE_EDGES 12
const int CUBE_EDGES[][2] = // (vertex0, vertex1)
    {{0, 1}, {1, 2}, {2, 3}, {3, 0},
     {4, 5}, {5, 6}, {6, 7}, {7, 4},
     {0, 4}, {1, 5}, {2, 6}, {3, 7}};
#define NUM_CUBE_FACES 6
const int CUBE_FACES[][4] = // (vertex0, vertex1, vertex2, vertex3)
    {{0, 3, 2, 1}, {4, 5, 6, 7},  // BOTTOM, TOP
     {0, 1, 5, 4}, {2, 3, 7, 6},  // FRONT, BACK
     {1, 2, 6, 5}, {3, 0, 4, 7}}; // RIGHT, LEFT
#define BOTTOM 0
#define TOP 1
#define FRONT 2
#define BACK 3
#define RIGHT 4
#define LEFT 5
const float FACE_NORMALS[][3] =
    {{0, 0, -1}, {0, 0, 1},
     {0, -1, 0}, {0, 1, 0},
     {1, 0, 0}, {-1, 0, 0}};
const float EDGE_TO_FACES[][2] = // (face0, face1)
    {{BOTTOM, FRONT}, {BOTTOM, RIGHT}, {BOTTOM, BACK}, {BOTTOM, LEFT},
     {TOP, FRONT}, {TOP, RIGHT}, {TOP, BACK}, {TOP, LEFT},
     {LEFT, FRONT}, {RIGHT, FRONT}, {RIGHT, BACK}, {BACK, LEFT}};

void Cube::draw() {
    cube_material();
    glPushMatrix();
        glTranslatef(x, y, z);
        glutSolidCube(size);
    glPopMatrix();
}

void Cube::draw_shadow_volume(float light_pos[]) {
    glm::vec3 p_light = glm::vec3(light_pos[0], light_pos[1], light_pos[2]);
    glm::vec3 center = glm::vec3(x, y, z);

    // Find all edges that connect two faces, such that one face looks towards the light,
    // and another looks away from it.
    for (int i = 0; i < NUM_CUBE_EDGES; i++) {
        int v1 = CUBE_EDGES[i][0], v2 = CUBE_EDGES[i][1];
        int f1 = EDGE_TO_FACES[i][0], f2 = EDGE_TO_FACES[i][1];
        glm::vec3 vertex1 = glm::vec3(CUBE_VERTICES[v1][0], CUBE_VERTICES[v1][1], CUBE_VERTICES[v1][2]);
        glm::vec3 vertex2 = glm::vec3(CUBE_VERTICES[v2][0], CUBE_VERTICES[v2][1], CUBE_VERTICES[v2][2]);
        vertex1 *= size/2;
        vertex2 *= size/2;
        glm::vec3 edge_midpoint = vertex1 + vertex2;
        edge_midpoint *= 0.5;
        edge_midpoint += center;
        glm::vec3 v_light = p_light - edge_midpoint;
        glm::vec3 normal1 = glm::vec3(FACE_NORMALS[f1][0], FACE_NORMALS[f1][1], FACE_NORMALS[f1][2]);
        glm::vec3 normal2 = glm::vec3(FACE_NORMALS[f2][0], FACE_NORMALS[f2][1], FACE_NORMALS[f2][2]);
        if (glm::dot(v_light, normal1) * glm::dot(v_light, normal2) < 0) {
            // Found a boundary edge. Extrude it to infinity and render as a quad
            vertex1 += center;
            vertex2 += center;
            // Detect the order of vertices. We want cross((v1 - light) x (v2 - light)) to be in the same direction
            // as the normal
            if (glm::dot(glm::cross(vertex1 - p_light, vertex2 - p_light), normal1) < 0) {
                glm::vec3 t = vertex1;
                vertex1 = vertex2;
                vertex2 = t;
            }
            glBegin(GL_QUADS);
                glVertex3f(vertex2.x, vertex2.y, vertex2.z);
                glVertex3f(vertex1.x, vertex1.y, vertex1.z);
                // Two vertices at infinity
                vertex2 -= p_light;
                vertex1 -= p_light;
                glVertex4f(vertex1.x, vertex1.y, vertex1.z, 0);
                glVertex4f(vertex2.x, vertex2.y, vertex2.z, 0);
            glEnd();
        }
    }

    // Note that in some cases (e.g. for the Depth Fail approach) you also need to render the
    // "front" and "back caps" of the shadow volumes. This is not done here (as it is not needed in our case).
}


// ----------------- Shadow-related routines ----------------- //
/**
 * Multiplies the current matrix with the "shadow projection matrix".
 * Parameters:
 *     ground: {A, B, C, D} coefficients of the plane equation to which the projection is performed.
 *     light:  {x, y, z, 1} light source position.
 *     offset: amount by how much the shadow will be offset from the ground (to avoid depth buffer problems)
 */
void shadowMatrix(float light[4], float ground[4], float offset) {
    // This is a slightly modified version of
    // http://www.bluevoid.com/opengl/sig00/advanced00/notes/node199.html, modified slightly
    float  dot;
    float  shadowMat[4][4];

    dot = ground[0] * light[0] +
          ground[1] * light[1] +
          ground[2] * light[2] +
          ground[3] * light[3];

    shadowMat[0][0] = dot - light[0] * ground[0];
    shadowMat[1][0] = 0.0 - light[0] * ground[1];
    shadowMat[2][0] = 0.0 - light[0] * ground[2];
    shadowMat[3][0] = 0.0 - light[0] * ground[3];

    shadowMat[0][1] = 0.0 - light[1] * ground[0];
    shadowMat[1][1] = dot - light[1] * ground[1];
    shadowMat[2][1] = 0.0 - light[1] * ground[2];
    shadowMat[3][1] = 0.0 - light[1] * ground[3];

    shadowMat[0][2] = 0.0 - light[2] * ground[0];
    shadowMat[1][2] = 0.0 - light[2] * ground[1];
    shadowMat[2][2] = dot - light[2] * ground[2];
    shadowMat[3][2] = 0.0 - light[2] * ground[3];

    shadowMat[0][3] = 0.0 - light[3] * ground[0];
    shadowMat[1][3] = 0.0 - light[3] * ground[1];
    shadowMat[2][3] = 0.0 - light[3] * ground[2];
    shadowMat[3][3] = dot - light[3] * ground[3];

    // Apply offset
    float k = sqrt(ground[0]*ground[0] + ground[1]*ground[1] + ground[2]*ground[2]);
    glTranslatef(ground[0]*offset/k, ground[1]*offset/k, ground[2]*offset/k);

    // Multiply by the shadow matrix
    glMultMatrixf((const GLfloat*)shadowMat);
}
