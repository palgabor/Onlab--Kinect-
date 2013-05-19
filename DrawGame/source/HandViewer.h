#pragma once

#include "Viewer.h"
#include "HandTracker.h"

class HandViewer: public Viewer
{
	private:
		HandTracker	handTracker;

	protected:
		HandViewer(xn::Context& p_context);

		virtual void DisplayPostDraw();
		virtual void DisplayGameStatus();
		virtual void DisplayDistanceFromSensor();
		
		virtual XnStatus InitOpenGL(int argc, char **argv);

	public:
		// Singleton
		static Viewer& CreateInstance(xn::Context& p_context);

		virtual XnStatus Init(int argc, char **argv);
		virtual XnStatus Run();	//Does not return if successful
};
