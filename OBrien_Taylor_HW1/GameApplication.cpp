#include "GameApplication.h"
 // Lecture 5
#include <fstream>
#include <sstream>
#include <map> 

//-------------------------------------------------------------------------------------
GameApplication::GameApplication(void)
{
	agent = NULL; // Init member data
	this->play = false;
	this->playalarm = false;
	this->reloadedGui = false;
}
//-------------------------------------------------------------------------------------
GameApplication::~GameApplication(void)
{
	if (agent != NULL)  // clean up memory
		delete agent; 
}

//-------------------------------------------------------------------------------------
void GameApplication::createScene(void)
{
    loadEnv("startScreen.txt", "Floor", "floor");
	this->currentLevel = "Start";
	level = 0;
	//setupEnv();
	loadObjects();
	loadCharacters();
	/*
	PlaySound("beep.wav", NULL, SND_LOOP | SND_ASYNC);
	int retval2 = waveOutGetNumDevs();
	if(retval2>=1){
		retval2 = waveOutSetVolume(0, 0);
	}
	*/
	PlaySound("gunfire.wav", NULL, SND_LOOP | SND_ASYNC);
	int retval = waveOutGetNumDevs();
	if(retval>=1){
		retval = waveOutSetVolume(0, 0);
	}


	
	
}

