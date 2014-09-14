export BUILD_ROOT := $(shell pwd)
include config.mak

SUBDIRS = KMotionX/KMotionSupport KMotionDLL GCodeInterpreter KMotionX/KMotionServer KMotionX/other
ifeq ($(BUILD_JNI),true)
SUBDIRS += KMotionX/JNI
endif

          
all: subdirs

subdirs:
	for n in $(SUBDIRS); do $(MAKE) -C $$n || exit 1; done

clean:
#rm -f *.o *~
	rm -f ./bin/*
	for n in $(SUBDIRS); do $(MAKE) -C $$n clean; done