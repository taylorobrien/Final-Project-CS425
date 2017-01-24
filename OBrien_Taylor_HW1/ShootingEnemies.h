#ifndef __ShootingEnemies_h_
#define __ShootingEnemies_h_
#include "BaseApplication.h"
#include <deque>
#include "Grid.h"
#include "Agent.h"

class GameApplication;
class Grid;
class GridNode;
class Agent;

class ShootingEnemies{
private:
	Grid* grid;	//the grid for the enemy - used for position purposes
	Ogre::SceneManager* mSceneMgr;		// pointer to scene graph
	Ogre::SceneNode* mBodyNode;			//pointer to the Scene node
	Ogre::Entity* mBodyEntity;
	float height;						// height the character should be moved up
	float scale;						//scale of the body node

	// all of the animations our character has, and a null ID
	// some of these affect separate body parts and will be blended together
	enum AnimID				//the animations for the Robot.mesh
	{
		ANIM_DIE,
		ANIM_IDLE_BASE,
		ANIM_SHOOT
	};

	Ogre::AnimationState* mAnims;		    // master animation list
	AnimID mBaseAnimID;						// current base (full- or lower-body) animation
	AnimID mTopAnimID;						// current top (upper-body) animation
	bool mFadingIn[3];						// which animations are fading in
	bool mFadingOut[3];					    // which animations are fading out
	Ogre::Real mTimer;						// general timer to see how long animations have been playing
	Ogre::Real mVerticalVelocity;			// for jumping

	
	//void fadeAnimations(Ogre::Real deltaTime);			// blend from one animation to another
	void updateAnimations(Ogre::Real deltaTime);			// update the animation frame
	void shootBullet(Agent* agent);							// used with the bullet list - not getting used currently
	std::list<Ogre::Entity*> bulletList;					// This was going to be used for entities getting produced - all commented out temporarily  
	// for locomotion
	Ogre::Real mDistance;					// The distance the agent has left to travel
	Ogre::Vector3 mDirection;				// The direction the object is moving
	Ogre::Vector3 mDestination;				// The destination the object is moving towards
	std::deque<Ogre::Vector3> mWalkList;	// The list of points we are walking to
	Ogre::Real mWalkSpeed;					// The speed at which the object is moving
	bool nextLocation();					// Is there another destination?
	//void updateLocomote(Ogre::Real deltaTime);			// update the character's walking

	bool procedural;


	//Bullets all of these not getting used currently
	Ogre::Entity* entB1;
	Ogre::Entity* entB2;
	Ogre::Entity* entB3;
	Ogre::Entity* entB4;
	Ogre::Entity* entB5;
	Ogre::SceneNode* nodeB1; //Not getting used - was for bullet

	Ogre::Vector3 initPos;		//initial position of the robot
	Ogre::Vector3 vel;			//velocity of the robot
	Ogre::Vector3 gravity;		//gravity placed on the robot
	

public:
	ShootingEnemies(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale);
	~ShootingEnemies();						//deconstructor
	void setupAnimations();					// load this character's animations
	void setPosition(float x, float y, float z);	//set robot position
	void update(Ogre::Real deltaTime, Agent* agent);		// update the agent
	int getBaseAnimation();								//get the animation of the robot
	void setGrid(Grid* grid);				//set the grid for the robot
	void walkToNode(GridNode* node);			//walk to node - robot walks to node (not used)
	void setBaseAnimation(AnimID id, bool reset = false);	// choose animation to display
	void setTopAnimation(AnimID id, bool reset = false);	//set the top animation of the robot
	void forwardKey(bool keypressed);		//all these are in case I want to move the robot - temporarily not used
	void rotateLeft(bool keypressed);		//not used
	void rotateRight(bool keypressed);		//not used
	void moveForward();		//not used
	bool rotatel;		//not used
	bool rotater;		//not used
	bool moveF;		//not used
	Ogre::Timer bulletTimer;		//Timer used for the bullets 
	void shooting(Ogre::Real deltaTime);	//this handles the particle system and animations
	void preShoot();				//used for testing - not used
	void updatePS(Agent* agent); //updates the particle system
	Ogre::Vector3 getPosition();	//get position of robot
	Ogre::ParticleSystem* ps;	//Particle system for "Shooting guns"
	std::string getNewName();	//gets a new name for the robot's particle system 
};

#endif //#ifndef __Agent_h_