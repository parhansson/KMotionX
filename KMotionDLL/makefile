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
LD_LIBS+=usb-1.0
LD_LIBS+=pthread

SOURCES=COFFMAIN.cpp CLOAD.cpp KMotionDLL_Direct.cpp KMotionDLL.cpp KMotionLocal.cpp TranslateX.cpp

ifeq ($(FTDILIB),ftd2xx)
SOURCES+=KmotionIO.cpp
SOURCES+=Ping.cpp
else
SOURCES+=KmotionIOX.cpp
SOURCES+=PingX.cpp
#Hotplug works but do not scan for devices on startup
#SOURCES+=PingXHotplug.cpp
IFLAGS+=$(IFLAG_FTDI)
endif

EXECUTABLE=libKMotion$(LIBEXT)
BIN_TYPE=$(LIBEXT)

include $(BUILD_ROOT)/include.mk
