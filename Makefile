# Dear emacs, this is -*- makefile -*-
# André Rabello <Andre.dos.Anjos@cern.ch>
# $Id$

# Includes general configuration
sinclude config.mk

# Iterates over all packages and build each library/application

PROJECT_DIR	= sys data roiformat rbuild config network
GARB	= $(shell find . -name "*~" -or -name "*.o")

all: build

build:
	$(foreach dir, $(PROJECT_DIR), $(MAKE) -C $(dir) install;)

progs: ringer getroi filter merge xml2text mlp-train mlp-relevance xml2dot

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

mlp-relevance: prog/mlp-relevance.o
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) $(PROJECT_DIR:%=-l%) -lpopt $< -o$(INSTALL_BIN)/$@

xml2dot: prog/xml2dot.o
	$(CC) $(CXXFLAGS) -L$(INSTALL_LIB) $(PROJECT_DIR:%=-l%) $< -o$(INSTALL_BIN)/$@

doc:
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
	@mv doxy-doc ../ringer-doc-$(VERSION)
	@cd .. && \
	 tar cvfj ringer-doc-$(VERSION).tar.bz2 ringer-doc-$(VERSION) && \
	 rm -rf ringer-doc-$(VERSION) && cd -
	@cd .. && mv ringer ringer-$(VERSION) && \
	 tar cvfj ringer-$(VERSION).tar.bz2 ringer-$(VERSION) \
	 --exclude='.svn' && \
	 mv ringer-$(VERSION) ringer && cd -
	@echo Ringer distribution and documentation packed

