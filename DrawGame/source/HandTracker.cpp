#include "HandTracker.h"
#include <cassert>


using namespace xn;


//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define LENGTHOF(arr)			(sizeof(arr)/sizeof(arr[0]))
#define FOR_ALL(arr, action)	{for(int i = 0; i < LENGTHOF(arr); ++i){action(arr[i])}}

#define ADD_GESTURE(name)		{if(gestureGenerator.AddGesture(name, NULL) != XN_STATUS_OK){printf("Unable to add gesture"); exit(1);}}
#define ADD_ALL_GESTURES		FOR_ALL(cGestures, ADD_GESTURE)


//---------------------------------------------------------------------------
// Consts
//---------------------------------------------------------------------------
// Gestures to track
static const char			cClickStr[] = "Click";
static const char			cWaveStr[] = "Wave";
static const char* const	cGestures[] =
{
	cClickStr,
	cWaveStr
};

//---------------------------------------------------------------------------
// Statics
//---------------------------------------------------------------------------
XnList HandTracker::instances;

//---------------------------------------------------------------------------
// Hooks
//---------------------------------------------------------------------------
void XN_CALLBACK_TYPE HandTracker::Gesture_Recognized(	xn::GestureGenerator&	generator, 
														const XnChar*			strGesture, 
														const XnPoint3D*		pIDPosition, 
														const XnPoint3D*		pEndPosition, 
														void*					pCookie)
{
	printf("Gesture recognized: %s\n", strGesture);

	HandTracker*	pThis = static_cast<HandTracker*>(pCookie);
	if(instances.Find(pThis) == instances.end())
	{
		printf("Dead HandTracker: skipped!\n");
		return;
	}

	pThis->handsGenerator.StartTracking(*pEndPosition);
}

void XN_CALLBACK_TYPE HandTracker::Hand_Create(	xn::HandsGenerator& generator, 
												XnUserID			nId, 
												const XnPoint3D*	pPosition, 
												XnFloat				fTime, 
												void*				pCookie)
{
	printf("New Hand: %d @ (%f,%f,%f)\n", nId, pPosition->X, pPosition->Y, pPosition->Z);

	HandTracker*	pThis = static_cast<HandTracker*>(pCookie);
	if(instances.Find(pThis) == instances.end())
	{
		printf("Dead HandTracker: skipped!\n");
		return;
	}

	// Add to existing users if this user is not already tracked
	if(!pThis->pointListHandler.Find(nId))
	{
		pThis->pointListHandler.Add(nId).Push(*pPosition);
	}
}

void XN_CALLBACK_TYPE HandTracker::Hand_Update(	xn::HandsGenerator& generator, 
												XnUserID			nId, 
												const XnPoint3D*	pPosition, 
												XnFloat				fTime, 
												void*				pCookie)
{
	HandTracker*	pThis = static_cast<HandTracker*>(pCookie);
	if(instances.Find(pThis) == instances.end())
	{
		printf("Dead HandTracker: skipped!\n");
		return;
	}

	// Add to this user's hands history
	PointList* const pointList = pThis->pointListHandler.Find(nId);
	if(!pointList)
	{
		printf("Dead hand update: skipped!\n");
		return;
	}
	pointList->Push(*pPosition);
}

void XN_CALLBACK_TYPE HandTracker::Hand_Destroy(	xn::HandsGenerator& generator, 
													XnUserID			nId, 
													XnFloat				fTime, 
													void*				pCookie)
{
	printf("Lost Hand: %d\n", nId);

	HandTracker*	pThis = static_cast<HandTracker*>(pCookie);
	if(instances.Find(pThis) == instances.end())
	{
		printf("Dead HandTracker: skipped!\n");
		return;
	}

	// Remove this user from PointListHandler
	pThis->pointListHandler.Remove(nId);
}


//---------------------------------------------------------------------------
// Method Definitions
//---------------------------------------------------------------------------
HandTracker::HandTracker(xn::Context& p_context):context(p_context),pointListHandler(SIZE_OF_POINT_LIST)
{
	XnStatus rc = instances.AddLast(this);
	if (rc != XN_STATUS_OK)
	{
		printf("Unable to add NiHandTracker instance to the list.");
		exit(1);
	}
	
}

HandTracker::~HandTracker()
{
	// Remove the current instance from living instances list
	XnList::ConstIterator it = instances.Find(this);
	assert(it != instances.end());
	instances.Remove(it);
}

XnStatus HandTracker::Init()
{            
	XnStatus			rc;
	XnCallbackHandle	chandle;

	// Create generators
	rc = gestureGenerator.Create(context);
	if (rc != XN_STATUS_OK)
	{
		printf("Unable to create GestureGenerator.");
		return rc;
	}

	rc = handsGenerator.Create(context);
	if (rc != XN_STATUS_OK)
	{
		printf("Unable to create HandsGenerator.");
		return rc;
	}

	// Register callbacks
	// Using this as cookie
	rc = gestureGenerator.RegisterGestureCallbacks(Gesture_Recognized, Gesture_Process, this, chandle);
	if (rc != XN_STATUS_OK)
	{
		printf("Unable to register gesture callbacks.");
		return rc;
	}

	rc = handsGenerator.RegisterHandCallbacks(Hand_Create, Hand_Update, Hand_Destroy, this, chandle);
	if (rc != XN_STATUS_OK)
	{
		printf("Unable to register hand callbacks.");
		return rc;
	}

	return XN_STATUS_OK;
}

XnStatus HandTracker::Run()
{
	XnStatus	rc = context.StartGeneratingAll();
	if (rc != XN_STATUS_OK)
	{
		printf("Unable to start generating.");
		return rc;
	}

	ADD_ALL_GESTURES;

	return XN_STATUS_OK;
}

const PointListHandler& HandTracker::GetPointListHandler() const
{
	return pointListHandler;
}

