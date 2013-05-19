#include <XnOS.h>
#include <GL/glut.h>
#include <cmath>
#include <cassert>

#include "Viewer.h"



using namespace xn;


//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define MIN_NUM_CHUNKS(data_size, chunk_size)	((((data_size)-1) / (chunk_size) + 1))
#define MIN_CHUNKS_SIZE(data_size, chunk_size)	(MIN_NUM_CHUNKS(data_size, chunk_size) * (chunk_size))

//---------------------------------------------------------------------------
// Statics
//---------------------------------------------------------------------------
Viewer* Viewer::instance = NULL;


//---------------------------------------------------------------------------
// GLUT Hooks
//---------------------------------------------------------------------------
void Viewer::glutIdle (void)
{
	// Display the frame
	glutPostRedisplay();
}

void Viewer::glutDisplay (void)
{
	Instance().Display();
}

void Viewer::glutKeyboard (unsigned char key, int x, int y)
{
	Instance().OnKey(key, x, y);
}


//---------------------------------------------------------------------------
// Method Definitions
//---------------------------------------------------------------------------
Viewer::Viewer(xn::Context& p_context):texMap(NULL),texMapX(0),texMapY(0),displayState(DEFAULT_DISPLAY_MODE),context(p_context)
{

}

Viewer::~Viewer()
{
	delete[] texMap;
}

Viewer& Viewer::CreateInstance(xn::Context& p_context)
{
	assert(!instance);
	return *(instance = new Viewer(p_context));
}

void Viewer::DestroyInstance(Viewer& p_instance)
{
	assert(p_instance);
	assert(instance == &p_instance);
	delete instance;
	instance = NULL;
}

Viewer& Viewer::Instance()
{
	assert(instance);
	return *instance;
}

XnStatus Viewer::Init(int argc, char **argv)
{
	XnStatus rc;

	rc = context.FindExistingNode(XN_NODE_TYPE_DEPTH, depthGenerator);
	if (rc != XN_STATUS_OK)
	{
		printf("No depth node exists! Check your XML.");
		return rc;
	}

	rc = context.FindExistingNode(XN_NODE_TYPE_IMAGE, imageGenerator);
	if (rc != XN_STATUS_OK)
	{
		printf("No image node exists! Check your XML.");
		return rc;
	}

	depthGenerator.GetMetaData(depthMD);
	imageGenerator.GetMetaData(imageMD);

	if (imageMD.FullXRes() != depthMD.FullXRes() || imageMD.FullYRes() != depthMD.FullYRes())
	{
		printf ("The device depth and image resolution must be equal!\n");
		return 1;
	}

	// RGB is the only image format supported.
	if (imageMD.PixelFormat() != XN_PIXEL_FORMAT_RGB24)
	{
		printf("The device image format must be RGB24\n");
		return 1;
	}

	// Texture map init
	texMapX = MIN_CHUNKS_SIZE(depthMD.FullXRes(), TEXTURE_SIZE);
	texMapY = MIN_CHUNKS_SIZE(depthMD.FullYRes(), TEXTURE_SIZE);
	texMap = new XnRGB24Pixel[texMapX * texMapY];

	return InitOpenGL(argc, argv);
}

XnStatus Viewer::Run()
{
	// Per frame code is in Display
	glutMainLoop();	// Does not return!

	return XN_STATUS_OK;
}

XnStatus Viewer::InitOpenGL(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	glutCreateWindow (WINDOW_TITLE);
// 	glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);

	InitOpenGLHooks();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	return XN_STATUS_OK;
}

void Viewer::InitOpenGLHooks()
{
	glutKeyboardFunc(glutKeyboard);
	glutDisplayFunc(glutDisplay);
	glutIdleFunc(glutIdle);
}

