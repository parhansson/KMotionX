include $(BUILD_ROOT)/config.mak

#LDFLAGS+= -Wl,--allow-multiple-definition 
#DEFS += -D_DEBUG
W_FLAGS+=-Wno-deprecated-declarations
W_FLAGS+=-Wno-sometimes-uninitialized
W_FLAGS+=-Wno-shift-negative-value
W_FLAGS+=-Wno-unused-function
W_FLAGS+=-Wno-unused-but-set-variable


SOURCES=tcc.cpp WriteCoff.cpp


EXECUTABLE=tcc67


include $(BUILD_ROOT)/include.mk


# Following target runs the local tcc and the Windows TCC67 (assuming 'wine' installed) on sample C
# code and compares the result.  Should be identical.
TESTPARMS=-text 80050000 -g -nostdinc -I$(BUILD_ROOT)/DSP_KFLOP
TESTCPROG=test
KOUT=$(BUILD_ROOT)/DSP_KFLOP/DSPKFLOP.out
PATH_TO_EXE=../../c:/KMotion432/KMotion/Release/TCC67.exe

testtcc:
	wine $(PATH_TO_EXE) $(TESTPARMS) -o $(TESTCPROG)-wine.out $(TESTCPROG).c  $(KOUT)
	hd $(TESTCPROG)-wine.out > $(TESTCPROG)-wine.hex
	$(BINDIR)/tcc67  $(TESTPARMS) -o $(TESTCPROG).out $(TESTCPROG).c  $(KOUT)
	hd $(TESTCPROG).out > $(TESTCPROG).hex
	diff $(TESTCPROG)-wine.out $(TESTCPROG).out

