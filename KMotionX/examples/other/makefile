include $(BUILD_ROOT)/config.mak
#SOURCES+=client.c
#SOURCES+=SimpleConsole.cpp
SOURCES+=ExecuteGCode.cpp

IDIR+=$(BUILD_ROOT)/KMotionX/include
IDIR+=$(BUILD_ROOT)/KMotionX/KMotionSupport/include
IDIR+=$(BUILD_ROOT)/KMotionDLL

LD_LIBS+=KMotionSupport
LD_LIBS+=KMotion 

#EXECUTABLE=client SimpleConsole ExecuteGCode
SHARED=
include $(BUILD_ROOT)/include.mk

#all: client SimpleConsole ExecuteGCode	
all:	
client:
	$(CC) -o $(BINDIR)/$@ $@.c	

#Add extra library for ExecuteGCode

SimpleConsole: $(ODIR)/SimpleConsole.o
	$(CC) $< $(LDFLAGS) -o $(BINDIR)/$@


