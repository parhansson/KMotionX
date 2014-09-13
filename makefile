export BUILD_ROOT := $(shell pwd)
include config.mak

SUBDIRS = Linux/KMotionSupport KMotionDLL GCodeInterpreter Linux/KMotionServer Linux/other
ifeq ($(BUILD_JNI),true)
SUBDIRS += Linux/JNI
endif

          
all: subdirs

subdirs:
	for n in $(SUBDIRS); do $(MAKE) -C $$n || exit 1; done

clean:
#rm -f *.o *~
	rm -f ./bin/*
	for n in $(SUBDIRS); do $(MAKE) -C $$n clean; done