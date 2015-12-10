include $(BUILD_ROOT)/config.mak
IDIR+=$(BUILD_ROOT)/KMotionX/include
IDIR+=$(BUILD_ROOT)/KMotionX/include/compatibility
IDIR+=$(BUILD_ROOT)/KMotionX/KMotionSupport/include
IDIR+=$(BUILD_ROOT)/KMotionDLL
IDIR+=$(BUILD_ROOT)/DSP_KFLOP
IDIR+=$(BUILD_ROOT)/GCodeInterpreter
IDIR+=$(BUILD_ROOT)/python
IDIR+=/usr/include/python2.7

LD_LIBS+=KMotionSupport
LD_LIBS+=KMotion
LD_LIBS+=GCodeInterpreter
LD_LIBS+=$(FTDILIB)


SOURCES=kmotion_wrap.cpp


EXECUTABLE=_kmotion.$(LIBEXT)
SHARED=-shared

include $(BUILD_ROOT)/include.mk

kmotion_wrap.cpp:   kmotion.i ../KMotionDLL/KMotionDLL.cpp ../KMotionDLL/KMotionDLL.h swig_extras.cpp
	swig -c++ -builtin $(addprefix -I,$(IDIR)) -I/usr/include $(DEFS) -cppext cpp  -python kmotion.i
    

