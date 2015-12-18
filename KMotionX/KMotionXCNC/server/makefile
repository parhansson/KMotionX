include $(BUILD_ROOT)/config.mak
#DEFS+=-DDEBUG
SOURCES=utils.cpp frozen.cpp mongoose.cpp json.cpp handler.cpp webserver.cpp MessageQueue.cpp KmxController.cpp WebController.cpp
IDIR+=$(BUILD_ROOT)/KMotionX/include
IDIR+=$(BUILD_ROOT)/KMotionX/KMotionSupport/include
IDIR+=$(BUILD_ROOT)/KMotionDLL
IDIR+=$(BUILD_ROOT)/GCodeInterpreter
#DEFS+=-DNS_STACK_SIZE=500
LD_LIBS+=KMotionSupport
LD_LIBS+=KMotion
LD_LIBS+=GCodeInterpreter

EXECUTABLE=kmxWeb
SHARED=

include $(BUILD_ROOT)/include.mk




