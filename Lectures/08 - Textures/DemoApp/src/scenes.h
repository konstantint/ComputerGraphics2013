/**
 * MTAT 03.015 Computer Graphics.
 * Helper illustrations for Lecture 8.
 *
 * Scene definitions.
 *
 * Copyright 2013, Konstantin Tretyakov.
 */
#include "scene_util.h"
#include "shader_util.h"
#include <vector>
#include <memory>
#include <glimg/glimg.h>
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
    void label(const char* text) {
        draw_label(-0.9, 0.9, text);
    }
};

class BaseScene: public Scene {
public:
    int t, n;
    void start() {
        t = glutGet(GLUT_ELAPSED_TIME);
        glMatrixMode(GL_PROJECTION);    // Set default projection and lighing
            glLoadIdentity();
            gluPerspective(DOF_Y, ASPECT_RATIO, 1, 100);
        glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        float light_pos[] = {0, 0, 0, 1};
        float black[] = {0, 0, 0, 1};
        float white[] = {1, 1, 1, 1};
        float yellow[] = {1, 1, 0, 1};
        float ambient[] = {0.2, 0.2, 0.2, 1};
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, black);
        glLightfv(GL_LIGHT0, GL_SPECULAR, white);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);
        glMaterialfv(GL_FRONT, GL_AMBIENT, yellow);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
        glMaterialfv(GL_FRONT, GL_SPECULAR, white);
        glMaterialf(GL_FRONT, GL_SHININESS, 30.0);
        glColor3f(1, 1, 1);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    }
    void idle() {
        n = (glutGet(GLUT_ELAPSED_TIME) - t)/30;
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
    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        //glutSolidTeapot(1);
    }
};


class ShadowScene: public Scene {
public:
    GLuint theTexture;
    shader_prog shader = shader_prog("../src/base_vertex_shader.glsl", "../src/base_fragment_shader.glsl");
    void start() {
        shader.use();
        shader.uniform1i("mode", 0);
        glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(DOF_Y, ASPECT_RATIO, 0.5, 100);
            gluLookAt(0, -10, 5, 0, 0, 1, 0, 0, 1);
        glMatrixMode(GL_MODELVIEW);
        glClearColor(0, 0, 0, 0);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        float ambient[] = {0.2, 0.2, 0.2, 1.0};
        float zero[] = {0.0, 0.0, 0.0, 0.0};
        float white[] = {1.0, 1.0, 1.0, 1.0};
        float light[] = {10.0, 10.0, 10.0, 1.0};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_AMBIENT, zero);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light);
        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 2.0);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);
        glMaterialfv(GL_FRONT, GL_SPECULAR, white);
        glMaterialf(GL_FRONT, GL_SHININESS, 30);
    }

    void draw() {
        float t = glutGet(GLUT_ELAPSED_TIME);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glLoadIdentity();
        float light_pos[] = {3*sin(t*0.002), 3*cos(t*0.002), 5, 1.0};
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

        glPushMatrix();
            glTranslatef(3*sin(t*0.002), 3*cos(t*0.002), 5);
            shader.uniform1i("disable_lighting", 1);
            glColor3f(1, 1, 0);
            glutSolidSphere(0.1, 10, 10);
            shader.uniform1i("disable_lighting", 0);
        glPopMatrix();

        // White floor
        float floorWhite[] = {0.5, 0.6, 0.5, 1.0};
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, floorWhite);
        glPushMatrix();
        glScalef(10, 10, 10);
        glBegin(GL_QUADS);
            glNormal3f(0, 0, 1);
            glVertex2f(-1, -1);
            glVertex2f(-1, 1);
            glVertex2f(1, 1);
            glVertex2f(1, -1);
        glEnd();
        glPopMatrix();

        // Yellow walls
        float yellow[] = {0.8, 0.8, 0.1, 1.0};
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, yellow);
        glPushMatrix();
        glTranslatef(3.5, 0, 0);
        glRotatef(-90, 0, 1, 0);
        glScalef(10, 10, 10);
        glBegin(GL_QUADS);
            glNormal3f(0, 0, 1);
            glVertex2f(-1, -1);
            glVertex2f(-1, 1);
            glVertex2f(1, 1);
            glVertex2f(1, -1);
        glEnd();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 3.5, 0);
        glRotatef(90, 1, 0, 0);
        glScalef(10, 10, 10);
        glBegin(GL_QUADS);
            glNormal3f(0, 0, 1);
            glVertex2f(-1, -1);
            glVertex2f(-1, 1);
            glVertex2f(1, 1);
            glVertex2f(1, -1);
        glEnd();
        glPopMatrix();

        // Pink teapot
        glPushMatrix();
            glTranslatef(0, 0, 2);
            glRotatef(t*0.01, 1, 1, 1);
            glRotatef(90, 1, 0, 0);
            float pink[] = {0.5, 0.1, 0.3, 1.0};
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, pink);
            glutSolidTeapot(1);
        glPopMatrix();

        // Teapot shadows
        glColor4f(0.2, 0.2, 0.2, 1);
        shader.uniform1i("disable_lighting", 1);
        glPushMatrix();
            float ground[] = {0, 0, 1, 0};
            glTranslatef(0, 0, 0.001);
            shadowMatrix(ground, light_pos);
            glTranslatef(0, 0, 2);
            glRotatef(t*0.01, 1, 1, 1);
            glRotatef(90, 1, 0, 0);
            glutSolidTeapot(1);
        glPopMatrix();
        glPushMatrix();
            float wall1[] = {-1, 0, 0, 3.5};
            glTranslatef(-0.001, 0, 0);
            shadowMatrix(wall1, light_pos);
            glTranslatef(0, 0, 2);
            glRotatef(t*0.01, 1, 1, 1);
            glRotatef(90, 1, 0, 0);
            glutSolidTeapot(1);
        glPopMatrix();
        glPushMatrix();
            float wall2[] = {0, -1, 0, 3.5};
            glTranslatef(0, -0.001, 0);
            shadowMatrix(wall2, light_pos);
            glTranslatef(0, 0, 2);
            glRotatef(t*0.01, 1, 1, 1);
            glRotatef(90, 1, 0, 0);
            glutSolidTeapot(1);
        glPopMatrix();
        shader.uniform1i("disable_lighting", 0);
    }
};


