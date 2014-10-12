include $(BUILD_ROOT)/config.mak
SOURCES+=client.c
SOURCES+=test.cpp

IDIR+=$(BUILD_ROOT)/KMotionX/include
IDIR+=$(BUILD_ROOT)/KMotionX/KMotionSupport/include
IDIR+=$(BUILD_ROOT)/KMotionDLL

LD_LIBS+=KMotionSupport
LD_LIBS+=KMotion 

EXECUTABLE=client testRunner
SHARED=
include $(BUILD_ROOT)/include.mk

#all: client testRunner	
all:	
client:
	$(CC) -o $(BINDIR)/$@ $@.c	

#Add extra library for ExecuteGCode

testRunner: $(ODIR)/test.o
	$(CC) $< $(LDFLAGS) -o $(BINDIR)/$@


