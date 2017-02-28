include $(BUILD_ROOT)/config.mak

LD_LIBS+=KMotionX
LD_LIBS+=$(FTDILIB)


SOURCES=COFFMAIN.cpp CLOAD.cpp KMotionDLL_Direct.cpp KMotionDLL.cpp KMotionLocal.cpp

ifeq ($(FTDILIB),ftd2xx)
SOURCES+=KmotionIO.cpp
else
SOURCES+=KmotionIOX.cpp
IDIR+=/usr/include/lib$(FTDILIB)
ifeq ($(OSNAME),Darwin)
#if installed with macports
 	IDIR+=/opt/local/include/lib$(FTDILIB)
#if installed with homebrew
	IDIR+=/usr/local/include/lib$(FTDILIB)
endif	
endif

EXECUTABLE=libKMotion$(LIBEXT)
SHARED=-shared

include $(BUILD_ROOT)/include.mk
