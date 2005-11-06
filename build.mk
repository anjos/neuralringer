# Dear emacs, this is -*- makefile -*-
# Andre Rabello <Andre.dos.Anjos@cern.ch>

all: $(INSTALL_LIB)/$(LIB)

$(INSTALL_LIB)/$(LIB): $(OBJ) clean-lib
	@[ -d $(INSTALL_LIB) ] || mkdir -pv $(INSTALL_LIB)
	$(CC) $(CXXFLAGS) -shared -o $(INSTALL_LIB)/$(LIB) $(OBJ) $(LDFLAGS)

test: $(TSTSRC:%.cxx=%)

install_bin:
	@[ -d $(INSTALL_BIN) ] || mkdir -pv $(INSTALL_BIN)

.PHONY: clean clean-lib dep

dep: $(SRC) $(TSTSRC)
	gcc -MM $(CPPFLAGS) $? > .depend

clean:
	@rm -vf $(GARB) $(LIB) .depend 
	@rm -vf $(TSTSRC:src/%.cxx=%) $(TSTSRC:%.cxx=%.o)

clean-lib:
	@rm -rf $(INSTALL_LIB)/$(LIB)

sinclude .depend
