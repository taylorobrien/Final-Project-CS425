#ifndef __Agent_h_
#define __Agent_h_
#include "BaseApplication.h"
#include <deque>
#include "Grid.h"

class GameApplication;
class Grid;
class GridNode;

class Agent{
private:
	Grid* grid;							//pointer to the grid
	Ogre::SceneManager* mSceneMgr;		// pointer to scene graph
	Ogre::SceneNode* mBodyNode;			//pointer to the body node
	
	float height;						// height the character should be moved up
	float scale;						//scale of the character

	// all of the animations our character has, and a null ID
	// some of these affect separate body parts and will be blended together
	enum AnimID
	{
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

	Ogre::AnimationState* mAnims[13];		// master animation list
	AnimID mBaseAnimID;						// current base (full- or lower-body) animation
	AnimID mTopAnimID;						// current top (upper-body) animation
	bool mFadingIn[13];						// which animations are fading in
	bool mFadingOut[13];					// which animations are fading out
	Ogre::Real mTimer;						// general timer to see how long animations have been playing
	Ogre::Real mVerticalVelocity;			// for jumping

	
	void fadeAnimations(Ogre::Real deltaTime);				// blend from one animation to another
	void updateAnimations(Ogre::Real deltaTime);			// update the animation frame

	// for locomotion
	Ogre::Real mDistance;					// The distance the agent has left to travel
	Ogre::Vector3 mDirection;				// The direction the object is moving
	Ogre::Vector3 mDestination;				// The destination the object is moving towards
	std::deque<Ogre::Vector3> mWalkList;	// The list of points we are walking to
	Ogre::Real mWalkSpeed;					// The speed at which the object is moving
	bool nextLocation();					// Is there another destination?
	void updateLocomote(Ogre::Real deltaTime);			// update the character's walking

	bool procedural;

	//Boxes to jump on
	Ogre::Entity* entBox;				//These are all for the jumping boxes
	Ogre::SceneNode* nodeBox;
	Ogre::Entity* entBox2;
	Ogre::SceneNode* nodeBox2;
	Ogre::Entity* entBox3;
	Ogre::SceneNode* nodeBox3;
	Ogre::Entity* entBox4;
	Ogre::SceneNode* nodeBox4;
	Ogre::Entity* entBox5;
	Ogre::SceneNode* nodeBox5;
	Ogre::Entity* entBox6;
	Ogre::SceneNode* nodeBox6;
	Ogre::Entity* entBox7;
	Ogre::SceneNode* nodeBox7;
	Ogre::Entity* entPowerOff;			//box for the power off button
	Ogre::SceneNode* nodePowerOff; 

	Ogre::Entity* puJumpEnt;			//box for the power up jump box
	Ogre::SceneNode* puJumpNode;

	bool powerJump;				//bool to show if the character has the jump power up
	float powerStart;			//time the power jump starts
	Ogre::Timer finalTimer; //final timer for the alarm system
	Ogre::ParticleSystem* ps; //particle system for the character

	Ogre::Entity* entKey;	//entity and scene node for the key the player picks up
	Ogre::SceneNode* nodeKey;

	Ogre::Entity* entExit;		//entity and scene node for the exit door (aka tutor house)
	Ogre::SceneNode* nodeExit;

	bool hasKey;		//does the player have the key
	int level;			//current level

	Ogre::Timer alarmTimer;		//These are all used to show the alarm going off
	Ogre::Light* alarmLight;		//all lights are high point lights, with red color
	Ogre::Light* alarmLight2;
	Ogre::Light* alarmLight3;
	Ogre::Light* alarmLight4;
	Ogre::Light* alarmLight5;


public:
	Agent(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, std::string level);
	~Agent();
	void setupAnimations();					// load this character's animations
	void setPosition(float x, float y, float z);  //set characters position 
	void update(Ogre::Real deltaTime);		// update the agent
	int getBaseAnimation();				//get animation from character 
	void setGrid(Grid* grid);			//set characters grid
	void walkToNode(GridNode* node);		//let the character walk to a node - temp. taken out
	void setBaseAnimation(AnimID id, bool reset = false);	// choose animation to display
	void setTopAnimation(AnimID id, bool reset = false); //set the top animation for the character
	void forwardKey(bool keypressed); //these all handle the character's movement
	void rotateLeft(bool keypressed); //the way these work is until the key is released
	void rotateRight(bool keypressed); //the player keeps moving in that motion
	void moveForward();
	void backwardsKey(bool keypressed);
	void moveBackwards();
	bool rotatel;	//bools to see if the key is still pressed down
	bool rotater;
	bool moveF;
	bool moveB;
	Ogre::Vector3 getPosition();	//get the character position
	void physicsJump();	//do a high jump for the character
	bool projectile; //is the character a projectile / in high jump still?
	void shoot(Ogre::Real deltaTime);	//shoot the character.. aka. physics jump
	Ogre::Vector3 initPos;		//initial position of character before jump
	Ogre::Vector3 vel; //character velocity
	Ogre::Vector3 gravity;  //character gravity
	void buildBoxes(int level); //build boxes - handled in constructor instead
	bool onBlock;// isthe character on a box (jumped onto a box)
	bool collideBox(); //did the character colllide with a box
	bool hoverBox(); //are they hovering over a box
	bool soundPlaying; //is the sound playing 
	bool alarm; //is the alarm going off
	bool poweron; //is the robot's power still on
	float health; //character's health
	bool levelDone; //bool to tell the game application that the level has been completed



	Ogre::AxisAlignedBox objBox; //Box for the character
	//objBox.intersects(objBox); 
	Ogre::AxisAlignedBox Box1; //these are boxes for the jumping box
	Ogre::AxisAlignedBox Box2;
	Ogre::AxisAlignedBox Box3;
	Ogre::AxisAlignedBox Box4;
	Ogre::AxisAlignedBox Box5;
	Ogre::AxisAlignedBox Box6;
	Ogre::AxisAlignedBox Box7;

	Ogre::AxisAlignedBox PowerBox; //box for the power up 
	Ogre::Entity* mBodyEntity;//pointer to the entity
};

#endif //#ifndef __Agent_h_