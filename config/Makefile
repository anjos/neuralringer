# Dear emacs, this is -*- makefile -*-
# André Rabello <Andre.dos.Anjos@cern.ch>

sinclude ../config.mk
DEPEND = sys data
CXXFLAGS += `xml2-config --cflags`
SCHEMA += ./schema/network.xsd
INSTALL_DIR = ../installed
sinclude ../build.mk
