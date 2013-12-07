/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 14: OGRE
 *
 * Meshes
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

using namespace Ogre;

#define NUM_ANIMS 13           // number of animations the character has
#define CHAR_HEIGHT 5          // height of character's center of mass above ground
#define CAM_HEIGHT 2           // height of camera above character's center of mass
#define RUN_SPEED 17           // character running speed in units per second
#define TURN_SPEED 500.0f      // character turning in degrees per second
#define ANIM_FADE_SPEED 7.5f   // animation crossfade speed in % of full weight per second
#define JUMP_ACCEL 30.0f       // character jump acceleration in upward units per squared second
#define GRAVITY 90.0f          // gravity in downward units per squared second

class Application: public FrameListener, public OIS::KeyListener, public OIS::MouseListener {
public:

    Root *mRoot;
    RenderWindow* mWindow;
    SceneManager* mScene;
    SimpleMouseAndKeyboardListener* mEventListener;

    // List of available movements (animations)
    enum AnimID {
		ANIM_IDLE_BASE,
		ANIM_IDLE_TOP,
		ANIM_RUN_BASE,
		ANIM_RUN_TOP,
		ANIM_HANDS_CLOSED,
		ANIM_HANDS_RELAXED,
		ANIM_DRAW_SWORDS,
		ANIM_SLICE_VERTICAL,
		ANIM_SLICE_HORIZONTAL,
		ANIM_DANCE,
		ANIM_JUMP_START,
		ANIM_JUMP_LOOP,
		ANIM_JUMP_END,
		ANIM_NONE
	};



    // ------------------------- Application main function ------------------------- //

    void run() {

        // ----------------------- Create root object & window --------------------- //
        mRoot = new Root();
        mRoot->restoreConfig();                  // Read config from ogre.cfg
        //if(!mRoot->showConfigDialog()) return; // Alternatively, you can show a dialog window here
        mWindow = mRoot->initialise(true, "Ogre running");
        mRoot->addFrameListener(this);
        mEventListener = new SimpleMouseAndKeyboardListener(mWindow, mRoot, this, this);

        // Load resources
        ResourceGroupManager::getSingleton().addResourceLocation("../data/Character", "FileSystem");
        ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

        // ----------- Create scene ----------------- //
        mScene = createOgreHeadScene();
        mWindow->removeAllViewports();
        Viewport* vp = mWindow->addViewport(mScene->getCamera("MainCamera"));
        vp->setBackgroundColour(ColourValue(0, 0, 0));

        // ------------------------ Configuration complete ----------------- //
        mRoot->startRendering();
    }



    // ------------------------- Keyboard and mouse listeners ------------------------- //

    // KeyListener
    bool keyPressed(const OIS::KeyEvent &arg) {

        // Draw swords
        if (arg.key == OIS::KC_Q && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP)) {
			// Exercise 6: Draw swords
            // ...
			mTimer = 0;
		}

		// Change direction
		else if (arg.key == OIS::KC_W) mKeyDirection.z = -1;
		// Exercise 6: Add other three directions
        // ...

