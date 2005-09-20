# Dear emacs, this is -*- makefile -*-
# Andre Rabello <Andre.dos.Anjos@cern.ch>
# $Id$

# Includes general configuration
sinclude config.mk

# Iterates over all packages and build each library/application

PROJECT_DIR	= sys data roiformat rbuild config network
GARB	= $(shell find . -name "*~" -or -name "*.o")

all: build progs

build:
	$(foreach dir, $(PROJECT_DIR), $(MAKE) -C $(dir) install;)

progs: ringer getroi filter merge xml2text mlp-train mlp-relevance eta-filter relevance-filter xml2dot mlp-run ringer-run

ringer: prog/ringer.o
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) $(PROJECT_DIR:%=-l%) -lpopt $< -o$(INSTALL_BIN)/$@

getroi: prog/getroi.o
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) $(PROJECT_DIR:%=-l%) $< -o$(INSTALL_BIN)/$@

merge: prog/merge.o
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) $(PROJECT_DIR:%=-l%) -lpopt $< -o$(INSTALL_BIN)/$@

filter: prog/filter.o
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) $(PROJECT_DIR:%=-l%) $< -o$(INSTALL_BIN)/$@

xml2text: prog/xml2text.o
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) $(PROJECT_DIR:%=-l%) -lpopt $< -o$(INSTALL_BIN)/$@

mlp-train: prog/mlp-train.o
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) $(PROJECT_DIR:%=-l%) -lpopt $< -o$(INSTALL_BIN)/$@

mlp-run: prog/mlp-run.o
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) $(PROJECT_DIR:%=-l%) -lpopt $< -o$(INSTALL_BIN)/$@

mlp-relevance: prog/mlp-relevance.o
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) $(PROJECT_DIR:%=-l%) -lpopt $< -o$(INSTALL_BIN)/$@

eta-filter: prog/eta-filter.o
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) $(PROJECT_DIR:%=-l%) -lpopt $< -o$(INSTALL_BIN)/$@

relevance-filter: prog/relevance-filter.o
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) $(PROJECT_DIR:%=-l%) -lpopt $< -o$(INSTALL_BIN)/$@

xml2dot: prog/xml2dot.o
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) $(PROJECT_DIR:%=-l%) $< -o$(INSTALL_BIN)/$@

ringer-run: prog/ringer-run.o
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) $(PROJECT_DIR:%=-l%) -lpopt $< -o$(INSTALL_BIN)/$@

doc:
	@sed -e 's/\([[:space:]]*PROJECT_NUMBER[[:space:]]*\=[[:space:]]*\)[0-9\.]*/\1$(VERSION)/g' Doxyfile > Doxyfile.updated
	@mv -f Doxyfile.updated Doxyfile
	@doxygen

install:
	$(foreach dir, $(PROJECT_DIR), $(MAKE) -C $(dir) install;)

.PHONY: clean clean-doc dist

edit: clean
	@emacs `find . -name "*.h" -or -name "*.cxx"` &

clean-doc:
	@rm -rf doxy-doc

clean:
	@rm -rf installed
	@rm -rf doxy-doc
	$(foreach dir, $(PROJECT_DIR), $(MAKE) -C $(dir) clean;)
ifeq ($(GARB),)
	@echo Your current directory is already clean.
else
	rm -rf $(GARB)
endif

dist: clean doc
	@mv doxy-doc ../$(PACKAGE)-doc-$(VERSION)
	@cd .. && \
	 tar cvfj $(PACKAGE)-doc-$(VERSION).tar.bz2 $(PACKAGE)-doc-$(VERSION) && \
	 rm -rf $(PACKAGE)-doc-$(VERSION) && cd -
	@cd .. && mv $(PACKAGE) $(PACKAGE)-$(VERSION) && \
	 tar cvfj $(PACKAGE)-$(VERSION).tar.bz2 $(PACKAGE)-$(VERSION) \
	 --exclude='.svn' && \
	 mv $(PACKAGE)-$(VERSION) $(PACKAGE) && cd -
	@echo $(PACKAGE) distribution and documentation packed