void Viewer::Display()
{
	XnStatus		rc = XN_STATUS_OK;

	// Read a new frame
	rc = context.WaitAnyUpdateAll();
	if (rc != XN_STATUS_OK)
	{
		printf("Read failed: %s\n", xnGetStatusString(rc));
		return;
	}

	depthGenerator.GetMetaData(depthMD);
	imageGenerator.GetMetaData(imageMD);

	const XnDepthPixel* pDepth = depthMD.Data();
	const XnUInt8* pImage = imageMD.Data();

	unsigned int nImageScale = GL_WIN_SIZE_X / depthMD.FullXRes();

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup the OpenGL viewpoint
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, GL_WIN_SIZE_X, GL_WIN_SIZE_Y, 0, -1.0, 1.0);

	// Calculate the accumulative histogram (the yellow display...)
	xnOSMemSet(depthHist, 0, MAX_DEPTH*sizeof(float));

	unsigned int nNumberOfPoints = 0;
	for (XnUInt y = 0; y < depthMD.YRes(); ++y)
	{
		for (XnUInt x = 0; x < depthMD.XRes(); ++x, ++pDepth)
		{
			if (*pDepth != 0)
			{
				depthHist[*pDepth]++;
				nNumberOfPoints++;
			}
		}
	}
	for (int nIndex=1; nIndex<MAX_DEPTH; nIndex++)
	{
		depthHist[nIndex] += depthHist[nIndex-1];
	}
	if (nNumberOfPoints)
	{
		for (int nIndex=1; nIndex<MAX_DEPTH; nIndex++)
		{
			depthHist[nIndex] = (unsigned int)(256 * (1.0f - (depthHist[nIndex] / nNumberOfPoints)));
		}
	}

	xnOSMemSet(texMap, 0, texMapX*texMapY*sizeof(XnRGB24Pixel));

	// check if we need to draw image frame to texture
	if (displayState == DISPLAY_MODE_OVERLAY ||
		displayState == DISPLAY_MODE_IMAGE)
	{
		const XnRGB24Pixel* pImageRow = imageMD.RGB24Data();
		XnRGB24Pixel* pTexRow = texMap + imageMD.YOffset() * texMapX;

		for (XnUInt y = 0; y < imageMD.YRes(); ++y)
		{
			const XnRGB24Pixel* pImage = pImageRow;
			XnRGB24Pixel* pTex = pTexRow + imageMD.XOffset();

			for (XnUInt x = 0; x < imageMD.XRes(); ++x, ++pImage, ++pTex)
			{
				*pTex = *pImage;
			}

			pImageRow += imageMD.XRes();
			pTexRow += texMapX;
		}
	}

	// check if we need to draw depth frame to texture
	if (displayState == DISPLAY_MODE_OVERLAY ||
		displayState == DISPLAY_MODE_DEPTH)
	{
		const XnDepthPixel* pDepthRow = depthMD.Data();
		XnRGB24Pixel* pTexRow = texMap + depthMD.YOffset() * texMapX;

		for (XnUInt y = 0; y < depthMD.YRes(); ++y)
		{
			const XnDepthPixel* pDepth = pDepthRow;
			XnRGB24Pixel* pTex = pTexRow + depthMD.XOffset();

			for (XnUInt x = 0; x < depthMD.XRes(); ++x, ++pDepth, ++pTex)
			{
				if (*pDepth != 0)
				{
					int nHistValue = depthHist[*pDepth];
					pTex->nRed = nHistValue;
					pTex->nGreen = nHistValue;
					pTex->nBlue = nHistValue;
				}
			}

			pDepthRow += depthMD.XRes();
			pTexRow += texMapX;
		}
	}

	// Create the OpenGL texture map
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texMapX, texMapY, 0, GL_RGB, GL_UNSIGNED_BYTE, texMap);


	// Display the OpenGL texture map
	glColor4f(1,1,1,1);

	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	int nXRes = depthMD.FullXRes();
	int nYRes = depthMD.FullYRes();

	
	// upper left
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);
	// upper right
	glTexCoord2f((float)nXRes/(float)texMapX, 0);
	glVertex2f(GL_WIN_SIZE_X, 0);
	// bottom right
	glTexCoord2f((float)nXRes/(float)texMapX, (float)nYRes/(float)texMapY);
	glVertex2f(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	// bottom left
	glTexCoord2f(0, (float)nYRes/(float)texMapY);
	glVertex2f(0, GL_WIN_SIZE_Y);

	glEnd();
	glDisable(GL_TEXTURE_2D);		

	// Subclass draw hook
	DisplayPostDraw();

	// Displays game status (enabled or not)
	DisplayGameStatus();
	
	// Displays game status (enabled or not)
	DisplayDistanceFromSensor();
	
	// Swap the OpenGL display buffers
	glutSwapBuffers();
}

void Viewer::OnKey(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit (1);
	case '1':
		displayState = DISPLAY_MODE_OVERLAY;
		depthGenerator.GetAlternativeViewPointCap().SetViewPoint(imageGenerator);
		break;
	case '2':
		displayState = DISPLAY_MODE_DEPTH;
		depthGenerator.GetAlternativeViewPointCap().ResetViewPoint();
		break;
	case '3':
		displayState = DISPLAY_MODE_IMAGE;
		depthGenerator.GetAlternativeViewPointCap().ResetViewPoint();
		break;
	case 'm':
		context.SetGlobalMirror(!context.GetGlobalMirror());
		break;
	case 'q':
		device.Move(device.GetCurrentPosition()+1);
		device.SetLedColour(14);
		break;
	case 'a':
		device.Move(device.GetCurrentPosition()-1);
		break;
	}
}

void Viewer::ScalePoint(XnPoint3D& point)
{
	point.X *= GL_WIN_SIZE_X;
	point.X /= depthMD.XRes();

	point.Y *= GL_WIN_SIZE_Y;
	point.Y /= depthMD.YRes();
}