        // Jump
		else if (arg.key == OIS::KC_SPACE && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP)) {
			// Exercise 6: Start jump animation
            // ...
			mTimer = 0;
		}

        // Start running if not already moving and the player wants to move
		if (!mKeyDirection.isZeroLength() && mBaseAnimID == ANIM_IDLE_BASE) {
			setBaseAnimation(ANIM_RUN_BASE, true);
			if (mTopAnimID == ANIM_IDLE_TOP) setTopAnimation(ANIM_RUN_TOP, true);
		}
		return true;
    }
    bool keyReleased(const OIS::KeyEvent &arg) {

        // Cancel movement when button is released
        if (arg.key == OIS::KC_W && mKeyDirection.z == -1) mKeyDirection.z = 0;
        // Exercise 6: Stop moving
        // ...

        // Stop running if already moving and the player doesn't want to move
		if (mKeyDirection.isZeroLength() && mBaseAnimID == ANIM_RUN_BASE) {
			setBaseAnimation(ANIM_IDLE_BASE);
			if (mTopAnimID == ANIM_RUN_TOP) setTopAnimation(ANIM_IDLE_TOP);
		}
		return true;
    }

    // MouseListener
    bool mouseMoved(const OIS::MouseEvent &arg) {
        updateCameraGoal(-0.05f * arg.state.X.rel, -0.05f * arg.state.Y.rel, -0.0005f * arg.state.Z.rel);
        return true;
    }
    bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id) {

        // Swing swords
        if (mSwordsDrawn && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP)) {
			if (id == OIS::MB_Left) setTopAnimation(ANIM_SLICE_VERTICAL, true);
			// Exercise 6: Horizontal swing on left mouse click
			// ...
			mTimer = 0;
		}
        return true;
    }
    bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id) {
        return true;
    }



    // ------------------------- Scene description ------------------------- //

    SceneManager* createOgreHeadScene() {
        SceneManager* scene = mRoot->createSceneManager(ST_GENERIC);

        // Initialize camera
        Camera* camera = scene->createCamera("MainCamera");
        camera->setPosition(Vector3(0, 0.5, 3));
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

        // Enable shadow computations
        scene->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

        // -------- Character -------- //
        setupCamera(camera);
        setupBody(scene);
        setupAnimations();

        // -------- Scene ----------- //
        Ogre::Entity* plane = scene->createEntity("Plane", Ogre::SceneManager::PT_PLANE);
        Ogre::SceneNode* planeNode = scene->getRootSceneNode()->createChildSceneNode("PlaneNode");
        planeNode->attachObject(plane);
        planeNode->rotate(Ogre::Vector3(-1, 0, 0), Ogre::Degree(90));
        plane->setCastShadows(false);
        plane->setMaterialName("Floor/Marble");

        return scene;
    }



    // ------------------------- Lots of helper functions ------------------------- //

    // update everything with time
    void addTime(Real deltaTime) {
		updateBody(deltaTime);
		updateAnimations(deltaTime);
		updateCamera(deltaTime);
	}

    // FrameListener callback (~ idleFunc). Perform animation here
    bool frameRenderingQueued(const FrameEvent& evt) {
        addTime(evt.timeSinceLastFrame);
        return true;
    }

    // Load character meshes
    void setupBody(SceneManager* sceneMgr)
	{
		// create main model
		mBodyNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y * CHAR_HEIGHT);
		mBodyEnt = sceneMgr->createEntity("SinbadBody", "Sinbad.mesh");
		mBodyNode->attachObject(mBodyEnt);

		// create swords and attach to sheath
		mSword1 = sceneMgr->createEntity("SinbadSword1", "Sword.mesh");
		mSword2 = sceneMgr->createEntity("SinbadSword2", "Sword.mesh");
		mBodyEnt->attachObjectToBone("Sheath.L", mSword1);
		mBodyEnt->attachObjectToBone("Sheath.R", mSword2);

        // initial movement direction and jump velocity
		mKeyDirection = Vector3::ZERO;
		mVerticalVelocity = 0;
	}

    // Load animations
	void setupAnimations()
	{
		// this is very important due to the nature of the exported animations
		mBodyEnt->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);

		String animNames[] = {"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed",
		"DrawSwords","SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};

		// populate our animation list
		for (int i = 0; i < NUM_ANIMS; i++) {
			mAnims[i] = mBodyEnt->getAnimationState(animNames[i]);
			mAnims[i]->setLoop(true);
			mFadingIn[i] = false;
			mFadingOut[i] = false;
		}

		// start off in the idle state (top and bottom together)
		setBaseAnimation(ANIM_IDLE_BASE);
		setTopAnimation(ANIM_IDLE_TOP);

		// relax the hands since we're not holding anything
		mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
		mSwordsDrawn = false;
	}

    // Setup camera
	void setupCamera(Camera* cam)
	{
		// create a pivot at roughly the character's shoulder
		mCameraPivot = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();
		// this is where the camera should be soon, and it spins around the pivot
		mCameraGoal = mCameraPivot->createChildSceneNode(Vector3(0, 0, 15));
		// this is where the camera actually is
		mCameraNode = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();
		mCameraNode->setPosition(mCameraPivot->getPosition() + mCameraGoal->getPosition());

		mCameraPivot->setFixedYawAxis(true);
		mCameraGoal->setFixedYawAxis(true);
		mCameraNode->setFixedYawAxis(true);

		// our model is quite small, so reduce the clipping planes
		cam->setNearClipDistance(0.1);
		cam->setFarClipDistance(100);
		mCameraNode->attachObject(cam);

		mPivotPitch = 0;
	}

    // Update character with time
	void updateBody(Real deltaTime)
	{
		mGoalDirection = Vector3::ZERO;

		if (mKeyDirection != Vector3::ZERO && mBaseAnimID != ANIM_DANCE)
		{
			// calculate actually goal direction in world based on player's key directions
			mGoalDirection += mKeyDirection.z * mCameraNode->getOrientation().zAxis();
			mGoalDirection += mKeyDirection.x * mCameraNode->getOrientation().xAxis();
			mGoalDirection.y = 0;
			mGoalDirection.normalise();

			Quaternion toGoal = mBodyNode->getOrientation().zAxis().getRotationTo(mGoalDirection);

			// calculate how much the character has to turn to face goal direction
			Real yawToGoal = toGoal.getYaw().valueDegrees();
			// this is how much the character CAN turn this frame
			Real yawAtSpeed = yawToGoal / Math::Abs(yawToGoal) * deltaTime * TURN_SPEED;
			// reduce "turnability" if we're in midair
			if (mBaseAnimID == ANIM_JUMP_LOOP) yawAtSpeed *= 0.2f;

			// turn as much as we can, but not more than we need to
			if (yawToGoal < 0) yawToGoal = std::min<Real>(0, std::max<Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, yawAtSpeed, 0);
			else if (yawToGoal > 0) yawToGoal = std::max<Real>(0, std::min<Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, 0, yawAtSpeed);

			mBodyNode->yaw(Degree(yawToGoal));

			// move in current body direction (not the goal direction)
            // Exercise 6: Forbid from running out of the plane
            // ...
			mBodyNode->translate(0, 0, deltaTime * RUN_SPEED * mAnims[mBaseAnimID]->getWeight(), Node::TS_LOCAL);

		}

		if (mBaseAnimID == ANIM_JUMP_LOOP)
		{
			// if we're jumping, add a vertical offset too, and apply gravity
			mBodyNode->translate(0, mVerticalVelocity * deltaTime, 0, Node::TS_LOCAL);
			mVerticalVelocity -= GRAVITY * deltaTime;

			Vector3 pos = mBodyNode->getPosition();
			if (pos.y <= CHAR_HEIGHT)
			{
				// if we've hit the ground, change to landing state
				pos.y = CHAR_HEIGHT;
				mBodyNode->setPosition(pos);
				setBaseAnimation(ANIM_JUMP_END, true);
				mTimer = 0;
			}
		}
	}

    // Update animation with time
	void updateAnimations(Real deltaTime)
	{
		Real baseAnimSpeed = 1;
		Real topAnimSpeed = 1;

		mTimer += deltaTime;

		if (mTopAnimID == ANIM_DRAW_SWORDS)
		{
			// flip the draw swords animation if we need to put it back
			topAnimSpeed = mSwordsDrawn ? -1 : 1;

			// half-way through the animation is when the hand grasps the handles...
			if (mTimer >= mAnims[mTopAnimID]->getLength() / 2 &&
				mTimer - deltaTime < mAnims[mTopAnimID]->getLength() / 2)
			{
				// so transfer the swords from the sheaths to the hands
				mBodyEnt->detachAllObjectsFromBone();
				mBodyEnt->attachObjectToBone(mSwordsDrawn ? "Sheath.L" : "Handle.L", mSword1);
				mBodyEnt->attachObjectToBone(mSwordsDrawn ? "Sheath.R" : "Handle.R", mSword2);
				// change the hand state to grab or let go
				mAnims[ANIM_HANDS_CLOSED]->setEnabled(!mSwordsDrawn);
				mAnims[ANIM_HANDS_RELAXED]->setEnabled(mSwordsDrawn);
			}

			if (mTimer >= mAnims[mTopAnimID]->getLength())
			{
				// animation is finished, so return to what we were doing before
				if (mBaseAnimID == ANIM_IDLE_BASE) setTopAnimation(ANIM_IDLE_TOP);
				else
				{
					setTopAnimation(ANIM_RUN_TOP);
					mAnims[ANIM_RUN_TOP]->setTimePosition(mAnims[ANIM_RUN_BASE]->getTimePosition());
				}
				mSwordsDrawn = !mSwordsDrawn;
			}
		}
		else if (mTopAnimID == ANIM_SLICE_VERTICAL || mTopAnimID == ANIM_SLICE_HORIZONTAL)
		{
			if (mTimer >= mAnims[mTopAnimID]->getLength())
			{
				// animation is finished, so return to what we were doing before
				if (mBaseAnimID == ANIM_IDLE_BASE) setTopAnimation(ANIM_IDLE_TOP);
				else
				{
					setTopAnimation(ANIM_RUN_TOP);
					mAnims[ANIM_RUN_TOP]->setTimePosition(mAnims[ANIM_RUN_BASE]->getTimePosition());
				}
			}

			// don't sway hips from side to side when slicing. that's just embarrassing.
			if (mBaseAnimID == ANIM_IDLE_BASE) baseAnimSpeed = 0;
		}
		else if (mBaseAnimID == ANIM_JUMP_START)
		{
			if (mTimer >= mAnims[mBaseAnimID]->getLength())
			{
				// takeoff animation finished, so time to leave the ground!
				setBaseAnimation(ANIM_JUMP_LOOP, true);
				// apply a jump acceleration to the character
				mVerticalVelocity = JUMP_ACCEL;
			}
		}
		else if (mBaseAnimID == ANIM_JUMP_END)
		{
			if (mTimer >= mAnims[mBaseAnimID]->getLength())
			{
				// safely landed, so go back to running or idling
				if (mKeyDirection == Vector3::ZERO)
				{
					setBaseAnimation(ANIM_IDLE_BASE);
					setTopAnimation(ANIM_IDLE_TOP);
				}
				else
				{
					setBaseAnimation(ANIM_RUN_BASE, true);
					setTopAnimation(ANIM_RUN_TOP, true);
				}
			}
		}

		// increment the current base and top animation times
		if (mBaseAnimID != ANIM_NONE) mAnims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);
		if (mTopAnimID != ANIM_NONE) mAnims[mTopAnimID]->addTime(deltaTime * topAnimSpeed);

		// apply smooth transitioning between our animations
		fadeAnimations(deltaTime);
	}

    // Smooth transition between anitmations
	void fadeAnimations(Real deltaTime)
	{
		for (int i = 0; i < NUM_ANIMS; i++)
		{
			if (mFadingIn[i])
			{
				// slowly fade this animation in until it has full weight
				Real newWeight = mAnims[i]->getWeight() + deltaTime * ANIM_FADE_SPEED;
				mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
				if (newWeight >= 1) mFadingIn[i] = false;
			}
			else if (mFadingOut[i])
			{
				// slowly fade this animation out until it has no weight, and then disable it
				Real newWeight = mAnims[i]->getWeight() - deltaTime * ANIM_FADE_SPEED;
				mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
				if (newWeight <= 0)
				{
					mAnims[i]->setEnabled(false);
					mFadingOut[i] = false;
				}
			}
		}
	}

    // Update camera with time
	void updateCamera(Real deltaTime)
	{
		// place the camera pivot roughly at the character's shoulder
		mCameraPivot->setPosition(mBodyNode->getPosition() + Vector3::UNIT_Y * CAM_HEIGHT);
		// move the camera smoothly to the goal
		Vector3 goalOffset = mCameraGoal->_getDerivedPosition() - mCameraNode->getPosition();
		mCameraNode->translate(goalOffset * deltaTime * 9.0f);
		// always look at the pivot
		mCameraNode->lookAt(mCameraPivot->_getDerivedPosition(), Node::TS_WORLD);
	}

	void updateCameraGoal(Real deltaYaw, Real deltaPitch, Real deltaZoom)
	{
		mCameraPivot->yaw(Degree(deltaYaw), Node::TS_WORLD);

		// bound the pitch
		if (!(mPivotPitch + deltaPitch > 25 && deltaPitch > 0) &&
			!(mPivotPitch + deltaPitch < -60 && deltaPitch < 0))
		{
			mCameraPivot->pitch(Degree(deltaPitch), Node::TS_LOCAL);
			mPivotPitch += deltaPitch;
		}

		Real dist = mCameraGoal->_getDerivedPosition().distance(mCameraPivot->_getDerivedPosition());
		Real distChange = deltaZoom * dist;

		// bound the zoom
		if (!(dist + distChange < 8 && distChange < 0) &&
			!(dist + distChange > 25 && distChange > 0))
		{
			mCameraGoal->translate(0, 0, distChange, Node::TS_LOCAL);
		}
	}

    // Set current animation for lower body
	void setBaseAnimation(AnimID id, bool reset = false)
	{
		if (mBaseAnimID >= 0 && mBaseAnimID < NUM_ANIMS)
		{
			// if we have an old animation, fade it out
			mFadingIn[mBaseAnimID] = false;
			mFadingOut[mBaseAnimID] = true;
		}

		mBaseAnimID = id;

		if (id != ANIM_NONE)
		{
			// if we have a new animation, enable it and fade it in
			mAnims[id]->setEnabled(true);
			mAnims[id]->setWeight(0);
			mFadingOut[id] = false;
			mFadingOut[id] = false;
			mFadingIn[id] = true;
			if (reset) mAnims[id]->setTimePosition(0);
		}
	}

    // Set current animation for upper body
	void setTopAnimation(AnimID id, bool reset = false)
	{
		if (mTopAnimID >= 0 && mTopAnimID < NUM_ANIMS)
		{
			// if we have an old animation, fade it out
			mFadingIn[mTopAnimID] = false;
			mFadingOut[mTopAnimID] = true;
		}

		mTopAnimID = id;

		if (id != ANIM_NONE)
		{
			// if we have a new animation, enable it and fade it in
			mAnims[id]->setEnabled(true);
			mAnims[id]->setWeight(0);
			mFadingOut[id] = false;
			mFadingIn[id] = true;
			if (reset) mAnims[id]->setTimePosition(0);
		}
	}

    // Lots of variables
	Camera* mCamera;
	SceneNode* mBodyNode;
	SceneNode* mCameraPivot;
	SceneNode* mCameraGoal;
	SceneNode* mCameraNode;
	Real mPivotPitch;
	Entity* mBodyEnt;
	Entity* mSword1;
	Entity* mSword2;
	AnimationState* mAnims[NUM_ANIMS];    // master animation list
	AnimID mBaseAnimID;                   // current base (full- or lower-body) animation
	AnimID mTopAnimID;                    // current top (upper-body) animation
	bool mFadingIn[NUM_ANIMS];            // which animations are fading in
	bool mFadingOut[NUM_ANIMS];           // which animations are fading out
	bool mSwordsDrawn;
	Vector3 mKeyDirection;      // player's local intended direction based on WASD keys
	Vector3 mGoalDirection;     // actual intended direction in world-space
	Real mVerticalVelocity;     // for jumping
	Real mTimer;
};

// Program entry point
int main(int argc, char *argv[]) {
    Application app;
    app.run();
    return 0;
}
