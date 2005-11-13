# Dear emacs, this is -*- makefile -*-
# Andre Rabello <Andre.dos.Anjos@cern.ch>
# $Id$

# Includes general configuration
sinclude config.mk

# Iterates over all packages and build each library/application

PKGS=sys data roiformat lvl1 rbuild config network
GARB=$(shell find . -name "*~" -or -name "*.o")
PROG_SRC=ringer getroi filter merge xml2text mlp-train mlp-relevance eta-filter relevance-filter xml2dot mlp-run ringer-run lvl1-filter

all: bin

lib: $(PKGS:%=$(INSTALL_LIB)/lib%.so)

bin: lib $(PROG_SRC:%=$(INSTALL_BIN)/%)

$(INSTALL_LIB)/lib%.so: PKG = $(@:$(INSTALL_LIB)/lib%.so=%)

$(INSTALL_LIB)/lib%.so: 
	$(MAKE) PKG=$(PKG) -f makefile.$(PKG) all

$(INSTALL_BIN)/ringer: prog/ringer.o
	@[ -d $(INSTALL_BIN) ] || mkdir -pv $(INSTALL_BIN)
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) -lrbuild $< -o$@

$(INSTALL_BIN)/getroi: prog/getroi.o
	@[ -d $(INSTALL_BIN) ] || mkdir -pv $(INSTALL_BIN)
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) -lroiformat -lpopt $< -o$@

$(INSTALL_BIN)/merge: prog/merge.o
	@[ -d $(INSTALL_BIN) ] || mkdir -pv $(INSTALL_BIN)
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) -ldata -lpopt $< -o$@

$(INSTALL_BIN)/filter: prog/filter.o
	@[ -d $(INSTALL_BIN) ] || mkdir -pv $(INSTALL_BIN)
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) -lroiformat -ldata $< -o$@

$(INSTALL_BIN)/xml2text: prog/xml2text.o
	@[ -d $(INSTALL_BIN) ] || mkdir -pv $(INSTALL_BIN)
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) -ldata -lpopt $< -o$@

$(INSTALL_BIN)/mlp-train: prog/mlp-train.o
	@[ -d $(INSTALL_BIN) ] || mkdir -pv $(INSTALL_BIN)
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) -lnetwork -lpopt $< -o$@

$(INSTALL_BIN)/mlp-run: prog/mlp-run.o
	@[ -d $(INSTALL_BIN) ] || mkdir -pv $(INSTALL_BIN)
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) -lnetwork -lpopt $< -o$@

$(INSTALL_BIN)/mlp-relevance: prog/mlp-relevance.o
	@[ -d $(INSTALL_BIN) ] || mkdir -pv $(INSTALL_BIN)
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) -lnetwork -lpopt $< -o$@

$(INSTALL_BIN)/eta-filter: prog/eta-filter.o
	@[ -d $(INSTALL_BIN) ] || mkdir -pv $(INSTALL_BIN)
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) -ldata -lpopt $< -o$@

$(INSTALL_BIN)/relevance-filter: prog/relevance-filter.o
	@[ -d $(INSTALL_BIN) ] || mkdir -pv $(INSTALL_BIN)
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) -ldata -lpopt $< -o$@

$(INSTALL_BIN)/xml2dot: prog/xml2dot.o
	@[ -d $(INSTALL_BIN) ] || mkdir -pv $(INSTALL_BIN)
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) -lnetwork $< -o$@

$(INSTALL_BIN)/ringer-run: prog/ringer-run.o
	@[ -d $(INSTALL_BIN) ] || mkdir -pv $(INSTALL_BIN)
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) -lnetwork -lrbuild $< -o$@

$(INSTALL_BIN)/lvl1-filter: prog/lvl1-filter.o
	@[ -d $(INSTALL_BIN) ] || mkdir -pv $(INSTALL_BIN)
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) -llvl1 -lroiformat -lsys $< -o$@

doc:
	@sed -e 's/\([[:space:]]*PROJECT_NUMBER[[:space:]]*\=[[:space:]]*\)[0-9\.]*/\1$(VERSION)/g' Doxyfile > Doxyfile.updated
	@mv -f Doxyfile.updated Doxyfile
	@doxygen

install:
	$(foreach PACKAGE, $(PACKAGES), $(MAKE) -f makefile.$(PACKAGE) install;)

.PHONY: clean clean-doc dist 

edit: clean
	@emacs `find . -name "*.h" -or -name "*.cxx"` &

clean-doc:
	@rm -rf doxy-doc

clean:
	@rm -rf $(INSTALL_LIB) $(INSTALL_BIN)
	@rm -rf doxy-doc
ifeq ($(GARB),)
	@echo Your current directory is already clean.
else
	rm -rf $(GARB)
endif

dist: clean clean-doc doc
	@mv doxy-doc ../$(PROJ)-doc-$(VERSION)
	@cd .. && \
	 tar cfj $(PROJ)-doc-$(VERSION).tar.bz2 $(PROJ)-doc-$(VERSION) && \
	 rm -rf $(PROJ)-doc-$(VERSION) && cd -
	@cd .. && mv $(PROJ) $(PROJ)-$(VERSION) && \
	 tar cfj $(PROJ)-$(VERSION).tar.bz2 --exclude='.svn' \
         $(PROJ)-$(VERSION) && \
	 mv $(PROJ)-$(VERSION) $(PROJ) && cd -
	@echo $(PROJ) distribution and documentation packed

