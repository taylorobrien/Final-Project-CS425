#ifndef __Ninja_h_
#define __Ninja_h_
#include "BaseApplication.h"
#include <deque>
#include "Grid.h"
#include "Agent.h"

class Agent;
class GameApplication;
class Grid;
class GridNode;

class Ninja{
private:
	Grid* grid;							//pointer to the ninja's grid
	Ogre::SceneManager* mSceneMgr;		// pointer to scene graph
	Ogre::SceneNode* mBodyNode;			//pointer to the scene node
	Ogre::Entity* mBodyEntity;			//pointer to the entity
	float height;						// height the character should be moved up
	float scale;						//scale  of the ninja

	// all of the animations our character has, and a null ID
	// some of these affect separate body parts and will be blended together
	enum AnimID
	{
		ANIM_ATTACK1,
		ANIM_ATTACK2,
		ANIM_ATTACK3,
		ANIM_BACKFLIP,
		ANIM_BLOCK,
		ANIM_CLIMB,
		ANIM_CROUCH,
		ANIM_DEATH1,
		ANIM_DEATH2,
		ANIM_HIGH_JUMP,
		ANIM_IDLE1,
		ANIM_IDLE2,
		ANIM_IDLE3,
		ANIM_JUMP,
		ANIM_JUMP_NO_HEIGHT,
		ANIM_KICK,
		ANIM_SIDE_KICK,
		ANIM_SPIN,
		ANIM_STEALTH,
		ANIM_WALK
	};

	Ogre::AnimationState* mAnims[20];		// master animation list
	AnimID mBaseAnimID;						// current base (full- or lower-body) animation
	AnimID mTopAnimID;						// current top (upper-body) animation
	bool mFadingIn[20];						// which animations are fading in
	bool mFadingOut[20];					// which animations are fading out
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


	Ogre::Entity* puJumpEnt;			//holds powerup
	Ogre::SceneNode* puJumpNode;		//node for the powerup

	bool powerJump;					//power jump - not used currently
	float powerStart;				//number for timer of the power jump
	float finalTimer;				//timer for when the alarm is going off
	Ogre::ParticleSystem* ps;		//particle system - none currently assigned
	std::list<GridNode*> path;		//path for the ninja
//	bool procedural;
	GridNode* temp;					//used in A*, as the temporary grid node they are on/at
	int lowestF;					


public:
	Ninja(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale);
	~Ninja();								//Deconstructor
	void setupAnimations();					// load this character's animations
	void setPosition(float x, float y, float z);	//set's character's position
	void update(Ogre::Real deltaTime, Agent* agent);		// update the agent
	int getBaseAnimation();					//gets the base animation
	void setGrid(Grid* grid);				//sets the ninja's grid
	void walkToNode(GridNode* node);		//walk to specified node on the grid
	void setBaseAnimation(AnimID id, bool reset = false);	// choose animation to display
	void setTopAnimation(AnimID id, bool reset = false);	//sets the top animation to display
	std::list<GridNode*> open;			//used for A*, the grid nodes that still need to be looked at 
	std::list<GridNode*> closed;		//used for A*, the grid nodes that have been looked at
	Ogre::Vector3 getPosition();		//get the position of the ninja
	Ogre::AxisAlignedBox objBox;		//used for collision detecting of the ninja
	//void walkToNode(GridNode* node);
	void walkBackwards(GridNode* start, GridNode* Destination);		//used for A*, walks backwards from destination node
	void AStarWalk(GridNode* startPos, GridNode* currentPos, GridNode* destination); //A* path finding, to randomized destination
	int addLocationTimer;	//adds time to the timer
	bool firstTime;		//used to set things up
	void nextLoc();		//nextLocation - used to call A*, to find the next location to go to
	Ogre::Timer testTime;	//testTime used for calling A*
	void stun();			//stuns the ninjas - called from GameApplication with 'x'
	bool stunned;			//if the ninjas are already stunned
	Ogre::Timer stunTimer;	//sets the ninjas stun for 5 seconds - times this
	Ogre::Timer stunCoolDown;	//the ninja's stun cooldown = 15 seconds
	bool onCooldown;	//if the stun is on cooldown

};

#endif //#ifndef __Agent_h_