void GameApplication::createGUI(void)
{
	//////////////////////////////////////////////////////////////////////////////////
	// Lecture 12GUI
	
	if(currentLevel != "Start"){
	if (mTrayMgr == NULL) return;
	mTrayMgr->hideCursor();
	using namespace OgreBites;
	//if(mTrayMgr->getWidget("S
	if(mTrayMgr->getWidget("SampleSlider") == NULL){
		mSampleSlider = mTrayMgr->createThickSlider(TL_TOPRIGHT, "SampleSlider", "Health", 250, 80, 0, 0, 0);
		mSampleSlider->setRange(0,100,1000);
	//mTrayMgr->sliderMoved(mSampleSlider);

		
		mStunCoolDown = mTrayMgr->createThickSlider(TL_TOPLEFT, "StunSlider", "Stun Cool Down", 250, 80, 0, 15, 150);
		
		Ogre::StringVector items;
		items.push_back("Level");
		mParamsPanel = mTrayMgr->createParamsPanel(OgreBites::TL_TOP,"SamplePanel",110,items);

	}
	mSampleSlider->setValue(100);
	mStunCoolDown->setValue(0);
	// Lecture 12GUI: Setup parameter panel: Updated in addTime
	

	
	mTrayMgr->showAll();
	}
	else{ //if start up screen
		if(mTrayMgr == NULL) return;
		//Ogre::StringVector items;
		//items.push_back("Instructions");
		Ogre::FontManager& fm = Ogre::FontManager::getSingleton();
		Ogre::FontPtr valueFontPtr = fm.getByName("SdkTrays/Value");
		if(!valueFontPtr.isNull()){
			valueFontPtr.getPointer()->load();
		}
		mTextBox = mTrayMgr->createTextBox(OgreBites::TL_TOPLEFT, "instruc", "Instructions:", 300, 450);
		mTextBox->setText("Use the arrow Keys to move, space to jump and X to stun the ninjas. You are trying to get out of the maze, by avoiding all robots and ninjas. The robots are protecting the key to the door, but you must turn off their power before getting the key. You can stun the ninjas but that only lasts for 5 seconds. There is also a powerup box that will allow you to jump higher, but only for a small amount of time. Be careful with the powerup though, the ninjas messed with it and there's a chance it won't always work (Double jump helps!). Best of luck! Press S to start.");
		OgreBites::Label* title = mTrayMgr->createLabel(OgreBites::TL_RIGHT, "title", "Ninja Nightmare", 250);

		//mTextBox->setPadding(5);
		//mTextBox->setTextAlignment(Ogre::TextAreaOverlayElement::Left);
		//mTextBox->refitContents();
		mTrayMgr->showAll();
		//mTrayMgr->
		//mTextBox->
		//createParamsPanel(OgreBites::TL_TOP,"SamplePanel",500,items);
		//mParamsPanel->setParamValue(0, "Use the arrow Keys to move around the maze.\nYou are trying to get out of the maze, \
			by avoiding all robots and ninjas. The robots are protecting the key to the door, but you must turn off their power \
			before getting the key. You can stun the ninjas by pressing the 'X' button, but that only lasts for 5 seconds. \
			There is also a powerup box that will allow you to jump higher, but only for a small amount of time. \
			Be careful with the powerup though, the ninjas messed with it and there's a chance it won't always work. Best of luck! Press enter to start.");

		if(!reloadedGui){
			reloadedGui = true;
			mTrayMgr->toggleAdvancedFrameStats();
		}

	}

	//////////////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
// Lecture 5: Returns a unique name for loaded objects and agents
std::string getNewName() // return a unique name 
{
	static int count = 0;	// keep counting the number of objects

	std::string s;
	std::stringstream out;	// a stream for outputing to a string
	out << count++;			// make the current count into a string
	s = out.str();
	//std::cout << "object_" << s << std::endl;
	return "object_" + s;	// append the current count onto the string
}

// Lecture 5: Load level from file!
// Load the buildings or ground plane, etc
void GameApplication::loadEnv(std::string level, std::string name1, std::string name2)
{
	using namespace Ogre;	// use both namespaces
	using namespace std;

	class readEntity // need a structure for holding entities
	{
	public:
		string filename;
		float y;
		float scale;
		float orient;
		bool agent;
	};

	ifstream inputfile;		// Holds a pointer into the file

	string path = __FILE__; //gets the current cpp file's path with the cpp file
	path = path.substr(0,1+path.find_last_of('\\')); //removes filename to leave path
	path+= level; //if txt file is in the same directory as cpp file
	inputfile.open(path);

	//inputfile.open("D:/CS425-2012/Lecture 8/GameEngine-loadLevel/level001.txt"); // bad explicit path!!!
	if (!inputfile.is_open()) // oops. there was a problem opening the file
	{
		cout << "ERROR, FILE COULD NOT BE OPENED" << std::endl;	// Hmm. No output?
		return;
	}

	// the file is open
	int x,z;
	inputfile >> x >> z;	// read in the dimensions of the grid
	string matName;
	inputfile >> matName;	// read in the material name

	// create floor mesh using the dimension read
	MeshManager::getSingleton().createPlane(name2, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		Plane(Vector3::UNIT_Y, 0), x*NODESIZE, z*NODESIZE, x, z, true, 1, x, z, Vector3::UNIT_Z);
	
	//create a floor entity, give it material, and place it at the origin
	Entity* floor = mSceneMgr->createEntity(name1, name2);
	floor->setMaterialName(matName);
	AxisAlignedBox a = floor->getWorldBoundingBox();
	floor->setCastShadows(false);
	SceneNode* pNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	pNode->attachObject(floor);//mSceneMgr->getRootSceneNode()->attachObject(floor);

	grid = new Grid(mSceneMgr, z, x); // Set up the grid. z is rows, x is columns
	
	string buf;
	inputfile >> buf;	// Start looking for the Objects section
	while  (buf != "Objects")
		inputfile >> buf;
	if (buf != "Objects")	// Oops, the file must not be formated correctly
	{
		cout << "ERROR: Level file error" << endl;
		return;
	}

	// read in the objects
	readEntity *rent = new readEntity();	// hold info for one object
	std::map<string,readEntity*> objs;		// hold all object and agent types;
	while (!inputfile.eof() && buf != "Characters") // read through until you find the Characters section
	{ 
		inputfile >> buf;			// read in the char
		if (buf != "Characters")
		{
			inputfile >> rent->filename >> rent->y >> rent->orient >> rent->scale;  // read the rest of the line
			rent->agent = false;		// these are objects
			objs[buf] = rent;			// store this object in the map
			rent = new readEntity();	// create a new instance to store the next object
		}
	}

	while  (buf != "Characters")	// get through any junk
		inputfile >> buf;
	
	// Read in the characters
	while (!inputfile.eof() && buf != "World") // Read through until the world section
	{
		inputfile >> buf;		// read in the char
		if (buf != "World")
		{
			inputfile >> rent->filename >> rent->y >> rent->scale; // read the rest of the line
			rent->agent = true;			// this is an agent
			objs[buf] = rent;			// store the agent in the map
			rent = new readEntity();	// create a new instance to store the next object
		}
	}
	delete rent; // we didn't need the last one

	// read through the placement map
	char c;
	for (int i = 0; i < z; i++)			// down (row)
		for (int j = 0; j < x; j++)		// across (column)
		{
			inputfile >> c;			// read one char at a time
			buf = c + '\0';			// convert char to string
			rent = objs[buf];		// find cooresponding object or agent
			if (rent != NULL)		// it might not be an agent or object
				if (rent->agent && rent->filename == "sinbad.mesh" )	// if it is an agent...
				{
					// Use subclasses instead!
					agent = new Agent(this->mSceneMgr, getNewName(), rent->filename, rent->y, rent->scale, level);
					agentList.push_back(agent);
					agent->setPosition(grid->getPosition(i,j).x, 5, grid->getPosition(i,j).z);

					// If we were using different characters, we'd have to deal with 
					// different animation clips. 
				}
				else if(rent->agent && rent->filename == "robot.mesh"){
					shootingEnemy = new ShootingEnemies(this->mSceneMgr, getNewName(), rent->filename, rent->y, rent->scale);
					seList.push_back(shootingEnemy);
					shootingEnemy->setPosition(grid->getPosition(i,j).x, 5, grid->getPosition(i,j).z);
				}
				else if(rent->agent && rent->filename == "ninja.mesh"){
					ninja = new Ninja(this->mSceneMgr, getNewName(), rent->filename, rent->y, rent->scale);
					ninjaList.push_back(ninja);
					ninja->setPosition(grid->getPosition(i,j).x, 5, grid->getPosition(i,j).z);
				}
				else	// Load objects
				{
					grid->loadObject(getNewName(), rent->filename, i, rent->y, j, rent->scale);
				}
			else // not an object or agent
			{
				if (c == 'w') // create a wall
				{
					Entity* ent = mSceneMgr->createEntity(getNewName(), Ogre::SceneManager::PT_CUBE);
					ent->setMaterialName("Examples/RustySteel");
					Ogre::SceneNode* mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
					mNode->attachObject(ent);
					mNode->scale(0.1f,0.5f,0.1f); // cube is 100 x 100
					grid->getNode(i,j)->setOccupied();  // indicate that agents can't pass through
					mNode->setPosition(grid->getPosition(i,j).x, 10.0f, grid->getPosition(i,j).z);
				}
				else if (c == 'e')
				{
					ParticleSystem::setDefaultNonVisibleUpdateTimeout(5);  // set nonvisible timeout
					ParticleSystem* ps = mSceneMgr->createParticleSystem(getNewName(), "Examples/PurpleFountain");
					Ogre::SceneNode* mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
					mNode->attachObject(ps);
					mNode->setPosition(grid->getPosition(i,j).x, 0.0f, grid->getPosition(i,j).z);
				}
			}
		}
	
	// delete all of the readEntities in the objs map
	rent = objs["s"]; // just so we can see what is going on in memory (delete this later)
	
	std::map<string,readEntity*>::iterator it;
	for (it = objs.begin(); it != objs.end(); it++) // iterate through the map
	{
		delete (*it).second; // delete each readEntity
	}
	objs.clear(); // calls their destructors if there are any. (not good enough)
	
	inputfile.close();
	grid->printToFile(); // see what the initial grid looks like.
}

// Set up lights, shadows, etc
void GameApplication::setupEnv()
{
	createGUI();

	using namespace Ogre;

	// set shadow properties
	mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
	mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
	mSceneMgr->setShadowTextureSize(1024);
	mSceneMgr->setShadowTextureCount(1);
	
	// disable default camera control so the character can do its own 
	//mCameraMan->setStyle(OgreBites::CS_FREELOOK); // CS_FREELOOK, CS_ORBIT, CS_MANUAL

	// use small amount of ambient lighting
	mSceneMgr->setAmbientLight(ColourValue(0.3f, 0.3f, 0.3f));

	// add a bright light above the scene
	Light* light = mSceneMgr->createLight();
	light->setType(Light::LT_POINT);
	light->setPosition(-10, 300, 20);
	light->setSpecularColour(ColourValue::White);
	
	if(currentLevel == "level002"){
		Light* boxLight = mSceneMgr->createLight();
		boxLight->setType(Light::LT_POINT);
		boxLight->setPosition(-10, 5, -60);
		//boxLight->setDirection(Ogre::Vector3(0,-1, 0));
		boxLight->setSpecularColour(ColourValue::White);
	}
	agent->setGrid(grid);
	agent->buildBoxes(level);
	std::list<Ninja*>::iterator iterthree;
	for(iterthree = ninjaList.begin(); iterthree != ninjaList.end(); iterthree++) {
		if(*iterthree != NULL){
			(*iterthree)->setGrid(grid);
		}
		
	}
	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8); // Lecture 4
}

