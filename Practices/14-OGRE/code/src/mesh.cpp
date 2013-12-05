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
        mScene = createOgreHeadScene();      // Very basic colored triangle

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
     * A scene that loads a mesh
     */
    Ogre::SceneManager* createOgreHeadScene() {
        Ogre::SceneManager* scene = mRoot->createSceneManager(Ogre::ST_GENERIC);

        // Configure camera (~ view & projection transforms, i.e. gluLookAt + gluPerspective)
        Ogre::Camera* camera = scene->createCamera("MainCamera"); // We can use an arbitrary name here
        camera->setPosition(Ogre::Vector3(0,0,80));               // ~gluLookAt
        camera->lookAt(Ogre::Vector3(0,0,0));
        camera->setNearClipDistance(5);                           // By default, a perspective camera is used
        camera->setFOVy(Ogre::Degree(60.0));
        camera->setAspectRatio((float) mWindow->getWidth() / mWindow->getHeight());

        // Set up lighting (~ glEnable(GL_LIGHTING), glEnable(GL_LIGHT0), glLightfv(...)):
        scene->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
        Ogre::Light* light = scene->createLight("MainLight");
        light->setType(Ogre::Light::LT_POINT);
        light->setDiffuseColour(1, 1, 1);
        light->setSpecularColour(1, 1, 1);
        light->setPosition(20.0f, 80.0f, 50.0f);

        // Enable shadow computations
        scene->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

        // -------- Scene geometry -------- //
        // Load a mesh
        Ogre::Entity* ogreHead = scene->createEntity("Head/Mesh", "ogrehead.mesh");
        // The mesh uses materials specified in the script file Ogre.material.
        // Note that the mesh materials use both usual ambient/diffuse/specular
        // as well as texturing and sphere mapping.

        // Create a scene node and attach to the graph
        scene->getRootSceneNode()->createChildSceneNode("Head")->attachObject(ogreHead);

        // And another head (note that OGRE is smart enough to re-use the existing mesh data)
        Ogre::Entity* smallHead = scene->createEntity("SmallHead/Mesh", "ogrehead.mesh");

        // Attach the small head as a child of headNode
        Ogre::SceneNode* smallHeadNode = scene->getSceneNode("Head")->createChildSceneNode("SmallHead");
        smallHeadNode->attachObject(smallHead);

        // Configure the model transform for SmallHead
        // Note that in OGRE transformations are always applied in the order
        // scale --> rotate --> translate.
        // I.e. the order we specify them does not matter (as it was in OpenGL)
        smallHeadNode->scale(0.2, 0.2, 0.2);
        smallHeadNode->translate(25, 10, 0);

        return scene;
    }

    // FrameListener callback (~ idleFunc). Perform animation here
    bool frameRenderingQueued(const Ogre::FrameEvent& evt) {
        float t = mTimer.getMilliseconds()*0.001;

        Ogre::SceneNode* head = mScene->getSceneNode("SmallHead");
        head->resetToInitialState();
        head->scale(0.2, 0.2, 0.2);
        head->rotate(Ogre::Vector3(0, 1, 0), Ogre::Radian(t));
        head->translate(25*cos(t), 10, 20*sin(t));

        return true; // Return false to quit
    }
};

int main(int argc, char *argv[]) {
    Application app;
    app.run();
    return 0;
}
