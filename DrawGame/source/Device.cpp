#include "Device.h"

Device::Device()
{
	XnStatus res;
    isOpened = false;
	if (!Open()) // Open motor devices
		xnPrintError(res, "Opening motor failed");
	
	Move(0);	
	currentPosition = 0;
}

Device::~Device()
{
    Close();
}

bool Device::Open()
{
    const XnUSBConnectionString *paths;
    XnUInt32 count;
    XnStatus res;
	
	//Don't need this code, because USB is already initialized
	/*
    // Init OpenNI USB
    res = xnUSBInit();
    if (res != XN_STATUS_OK)
    {
        xnPrintError(res, "xnUSBInit failed");
        return false;
    }
    */
    // Open all "Kinect motor" USB devices
    res = xnUSBEnumerateDevices(0x045E /* VendorID */, 0x02B0 /*ProductID*/, &paths, &count);
    if (res != XN_STATUS_OK)
    {
        xnPrintError(res, "xnUSBEnumerateDevices failed");
        return false;
    }
    
    // Open devices
    for (XnUInt32 index = 0; index < count; ++index)
    {
        res = xnUSBOpenDeviceByPath(paths[index], &devices[index]);
        if (res != XN_STATUS_OK) {
            xnPrintError(res, "xnUSBOpenDeviceByPath failed");
            return false;
        }
    }
    
    num = count;
    XnUChar buf[1]; // output buffer
    
    // Init motors
    for (XnUInt32 index = 0; index < num; ++index)
    {
        res = xnUSBSendControl(devices[index], (XnUSBControlType) 0xc0, 0x10, 0x00, 0x00, buf, sizeof(buf), 0);
        if (res != XN_STATUS_OK) {
            xnPrintError(res, "xnUSBSendControl failed");
            Close();
            return false;
        }
        
        res = xnUSBSendControl(devices[index], XN_USB_CONTROL_TYPE_VENDOR, 0x06, 0x01, 0x00, NULL, 0, 0);
        if (res != XN_STATUS_OK) {
            xnPrintError(res, "xnUSBSendControl failed");
            Close();
            return false;
        }
    }
    
    isOpened = true;
    return true;
}

void Device::Close()
{
    if (isOpened) {
        for (XnUInt32 index = 0; index < num; ++index) {
            xnUSBCloseDevice(devices[index]);
        }
        isOpened = false;
    }
}
bool Device::Move(int angle)
{
    XnStatus res;
    
    if(angle > currentPosition && angle > 40 || angle < currentPosition && angle < -40)
		return true;
    
    // Send move control requests
    for (XnUInt32 index = 0; index < num; ++index)
    {
        res = xnUSBSendControl(devices[index], XN_USB_CONTROL_TYPE_VENDOR, 0x31, angle, 0x00, NULL, 0, 0);

        if (res != XN_STATUS_OK)
        {
            xnPrintError(res, "xnUSBSendControl failed");
            return false;
        }
    }
    currentPosition = angle;
    return true;
}

bool Device::SetLedColour(int colour)
{
	XnStatus res;
	for (XnUInt32 index = 0; index < num; ++index)
	{
		res = xnUSBSendControl(devices[index], XN_USB_CONTROL_TYPE_VENDOR, 0x06, colour, 0x00, NULL, 0, 0);
 
		if (res != XN_STATUS_OK)
		{
			xnPrintError(res, "xnUSBSendControl failed");
			return false;
		}
	}
	return true;
}

int Device::GetCurrentPosition()
{
	return currentPosition;
}
