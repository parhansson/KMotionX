include $(BUILD_ROOT)/config.mak

W_FLAGS+=-Wno-deprecated-register
W_FLAGS+=-Wno-null-conversion
W_FLAGS+=-Wno-parentheses
W_FLAGS+=-Wno-unused-variable
W_FLAGS+=-Wno-unused-but-set-variable
W_FLAGS+=-Wno-unknown-pragmas
W_FLAGS+=-Wno-unused-private-field
#todo fix ftdi_usb_purge_buffers KmotionXIOX
#W_FLAGS+=-Wno-deprecated-declarations

LD_LIBS+=KMotionX
LD_LIBS+=$(FTDILIB)

SOURCES=COFFMAIN.cpp CLOAD.cpp KMotionDLL_Direct.cpp KMotionDLL.cpp KMotionLocal.cpp TranslateX.cpp

ifeq ($(FTDILIB),ftd2xx)
SOURCES+=KmotionIO.cpp
else
SOURCES+=KmotionIOX.cpp
SOURCES+=PingX.cpp
IDIR+=/usr/include/lib$(FTDILIB)
ifeq ($(OSNAME),Darwin)
#if installed with macports
 	IDIR+=/opt/local/include/lib$(FTDILIB)
#if installed with homebrew
	IDIR+=/usr/local/include/lib$(FTDILIB)
endif	
endif

EXECUTABLE=libKMotion$(LIBEXT)
BIN_TYPE=$(LIBEXT)

include $(BUILD_ROOT)/include.mk