// Load other props or objects
void GameApplication::loadObjects()
{

	// Lecture 5: comment out all because loading from file
	//using namespace Ogre;
	//
	//Ogre::Entity *ent;
	//Ogre::SceneNode *node;

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//// Lecture 4-hold
	//ent = mSceneMgr->createEntity("Gun", "38pistol.mesh");
	//node = mSceneMgr->getRootSceneNode()->createChildSceneNode("GunNode", Ogre::Vector3(5.0f, 1.4f,  5.0f));
	//node->attachObject(ent);
	//node->pitch(Degree(90));
	//node->setScale(0.1f, 0.1f, 0.1f);

	//ent = mSceneMgr->createEntity("Gun2", "9mm.mesh");
	//node = mSceneMgr->getRootSceneNode()->createChildSceneNode("GunNode2", Ogre::Vector3(5.0f, 1.4f,  5.0f));
	//node->attachObject(ent);
	//node->pitch(Degree(90));
	//node->setScale(0.1f, 0.1f, 0.1f);
	/////////////////////////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////
	//// Draw a line: Lecture 4
	//Ogre::ManualObject* myManualObject =  mSceneMgr->createManualObject("manual1"); 
	//Ogre::SceneNode* myManualObjectNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("manual1_node"); 
 //
	//// NOTE: The second parameter to the create method is the resource group the material will be added to.
	//// If the group you name does not exist (in your resources.cfg file) the library will assert() and your program will crash
	//Ogre::MaterialPtr myManualObjectMaterial = Ogre::MaterialManager::getSingleton().create("manual1Material","General"); 
	//myManualObjectMaterial->setReceiveShadows(false); 
	//myManualObjectMaterial->getTechnique(0)->setLightingEnabled(true); 
	//myManualObjectMaterial->getTechnique(0)->getPass(0)->setDiffuse(1,0,0,0); 
	//myManualObjectMaterial->getTechnique(0)->getPass(0)->setAmbient(1,0,0); 
	//myManualObjectMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(1,0,0); 
	////myManualObjectMaterial->dispose();  // dispose pointer, not the material
 // 
	//myManualObject->begin("manual1Material", Ogre::RenderOperation::OT_LINE_LIST); 
	//myManualObject->position(3, 2, 1); 
	//myManualObject->position(8, 1, 0); 
	//// etc 
	//myManualObject->end(); 
 //
	//myManualObjectNode->attachObject(myManualObject);
	/////////////////////////////////////////////////////////////////////
}

