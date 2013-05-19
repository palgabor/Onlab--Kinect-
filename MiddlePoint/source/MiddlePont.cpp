//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOpenNI.h>
#include <XnLog.h>
#include <XnCppWrapper.h>
#include <XnFPSCalculator.h>

#include <iostream>

//---------------------------------------------------------------------------
// Constants
//---------------------------------------------------------------------------
const std::string CONFIG_XML_PATH="Config.xml";

//---------------------------------------------------------------------------
// Inline functions
//---------------------------------------------------------------------------
inline void checkRetVal(XnStatus retVal, const std::string &task)
{
	if(retVal != XN_STATUS_OK) std::cout<<task<<" failed: "<<xnGetStatusString(retVal)<<std::endl;
}

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
using namespace xn;

void checkFileExists(const std::string &fileName)
{
	XnBool exists;
	xnOSDoesFileExist(fileName.c_str(), &exists);
	
	if (exists == false)
	{
		std::cout<<"Could not find "<<CONFIG_XML_PATH<<". Aborting."<<std::endl;
		exit(XN_STATUS_ERROR);
	}
}

XnStatus readConfigFromXmlFile(const std::string &configFileName, Context &context, ScriptNode &scriptNode, EnumerationErrors &errors)
{
		//check that Config.xml exists or not
	checkFileExists(configFileName);

	std::cout<<"Reading config from: "<<configFileName<<std::endl;
	std::cout<<"Press any key to exit..."<<std::endl<<std::endl;
		//reading config from Config.xml file
	XnStatus nRetVal = context.InitFromXmlFile(configFileName.c_str(), scriptNode, &errors);
	
		//if kinect doesn't available
	if (nRetVal == XN_STATUS_NO_NODE_PRESENT)
	{
		checkRetVal(nRetVal,"Read config file");
		exit(nRetVal);
	}
		//if Config.xml is exists, but can not readable
	else if (nRetVal != XN_STATUS_OK)
	{
		checkRetVal(nRetVal,"Open");
		exit(nRetVal);
	}
}

int main()
{
		//variable for return values
	XnStatus nRetVal = XN_STATUS_OK;

	Context context;
	ScriptNode scriptNode;
	EnumerationErrors errors;

		//check and read config from Config.xml file
	nRetVal = readConfigFromXmlFile(CONFIG_XML_PATH, context, scriptNode, errors);

		//create DepthGenerator type of variable
	DepthGenerator depthGenerator;
		//search and add existing DepthGenerator type of Production node to depthGenerator variable
	nRetVal = context.FindExistingNode(XN_NODE_TYPE_DEPTH, depthGenerator);
		//check return value
	checkRetVal(nRetVal, "Find depth generator");

		//create variable for frame per second value
	XnFPSData xnFPS;
		//set frame per second value
	nRetVal = xnFPSInit(&xnFPS, 180);
		//check return value
	checkRetVal(nRetVal, "FPS Init");

		//variable for depth matrix
	DepthMetaData depthMD;
	
		//"Press any key to exit"
	while (!xnOSWasKeyboardHit())
	{
			//wait until get new values from depth generator node (timeout is 2 seconds)
		nRetVal = context.WaitOneUpdateAll(depthGenerator);
		checkRetVal(nRetVal,"UpdateData");
		if (nRetVal != XN_STATUS_OK) continue;

			//marks that another frame was processed
		xnFPSMarkFrame(&xnFPS);

			//read depth matrix from depth generator node
		depthGenerator.GetMetaData(depthMD);
		
			//depth map with 16 bit pixels (this variable is never used, it is just a demonstration how to get the depth map)
		//const XnDepthPixel* depthMap = depthMD.Data();

			//Print some information to the screen
		const unsigned middlePoint = depthMD(depthMD.XRes()/2, depthMD.YRes()/2);
		std::string message;
		if(middlePoint == 0) message="You are too close to the sensor. Move back!";
		else if(middlePoint >0 && middlePoint <1000) message="You are close to the sensor.";
		else if(middlePoint >=1000 && middlePoint<5000) message="You are far from the sensor.";
		else if(middlePoint >=5000) message="You are too far from the sensor. Come closer!";
		
		std::cout<<message<<" Middle point is: ["<<middlePoint<<"cm] far from the sensor. Number or frames: ["<<depthMD.FrameID()<<"] FPS: ["<<xnFPSCalc(&xnFPS)<<"]\r";
	}

		//goodbye message:-)
	std::cout<<std::endl<<std::endl<<"Thank you for use my application."<<std::endl<<"Best Regards: Gabor Pal"<<std::endl;

		//destroy the previously created datastructures
	depthGenerator.Release();
	scriptNode.Release();
	context.Release();

	return 0;
}
