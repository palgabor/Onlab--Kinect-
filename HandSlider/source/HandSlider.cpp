#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include <XnVHandPointContext.h>
#include <XnVSessionManager.h>
#include <XnVFlowRouter.h>
#include <XnVSwipeDetector.h>
#include <XnVSelectableSlider1D.h>
#include <XnVSteadyDetector.h>
#include <XnVBroadcaster.h>
#include <XnVPushDetector.h>

#include <GL/glut.h>

#include "signal_catch.h"

#include <iostream>


#define GL_WIN_SIZE_X 1100
#define GL_WIN_SIZE_Y 200

#define SAMPLE_XML_PATH "Config.xml"

#define CHECK_RC(rc, what)											\
	if (rc != XN_STATUS_OK)											\
{																\
	printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
	return rc;													\
}

#define CHECK_ERRORS(rc, errors, what)		\
	if (rc == XN_STATUS_NO_NODE_PRESENT)	\
{										\
	XnChar strError[1024];				\
	errors.ToString(strError, 1024);	\
	printf("%s\n", strError);			\
	return (rc);						\
}

XnVSessionManager* g_pSessionManager = NULL;
XnVFlowRouter* g_pMainFlowRouter;

XnVSelectableSlider1D* g_pMainSlider;

XnBool g_bActive = false;
XnBool g_bIsInput = false;
XnBool g_bInSession = false;

XnFloat g_fValue = 0.5f;

xn::Context g_Context;
xn::ScriptNode g_ScriptNode;
xn::DepthGenerator g_DepthGenerator;

XnBool g_bQuit = false;



// Drawing functions
void DrawLine(XnFloat fMinX, XnFloat fMinY, XnFloat fMinZ,
			  XnFloat fMaxX, XnFloat fMaxY, XnFloat fMaxZ,
			  int width, double r = 1, double g = 1, double b = 1)
{
	glLineWidth(width);
	glBegin(GL_LINES);
	glColor3f(r, g, b);
	glVertex3f(fMinX, fMinY, fMinZ);
	glVertex3f(fMaxX, fMaxY, fMaxZ);
	glEnd();
}

void DrawFrame(const XnPoint3D& ptMins, const XnPoint3D& ptMaxs, int width, double r, double g, double b)
{
	XnPoint3D ptTopLeft = ptMins;
	XnPoint3D ptBottomRight = ptMaxs;

	// Top line
	DrawLine(ptTopLeft.X, ptTopLeft.Y, 0,
		ptBottomRight.X, ptTopLeft.Y, 0,
		width, r, g, b);
	// Right Line
	DrawLine(ptBottomRight.X, ptTopLeft.Y, 0,
		ptBottomRight.X, ptBottomRight.Y,0,
		width, r, g, b);
	// Bottom Line
	DrawLine(ptBottomRight.X, ptBottomRight.Y,0,
		ptTopLeft.X, ptBottomRight.Y,0,
		width, r, g, b);
	// Left Line
	DrawLine(ptTopLeft.X, ptBottomRight.Y,0,
		ptTopLeft.X, ptTopLeft.Y,0,
		width, r, g, b);

}

void DrawSlider()
{
	if (!g_bInSession)
		return;

	XnPoint3D ptMin = xnCreatePoint3D(50, GL_WIN_SIZE_Y-50, 0);
	XnPoint3D ptMax = xnCreatePoint3D(1050, GL_WIN_SIZE_Y-150, 0);

	XnDouble r, g, b;
	XnBool bDrawCursor = false;

	if (!g_bActive)
	{
		r = g = b = 1;
	}
	else if (!g_bIsInput)
	{
		r = g = b = 0.5;
	}
	else
	{
		r = b = 0;
		g = 1;
		bDrawCursor = true;
	}

	DrawFrame(ptMin, ptMax, 20, r, g, b);

	if (bDrawCursor)
	{
		DrawLine(1000*g_fValue+50, GL_WIN_SIZE_Y-50, 0,
				1000*g_fValue+50, GL_WIN_SIZE_Y-150, 0,
				30, 1, 0, 0);
	}

}

////////////////////////
// XnVDirectionAsString(XnVDirection dir)
/////////////////////////

// Slider callbacks
void XN_CALLBACK_TYPE MainSlider_OnHover(XnInt32 nItem, void* cxt)
{

}

void XN_CALLBACK_TYPE MainSlider_OnSelect(XnInt32 nItem, XnVDirection dir, void* cxt)
{
	
}

void XN_CALLBACK_TYPE MainSlider_OnValueChange(XnFloat fValue, void* cxt)
{
	g_bActive = true;
	g_bIsInput = true;
	g_fValue = fValue;
	
	std::cout<<"Value: "<<fValue<<std::endl;
}

