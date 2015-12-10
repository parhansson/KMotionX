include $(BUILD_ROOT)/config.mak
IDIR+=$(BUILD_ROOT)/KMotionX/include
IDIR+=$(BUILD_ROOT)/KMotionX/KMotionSupport/include

LD_LIBS+=KMotionSupport
#not allowed on mac
#LD_LIBS+=rt
LD_LIBS+=$(FTDILIB) 


SOURCES=COFFMAIN.cpp CLOAD.cpp KMotionDLL_Direct.cpp KMotionDLL.cpp KMotionLocal.cpp

ifeq ($(FTDILIB),ftd2xx)
SOURCES+=KmotionIO.cpp
else
SOURCES+=KmotionIOX.cpp
IDIR+=/usr/include/lib$(FTDILIB)
ifeq ($(OSNAME),Darwin)
	IDIR+=/opt/local/include/lib$(FTDILIB)
endif	
endif

EXECUTABLE=libKMotion.$(LIBEXT)
SHARED=-shared

include $(BUILD_ROOT)/include.mk
