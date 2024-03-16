ODIR=build
BINDIR=$(BUILD_ROOT)/bin
LD_LIB_PATH+=$(BINDIR)
#LD_LIB_PATH+=../bin
# add KMotionX include first
IDIR_ALL=$(BUILD_ROOT)/KMotionX/include $(BUILD_ROOT)/DSP_KFLOP $(IDIR)
IFLAGS=$(addprefix -I,$(IDIR_ALL))

W_FLAGS=-Wall -Wno-unknown-pragmas
#exceptions on warnings
W_FLAGS+=-Wno-overloaded-virtual
W_FLAGS+=-Wno-deprecated-declarations
W_FLAGS+=-Wno-dangling-else
W_FLAGS+=-Wno-unused-variable
W_FLAGS+=-Wno-writable-strings
W_FLAGS+=-Wno-parentheses
W_FLAGS+=-Wno-unused-but-set-variable
W_FLAGS+=-Wno-char-subscripts
W_FLAGS+=-Wno-comment
W_FLAGS+=-Wno-deprecated-register

CFLAGS+=-g -c -fmessage-length=0 -fPIC -MMD -MP
CFLAGS+=-std=c++0x
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