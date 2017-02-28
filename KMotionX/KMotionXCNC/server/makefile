include $(BUILD_ROOT)/config.mak
#DEFS+=-DDEBUG
SOURCES=utils.cpp frozen.cpp mongoose.cpp json.cpp handler.cpp webserver.cpp
SOURCES+=MessageQueue.cpp AbstractController.cpp FileStatus.cpp KmxController.cpp WebController.cpp

IDIR+=$(BUILD_ROOT)/KMotionDLL
IDIR+=$(BUILD_ROOT)/GCodeInterpreter
#DEFS+=-DNS_STACK_SIZE=500
LD_LIBS+=KMotionX
LD_LIBS+=KMotion
LD_LIBS+=GCodeInterpreter

EXECUTABLE=kmxWeb
SHARED=

include $(BUILD_ROOT)/include.mk




