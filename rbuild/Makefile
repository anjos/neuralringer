# Dear emacs, this is -*- makefile -*-
# Andre Rabello <Andre.dos.Anjos@cern.ch>

sinclude ../config.mk
DEPEND = sys data roiformat
CXXFLAGS += $(shell xml2-config --cflags)
LDFLAGS += $(shell xml2-config --libs)
SCHEMA += ./schema/ring.xsd
INSTALL_DIR=../$(INSTALL_NAME)
sinclude ../build.mk
