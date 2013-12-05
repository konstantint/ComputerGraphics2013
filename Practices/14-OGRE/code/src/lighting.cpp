/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 14: OGRE
 *
 * (Those are preliminary ideas for the practice session)
 */
#include <OgreCommon.h>
#include <OgreManualObject.h>
#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreResourceGroupManager.h>
#include <OgreWindowEventUtilities.h>
#include <OgreEntity.h>

#include "input_util.h"

/**
 * Rather than having separate functions, like we did in GLUT (i.e. display, idle, ...)
 * This time we shall put all of them into a class.
 * Our "main" function (below at the end of the file) simply creates this class and calls its "run"
 * function.
 */
class Application: public Ogre::FrameListener, public OIS::KeyListener {
public:
    Ogre::Root *mRoot;
    Ogre::RenderWindow* mWindow;
    SimpleMouseAndKeyboardListener* mEventListener;

    Ogre::SceneManager* mScene; // The scene

    Ogre::Timer mTimer; // ~ for glutGet(GLUT_ELAPSED_TIME)

    /**
     * Application main function:
     *   Initializes Ogre, creates a window, creates scenes, starts the rendering loop
     */
    void run() {
        // ----------------------- Create root object & window --------------------- //
        // Initialize (~ glutInit)
        mRoot = new Ogre::Root("plugins.cfg");
        mRoot->restoreConfig();                  // Read config from ogre.cfg
        //if(!mRoot->showConfigDialog()) return; // Alternatively, you can show a dialog window here

        // Create window (~ glutCreateWindow)
        mWindow = mRoot->initialise(true, "Basic OGRE example");

        // Register per-frame callbacks (~ glutIdleFunc)
        mRoot->addFrameListener(this);

        // Register keyboard and mouse callbacks (~ glutMouseFunc, glutKeyboardFunc, glutWindowFunc)
        // This class already implements some logic, such as "quit on Escape".
        mEventListener = new SimpleMouseAndKeyboardListener(mWindow, mRoot, this);

        // Also, tell OGRE where to look for data files (textures, materials, etc)
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../data", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups(); // Scan files in the directory we provided above

        // ----------- Create scenes ----------------- //
        // Each scene is represented by a "SceneManager" object, which
        // combines a SceneGraph (set of objects with their model transforms)
        // with a Camera (view-projection transform)
        mScene = createLitTriangleScene();      // Very basic colored triangle

        // Let mScene[0] be the starting "current scene"
        // Configure the window to show the camera from the current scene
        // This is like (~ glViewport), in that you could specify the region of the window to draw to.
        // and have several scenes rendered to different parts in the window.
        mWindow->removeAllViewports();
        Ogre::Viewport* vp = mWindow->addViewport(mScene->getCamera("MainCamera"));

        // ~ glClearColor
        vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

        // ------------------------ Configuration complete ----------------- //
        // Enter the infinite rendering & event processing loop.
        // ~ glutMainLoop()
        mRoot->startRendering();
    }

    // KeyListener (~ keyboardFunc)
    bool keyReleased(const OIS::KeyEvent &arg) {
        return true;
    }
    bool keyPressed(const OIS::KeyEvent &arg) {
        return true;
    }

    // ------------------------- Now the sample scene descriptions ------------------------- //
    // In contrast to OpenGL where we had to emit actual drawing commands,
    // here we describe what will be drawn and store it in a scene graph.
    // Ogre will then perform the drawing on each frame by traversing the scene graph as needed
    // (perhaps in multiple rendering passes).
    //
    /**
     * Triangle with material, lighting and perspective projection.
     */
    Ogre::SceneManager* createLitTriangleScene() {
        Ogre::SceneManager* scene = mRoot->createSceneManager(Ogre::ST_GENERIC);

        // Configure camera (~ view & projection transforms, i.e. gluLookAt + gluPerspective)
        Ogre::Camera* camera = scene->createCamera("MainCamera"); // We can use an arbitrary name here
        camera->setPosition(Ogre::Vector3(0,0,5));                // ~gluLookAt
        camera->lookAt(Ogre::Vector3(0,0,0));                     // Let's look along the negative Z
        camera->setNearClipDistance(0.5);                         // By default, a perspective camera is used
        camera->setFOVy(Ogre::Degree(60.0));
        camera->setAspectRatio((float) mWindow->getWidth() / mWindow->getHeight());

        // Set up lighting (~ glEnable(GL_LIGHTING), glEnable(GL_LIGHT0), glLightfv(...)):
        scene->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));
        Ogre::Light* light = scene->createLight("MainLight");
        light->setType(Ogre::Light::LT_POINT);
        light->setDiffuseColour(1, 1, 1);
        light->setSpecularColour(1, 1, 1);
        light->setPosition(-2, -0.2, 5);

        // Next we need to create a material.
        // A common method is to provide it as a "material script" in the resources directory and
        // then simply refer by name
        // (see http://www.ogre3d.org/docs/manual/manual_14.html, http://www.ogre3d.org/tikiwiki/Materials)
        // However, to understand the inner workings, let's create the material in code here.
        Ogre::MaterialPtr m = Ogre::MaterialManager::getSingleton().create("TriangleMaterial",
                                            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        m->setAmbient(1.0, 0.0, 0.0);
        m->setCullingMode(Ogre::CULL_NONE);
        m->setDiffuse(1.0, 0.0, 0.0, 1.0);
        m->setSpecular(1.0, 1.0, 1.0, 1.0);
        m->setShininess(40);
        m->setLightingEnabled(true);
        m->setDepthCheckEnabled(true);
        m->setDepthWriteEnabled(true);
        m->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
        // .. and etc. Material description lets you specify the whole configuration
        // of the graphics system needed for rendering the primitive,
        // this includes all kinds of parameters, textures, shaders, shader uniform variables, etc.
        // In addition, the material can describe several rendering "techniques",
        // each with potentially several passes.
        // http://www.ogre3d.org/docs/manual/manual_15.html#Techniques
        // http://www.ogre3d.org/docs/manual/manual_16.html#Passes

        // -------- Scene geometry -------- //
        Ogre::ManualObject* triangle = scene->createManualObject("Triangle");
        triangle->begin("TriangleMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);
        triangle->normal(0, 0, 1); // ~glNormal
        triangle->position(0, 1.0, 0);
        triangle->position(-1.0, -1.0, 0);
        triangle->position(1.0, -1.0, 0);
        triangle->end();
        scene->getRootSceneNode()->createChildSceneNode("Triangle")->attachObject(triangle);
        return scene;
    }

    // FrameListener callback (~ idleFunc). Perform animation here
    bool frameRenderingQueued(const Ogre::FrameEvent& evt) {
        float t = mTimer.getMilliseconds()*0.001;

        // Do some simple animations by changing scene node transform parameters
        Ogre::SceneNode* triangle2 = mScene->getSceneNode("Triangle");
        triangle2->resetToInitialState();
        triangle2->rotate(Ogre::Vector3(0, 1, 0), Ogre::Radian(t));

        return true; // Return false to quit
    }
};

int main(int argc, char *argv[]) {
    Application app;
    app.run();
    return 0;
}
