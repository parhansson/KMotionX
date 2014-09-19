include $(BUILD_ROOT)/config.mak
SOURCES=ExecuteGCode.cpp
IDIR+=$(BUILD_ROOT)/KMotionX/include
IDIR+=$(BUILD_ROOT)/KMotionX/KMotionSupport/include
IDIR+=$(BUILD_ROOT)/KMotionDLL
IDIR+=$(BUILD_ROOT)/GCodeInterpreter

LD_LIBS+=KMotionSupport
LD_LIBS+=KMotion
LD_LIBS+=GCodeInterpreter

EXECUTABLE=executeGCode
SHARED=

include $(BUILD_ROOT)/include.mk




