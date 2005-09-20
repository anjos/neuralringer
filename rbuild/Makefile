# Dear emacs, this is -*- makefile -*-
# Andre Rabello <Andre.dos.Anjos@cern.ch>

sinclude ../config.mk
DEPEND = sys data roiformat
ifeq ($(XML_BACK_END),xerces)
 CXXFLAGS += -I${XERCESCROOT}/include -DXERCES_XML_BACK_END
 LDFLAGS += -lxerces-c
else
 CXXFLAGS += $(shell xml2-config --cflags)
 LDFLAGS += $(shell xml2-config --libs)
endif
SCHEMA += ./schema/ring.xsd
INSTALL_DIR=../$(INSTALL_NAME)
sinclude ../build.mk
