include MakeVars

#### Make the list of modules from the list of .cc files in the SRC directory
MODULES=$(shell ls $(SRCDIR)/*.cc | sed "s|.cc|.o|;s|$(SRCDIR)|$(OBJ_DIR)|g")
MODULES_EXT=-L$(CMSSW_BASE)/lib/$(SCRAM_ARCH)/ -L$(CMSSW_RELEASE_BASE)/lib/$(SCRAM_ARCH)/

-include $(CMSSW_BASE)/.SCRAM/$(SCRAM_ARCH)/MakeData/Tools/cmssw.mk
ifdef cmssw_EX_LIBDIR
MODULES_EXT+=-L$(cmssw_EX_LIBDIR)
endif

MODULESEoP=$(shell ls $(EoPDir)/$(SRCDIR)/*.cc | sed "s|.cc|.o|;s|$(SRCDIR)|$(OBJ_DIR)|g")
#### Make the list of dependencies for a particular module

default: $(MODULES) ZFitter.exe

#------------------------------ MODULES (static libraries)

MAKEDEPEND = -MMD  -MT '$@ lib/$*.d'

# $<: first prerequisite -> put always the .cc as first 
#### General rule for classes (libraries) compiled statically
### Generate also a .d file with prerequisites
lib/%.o: $(SRCDIR)/%.cc
	@echo "--> Making $@"
	@if [ "$(SCRAMTOOL)" == "1" ]; then $(COMPILE.cc) $(CXXFLAGS) $(INCLUDE) $(MAKEDEPEND) -o $@ $< -L$(MODULES_EXT); \
	else $(COMPILE.cc) -DCMSSW $(CXXFLAGS) $(INCLUDE) $(MAKEDEPEND) -o $@ $< -L$(MODULES_EXT); fi

-include $(MODULES:.o=.d)


# $(OBJ_DIR)/setTDRStyle.o: $(SRC)/setTDRStyle.C $(INCLUDEDIR)/setTDRStyle.h
# 	@g++ $(OPT) $(INCLUDE) -c -o $(OBJ_DIR)/setTDRStyle.o $(SRC)/setTDRStyle.C 


###### Main program
ZFitter.exe: $(BUILDDIR)/ZFitter.exe
$(BUILDDIR)/ZFitter.exe:  $(BUILDDIR)/ZFitter.cpp $(MODULES)
	cd $(EoPDir) && $(MAKE)
	@echo "---> Making ZFitter $(COMPILE.exe)"
	@if [ "$(SCRAMTOOL)" == "1" ]; then g++ $(CXXFLAGS) $(INCLUDE) $(MAKEDEPEND) -o $@ $< $(MODULES)  $(MODULESEoP) $(LIB) $(ROOT_LIB) $(ROOFIT_LIB) $(ROOSTAT_LIB) $(ROOT_FLAGS) \
	-lboost_program_options -lTreePlayer; else g++ $(CXXFLAGS) $(INCLUDE) $(MAKEDEPEND) -o $@ $< $(MODULES) $(MODULES_EXT) -lFWCoreParameterSet $(MODULESEoP) $(LIB) $(ROOT_LIB) $(ROOFIT_LIB) $(ROOSTAT_LIB) $(ROOT_FLAGS) \
	-lboost_program_options -lTreePlayer ; fi

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(OBJ_DIR)/*.d
	rm -f $(BUILDDIR)/*.exe
	cd $(EoPDir); make clean


