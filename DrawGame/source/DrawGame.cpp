//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <iostream>

#include "HandViewer.h"


//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
xn::Context		context;
xn::ScriptNode	scriptNode;

int main(int argc, char** argv)
{
	XnStatus returnCode;
	xn::EnumerationErrors enumErrors;

	// Create a context with default settings
	returnCode = context.InitFromXmlFile(CONFIG_XML_PATH, scriptNode, &enumErrors);
	if (returnCode == XN_STATUS_NO_NODE_PRESENT)
	{
		XnChar strError[1024];
		enumErrors.ToString(strError, 1024);
		std::cout<<strError<<std::endl;
		return returnCode;
	}
	else if (returnCode != XN_STATUS_OK)
	{
		std::cout<<std::endl<<"Open failed: "<<xnGetStatusString(returnCode)<<std::endl;
		return (returnCode);
	}

	Viewer& viewer = HandViewer::CreateInstance(context);
	returnCode = viewer.Init(argc, argv);
	if (returnCode != XN_STATUS_OK)
	{
		std::cout<<std::endl<<"Viewer init failed: "<<xnGetStatusString(returnCode)<<std::endl;
		return returnCode;
	}

	returnCode = viewer.Run();
	if (returnCode != XN_STATUS_OK)
	{
		std::cout<<std::endl<<"Viewer run failed: "<<xnGetStatusString(returnCode)<<std::endl;
		return returnCode;
	}

	return 0;
}
