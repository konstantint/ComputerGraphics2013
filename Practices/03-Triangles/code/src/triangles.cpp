/*
Write the solution to Exercise 6 here:


*/

// ---------------------------- Includes -------------------------- //

// Include for console logging.
#include <iostream>
using namespace std;

// Use mathematics routines
#include <math.h>

// Include Allegro headers.
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_image.h>

// ---------------------------- Global variables -------------------------- //
ALLEGRO_DISPLAY*    display;
ALLEGRO_EVENT_QUEUE* event_queue;
ALLEGRO_EVENT       event;
ALLEGRO_BITMAP*     texture;

// ---------------------------- Vector3f class -------------------------- //
/**
 * This is a simple three-dimensional vector structure, which lets you do
 * addition/subtraction of vectors as well as multiplication and division by constants.
 * Example:
 *   vector3f red(1.0, 0.0, 0.0);   // Red color
 *   vector3f green(0.0, 1.0, 0.0); // Blue color
 *   vector3f yellow = red * 0.5 + green * 0.5; // Mixture of red and green
 *   cout << yellow << endl;        // Debug output
 *   al_put_pixel(x, y, yellow.as_color()); // Use with Allegro
 */
struct vector3f {
    float x, y, z;
    vector3f (float x, float y): x(x), y(y), z(0) {};
    vector3f (float x, float y, float z): x(x), y(y), z(z) {};
    vector3f (ALLEGRO_COLOR color) {
        al_unmap_rgb_f(color, &x, &y, &z);
    }
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
    ALLEGRO_COLOR as_color() {
        float r = min(1.0f, max(0.0f, x));
        float g = min(1.0f, max(0.0f, y));
        float b = min(1.0f, max(0.0f, z));
        return al_map_rgb_f(r, g, b);
    }
    friend ostream& operator<< (ostream& out, vector3f& v) {
        out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return out;
    }
};

// ---------------------------- Forward declarations -------------------------- //
void init();
void deinit();
void event_loop();
void draw();


// You will need to implement those functions
void simple_triangle(int x1, int y1, int x2, int y2, int x3, int y3, ALLEGRO_COLOR color);
void color_triangle(int x1, int y1, int x2, int y2, int x3, int y3, vector3f c1, vector3f c2, vector3f c3);
void textured_triangle(int x1, int y1, int x2, int y2, int x3, int y3, vector3f uv1, vector3f uv2, vector3f uv3);

// Utility functions
void sort_triangle(int& x1, int& y1, int& x2, int& y2, int& x3, int& y3);
void sort_triangle_with_attributes(int& x1, int& y1, int& x2, int& y2, int& x3, int& y3, vector3f& c1, vector3f& c2, vector3f& c3);
void horizontal_line(float x1, float x2, int y, ALLEGRO_COLOR color);
void gradient_line(float x1, vector3f c1, float x2, vector3f c2, int y);

// ---------------------------- Main -------------------------- //
// The overall structure of our program is the familiar GUI event loop:

int main(){
    init();         // Initialize Allegro.
    event_loop();   // Run the event processing loop until a program is requested to quit
    deinit();       // Deinitialize

    return 0;       // This is simply required by the C++ standard to indicate successful completion
}


// ---------------------------- Allegro initialization -------------------------- //
// Initialize the Allegro system.

void init() {
    al_init();
    display = al_create_display(600, 600);
    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_install_keyboard();
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_init_primitives_addon();
    al_init_image_addon();
    texture = al_load_bitmap("../data/texture.png");
    if (!texture) cout << "Failed to load texture!" << endl;
    al_lock_bitmap(texture, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY); // Without this, al_get_pixel is very slow
}

// ---------------------------- Allegro deinitialization -------------------------- //
// Just the opposite of "init". Destroy whatever we created.

void deinit() {
    al_destroy_bitmap(texture);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
}

// ---------------------------- Event loop -------------------------- //

void event_loop() {
    while(1) {
        // We only need events to close the app when necessary
        bool have_event = al_wait_for_event_timed(event_queue, &event, 0.05);
        if (have_event) {
            if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || event.type == ALLEGRO_EVENT_KEY_DOWN) break;
        }
        // No animation this time, simply drawing
        draw();
   }
}

// ---------------------------- Drawing routines -------------------------- //
// Where all the drawing is performed.

