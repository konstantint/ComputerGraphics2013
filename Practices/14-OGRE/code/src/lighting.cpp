/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 14: OGRE
 *
 * Lighting, materials and
 */
#include <OgreCommon.h>
#include <OgreManualObject.h>
#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreResourceGroupManager.h>
#include <OgreWindowEventUtilities.h>
#include <OgreEntity.h>

#include "input_util.h"

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
        mRoot->restoreConfig();                  // Read config from ogre.cfg
        //if(!mRoot->showConfigDialog()) return; // Alternatively, you can show a dialog window here
        mWindow = mRoot->initialise(true, "Light, Material and Textures");
        mRoot->addFrameListener(this);
        mEventListener = new SimpleMouseAndKeyboardListener(mWindow, mRoot, this);

        // Also, tell OGRE where to look for data files (textures, materials, etc)
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../data", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups(); // Scan files in the directory we provided above

        // ----------- Create scene ----------------- //
        mScene = createLitSphereScene();

        // Set current scene
        mWindow->removeAllViewports();
        Ogre::Viewport* vp = mWindow->addViewport(mScene->getCamera("MainCamera"));
        vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

        // Enable control of a camera with mouse & keyboard using the utility class.
        mEventListener->controlCamera(mScene->getCamera("MainCamera"));

        // ------------------------ Configuration complete ----------------- //
        mRoot->startRendering();
    }

    // KeyListener (~ keyboardFunc)
    bool keyReleased(const OIS::KeyEvent &arg) {
        return true;
    }
    bool keyPressed(const OIS::KeyEvent &arg) {
        return true;
    }

    // ------------------------- Description of the scene ------------------------- //
    /**
     * Triangle with material, lighting and perspective projection.
     */
    Ogre::SceneManager* createLitSphereScene() {
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

        // Positinal light
        Ogre::Light* posLight = scene->createLight("PosLight");
        posLight->setType(Ogre::Light::LT_POINT);
        posLight->setDiffuseColour(0.7, 0.7, 0.7);
        posLight->setSpecularColour(0.7, 0.7, 0.7);
        posLight->setPosition(3, 4, 10);

        // Next we need to create a material.
        // A common method is to provide it as a "material script" in the resources directory and
        // then simply refer by name
        // (see http://www.ogre3d.org/docs/manual/manual_14.html, http://www.ogre3d.org/tikiwiki/Materials)
        // However, to understand the inner workings, let's create the material in code here
        Ogre::MaterialPtr m = Ogre::MaterialManager::getSingleton().create("SphereMaterialCode",
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

        // Sphere
        Ogre::Entity* sphere = scene->createEntity("Sphere", Ogre::SceneManager::PT_SPHERE);
        Ogre::SceneNode* sphereNode = scene->getRootSceneNode()->createChildSceneNode("SphereNode");
        sphereNode->attachObject(sphere);
        sphereNode->setPosition(Ogre::Vector3(0.0, 0.0, 0.0));
        sphereNode->setScale(Ogre::Vector3(0.02, 0.02, 0.02));

        // Exercise 2: Change material name to the one you described in .material file
        sphere->setMaterialName("SphereMaterialCode");

        // Plane
        Ogre::Entity* plane = scene->createEntity("Plane", Ogre::SceneManager::PT_PLANE);
        Ogre::SceneNode* planeNode = scene->getRootSceneNode()->createChildSceneNode("PlaneNode");
        planeNode->attachObject(plane);
        planeNode->rotate(Ogre::Vector3(-1, 0, 0), Ogre::Degree(90));
        planeNode->translate(0, -10, 0);

        // Exercise 3: Add your favourite picture as texture for the plane
        // ...

        // Exercise 3: Enable SkyDome or SkyBox
        // ...

        return scene;
    }

    // FrameListener callback (~ idleFunc). Perform animation here
    bool frameRenderingQueued(const Ogre::FrameEvent& evt) {
        float t = mTimer.getMilliseconds()*0.001;

        // Here you can do some simple animations by playing with scene node transformation methods

        return true; // Return false to quit
    }

};

int main(int argc, char *argv[]) {
    Application app;
    app.run();
    return 0;
}
