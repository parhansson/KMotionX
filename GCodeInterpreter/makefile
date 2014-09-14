include $(BUILD_ROOT)/config.mak
#CLOAD.CPP COFFMAIN.CPP 
SOURCES=canon_stand_alone.cpp CoordMotion.cpp driver.cpp GCodeInterpreter.cpp Kinematics.cpp Kinematics3Rod.cpp PT2D.cpp PT3D.cpp rs274ngc.cpp SetupTracker.cpp TrajectoryPlanner.cpp
IDIR+=$(BUILD_ROOT)/KMotionX/include
IDIR+=$(BUILD_ROOT)/KMotionX/KMotionSupport/include
IDIR+=$(BUILD_ROOT)/KMotionX/KMotionSupport $(BUILD_ROOT)/KMotionDLL
IDIR+=./

LD_LIBS+=KMotionSupport
LD_LIBS+=KMotion

EXECUTABLE=libGCodeInterpreter.$(LIBEXT)
SHARED=-shared
include $(BUILD_ROOT)/include.mk
