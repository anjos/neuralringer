# Dear emacs, this is -*- makefile -*-
# André Rabello <Andre.dos.Anjos@cern.ch>

all: $(foreach dep,$(DEPEND),../$(dep)/lib$(dep).so) $(LIB)

$(LIB): $(OBJ)
	$(CC) $(CXXFLAGS) -shared -o $(LIB) $(OBJ) $(LDFLAGS)

test: $(TSTSRC:%.cxx=%)

mytest: $(MYTSTSRC:%.cxx=%)

install_lib: $(LIB)
	@[ -d $(INSTALL_LIB) ] || mkdir -pv $(INSTALL_LIB);
	@[ -e $(LIB) ] && install --mode=0755 -Cv $(LIB) $(INSTALL_LIB);
	@for tp in $(TSTSRC:src/%.cxx=%); do \
	  if [ -e $${tp} ]; then \
		install --mode=0755 -Cv $${tp} $(INSTALL_LIB); \
	  fi \
	done

install_bin:
	@[ -d $(INSTALL_BIN) ] || mkdir -pv $(INSTALL_BIN);

install_inc: $(shell find $(PACKAGE) -name "*.h")
	@[ -e $(INSTALL_INC)/$(PACKAGE) ] \
	 || mkdir -pv $(INSTALL_INC)/$(PACKAGE);
	@install --mode=0644 -Cv $? $(INSTALL_INC)/$(PACKAGE);

install_schema: $(SCHEMA)
	@[ -e $(INSTALL_SCHEMA) ] \
	         || mkdir -pv $(INSTALL_SCHEMA);
	@[ -z $? ] || install --mode=0644 -Cv $? $(INSTALL_SCHEMA);

install: install_lib install_bin install_inc install_schema

.PHONY: clean dep

dep: $(SRC) $(TSTSRC) $(MYTSTSRC)
	gcc -MM $(CPPFLAGS) $? > .depend

clean:
	@rm -vf $(GARB) $(LIB) .depend 
	@rm -vf $(TSTSRC:src/%.cxx=%) $(TSTSRC:%.cxx=%.o)
	@rm -vf $(MYTSTSRC:src/%.cxx=%) $(MYTSTSRC:%.cxx=%.o)

sinclude .depend
