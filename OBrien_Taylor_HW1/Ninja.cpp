#include "Ninja.h"

Ninja::Ninja(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale)
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
	this->addLocationTimer = 0;
	this->firstTime = true;
	this->onCooldown = false;
	this->stunned = false;
	
	testTime.reset();
	mBodyNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(); // create a new scene node
	mBodyEntity = mSceneMgr->createEntity(name, filename); // load the model
	mBodyNode->attachObject(mBodyEntity);	// attach the model to the scene node
	
	//mBodyNode->translate(0,0, 25); //4.5,0); // make the Ogre stand on the plane (almost)
	//mBodyNode->yaw(Ogre::Degree(90));
	setupAnimations();  // load the animation for this character

	// configure walking parameters
	mWalkSpeed = 10.0f;	
	mDirection = Ogre::Vector3::ZERO;
	mDistance = 0;
	
	//mDestination = Ogre::Vector3::ZERO;
	//mBodyEntity = 0;
	//mBodyNode = 0;
	mBodyNode->scale(0.05,0.05,0.05);
	Vector3 temp = mBodyNode->getPosition();

//	mBodyNode->translate(0,-15,0);

	mDestination = Ogre::Vector3::ZERO;
	// create random points to walk to 
//	float x, y;
//	Ogre::Entity* ent;
//	Ogre::SceneNode* node;
	
	


}

Ninja::~Ninja(){
	// mSceneMgr->destroySceneNode(mBodyNode); // Note that OGRE does not recommend doing this. It prefers to use clear scene
	// mSceneMgr->destroyEntity(mBodyEntity);
}

// update is called at every frame from GameApplication::addTime
void Ninja::update(Ogre::Real deltaTime, Agent* agent) 
{
	this->updateAnimations(deltaTime);	// Update animation playback
	this->updateLocomote(deltaTime);	// Update Locomotion
	if(testTime.getMilliseconds() > 10000 ){
		testTime.reset();
		nextLoc();
		std::cout << "nextLoc called" << std::endl;
	}
	objBox = this->mBodyEntity->getWorldBoundingBox();
	Ogre::AxisAlignedBox agentBox = agent->mBodyEntity->getWorldBoundingBox();
	if(!stunned && objBox.intersects(agentBox)){
		agent->health -= 2;
		std::cout << agent->health << std::endl;
	}
	if(stunned && stunTimer.getMilliseconds() > 5000){ //stunned for 5 seconds
		onCooldown = true;
		stunned = false;
		if(agent->alarm){
			mWalkSpeed = 30.0f;
		}
		else{
			mWalkSpeed = 10.0f;
		}
		stunCoolDown.reset();
		setBaseAnimation(ANIM_STEALTH);
		setTopAnimation(ANIM_STEALTH);
	}
	if(onCooldown && stunCoolDown.getMilliseconds() > 15000){
		onCooldown = false;

	}

	if(agent->alarm && mWalkSpeed != 0){
		mWalkSpeed = 30.0f;
	}
	else if(mWalkSpeed != 0){
		mWalkSpeed = 10.0f;
	}
	
	
	
}


void Ninja::setupAnimations()
{
	this->mTimer = 0;	// Start from the beginning
	this->mVerticalVelocity = 0;	// Not jumping

	// this is very important due to the nature of the exported animations
	mBodyEntity->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);

	// Name of the animations for this character
	Ogre::String animNames[] =
		{"Attack1", "Attack2", "Attack3", "Backflip", "Block", "Climb", "Crouch",
		"Death1", "Death2", "HighJump", "Idle1", "Idle2", "Idle3", "Jump", "JumpNoHeight",
		"Kick" , "SideKick", "Spin", "Stealth", "Walk"};

	// populate our animation list
	for (int i = 0; i < 20; i++)
	{
		mAnims[i] = mBodyEntity->getAnimationState(animNames[i]);
		mAnims[i]->setLoop(true);
		
		mFadingIn[i] = false;
		mFadingOut[i] = false;
	}

	// start off in the idle state (top and bottom together)
	setBaseAnimation(ANIM_STEALTH);
	setTopAnimation(ANIM_STEALTH);
	//setBaseAnimation(ANIM_RUN_BASE);
	//setTopAnimation(ANIM_RUN_TOP);

	// relax the hands since we're not holding anything
	//mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
}

