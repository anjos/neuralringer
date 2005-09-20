# Dear emacs, this is -*- makefile -*-
# Andre Rabello <Andre.dos.Anjos@cern.ch>

sinclude ../config.mk
DEPEND = sys
CXXFLAGS += $(shell gsl-config --cflags) `xml2-config --cflags` -DHAVE_INLINE
LDFLAGS += $(shell gsl-config --libs)
INSTALL_DIR = ../installed
SCHEMA += ./schema/database.xsd
sinclude ../build.mk
