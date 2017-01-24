#include "ShootingEnemies.h"
#include "windows.h"

ShootingEnemies::ShootingEnemies(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale)
{
	using namespace Ogre;

	mSceneMgr = SceneManager; // keep a pointer to where this agent will be

	if (mSceneMgr == NULL)
	{
		std::cout << "ERROR: No valid scene manager in Agent constructor" << std::endl;
		return;
	}

	this->height = height;
	this->scale = scale;
	this->rotatel = false;
	this->rotater = false;
	this->moveF = false;

	
	mBodyNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(); // create a new scene node
	mBodyEntity = mSceneMgr->createEntity(name, filename); // load the model
	mBodyNode->attachObject(mBodyEntity);	// attach the model to the scene node
	//mBodyNode->attachObject(mSceneMgr->getCamera("PlayerCam"));
	ParticleSystem::setDefaultNonVisibleUpdateTimeout(5);  // set nonvisible timeout
	ps = mSceneMgr->createParticleSystem(getNewName(), "Examples/GunFire");
	Ogre::SceneNode* mnode = mBodyNode->createChildSceneNode();
	mnode->yaw(Degree(90));
	//ps->setDefaultWidth(10);
	//ps->setDefaultHeight(10);
	//mnode->yaw(Ogre::Degree(90));
	mnode->translate(0,10,0);
	mnode->attachObject(ps);

	
	ps->setVisible(false);
	mBodyNode->scale(0.07, 0.07, 0.07);
	//mBodyNode->translate(0,-70, 0); //4.5,0); // make the Ogre stand on the plane (almost)
	mBodyNode->yaw(Ogre::Degree(90));
	//setupAnimations();  // load the animation for this character
	// configure walking parameters
	mWalkSpeed = 30.0f;	
	mDirection = Ogre::Vector3::ZERO;
	mDistance = 0;
	//mDestination = Ogre::Vector3::ZERO;
	//mBodyEntity = 0;
	//mBodyNode = 0;
	
	mDestination = Ogre::Vector3::ZERO;
	mAnims = mBodyEntity->getAnimationState("Shoot");//animNames[i]);
	mAnims->setLoop(true);
	mAnims->setEnabled(true);
	// create random points to walk to 
//	float x, y;
 
	//bulletTimer->reset();



	



}

std::string ShootingEnemies::getNewName() // return a unique name 
{
	static int count = 0;	// keep counting the number of objects

	std::string s;
	std::stringstream out;	// a stream for outputing to a string
	out << count++;			// make the current count into a string
	s = out.str();
	//std::cout << "object_" << s << std::endl;
	return "object_" + s;	// append the current count onto the string
}

ShootingEnemies::~ShootingEnemies(){
	// mSceneMgr->destroySceneNode(mBodyNode); // Note that OGRE does not recommend doing this. It prefers to use clear scene
	// mSceneMgr->destroyEntity(mBodyEntity);
}

// update is called at every frame from GameApplication::addTime
void ShootingEnemies::update(Ogre::Real deltaTime, Agent* agent) 
{
	if(agent->poweron){
		this->updateAnimations(deltaTime);	// Update animation playback
		//this->updateLocomote(deltaTime);	// Update Locomotion
	
		mDestination = agent->getPosition();
		mBodyNode->setPosition(mBodyNode->getPosition()[0], 0, mBodyNode->getPosition()[2]);
		Ogre::Vector3 temp = mBodyNode->getPosition();
		//mBodyNode->setPosition(mDestination);
		//mDirection = Ogre::Vector3::ZERO;	
		this->mDirection = mDestination - mBodyNode->getPosition();
		this->mDirection[1] = 0;
		//rotation code
		Ogre::Vector3 src = mBodyNode->getOrientation() * Ogre::Vector3::UNIT_X;
		if ((1.0 + src.dotProduct(mDirection)) < 0.0001) 
		{
		  mBodyNode->yaw(Ogre::Degree(180));
		}
		else
		{
		  Ogre::Quaternion quat = src.getRotationTo(mDirection);
		  mBodyNode->rotate(quat);
					 //mBodyNode->lookAt(Ogre::Vector3(mDestination.x, mDestination.y, mDestination.z + 100), Node::TS
		 // mBodyNode->yaw(Ogre::Degree(-90));
		}
		/*
		if(bulletTimer.getMilliseconds() > 3000){
			bulletTimer.reset();
			shootBullet(agent);

		}
		*/
		updatePS(agent);
	}
}
Ogre::Vector3 ShootingEnemies::getPosition(){
	return mBodyNode->getPosition();
}

