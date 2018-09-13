#TOP?=$(shell pwd)
export BUILD_ROOT := $(shell pwd)
include config.mak

SUBDIRS+=KMotionX/src
SUBDIRS+=KMotionDLL
SUBDIRS+=GCodeInterpreter
SUBDIRS+=KMotionServer
SUBDIRS+=KMotionX/examples/test
SUBDIRS+=KMotionX/examples/KFlopConsole
SUBDIRS+=KMotionX/examples/ExecuteGCode
SUBDIRS+=TCC67

ifeq ($(BUILD_JNI),true)
SUBDIRS+=KMotionX/JNI
endif

KMXPROGS+=KMotionServer$(EXESUF)
KMXPROGS+=tcc67$(EXESUF)
KMXLIBS+=libGCodeInterpreter$(LIBEXT)
KMXLIBS+=libKMotion$(LIBEXT)
KMXLIBS+=libKMotionX$(LIBEXT)

DSP_HEADERS:=$(wildcard DSP_KFLOP/*.h)
GCI_HEADERS:= $(wildcard GCodeInterpreter/*.h)
KM_HEADERS:=$(wildcard KMotionDLL/*.h)
KMX_HEADERS:=$(notdir $(wildcard KMotionX/include/*.h)) 
#$(subst KMotionX/include/,,$(wildcard KMotionX/include/**/*.h))

all: subdirs

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

INSTALL=install

install: subdirs
	mkdir -p "$(kmxdir)"
	$(INSTALL) -m755 $(addprefix $(BUILD_ROOT)/bin/,$(KMXLIBS)) "$(kmxdir)"
	mkdir -p "$(includedir)/kmx"
	mkdir -p "$(includedir)/kmx/DSP_KFLOP"
	mkdir -p "$(includedir)/kmx/GCodeInterpreter"
	mkdir -p "$(includedir)/kmx/KMotion"
	mkdir -p "$(includedir)/kmx/KMotionX"
	mkdir -p "$(kmxhome)/bin"
	$(INSTALL) -m755 $(addprefix $(BUILD_ROOT)/bin/,$(KMXPROGS)) "$(kmxhome)/bin"
	mkdir -p $(kmxhome)
	mkdir -p "$(kmxhome)/Data"
	$(INSTALL) -m644 $(addprefix $(BUILD_ROOT)/,$(DSP_HEADERS)) "$(includedir)/kmx/DSP_KFLOP"
	$(INSTALL) -m644 $(addprefix $(BUILD_ROOT)/,$(GCI_HEADERS)) "$(includedir)/kmx/GCodeInterpreter"
	$(INSTALL) -m644 $(addprefix $(BUILD_ROOT)/,$(KM_HEADERS)) "$(includedir)/kmx/KMotion"
	$(INSTALL) -m644 $(addprefix $(BUILD_ROOT)/KMotionX/include/,$(KMX_HEADERS)) "$(includedir)/kmx/KMotionX"
	cp -R $(BUILD_ROOT)/KMotionX/include/ftdi "$(includedir)/kmx/KMotionX"
	cp -R $(BUILD_ROOT)/KMotionX/include/win "$(includedir)/kmx/KMotionX"
	cp -R $(BUILD_ROOT)/DSP_KFLOP/ $(kmxhome)/DSP_KFLOP
	cp -R $(BUILD_ROOT)/DSP_KMotion/ $(kmxhome)/DSP_KMotion
	cp "$(BUILD_ROOT)/KMotion/Data/emc.var" "$(kmxhome)/Data/"


uninstall:
	rm -fv $(foreach P,$(KMXLIBS),"$(kmxdir)/$P")
	rm -rfv "$(includedir)/kmx"

clean:
#rm -f *.o *~
	rm -f ./bin/*
	for n in $(SUBDIRS); do $(MAKE) -C $$n clean; done
	
testtcc:
	make -C TCC67 testtcc


distclean: clean
	rm -vf config.h config.mak

config.mak:
	@echo "Please run ./configure."
	@exit 1


.PHONY: install uninstall subdirs $(SUBDIRS) clean distclean testtcc