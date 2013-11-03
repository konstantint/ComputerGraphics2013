/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 9: Shadows
 *
 * Helper routines.
 */
#include <vector>
using std::vector;

// You should not change this file.

// ----------------- Objects used in the scene ----------------- //
/**
 * The following is a structure storing the decription of a cube centered at (x, y, z) with side length size.
 * Note that although we can *render* the cube using a simple glutSolidCube,
 * we need some additional logic to be able to render shadow volumes in the Stencil shadow algorithm.
 * This is the reason we encapsulate the "cube" object into this structure.
 * Usage:
 *   Cube c = Cube(size, x, y, z);
 *   c.draw(); // Renders the cube
 *   c.draw_shadow_volume(light); // Renders the shadow volume of the cube wrt given light source position.
 */
struct Cube {
    float size, x, y, z;

    // Create a cube of given size with its center positioned at dx, dy, dz
    Cube(float size, float x, float y, float z):
        size(size), x(x), y(y), z(z) {};

    // Render the cube
    void draw();

    // Render the shadow volume for the cube wrt light pos
    void draw_shadow_volume(float light_pos[]);
};

// ------------------- Scene geometry ----------------- //
// Configures OpenGL light source parameters. Invoked in the initialization only.
void configure_lighting();

// Creates a vector of two light source position vectors, given the current time t.
// Also invokes glLightfv(GL_LIGHTx, GL_POSITION, .) with these values to position the light sources.
vector<float*> set_light_positions(float t);

// Create a vector of two "cube" objects, positioned as needed for current time t.
vector<Cube> create_objects(float t);

// ------------------- Rendering of scene geometry ----------------- //
// Renders small spheres denoting positions of the light sources
void draw_lights(vector<float*>);

// Renders the "walls" in the scene (i.e. two walls and the floor).
void draw_walls();


// ----------------- Projection-shadows ----------------- //
/**
 * Multiplies the current matrix with the "shadow projection matrix".
 * Parameters:
 *     ground: {A, B, C, D} coefficients of the plane equation to which the projection is performed.
 *     light:  {x, y, z, 1} light source position.
 *     offset: amount by how much the shadow will be offset from the ground (to avoid depth buffer problems)
 */
void shadowMatrix(float light[4], float ground[4], float offset = 0.0);


