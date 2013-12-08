/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 14: OGRE
 *
 * Meshes
 */
#include <OgreCommon.h>
#include <OgreManualObject.h>
#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreResourceGroupManager.h>
#include <OgreWindowEventUtilities.h>
#include <OgreEntity.h>
#include "input_util.h"

using namespace Ogre;

class Application: public FrameListener, public OIS::KeyListener {
public:

    Root *mRoot;
    RenderWindow* mWindow;
    SceneManager* mScene;
    SimpleMouseAndKeyboardListener* mEventListener;

	// Global variables
	Entity* mBodyEnt;
	SceneNode* mBodyNode;

	// Exercise 7: Add variable to store pointer to AnimationState
	// ...

	// Exercise 6: Add direction vector
	// ...

	Real mTimer;

    // ------------------------- Application main function ------------------------- //
    void run() {

        // Create root object and window
        mRoot = new Root();
        mRoot->restoreConfig();                  // read config from ogre.cfg
        //if(!mRoot->showConfigDialog()) return; // alternatively, you can show a dialog window here
        mWindow = mRoot->initialise(true, "Ogre running");
        mRoot->addFrameListener(this);
        mEventListener = new SimpleMouseAndKeyboardListener(mWindow, mRoot, this);

        // Load resources
        ResourceGroupManager::getSingleton().addResourceLocation("../data/Character", "FileSystem");
        ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

        // Create scene
        mScene = createOgreScene();
        mWindow->removeAllViewports();
        Viewport* vp = mWindow->addViewport(mScene->getCamera("MainCamera"));
        vp->setBackgroundColour(ColourValue(0, 0, 0));

        // Start rendering loop
        mRoot->startRendering();
    }

    // ------------------------- Keyboard listener ------------------------- //
    bool keyPressed(const OIS::KeyEvent &arg) {

		// Change direction
		// Exercise 6: Move character using WASD buttons
		// For example the following checks if "W" button is pressed
		// if (arg.key == OIS::KC_W) do domething
		// ...

        // Start running
        // Exercise 7: Enable running animation
        // ...

		return true;
    }
    bool keyReleased(const OIS::KeyEvent &arg) {

        // Cancel direction
        // Exercise 6: Make character stop when key is released
        // ...

        // Stop running
        // Exercise 7: Disable running animation
        // ..

		return true;
    }

    // ------------------------- Scene description ------------------------- //
    SceneManager* createOgreScene() {
        SceneManager* scene = mRoot->createSceneManager(ST_GENERIC);

        // Initialize camera
        Camera* camera = scene->createCamera("MainCamera");
        camera->setPosition(Vector3(0, 10, 30));
        camera->lookAt(Vector3(0, 0, 0));
        camera->setNearClipDistance(0.5);
        camera->setFOVy(Ogre::Degree(60.0));
        camera->setAspectRatio((float) mWindow->getWidth() / mWindow->getHeight());

        // Set up lighting (~ glEnable(GL_LIGHTING), glEnable(GL_LIGHT0), glLightfv(...)):
        scene->setAmbientLight(ColourValue(0.5f, 0.5f, 0.5f));
        Light* light = scene->createLight("MainLight");
        light->setType(Light::LT_POINT);
        light->setDiffuseColour(1, 1, 1);
        light->setSpecularColour(1, 1, 1);
        light->setPosition(20.0f, 80.0f, 50.0f);

        // Load character mesh
		mBodyNode = scene->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y * 5);
		mBodyEnt = scene->createEntity("SinbadBody", "Sinbad.mesh");
		mBodyNode->attachObject(mBodyEnt);

        // Load animations
        // Exercise 7: Get pointer to animation state
        // ...

        // Enable shadow computations
        // Exercise 5: Enable shadows
        // ..

        // Scenery
        // Exercise 5: Add the floor with repeating texture
        // ..

        return scene;
    }

    // ------------------------- Helper functions ------------------------- //
    // FrameListener callback (~ idleFunc). Perform animation here
    bool frameRenderingQueued(const FrameEvent& evt) {
        animateObjects(evt.timeSinceLastFrame);
        return true;
    }

    // Update animation and body position with time
    void animateObjects(Real deltaTime) {

		// Exercise 6: update character position and
        // turn him accoring to the current direction
        // ...

        // Exercise 7: Change animation step using addTime() method of AnimationState object
        // ..

	}

};

// Program entry point
int main(int argc, char *argv[]) {
    Application app;
    app.run();
    return 0;
}
