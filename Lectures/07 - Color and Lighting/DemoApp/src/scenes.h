/**
 * MTAT 03.015 Computer Graphics.
 * Helper illustrations for Lecture 05.
 *
 * Scene definitions.
 *
 * Copyright 2013, Konstantin Tretyakov.
 */
#include "scene_util.h"
#include "shader_util.h"
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




class SpotScene: public BaseScene {
public:
    void start() {
        BaseScene::start();
        float light_dir[] = {3, 0, -4, 1};
        float spot_dir[] = {-1, 0, 0, 0};
        glLightfv(GL_LIGHT0, GL_POSITION, light_dir);
        glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_dir);
        glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 25);
        glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 1);
        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1f);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01f);
        float light_ambient[] = {0.02, 0.02, 0.02, 1};
        float light_diffuse[] = {1, 1, 1, 1};
        float light_specular[] = {1, 1, 1, 1};
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3f(1, 1, 1);
        glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
        glColor3f(1, 1, 1);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);
        glShadeModel(GL_SMOOTH);
    }

    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_triangles(triangles, -1, false);
        label("Spotlight");
    }
};


class PhongScene: public BaseScene {
public:
    void start() {
        BaseScene::start();
        float light_dir[] = {1, 0, 0, 0};
        glLightfv(GL_LIGHT0, GL_POSITION, light_dir);
        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0f);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);
        float light_ambient[] = {0.02, 0.02, 0.02, 1};
        float light_diffuse[] = {1, 1, 1, 1};
        float light_specular[] = {1, 1, 1, 1};
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3f(1, 1, 1);
        glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
        glColor3f(1, 1, 1);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);
        glShadeModel(GL_SMOOTH);
    }

    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_triangles(triangles, -1, false);
        label("Phong lighting (per-vertex, no gamma correction)");
    }
};


class ShaderScene: public BaseScene {
public:
    const char* vshader;
    const char* fshader;
    const char* message;
    ShaderScene(const char* vshader, const char* fshader, const char* message):
        vshader(vshader), fshader(fshader), message(message) {};
    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader_prog shader(vshader, fshader);
        shader.use();
        draw_triangles(triangles, -1, false);
        shader.free();
        label(message);
    }
};


class LambertScene: public BaseScene {
public:
    bool smooth;
    LambertScene(bool smooth): smooth(smooth) {}
    void start() {
        BaseScene::start();
        glLoadIdentity();
        float light_dir[] = {1, 0, 0, 0};
        glLightfv(GL_LIGHT0, GL_POSITION, light_dir);
        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0f);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);
        float light_ambient[] = {0, 0, 0, 0};
        float light_diffuse[] = {1, 1, 1, 1};
        float light_specular[] = {0, 0, 0, 0};
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3f(0, 0, 0);
        glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
        glColor3f(1, 1, 1);
        glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
        glColor3f(0, 0, 0);
        smooth ? glShadeModel(GL_SMOOTH) : glShadeModel(GL_FLAT);
    }

    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_triangles(triangles, -1, false);
        label(smooth ? "Lambert diffusion with interpolation" : "Lambert diffusion");
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
