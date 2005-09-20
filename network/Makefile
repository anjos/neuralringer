# Dear emacs, this is -*- makefile -*-
# Andre Rabello <Andre.dos.Anjos@cern.ch>

sinclude ../config.mk
DEPEND = sys data config
CXXFLAGS += `xml2-config --cflags`
GARB += *.ps *.ps2 *.png *.gif *.jpg *.svg* *.pdf *.dot*
INSTALL_DIR = ../installed
sinclude ../build.mk