class SphereMapScene: public Scene {
public:
    GLuint theTexture;
    void start() {
        glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(DOF_Y, ASPECT_RATIO,1,200);
        glMatrixMode(GL_MODELVIEW);
        glEnable (GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glGenTextures (1, &theTexture);
        glBindTexture (GL_TEXTURE_2D, theTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        std::auto_ptr<glimg::ImageSet> pImgSet(glimg::loaders::stb::LoadFromFile("../data/spheremap.jpg"));
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8,
        //             pImgSet->GetImage(0).GetDimensions().width, pImgSet->GetImage(0).GetDimensions().height,
        //             0, GL_RGB, GL_UNSIGNED_BYTE, pImgSet->GetImage(0).GetImageData());
        gluBuild2DMipmaps ( GL_TEXTURE_2D, GL_SRGB8,
                           pImgSet->GetImage(0).GetDimensions().width, pImgSet->GetImage(0).GetDimensions().height,
                           GL_RGB, GL_UNSIGNED_BYTE, pImgSet->GetImage(0).GetImageData());
        glClearColor(135.0/255, 206.0/255, 235.0/255, 0.0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
    }
    void draw() {
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glLoadIdentity();
        gluLookAt(0, 0, -5, 0,0,0, 0, 1, 0);
        glEnable(GL_TEXTURE_2D);
        glColor3f(1, 1, 1);
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
        glRotatef(glutGet(GLUT_ELAPSED_TIME)*0.01, 0, 1, 1);
        glutSolidTeapot(1);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        label("Environment mapping");
    }
};

class TextureScene: public Scene {
public:
    GLuint theTexture;
    shader_prog shader = shader_prog("../src/tex_vertex_shader.glsl", "../src/tex_fragment_shader.glsl");
    int mode;
    const char* text;
    TextureScene(int mode, const char* text): Scene(), mode(mode), text(text) {}
    void start() {
        glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(DOF_Y, ASPECT_RATIO,1,200);
        glMatrixMode(GL_MODELVIEW);
        glEnable (GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glGenTextures (1, &theTexture);
        glBindTexture (GL_TEXTURE_2D, theTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        std::auto_ptr<glimg::ImageSet> pImgSet(glimg::loaders::stb::LoadFromFile("../data/checkerboard.png"));
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8,
        //             pImgSet->GetImage(0).GetDimensions().width, pImgSet->GetImage(0).GetDimensions().height,
        //             0, GL_RGB, GL_UNSIGNED_BYTE, pImgSet->GetImage(0).GetImageData());
        gluBuild2DMipmaps ( GL_TEXTURE_2D, GL_SRGB8,
                           pImgSet->GetImage(0).GetDimensions().width, pImgSet->GetImage(0).GetDimensions().height,
                           GL_RGB, GL_UNSIGNED_BYTE, pImgSet->GetImage(0).GetImageData());
        glClearColor(135.0/255, 206.0/255, 235.0/255, 0.0);
    }

    void draw() {
        shader.use();
        shader.uniform1i("mode", mode);
        glLoadIdentity();
        gluLookAt(0, 0, 4, 0,0,0, 0,1,0);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        const float repeat = 5.0;
        glEnable(GL_TEXTURE_2D);
        glColor3f(1, 1, 1);
        glPushMatrix();
            glRotatef(glutGet(GLUT_ELAPSED_TIME)*0.01, 0, 1, 0);
            glBegin ( GL_QUADS );
                // Front Face
                glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
                glTexCoord2f(repeat, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
                glTexCoord2f(repeat, repeat); glVertex3f( 1.0f,  1.0f,  1.0f);
                glTexCoord2f(0.0f, repeat); glVertex3f(-1.0f,  1.0f,  1.0f);
                // Back Face
                glTexCoord2f(repeat, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
                glTexCoord2f(repeat, repeat); glVertex3f(-1.0f,  1.0f, -1.0f);
                glTexCoord2f(0.0f, repeat); glVertex3f( 1.0f,  1.0f, -1.0f);
                glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
                // Top Face
                glTexCoord2f(0.0f, repeat); glVertex3f(-1.0f,  1.0f, -1.0f);
                glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
                glTexCoord2f(repeat, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
                glTexCoord2f(repeat, repeat); glVertex3f( 1.0f,  1.0f, -1.0f);
                // Bottom Face
                glTexCoord2f(repeat, repeat); glVertex3f(-1.0f, -1.0f, -1.0f);
                glTexCoord2f(0.0f, repeat); glVertex3f( 1.0f, -1.0f, -1.0f);
                glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0, -1.0f,  1.0f);
                glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
                // Right face
                glTexCoord2f(repeat, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
                glTexCoord2f(repeat, repeat); glVertex3f( 1.0f,  1.0f, -1.0f);
                glTexCoord2f(0.0f, repeat); glVertex3f( 1.0f,  1.0f,  1.0f);
                glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
                // Left Face
                glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
                glTexCoord2f(repeat, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
                glTexCoord2f(repeat, repeat); glVertex3f(-1.0f,  1.0f,  1.0f);
                glTexCoord2f(0.0f, repeat); glVertex3f(-1.0f,  1.0f, -1.0f);
            glEnd();
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);
        shader.free();
        label(text);
    }
};

#define GAMMA_CORRECT 12345
class TextureCheckerboardScene: public TextureScene {
public:
    TextureCheckerboardScene(int mode, const char* text): TextureScene(mode, text) {}

    void draw() {
        shader.use();
        int shader_mode = 1;
        shader.uniform1i("mode", shader_mode);
        if (mode == GL_TEXTURE_MAX_ANISOTROPY_EXT) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
        }
        else if (mode == GAMMA_CORRECT) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
            int gamma = 1;
            shader.uniform1i("gamma", gamma);
        }
        else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 0.0f);
        }
        glLoadIdentity();
        gluLookAt(0, 1, 1, 0,0,0.8, 0, 0, 1);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        const float repeat = 100.0;
        glEnable(GL_TEXTURE_2D);
        glColor3f(1, 1, 1);
        glPushMatrix();
            glBegin ( GL_QUADS );
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-100.0f, 0.0f,  0.0f);
            glTexCoord2f(repeat, 0.0f); glVertex3f( 100.0f, 0.0f,  0.0f);
            glTexCoord2f(repeat, repeat); glVertex3f( 100.0f,  -100.0f,  0.0f);
            glTexCoord2f(0.0f, repeat); glVertex3f(-100.0f,  -100.0f,  0.0f);
            glEnd();
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);
        shader.free();
        label(text);
    }
};



class TorusScene: public BaseScene {
public:
    shader_prog shader = shader_prog("../src/base_vertex_shader.glsl", "../src/base_fragment_shader.glsl");
    int mode;
    const char* text;
    TorusScene(int mode, const char* text, const char* fragment_shader = NULL): BaseScene(), mode(mode), text(text) {};

    void start() {
        BaseScene::start();
        float f = pow(0.1, 1/2.2);
        if (mode == 6) glClearColor(f,f,f,0);
        else glClearColor(0, 0, 0, 0);
    }
    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        shader.uniform1i("mode", mode);
        glLoadIdentity();
        gluLookAt(0, -10, 3, 0, 0, 0, 0, 0, 1);
        glPushMatrix();
            glTranslatef(5, -5, 2);
            float light_pos[] = {0, 0, 0, 1};
            glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
        glPopMatrix();
        glRotatef(glutGet(GLUT_ELAPSED_TIME)*0.01, 1, 0, 1);
        glutSolidTorus(1, 3.5, 40, 40);
        shader.free();
        label(text);
    }
};

