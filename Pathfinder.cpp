//features
//	sidescrolling.... done!
//	portals
//	list of moves for shortest path between two points
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
using namespace std;

class MH_LevelState {
	enum peice {player = '+',enemy = 'X',exit = '*',pit = 'O',wall = 'X', floor = ' ', end = 'e', start = 's'};
	struct person {
		peice type;
		int x;
		int y;
		peice underPeice; //remember what is under the player for when they make their next move
	};
	struct screenView {
		int width;
		int length;
	};
	typedef vector<vector<peice> > MatrixPeice;
	MatrixPeice boardState;
	vector<person> enemies;
	screenView view;
	person playerPos;

	struct direction {
		int xadd;
		int yadd;
	};
	
	struct step {
		direction dir; //direction taken to GET to that position
		int x;
		int y;
	};
	typedef vector<step> path; //path is a vector of step
	
	// The following has been added to track starting and ending postions of the maze
	//-------------------------
	struct position {
		int x;
		int y;
	};
	position startPos;
	position endPos;
	//-------------------------

    public:
	//returns the shortest number of steps required to get from first coordinates to second coordinates
	int getNumberSteps(int xS, int yS, int xE, int yE)
	{
		vector<path> AllshortestPaths = getShortestPath(xS, yS, xE, yE);
		return AllshortestPaths.at(0).size();
	}
	//returns a vector of ALL(!) shortest paths to get from starting coordinates to ending coordinates.
	//This function can be very slow as it's finding all paths. Because of this it only removes paths.
	//that are clearly not going to be the shortest.
	//Paths that are removed include:
	//	-A path that collides with the wall ('=').
	//	-A path that collides with it'self
	//	-A path that collides with any other path at any step befor last step.
	vector<path> getShortestPath(int xS, int yS, int xE, int yE)
	{
		if(xS < 0 || yS < 0 || xE < 0 || yE < 0)
		{
			xS = startPos.x;
			yS = startPos.y;
			xE = endPos.x;
			yE = endPos.y;
		}
		vector<path> allPaths;
		allPaths.resize(1);
		allPaths.at(0).resize(1);
		allPaths.at(0).at(0).dir.xadd = 0;
		allPaths.at(0).at(0).dir.yadd = 0;
		allPaths.at(0).at(0).x = xS;
		allPaths.at(0).at(0).y = yS;		
		while(!foundShortestPath(allPaths, xE, yE))
		{
			printAllPaths(allPaths);
			printAllPathsOverBoard(allPaths);
			cout << endl << endl;
			allPaths = getNextPathVector(allPaths);
		}
		cout << allPaths.size();
		for(int i = 0; i < allPaths.size(); i++)
		{
			if(allPaths.at(i).back().x != xE || allPaths.at(i).back().y != yE)
			{
				allPaths.erase(allPaths.begin() + i);
				i--;
			}
			
		}
		printAllPaths(allPaths);
		printAllPathsOverBoard(allPaths);
		return allPaths;
	}
	//Checks if the shortest path was found
	bool foundShortestPath(vector<path> allPaths, int xE, int yE)
	{
		if(allPaths.size() == 0)
		{
			return true;
		}
		for(int i = 0; i < allPaths.size(); i++)
		{
		if (allPaths.at(i).back().x == xE && allPaths.at(i).back().y == yE)
			{
				return true;
			}
		}
		return false;
	}
	//takes one more step on each vector.
	vector<path> getNextPathVector(vector<path> allPaths)
	{
		int tempXadd, tempYadd;
		path tempPath;
		step tempStep;
		vector<path> newPaths;
		person p;
		for(int i = 0; i < allPaths.size(); i++)
		{
			for(int xadd = -1; xadd <= 1; xadd++)
			{	
				for(int yadd = -1; yadd <= 1; yadd++)
				{
					//cout << endl << endl << "start of loop. i is = " << i << " xadd = " << xadd << " yadd = " << yadd << endl;
					//cout << allPaths.at(i).back().dir.xadd << " and " << xadd << endl;
					if(xadd == 0 && yadd == 0) //avoid going no where
					{
						//cout << "skiped xadd = 0 and yadd = 0" << endl;
						continue;
					}
					if(positionExistsInAnyPath(allPaths.at(i).back().x + xadd, allPaths.at(i).back().y + yadd, allPaths))
					{
						//cout << "retracing step, skiped ";
						continue;
					}
					p.x = allPaths.at(i).back().x;
					p.y = allPaths.at(i).back().y;
					tempXadd = xadd;
					tempYadd = yadd;
					getMoveXYadd(tempXadd, tempYadd, p);
					//cout << "check if new move is allowed" << xadd << " " << tempXadd << " , " << yadd << " " << tempYadd << endl;
					if(tempXadd == xadd && tempYadd == yadd) // only make moves that are allowed
					{
						//cout << "move is allowed, creating new path to add to array" << endl;
						tempPath = allPaths.at(i);
						tempStep.dir.xadd = xadd;
						tempStep.dir.yadd = yadd;
						tempStep.x = tempPath.back().x + xadd;
						tempStep.y = tempPath.back().y + yadd;
						tempPath.push_back(tempStep);
						newPaths.push_back(tempPath);
					}
				}
			}
		}
		return newPaths;
	}
	
