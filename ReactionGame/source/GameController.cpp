#include "GameController.h"

const unsigned THRESHOLD = 30;

GameController::GameController()
{
	numOfHands = 0;
	storedPoint.X = 300;
	storedPoint.Y = 300;
	storedPoint.Z = 0;
	
	badPoint.X = 1000;
	badPoint.Y = 800;
	badPoint.Z = 0;
	
	goodHits = 0;
	badHits = 0;
	sumOfTime = 0;
	isBadPointAlive = false;
	badPointCounter = 0;
}

GameController::~GameController()
{
	
}

void GameController::displayGameStatus(const unsigned hands)
{
	const int POINT_SIZE = 50;
	numOfHands = hands;
	
	if(numOfHands > 0)
	{
		glColor3f(0.0, 1.0, 0.0); //RGB green
	}
	else
	{
		glColor3f(1.0, 0.0, 0.0); //RGB red
		resetClock();
	}
	
	XnFloat coordinates[3];
	glPointSize(50);
	
	//Top left corner
	coordinates[0] = 0;
	coordinates[1] = 0;
	coordinates[2] = 0;
	glVertexPointer(3,GL_FLOAT,0,coordinates);
	glDrawArrays(GL_POINTS,0,1);
	
	//Top right corner
	coordinates[0] = 1280;
	coordinates[1] = 0;
	coordinates[2] = 0;
	glVertexPointer(3,GL_FLOAT,0,coordinates);
	glDrawArrays(GL_POINTS,0,1);
	
	//Bottom left corner
	coordinates[0] = 0;
	coordinates[1] = 1024;
	coordinates[2] = 0;
	glVertexPointer(3,GL_FLOAT,0,coordinates);
	glDrawArrays(GL_POINTS,0,1);
	
	//Bottom right corner
	coordinates[0] = 1280;
	coordinates[1] = 1024;
	coordinates[2] = 0;
	glVertexPointer(3,GL_FLOAT,0,coordinates);
	glDrawArrays(GL_POINTS,0,1);
}

void GameController::displayRandomPoint(XnPoint3D currentPoint)
{
	if(numOfHands == 0)
	{
		return;
	}
	
	if(isPointAdjacent(currentPoint))
	{
		createRandomPoint();
		goodHits++;
		setClock();
	}

	glColor3f(0.0, 0.0, 1.0); //RGB blue
	glPointSize(50);
	XnFloat coordinates[3];
	coordinates[0] = storedPoint.X;
	coordinates[1] = storedPoint.Y;
	coordinates[2] = storedPoint.Z;
	glVertexPointer(3,GL_FLOAT,0,coordinates);
	glDrawArrays(GL_POINTS,0,1);
}

void GameController::displayRandomBadPoint(XnPoint3D currentPoint)
{
	if(numOfHands == 0)
	{
		return;
	}
	
	if(badPointCounter > 100)
	{
		createRandomPoint(true);
		isBadPointAlive = false;
		badPointCounter = 0;
		return;
	}
	
	badPointCounter++;
	isBadPointAlive == true;
	
	if(isPointAdjacent(currentPoint,true))
	{
		createRandomPoint(true);
		isBadPointAlive == false;
		badPointCounter = 0;
		badHits++;
		return;
	}

	glColor3f(1.0, 0.0, 0.0); //RGB red
	glPointSize(50);
	XnFloat coordinates[3];
	coordinates[0] = badPoint.X;
	coordinates[1] = badPoint.Y;
	coordinates[2] = badPoint.Z;
	glVertexPointer(3,GL_FLOAT,0,coordinates);
	glDrawArrays(GL_POINTS,0,1);
}

void GameController::createRandomPoint(bool isBadPoint)
{
	srand(time(0));
	
	if(isBadPoint)
	{
		do
		{
			badPoint.X = rand() % 1080 + 100;
			badPoint.Y = rand() % 824 + 100;
		}while(badPoint.X == storedPoint.X && badPoint.Y == storedPoint.Y);
	}
	else
	{
		do
		{
			storedPoint.X = rand() % 1080 + 100;
			storedPoint.Y = rand() % 824 + 100;
		} while(badPoint.X == storedPoint.X && badPoint.Y == storedPoint.Y);
	}
}

bool GameController::isPointAdjacent(XnPoint3D currentPoint, bool isBadPoint)
{
	if(isBadPoint == true)
	{
		if( badPoint.X - THRESHOLD < currentPoint.X &&
			badPoint.X + THRESHOLD > currentPoint.X &&
			badPoint.Y - THRESHOLD < currentPoint.Y &&
			badPoint.Y + THRESHOLD > currentPoint.Y )
		{
			return true;
		}
	}
	else
	{
		if( storedPoint.X - THRESHOLD < currentPoint.X &&
			storedPoint.X + THRESHOLD > currentPoint.X &&
			storedPoint.Y - THRESHOLD < currentPoint.Y &&
			storedPoint.Y + THRESHOLD > currentPoint.Y )
		{
			return true;
		}
	}
	
	return false;		
}

void GameController::displayResult()
{
	std::stringstream ss;
	ss << goodHits;
	std::string str = "Good hits: " + ss.str();
	
	
	glColor3f(0.0, 1.0, 1.0); //RGB
	glRasterPos2f(40, 30);
	for(int i=0;i<str.length();i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,str[i]);
	}
	
	ss.clear();
	ss.str("");
	ss << badHits;
	str = "Bad hits: " + ss.str();
	
	
	glColor3f(0.0, 1.0, 1.0); //RGB
	glRasterPos2f(40, 60);
	for(int i=0;i<str.length();i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,str[i]);
	}
	
	ss.clear();
	ss.str("");
	ss << getClock();
	str = "Time: " + ss.str();
	
	glRasterPos2f(40, 90);
	for(int i=0;i<str.length();i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,str[i]);
	}
	
	ss.clear();
	ss.str("");
	ss << getAverageTime();
	str = "Average time: " + ss.str();
	
	glRasterPos2f(40, 120);
	for(int i=0;i<str.length();i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,str[i]);
	}
}

void GameController::setClock()
{
	sumOfTime += getClock();
	startTime = std::clock();
}

void GameController::resetClock()
{
	startTime = 0;
}

float GameController::getClock()
{
	if(startTime == 0)
	{
		return 0;
	}
	
	return ((float)std::clock() - startTime)/CLOCKS_PER_SEC;
}

float GameController::getAverageTime()
{
	if(goodHits != 0)
	{
		return sumOfTime/goodHits;
	}
	
	return 0;
}
