include $(BUILD_ROOT)/config.mak
IDIR+=$(BUILD_ROOT)/KMotionX/include
IDIR+=$(BUILD_ROOT)/KMotionX/KMotionSupport/include

LD_LIBS+=KMotionSupport
LD_LIBS+=$(FTDILIB)

LDFLAGS+= -Wl,--allow-multiple-definition 


SOURCES=tcc.cpp WriteCoff.cpp


EXECUTABLE=tcc67

include $(BUILD_ROOT)/include.mk
