include $(BUILD_ROOT)/config.mak
SOURCES=KMotionXjni.cpp

IDIR+=$(BUILD_ROOT)/KMotionDLL
# need a better way to detect java home
IDIR+=$(JAVA_HOME)/include
ifeq ($(OSNAME),Linux)
IDIR+=$(JAVA_HOME)/include/linux
else ifeq ($(OSNAME),Darwin)
IDIR+=$(JAVA_HOME)/include/darwin
endif

LD_LIBS+=KMotionX
LD_LIBS+=KMotion
LD_LIBS+=GCodeInterpreter

EXECUTABLE=libKMotionXjni$(LIBEXT)
SHARED=-shared

include $(BUILD_ROOT)/include.mk

