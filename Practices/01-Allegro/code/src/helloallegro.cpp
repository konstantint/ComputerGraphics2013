// ---------------------------- Includes -------------------------- //

// Include for console logging.
#include <iostream>
using namespace std;

// Include Allegro headers.
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

// ---------------------------- Global variables -------------------------- //
ALLEGRO_DISPLAY     *display;
ALLEGRO_EVENT_QUEUE *event_queue;
ALLEGRO_EVENT       event;
ALLEGRO_FONT*       my_font;
ALLEGRO_BITMAP*     photo;
ALLEGRO_BITMAP*     kitten;

int current_scene_id;
int quit_now;
float kitten_position;
float kitten_speed;


// ---------------------------- Forward declarations -------------------------- //
// C uses a single pass to compile the file. Because of that, we can only
// invoke within any function those other functions that have been defined or declared
// before it. This can be inconventient. For example, we would like to write the
// "main" function first in this file, but if the "main" function invokes, say,
// the "init" function we must define "init" before the "main".
//
// The solution to this is a "forward declaration". I.e. we can provide just the
// name of the function that will come some time later on. In this case, when the
// function is used, the compiler will feel fine.

// Providing forward declarations is also a nice general practice, as it
// gives you an easy way of seeing what functions you are using later on.

void init();
void deinit();
void event_loop();
void process_event(ALLEGRO_EVENT event);
void animate();
void draw();
void draw_primitives();
void draw_bitmaps();
void draw_animation();

// ---------------------------- Main -------------------------- //
// The overall structure of our program is very simple:

int main(int argc, char **argv){
    init();         // Initialize Allegro.
    event_loop();   // Run the event processing loop until a program is requested to quit
    deinit();       // Deinitialize

    return 0;       // This is simply required by the C++ standard to indicate successful completion
}


// ---------------------------- Allegro initialization -------------------------- //
// Initialize the Allegro system.

void init() {
    // Note that strictly speaking, we have to check the output of each call
    // and abort if any of those fails. E.g. if (!al_init()) return 0;

    // This is a compulsory first call to initialize Allegro
    al_init();

    // Next we initialize the display for drawing
    display = al_create_display(640, 480);

    // An `event queue` will be used to process keyboard and mouse input
    event_queue = al_create_event_queue();

    // We need to explicitly specify what events will be "listened by" the event queue.
    // For example, if we want to receive and react to a "close" button, we need to register this event
    al_register_event_source(event_queue, al_get_display_event_source(display));

    // And those are keyboard events
    al_install_keyboard();
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    // To draw the primitives (circles, etc) we need to init the corresponding addons.
    al_init_primitives_addon();

    // Load images
    al_init_image_addon();
    photo = al_load_bitmap("../data/photo.jpg");
    kitten = al_load_bitmap("../data/kitten.png");

    // Initialize font for text drawing
    al_init_font_addon();
    my_font = al_load_font("../data/a4_font.tga", 0, 0);

    // We will be showing one of three different scenes.
    current_scene_id = 0;
    quit_now = 0;
    kitten_position = 320.0;
    kitten_speed = 0.0;
}

// ---------------------------- Allegro deinitialization -------------------------- //
// Just the opposite of "init". Destroy whatever we created.

void deinit() {
    al_destroy_bitmap(photo);
    al_destroy_bitmap(kitten);
    al_destroy_font(my_font);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
}

// ---------------------------- Event loop -------------------------- //
// Event loop forms the backbone of the program. Here
// we simply loop around waiting for events and/or redrawing the screen
// as necessary.

void event_loop() {
    while(1) {  // Yes, it is an infinite loop.

        // We first wait some time until an event (e.g. keyboard or mouse) comes into the queue,
        // but no longer than 0.05 seconds.
        bool have_event = al_wait_for_event_timed(event_queue, &event, 0.05);

        // If an event came, react
        if (have_event) process_event(event);

        // If we have to quit now, quit.
        if (quit_now) break;

        // Else, perform any running animations and update the screen, if necessary.
        animate();
        draw();
   }
}

// ---------------------------- Event processing -------------------------- //
// Here we can decide what to do depending on the event.

void process_event(ALLEGRO_EVENT event) {
    switch(event.type) {
    case ALLEGRO_EVENT_DISPLAY_CLOSE:
        // User clicked the "close" button.
        quit_now = 1;
        break;
    case ALLEGRO_EVENT_KEY_UP:
        // User pushed down a key, if it is ESCAPE, we quit, otherwise switch current scene:
        switch(event.keyboard.keycode) {
        case ALLEGRO_KEY_ESCAPE:
            quit_now = 1;
            break;
        case ALLEGRO_KEY_SPACE:
            current_scene_id = (current_scene_id + 1) % 3;
            break;
        case ALLEGRO_KEY_LEFT:
            kitten_speed -= 20;
            break;
        case ALLEGRO_KEY_RIGHT:
            kitten_speed += 20;
            break;
        }
        break;
    }
}

// ---------------------------- Drawing routines -------------------------- //
// Here is where all the drawing is performed.

void draw() {
    // We start by clearing the screen:
    al_clear_to_color(al_map_rgb(0,0,0));

    switch(current_scene_id) {
        case 0:
            draw_primitives();
            break;
        case 1:
            draw_bitmaps();
            break;
        case 2:
            draw_animation();
            break;
    }

    // We end by flipping the buffer:
    al_flip_display();
}

void draw_primitives() {
    // Pixels
    al_put_pixel(30, 30, al_map_rgb(255, 0, 0));    // Red pixel at position 30, 30
    al_put_pixel(60, 60, al_map_rgb(0, 255, 0));    // Green pixel at 60, 60
    al_put_pixel(90, 90, al_map_rgb(0, 0, 255));    // Blue pixel at 90, 90

    // Lines
    al_draw_line(0, 50, 600, 50, al_map_rgb(255, 255, 0), 1);
    al_draw_line(50, 0, 50, 400, al_map_rgb(0, 255, 255), 1);

    // Rectangles
    al_draw_rectangle(100, 100, 175, 175, al_map_rgb(128, 128, 255), 1);
    al_draw_filled_rectangle(150, 200, 400, 250, al_color_name("green"));

    // Ellipses
    al_draw_ellipse(500, 150, 50, 50, al_color_name("purple"), 1);
    al_draw_filled_ellipse(600, 250, 50, 100, al_color_html("#ff0000"));

    // Text
    al_draw_text(my_font, al_color_hsv(0.1, 1.0, 1.0), 70, 12, ALLEGRO_ALIGN_LEFT, "Screen 1: draw_primitives().");
    al_draw_text(my_font, al_color_hsv(0.1, 1.0, 1.0), 70, 65, ALLEGRO_ALIGN_LEFT, "Press space.");
}

void draw_bitmaps() {
    // Fully stretched background image
    al_draw_scaled_bitmap(photo, 0, 0, al_get_bitmap_width(photo), al_get_bitmap_height(photo),
                                 0, 0, al_get_display_width(display), al_get_display_height(display), 0);

    // Kitten at some position
    al_draw_bitmap(kitten, 500, 200, 0);

    // Half-transparent kitten
    al_draw_tinted_bitmap(kitten, al_map_rgba_f(0.5, 0.5, 0.5, 0.5), 200, 200, 0);
}

void draw_animation(){
    al_draw_bitmap(kitten, kitten_position, al_get_display_height(display) - al_get_bitmap_height(kitten), 0);
}

// ------------------- Animation routine ------------------ //

void animate() {
    // Move the cat if current scene is 2.
    if (current_scene_id == 2) {
        kitten_position += kitten_speed;
        kitten_speed /= 1.4;
    }
}