void Ninja::setPosition(float x, float y, float z){
	this->mBodyNode->setPosition(x, 0, z);//+height, z);

}

void Ninja::setBaseAnimation(AnimID id, bool reset)
{
	if (mBaseAnimID >= 0 && mBaseAnimID < 20)
	{
		// if we have an old animation, fade it out
		mFadingIn[mBaseAnimID] = false;
		mFadingOut[mBaseAnimID] = true;
	}

	mBaseAnimID = id; 

	//if (id != ANIM_NONE)
	//{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset) mAnims[id]->setTimePosition(0);
	//}
}
	
void Ninja::setTopAnimation(AnimID id, bool reset)
{
	if (mTopAnimID >= 0 && mTopAnimID < 20)
	{
		// if we have an old animation, fade it out
		mFadingIn[mTopAnimID] = false;
		mFadingOut[mTopAnimID] = true;
	}

	mTopAnimID = id;

	//if (id != ANIM_NONE)
	//{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset) mAnims[id]->setTimePosition(0);
	//}
}

int Ninja::getBaseAnimation(){
	return mBaseAnimID;
}

void Ninja::updateAnimations(Ogre::Real deltaTime)
{
	using namespace Ogre;

	Real baseAnimSpeed = 1;
	Real topAnimSpeed = 1;

	mTimer += deltaTime; // how much time has passed since the last update 
	// increment the current base and top animation times
	//if (mBaseAnimID != ANIM_NONE) mAnims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);
	//if (mTopAnimID != ANIM_NONE) mAnims[mTopAnimID]->addTime(deltaTime * topAnimSpeed);

	// apply smooth transitioning between our animations
	fadeAnimations(deltaTime);
}

void Ninja::setGrid(Grid* grid){
	this->grid = grid;
}

