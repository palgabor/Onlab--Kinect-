//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "NiHandViewer.h"
#include <GL/glut.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define LENGTHOF(array)			(sizeof(array)/sizeof(array[0]))


//---------------------------------------------------------------------------
// Function Definitions
//---------------------------------------------------------------------------
SimpleViewer& HandViewer::CreateInstance(xn::Context& context)
{
	assert(!sm_pInstance);
	return *(sm_pInstance = new HandViewer(context));
}

HandViewer::HandViewer(xn::Context& context):SimpleViewer(context),m_HandTracker(context)
{
	
}

XnStatus HandViewer::Init(int argc, char **argv)
{
	XnStatus rc;
	rc = SimpleViewer::Init(argc, argv);
	if(rc != XN_STATUS_OK)
	{
		return rc;
	}

	return m_HandTracker.Init();
}

XnStatus HandViewer::Run()
{
	XnStatus rc = m_HandTracker.Run();

	if(rc != XN_STATUS_OK)
	{
		return rc;
	}

	return SimpleViewer::Run(); // Does not return, enters OpenGL main loop instead
}

const XnPoint3D HandViewer::DisplayPostDraw()
{
	typedef TrailHistory			History;
	typedef History::ConstIterator	HistoryIterator;
	typedef History::Trail			Trail;
	typedef Trail::ConstIterator	TrailIterator;

	static const float colours[][3] =
	{
		//{ 0.5f, 0.5f, 0.5f},
		{ 0.0f, 1.0f, 0.0f}//,
		//{ 0.0f, 0.5f, 1.0f},
		//{ 1.0f, 1.0f, 0.0f},
		//{ 1.0f, 0.5f, 0.0f},
		//{ 1.0f, 0.0f, 1.0f}
	};
	const TrailHistory&	history = m_HandTracker.GetHistory();

	// History points coordinates buffer
	XnFloat	coordinates[3 * MAX_HAND_TRAIL_LENGTH];

	const HistoryIterator	hend = history.end();
	for(HistoryIterator		hit = history.begin(); hit != hend; ++hit)
	{

		// Dump the history to local buffer
		int				numpoints = 0;
		const Trail&	trail = hit.GetTrail();
		const TrailIterator	tend = trail.end();
		
		XnPoint3D point;
		
		for(TrailIterator	tit = trail.begin(); tit != tend; ++tit)
		{
			point = *tit;
			m_depth.ConvertRealWorldToProjective(1, &point, &point);
			ScalePoint(point);
			coordinates[numpoints * 3] = point.X;
			coordinates[numpoints * 3 + 1] = point.Y;
			coordinates[numpoints * 3 + 2] = 0;

			++numpoints;
		}
		assert(numpoints <= MAX_HAND_TRAIL_LENGTH);

		// Draw the hand trail history
		XnUInt32 nColor = hit.GetKey() % LENGTHOF(colours);
		glColor4f(colours[nColor][0],
			colours[nColor][1],
			colours[nColor][2],
			1.0f);
		glPointSize(3);
		glVertexPointer(3, GL_FLOAT, 0, coordinates);
		glDrawArrays(GL_LINE_STRIP, 0, numpoints);
		
		// Current point as a larger dot
		glPointSize(10);
		glDrawArrays(GL_POINTS, 0, 1);
		glFlush();
		
		point.X = coordinates[0];
		point.Y = coordinates[1];
		point.Z = coordinates[2];
		return point;
	}
}

unsigned HandViewer::getNumOfHands()
{
	return m_HandTracker.getNumOfHands();
}


XnStatus HandViewer::InitOpenGL( int argc, char **argv )
{
	XnStatus rc = SimpleViewer::InitOpenGL(argc, argv); 

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	return rc;
}
