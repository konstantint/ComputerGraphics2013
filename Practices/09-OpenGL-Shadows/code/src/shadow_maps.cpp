/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 9: Shadows
 */

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
#include <vector>
using namespace std;

#include "shader_util.h"
#include "scene_primitives.h"

// --------------- Forward declarations ------------- //
int main(int argc, char* argv[]);
void keyboard(unsigned char key, int x, int y);
void reshape(int w, int h);
void display(); // Modify this function

// Utility functions you will need
/**
 * Renders the whole scene, then copies the depth buffer to a texture
 * identified by given handle.
 */
void render_scene_depth_buffer_to_texture(GLuint textureHandle, vector<Cube>& objects);

/**
 * Set up the projection matrix for viewing the world through the camera (as needed for normal rendering)
 */
void set_viewer_projection();

/**
 * Set up the projection matrix for viewing the world from the point of view of the light source
 */
void set_lightsource_projection(float* light_pos);

// ----------------------------------------------- //
shader_prog shader("../src/shadowmap.vert.glsl", "../src/shadowmap.frag.glsl");
GLuint shadowMapTextures[2]; // Textures used for shadow mapping

// ----------------------------------------------- //
/**
 * Program entry point
 */
int main(int argc, char* argv[]) {
    // ---------- Initialize GLUT & GLEW ----------
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Shadows");
    if (glewInit() != GLEW_OK) {
        cout << "Glew initialization failed" << endl;
        return 1;
    }

    // ---------- Register handlers ----------
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc((void (*)())glutPostRedisplay);
    reshape(800, 600);

    // ---------- Set basic config ----------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ---------- Set light source parameters ------------
    configure_lighting();

    // ---------- Do per-fragment lighting ----------
    shader.use();

    // Create shadow map textures, configure and bind them to the sampler objects in the shader
    glGenTextures(2, shadowMapTextures);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowMapTextures[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // No mipmapping, please
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    shader.uniform1i("shadowMapTexture0", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadowMapTextures[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    shader.uniform1i("shadowMapTexture1", 1);

    // ---------- Run the event loop ----------
    glutMainLoop();

    // Free depth textures
    glDeleteTextures(2, shadowMapTextures);

    return 0;
}

// ----------------------------------------------- //
void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

// ----------------------------------------------- //
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) glutLeaveMainLoop();
}

// ----------------------------------------------- //
/**
 * Exercise 4. Shadow maps
 */
void display() {
    float t = glutGet(GLUT_ELAPSED_TIME)*0.001;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // --------------------------------------------------------
    // Prepare positions of two light sources and two "Cube" objects
    vector<float*> lights = set_light_positions(t);
    vector<Cube> objects = create_objects(t);

    // --------------------------- The shadow mapping algorithm ---------------------------
    // You do not have to change anything here, the exercise is about fixing the fragment shader,
    // however, you should understand the algorithm logic.

    // For each light source, we have to prepare a "shadow map texture", which is
    // the depth buffer, obtained by rendering the scene from the point of view of that light source.
    // In addition, we extract the view-projection matrix used to obtain this texture into the corresponding
    // variable in the shader.
    for (int i = 0; i < 2; i++) {
        // Set the projection "from the point of view of the light source" and render depth buffer to a texture
        set_lightsource_projection(lights[i]);
        render_scene_depth_buffer_to_texture(shadowMapTextures[i], objects);
        // Extract the projection matrix and bind it to a variable in the shader
        float shadowMapMatrix[16];
        glGetFloatv(GL_PROJECTION_MATRIX, shadowMapMatrix);
        shader.uniformMatrix4fv((i == 0) ? "shadowMapMatrix[0]" : "shadowMapMatrix[1]", shadowMapMatrix);
    }

    // Now that the shadow map textures are prepared, render the scene.
    // The shader is responsible for making appropriate use of the shadow textures.
    // Exercise: Fix the shader.
    set_viewer_projection();
    draw_walls();
    for (int i = 0; i < objects.size(); i++) objects[i].draw();

    // Draw light source positions
    shader.uniform1i("no_lighting", 1);
    draw_lights(lights);
    shader.uniform1i("no_lighting", 0);

    glutSwapBuffers();
}

// ------------------------ Utility functions --------------------- //
/**
 * Set up the projection matrix for viewing the world through the camera (as needed for normal rendering)
 */
void set_viewer_projection() {
    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective (45.0, (float)glutGet(GLUT_WINDOW_WIDTH)/glutGet(GLUT_WINDOW_HEIGHT), 0.5, 100);

        // NB: In this exercise we actually want to group model, view and projection transforms
        // as (model) + (view-projection), rather than traditional (model-view) + (projection).
        // The reason is that we need to operate with several different (view-projection) setups
        // (those corresponding to the viewer and two light sources) on the same world (i.e. model)
        //
        // Consequently, we prepend the view transform into our projection matrix.
        // (rather than appending to the modeling matrix, as is more customary).
        gluLookAt(0, -10, 6,
                  0, -1, 3,
                  0, 0, 1);

        // This, however, means that we have to change the lighting computations to deal with the fact
        // that our MODELVIEW transform only puts things into the world frame, not the camera frame.
        // This means that in the lighting computations the viewer is not at (0, 0, 0), but in a particular
        // location, that we specify for the shader to use:
        shader.uniform3f("viewer_position", 0, -10, 6);

        // If this is not clear, try moving gluLookAt into the modelview matrix and fixing the consequences.
        // (or assume this code is not given to you and try implementing shadow mapping from scratch)
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Set up the projection matrix for viewing the world from the point of view of the light source
 */
void set_lightsource_projection(float* light_pos) {
    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(120, (float)glutGet(GLUT_WINDOW_WIDTH)/glutGet(GLUT_WINDOW_HEIGHT), 0.1, 20);
        gluLookAt(light_pos[0], light_pos[1], light_pos[2],
                  0, 0, 0,  // This corresponds somehow to the middle of the scene.
                  0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Renders the whole scene, then copies the depth buffer to a texture
 * identified by given handle.
 */
void render_scene_depth_buffer_to_texture(GLuint textureHandle, vector<Cube>& objects) {
    int w = glutGet(GLUT_WINDOW_WIDTH);   // Determine current width and height - this i
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    float* depth_data = (float*)malloc(w*h*sizeof(float)); // Initialize array for depth data
    shader.uniform1i("no_lighting", 1);                    // Turn off lighting in the shader

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);    // Disable writing to screen
    draw_walls();                                           // Render the scene
    for (int i = 0; i < objects.size(); i++) {
        objects[i].draw();
    }

    // Now read out the depth buffer data and copy it to the given texture

    // Note: what we are doing here is not the most efficient way to do it.
    // In fact, in modern OpenGL you would rather render directly to the texture by creading a separate framebuffer object.
    // (see e.g. http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/ and
    // https://developer.nvidia.com/sites/default/files/akamai/gamedev/docs/opengl_rendertexture.pdf)
    // However, this might not work on some older machines (and it requires the introduction of a bunch of
    // OpenGL-specific commands, that are not in principle the topic of this practice session nor this course per se).

    glReadPixels(0, 0, w, h, GL_DEPTH_COMPONENT, GL_FLOAT, depth_data);
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, w, h, 0, GL_LUMINANCE, GL_FLOAT, depth_data);

    // NB: If you want to debug such code, the following line flushes the depth array to the screen
    //glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); glDrawPixels(w, h, GL_LUMINANCE, GL_FLOAT, depth_data);

    free(depth_data);   // Release memory

    // Switch writing to screen back on, clear the depth buffer that we messed up
    glClear(GL_DEPTH_BUFFER_BIT);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    shader.uniform1i("no_lighting", 0); // Enable lighting again
}
