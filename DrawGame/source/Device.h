#pragma once

#include <XnUSB.h>
#include <cstdio>
#include <unistd.h>

class Device
{
	private:
		enum 
		{
			MaxDevs = 16
		};
		XN_USB_DEV_HANDLE devices[MaxDevs];
		XnUInt32 num;
		bool isOpened;
		int currentPosition;
		
		bool Open();
		void Close();
		
	public:
		Device();
		virtual ~Device();
		bool Move(int angle);
		bool SetLedColour(int colour);
		int GetCurrentPosition();
};