// Load actors, agents, characters
void GameApplication::loadCharacters()
{
	// Lecture 5: now loading from file
	// agent = new Agent(this->mSceneMgr, "Sinbad", "Sinbad.mesh");
}

void GameApplication::addTime(Ogre::Real deltaTime)
{
	updateSound();
	if(currentLevel != "Start" && currentLevel != "GameOver"){
	// Lecture 5: Iterate over the list of agents
	std::list<Agent*>::iterator iter;
	for (iter = agentList.begin(); iter != agentList.end(); iter++)
		if (*iter != NULL)
			(*iter)->update(deltaTime);
	std::list<ShootingEnemies*>::iterator itertwo;
	for(itertwo = seList.begin(); itertwo != seList.end(); itertwo++) {
		if(*itertwo != NULL){
			(*itertwo)->update(deltaTime, agent);
		}
	}
	std::list<Ninja*>::iterator iterthree;
	for(iterthree = ninjaList.begin(); iterthree != ninjaList.end(); iterthree++) {
		if(*iterthree != NULL){
			(*iterthree)->update(deltaTime, agent);
		}
		
	}
	if(ninja->onCooldown){
		mStunCoolDown->setValue(15 - ninja->stunCoolDown.getMilliseconds()/1000);
		//std::cout << ninja->stunCoolDown.getMilliseconds()/1000 << std::endl;
	}
	else{
		mStunCoolDown->setValue(0);
	}
	//updateSound();
	mSampleSlider->setValue(agent->health);

	if(agent->levelDone){
		//Change to next Level
		if(currentLevel == "level001"){
			this->currentLevel = "level002";
			std::cout << "Change to level 2." << std::endl;
			//clear the scene deletes everything needed
			mSceneMgr->clearScene();
			//deleting the grid to make new one
			delete(grid);
			//clears agent List - so theres no NULL ptrs in it
			agentList.clear();
			seList.clear();
			ninjaList.clear();
			//mTrayMgr->clearAllTrays();
			//mTrayMgr->destroyAllWidgets();
			//mTrayMgr->
			//call the Load envionment, with level2 text and then rename the floor
			loadEnv("level002.txt", "Floor2", "floor2");
			level = 2;
			//set up lights/cameras/etc
			setupEnv();
		

		}
		else if(currentLevel == "level002"){
			this->currentLevel = "level003";
			std::cout << "Change to level 3." << std::endl;
			//clear the scene deletes everything needed
			mSceneMgr->clearScene();
			//deleting the grid to make new one
			delete(grid);
			//clears agent List - so theres no NULL ptrs in it
			agentList.clear();
			seList.clear();
			ninjaList.clear();
			//mTrayMgr->clearAllTrays();
			//mTrayMgr->destroyAllWidgets();
			//mTrayMgr->
			//call the Load envionment, with level2 text and then rename the floor
			loadEnv("level003.txt", "Floor3", "floor3");
			level = 2;
			//set up lights/cameras/etc
			setupEnv();
		}
		else if(currentLevel == "level003"){
			currentLevel = "Winner";

		}

	}

	if(currentLevel == "level001"){
		mParamsPanel->setParamValue(0, "1");
	}
	else if(currentLevel == "level002"){
		mParamsPanel->setParamValue(0, "2");
	}
	else if(currentLevel == "level003"){
		mParamsPanel->setParamValue(0, "3");
	}

	if(agent->health <= 0){
		if(mTrayMgr->getWidget("gameOver") == NULL){
			OgreBites::Label* gameOverLabel = mTrayMgr->createLabel(OgreBites::TL_CENTER, "gameOver", "Game Over! Press S to go to Menu", 350);		
			currentLevel = "GameOver";
		}
	}
	if(currentLevel == "Winner"){
		if(mTrayMgr->getWidget("Winner") == NULL){
			OgreBites::Label* winnerLabel = mTrayMgr->createLabel(OgreBites::TL_CENTER, "Winner", "Congratulations! You Win! Press S", 350);
		}
	}

  }
}

