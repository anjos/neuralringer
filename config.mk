# Dear emacs, this is -*- makefile -*-
# Andre Rabello <Andre.dos.Anjos@cern.ch>

# Establishes the default system configuration
VERSION=0.6
CC=g++
CXXFLAGS += -DRINGER_DEBUG=0 -D_GNU_SOURCE --ansi --pedantic -Wall -pthread -fPIC -O2 -DNR_VERSION=\"$(VERSION)\"

# The XML backend to use, can be "xerces" or "libxml2"
XML_BACK_END = libxml2

# How to define the package name
PACKAGE = $(shell basename `pwd`)

# Where to place installed files
INSTALL_NAME=installed 
INSTALL_DIR=./$(INSTALL_NAME)
INSTALL_LIB=$(INSTALL_DIR:%=%/lib)
INSTALL_BIN=$(INSTALL_DIR:%=%/bin)
INSTALL_INC=$(INSTALL_DIR:%=%/include)
INSTALL_SCHEMA=$(INSTALL_DIR:%=%/schema)

# Files and names
SRCDIR   = src
LIBDIR   = . $(INSTALL_LIB)
SRC	 = $(shell find $(SRCDIR) -name "*.cxx" | grep -v "test")
TSTSRC   = $(shell find $(SRCDIR) -name "test_*.cxx")
MYTSTSRC = $(shell find $(SRCDIR) -name "mytest_*.cxx")
HEADER   = $(shell find $(PACKAGE) -name "*.h")
INCLUDE	+= . $(INSTALL_INC)
OBJ	 = $(SRC:%.cxx=%.o)
CPPFLAGS+= $(INCLUDE:%=-I%) -D__PACKAGE__=\"$(PACKAGE)\"
LDFLAGS  = $(LIBDIR:%=-L%) $(DEPEND:%=-l%) -lpthread

LIBNAME = $(PACKAGE)
LIB = $(LIBNAME:%=lib%.so)

# XML back end
PROGS_CPPFLAGS = -I./installed/include
ifeq ($(XML_BACK_END),xerces)
 PROGS_CPPFLAGS += -DXERCES_XML_BACK_END
else
 PROGS_CPPFLAGS += $(shell xml2-config --cflags)
endif

# Garbage
GARB	= $(shell find . -name "*~" -or -name "*.o")

# How to build simple executables
%: %.o
	$(CC) -o $@ $< $(LIBDIR:%=-L%) $(LIBNAME:%=-l%)
	@mv -v $@ ./

# How to build cxx programs
prog/%.o: prog/%.cxx
	$(CC) $(PROGS_CPPFLAGS) $(PROGS_XML_LDFLAGS) -D__PACKAGE__=\"$(@:prog/%.o=%)\" $(CXXFLAGS) -c $< -o $@

# How to build cxx (C++) files
%.o: %.cxx
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

