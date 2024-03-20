ODIR=build
BINDIR=$(BUILD_ROOT)/bin
LD_LIB_PATH+=$(BINDIR)
#LD_LIB_PATH+=../bin
# add KMotionX include first
IDIR_ALL=$(BUILD_ROOT)/KMotionX/include $(BUILD_ROOT)/DSP_KFLOP $(IDIR)
IFLAGS+=$(addprefix -I,$(IDIR_ALL))

CFLAGS+=-g -c -fmessage-length=0 -fPIC -MMD -MP
CFLAGS+=-std=c++0x
#exceptions on warnings default
CFLAGS+=-Wall
#wflag set by others might exclude specific warnings
CFLAGS+=$(W_FLAGS)
CFLAGS+=$(IFLAGS)
CFLAGS+=$(DEFS)

ifeq ($(BIN_TYPE),$(LIBEXT))
ifeq ($(OSNAME),Linux)
LDFLAGS+=-shared
else ifeq ($(OSNAME),Darwin)
LDFLAGS+=-dynamiclib
LDFLAGS+=-install_name '$(kmxdir)/$(EXECUTABLE)'
endif
endif
LDFLAGS+=$(addprefix -l,$(LD_LIBS))
LDFLAGS+=$(addprefix -L,$(LD_LIB_PATH))
LDFLAGS+=-Wl,-rpath,$(realpath $(kmxdir))
#LDFLAGS+=-Wl,-rpath $(realpath $(BINDIR))
#LDFLAGS+=-Wl,-rpath,$$ORIGIN
#LDFLAGS+=-Wl,-z,origin




# Make sure the output directory exists.
dummy := $(shell test -d $(BINDIR) || mkdir -p $(BINDIR))

 
OBJECTS=$(SOURCES:%.cpp=$(ODIR)/%.o)

all: $(ODIR) $(EXECUTABLE)

$(ODIR):
	mkdir $@
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(BINDIR)/$@

#GNU style	
$(ODIR)/%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@

$(ODIR)/%.o: %.CPP
	$(CC) $(CFLAGS) $< -o $@
	

clean:
	@echo "Cleaning... $(shell pwd)"
	rm -rf $(ODIR) $(EXECUTABLE)

.PHONY: clean