void ShootingEnemies::updatePS(Agent* agent){
	int xDiff = abs(agent->getPosition()[0] - mBodyNode->getPosition()[0]);
	int zDiff = abs(agent->getPosition()[2] - mBodyNode->getPosition()[2]);
	int Dist = xDiff + zDiff; //could max be 48
	if( xDiff < 35 && zDiff < 35 && abs(agent->getPosition()[1] - mBodyNode->getPosition()[1]) < 10){
		ps->setVisible(true);
		agent->health-= 0.5;
	}
	else{
		ps->setVisible(false);
	}

}



void ShootingEnemies::shootBullet(Agent* agent){
	if(abs(agent->getPosition()[0] - mBodyNode->getPosition()[0]) < 30 && abs(agent->getPosition()[2] - mBodyNode->getPosition()[2]) < 30 && abs(agent->getPosition()[1] - mBodyNode->getPosition()[1]) < 10){
		if(bulletList.size() > 0){
			Ogre::Entity* bullet = bulletList.front();
			bulletList.pop_front();
		}
	}
}

void ShootingEnemies::preShoot(){
	/*
	//air blowing up particle simulator?
	if(!this->projectile || onBlock){
		projectile = true; // turns on the movement
	//	this->setBaseAnimation(ANIM_NOD);
		// set up the initial state
		//initPos = this->mBodyNode->getPosition();
		vel.x = 0;
		vel.y = 30;
		if(powerJump){
			vel.y = 60;
		}
		vel.z = 0; //-mWalkSpeed;

	
		gravity.x = 0;
		gravity.y = -9.81;
		gravity.z = 0;
	
		//this->mBodyNode->yaw(Ogre::Degree(180));
		//this->mBodyNode->pitch(Ogre::Degree(45));
		//this->mBodyNode->showBoundingBox(true); 
		
	} */
}

void ShootingEnemies::shooting(Ogre::Real deltaTime) // lecture 10 call for every frame of the animation
{
	/*using namespace Ogre;

	Vector3 pos = this->mBodyNode->getPosition();

	// Calculate projectile motion here. What is vel? and What is new pos?
	//mass = 5;
	vel += deltaTime * gravity * 10; // 5 is the mass
	pos += 0.5*vel*deltaTime;


	this->mBodyNode->setPosition(pos);

	//objBox = this->mBodyEntity->getWorldBoundingBox();

	
	
	if(collideBox()){
		projectile = false;
		this->mBodyNode->setPosition(getPosition()[0], getPosition()[1] + 1.5, getPosition()[2]);
		//ps->setVisible(true);
		//PlaySound("DingSound.wave", NULL, SND_FILENAME);
	}else{
		projectile = true;
	}

	

	if (this->mBodyNode->getPosition().y <= 5) // if it get close to the ground, stop
	{
		// when finished reset
		projectile = false;
		//setBaseAnimation(ANIM_WAVE);
		//ps->setVisible(false);
		//this->mBodyNode->pitch(Ogre::Degree(-45));
		//this->mBodyNode->yaw(Ogre::Degree(180));
		//this->mBodyNode->setPosition(initPos);
	}*/
}

void ShootingEnemies::setupAnimations()
{
	this->mTimer = 0;	// Start from the beginning
	this->mVerticalVelocity = 0;	// Not jumping

	// this is very important due to the nature of the exported animations
	mBodyEntity->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);

	// Name of the animations for this character
	Ogre::String animNames[] =
		{"Die", "Idle", "Shoot"};

	// populate our animation list
	for (int i = 0; i < 3; i++)
	{
		mAnims = mBodyEntity->getAnimationState("Shoot");//animNames[i]);
		mAnims->setLoop(true);
	
		
		//mFadingIn[i] = false;
		//mFadingOut[i] = false;
	}

	// start off in the idle state (top and bottom together)
	setBaseAnimation(ANIM_SHOOT);
	//setTopAnimation(ANIM_SHOOT);
	//setBaseAnimation(ANIM_RUN_BASE);
	//setTopAnimation(ANIM_RUN_TOP);

	// relax the hands since we're not holding anything
	//mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
}

void ShootingEnemies::setPosition(float x, float y, float z){
	this->mBodyNode->setPosition(x, y, z);//+height, z);

}

void ShootingEnemies::setBaseAnimation(AnimID id, bool reset)
{
	/*
	if (mBaseAnimID >= 0 && mBaseAnimID < 3)
	{
		// if we have an old animation, fade it out
		mFadingIn[mBaseAnimID] = false;
		mFadingOut[mBaseAnimID] = true;
	}
	*/

	mBaseAnimID = id; 


		// if we have a new animation, enable it and fade it in
		mAnims->setEnabled(true);
		mAnims->setWeight(0);
		//mFadingOut = false;
		//mFadingIn[id] = true;
		if (reset) mAnims->setTimePosition(0);

}
	
