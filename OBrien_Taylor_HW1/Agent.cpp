#include "Agent.h"

Agent::Agent(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, std::string level)
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
	this->moveB = false;
	this->projectile = false;
	this->onBlock = false;
	this->powerJump = false;
	this->powerStart = 0.0;
	this->finalTimer.reset(); // = 0.0;
	this->alarmTimer.reset();
	this->alarm = false;
	this->poweron = true;
	this->health = 100;
	this->soundPlaying = false;
	this->hasKey = false; //change to false
	this->levelDone = false;
	if(level == "level001.txt"){
		this->level = 1;
	}
	else if(level == "level002.txt"){
		this->level = 2;
	}
	else if(level == "level003.txt"){
		this->level = 3;
	}

	alarmLight = mSceneMgr->createLight();
	alarmLight->setType(Light::LT_POINT);
	alarmLight->setPosition(0,200,0);
	alarmLight->setDiffuseColour(ColourValue::ColourValue(1, 0, 0));
	alarmLight->setVisible(false);
	alarmLight2 = mSceneMgr->createLight();
	alarmLight2->setType(Light::LT_POINT);
	alarmLight2->setPosition(-100,200,0);
	alarmLight2->setDiffuseColour(ColourValue::ColourValue(1, 0, 0));
	alarmLight2->setVisible(false);
	alarmLight3 = mSceneMgr->createLight();
	alarmLight3->setType(Light::LT_POINT);
	alarmLight3->setPosition(0,200,100);
	alarmLight3->setDiffuseColour(ColourValue::ColourValue(1, 0, 0));
	alarmLight3->setVisible(false);
	alarmLight4 = mSceneMgr->createLight();
	alarmLight4->setType(Light::LT_POINT);
	alarmLight4->setPosition(50,200,50);
	alarmLight4->setDiffuseColour(ColourValue::ColourValue(1, 0, 0));
	alarmLight4->setVisible(false);
	alarmLight5 = mSceneMgr->createLight();
	alarmLight5->setType(Light::LT_POINT);
	alarmLight5->setPosition(-50,200,-50);
	alarmLight5->setDiffuseColour(ColourValue::ColourValue(1, 0, 0));
	alarmLight5->setVisible(false);

	mBodyNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(); // create a new scene node
	mBodyEntity = mSceneMgr->createEntity(name, filename); // load the model
	mBodyNode->attachObject(mBodyEntity);	// attach the model to the scene node
	mBodyNode->attachObject(mSceneMgr->getCamera("PlayerCam"));
	
	//mBodyNode->translate(0,0, 25); //4.5,0); // make the Ogre stand on the plane (almost)
	mBodyNode->yaw(Ogre::Degree(90));
	setupAnimations();  // load the animation for this character

	// configure walking parameters
	mWalkSpeed = 30.0f;	
	mDirection = Ogre::Vector3::ZERO;
	mDistance = 0;
	//mDestination = Ogre::Vector3::ZERO;
	//mBodyEntity = 0;
	//mBodyNode = 0;
	
	mDestination = Ogre::Vector3::ZERO;
	// create random points to walk to 
