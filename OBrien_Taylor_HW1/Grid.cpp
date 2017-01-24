#include "Grid.h"
#include <iostream>
#include <fstream>

////////////////////////////////////////////////////////////////
// create a node
GridNode::GridNode(int nID, int row, int column, bool isC)
{
	this->clear = isC;

	this->rCoord = row;
	this->cCoord = column;
	this->parent = NULL;
	this->entity = NULL;

	if (isC)
		this->contains = '.';
	else
		this->contains = 'B';
}

// default constructor
GridNode::GridNode()
{
	nodeID = -999;			// mark these as currently invalid
	this->clear = true;
	this->contains = '.';
} 

////////////////////////////////////////////////////////////////
// destroy a node
GridNode::~GridNode()
{}  // doesn't contain any pointers, so it is just empty

////////////////////////////////////////////////////////////////
// set the node id
void GridNode::setID(int id)
{
	this->nodeID = id;
}

////////////////////////////////////////////////////////////////
// set the x coordinate
void GridNode::setRow(int r)
{
	this->rCoord = r;
}

////////////////////////////////////////////////////////////////
// set the y coordinate
void GridNode::setColumn(int c)
{
	this->cCoord = c;
}

////////////////////////////////////////////////////////////////
// get the x and y coordinate of the node
int GridNode::getRow()
{
	if(rCoord){
		return rCoord;
	}
	return NULL;
}

int GridNode::getColumn()
{
	if(cCoord){
		return cCoord;
	}
	return NULL;
}

// return the position of this node given the number of rows and columns as parameters
Ogre::Vector3 GridNode::getPosition(int rows, int cols)
{
	
	Ogre::Vector3 t;
	t.z = (rCoord * NODESIZE) - (rows * NODESIZE)/2.0 + (NODESIZE/2.0); 
	t.y = 0; 
	t.x = (cCoord * NODESIZE) - (cols * NODESIZE)/2.0 + (NODESIZE/2.0); 

	return t;
}

////////////////////////////////////////////////////////////////
// set the node as walkable
void GridNode::setClear()
{
	this->clear = true;
	this->contains = '.';
}

////////////////////////////////////////////////////////////////
// set the node as occupied
void GridNode::setOccupied()
{
	this->clear = false;
	this->contains = 'B';
}

void GridNode::setContains(char containsChar){
	this->contains = containsChar;
}

////////////////////////////////////////////////////////////////
// is the node walkable
bool GridNode::isClear()
{
	return this->clear;
}

void Grid::clearContains(){
	int r = nRows;
	int c = nCols;
	GridNode* temp;
	// put the coordinates in each node
	int count = 0;
	for (int i = 0; i < r; i++){
		for (int j = 0; j < c; j++){
			temp = getNode(i,j);
			if(temp->contains != 'B'){
				temp->contains = '.';
			}
		}
	}


}
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// create a grid
Grid::Grid(Ogre::SceneManager* mSceneMgr, int numRows, int numCols)
{
	this->mSceneMgr = mSceneMgr; 

	assert(numRows > 0 && numCols > 0);
	this->nRows = numRows;
	this->nCols = numCols;

	data.resize(numCols, GridRow(numRows));
		
	// put the coordinates in each node
	int count = 0;
	for (int i = 0; i < numRows; i++)
		for (int j = 0; j < numCols; j++)
		{
			GridNode *n = this->getNode(i,j);
			n->setRow(i);
			n->setColumn(j);
			n->setID(count);
			count++;
		}
}

/////////////////////////////////////////
// destroy a grid
Grid::~Grid(){};  														

////////////////////////////////////////////////////////////////
// get the node specified 
GridNode* Grid::getNode(int r, int c)
{
	if (r >= nRows || c >= nCols || r < 0 || c < 0)
		return NULL;

	return &this->data[c].data[r];
}
//getters for Rows/Cols
int Grid::getRows(){
	return nRows;
}

int Grid::getCols(){
	return nCols;
}


////////////////////////////////////////////////////////////////
// get adjacent nodes;
GridNode* Grid::getNorthNode(GridNode* n)
{
	if(!n){ return NULL; }
	//std::cout << "NorthNode" << std::endl;
	int oneRowUp = n->getRow() - 1;
	int sameCol = n->getColumn();
	if(oneRowUp < 0) { return NULL; }
	return getNode(oneRowUp, sameCol);
}

GridNode* Grid::getSouthNode(GridNode* n)
{
	if(!n){ return NULL; }
	//std::cout << "SouthNode" << std::endl;
	if (n->getRow() + 1 > nRows){ return NULL; }
	int oneRowDown = n->getRow() + 1;
	int sameCol = n->getColumn();
	return getNode(oneRowDown, sameCol);
}

