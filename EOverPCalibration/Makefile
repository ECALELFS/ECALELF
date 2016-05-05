include ../ZFitter/MakeVars

#### Make the list of modules from the list of .cc files in the SRC directory
MODULES=$(shell ls $(SRCDIR)/*.cc | sed "s|.cc|.o|;s|$(SRCDIR)|$(OBJ_DIR)|g")

#### Make the list of dependencies for a particular module

default: directories $(MODULES)

MAKEDEPEND = -MMD  -MT '$@ lib/$*.d'

# $<: first prerequisite -> put always the .cc as first 
#### General rule for classes (libraries) compiled statically
### Generate also a .d file with prerequisites
lib/%.o: $(SRCDIR)/%.cc
	@echo "--> Making $@" 
	@$(COMPILE.cc) $(CXXFLAGS) $(INCLUDE) $(MAKEDEPEND) -o $@ $<


-include $(MODULES:.o=.d)

#$(BUILDDIR)/ZFitter.cpp $(OBJ_DIR)/ZFit_class.o $(OBJ_DIR)/ElectronCategory_class.o EnergyScaleCorrection_class.o $(OBJ_DIR)/puWeights_class.o $(OBJ_DIR)/runDivide_class.o  $(OBJ_DIR)/r9Weights_class.o $(OBJ_DIR)/addBranch_class.o $(OBJ_DIR)/RooSmearer.o $(OBJ_DIR)/SmearingImporter.o $(OBJ_DIR)/ZPtWeights_class.o $(OBJ_DIR)/hChain.o $(OBJ_DIR)/h2Chain.o $(OBJ_DIR)/readJSONFile.o $(OBJ_DIR)/CalibrationUtils.o $(OBJ_DIR)/TEndcapRings.o $(OBJ_DIR)/GetHashedIndexEB.o $(OBJ_DIR)/GetHashedIndexEE.o $(OBJ_DIR)/FastCalibratorEB.o $(OBJ_DIR)/FastCalibratorEE.o

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(OBJ_DIR)/*.d
	rm -f $(BUILDDIR)/*.exe


directories: lib

lib:
	mkdir -p lib/


