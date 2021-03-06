#include <GL/glut.h>

#include "HandViewer.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define LENGTHOF(array)			(sizeof(array)/sizeof(array[0]))


//---------------------------------------------------------------------------
// Function Definitions
//---------------------------------------------------------------------------
Viewer& HandViewer::CreateInstance(xn::Context& p_context)
{
	assert(!instance);
	return *(instance = new HandViewer(p_context));
}

HandViewer::HandViewer(xn::Context& p_context):Viewer(p_context),handTracker(p_context)
{
	
}

XnStatus HandViewer::Init(int argc, char **argv)
{
	XnStatus rc;
	rc = Viewer::Init(argc, argv);
	if(rc != XN_STATUS_OK)
	{
		return rc;
	}

	return handTracker.Init();
}

XnStatus HandViewer::Run()
{
	XnStatus rc = handTracker.Run();

	if(rc != XN_STATUS_OK)
	{
		return rc;
	}

	return Viewer::Run(); // Does not return, enters OpenGL main loop instead
}

XnStatus HandViewer::InitOpenGL( int argc, char **argv )
{
	XnStatus rc = Viewer::InitOpenGL(argc, argv); 

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	return rc;
}

void HandViewer::DisplayPostDraw()
{
	const PointListHandler& handler = handTracker.GetPointListHandler();

	XnFloat	coordinates[3 * SIZE_OF_POINT_LIST];
	
	for(PointListHandler::ConstIterator it = handler.begin(); it != handler.end(); ++it)
	{
		int	numpoints = 0;
		const PointList &pointList = it.GetPointList();
		
		XnPoint3D point;
			
		for(PointList::ConstIterator it2 = pointList.begin(); it2 != pointList.end(); ++it2)
		{
			point = *it2;
			depthGenerator.ConvertRealWorldToProjective(1, &point, &point);
			ScalePoint(point);
			coordinates[numpoints * 3] = point.X;
			coordinates[numpoints * 3 + 1] = point.Y;
			if(point.Z < HAND_CLOSE)
				coordinates[numpoints * 3 + 2] = 0;
			else
				coordinates[numpoints * 3 + 2] = point.Z;

			++numpoints;
		}
		assert(numpoints <= Constants::SIZE_OF_POINT_LIST);

		//Draw all points
		XnUInt32 nColor = it.GetKey() % LENGTHOF(COLOURS);
		glColor4f(COLOURS[nColor][0], COLOURS[nColor][1], COLOURS[nColor][2], 1.0f);
		glPointSize(3);
		glVertexPointer(3, GL_FLOAT, 0, coordinates);
		glDrawArrays(GL_LINE_STRIP, 0, numpoints);
		
		//Draw current point as a larger dot
		glPointSize(10);
		glDrawArrays(GL_POINTS, 0, 1);

		glFlush();
	}
}

void HandViewer::DisplayGameStatus()
{
	const unsigned POINT_SIZE = 50;
	
	const PointListHandler& handler = handTracker.GetPointListHandler();
	unsigned numOfUsers = 0;
	
	for(PointListHandler::ConstIterator it = handler.begin(); it != handler.end(); ++it)
		numOfUsers++;
		
	if(numOfUsers > 0)
	{
		glColor3f(COLOURS[GREEN][0], COLOURS[GREEN][1], COLOURS[GREEN][2]);
		device.SetLedColour(LED_GREEN);
	}
	else
	{
		glColor3f(COLOURS[RED][0], COLOURS[RED][1], COLOURS[RED][2]);
		device.SetLedColour(LED_RED);
	}
	
	XnFloat coordinates[3];
	glPointSize(POINT_SIZE);
	
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

void HandViewer::DisplayDistanceFromSensor()
{
	std::stringstream ss;
	unsigned counter = 1;

	const PointListHandler& handler = handTracker.GetPointListHandler();

	for(PointListHandler::ConstIterator it = handler.begin(); it != handler.end(); ++it)
	{
		const PointList &pointList = it.GetPointList();
		XnPoint3D lastPoint = pointList.GetLastPoint();
		
		std::string str = "Hand ";
		ss << counter;
		str = str + ss.str();
		ss.str("");
		
		ss << " distance from the sensor: ";
		str = str + ss.str();
		ss.str("");
		
		if(lastPoint.Z < HAND_CLOSE)
		{
			ss << 0;
			device.SetLedColour(LED_GREEN);
		}
		else
		{
			ss << lastPoint.Z - HAND_CLOSE;
			device.SetLedColour(LED_ORANGE);	
		}
		str = str + ss.str();
		ss.str("");
	
		XnUInt32 nColor = it.GetKey() % LENGTHOF(COLOURS);
		glColor4f(COLOURS[nColor][0], COLOURS[nColor][1], COLOURS[nColor][2], 1.0f);
		glRasterPos2f(40, counter*30);
		for(int i=0;i<str.length();i++)
		{
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,str[i]);
		}
		
		counter++;
	}
	

}