//	float x, y;
//	Ogre::Entity* ent;
//	Ogre::SceneNode* node;
	
	ParticleSystem::setDefaultNonVisibleUpdateTimeout(5);  // set nonvisible timeout
	ps = mSceneMgr->createParticleSystem("Fountain1", "Examples/Aureola");
	Ogre::SceneNode* mnode = mBodyNode->createChildSceneNode();
	mnode->yaw(Degree(90));
	mnode->attachObject(ps);
	ps->setVisible(false);

	//House - for the exit
	entExit = mSceneMgr->createEntity("tudorhouse.mesh");
	entExit->setMaterialName("Examples/TudorHouse");
	nodeExit = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-120, 50, -82.0));
	nodeExit->yaw(Ogre::Degree(90)); //not 0, not 270
	nodeExit->scale(.1, .10, .10);
	nodeExit->attachObject(entExit);

	//Key - represented as.. different model
	entKey = mSceneMgr->createEntity("spine.mesh");
	entKey->setMaterialName("Examples/Chrome");
	nodeKey = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(22.9,2,-35.58)); //
	//nodeKey->yaw(Ogre::Degree(90));
	nodeKey->scale(0.02, 0.02, 0.02);
	nodeKey->attachObject(entKey);
	if(this->level == 2){
		nodeKey->setPosition(50.9, 2, 34.4);
	}
	else if(this->level == 3){
		nodeKey->setPosition(-23.6, 2, 30.1);
	}

	//box 1
	entBox = mSceneMgr->createEntity(Ogre::SceneManager::PT_CUBE);
	entBox->setMaterialName("Examples/RustySteel");
	nodeBox = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-62.6,10,-15.9));
	nodeBox->yaw(Ogre::Degree(90));
	nodeBox->attachObject(entBox);
	if(this->level == 1){
		nodeBox->scale(0.2, 0.01, 0.2);
	}
	else if(this->level == 2){
		nodeBox->setPosition(-10, 10, -60.0); //
		nodeBox->scale(0.05, 0.01, 0.1);
	}
	else if(this->level == 3){
		nodeBox->setPosition(46.7, 10, -34.7);
		nodeBox->scale(0.1, 0.01, 0.1);
	}
	
	//box2
	entBox2 = mSceneMgr->createEntity(Ogre::SceneManager::PT_CUBE);
	entBox2->setMaterialName("Examples/RustySteel");
	nodeBox2 = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-43.618,15,-15.9));
	nodeBox2->yaw(Ogre::Degree(90));
	nodeBox2->attachObject(entBox2);
	if(this->level == 1){
		nodeBox2->scale(0.2, 0.01, 0.2);
	}
	if(this->level == 2){
		nodeBox2->setPosition(10, 15, -60.0); //
		nodeBox2->scale(0.05, 0.01, 0.1);
	}
	else if(this->level == 3){
		nodeBox2->setPosition(29.3, 15, -30.3);
		nodeBox2->scale(0.1, 0.01, 0.1);
	}
	//box3
	entBox3 = mSceneMgr->createEntity(Ogre::SceneManager::PT_CUBE);
	entBox3->setMaterialName("Examples/RustySteel");
	nodeBox3 = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0,20,-15.9));
	nodeBox3->yaw(Ogre::Degree(90));
	
	nodeBox3->attachObject(entBox3);
	if(this->level == 1){
		nodeBox3->scale(0.2, 0.01, 0.7);
	}
	if(this->level == 2){
		nodeBox3->setPosition(30, 20, -60.0);
		nodeBox3->scale(0.05, 0.01, 0.05);
		
	}
	else if(this->level == 3){
		nodeBox3->setPosition(22.7, 18, -2.9);
		nodeBox3->scale(0.1, 0.01, 0.1);
	}
	//box4
	entBox4 = mSceneMgr->createEntity(Ogre::SceneManager::PT_CUBE);
	entBox4->setMaterialName("Examples/RustySteel");
	nodeBox4 = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(50, 22, -50.0));
	nodeBox4->yaw(Ogre::Degree(90));
	nodeBox4->scale(0.05, 0.01, 0.1);
	nodeBox4->attachObject(entBox4);
	if(this->level == 1){
		nodeBox4->scale(0,0,0);
	}
	else if(this->level == 3){
		nodeBox4->setPosition(38.1, 20, 19.1);
	}
	
	//box5
	entBox5 = mSceneMgr->createEntity(Ogre::SceneManager::PT_CUBE);
	entBox5->setMaterialName("Examples/RustySteel");
	nodeBox5 = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(70, 23, -40.0));
	nodeBox5->yaw(Ogre::Degree(90));
	nodeBox5->scale(0.05, 0.01, 0.1);
	nodeBox5->attachObject(entBox5);
	if(this->level == 1){
		nodeBox5->scale(0,0,0);
	}
	if(this->level == 3){
		nodeBox5->setPosition(17.1, 18, -42);
	}

	//box6
	entBox6 = mSceneMgr->createEntity(Ogre::SceneManager::PT_CUBE);
	entBox6->setMaterialName("Examples/RustySteel");
	nodeBox6 = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(75, 23, -10.0));
	nodeBox6->yaw(Ogre::Degree(90));
	nodeBox6->attachObject(entBox6);
	if (this->level == 1){
		nodeBox6->scale(0, 0, 0);
	}
	else if(this->level == 2){
		nodeBox6->scale(0.5, 0.01, 0.1);
	}
	else if(this->level == 3){
		nodeBox6->setPosition(25.8, 20, 52.6);
		nodeBox6->scale(0.1, 0.01, 0.1);
	}
	//box 7
	entBox7 = mSceneMgr->createEntity(Ogre::SceneManager::PT_CUBE);
	entBox7->setMaterialName("Examples/RustySteel");
	nodeBox7 = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(75, 23, 40.0));
	nodeBox7->yaw(Ogre::Degree(90));
	nodeBox7->scale(0.1, 0.01, 0.05);
	nodeBox7->attachObject(entBox7);
	if(this->level == 3){
		nodeBox7->setPosition(27.8, 15, 61);
	}

	puJumpEnt =  mSceneMgr->createEntity(Ogre::SceneManager::PT_CUBE);
	puJumpEnt->setMaterialName("Examples/TrippySkyBox");
	puJumpNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-35,5,-75.7)); //
	puJumpNode->scale(0.03, 0.03, 0.03);
	puJumpNode->attachObject(puJumpEnt);
	if(this->level == 2){
		puJumpNode->setPosition(0, 5, -89.0);
	}
	else if(this->level == 3){
		puJumpNode->setPosition(18.1, 5, -54.6);
	}

	entPowerOff =  mSceneMgr->createEntity(Ogre::SceneManager::PT_SPHERE);
	entPowerOff->setMaterialName("Examples/Hilite/Yellow");
	nodePowerOff = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(73.6, 0.5, -89.6));
	nodePowerOff->scale(0.03, 0.005, 0.03);
	nodePowerOff->attachObject(entPowerOff);
	if(this->level == 2 || this->level ==3){
		nodePowerOff->setPosition(-107.375, 0.5, .44); //
	}
	
	//entPowerOff
}