	int indexOfpositionInPath(int x, int y, path p) //returns the INDEX where it is found
	{
		for(int i = 0; i < p.size(); i++)
		{
			if(p.at(i).x == x && p.at(i).y == y)
				return i;
		}
		return -1;
	}

	bool positionExistsInAnyPath(int x, int y, vector<path> allPaths)
	{	for(int i = 0; i < allPaths.size(); i++)
			for(int j = 0; j < allPaths.at(i).size(); j++)
			{
				if(allPaths.at(i).at(j).x == x && allPaths.at(i).at(j).y == y)
					return true;
			}
			return false;
	}


	void printAllPaths(vector<path> allPaths)
	{
		if(allPaths.size() == 0)
		{
			cout << "No possible paths";
		}
		for(int i = 0; i < allPaths.size(); i++)
		{
			for(int j = 0; j < allPaths.at(i).size(); j++)
			{
				cout << right << setw(3) << allPaths.at(i).at(j).x << "," << left << setw(3) <<  allPaths.at(i).at(j).y;
			}
			cout << endl;
			for(int j = 0; j < allPaths.at(i).size(); j++)
			{
				cout << right << setw(3) << allPaths.at(i).at(j).dir.xadd << "," << left << setw(3) << allPaths.at(i).at(j).dir.yadd;
			}
			cout << endl << endl;
		}
	}

	void printAllPathsOverBoard(vector<path> allPaths)
	{
		int tempIndex;
		for(int i = 0; i < allPaths.size(); i++)
		{
			cout << endl;
			for(int y = 0; y < boardState.size(); y++)
			{
				for(int x = 0; x < boardState.at(y).size(); x++)
				{
					tempIndex = indexOfpositionInPath(x,y, allPaths.at(i));
					if(tempIndex != -1)
					{
						if(tempIndex == 0  || tempIndex == allPaths.at(i).size() - 1)
						{
							cout << ".";
						} else if(allPaths.at(i).at(tempIndex).dir.xadd == -1 && allPaths.at(i).at(tempIndex).dir.yadd == -1
						|| allPaths.at(i).at(tempIndex).dir.xadd == 1 && allPaths.at(i).at(tempIndex).dir.yadd == 1)
						{
							cout << "\\";
						} else if (allPaths.at(i).at(tempIndex).dir.xadd == 0 && allPaths.at(i).at(tempIndex).dir.yadd == -1
							|| allPaths.at(i).at(tempIndex).dir.xadd == 0 && allPaths.at(i).at(tempIndex).dir.yadd == 1)
						{
							cout << "|";
						} else if (allPaths.at(i).at(tempIndex).dir.xadd == 1 && allPaths.at(i).at(tempIndex).dir.yadd == -1
							|| allPaths.at(i).at(tempIndex).dir.xadd == -1 && allPaths.at(i).at(tempIndex).dir.yadd == 1)
						{
							cout << "/";
						} else if (allPaths.at(i).at(tempIndex).dir.xadd == -1 && allPaths.at(i).at(tempIndex).dir.yadd == 0
							|| allPaths.at(i).at(tempIndex).dir.xadd == 1 && allPaths.at(i).at(tempIndex).dir.yadd == 0)
						{
							cout << "_";
						}
					} else {
						cout << (char)(boardState.at(y).at(x));
					}
				}
				cout << endl;
			}
		}

	}

