ODIR=build
BINDIR=$(BUILD_ROOT)/bin
LD_LIB_PATH+=$(BINDIR)

IFLAGS=-I$(BUILD_ROOT)/Linux/include/compatibility $(addprefix -I,$(IDIR))

W_FLAGS=-Wall -Wno-unknown-pragmas

CFLAGS+=-O0 -g3 -c -fmessage-length=0 -fPIC -MMD -MP
CFLAGS+=-std=c++11
CFLAGS+=$(W_FLAGS)
CFLAGS+=$(IFLAGS)
CFLAGS+=$(DEFS)

LDFLAGS+=$(SHARED)
LDFLAGS+=$(addprefix -l,$(LD_LIBS))
LDFLAGS+=$(addprefix -L,$(LD_LIB_PATH))
LDFLAGS+=-Wl,-rpath $(realpath $(BINDIR))



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
	
.PHONY: clean
clean:
	rm -rf $(ODIR) $(EXECUTABLE)
