include $(BUILD_ROOT)/config.mak
#CLOAD.CPP COFFMAIN.CPP 
SOURCES=canon_stand_alone.cpp CoordMotion.cpp driver.cpp GCodeInterpreter.cpp Kinematics.cpp Kinematics3Rod.cpp PT2D.cpp PT3D.cpp rs274ngc.cpp SetupTracker.cpp TrajectoryPlanner.cpp
SOURCES+=KinematicsGeppetto.cpp KinematicsGeppettoExtrude.cpp Kinematics5AxisGimbalAB.cpp Kinematics5AxisGimbalCB.cpp Kinematics5AxisTableAC.cpp Kinematics5AxisTableBC.cpp

IDIR+=$(BUILD_ROOT)/KMotionDLL

LD_LIBS+=KMotionX
LD_LIBS+=KMotion

EXECUTABLE=libGCodeInterpreter$(LIBEXT)
BIN_TYPE=$(LIBEXT)
include $(BUILD_ROOT)/include.mk