Agent::~Agent(){
	// mSceneMgr->destroySceneNode(mBodyNode); // Note that OGRE does not recommend doing this. It prefers to use clear scene
	// mSceneMgr->destroyEntity(mBodyEntity);
}

// update is called at every frame from GameApplication::addTime
void Agent::update(Ogre::Real deltaTime) 
{
	this->updateAnimations(deltaTime);	// Update animation playback
	this->updateLocomote(deltaTime);	// Update Locomotion
	if(rotatel){
		mBodyNode->yaw(Ogre::Degree(5));
	}
	if(rotater){
		mBodyNode->yaw(Ogre::Degree(-5));
	}
	if(moveF && !moveB){
		moveForward();
	}
	if(moveB && !moveF){
		moveBackwards();
	}
	if (projectile){ // Lecture 10
		shoot(deltaTime);
	}
	if(getPosition()[1] < 5){
		mBodyNode->setPosition(getPosition()[0], 5, getPosition()[2]);
	}
	puJumpNode->yaw(Ogre::Degree(3));

	//std::cout << finalTimer << std::endl;

	

	if(!powerJump){
		finalTimer.reset();
	}
	
	else if(powerJump){
		if(finalTimer.getMilliseconds() >= 40000){
			powerJump = false;
			puJumpNode->setVisible(true);
			ps->setVisible(false);
			finalTimer.reset();
		}
	}
	std::cout << getPosition() << std::endl;

	nodeKey->yaw(Ogre::Degree(2));
	nodeKey->roll(Ogre::Degree(2));
	
	if(alarm && alarmTimer.getMilliseconds() > 800){
		alarmLight->setVisible(true);
		alarmLight2->setVisible(true);
		alarmLight3->setVisible(true);
		alarmLight4->setVisible(true);
		alarmLight5->setVisible(true);

	}
	if(alarm && alarmTimer.getMilliseconds() > 1000){
		alarmTimer.reset();
		alarmLight->setVisible(false);
		alarmLight2->setVisible(false);
		alarmLight3->setVisible(false);
		alarmLight4->setVisible(false);
		alarmLight5->setVisible(false);
	}
}