	enum levelState {ongoing, win, lose};

	void movePlayer(int xadd, int yadd)
	{
		movePersonProper(xadd,yadd,playerPos);
	}
	void moveEnemie(person &p) 
	{
		int xadd;
		int yadd;
		
		getEnemyXYaddRAW(xadd, yadd, p);

		movePersonProper(xadd,yadd,p);
	}

	void getEnemyXYaddRAW(int &xadd, int &yadd, person p) 
	{
		xadd = getEnemyXaddRAW(p);
		
		yadd = getEnemyYaddRAW(p);	
	}
	int getEnemyXaddRAW(person p) 
	{
		int xadd;
		if(playerPos.x > p.x)
		{
			xadd = 1;
		} else if (playerPos.x < p.x)
		{
			xadd = -1;
		} else
		{
			xadd = 0;
		}
		return xadd;
	}
	
	int getEnemyYaddRAW(person p)
	{
		int yadd;
		if(playerPos.y > p.y)
		{
			yadd = 1;
		} else if (playerPos.y < p.y)
		{
			yadd = -1;
		} else
		{
			yadd = 0;
		}
		return yadd;
	}

	void moveEnemiesAll_Anhalte() 
	{
		for(int i = 0; i < enemies.size(); i++)
		{
			 moveEnemie(enemies.at(i));
		}
		evaluateEnemieMoves();
	}
	
	void moveEnemiesAll_Colide()
	{
		int numberTrue = 0;
		int loopCount = 0;
		int tempXaxis;
		int tempYaxis;
		vector<bool> isMoved(enemies.size(), false);
		//while(numberTrue != enemies.size() && loopCount < enemies.size())
		while(loopCount < enemies.size())		
		{
			for(int i = 0; i < enemies.size(); i++)
			{
				if(isMoved.at(i) == true)
				{
					continue;
				}
				getEnemyMoveXYaxis(tempXaxis,tempYaxis,enemies.at(i));
				//getMoveXYaxis(getEnemyXaddRAW(enemies.at(i)),getEnemyYaddRAW(enemies.at(i)),tempXaxis,tempYaxis,enemies.at(i));
				if(boardState.at(tempYaxis).at(tempXaxis) != enemy)
				{
					movePersonToXY(tempXaxis,tempYaxis, enemies.at(i));
					//moveEnemie(enemies.at(i));
					isMoved.at(i) = true;
				}
			}
			loopCount++;
		}
		evaluateEnemieMoves();
	}

	void evaluateEnemieMoves()
	{
		for(int i = 0; i < enemies.size(); i++)
		{
			if(enemies.at(i).underPeice == enemy)
			{
				enemies.erase(enemies.begin()+i);
				i--;
			} else if (enemies.at(i).underPeice == pit) {
				boardState.at(enemies.at(i).y).at(enemies.at(i).x) = pit;
				enemies.erase(enemies.begin()+i);
				i--;
			}
		}

	}
	
	void getMoveXYaxis(int &xadd, int &yadd, int &xaxis, int &yaxis, person p){
		getMoveXYadd(xadd, yadd, p);
		xaxis = p.x + xadd;
		yaxis = p.y + yadd;
	}

	void getEnemyMoveXYaxis(int &xaxis, int &yaxis, person p)
	{
		getMoveXYaxis(getEnemyXaddRAW(p),getEnemyYaddRAW(p), xaxis, yaxis, p);//--------------------------------------------------------------------------------------
	}

