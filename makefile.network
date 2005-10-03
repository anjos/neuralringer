# Dear emacs, this is -*- makefile -*-
# Andre Rabello <Andre.dos.Anjos@cern.ch>

sinclude ../config.mk
DEPEND = sys data config
ifeq ($(XML_BACK_END),xerces)
 CXXFLAGS += -I${XERCESCROOT}/include -DXERCES_XML_BACK_END
 LDFLAGS += -lxerces-c
else
 CXXFLAGS += $(shell xml2-config --cflags)
 LDFLAGS += $(shell xml2-config --libs)
endif
GARB += *.ps *.ps2 *.png *.gif *.jpg *.svg* *.pdf *.dot*
INSTALL_DIR = ../installed
sinclude ../build.mk
