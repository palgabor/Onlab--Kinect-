#pragma once

//DrawGame
#define CONFIG_XML_PATH "Config.xml"


//Viewer
#define WINDOW_TITLE "Draw game"

enum DisplayModes_e
{
	DISPLAY_MODE_OVERLAY,
	DISPLAY_MODE_DEPTH,
	DISPLAY_MODE_IMAGE
};
#define DEFAULT_DISPLAY_MODE DISPLAY_MODE_IMAGE

static const int MAX_DEPTH = 10000;
static const int GL_WIN_SIZE_X = 1280;
static const int GL_WIN_SIZE_Y = 1024;
static const int TEXTURE_SIZE = 512;


//HandTracker, HandViewer
static const int SIZE_OF_POINT_LIST = 10000;


//HandViewer
//hand is close to the sensor if the distance between hand and the sensor is lower than HAND_CLOSE value
static const unsigned HAND_CLOSE = 1000;

static const float COLOURS[][3] = 
{
	{1.0f,0.0f,0.0f},
	{0.0f,1.0f,0.0f},
	{0.0f,0.0f,1.0f},
	{0.5f,0.5f,0.5f},
	{0.0f,0.5f,1.0f},
	{1.0f,1.0f,0.0f},
	{1.0f,0.5f,0.0f},
	{1.0f,0.0f,1.0f}
};

static const unsigned RED = 0;
static const unsigned GREEN = 1;
static const unsigned BLUE = 2;
static const unsigned GREY = 3;
static const unsigned LBLUE = 4;
static const unsigned YELLOW = 5;
static const unsigned ORANGE = 6;
static const unsigned PINK = 7;

//Device
enum LED_STATUS {
	LED_OFF = 0,
	LED_GREEN,
	LED_RED,
	LED_ORANGE,
	LED_BLINK_ORANGE,
	LED_BLINK_GREEN,
	LED_BLINK_RED_ORANGE
};