	void movePersonProper(int xadd, int yadd, person &p)//**************************************
	{
		getMoveXYadd(xadd, yadd, p);
		movePersonExact(xadd, yadd, p);
	}
	void movePersonExact(int xadd, int yadd, person &p)//***********************************
	{
		boardState.at(p.y).at(p.x) = p.underPeice;
		p.x = p.x + xadd;
		p.y = p.y + yadd;
		p.underPeice = boardState.at(p.y).at(p.x);
		boardState.at(p.y).at(p.x) = p.type;
	
	}
	void movePersonToXY(int xaxis, int yaxis, person &p)
	{
		boardState.at(p.y).at(p.x) = p.underPeice;
		p.x = xaxis;
		p.y = yaxis;
		p.underPeice = boardState.at(p.y).at(p.x);
		boardState.at(p.y).at(p.x) = p.type;
	}
	void getMoveXYaxis(int xadd, int yadd, int &xaxis, int &yaxis, person p)
	{
		getMoveXYadd(xadd, yadd, p);
		xaxis = p.x + xadd;
		yaxis = p.y + yadd;
	}

	int getMoveXaxis(int xadd, int yadd, person p)
	{
		return p.x + getMoveXadd(xadd, yadd, p);
	}

	int getMoveYaxis(int xadd, int yadd, person p)
	{
		return p.y + getMoveYadd(xadd, yadd, p);
	}

	
	void getMoveXYadd(int &xadd, int &yadd, person p) //get the modefied xadd and yadd after wall collisions have been checked **********************************
	{
			//Check for out of board
		if (p.y + yadd < 0 || p.y + yadd > boardState.size() - 1)
		{
			yadd = 0; //y move is illegal
		}
		if (p.x + xadd < 0 || p.x + xadd > boardState.at(0).size() - 1)
		{
			xadd = 0; //x move is illegal
		}
		
		//Check for walls
		if((boardState.at(p.y + yadd).at(p.x + xadd) != wall)		// This move is allowed
		   && (boardState.at(p.y).at(p.x + xadd) != wall)		
		   && (boardState.at(p.y + yadd).at(p.x) != wall) )		
		{
			
		} else if ((boardState.at(p.y).at(p.x + xadd) == wall)		// This move is not allowed
		   && (boardState.at(p.y + yadd).at(p.x) == wall) 
		   || (boardState.at(p.y + yadd).at(p.x + xadd) == wall)
		   && (boardState.at(p.y).at(p.x + xadd) != wall)
		   && (boardState.at(p.y + yadd).at(p.x) != wall))

		{
			yadd = 0;
			xadd = 0;
		} else if ((boardState.at(p.y).at(p.x + xadd) != wall))		// ONLY x portion of the move is allowd
		{
			yadd = 0;
		} else if ((boardState.at(p.y + yadd).at(p.x) != wall))		// ONLY y portion of the move is allowd
		{
			xadd = 0;
		}	
	}

	int getMoveXadd(int xadd, int yadd, person p)
	{
		getMoveXYadd(xadd, yadd, p);
		return xadd;
	}

	int getMoveYadd(int xadd, int yadd, person p)
	{
		getMoveXYadd(xadd, yadd, p);
		return yadd;
	}

	//todo
	bool drawBoardView(){ 
		int sHight;
		int sWidth;
		sHight = playerPos.y - view.length/2;
		sWidth = playerPos.x - view.width/2;
		
		if(sHight < 0){
			sHight = 0;
		} else if (sHight + view.length > boardState.size()) {
			sHight = boardState.size() - view.length;
		}

		if(sWidth < 0){
			sWidth = 0;
		} else if (sWidth + view.width > boardState.at(0).size()) {
			sWidth = boardState.at(0).size() - view.width;
		}


		for(int i = sHight; i < sHight + view.length; i++)
		{
			for(int j = sWidth; j < sWidth + view.width; j++)
			{
				cout << (char)(boardState.at(i).at(j));
			}
			cout << endl;
		}
		return true;
		
	}
	bool drawBoardFull(){ 

		for(int i = 0; i < boardState.size(); i++)
		{
			for(int j = 0; j < boardState.at(i).size(); j++)
			{
				cout << (char)(boardState.at(i).at(j));
			}
			cout << endl;
		}
		return true;
	
	}
	void drawMoveXY()
	{
		int tempXaxis,tempYaxis;
		for(int i = 0; i < enemies.size(); i++)
		{
			getEnemyMoveXYaxis(tempXaxis,tempYaxis,enemies.at(i));
			//getMoveXYaxis(getEnemyXaddRAW(enemies.at(i)),getEnemyYaddRAW(enemies.at(i)),tempXaxis,tempYaxis,enemies.at(i));
			cout << "future x axis: " << tempXaxis <<  "future y axis: " << tempYaxis << endl;
		}
	}

