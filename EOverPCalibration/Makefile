CXXFLAGS=-Wall # -pedantic -ansi  #-std=c++0x

ROOT_LIB:=`root-config --libs --glibs`
ROOT_FLAGS:=`root-config --cflags --ldflags`
ROOT_INCLUDE:=`root-config --incdir`

# Boost
BOOST = /afs/cern.ch/cms/slc5_amd64_gcc434/external/boost/1.47.0
#BOOST=/afs/cern.ch/cms/slc5_amd64_gcc472/external/boost/1.50.0
SCRAMTOOL=$(shell which scram &>/dev/null || echo 1)

ifeq ($(SCRAMTOOL),1)
	ROOFIT_LIB="-lRooFit"
	ROOSTAT_LIB="-lRooStats"
	ROOFIT_INCLUDE="./"
else
	ROOFIT_INCLUDE := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep INCLUDE= | sed 's|INCLUDE=||')
	ROOFIT_LIB := -l$(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep LIB= | sed 's|LIB=||')
	ROOFIT_LIB += -l$(shell cd $(CMSSW_BASE); scram tool info roofit | grep LIB= | sed 's|LIB=||')
	ROOFIT_LIBDIR = -L$(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep LIBDIR= | sed 's|LIBDIR=||')
	ROOFIT_LIB+=$(ROOFIT_LIBDIR)
endif
ROOSTAT_LIB="-lRooStats"


INCLUDEDIR=./interface
SRCDIR=./src
BUILDDIR=./bin
OBJ_DIR=./lib

#################
INCLUDE=-I$(INCLUDEDIR) -isystem$(ROOT_INCLUDE)   -I$(ROOFIT_INCLUDE) -I$(BOOST)/include
LIB=-L$(BOOST)/lib -L/usr/lib64 # -L/usr/lib 


#### Make the list of modules from the list of .cc files in the SRC directory
MODULES=$(shell ls $(SRCDIR)/*.cc | sed "s|.cc|.o|;s|$(SRCDIR)|$(OBJ_DIR)|g")

#### Make the list of dependencies for a particular module

default: $(MODULES)

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


