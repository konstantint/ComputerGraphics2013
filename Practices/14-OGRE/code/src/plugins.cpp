/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 14: OGRE
 *
 * Plugins
 */
#include <iostream>
#include <OgreCommon.h>
#include <OgreManualObject.h>
#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreResourceGroupManager.h>
#include <OgreWindowEventUtilities.h>
#include <OgreEntity.h>

#include "input_util.h"

// Exercise 8: make Application class inherit OIS::MouseListener class
// ...
class Application: public Ogre::FrameListener, public OIS::KeyListener {
public:
    Ogre::Root *mRoot;
    Ogre::RenderWindow* mWindow;
    SimpleMouseAndKeyboardListener* mEventListener;

    Ogre::SceneManager* mScene;

    Ogre::Timer mTimer;

    /**
     * Application main function:
     *   Initializes Ogre, creates a window, creates scenes, starts the rendering loop
     */
    void run() {
        // ----------------------- Create root object & window --------------------- //
        mRoot = new Ogre::Root("plugins.cfg");
        mRoot->restoreConfig();
        //if(!mRoot->showConfigDialog()) return; // Alternatively, you can show a dialog window here
        mWindow = mRoot->initialise(true, "Basic OGRE example");
        mRoot->addFrameListener(this);

        // Exercise 8: Read class description in input_util.h and update this line
        // ...
        mEventListener = new SimpleMouseAndKeyboardListener(mWindow, mRoot, this);

        // Also, tell OGRE where to look for data files (textures, materials, etc)
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../data", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups(); // Scan files in the directory we provided above

        // ----------- Create scene ----------------- //
        mScene = createParticleScene();      // Very basic colored triangle
        mWindow->removeAllViewports();
        Ogre::Viewport* vp = mWindow->addViewport(mScene->getCamera("MainCamera"));
        vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

        // Enable control of a camera with mouse & keyboard using the utility class.
        mEventListener->controlCamera(mScene->getCamera("MainCamera"));

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

    // Exercise 8: add mouse listener here
    // ...

    // ------------------------- Now the sample scene descriptions ------------------------- //
    Ogre::SceneManager* createParticleScene() {
        Ogre::SceneManager* scene = mRoot->createSceneManager(Ogre::ST_GENERIC);

        // Configure camera
        Ogre::Camera* camera = scene->createCamera("MainCamera");
        camera->setPosition(Ogre::Vector3(0, 0, 150));
        camera->lookAt(Ogre::Vector3(0, 0, 0));
        camera->setNearClipDistance(0.5);
        camera->setFOVy(Ogre::Degree(60.0));
        camera->setAspectRatio((float) mWindow->getWidth() / mWindow->getHeight());

        // -------- Scene geometry -------- //
        // Create a particle system
        Ogre::ParticleSystem* psystem = scene->createParticleSystem("Jet", "Examples/JetEngine1");
        scene->getRootSceneNode()->createChildSceneNode("Particle")->attachObject((Ogre::MovableObject*)psystem);

        return scene;
    }

    // FrameListener callback (~ idleFunc). Perform animation here
    bool frameRenderingQueued(const Ogre::FrameEvent& evt) {

        // Exercise 8: update particle source potision here
        // ...

        return true;
    }
};

int main(int argc, char *argv[]) {
    Application app;
    app.run();
    return 0;
}
