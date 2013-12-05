/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 14: OGRE
 *
 * Simple triangle
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
        mRoot = new Ogre::Root();
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
        mScene = createTriangleScene();      // Very basic colored triangle

        // Configure the window to show the camera from the current scene
        // This is like (~ glViewport), in that you could specify the region of the window to draw to.
        // and have several scenes rendered to different parts in the window.
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
     * This is the most basic "triangle" example, done as a Scene in Ogre.
     */
    Ogre::SceneManager* createTriangleScene() {
        Ogre::SceneManager* scene = mRoot->createSceneManager(Ogre::ST_GENERIC);

        // Configure camera (~ view & projection transforms, i.e. gluLookAt + glOrtho)
        Ogre::Camera* camera = scene->createCamera("MainCamera"); // We can use an arbitrary name here
        camera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
        camera->setOrthoWindow(2, 2);              // ~ glOrtho(-1, 1, -1, 1)
        camera->setAspectRatio((float) mWindow->getWidth() / mWindow->getHeight());
        camera->setNearClipDistance(0.5);
        camera->setPosition(Ogre::Vector3(0,0,1)); // Move camera away from (0, 0, 0), otherwise the triangle at z=0 will be clipped

        // Now add some geometry to the scene
        Ogre::ManualObject* triangle = scene->createManualObject("Triangle");

        // ~ glBegin, glVertex, glEnd
        // "BaseWhiteNoLighting" is a built-in name for a basic non-lit material
        triangle->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
        triangle->position(0, 0.5, 0);            // ~ glVertex.
                                                  // Contrary to OpenGL we *first* must create the vertex
        triangle->colour(Ogre::ColourValue::Red); // .. and then provide its attributes such as color (~ glColor)
        triangle->position(-0.5, -0.5, 0);
        triangle->colour(Ogre::ColourValue::Green);
        triangle->position(0.5, -0.5, 0);
        triangle->colour(Ogre::ColourValue::Blue);
        triangle->end();

        // Add the created triangle object to the scene graph
        // For this we create a SceneNode object, which will combine information about
        // the object's geometry with its modeling transform
        // (see frameRenderingQueued to understand how to rotate the triangle by changing this transform)
        scene->getRootSceneNode()->createChildSceneNode("Triangle")->attachObject(triangle);

        // Exercise 1: Create new object, add vertices, attach the objec to a new SceneNode
        // ...

        return scene;
    }

    // FrameListener callback (~ idleFunc). Perform animation here
    bool frameRenderingQueued(const Ogre::FrameEvent& evt) {
        float t = mTimer.getMilliseconds()*0.001;

        // Do some simple animations by changing scene node transform parameters
        Ogre::SceneNode* triangle = mScene->getSceneNode("Triangle");
        triangle->resetToInitialState();
        triangle->rotate(Ogre::Vector3(0, 0, 1), Ogre::Radian(t));

        // Exercise 1: Use SceneNode to manipulate the object and create anit
        // ...

        return true; // Return false to quit
    }
};

int main(int argc, char *argv[]) {
    Application app;
    app.run();
    return 0;
}
