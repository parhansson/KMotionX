ODIR=build
BINDIR=$(BUILD_ROOT)/bin
# add KMotionX include first
IDIR_ALL=$(BUILD_ROOT)/KMotionX/include $(BUILD_ROOT)/DSP_KFLOP $(IDIR)
IFLAGS+=$(addprefix -I,$(IDIR_ALL))

CFLAGS+=-g -c -fmessage-length=0 -fPIC -MMD -MP
CFLAGS+=-std=c++20
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
LDFLAGS+=-install_name '@rpath/$(EXECUTABLE)'
endif
endif
LDFLAGS+=$(addprefix -l,$(LD_LIBS))
LDFLAGS+=-L$(BINDIR)
ifeq ($(OSNAME),Linux)
LDFLAGS+=-Wl,-rpath,'$$ORIGIN'
else ifeq ($(OSNAME),Darwin)
LDFLAGS+=-Wl,-rpath,@loader_path
endif
LDFLAGS+=-Wl,-rpath,"$(RUNTIME_LIBDIR)"




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
