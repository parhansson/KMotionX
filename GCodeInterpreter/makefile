include $(BUILD_ROOT)/config.mak

W_FLAGS+=-Wno-deprecated-register
W_FLAGS+=-Wno-overloaded-virtual
W_FLAGS+=-Wno-char-subscripts
W_FLAGS+=-Wno-dangling-else
W_FLAGS+=-Wno-parentheses
W_FLAGS+=-Wno-unused-but-set-variable
W_FLAGS+=-Wno-unused-variable
W_FLAGS+=-Wno-unknown-pragmas
W_FLAGS+=-Wno-unused-private-field

SOURCES=canon_stand_alone.cpp CoordMotion.cpp driver.cpp GCodeInterpreter.cpp Kinematics.cpp Kinematics3Rod.cpp PT2D.cpp PT3D.cpp rs274ngc.cpp SetupTracker.cpp TrajectoryPlanner.cpp
SOURCES+=KinematicsGeppetto.cpp KinematicsGeppettoExtrude.cpp Kinematics5AxisGimbalAB.cpp Kinematics5AxisGimbalCB.cpp
SOURCES+=Kinematics5AxisTableAB.cpp Kinematics5AxisTableAC.cpp Kinematics5AxisTableAGimbalB.cpp Kinematics5AxisTableBC.cpp KinematicsScara.cpp Kinematics2AxisRobot.cpp


IDIR+=$(BUILD_ROOT)/KMotionDLL

LD_LIBS+=KMotionX
LD_LIBS+=KMotion

EXECUTABLE=libGCodeInterpreter$(LIBEXT)
BIN_TYPE=$(LIBEXT)
include $(BUILD_ROOT)/include.mk