void XN_CALLBACK_TYPE MainSlider_OnActivate(void* cxt)
{
	g_bActive = true;
	g_bIsInput = false;
}

void XN_CALLBACK_TYPE MainSlider_OnDeactivate(void* cxt)
{
	g_bActive = false;
	g_bIsInput = false;
}

void XN_CALLBACK_TYPE MainSlider_OnPrimaryCreate(const XnVHandPointContext* hand, const XnPoint3D& ptFocus, void* cxt)
{
	g_bIsInput = true;
}

void XN_CALLBACK_TYPE MainSlider_OnPrimaryDestroy(XnUInt32 nID, void* cxt)
{
	g_bIsInput = false;
}

void XN_CALLBACK_TYPE SessionStart(const XnPoint3D& pFocus, void* UserCxt)
{
	g_bInSession = true;
	g_pMainFlowRouter->SetActive(g_pMainSlider);
}
void XN_CALLBACK_TYPE SessionEnd(void* UserCxt)
{
	g_bInSession = false;
	g_pMainFlowRouter->SetActive(NULL);
}


void CleanupExit()
{
	if (NULL != g_pSessionManager) {
		delete g_pSessionManager;
		g_pSessionManager = NULL;
	}

	delete g_pMainFlowRouter;
	delete g_pMainSlider;

	exit (1);
}

// this function is called each frame
void glutDisplay (void)
{
	// Read next available data
	g_Context.WaitOneUpdateAll(g_DepthGenerator);

	// Process the data
	g_pSessionManager->Update(&g_Context);

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup the OpenGL viewpoint
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, GL_WIN_SIZE_X, 0, GL_WIN_SIZE_Y, -1.0, 1.0);

	glDisable(GL_TEXTURE_2D);

	// Draw the slider
	DrawSlider();

	glutSwapBuffers();
}

void glutIdle (void)
{
	if (g_bQuit) {
		CleanupExit();
	}

	// Display the frame
	glutPostRedisplay();
}
void glutKeyboard (unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		CleanupExit();
	}
}
void glInit (int * pargc, char ** argv)
{
	glutInit(pargc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	glutCreateWindow ("HandSlider");
	glutSetCursor(GLUT_CURSOR_NONE);

	glutKeyboardFunc(glutKeyboard);
	glutDisplayFunc(glutDisplay);
	glutIdleFunc(glutIdle);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
}


int main(int argc, char **argv)
{
	XnStatus rc = XN_STATUS_OK;
	xn::EnumerationErrors errors;

	rc = g_Context.InitFromXmlFile(SAMPLE_XML_PATH, g_ScriptNode, &errors);
	CHECK_ERRORS(rc, errors, "InitFromXmlFile");
	CHECK_RC(rc, "InitFromXml");

	rc = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
	CHECK_RC(rc, "Find depth generator");

	// Create and initialize point tracker
	g_pSessionManager = new XnVSessionManager;
	rc = g_pSessionManager->Initialize(&g_Context, "Wave", "RaiseHand");
	if (rc != XN_STATUS_OK)
	{
		printf("Couldn't initialize the Session Manager: %s\n", xnGetStatusString(rc));
		delete g_pSessionManager;
		return rc;
	}

	g_pSessionManager->RegisterSession(NULL, &SessionStart, &SessionEnd);

	// Start catching signals for quit indications
	CatchSignals(&g_bQuit);

	// Create and initialize the main slider
	g_pMainSlider = new XnVSelectableSlider1D(3);
	g_pMainSlider->RegisterItemHover(NULL, &MainSlider_OnHover);
	g_pMainSlider->RegisterItemSelect(NULL, &MainSlider_OnSelect);
	g_pMainSlider->RegisterActivate(NULL, &MainSlider_OnActivate);
	g_pMainSlider->RegisterDeactivate(NULL, &MainSlider_OnDeactivate);
	g_pMainSlider->RegisterPrimaryPointCreate(NULL, &MainSlider_OnPrimaryCreate);
	g_pMainSlider->RegisterPrimaryPointDestroy(NULL, &MainSlider_OnPrimaryDestroy);
	g_pMainSlider->RegisterValueChange(NULL, &MainSlider_OnValueChange);
	g_pMainSlider->SetValueChangeOnOffAxis(true);

	// Creat the flow manager
	g_pMainFlowRouter = new XnVFlowRouter;

	// Connect flow manager to the point tracker
	g_pSessionManager->AddListener(g_pMainFlowRouter);

	g_Context.StartGeneratingAll();

	glInit(&argc, argv);
	glutMainLoop();
}
