####################################################################
####################################################################
########    Kinect makefile for personal usage              ########
########    Created by Gábor Pál (pal.gabor89@gmail.com)    ########
########    Date: 2012. 11. 06.                             ########
####################################################################
####################################################################

# Set your Kinect home to this variable. You have to store your Common, Include, Lib, etc. libraries under this directory.
OPENNI_LIB_HOME = /home/palgabor/kinect/prog/OpenNILib



# You can modify these variables depending on your project or your needs.
BIN_DIR = .
SRC_FILES = source/*.cpp
EXE_NAME = MiddlePoint



# Common stuff, do not change it unless you have good reason for it.
USED_LIBS = OpenNI
INC_DIRS = $(OPENNI_LIB_HOME)/Include =/usr/include/ni
LIB_DIRS += $(OPENNI_LIB_HOME)/Lib
include $(OPENNI_LIB_HOME)/Common/CommonCppMakefile	