void GameApplication::updateSound(){
	

	std::list<ShootingEnemies*>::iterator iterthree;
	for(iterthree = seList.begin(); iterthree != seList.end(); iterthree++) {
		if(*iterthree != NULL ){
			int xDiff = abs(agent->getPosition()[0] - (*iterthree)->getPosition()[0]);
			int zDiff = abs(agent->getPosition()[2] - (*iterthree)->getPosition()[2]);
			int Dist = xDiff + zDiff; //could max be 48
			if( xDiff < 35 && zDiff < 35 && abs(agent->getPosition()[1] - (*iterthree)->getPosition()[1]) < 10){
				//ps->setVisible(true);
				play = true;
				//max = 65535 = when xDiff + zDiff = 0
			}
			else{
				play = false;
			}
		}//end if
	}//end for
		if(play && agent->poweron && currentLevel != "Start" && currentLevel != "gameOver" && currentLevel != "Winner"){
			int retval = waveOutGetNumDevs();
			if(retval>=1){
				retval = waveOutSetVolume(0, 64338);
			}
		}
		else{
			int retval = waveOutGetNumDevs();
			if(retval>=1){
				retval = waveOutSetVolume(0, 0);
			}
		}
		/*
	if(!playalarm && !agent->poweron){
		playalarm = true;
		int retval = waveOutGetNumDevs();
		if(retval>=1){
			retval = waveOutSetVolume(0, 64338);
		}
	}
	*/
	

}