void Ninja::walkToNode(GridNode* node){
	if(node){
		//if the node exists, get the row and column of it
		int row = node->getRow();
		int col = node->getColumn();
		//if the row and column arent NULL
		if(row != NULL && col != NULL){
			//get the X, Y, and Z position of it (so we can tell the agent to go there)
			float z = (row * NODESIZE) - (grid->getRows() * NODESIZE)/2.0 + NODESIZE/2.0; 
			float y = 0; 
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

void Ninja::fadeAnimations(Ogre::Real deltaTime)
{
	using namespace Ogre;

	for (int i = 0; i < 20; i++)
	{
		if (mFadingIn[i])
		{
			// slowly fade this animation in until it has full weight
			Real newWeight = mAnims[i]->getWeight() + deltaTime * 7.5f; //ANIM_FADE_SPEED;
			mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
			if (newWeight >= 1) mFadingIn[i] = false;
		}
		else if (mFadingOut[i])
		{
			// slowly fade this animation out until it has no weight, and then disable it
			Real newWeight = mAnims[i]->getWeight() - deltaTime * 7.5f; //ANIM_FADE_SPEED;
			mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
			if (newWeight <= 0)
			{
				mAnims[i]->setEnabled(false);
				mFadingOut[i] = false;
			}
		}
	}
}

bool Ninja::nextLocation()
{
	if(mWalkList.empty()){
		return false;
	}
	mDestination = mWalkList.front();
	mWalkList.pop_front();
	this->mDirection = mDestination - mBodyNode->getPosition();
	mDistance = mDirection.normalise();
	//mSceneMgr->getCamera("PlayerCam")->lookAt(mDestination); //setDirection(mDirection);
	//mDirection = mDestination - mBodyNode->getPosition();
	return true;
}

Ogre::Vector3 Ninja::getPosition(){
	return mBodyNode->getPosition();
}

void Ninja::updateLocomote(Ogre::Real deltaTime)
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
			mAnims[ANIM_STEALTH]->setEnabled(true);
			mAnims[ANIM_STEALTH]->setTimePosition(mTimer);
			//mAnims[ANIM_RUN_BASE]->setEnabled(true);
			//mAnims[ANIM_RUN_BASE]->setTimePosition(mTimer);
			//mAnims[ANIM_IDLE_BASE]->setEnabled(false);
			//mAnims[ANIM_IDLE_TOP]->setEnabled(false);
			Ogre::Vector3 src = mBodyNode->getOrientation() * Ogre::Vector3::UNIT_X;
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
		mAnims[ANIM_STEALTH]->setTimePosition(mTimer);
		//mAnims[ANIM_RUN_BASE]->setTimePosition(mTimer);
		Ogre::Real move = mWalkSpeed * deltaTime;
		mDistance -= move;
		if(mDistance <= 0){
			mBodyNode->setPosition(mDestination);
			mDirection = Ogre::Vector3::ZERO;
			if(nextLocation()){
				
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
				 //mBodyNode->yaw(Ogre::Degree(90));
				}
			}
			else{
				mAnims[ANIM_STEALTH]->setEnabled(true);
				//mAnims[ANIM_IDLE_TOP]->setEnabled(true);
				//mAnims[ANIM_RUN_TOP]->setEnabled(false);
				//mAnims[ANIM_RUN_TOP]->setTimePosition(mTimer);
				//mAnims[ANIM_RUN_BASE]->setEnabled(false);
				//mAnims[ANIM_RUN_BASE]->setTimePosition(mTimer);
				
			}
		}
		else{
			mBodyNode->translate(move * mDirection);
		}

	}
		

}

void Ninja::stun(){
	if(!stunned && !onCooldown){
		stunTimer.reset();
		stunned = true;
		mWalkSpeed = 0;
		setBaseAnimation(ANIM_IDLE1);
		setTopAnimation(ANIM_IDLE1);

	}

}


