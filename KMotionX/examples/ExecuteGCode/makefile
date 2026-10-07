include $(BUILD_ROOT)/config.mak
W_FLAGS+=-Wno-overloaded-virtual
W_FLAGS+=-Wno-unknown-pragmas

SOURCES=ExecuteGCode.cpp
IDIR+=$(BUILD_ROOT)/KMotionDLL
IDIR+=$(BUILD_ROOT)/GCodeInterpreter

LD_LIBS+=KMotionX
LD_LIBS+=KMotion
LD_LIBS+=GCodeInterpreter

EXECUTABLE=executeGCode
BIN_TYPE=$(EXESUF)

include $(BUILD_ROOT)/include.mk




