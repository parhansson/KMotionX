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
SUBDIRS+=KMotionX/KMotionXCNC/server
SUBDIRS+=TCC67

ifeq ($(BUILD_JNI),true)
SUBDIRS+=KMotionX/JNI
endif

KMXPROGS+=KMotionServer$(EXESUF)
KMXLIBS+=libGCodeInterpreter$(LIBEXT)
KMXLIBS+=libKMotion$(LIBEXT)
KMXLIBS+=libKMotionX$(LIBEXT)

DSP_HEADERS+=KMotionDef.h PC-DSP.h
GCI_HEADERS+=canon.h CoordMotion.h driver.h GCodeInterpreter.h 
GCI_HEADERS+=GCodeInterpreterX.h Kinematics.h Kinematics3Rod.h 
GCI_HEADERS+=PT2D.h PT3D.h rs274ngc_errors.h rs274ngc_return.h
GCI_HEADERS+=rs274ngc.h SetupTracker.h StdAfx.h TrajectoryPlanner.h

KM_HEADERS+=CLOAD.h COFF.h KMotionDLL_Direct.h KMotionDLL.h
KM_HEADERS+=KMotionDLLX.h KmotionIO.h KMotionLocal.h
KM_HEADERS+=PARAMS.h StdAfx.h

# KMX_HEADERS+=KMotionX.h StdAfx.h

all: subdirs

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

INSTALL=install

install: subdirs
	mkdir -p "$(bindir)"
	$(INSTALL) -m755 $(addprefix $(BUILD_ROOT)/bin/,$(KMXPROGS)) "$(bindir)"
	mkdir -p "$(kmxdir)"
	$(INSTALL) -m755 $(addprefix $(BUILD_ROOT)/bin/,$(KMXLIBS)) "$(kmxdir)"
	mkdir -p "$(includedir)/kmx"
	mkdir -p "$(includedir)/kmx/DSP_KLFOP"
	mkdir -p "$(includedir)/kmx/GCodeInterpreter"
	mkdir -p "$(includedir)/kmx/KMotion"
	$(INSTALL) -m644 $(addprefix $(BUILD_ROOT)/DSP_KFLOP/,$(DSP_HEADERS)) "$(includedir)/kmx/DSP_KLFOP"
	$(INSTALL) -m644 $(addprefix $(BUILD_ROOT)/GCodeInterpreter/,$(GCI_HEADERS)) "$(includedir)/kmx/GCodeInterpreter"
	$(INSTALL) -m644 $(addprefix $(BUILD_ROOT)/KMotionDLL/,$(KM_HEADERS)) "$(includedir)/kmx/KMotion"

uninstall:
	rm -fv $(foreach P,$(KMXPROGS),"$(bindir)/$P")
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