void Agent::setupAnimations()
{
	this->mTimer = 0;	// Start from the beginning
	this->mVerticalVelocity = 0;	// Not jumping

	// this is very important due to the nature of the exported animations
	mBodyEntity->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);

	// Name of the animations for this character
	Ogre::String animNames[] =
		{"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",
		"SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};

	// populate our animation list
	for (int i = 0; i < 13; i++)
	{
		mAnims[i] = mBodyEntity->getAnimationState(animNames[i]);
		mAnims[i]->setLoop(true);
		
		mFadingIn[i] = false;
		mFadingOut[i] = false;
	}

	// start off in the idle state (top and bottom together)
	setBaseAnimation(ANIM_IDLE_BASE);
	setTopAnimation(ANIM_IDLE_TOP);
	//setBaseAnimation(ANIM_RUN_BASE);
	//setTopAnimation(ANIM_RUN_TOP);

	// relax the hands since we're not holding anything
	mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
}

void Agent::setPosition(float x, float y, float z){
	this->mBodyNode->setPosition(x, y, z);//+height, z);

}

void Agent::setBaseAnimation(AnimID id, bool reset)
{
	if (mBaseAnimID >= 0 && mBaseAnimID < 13)
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
		mFadingIn[id] = true;
		if (reset) mAnims[id]->setTimePosition(0);
	}
}
	