void draw() {
    al_clear_to_color(al_map_rgb(0,0,0));

    // Here we test the implementations of the algorithms.
    // We shall draw four sample triangles for each method
    int NUM_TRIANGLES = 4;
    int triangles[][3][2] = { { { 75,  20}, { 25,  75}, {125, 130} },  // Default case
                              { {125,  75}, { 75,  20}, { 25, 130} },  // Non-sequential order of along Y axis, left and right edges flipped
                              { { 25,  75}, {125,  75}, { 75,  20} },  // Lacking bottom part
                              { { 25,  25}, {125,  25}, { 75, 80} },  // Lacking top part
                            };

    // Reference
    ALLEGRO_COLOR c = al_color_name("white");
    int OX = 0, OY = 0;
    for (int i = 0; i < NUM_TRIANGLES; i++) {
        al_draw_filled_triangle(triangles[i][0][0] + OX, triangles[i][0][1] + OY,
                                triangles[i][1][0] + OX, triangles[i][1][1] + OY,
                                triangles[i][2][0] + OX, triangles[i][2][1] + OY,
                                c);
        OY += 150;
    }

    // Simple triangle
    c = al_color_name("red");
    OX = 150;
    OY = 0;
    for (int i = 0; i < NUM_TRIANGLES; i++) {
        simple_triangle(triangles[i][0][0] + OX, triangles[i][0][1] + OY,
                        triangles[i][1][0] + OX, triangles[i][1][1] + OY,
                        triangles[i][2][0] + OX, triangles[i][2][1] + OY,
                        c);
        OY += 150;
    }

    // Colored triangle
    vector3f red(1.0, 0.0, 0.0);
    vector3f green(0.0, 1.0, 0.0);
    vector3f blue(0.0, 0.0, 1.0);
    OX = 300;
    OY = 0;
    for (int i = 0; i < NUM_TRIANGLES; i++) {
        color_triangle(triangles[i][0][0] + OX, triangles[i][0][1] + OY,
                       triangles[i][1][0] + OX, triangles[i][1][1] + OY,
                       triangles[i][2][0] + OX, triangles[i][2][1] + OY,
                       red, green, blue);
        OY += 150;
    }

    // Textured triangle
    vector3f topleft(0, 0);
    vector3f bottomleft(0, 55);
    vector3f topright(135, 0);
    OX = 450;
    OY = 0;
    for (int i = 0; i < NUM_TRIANGLES; i++) {
        textured_triangle(triangles[i][0][0] + OX, triangles[i][0][1] + OY,
                       triangles[i][1][0] + OX, triangles[i][1][1] + OY,
                       triangles[i][2][0] + OX, triangles[i][2][1] + OY,
                       topleft, bottomleft, topright);
        OY += 150;
    }


    // We end by flipping the buffer:
    al_flip_display();
}

// ----------------------- Exercises ------------------------ //

/**
 * Part 1: Simple triangle.
 */
void simple_triangle(int x1, int y1, int x2, int y2, int x3, int y3, ALLEGRO_COLOR color) {
    // Sort points vertically
    sort_triangle(x1, y1, x2, y2, x3, y3);

    // Implement  the drawing routine here:
    //      Compute dx_a, dx_b, dx_c
    //      Draw the top part
    //      Draw the bottom part

    // NB: Note the special case y1 == y2
}



/**
 * Part 2. Triangle with color interpolation.
 */
void color_triangle(int x1, int y1, int x2, int y2, int x3, int y3, vector3f c1, vector3f c2, vector3f c3) {
    // Sort points vertically
    sort_triangle_with_attributes(x1, y1, x2, y2, x3, y3, c1, c2, c3);

    // Implement the drawing routine here
    // It is largely the same as simple_triangle, with the addition
    // of interpolation over the colors.
}

/**
 * Part 3. Triangle with texture coordinate interpolation.
 */
void textured_triangle(int x1, int y1, int x2, int y2, int x3, int y3, vector3f uv1, vector3f uv2, vector3f uv3) {
    // Sort points vertically
    sort_triangle_with_attributes(x1, y1, x2, y2, x3, y3, uv1, uv2, uv3);

    // This routine can be literally a copy-paste of color_triangle (rename uv1, uv2, uv3 to c1, c2, c3, though)
    // with the gradient_line invocation replaced by textured_line (not provided)
}

// ----------------------- Utility functions ------------------------ //

/**
 * Given three triangle vertices sorts them vertically.
 */
void sort_triangle(int& x1, int& y1, int& x2, int& y2, int& x3, int& y3) {
    if (y2 > y3) {
        swap(x2, x3); swap(y2, y3);
    }
    if (y1 > y2) {
        swap(x1, x2); swap(y1, y2);
    }
    if (y2 > y3) {
        swap(x2, x3); swap(y2, y3);
    }
    if (y1 == y2 && x1 > x2) {
        swap(x1, x2); swap(y1, y2);
    }
}

/**
 * Same as sort_triangle, but also sorts attribute vectors attached to the vertices.
 */
void sort_triangle_with_attributes(int& x1, int& y1, int& x2, int& y2, int& x3, int& y3, vector3f& c1, vector3f& c2, vector3f& c3) {
    if (y2 > y3) {
        swap(x2, x3); swap(y2, y3); swap(c2, c3);
    }
    if (y1 > y2) {
        swap(x1, x2); swap(y1, y2); swap(c1, c2);
    }
    if (y2 > y3) {
        swap(x2, x3); swap(y2, y3); swap(c2, c3);
    }
    if (y1 == y2 && x1 > x2) {
        swap(x1, x2); swap(y1, y2); swap(c1, c2);
    }
}

/**
 * Draws a horizontal line from (x1, y) to (x2, y)
 */
void horizontal_line(float x1, float x2, int y, ALLEGRO_COLOR color) {
    if (x1 > x2) swap(x1, x2);
    for (int x = x1; x <= x2; x++) al_put_pixel(x, y, color);
}

/**
 * Draws a horizontal line from (x1, y) to (x2, y) with color interpolation
 */
void gradient_line(float x1, vector3f c1, float x2, vector3f c2, int y) {
    if (x1 > x2) {
        swap(x1, x2);
        swap(c1, c2);
    }
    vector3f dc = (c2 - c1)/(x2 - x1);
    for (int x = x1; x <= x2; x++) {
            al_put_pixel(x, y, c1.as_color());
            c1 += dc;
    }
}

