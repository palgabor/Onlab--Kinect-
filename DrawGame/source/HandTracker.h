#pragma once

#include <XnCppWrapper.h>

#include "PointListHandler.h"
#include "Constants.h"


class HandTracker
{
	private:
		// OpenNI Gesture and Hands Generator callbacks
		static void XN_CALLBACK_TYPE Gesture_Recognized(xn::GestureGenerator&	generator, 
														const XnChar*			strGesture, 
														const XnPoint3D*		pIDPosition, 
														const XnPoint3D*		pEndPosition, 
														void*					pCookie);
		static void XN_CALLBACK_TYPE Gesture_Process(	xn::GestureGenerator&	generator, 
														const XnChar*			strGesture, 
														const XnPoint3D*		pPosition, 
														XnFloat					fProgress, 
														void*					pCookie)	{}
		static void XN_CALLBACK_TYPE Hand_Create(	xn::HandsGenerator& generator, 
													XnUserID			nId, 
													const XnPoint3D*	pPosition, 
													XnFloat				fTime, 
													void*				pCookie);
		static void XN_CALLBACK_TYPE Hand_Update(	xn::HandsGenerator& generator, 
													XnUserID			nId, 
													const XnPoint3D*	pPosition, 
													XnFloat				fTime, 
													void*				pCookie);
		static void XN_CALLBACK_TYPE Hand_Destroy(	xn::HandsGenerator& generator, 
													XnUserID			nId, 
													XnFloat				fTime, 
													void*				pCookie);

		xn::Context&			context;
		PointListHandler		pointListHandler;
		xn::GestureGenerator	gestureGenerator;
		xn::HandsGenerator		handsGenerator;

		static XnList instances; // Living instances of the class

	
	public:
		HandTracker(xn::Context& context);
		~HandTracker();

		XnStatus Init();
		XnStatus Run();

		const PointListHandler& GetPointListHandler() const;
};
