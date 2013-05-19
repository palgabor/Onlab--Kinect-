#pragma once

#include <XnCppWrapper.h>

#include "Constants.h"
#include "PointListHandler.h"

class Viewer
{
	private:
		// GLUT callbacks
		static void glutIdle();
		static void glutDisplay();
		static void glutKeyboard(unsigned char key, int x, int y);

		float				depthHist[MAX_DEPTH];
		XnRGB24Pixel*		texMap;
		unsigned int		texMapX;
		unsigned int		texMapY;
		DisplayModes_e	displayState;
		xn::DepthMetaData	depthMD;
		xn::ImageMetaData	imageMD;
		
	protected:
		Viewer(xn::Context& p_context);
		virtual ~Viewer();

		virtual void Display();
		virtual void DisplayPostDraw(){};	// Overload to draw over the screen image
		virtual void DisplayGameStatus(){};	// Overload to draw over the screen image

		virtual void OnKey(unsigned char key, int x, int y);

		virtual XnStatus InitOpenGL(int argc, char **argv);
		void InitOpenGLHooks();

		static Viewer& Instance();

		xn::Context&		context;
		xn::DepthGenerator	depthGenerator;
		xn::ImageGenerator	imageGenerator;

		static Viewer*		instance;
		
		float currentDisplayColour[3];

		void ScalePoint(XnPoint3D& point);
	
	public:
		// Singleton
		static Viewer& CreateInstance(xn::Context& p_context);
		static void DestroyInstance(Viewer& p_instance);

		virtual XnStatus Init(int argc, char **argv);
		virtual XnStatus Run();	//Does not return
};