void Agent::setTopAnimation(AnimID id, bool reset)
{
	if (mTopAnimID >= 0 && mTopAnimID < 13)
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

int Agent::getBaseAnimation(){
	return mBaseAnimID;
}

void Agent::updateAnimations(Ogre::Real deltaTime)
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

void Agent::setGrid(Grid* grid){
	this->grid = grid;
}

void Agent::walkToNode(GridNode* node){
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

void Agent::fadeAnimations(Ogre::Real deltaTime)
{
	using namespace Ogre;

	for (int i = 0; i < 13; i++)
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

bool Agent::nextLocation()
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

void Agent::rotateLeft(bool keypressed){
	this->rotatel = keypressed;
	/*while(keypressed){
	mBodyNode->yaw(Ogre::Degree(2));
	}*/

}
void Agent::rotateRight(bool keypressed){
	this->rotater = keypressed;
	/*while(keypressed){
	mBodyNode->yaw(Ogre::Degree(-2));
	}*/

}
void Agent::forwardKey(bool keypressed){
	this->moveF = keypressed;
}

void Agent::backwardsKey(bool keypressed){
	this->moveB = keypressed;
}

void Agent::moveForward(){
	using namespace std;
	
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
		//cout << "Clear node: " << r << ", " << c << endl;
	}
	
	if(mBodyNode->getPosition()[1] > 8 && !hoverBox()){
		if (!projectile){
			projectile = true;
		}
	}
	objBox = this->mBodyEntity->getWorldBoundingBox();
	Ogre::AxisAlignedBox BoxPU = this->puJumpEnt->getWorldBoundingBox();
	if(objBox.intersects(BoxPU)){
		puJumpNode->setVisible(false);
		powerJump = true;
		ps->setVisible(true);
	}
	Ogre::AxisAlignedBox KeyBox = this->entKey->getWorldBoundingBox();
	if(objBox.intersects(KeyBox)){
		nodeKey->setVisible(false);
		this->hasKey = true;
	}
	Ogre::AxisAlignedBox DoorBox = this->entExit->getWorldBoundingBox();
	if(objBox.intersects(DoorBox) && this->hasKey){
		std::cout << "NEXT LEVEL" << std::endl;
		this->levelDone = true;
	}

	PowerBox = this->entPowerOff->getWorldBoundingBox();
	if(objBox.intersects(PowerBox)){
		if(poweron){
			alarm = true;
			poweron = false;
			alarmTimer.reset();
		}
	}
}
void Agent::moveBackwards(){
	using namespace std;
	
	Ogre::Vector3 initialPlace = mBodyNode->getPosition();
	
	int initialR = (initialPlace[2] - (NODESIZE/2) + ((grid->getRows() * NODESIZE)/2))/NODESIZE;
	int initialC = (initialPlace[0] - (NODESIZE/2) + ((grid->getCols() * NODESIZE)/2))/NODESIZE;

	mBodyNode->translate(Ogre::Vector3(0, 0, 1), Ogre::Node::TS_LOCAL);
	Ogre::Vector3 currentPlace = mBodyNode->getPosition();
	
	int r = (currentPlace[2] - (NODESIZE/2) + ((grid->getRows() * NODESIZE)/2))/NODESIZE;
	int c = (currentPlace[0] - (NODESIZE/2) + ((grid->getCols() * NODESIZE)/2))/NODESIZE;
	
	if (grid->getNode(r, c) != NULL){
		if(!grid->getNode(r, c)->isClear()){ // || !grid->getNode(r+1, c)->isClear() || !grid->getNode(r, c+1)->isClear()) ) {
			//->contains('B')){
			mBodyNode->translate(Ogre::Vector3(0, 0, -1), Ogre::Node::TS_LOCAL);
			cout << "Blocked Node: " << r << ", " << c << endl;
		}
		else if(initialR < r && !grid->getNode(r-1, c)->isClear()){
			mBodyNode->translate(Ogre::Vector3(0, 0, -1), Ogre::Node::TS_LOCAL);
			cout << "Blocked Node (r): " << r << ", " << c << endl;
		}
		else if(initialC < c && !grid->getNode(r, c-1)->isClear()){
			mBodyNode->translate(Ogre::Vector3(0, 0, -1), Ogre::Node::TS_LOCAL);
			cout << "Blocked Node (c): " << r << ", " << c << endl;
		}
	}

	else if(grid->getNode(r, c) == NULL){
		cout << "Null node: " << r << ", " << c << endl;
	}
	else{
		//cout << "Clear node: " << r << ", " << c << endl;
	}
	
	if(mBodyNode->getPosition()[1] > 8 && !hoverBox()){
		if (!projectile){
			projectile = true;
		}
	}
	objBox = this->mBodyEntity->getWorldBoundingBox();
	Ogre::AxisAlignedBox BoxPU = this->puJumpEnt->getWorldBoundingBox();
	if(objBox.intersects(BoxPU)){
		puJumpNode->setVisible(false);
		powerJump = true;
		ps->setVisible(true);
	}
	
	Ogre::AxisAlignedBox KeyBox = this->entKey->getWorldBoundingBox();
	if(objBox.intersects(KeyBox)){
		nodeKey->setVisible(false);
		this->hasKey = true;
	}
	
	Ogre::AxisAlignedBox DoorBox = this->entExit->getWorldBoundingBox();
	if(objBox.intersects(DoorBox) && this->hasKey){
		std::cout << "NEXT LEVEL" << std::endl;
		this->levelDone = true;
	}

	PowerBox = this->entPowerOff->getWorldBoundingBox();
	if(objBox.intersects(PowerBox)){
		if(poweron){
			alarm = true;
			poweron = false;
			alarmTimer.reset();
		}
	}
}


Ogre::Vector3 Agent::getPosition(){
	return mBodyNode->getPosition();
}

void Agent::updateLocomote(Ogre::Real deltaTime)
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

void Agent::physicsJump(){
	//air blowing up particle simulator?
	if(!this->projectile || onBlock){
		projectile = true; // turns on the movement
	//	this->setBaseAnimation(ANIM_NOD);
		// set up the initial state
		//initPos = this->mBodyNode->getPosition();
		vel.x = 0;
		vel.y = 15;
		if(powerJump){
			vel.y = 80;
		}
		vel.z = 0; //-mWalkSpeed;

	
		gravity.x = 0;
		gravity.y = -9.81;
		gravity.z = 0;
	
		//this->mBodyNode->yaw(Ogre::Degree(180));
		//this->mBodyNode->pitch(Ogre::Degree(45));
		//this->mBodyNode->showBoundingBox(true); 
		
	}
}

void Agent::shoot(Ogre::Real deltaTime) // lecture 10 call for every frame of the animation
{
	using namespace Ogre;

	Vector3 pos = this->mBodyNode->getPosition();

	// Calculate projectile motion here. What is vel? and What is new pos?
	//mass = 5;
	vel += deltaTime * gravity * 10; // 5 is the mass
	pos += 0.5*vel*deltaTime;


	this->mBodyNode->setPosition(pos);

	objBox = this->mBodyEntity->getWorldBoundingBox();
	//objBox.intersects(objBox); 
	Box1 = this->entBox->getWorldBoundingBox();
	Box2 = this->entBox2->getWorldBoundingBox();
    Box3 = this->entBox3->getWorldBoundingBox();
    Box4 = this->entBox4->getWorldBoundingBox();
	Box5 = this->entBox5->getWorldBoundingBox();
	Box6 = this->entBox6->getWorldBoundingBox();
	Box7 = this->entBox7->getWorldBoundingBox();
	
	
	if(collideBox()){
		projectile = false;
		this->mBodyNode->setPosition(getPosition()[0], getPosition()[1] + 2, getPosition()[2]);
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
	}
}

bool Agent::collideBox(){
	if(objBox.intersects(Box1)  || objBox.intersects(Box2) || objBox.intersects(Box3) ||objBox.intersects(Box4) ||objBox.intersects(Box5) || objBox.intersects(Box6) || objBox.intersects(Box7)){
		return true;
	}
	return false;
}

void Agent::buildBoxes(int level){


}

bool Agent::hoverBox(){
	objBox = this->mBodyEntity->getWorldBoundingBox();
	Box1 = this->entBox->getWorldBoundingBox();
	Box2 = this->entBox2->getWorldBoundingBox();
	Box3 = this->entBox3->getWorldBoundingBox();
	Box4 = this->entBox4->getWorldBoundingBox();
	Box5 = this->entBox5->getWorldBoundingBox();
	Box6 = this->entBox6->getWorldBoundingBox();
	Box7 = this->entBox7->getWorldBoundingBox();
	Ogre::Real test = Box1.distance(mBodyNode->getPosition());
	Ogre::Real test2 = Box2.distance(mBodyNode->getPosition());
	Ogre::Real test3 = Box3.distance(mBodyNode->getPosition());
	Ogre::Real test4 = Box4.distance(mBodyNode->getPosition());
	Ogre::Real test5 = Box5.distance(mBodyNode->getPosition());
	Ogre::Real test6 = Box6.distance(mBodyNode->getPosition());
	Ogre::Real test7 = Box7.distance(mBodyNode->getPosition());
	if( test < 10 || test2 < 10 || test3 < 10 || test4 < 10 || test5 < 10 || test6 < 10 || test7 < 10){
		return true;
	}
	return false;

}