bool GameApplication::keyPressed( const OIS::KeyEvent &arg ) // Moved from BaseApplication
{
    if (mTrayMgr->isDialogVisible()) return true;   // don't process any more keys if dialog is up

    if (arg.key == OIS::KC_F)   // toggle visibility of advanced frame stats
    {
        mTrayMgr->toggleAdvancedFrameStats();
    }
    else if (arg.key == OIS::KC_G)   // toggle visibility of even rarer debugging details
    {
        if (mDetailsPanel->getTrayLocation() == OgreBites::TL_NONE)
        {
            mTrayMgr->moveWidgetToTray(mDetailsPanel, OgreBites::TL_TOPRIGHT, 0);
            mDetailsPanel->show();
        }
        else
        {
            mTrayMgr->removeWidgetFromTray(mDetailsPanel);
            mDetailsPanel->hide();
        }
    }
	/*
	else if (arg.key == OIS::KC_1 && ){
		std::cout << "Change to level 1." << std::endl;
		//Clear the scene deletes everything needed
		mSceneMgr->clearScene();
		//deleting the grid to make a new one
		delete(grid);
		//clears the agent list - so theres no NULL pts in it
		agentList.clear();
		//call the Load Env with level 1 text and rename floor
		loadEnv("level001.txt", "Floor", "floor");
		level = 1;
		//set up lights/cameras/etc
		setupEnv();
		


	}
	else if (arg.key == OIS::KC_2){
		std::cout << "Change to level 2." << std::endl;
		//clear the scene deletes everything needed
		mSceneMgr->clearScene();
		//deleting the grid to make new one
		delete(grid);
		//clears agent List - so theres no NULL ptrs in it
		agentList.clear();
		//call the Load envionment, with level2 text and then rename the floor
		loadEnv("level002.txt", "Floor2", "floor2");
		level = 2;
		//set up lights/cameras/etc
		setupEnv();
		
	}
	*/
	else if (arg.key == OIS::KC_SPACE && currentLevel != "Start" && currentLevel != "GameOver"){
		agent->physicsJump();
	}
	else if (arg.key == OIS::KC_X && currentLevel != "Start" && currentLevel != "GameOver"){
		std::list<Ninja*>::iterator iterthree;
	for(iterthree = ninjaList.begin(); iterthree != ninjaList.end(); iterthree++) {
		if(*iterthree != NULL){
			(*iterthree)->stun();
		}
		
	}

	}
    else if (arg.key == OIS::KC_T)   // cycle polygon rendering mode
    {
        Ogre::String newVal;
        Ogre::TextureFilterOptions tfo;
        unsigned int aniso;

        switch (mDetailsPanel->getParamValue(9).asUTF8()[0])
        {
        case 'B':
            newVal = "Trilinear";
            tfo = Ogre::TFO_TRILINEAR;
            aniso = 1;
            break;
        case 'T':
            newVal = "Anisotropic";
            tfo = Ogre::TFO_ANISOTROPIC;
            aniso = 8;
            break;
        case 'A':
            newVal = "None";
            tfo = Ogre::TFO_NONE;
            aniso = 1;
            break;
        default:
            newVal = "Bilinear";
            tfo = Ogre::TFO_BILINEAR;
            aniso = 1;
        }

        Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
        Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
        mDetailsPanel->setParamValue(9, newVal);
    }
    else if (arg.key == OIS::KC_R)   // cycle polygon rendering mode
    {
        Ogre::String newVal;
        Ogre::PolygonMode pm;

        switch (mCamera->getPolygonMode())
        {
        case Ogre::PM_SOLID:
            newVal = "Wireframe";
            pm = Ogre::PM_WIREFRAME;
            break;
        case Ogre::PM_WIREFRAME:
            newVal = "Points";
            pm = Ogre::PM_POINTS;
            break;
        default:
            newVal = "Solid";
            pm = Ogre::PM_SOLID;
        }

        mCamera->setPolygonMode(pm);
        mDetailsPanel->setParamValue(10, newVal);
    }
    else if(arg.key == OIS::KC_F5)   // refresh all textures
    {
        Ogre::TextureManager::getSingleton().reloadAll();
    }
    else if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
    {
        mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
    }
    else if (arg.key == OIS::KC_ESCAPE)
    {
        mShutDown = true;
    }
	else if(arg.key == OIS::KC_UP && currentLevel != "Start" && currentLevel != "GameOver"){ //|| arg.key == OIS::KC_DOWN){
		agent->forwardKey(true);
		//mCameraMan->injectKeyDown(arg);
	}
	else if(arg.key == OIS::KC_LEFT && currentLevel != "Start" && currentLevel != "GameOver"){
		agent->rotateLeft(true);
	}
	else if(arg.key == OIS::KC_RIGHT && currentLevel != "Start" && currentLevel != "GameOver"){
		agent->rotateRight(true);
	}
	else if(arg.key == OIS::KC_DOWN && currentLevel != "Start" && currentLevel != "GameOver"){
		agent->backwardsKey(true);
	}
	else if(arg.key == OIS::KC_S && currentLevel == "Start"){
		mTrayMgr->destroyWidget("instruc");
		mTrayMgr->destroyWidget("title");
		this->currentLevel = "level001";
		std::cout << "Change to level 1." << std::endl;
		//clear the scene deletes everything needed
		mSceneMgr->clearScene();
		//deleting the grid to make new one
		delete(grid);
		//clears agent List - so theres no NULL ptrs in it
		agentList.clear();
		seList.clear();
		ninjaList.clear();
		//mTrayMgr->clearAllTrays();
		//mTrayMgr->destroyAllWidgets();
		//mTrayMgr->
		//call the Load envionment, with level2 text and then rename the floor
		loadEnv("level001.txt", "Floor1", "floor1");
		level = 2;
		//set up lights/cameras/etc
		setupEnv();
	}
	else if(arg.key == OIS::KC_S && currentLevel == "Winner"){
		//if(mTrayMgr->getWidget("Winner") == NULL){
		currentLevel = "Start";
		mTrayMgr->destroyWidget("Winner"); //gameOver SampleSlider StunSlider SamplePanel
		mTrayMgr->destroyWidget("SampleSlider");
		mTrayMgr->destroyWidget("StunSlider");
		mTrayMgr->destroyWidget("SamplePanel");
		//mTrayMgr->destroyAllWidgets();
		mSceneMgr->clearScene();
		//deleting the grid to make new one
		//delete(grid);
		//clears agent List - so theres no NULL ptrs in it
		agentList.clear();
		seList.clear();
		ninjaList.clear();
		createGUI();
	}
	else if(arg.key == OIS::KC_S && currentLevel == "GameOver"){
		currentLevel = "Start";
		mTrayMgr->destroyWidget("gameOver"); //gameOver SampleSlider StunSlider SamplePanel
		mTrayMgr->destroyWidget("SampleSlider");
		mTrayMgr->destroyWidget("StunSlider");
		mTrayMgr->destroyWidget("SamplePanel");
		//mTrayMgr->destroyAllWidgets();
		mSceneMgr->clearScene();
		//deleting the grid to make new one
		//delete(grid);
		//clears agent List - so theres no NULL ptrs in it
		agentList.clear();
		seList.clear();
		ninjaList.clear();
		createGUI();

	}
	
    return true;
}

