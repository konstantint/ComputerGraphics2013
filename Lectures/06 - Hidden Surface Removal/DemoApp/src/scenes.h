/**
 * MTAT 03.015 Computer Graphics.
 * Helper illustrations for Lecture 05.
 *
 * Scene definitions.
 *
 * Copyright 2013, Konstantin Tretyakov.
 */
#include "scene_util.h"
#include "triangles.h"
#include <vector>
#include <algorithm>
using namespace std;

class Scene {
public:
    virtual void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    };
    virtual void start() { };
    virtual void idle() {
        glutPostRedisplay();
    }
    virtual ~Scene() {};
};

class BaseScene: public Scene {
public:
    vector<Triangle> triangles;
    int t, n;
    void start() {
        triangles = make_scene();
        glMatrixMode(GL_PROJECTION);    // Set default projection and lighing
            glLoadIdentity();
            gluPerspective(DOF_Y, ASPECT_RATIO, 1, 100);
        glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        glEnable(GL_LIGHTING);
        float light_pos[] = {0, 0, 0, 1};
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.5f);
        glShadeModel(GL_FLAT);
        t = glutGet(GLUT_ELAPSED_TIME);
    }
    void idle() {
        n = (glutGet(GLUT_ELAPSED_TIME) - t)/30;
        glutPostRedisplay();
    }
    void label(const char* text) {
        draw_label(-0.9, 0.9, text);
    }
};

class SceneSet: public vector<Scene*> {
public:
    int current_scene_id;
    Scene* current() {
        return (*this)[current_scene_id];
    }
    void next() {
        current_scene_id++;
        if (current_scene_id >= (int)this->size()) current_scene_id = 0;
        current()->start();
    }
    void prev() {
        current_scene_id--;
        if (current_scene_id < 0) current_scene_id = this->size()-1;
        current()->start();
    }
    void push(Scene* s) { this->push_back(s); }
};

class EmptyScene: public Scene {
};


class DepthPrepassScene: public BaseScene {
public:
    float* depth_data;
    int w, h;
    DepthPrepassScene(): BaseScene() {
        w = glutGet(GLUT_WINDOW_WIDTH);
        h = glutGet(GLUT_WINDOW_HEIGHT);
        depth_data = (float*)malloc(w*h*sizeof(float));
    }
    ~DepthPrepassScene() {
        free(depth_data);
    }
    void start() {
        BaseScene::start();
        get_depth_buffer(-1);
    }
    void get_depth_buffer(int num_elements) {
        glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            setup_scene_projection();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_triangles(triangles, num_elements, false);
        glReadPixels(0, 0, w, h, GL_DEPTH_COMPONENT, GL_FLOAT, depth_data);
    }
    void draw() {
        glClear(GL_COLOR_BUFFER_BIT);
        glDepthFunc(GL_LEQUAL);
        draw_triangles(triangles, n, false);
        label("Rendering after depth pre-pass");
    }
};


class DepthScene: public BaseScene {
public:
    float* depth_data;
    int w, h;
    DepthScene(): BaseScene() {
        w = glutGet(GLUT_WINDOW_WIDTH);
        h = glutGet(GLUT_WINDOW_HEIGHT);
        depth_data = (float*)malloc(w*h*sizeof(float));
    }
    ~DepthScene() {
        free(depth_data);
    }
    void start() {
        BaseScene::start();
    }
    void get_depth_buffer(int num_elements) {
        glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            setup_scene_projection();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_triangles(triangles, num_elements, false);
        glReadPixels(0, 0, w, h, GL_DEPTH_COMPONENT, GL_FLOAT, depth_data);
    }
    void idle() {
        n = (glutGet(GLUT_ELAPSED_TIME) - t)/30;
        get_depth_buffer(n);
        glutPostRedisplay();
    }
    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRasterPos2i(-1, -1); // glWindowPos would be nicer, but it requires GLEW and I'm too lazy to link it now
        glDrawPixels(w, h, GL_LUMINANCE, GL_FLOAT, depth_data);
        glPushAttrib(GL_CURRENT_BIT);
        glColor3f(0, 0, 0);
        glDisable(GL_DEPTH_TEST);
        label("Filling the Z-buffer");
        glEnable(GL_DEPTH_TEST);
        glPopAttrib();
    }
};

class PainterAlgorithmFailScene: public BaseScene {
    void start() {
        BaseScene::start();
    }
    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_LIGHTING);
        glLoadIdentity();
        gluLookAt(0, 0, 3, 0, 0, 0, 0, 1, 0);
        glRotatef(n, 0, 1, 0);
        glScalef(0.7, 0.7, 0.7);
        glBegin(GL_TRIANGLES);
        glColor3f(1, 0, 0);
        glVertex3f(-1, 0, 0);
        glVertex3f(-1, 1, 0);
        glVertex3f(1, 0.5, 0.5);
        glColor3f(0, 1, 0);
        glVertex3f(0, 1, 0);
        glVertex3f(1, 1, 0);
        glVertex3f(0.5, -1, 0.5);
        glColor3f(0, 0, 1);
        glVertex3f(1, -0.5, 0);
        glVertex3f(0, -1, 0);
        glVertex3f(-0.7, 0.7, 0.5);
        glEnd();
        glColor3f(1, 1, 1);
        label("Painter's algorithm can fail");
    }
};

bool sort_by_z(const Triangle& first, const Triangle& second) {
    return first.a.z < second.a.z;
}
class PainterAlgorithmScene: public BaseScene {
    void start() {
        BaseScene::start();
        sort(triangles.begin(), triangles.end(), sort_by_z);
    }
    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_triangles(triangles, -1, true);
        glClear(GL_DEPTH_BUFFER_BIT);
        draw_triangles(triangles, n, false);
        label("Painter's algorithm");
    }
};

