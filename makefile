export BUILD_ROOT := $(shell pwd)
include config.mak

SUBDIRS+=KMotionX/KMotionSupport
SUBDIRS+=KMotionDLL
SUBDIRS+=GCodeInterpreter
SUBDIRS+=KMotionX/KMotionServer
SUBDIRS+=KMotionX/examples/other
SUBDIRS+=KMotionX/examples/KFlopConsole
SUBDIRS+=KMotionX/examples/ExecuteGCode
SUBDIRS+=KMotionX/examples/webgui
SUBDIRS+=TCC67

ifeq ($(BUILD_JNI),true)
SUBDIRS+=KMotionX/JNI
endif

          
all: subdirs

subdirs:
	for n in $(SUBDIRS); do $(MAKE) -C $$n || exit 1; done

clean:
#rm -f *.o *~
	rm -f ./bin/*
	for n in $(SUBDIRS); do $(MAKE) -C $$n clean; done
	
testtcc:
	make -C TCC67 testtcc