void ShootingEnemies::setTopAnimation(AnimID id, bool reset)
{
	if (mTopAnimID >= 0 && mTopAnimID < 3)
	{
		// if we have an old animation, fade it out
		mFadingIn[mTopAnimID] = false;
		mFadingOut[mTopAnimID] = true;
	}

	mTopAnimID = id;

	
		// if we have a new animation, enable it and fade it in
	/*
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset) mAnims[id]->setTimePosition(0);
		*/
	
}

int ShootingEnemies::getBaseAnimation(){
	return mBaseAnimID;
}

void ShootingEnemies::updateAnimations(Ogre::Real deltaTime)
{
	using namespace Ogre;

	Real baseAnimSpeed = 1;
	//Real topAnimSpeed = 1;

	mTimer += deltaTime; // how much time has passed since the last update 
	mAnims->addTime(deltaTime);
	// increment the current base and top animation times
	//if (mBaseAnimID != ANIM_NONE) mAnims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);
	//if (mTopAnimID != ANIM_NONE) mAnims[mTopAnimID]->addTime(deltaTime * topAnimSpeed);

	// apply smooth transitioning between our animations
	//fadeAnimations(deltaTime);
}

void ShootingEnemies::setGrid(Grid* grid){
	this->grid = grid;
}

void ShootingEnemies::walkToNode(GridNode* node){
	if(node){
		//if the node exists, get the row and column of it
		int row = node->getRow();
		int col = node->getColumn();
		//if the row and column arent NULL
		if(row != NULL && col != NULL){
			//get the X, Y, and Z position of it (so we can tell the agent to go there)
			float z = (row * NODESIZE) - (grid->getRows() * NODESIZE)/2.0 + NODESIZE/2.0; 
			float y = 5; 
			float x = (col * NODESIZE) - (grid->getCols() * NODESIZE)/2.0 + NODESIZE/2.0; 
			//add the location to the walkList of the agent
			mWalkList.push_back(Ogre::Vector3(x, y, z));
			//This is for error checking
			if(mBodyNode->getPosition().y < 0){
				std::cout << "break" << std::endl;
			}
		}
	}
	else{
		//also used for error checking

		std::cout << "error" << std::endl;
	}
}



bool ShootingEnemies::nextLocation()
{
	if(mWalkList.empty()){
		return false;
	}
	mDestination = mWalkList.front();
	mWalkList.pop_front();
	this->mDirection = mDestination - mBodyNode->getPosition();
	mDistance = mDirection.normalise();
	mSceneMgr->getCamera("PlayerCam")->lookAt(mDestination); //setDirection(mDirection);
	//mDirection = mDestination - mBodyNode->getPosition();
	return true;
}

void ShootingEnemies::rotateLeft(bool keypressed){
	this->rotatel = keypressed;
	/*while(keypressed){
	mBodyNode->yaw(Ogre::Degree(2));
	}*/

}
void ShootingEnemies::rotateRight(bool keypressed){
	this->rotater = keypressed;
	/*while(keypressed){
	mBodyNode->yaw(Ogre::Degree(-2));
	}*/

}
void ShootingEnemies::forwardKey(bool keypressed){
	this->moveF = keypressed;
}
void ShootingEnemies::moveForward(){
	using namespace std;
	//mSceneMgr->getCamera("PlayerCam")->getPosition();
	//mBodyNode->setPosition(mSceneMgr->getCamera("PlayerCam")->getPosition());
	//Ogre::Real move = mWalkSpeed * deltaTime;
	//cout << "ogre position: " << mBodyNode->getPosition() << endl;
	//cout << "ogre orientation: " << mBodyNode->getOrientation() << endl;
	//cout << "Camera position: " << mSceneMgr->getCamera("PlayerCam")->getPosition() << endl;
	//Ogre::Vector3 bnPOS= mBodyNode->getPosition();

	Ogre::Vector3 initialPlace = mBodyNode->getPosition();
	
	int initialR = (initialPlace[2] - (NODESIZE/2) + ((grid->getRows() * NODESIZE)/2))/NODESIZE;
	int initialC = (initialPlace[0] - (NODESIZE/2) + ((grid->getCols() * NODESIZE)/2))/NODESIZE;

	mBodyNode->translate(Ogre::Vector3(0, 0, -1), Ogre::Node::TS_LOCAL);
	Ogre::Vector3 currentPlace = mBodyNode->getPosition();
	
	int r = (currentPlace[2] - (NODESIZE/2) + ((grid->getRows() * NODESIZE)/2))/NODESIZE;
	int c = (currentPlace[0] - (NODESIZE/2) + ((grid->getCols() * NODESIZE)/2))/NODESIZE;
	
	if (grid->getNode(r, c) != NULL){
		if(!grid->getNode(r, c)->isClear()){ // || !grid->getNode(r+1, c)->isClear() || !grid->getNode(r, c+1)->isClear()) ) {
			//->contains('B')){
			mBodyNode->translate(Ogre::Vector3(0, 0, 1), Ogre::Node::TS_LOCAL);
			cout << "Blocked Node: " << r << ", " << c << endl;
		}
		else if(initialR < r && !grid->getNode(r+1, c)->isClear()){
			mBodyNode->translate(Ogre::Vector3(0, 0, 1), Ogre::Node::TS_LOCAL);
			cout << "Blocked Node (r): " << r << ", " << c << endl;
		}
		else if(initialC < c && !grid->getNode(r, c+1)->isClear()){
			mBodyNode->translate(Ogre::Vector3(0, 0, 1), Ogre::Node::TS_LOCAL);
			cout << "Blocked Node (c): " << r << ", " << c << endl;
		}
	}

	else if(grid->getNode(r, c) == NULL){
		cout << "Null node: " << r << ", " << c << endl;
	}
	else{
		cout << "Clear node: " << r << ", " << c << endl;
	}
	cout << mBodyNode->getPosition() << endl;
	
	
	//mBodyNode->setPosition(src);
	//cout << "ogre NEW position: " << mBodyNode->getPosition() << endl;



}


