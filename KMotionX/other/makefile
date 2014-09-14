include $(BUILD_ROOT)/config.mak
SOURCES=client.c SimpleConsole.cpp ExecuteGCode.cpp
IDIR+=$(BUILD_ROOT)/KMotionX/include
IDIR+=$(BUILD_ROOT)/KMotionX/KMotionSupport/include
IDIR+=$(BUILD_ROOT)/KMotionDLL

LD_LIBS+=KMotionSupport
LD_LIBS+=KMotion 

#EXECUTABLE=client SimpleConsole ExecuteGCode
SHARED=
include $(BUILD_ROOT)/include.mk

all: client SimpleConsole ExecuteGCode 	
client:
	$(CC) -o $(BINDIR)/$@ $@.c	

#Add extra library for ExecuteGCode
ExecuteGCode: LD_LIBS+=GCodeInterpreter
ExecuteGCode: $(ODIR)/ExecuteGCode.o
	$(CC) $< $(LDFLAGS) -o $(BINDIR)/$@
	
SimpleConsole: $(ODIR)/SimpleConsole.o
	$(CC) $< $(LDFLAGS) -o $(BINDIR)/$@

#Add extra include dirs for ExecuteGCode
$(ODIR)/ExecuteGCode.o: IDIR+=../GCodeInterpreter ../KMotionDLL