bool GameApplication::keyReleased( const OIS::KeyEvent &arg )
{
    
	if(arg.key == OIS::KC_UP && currentLevel != "Start" && currentLevel != "GameOver"){ //|| arg.key == OIS::KC_DOWN){
		agent->forwardKey(false);
		//mCameraMan->injectKeyDown(arg);
	}
	else if(arg.key == OIS::KC_LEFT && currentLevel != "Start" && currentLevel != "GameOver"){
		agent->rotateLeft(false);
	}
	else if(arg.key == OIS::KC_RIGHT && currentLevel != "Start" && currentLevel != "GameOver"){
		agent->rotateRight(false);
	}
	else if(arg.key == OIS::KC_DOWN && currentLevel != "Start" && currentLevel != "GameOver"){
		agent->backwardsKey(false);
	}
	
    return true;
}

bool GameApplication::mouseMoved( const OIS::MouseEvent &arg )
{
	/*
	if(currentLevel == "Start"){
		if (mTrayMgr->injectMouseMove(arg)) return true;
			mCameraMan->injectMouseMove(arg);
	}
	*/
    return true;
}

bool GameApplication::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    if (mTrayMgr->injectMouseDown(arg, id)) return true;
    mCameraMan->injectMouseDown(arg, id);
    return true;
}

bool GameApplication::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    if (mTrayMgr->injectMouseUp(arg, id)) return true;
    mCameraMan->injectMouseUp(arg, id);
    return true;
}