void Ninja::AStarWalk(GridNode* startPos, GridNode* currentPos, GridNode* destination){
	bool isfound;
	if(!destination->isClear()){
		std::cout << "Space not empty, return"  << std::endl;
		return;
	}
	if(startPos == destination){
		return; //Character does not need to move
	}
	if(closed.size() >= 600){
		closed.clear();
		open.clear();
		/*int ranrow = rand() % grid->getRows();
		int rancol = rand() % grid->getCols();		
		Ogre::Vector3 currentPlace = mBodyNode->getPosition();
		int r = (currentPlace[2] - (NODESIZE/2) + ((grid->getRows() * NODESIZE)/2))/NODESIZE;
		int c = (currentPlace[0] - (NODESIZE/2) + ((grid->getCols() * NODESIZE)/2))/NODESIZE;
		if(grid->getNode(r, c) == NULL){
			std::cout << "ERROR" << std::endl;
		}
		AStarWalk(grid->getNode(r, c), grid->getNode(r, c), grid->getNode(ranrow, rancol));
		*/
		return;
	}
	//for the EastNode
	if(startPos == currentPos){
		currentPos->GValue = 0;
		currentPos->HValue = grid->getDistance(currentPos, destination);
		currentPos->FValue = currentPos->GValue + currentPos->HValue;

	}
	else{
		//if((std::find(open.begin(), open.end(), currentPos) != open.end())){
			open.remove(currentPos);
		//}
	}
	if(currentPos == NULL){
		std::cout <<"ERR"<<std::endl;
	}
	temp = grid->getEastNode(currentPos);
	if(temp != NULL && temp->isClear()){ //if the null exists and its not blocked
		isfound = (std::find(closed.begin(), closed.end(), temp) != closed.end());  
		if (!isfound){   //make sure its not in the closed list
			//need to check if its in the open list
			isfound = (std::find(open.begin(), open.end(), temp) != open.end()); 
			//if its on the open list, we need to compute a new G value, 
			//from parent and then see if that G value is lower then current.
			//if the G value is lower, reset the parent of the node & G value
			if(isfound){
				if(10 + currentPos->GValue < temp->GValue){
					temp->parent = currentPos;
					temp->GValue = 10 + currentPos->GValue;//FValue = 15 + grid->getDistance(temp, destination);
					//temp->HValue = grid->getDistance(temp, destination); 
					temp->FValue = temp->GValue + temp->HValue;	
				}
			}
			//if its not on the open list, set the parent to the current, compute G value
			//for this node using current node, add it to the open list
			else{
				temp->parent = currentPos;
				temp->GValue = 10 + temp->parent->GValue;
				temp->HValue = grid->getDistance(temp, destination); 
				temp->FValue = temp->GValue + temp->HValue;
				if(temp == NULL || currentPos == NULL){
					std::cout << "BREAK" << std::endl;
				}
				open.push_back(temp);
			}  }  }
	temp = grid->getNENode(currentPos);
	if(temp != NULL && temp->isClear()){ //if the null exists and its not blocked
		isfound = (std::find(closed.begin(), closed.end(), temp) != closed.end());  
		if (!isfound){   //make sure its not in the closed list
			//need to check if its in the open list
			isfound = (std::find(open.begin(), open.end(), temp) != open.end()); 
			//if its on the open list, we need to compute a new G value, 
			//from parent and then see if that G value is lower then current.
			//if the G value is lower, reset the parent of the node & G value
			if(isfound){
				if(14 + currentPos->GValue < temp->GValue){
					temp->parent = currentPos;
					temp->GValue = 14 + currentPos->GValue;//FValue = 15 + grid->getDistance(temp, destination);
					//temp->HValue = grid->getDistance(temp, destination); 
					temp->FValue = temp->GValue + temp->HValue;	
				}
			}
			//if its not on the open list, set the parent to the current, compute G value
			//for this node using current node, add it to the open list
			else{
				temp->parent = currentPos;
				temp->GValue = 14 + temp->parent->GValue;
				temp->HValue = grid->getDistance(temp, destination); 
				temp->FValue = temp->GValue + temp->HValue;
				if(temp == NULL || currentPos == NULL){
					std::cout << "BREAK" << std::endl;
				}
				open.push_back(temp);
			}  }  }
	temp = grid->getNorthNode(currentPos);
	if(temp != NULL && temp->isClear()){ //if the null exists and its not blocked
		isfound = (std::find(closed.begin(), closed.end(), temp) != closed.end());  
		if (!isfound){   //make sure its not in the closed list
			//need to check if its in the open list
			isfound = (std::find(open.begin(), open.end(), temp) != open.end()); 
			//if its on the open list, we need to compute a new G value, 
			//from parent and then see if that G value is lower then current.
			//if the G value is lower, reset the parent of the node & G value
			if(isfound){
				if(10 + currentPos->GValue < temp->GValue){
					temp->parent = currentPos;
					temp->GValue = 10 + currentPos->GValue;//FValue = 15 + grid->getDistance(temp, destination);
					//temp->HValue = grid->getDistance(temp, destination); 
					temp->FValue = temp->GValue + temp->HValue;	
				}
			}
			//if its not on the open list, set the parent to the current, compute G value
			//for this node using current node, add it to the open list
			else{
				temp->parent = currentPos;
				temp->GValue = 10 + temp->parent->GValue;
				temp->HValue = grid->getDistance(temp, destination); 
				temp->FValue = temp->GValue + temp->HValue;
				if(temp == NULL || currentPos == NULL){
					std::cout << "BREAK" << std::endl;
				}
				open.push_back(temp);
			}  }  }

	temp = grid->getNWNode(currentPos);
	if(temp != NULL && temp->isClear()){ //if the null exists and its not blocked
		isfound = (std::find(closed.begin(), closed.end(), temp) != closed.end());  
		if (!isfound){   //make sure its not in the closed list
			//need to check if its in the open list
			isfound = (std::find(open.begin(), open.end(), temp) != open.end()); 
			//if its on the open list, we need to compute a new G value, 
			//from parent and then see if that G value is lower then current.
			//if the G value is lower, reset the parent of the node & G value
			if(isfound){
				if(14 + currentPos->GValue < temp->GValue){
					temp->parent = currentPos;
					temp->GValue = 14 + currentPos->GValue;//FValue = 15 + grid->getDistance(temp, destination);
					//temp->HValue = grid->getDistance(temp, destination); 
					temp->FValue = temp->GValue + temp->HValue;	
				}
			}
			//if its not on the open list, set the parent to the current, compute G value
			//for this node using current node, add it to the open list
			else{
				temp->parent = currentPos;
				temp->GValue = 14 + temp->parent->GValue;
				temp->HValue = grid->getDistance(temp, destination); 
				temp->FValue = temp->GValue + temp->HValue;
				if(temp == NULL || currentPos == NULL){
					std::cout << "BREAK" << std::endl;
				}
				open.push_back(temp);
			}  }  }

	temp = grid->getSENode(currentPos);
	if(temp != NULL && temp->isClear()){ //if the null exists and its not blocked
		isfound = (std::find(closed.begin(), closed.end(), temp) != closed.end());  
		if (!isfound){   //make sure its not in the closed list
			//need to check if its in the open list
			isfound = (std::find(open.begin(), open.end(), temp) != open.end()); 
			//if its on the open list, we need to compute a new G value, 
			//from parent and then see if that G value is lower then current.
			//if the G value is lower, reset the parent of the node & G value
			if(isfound){
				if(14 + currentPos->GValue < temp->GValue){
					temp->parent = currentPos;
					temp->GValue = 14 + currentPos->GValue;//FValue = 15 + grid->getDistance(temp, destination);
					//temp->HValue = grid->getDistance(temp, destination); 
					temp->FValue = temp->GValue + temp->HValue;	
				}
			}
			//if its not on the open list, set the parent to the current, compute G value
			//for this node using current node, add it to the open list
			else{
				temp->parent = currentPos;
				temp->GValue = 14 + temp->parent->GValue;
				temp->HValue = grid->getDistance(temp, destination); 
				temp->FValue = temp->GValue + temp->HValue;
				if(temp == NULL || currentPos == NULL){
					std::cout << "BREAK" << std::endl;
				}
				open.push_back(temp);
			}  }  }
	temp = grid->getSouthNode(currentPos);
	if(temp != NULL && temp->isClear()){ //if the null exists and its not blocked
		isfound = (std::find(closed.begin(), closed.end(), temp) != closed.end());  
		if (!isfound){   //make sure its not in the closed list
			//need to check if its in the open list
			isfound = (std::find(open.begin(), open.end(), temp) != open.end()); 
			//if its on the open list, we need to compute a new G value, 
			//from parent and then see if that G value is lower then current.
			//if the G value is lower, reset the parent of the node & G value
			if(isfound){
				if(10 + currentPos->GValue < temp->GValue){
					temp->parent = currentPos;
					temp->GValue = 10 + currentPos->GValue;//FValue = 15 + grid->getDistance(temp, destination);
					//temp->HValue = grid->getDistance(temp, destination); 
					temp->FValue = temp->GValue + temp->HValue;	
				}
			}
			//if its not on the open list, set the parent to the current, compute G value
			//for this node using current node, add it to the open list
			else{
				temp->parent = currentPos;
				temp->GValue = 10 + temp->parent->GValue;
				temp->HValue = grid->getDistance(temp, destination); 
				temp->FValue = temp->GValue + temp->HValue;
				if(temp == NULL || currentPos == NULL){
					std::cout << "BREAK" << std::endl;
				}
				open.push_back(temp);
			}  }  }
	temp = grid->getSWNode(currentPos);
	if(temp != NULL && temp->isClear()){ //if the null exists and its not blocked
		isfound = (std::find(closed.begin(), closed.end(), temp) != closed.end());  
		if (!isfound){   //make sure its not in the closed list
			//need to check if its in the open list
			isfound = (std::find(open.begin(), open.end(), temp) != open.end()); 
			//if its on the open list, we need to compute a new G value, 
			//from parent and then see if that G value is lower then current.
			//if the G value is lower, reset the parent of the node & G value
			if(isfound){
				if(14 + currentPos->GValue < temp->GValue){
					temp->parent = currentPos;
					temp->GValue = 14 + currentPos->GValue;//FValue = 15 + grid->getDistance(temp, destination);
					//temp->HValue = grid->getDistance(temp, destination); 
					temp->FValue = temp->GValue + temp->HValue;	
				}
			}
			//if its not on the open list, set the parent to the current, compute G value
			//for this node using current node, add it to the open list
			else{
				temp->parent = currentPos;
				temp->GValue = 14 + temp->parent->GValue;
				temp->HValue = grid->getDistance(temp, destination); 
				temp->FValue = temp->GValue + temp->HValue;
				if(temp == NULL || currentPos == NULL){
					std::cout << "BREAK" << std::endl;
				}
				open.push_back(temp);
			}  }  }
	temp = grid->getWestNode(currentPos);
	if(temp != NULL && temp->isClear()){ //if the null exists and its not blocked
		isfound = (std::find(closed.begin(), closed.end(), temp) != closed.end());  
		if (!isfound){   //make sure its not in the closed list
			//need to check if its in the open list
			isfound = (std::find(open.begin(), open.end(), temp) != open.end()); 
			//if its on the open list, we need to compute a new G value, 
			//from parent and then see if that G value is lower then current.
			//if the G value is lower, reset the parent of the node & G value
			if(isfound){
   				if(10 + currentPos->GValue < temp->GValue){
					temp->parent = currentPos;
					temp->GValue = 10 + currentPos->GValue;//FValue = 15 + grid->getDistance(temp, destination);
					//temp->HValue = grid->getDistance(temp, destination); 
					temp->FValue = temp->GValue + temp->HValue;	
				}
			}
			//if its not on the open list, set the parent to the current, compute G value
			//for this node using current node, add it to the open list
			else{
				temp->parent = currentPos;
				temp->GValue = 10 + temp->parent->GValue;
				temp->HValue = grid->getDistance(temp, destination); 
				temp->FValue = temp->GValue + temp->HValue;
				if(temp == NULL || currentPos == NULL){
					std::cout << "BREAK" << std::endl;
				}
				open.push_back(temp);
			}  }  }
	if(currentPos){ if(!(std::find(closed.begin(), closed.end(), currentPos) != closed.end())){
		closed.push_front(currentPos); }}
	else{
		std::cout << "CurrentPos = ERROR" << std::endl;
		//currentPos = mBodyNode->getPosition();

		Ogre::Vector3 currentPlace = mBodyNode->getPosition();
		int r = (currentPlace[2] - (NODESIZE/2) + ((grid->getRows() * NODESIZE)/2))/NODESIZE;
		int c = (currentPlace[0] - (NODESIZE/2) + ((grid->getCols() * NODESIZE)/2))/NODESIZE;
				//GridNode* start = grid->getNode(r, c);
				//if(temp){
		currentPos = grid->getNode(r, c);
		temp = currentPos;
		//closed.push_front(currentPos);
	}
	//temp = NULL;
	lowestF = 1000;
	std::list<GridNode*>::iterator iter;
		for (iter = open.begin(); iter != open.end(); iter++){
			
			if((*iter) == destination){
				walkBackwards(startPos, destination);
				return;
			}
			if((*iter)->FValue < lowestF){
				temp = (*iter);
				lowestF = (*iter)->FValue;
			}
		}
		if(temp == NULL){ 
			temp = startPos;
			closed.clear();
			return;
			
		}
		AStarWalk(startPos, temp, destination);

}

