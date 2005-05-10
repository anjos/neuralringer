# Dear emacs, this is -*- makefile -*-
# André Rabello <Andre.dos.Anjos@cern.ch>

# Establishes the default system configuration
CC=g++
CXXFLAGS += -DRINGER_DEBUG=0 -D_GNU_SOURCE --ansi --pedantic -Wall -pthread -fPIC -g
VERSION=0.5.1

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

# Garbage
GARB	= $(shell find . -name "*~" -or -name "*.o")

# How to build simple executables
%: %.o
	$(CC) -o $@ $< $(LIBDIR:%=-L%) $(LIBNAME:%=-l%)
	@mv -v $@ ./

# How to build cxx programs
prog/%.o: prog/%.cxx
	$(CC) -I./installed/include $(shell xml2-config --cflags) -D__PACKAGE__=\"$(@:prog/%.o=%)\" $(CXXFLAGS) -c $< -o $@

# How to build cxx (C++) files
%.o: %.cxx
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

