/**
 * MTAT 03.015 Computer Graphics.
 * Helper illustrations for Lecture 05.
 *
 * Scene definitions.
 *
 * Copyright 2013, Konstantin Tretyakov.
 */
#include "scene_util.h"
#include <vector>
using namespace std;

class Scene {
public:
    virtual void draw() = 0;
    virtual void start() { };
    virtual void idle() {
        glutPostRedisplay();
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
        if (current_scene_id >= this->size()) current_scene_id = 0;
        current()->start();
    }
    void prev() {
        current_scene_id--;
        if (current_scene_id < 0) current_scene_id = this->size()-1;
        current()->start();
    }
    void push(Scene* s) { this->push_back(s); }
};

class SixthScene: public Scene {
public:
    int n, t;
    bool with_lighting, with_depth, with_smooth, fast;
    SixthScene(bool with_lighting, bool with_depth, bool with_smooth, bool fast):
        with_lighting(with_lighting), with_depth(with_depth), with_smooth(with_smooth), fast(fast) {
    }
    void start() {
        glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            setup_scene_projection();
        glMatrixMode(GL_MODELVIEW);
        glEnable(GL_LIGHTING);
        glLoadIdentity();
        float light_pos[] = {0, 0, 0, 1};
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
        t = glutGet(GLUT_ELAPSED_TIME);
    }
    void idle() {
        n = (glutGet(GLUT_ELAPSED_TIME) - t)/60;
        if (fast) n = 1000;
        glutPostRedisplay();
    }
    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_scene(-1);
        glClear(GL_DEPTH_BUFFER_BIT);
        if (!with_lighting) glDisable(GL_LIGHTING);
        if (!with_depth) glDisable(GL_DEPTH_TEST);
        if (!with_smooth) glShadeModel(GL_FLAT);
        draw_scene(n, true);
        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);
        glShadeModel(GL_SMOOTH);
    }
};

class FifthScene: public Scene {
    int t;
    vector3f target_pos = vector3f(-10, 3, 3);
    float animation_time = 1; // Seconds
    float progress = 0.0;
    double initial_matrix[16];
    double final_matrix[16];

    void start() {
        // Extract initial complete projection matrix for the whole scene
        glPushMatrix();
            glLoadIdentity();
            gluPerspective(DOF_Y, ASPECT_RATIO, 1, 100);
            gluLookAt(target_pos.x, target_pos.y, target_pos.z, 0, 0, -2.5, 0, 1, 0);
            setup_scene_projection_flipped();
            glGetDoublev(GL_MODELVIEW_MATRIX, initial_matrix);
        glPopMatrix();
        // Extract final complete matrix for the whole scene
        glPushMatrix();
            glLoadIdentity();
            setup_scene_projection();
            glGetDoublev(GL_MODELVIEW_MATRIX, final_matrix);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        t = glutGet(GLUT_ELAPSED_TIME);
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
        draw_scene(-1);
    }
};

class FourthScene: public Scene {
    int t;
    vector3f target_pos = vector3f(-10, 3, 3);
    float animation_time = 1; // Seconds
    float progress = 0.0;

    void start() {
        glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(DOF_Y, ASPECT_RATIO, 1, 100);
        glMatrixMode(GL_MODELVIEW);

        t = glutGet(GLUT_ELAPSED_TIME);
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
        gluLookAt(target_pos.x, target_pos.y, target_pos.z, 0, 0, -2.5, 0, 1, 0);
        interpolated_matrix((function_t)glLoadIdentity, setup_scene_projection_flipped, progress);
        draw_clip_volume(setup_scene_projection);
        draw_camera();
        draw_scene(-1);
    }
};

class ThirdScene: public Scene {
    int t;
    vector3f target_pos = vector3f(-10, 3, 3);

    void start() {
        t = glutGet(GLUT_ELAPSED_TIME);
    }

    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(target_pos.x, target_pos.y, target_pos.z, 0, 0, -2.5, 0, 1, 0);
        draw_clip_volume(setup_scene_projection);
        draw_camera();
        draw_scene(-1);
    }
};

class SecondScene: public Scene {
    int t;
    vector3f start_pos = vector3f(0, 0, 0);
    vector3f target_pos = vector3f(-10, 3, 3);
    vector3f cur_pos;
    vector3f d_pos;
    float animation_time = 1; // seconds

    void start() {
        t = glutGet(GLUT_ELAPSED_TIME);
        cur_pos = vector3f(0, 0, 0);
        d_pos = target_pos - start_pos;
        // We must traverse the distance in "animation_time" seconds
        d_pos = d_pos / animation_time;
        glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(DOF_Y, ASPECT_RATIO, 1, 100);
        glMatrixMode(GL_MODELVIEW);
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
        draw_scene(-1);
    }
};

class FirstScene: public Scene {
    int t;
    int n;
    void start() {
        glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            setup_scene_projection();
        glMatrixMode(GL_MODELVIEW);
        glEnable(GL_LIGHTING);
        glLoadIdentity();
        float light_pos[] = {0, 0, 0, 1};
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
        t = glutGet(GLUT_ELAPSED_TIME);
    }
    void idle() {
        n = (glutGet(GLUT_ELAPSED_TIME) - t)/20;
        glutPostRedisplay();
    }
    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_scene(n);
    }
};
