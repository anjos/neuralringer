# Dear emacs, this is -*- makefile -*-
# André Rabello <Andre.dos.Anjos@cern.ch>

sinclude ../config.mk
CXXFLAGS += $(shell xml2-config --cflags)
LDFLAGS += $(shell xml2-config --libs)
INSTALL_DIR=../$(INSTALL_NAME)
sinclude ../build.mk
