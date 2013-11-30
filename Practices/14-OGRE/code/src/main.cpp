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

    Ogre::SceneManager* mScene[4]; // Three scenes
    int mCurrentSceneId;
    Ogre::SceneManager* mCurrentScene;

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
        mScene[0] = createTriangleScene();      // Very basic colored triangle
        mScene[1] = createLitTriangleScene();   // Triangle with material & lighting
        mScene[2] = createOgreHeadScene();      // Mesh loaded from file
        mScene[3] = createParticleScene();      // Particle effects via ParticleFX plugin

        // Let mScene[0] be the starting "current scene". Use Spacebar to toggle.
        setCurrentScene(0);

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
        if (arg.key == OIS::KC_RETURN || arg.key == OIS::KC_SPACE) {
            setCurrentScene((mCurrentSceneId + 1) % 4);
        }
        return true;
    }

    /**
     * Switches between the three scene graphs.
     */
    void setCurrentScene(int id) {
        mCurrentSceneId = id;
        mCurrentScene = mScene[id];

        // Configure the window to show the camera from the current scene
        // This is like (~ glViewport), in that you could specify the region of the window to draw to.
        // and have several scenes rendered to different parts in the window.
        mWindow->removeAllViewports();
        Ogre::Viewport* vp = mWindow->addViewport(mCurrentScene->getCamera("MainCamera"));

        // ~ glClearColor
        vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));


        // In scene 2 we shall enable control of a camera with mouse & keyboard
        // using the utility class.
        if (id == 2) mEventListener->controlCamera(mCurrentScene->getCamera("MainCamera"));
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
        return scene;
    }

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

    Ogre::SceneManager* createParticleScene() {
        Ogre::SceneManager* scene = mRoot->createSceneManager(Ogre::ST_GENERIC);

        // Configure camera (~ view & projection transforms, i.e. gluLookAt + gluPerspective)
        Ogre::Camera* camera = scene->createCamera("MainCamera"); // We can use an arbitrary name here
        camera->setPosition(Ogre::Vector3(0,0,50));               // ~gluLookAt
        camera->lookAt(Ogre::Vector3(0,0,0));
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
        float t = mTimer.getMilliseconds()*0.001;

        // Do some simple animations for the three scenes by changing scene node transform parameters
        Ogre::SceneNode* triangle = mScene[0]->getSceneNode("Triangle");
        triangle->resetToInitialState();
        triangle->rotate(Ogre::Vector3(0, 0, 1), Ogre::Radian(t));

        Ogre::SceneNode* triangle2 = mScene[1]->getSceneNode("Triangle");
        triangle2->resetToInitialState();
        triangle2->rotate(Ogre::Vector3(0, 1, 0), Ogre::Radian(t));


        Ogre::SceneNode* head = mScene[2]->getSceneNode("SmallHead");
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
