include $(BUILD_ROOT)/config.mak
#DEFS+=-DDEBUG

W_FLAGS+=-Wno-deprecated-register
W_FLAGS+=-Wno-unknown-pragmas
W_FLAGS+=-Wno-overloaded-virtual

SOURCES=utils.cpp frozen.cpp mongoose.cpp json.cpp handler.cpp webserver.cpp
SOURCES+=MessageQueue.cpp AbstractController.cpp FileStatus.cpp KmxController.cpp WebController.cpp
IDIR+=$(includedir)/kmx/DSP_KFLOP
IDIR+=$(includedir)/kmx/KMotionX
IDIR+=$(includedir)/kmx/KMotion
IDIR+=$(includedir)/kmx/GCodeInterpreter
#DEFS+=-DNS_STACK_SIZE=500
LD_LIBS+=KMotionX
LD_LIBS+=KMotion
LD_LIBS+=GCodeInterpreter

EXECUTABLE=kmxWeb
SHARED=

include $(BUILD_ROOT)/include.mk