class PolygonFillScene: public BaseScene {
public:
    bool with_lighting, with_depth, with_smooth, fast, cull, cull_front;
    const char* scene_label;
    PolygonFillScene(const char* scene_label, bool with_lighting = true,
                     bool with_depth = true, bool with_smooth = false, bool fast = false,
                     bool cull = false, bool cull_front = false):
        with_lighting(with_lighting), with_depth(with_depth), with_smooth(with_smooth), fast(fast), scene_label(scene_label),
        cull(cull), cull_front(cull_front) {
    }
    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_triangles(triangles, -1, true);
        glClear(GL_DEPTH_BUFFER_BIT);
        if (!with_lighting) glDisable(GL_LIGHTING);
        if (!with_depth) glDisable(GL_DEPTH_TEST);
        if (with_smooth) glShadeModel(GL_SMOOTH);
        if (cull) glEnable(GL_CULL_FACE);
        if (cull_front) glCullFace(GL_FRONT);
        draw_triangles(triangles, fast ? -1 : n, false);
        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);
        glShadeModel(GL_FLAT);
        glDisable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        label(scene_label);
    }
};

class BackFaceCullFailScene: public BaseScene {
public:
    void start() {
        BaseScene::start();
        triangles.erase(triangles.begin(), triangles.begin()+10);
    }
    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glLoadIdentity();
        glTranslatef(0, 0, -5);
        glRotatef(n, 1, 0, 0);
        glTranslatef(0, 0, 5);
        draw_triangles(triangles, -1, false);
        glDisable(GL_CULL_FACE);
        label("Back-face culling fails on 'open' objects");
    }
};


class MoveClipSpaceIntoViewScene: public BaseScene {
    vector3f target_pos = vector3f(-10, 3, 3);
    float animation_time = 1; // Seconds
    float progress = 0.0;
    double initial_matrix[16];
    double final_matrix[16];

    void start() {
        BaseScene::start();
        // Extract initial complete projection matrix for the whole scene
        glLoadIdentity();
        gluPerspective(DOF_Y, ASPECT_RATIO, 1, 100);
        gluLookAt(target_pos.x, target_pos.y, target_pos.z, 0, 0, -2.5, 0, 1, 0);
        setup_scene_projection_flipped();
        glGetDoublev(GL_MODELVIEW_MATRIX, initial_matrix);
        // Extract final complete matrix for the whole scene
        glLoadIdentity();
        setup_scene_projection();
        glGetDoublev(GL_MODELVIEW_MATRIX, final_matrix);
        // Reset projection to identity
        glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
    }

    void idle() {
        progress = glutGet(GLUT_ELAPSED_TIME) - t;
        progress /= 1000;
        progress /= animation_time;
        if (progress > 1) progress = 1;
        glutPostRedisplay();
    }

    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        interpolated_matrix_v(initial_matrix, final_matrix, progress);
        draw_clip_volume(setup_scene_projection);
        draw_triangles(triangles, -1, true);
        label("The XY-view of the clipping space can be directly drawn using 2D rendering");
    }
};

class ProjectionTransformScene: public BaseScene {
    vector3f target_pos = vector3f(-10, 3, 3);
    float animation_time = 1; // Seconds
    float progress = 0.0;

    void idle() {
        progress = glutGet(GLUT_ELAPSED_TIME) - t;
        progress /= 1000;
        progress /= animation_time;
        if (progress > 1) progress = 1;
        glutPostRedisplay();
    }

    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(target_pos.x, target_pos.y, target_pos.z, 0, 0, -2.5, 0, 1, 0);
        interpolated_matrix((function_t)glLoadIdentity, setup_scene_projection_flipped, progress);
        draw_clip_volume(setup_scene_projection);
        draw_camera();
        draw_triangles(triangles, -1, true);
        label("Vertex shading applies model-view and projection matrices, mapping everything into normalized clipping space");
    }
};

class FrustumScene: public BaseScene {
    vector3f target_pos = vector3f(-10, 3, 3);

    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(target_pos.x, target_pos.y, target_pos.z, 0, 0, -2.5, 0, 1, 0);
        draw_clip_volume(setup_scene_projection);
        draw_camera();
        draw_triangles(triangles, -1, true);
        label("Projection matrix establishes a view volume");
    }
};

class ModelViewTransformScene: public BaseScene {
    vector3f start_pos = vector3f(0, 0, 0);
    vector3f target_pos = vector3f(-10, 3, 3);
    vector3f cur_pos;
    vector3f d_pos;
    float animation_time = 1; // seconds

    void start() {
        BaseScene::start();
        cur_pos = vector3f(0, 0, 0);
        d_pos = target_pos - start_pos;
        d_pos = d_pos / animation_time; // We must traverse the distance in "animation_time" seconds
    }

    void idle() {
        float elapsed_sec = glutGet(GLUT_ELAPSED_TIME) - t;
        elapsed_sec /= 1000;
        if (elapsed_sec >= animation_time) elapsed_sec = animation_time;
        cur_pos = start_pos + d_pos*elapsed_sec;
        glutPostRedisplay();
    }

    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(cur_pos.x, cur_pos.y, cur_pos.z, 0, 0, -2.5, 0, 1, 0);
        draw_camera();
        draw_triangles(triangles, -1, true);
        label("Model-view matrix positions objects in the world and in front of the camera");
    }
};

class ConstructObjectScene: public BaseScene {
    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_triangles(triangles, n, true);
        label("We describe objects using vertices and triangles");
    }
};