	void drawPlayerPos(){ 
		cout << "player:   X: " << playerPos.x << " Y: " << playerPos.y << " underpeice: " << (char)playerPos.underPeice << endl;
		
		for(int i = 0; i < enemies.size(); i++)
		{
			cout << "enemies:   X: " << enemies.at(i).x << " Y: " << enemies.at(i).y << " underpeice: " << (char)enemies.at(i).underPeice << endl;
		}
	}

	bool loadLevel(string level) { 
		string tempLine;
		ifstream levelFile;
		int lineNumber = 0;
		levelFile.open(level.c_str());
		if(levelFile.fail()){
			return false;
		}
		
		boardState.clear();

		levelFile >> view.width >> view.length;
		getline(levelFile, tempLine);

		while(!levelFile.eof())
		{
			getline(levelFile, tempLine);
			boardState.resize(boardState.size() + 1);
			boardState.at(boardState.size() - 1).resize(tempLine.size());
			for(int i = 0; i < tempLine.size(); i++)
			{
				boardState.at(lineNumber).at(i) = static_cast<peice>(tempLine.at(i));
				if(tempLine.at(i) == enemy)
				{
					enemies.resize(enemies.size() + 1);
					enemies.at(enemies.size() - 1).x = i;
					enemies.at(enemies.size() - 1).y = lineNumber;
					enemies.at(enemies.size() - 1).underPeice = floor;
					enemies.at(enemies.size() - 1).type = enemy;
				} else if(tempLine.at(i) == player)
				{
					playerPos.x = i;
					playerPos.y = lineNumber;
					playerPos.underPeice = floor;
					playerPos.type = player;
				} else if(tempLine.at(i) == end)
				{
					endPos.x = i;
					endPos.y = lineNumber;

				} else if(tempLine.at(i) == start)
				{
					startPos.x = i;
					startPos.y = lineNumber;
				}
			}
			lineNumber++;
		}
		return true;
	}
	
	
	levelState Status()
	{
		for(int i = 0; i < enemies.size(); i++)
		{
			if(enemies.at(i).underPeice == player || playerPos.underPeice == pit)
			{
				return lose;
			}
		}
		if (playerPos.underPeice == exit)
		{
			return win;
		}
		return ongoing;
	}
	//	check board state, win lose... that type of thing
};



int main(void){
	MH_LevelState level;
	char move;
	string fileName;
	int tempXaxis, tempYaxis;
	cout << "enter file name:";
	cin >> fileName;
	fileName = "/Users/chris/Documents/MummyHunt/" + fileName;
	level.loadLevel(fileName.c_str());
	cout << endl << "path length:" << level.getNumberSteps(-1,0,0,0);
	
	// This part of the code is used for gameplay.
	/*for(int i = 0; i < 10000; i++){
		level.drawBoardFull();
		cout << endl;
		level.drawBoardView();
		cin >> move;
		switch (move)
		{
			case 'w':
				level.movePlayer(0,-1);
				break;
			case 'a':
				level.movePlayer(-1,0);
				break;
			case 'x':
				level.movePlayer(0,1);
				break;
			case 'd':
				level.movePlayer(1,0);
				break;
			case 'q':
				level.movePlayer(-1,-1);
				break;
			case 'e':
				level.movePlayer(1,-1);
				break;
			case 'z':
				level.movePlayer(-1,1);
				break;
			case 'c':
				level.movePlayer(1,1);
				break;
		}
		level.drawMoveXY();
		level.moveEnemiesAll_Anhalte();
		switch (level.Status())
		{
			case MH_LevelState::win :
				cout << "YOU WIN" << endl;
				break;
			case MH_LevelState::lose :
				cout << "YOU LOSE" << endl;
				break;
			case MH_LevelState::ongoing :
				level.drawPlayerPos();
		}	
	}
	*/
}
