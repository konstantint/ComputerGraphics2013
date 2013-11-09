/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 10: OpenGL 3.1+
 */

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
using namespace std;

#include <glutil/MatrixStack.h>
using glutil::MatrixStack; // We shall be using a custom matrix stack implementation

#include "shader_util.h"
#define GLUT_KEY_ESCAPE 27
#define GLUT_KEY_ENTER 13

// --------------- Forward declarations ------------- //
int main(int argc, char* argv[]);
void keyboard(unsigned char key, int x, int y);
void display();
void prepare_vertex_data();

shader_prog shader("../src/triangle.vert.glsl", "../src/triangle.frag.glsl");
GLuint vertexArrayHandle;
GLuint arrayBufferHandle;
int current_scene_id = 0;

// ----------------------------------------------- //
/**
 * Program entry point
 */
int main(int argc, char* argv[]) {
    // Initialize GLUT and GLEW
    glutInit(&argc, argv);

    // The two lines below will tell GLUT to set the context up so that
    // we will not be able to use any of the old functions
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutCreateWindow("Triangle");

    // Initialize GLEW.
    glewExperimental = TRUE; // This is a hack. Without it the current GLEW version fails to load
                             // some extension functions. See http://www.opengl.org/wiki/OpenGL_Loading_Library
    if (glewInit() != GLEW_OK) {
        cout << "Glew initialization failed" << endl;
        return 1;
    }

    // Register handlers
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc((void (*)())glutPostRedisplay);

    // Configuration
    glClearColor(0, 0, 0, 0);

    // We *must* use a shader in a OpenGL 3.2+ core profile
    shader.use();

    // We must specify the name of the "output" variable of our
    // fragment shader. gl_FragColor does not work any more.
    // The good thing - you can output to several channels in parallel.
    glBindFragDataLocation(shader, 0, "fragColor");

    // Next, we prepare the vertex data
    prepare_vertex_data();

    // Run the event loop
    glutMainLoop();

    // Finally, be polite and clean up the objects from GPU memory
    glDeleteVertexArrays(1, &vertexArrayHandle);
    glDeleteBuffers(1, &arrayBufferHandle);

    return 0;
}

// ----------------------------------------------- //
void keyboard(unsigned char key, int x, int y) {
    if (key == GLUT_KEY_ESCAPE) glutLeaveMainLoop();
    else if (key == GLUT_KEY_ENTER || key == ' ') current_scene_id = 1 - current_scene_id;
}

// ----------------------------------------------- //
// In this function we:
//  * Initialize the vertex array object
//  * Create an array buffer with data
//  * Associate data from the array buffer with vertex attribute(s)
void prepare_vertex_data() {

    // First create the vertex array object (VAO).
    // We can't do anything without a VAO.
    // See http://www.opengl.org/wiki/Vertex_Specification
    glGenVertexArrays(1, &vertexArrayHandle);
    // We can have a separate VAO for each object in our scene.
    // But we have to specify what is the current VAO we are working with.
    glBindVertexArray(vertexArrayHandle); // This will be our current VAO now.

    // Next, we need to set up the data for our vertex objects
    // Data is provided in the form of "vertex attributes"
    // Before we can configure vertex attributes, however, we have
    // to upload the data itself, and for that we need to create an *array buffer*
    glGenBuffers(1, &arrayBufferHandle);

    // Now let us fill the buffer with data
    // As usual (recall textures), OpenGL wants you to first to "attach" the buffer object
    // you just created into the current context under the GL_ARRAY_BUFFER label.
    // (this makes it the "current buffer"). Whatever you will then make with this
    // current buffer
    const float vertexData[] = {
        -0.5, -0.5,
         0.5, -0.5,
         0.0,  0.5
    };
    glBindBuffer(GL_ARRAY_BUFFER, arrayBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    // Note the last parameter of glBufferData. It specifies how often we plan to change the data
    // GL_STATIC_DRAW means the array is fixed. If we would plan to continuously write new
    // values there, it would be GL_STREAM_DRAW.

    // Creating an array buffer and filling it with numbers is still not enough:
    // OpenGL has no idea what those numbers are. Are they vertex coordinates? Normals? Colors?
    // We must now specify which elements in the array correspond to which *attributes*
    // In our case there will only be one attribute - vec4 position.
    // We shall assign this attribute to "attribute index 0".

    glEnableVertexAttribArray(0); // Enable attribute 0
    // Configure attribute 0
    glVertexAttribPointer(0, // Attribute index
                          2, // Attribute is a two-element vector (vec2)
                          GL_FLOAT, // Means tt is a two-element vector of floats (not ivec2 or smth)
                          GL_FALSE, // It is not in a fixed-point format
                                    // This is something we did not cover in the course, so google it up, e.g. http://jet.ro/files/The_neglected_art_of_Fixed_Point_arithmetic_20060913.pdf
                                    // We did not cover it because we largely do not use it anywhere.
                                    // To indicate that we do not use it, we say GL_FALSE here.
                          2*sizeof(float), // Spacing between consequtive elements, in bytes.
                          (const GLvoid*)(0*sizeof(float)) // Offset to where the first element starts, in bytes.
                          );
}

// ----------------------------------------------- //
void display() {
    float t = glutGet(GLUT_ELAPSED_TIME);
    glClear(GL_COLOR_BUFFER_BIT);     // Clear screen

    // Initialize matrix stack. Equivalent to glLoadIdentity();
    MatrixStack m;
    // Equivalent to glRotatef(...), but rather than modifying OpenGL state,
    // modifies local MatrixStack object.
    m.Rotate(glm::vec3(0, 0, 1), t*0.01);
    // Assign the current matrix to a variable in the shader
    // the .Top() method returns the top of the stack.
    shader.uniformMatrix4fv("modelViewProjectionMatrix", glm::value_ptr(m.Top()));

    // Now basically emit a glBegin(GL_TRIANGLES),
    // followed by three glVertex calls, corresponding to vertex array object elements 0, 1, 2.
    // With each vertex we send to the shader all the vertex attributes we have configured
    // in prepare_vertex_data.
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Swap the front and back buffers (the usual double-buffering stuff)
    glutSwapBuffers();
}