GridNode* Grid::getEastNode(GridNode* n)
{
	if(!n){ return NULL; }
	//std::cout << "EastNode" << std::endl;
	if(n->getColumn() + 1 > nCols){ return NULL; }
	int sameRow = n->getRow();
	int rightCol = n->getColumn() + 1;
	return getNode(sameRow, rightCol);
	
}

GridNode* Grid::getWestNode(GridNode* n)
{
	if(!n){ return NULL; }
	//std::cout << "WestNode" << std::endl;
	if(n->getColumn()-1 < 0) { return NULL;}
	int sameRow = n->getRow();
	int leftCol = n->getColumn()-1;
	return getNode(sameRow, leftCol);
}

GridNode* Grid::getNENode(GridNode* n)  
{
	if(!n){ return NULL; }
//	std::cout << "NENode" << std::endl;
	if(n->getColumn() + 1 > nCols || n->getRow() -1 < 0) { return NULL; }
	int oneRowUp = n->getRow() - 1;
	int rightCol = n->getColumn() + 1;
	return getNode(oneRowUp, rightCol);
}

GridNode* Grid::getNWNode(GridNode* n) 
{
	if(!n){ return NULL; }
//	std::cout << "NWNode" << std::endl;
	if(n->getColumn() -1 < 0 || n->getRow() - 1 < 0) {return NULL;}
	int oneRowUp = n->getRow() - 1;
	int leftCol = n->getColumn() - 1;
	return getNode(oneRowUp, leftCol);
}

GridNode* Grid::getSENode(GridNode* n) 
{
	if(!n){ return NULL; }
//	std::cout << "SENode" << std::endl;
	if(n->getColumn() + 1 > nCols || n->getRow() + 1 > nRows) { return NULL;}
	int oneRowDown = n->getRow() + 1;
	int rightCol = n->getColumn() +1;
	return getNode(oneRowDown, rightCol);
}

GridNode* Grid::getSWNode(GridNode* n) 
{
	if(!n){ return NULL; }
//	std::cout << "SWNode" << std::endl;
	if(n->getColumn() - 1 < 0 || n->getRow() + 1 > nRows) {return NULL;}
	int oneRowDown = n->getRow() + 1;
	int leftCol = n->getColumn() -1;
	return getNode(oneRowDown, leftCol);
}
////////////////////////////////////////////////////////////////
//get distance between between two nodes
int Grid::getDistance(GridNode* node1, GridNode* node2)
{
	//std::cout << "getDistance" << std::endl;
	int RowDiff = abs(node1->getRow() - node2->getRow());
	int ColDiff = abs(node1->getColumn() - node2->getColumn());

	return RowDiff + ColDiff;
}

///////////////////////////////////////////////////////////////////////////////
// Print out the grid in ASCII
void Grid::printToFile()
{
	std::string path = __FILE__; //gets the current cpp file's path with the cpp file
	path = path.substr(0,1+path.find_last_of('\\')); //removes filename to leave path
	path+= "Grid.txt"; //if txt file is in the same directory as cpp file
	std::ofstream outFile;
	outFile.open(path);

	if (!outFile.is_open()) // oops. there was a problem opening the file
	{
		std::cout << "ERROR, FILE COULD NOT BE OPENED" << std::endl;	
		return;
	}

	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{
			outFile << this->getNode(i, j)->contains << " ";
		}
		outFile << std::endl;
	}
	outFile.close();
}

// load and place a model in a certain location.
void Grid::loadObject(std::string name, std::string filename, int row, int height, int col, float scale)
{
	using namespace Ogre;

	if (row >= nRows || col >= nCols || row < 0 || col < 0)
		return;

	Entity *ent = mSceneMgr->createEntity(name, filename);
    SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode(name,
        Ogre::Vector3(0.0f, 0.0f,  0.0f));
    node->attachObject(ent);
    node->setScale(scale, scale, scale);


	GridNode* gn = this->getNode(row, col);
	node->setPosition(getPosition(row, col)); 
	node->setPosition(getPosition(row, col).x, height, getPosition(row, col).z);
	gn->setOccupied();
	gn->entity = ent;
}

////////////////////////////////////////////////////////////////////////////
// Added this method and changed GridNode version to account for varying floor 
// plane dimensions. Assumes each grid is centered at the origin.
// It returns the center of each square. 
Ogre::Vector3 Grid::getPosition(int r, int c)	
{
	Ogre::Vector3 t;
	t.z = (r * NODESIZE) - (this->nRows * NODESIZE)/2.0 + NODESIZE/2.0; 
	t.y = 0; 
	t.x = (c * NODESIZE) - (this->nCols * NODESIZE)/2.0 + NODESIZE/2.0; 
	return t;
}