#pragma once

#include <XnCppWrapper.h>
#include <GL/glut.h>

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>

class GameController
{
	private:
		void createRandomPoint(bool isBadPoint=false);
		bool isPointAdjacent(XnPoint3D currentPoint, bool isBadPoint=false);
		
		void setClock();
		void resetClock();
		float getClock();
		float getAverageTime();
		
		unsigned numOfHands;
		XnPoint3D storedPoint;
		XnPoint3D badPoint;
		unsigned goodHits;
		unsigned badHits;
		bool isBadPointAlive;
		unsigned badPointCounter;
		bool isCounterEnabledGoodPoint;
		bool isCounterEnabledBadPoint;
		
		std::clock_t startTime;
		float sumOfTime;
		bool isGameStopped;
				
	public:
		GameController();
		~GameController();
		
		void displayGameStatus(const unsigned hands);
		void displayRandomPoint(XnPoint3D point);
		void displayRandomBadPoint(XnPoint3D point);
		void displayResult();
		
};
