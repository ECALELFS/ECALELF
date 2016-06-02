include MakeVars

#### Make the list of modules from the list of .cc files in the SRC directory
MODULES=$(shell ls $(SRCDIR)/*.cc | sed "s|.cc|.o|;s|$(SRCDIR)|$(OBJ_DIR)|g")
MODULESEoP=$(shell ls $(EoPDir)/$(SRCDIR)/*.cc | sed "s|.cc|.o|;s|$(SRCDIR)|$(OBJ_DIR)|g")
#### Make the list of dependencies for a particular module

default: $(MODULES)  ZFitter.exe

#------------------------------ MODULES (static libraries)

MAKEDEPEND = -MMD  -MT '$@ lib/$*.d'

# $<: first prerequisite -> put always the .cc as first 
#### General rule for classes (libraries) compiled statically
### Generate also a .d file with prerequisites
lib/%.o: $(SRCDIR)/%.cc
	@echo "--> Making $@" 
	echo $(INCLUDE)
	@$(COMPILE.cc) $(CXXFLAGS) $(INCLUDE) $(MAKEDEPEND) -o $@ $<


-include $(MODULES:.o=.d)


# $(OBJ_DIR)/setTDRStyle.o: $(SRC)/setTDRStyle.C $(INCLUDEDIR)/setTDRStyle.h
# 	@g++ $(OPT) $(INCLUDE) -c -o $(OBJ_DIR)/setTDRStyle.o $(SRC)/setTDRStyle.C 


###### Main program
ZFitter.exe: $(BUILDDIR)/ZFitter.exe
$(BUILDDIR)/ZFitter.exe:  $(BUILDDIR)/ZFitter.cpp 
	cd $(EoPDir) && make
	@echo "---> Making ZFitter $(COMPILE.exe)"
	@g++ $(CXXFLAGS) $(INCLUDE) $(MAKEDEPEND) -o $@ $< $(MODULES) $(MODULESEoP) $(LIB) $(ROOT_LIB) $(ROOFIT_LIB) $(ROOSTAT_LIB) $(ROOT_FLAGS) \
	-lboost_program_options -lTreePlayer 

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(OBJ_DIR)/*.d
	rm -f $(BUILDDIR)/*.exe


