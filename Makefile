# Project information
PROJNAME:=arduino-mux
VERSION:=0.0.1

# Compiler
CC:=clang
D_PROJECT_VERSION:=-DPROJECT_VERSION="$(VERSION)"
CCFLAGS:=-Wall -Werror $(D_PROJECT_VERSION)

# Inputs
SRCDIR:=./src
INCDIR:=$(SRCDIR)/inc
SRCEXT:=c
INCEXT:=h
SOURCES:=$(shell find $(SRCDIR) -type f -name '*.$(SRCEXT)')
INCLUDES:=$(shell fine $(INCDIR) -type f -name '*.$(INCEXT)')

# Outputs
BINDIR:=./bin
BUILDDIR:=./build
OBJEXT:=o
OBJECTS:=$(subst .$(SRCEXT),.$(OBJEXT),$(subst $(SRCDIR),$(BUILDDIR),$(SOURCES)))
EXE:=$(BINDIR)/$(PROJNAME)-$(VERSION)

all:	$(BUILDDIR) $(BINDIR) $(EXE)

$(BUILDDIR) $(BINDIR):
	mkdir -p $@

$(EXE):	$(OBJECTS)
	$(CC) $(CCFLAGS) -o $@ $^

# Build objects that have headers
$(BUILDDIR)/%.$(OBJEXT):	$(SRCDIR)/%.$(SRCEXT) $(INCDIR)/%.$(INCEXT)
	$(CC) $(CCFLAGS) -c -o $@ $<

# Build objects that don't have headers
$(BUILDDIR)/%.$(OBJEXT):	$(SRCDIR)/%.$(SRCEXT)
	$(CC) $(CCFLAGS) -c -o $@ $^

lint:
	cppcheck $(SOURCES) $(INCLUDES)

clean:
	rm -rf $(BUILDDIR) $(BINDIR)

.PHONY:	clean lint
