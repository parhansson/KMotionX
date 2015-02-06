include $(BUILD_ROOT)/config.mak
IDIR+=$(BUILD_ROOT)/KMotionX/include
IDIR+=$(BUILD_ROOT)/KMotionX/include/compatibility
IDIR+=$(BUILD_ROOT)/KMotionX/KMotionSupport/include
IDIR+=$(BUILD_ROOT)/KMotionDLL
IDIR+=$(BUILD_ROOT)/DSP_KFLOP
IDIR+=/usr/include/python2.7

LD_LIBS+=KMotionSupport
LD_LIBS+=KMotion
LD_LIBS+=$(FTDILIB)


SOURCES=kmotion_wrap.cpp


EXECUTABLE=_kmotion.$(LIBEXT)
SHARED=-shared

include $(BUILD_ROOT)/include.mk

kmotion_wrap.cpp:   kmotion.i
	swig -c++ -builtin $(addprefix -I,$(IDIR)) -I/usr/include $(DEFS) -cppext cpp  -python $^
    

