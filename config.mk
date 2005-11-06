# Dear emacs, this is -*- makefile -*-
# Andre Rabello <Andre.dos.Anjos@cern.ch>

# Establishes the default system configuration
PROJ=neuralringer
VERSION=0.7.0
CC=g++
CXXFLAGS += -O2 -DRINGER_DEBUG=0 -D_GNU_SOURCE --ansi --pedantic -Wall -pthread -fPIC -DNR_VERSION=\"$(VERSION)\"

# The XML backend to use, can be "xerces" or "libxml2"
XML_BACK_END = libxml2

# Where to place installed files
INSTALL_LIB=lib
INSTALL_BIN=bin

# Files and names
SRCDIR   = sources/$(PKG)
LIBDIR   = . $(INSTALL_LIB)
SRC	 = $(shell find $(SRCDIR) -name "*.cxx" | grep -v "test")
TSTSRC   = $(shell find $(SRCDIR) -name "test_*.cxx")
MYTSTSRC = $(shell find $(SRCDIR) -name "mytest_*.cxx")
INCLUDE	+= neuralringer
OBJ	 = $(SRC:%.cxx=%.o)
CPPFLAGS+= $(INCLUDE:%=-I%) -D__PACKAGE__=\"$(PKG)\"
LDFLAGS  = $(LIBDIR:%=-L%) $(DEPEND:%=-l%) -lpthread

LIBNAME = $(PKG)
LIB = $(LIBNAME:%=lib%.so)

# XML back end
PROGS_CPPFLAGS = -I./neuralringer
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