void Ninja::walkBackwards(GridNode* start, GridNode* Destination){
	temp = Destination;
	//start->setContains('S');
	
	int counter = 0;
	if(temp==NULL){
		open.clear();
		closed.clear();
		path.clear();
		return;
	}
	try{
		if(temp->parent == NULL){
			open.clear();
			closed.clear();
			path.clear();
			return;
		}
	}
	catch(const std::exception& e){
		return;
	}
	while(temp->parent != start){
		if(temp == NULL){
			std::cout << "ERROR" << std::endl;
			break;
		}
			//std::cout << "walkBackwards1" << std::endl;
			//std::cout << "Row: " << temp->getRow() << " Col: " << temp->getColumn()<< std::endl;
			path.push_front(temp);
			try{
				//if(temp->getRow() == 14 && temp->getColumn() == 6 && start->getRow() == 17 && start->getColumn() == 4){
				//	break;
				//}	
				if( !temp->isClear()){ //->parent == NULL || temp->parent->parent == NULL){
					return;
				}
			}
			catch(const std::exception& e){
				return;
			}
			temp = temp->parent;
	}
	
	path.push_front(temp);
	//std::cout << "walkBackwards2" << std::endl;
	//std::cout << "Row: " << temp->getRow() << " Col: " << temp->getColumn()<< std::endl;
	std::list<GridNode*>::iterator iter;
	for (iter = path.begin(); iter != path.end(); iter++){
		walkToNode((*iter));
		//(*iter)->setContains('0' + counter);
		counter++;
		(*iter) = NULL;
	}
	//Destination->setContains('E');
	//grid->printToFile();
	std::list<GridNode*>::iterator iterOPEN;
	for (iterOPEN = open.begin(); iterOPEN != open.end(); iterOPEN++){
		//(*iterOPEN)->parent = NULL;
		(*iterOPEN) = NULL;//->parent = NULL;
	}
	open.clear();		
	std::list<GridNode*>::iterator iterCLOSED;
	for (iterCLOSED = closed.begin(); iterCLOSED != closed.end(); iterCLOSED++){
		//(*iterCLOSED)->parent = NULL;
		(*iterCLOSED) = NULL;//->parent = NULL;
	}
	closed.clear();
	path.clear();
	

}

void Ninja::nextLoc(){
	addLocationTimer = 0;
	firstTime = false;
	//setGrid(grid);
				
			//find a random row and column
	int ranrow = rand() % grid->getRows();
	int rancol = rand() % grid->getCols();
			//get a gridNode for them to walk to.
			//GridNode* temp = grid->getNode(ranrow, rancol);
			//walk to the node
	Ogre::Vector3 currentPlace = getPosition();
	int r = (currentPlace[2] - (NODESIZE/2) + ((grid->getRows() * NODESIZE)/2))/NODESIZE;
	int c = (currentPlace[0] - (NODESIZE/2) + ((grid->getCols() * NODESIZE)/2))/NODESIZE;
								//GridNode* start = grid->getNode(r, c);
								//if(temp){
	if(grid->getNode(r, c) == NULL){
		std::cout << "ERROR" << std::endl;
	}
						//grid->clearContains();
	AStarWalk(grid->getNode(r, c), grid->getNode(r, c), grid->getNode(ranrow, rancol));
	Ogre::Vector3 currentPlace2 = getPosition();

}
