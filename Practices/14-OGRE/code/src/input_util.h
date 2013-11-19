/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 14: OGRE
 *
 * Utility class, hiding the specifics of Ogre Mouse & Keyboard processing.
 */
#include <OgreRenderWindow.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkCameraMan.h>

class SimpleMouseAndKeyboardListener:
    public OIS::KeyListener,
    public OIS::MouseListener,
    public Ogre::WindowEventListener,
    public Ogre::FrameListener {
public:
    OgreBites::SdkCameraMan* mCameraMan;       // basic camera controller
    Ogre::Camera* mCamera;

    //OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse*    mMouse;
    OIS::Keyboard* mKeyboard;

    // Key and Mouse listeners we can forward events to
    OIS::KeyListener* mKeyListener;
    OIS::MouseListener* mMouseListener;
    bool mShutdown;

    SimpleMouseAndKeyboardListener(Ogre::RenderWindow* window, Ogre::Root* root, OIS::KeyListener* key = 0, OIS::MouseListener* mouse = 0):
        mCameraMan(0), mCamera(0), mKeyListener(key), mMouseListener(mouse), mShutdown(false) {
        OIS::ParamList pl;
        size_t windowHnd = 0;
        std::ostringstream windowHndStr;

        window->getCustomAttribute("WINDOW", &windowHnd);
        windowHndStr << windowHnd;
        pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

        mInputManager = OIS::InputManager::createInputSystem(pl);

        mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
        mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));

        mMouse->setEventCallback(this);
        mKeyboard->setEventCallback(this);
        root->addFrameListener(this);
        Ogre::WindowEventUtilities::addWindowEventListener(window, this);
    }

    ~SimpleMouseAndKeyboardListener() {
        if (mCameraMan) delete mCameraMan;
    }

    // Starts using keyboard and mouse to control given camera
    void controlCamera(Ogre::Camera* cam) {
        if (mCameraMan) delete mCameraMan;
        mCameraMan = new OgreBites::SdkCameraMan(cam);
        mCamera = cam;
    }

    // Ogre::WindowEventListener
    void windowClosed(Ogre::RenderWindow* rw) {
        mShutdown = true;
    }

    // OIS::KeyListener
    bool keyPressed(const OIS::KeyEvent &arg) {
        if (arg.key == OIS::KC_ESCAPE) {
            mShutdown = true;
        }
        else if (arg.key == OIS::KC_R && mCamera) {  // cycle polygon rendering mode
            switch (mCamera->getPolygonMode()) {
            case Ogre::PM_SOLID:
                mCamera->setPolygonMode(Ogre::PM_WIREFRAME);
                break;
            case Ogre::PM_WIREFRAME:
                mCamera->setPolygonMode(Ogre::PM_POINTS);
                break;
            default:
                mCamera->setPolygonMode(Ogre::PM_SOLID);
                break;
            }
        }

        if (mCameraMan) mCameraMan->injectKeyDown(arg);
        return mKeyListener ? mKeyListener->keyPressed(arg) : true;
    }

    bool keyReleased(const OIS::KeyEvent &arg) {
        if (mCameraMan) mCameraMan->injectKeyUp(arg);
        return mKeyListener ? mKeyListener->keyReleased(arg) : true;
    }

    // OIS::MouseListener
    bool mouseMoved(const OIS::MouseEvent &arg) {
        if (mCameraMan) mCameraMan->injectMouseMove(arg);
        return mMouseListener ? mMouseListener->mouseMoved(arg) : true;
    }

    bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id) {
        if (mCameraMan) mCameraMan->injectMouseDown(arg, id);
        return mMouseListener ? mMouseListener->mousePressed(arg, id) : true;
    }

    bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id) {
        if (mCameraMan) mCameraMan->injectMouseUp(arg, id);
        return mMouseListener ? mMouseListener->mouseReleased(arg, id) : true;
    }

    // Ogre::FrameListener
    bool frameRenderingQueued(const Ogre::FrameEvent& evt) {
        if (mCameraMan) mCameraMan->frameRenderingQueued(evt);
        mKeyboard->capture();
        mMouse->capture();
        return !mShutdown;
    }
};

