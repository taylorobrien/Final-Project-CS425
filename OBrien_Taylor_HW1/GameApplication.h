#ifndef __GameApplication_h_
#define __GameApplication_h_

#include "BaseApplication.h"
#include "Agent.h"
#include "Grid.h"
#include "Ninja.h"
#include "ShootingEnemies.h"

class Grid;
class Agent;
class ShootingEnemies;
class Ninja;

class GameApplication : public BaseApplication
{
private:
	Agent* agent; // store a pointer to the character
	std::list<Agent*> agentList; // Lecture 5: now a list of agents
	ShootingEnemies* shootingEnemy;		//pointer to shooting enemy
	std::list<ShootingEnemies*> seList;	//list of shooting enemies 
	Ninja* ninja; //pointer to ninja
	std::list<Ninja*> ninjaList; //list of ninjas
	OgreBites::Slider* mSampleSlider; //the slider used for health
	OgreBites::Slider* mStunCoolDown; //the slider used for cool down
	OgreBites::TextBox* mTextBox; //the text box used to store the level
	bool play; //is the game being played
	bool playalarm; //is the alarm being played
	std::string currentLevel; //what is the current level
	bool reloadedGui; //reloads gui once to fix bugs

	
public:
    GameApplication(void);
    virtual ~GameApplication(void);
	Grid* grid;		//set a pointer to the grid
	void loadEnv(std::string level, std::string name1, std::string name2);			// Load the buildings or ground plane, etc.
	void setupEnv();		// Set up the lights, shadows, etc
	void loadObjects();		// Load other props or objects (e.g. furniture)
	void loadCharacters();	// Load actors, agents, characters
	//std::list
	void addTime(Ogre::Real deltaTime);		// update the game state
	int level;			//the level currently played
	void createGUI(void); //creates the GUI elements for the game
	OgreBites::ParamsPanel* mParamsPanel; //gui element - not being used.
	//////////////////////////////////////////////////////////////////////////
	// Lecture 4: keyboard interaction
	// moved from base application
	// OIS::KeyListener
    bool keyPressed( const OIS::KeyEvent &arg ); // these all handles user input
    bool keyReleased( const OIS::KeyEvent &arg );
    // OIS::MouseListener
    bool mouseMoved( const OIS::MouseEvent &arg );
    bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	////////////////////////////////////////////////////////////////////////////
	void updateSound(); //updates the sound depeneding on state and level

protected:
    virtual void createScene(void);		//creates scene to be played.
};

#endif // #ifndef __TutorialApplication_h_