/*void ShootingEnemies::updateLocomote(Ogre::Real deltaTime)
{
	//std::cout << "Location: " << mBodyNode->getPosition() << std::endl;
	//std::cout << "Direction: " << mDirection << std::endl;
	//std::cout << "Destination: " << mDestination << std::endl;
	Ogre::String animNames[] =
		{"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",
		"SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};
	if (mDirection == Ogre::Vector3::ZERO) 
	{
		if(nextLocation()){
			mAnims[ANIM_RUN_TOP]->setEnabled(true);
			mAnims[ANIM_RUN_TOP]->setTimePosition(mTimer);
			mAnims[ANIM_RUN_BASE]->setEnabled(true);
			mAnims[ANIM_RUN_BASE]->setTimePosition(mTimer);
			mAnims[ANIM_IDLE_BASE]->setEnabled(false);
			mAnims[ANIM_IDLE_TOP]->setEnabled(false);
			Ogre::Vector3 src = mBodyNode->getOrientation() * Ogre::Vector3::UNIT_Z;
			if ((1.0 + src.dotProduct(mDirection)) < 0.0001) 
			{
			  mBodyNode->yaw(Ogre::Degree(180));
			}
			else
			{
			// mBodyNode->setOrientation(mBodyNode->getOrientation()[0], mBodyNode->getOrientation()[1], 15, mBodyNode->getOrientation()[3]);

			 Ogre::Quaternion quat = src.getRotationTo(mDirection);
			//quat[2] = 5;
			 
			 mBodyNode->rotate(quat);
			 //mBodyNode->setOrientation(mBodyNode->getOrientation()[0], mBodyNode->getOrientation()[1], 15, mBodyNode->getOrientation()[3]);
				 //mBodyNode->yaw(Ogre::Degree(90));
			}
			
		}
	}
	else{
		mAnims[ANIM_RUN_TOP]->setTimePosition(mTimer);
		mAnims[ANIM_RUN_BASE]->setTimePosition(mTimer);
		Ogre::Real move = mWalkSpeed * deltaTime;
		mDistance -= move;
		if(mDistance <= 0){
			mBodyNode->setPosition(mDestination);
			mDirection = Ogre::Vector3::ZERO;
			if(nextLocation()){
				
				//rotation code
				Ogre::Vector3 src = mBodyNode->getOrientation() * Ogre::Vector3::UNIT_Z;
				if ((1.0 + src.dotProduct(mDirection)) < 0.0001) 
				{
				  mBodyNode->yaw(Ogre::Degree(180));
				}
				else
				{
				 Ogre::Quaternion quat = src.getRotationTo(mDirection);
				 mBodyNode->rotate(quat);
				 //mBodyNode->lookAt(Ogre::Vector3(mDestination.x, mDestination.y, mDestination.z + 100), Node::TS
				 //mBodyNode->yaw(Ogre::Degree(90));
				}
			}
			else{
				mAnims[ANIM_IDLE_BASE]->setEnabled(true);
				mAnims[ANIM_IDLE_TOP]->setEnabled(true);
				mAnims[ANIM_RUN_TOP]->setEnabled(false);
				//mAnims[ANIM_RUN_TOP]->setTimePosition(mTimer);
				mAnims[ANIM_RUN_BASE]->setEnabled(false);
				//mAnims[ANIM_RUN_BASE]->setTimePosition(mTimer);
				
			}
		}
		else{
			mBodyNode->translate(move * mDirection);
		}

	}
		

}
*/



