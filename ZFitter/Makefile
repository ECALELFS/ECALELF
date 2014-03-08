OPT=-Wall
ROOT_LIB=`root-config --libs --glibs`
ROOT_FLAGS=`root-config --cflags --ldflags`
ROOT_INCLUDE=`root-config --incdir`


# Boost
BOOST = /afs/cern.ch/cms/slc5_amd64_gcc434/external/boost/1.47.0
#BOOST=/afs/cern.ch/cms/slc5_amd64_gcc472/external/boost/1.50.0
SCRAMTOOL=$(shell scram || echo 1)
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

#ROOFIT_LIB=-lRooFitCore -lRooFit -L/afs/cern.ch/cms/slc5_amd64_gcc451/lcg/roofit/5.30.02-cms6/lib
#/afs/cern.ch/cms/slc5_amd64_gcc434/lcg/roofit/5.30.02-cms8/lib
#/afs/cern.ch/cms/slc5_amd64_gcc462/lcg/roofit/5.34.00-cms/lib
#/afs/cern.ch/cms/slc5_amd64_gcc462/lcg/roofit/5.32.00/lib
#ROOFIT_INCLUDE=/afs/cern.ch/cms/slc5_amd64_gcc451/lcg/roofit/5.30.02-cms6/include
#/afs/cern.ch/cms/slc5_amd64_gcc434/lcg/roofit/5.30.02-cms8/include
#/afs/cern.ch/cms/slc5_amd64_gcc462/lcg/roofit/5.34.00-cms/include


INCLUDEDIR=./interface
SRCDIR=./src
BUILDDIR=./bin
OBJ_DIR=./lib


#MODULES=hist_class
#LINKER=g++
INCLUDE=-I$(INCLUDEDIR) -I$(ROOT_INCLUDE)  -I../../myTools/functions/include -I$(ROOFIT_INCLUDE) -I$(BOOST)/include
#-I$(MYTOOL_DIR)/trigger_class/include/ -I$(MYTOOL_DIR)/json_class/include
LIB=-L$(BOOST)/lib -L/usr/lib64 # -L/usr/lib 

SRC=./src


default: ZFitter.exe #test #ZFit_class.o
all: 


#------------------------------ UTILITY

#------------------------------ MODULES
RooCruijff.o:  $(OBJ_DIR)/RooCruijff.o
$(OBJ_DIR)/RooCruijff.o: $(SRC)/RooCruijff.cc $(INCLUDEDIR)/RooCruijff.hh
	@echo "-> Making RooAbsPdf: RooCruijff"
	@g++ $(OPT) $(INCLUDE) -c -o $(OBJ_DIR)/RooCruijff.o $(SRC)/RooCruijff.cc

BW_CB_pdf_class.o:  $(OBJ_DIR)/BW_CB_pdf_class.o 
$(OBJ_DIR)/BW_CB_pdf_class.o: $(SRC)/BW_CB_pdf_class.cc $(INCLUDEDIR)/BW_CB_pdf_class.hh
	@echo "--> Making PDF class: BW_CB_pdf_class"
	@g++ $(OPT) $(INCLUDE) -c -o $(OBJ_DIR)/BW_CB_pdf_class.o $(SRC)/BW_CB_pdf_class.cc

Cruijff_pdf_class.o:  $(OBJ_DIR)/Cruijff_pdf_class.o 
$(OBJ_DIR)/Cruijff_pdf_class.o: $(SRC)/Cruijff_pdf_class.cc $(INCLUDEDIR)/Cruijff_pdf_class.hh $(OBJ_DIR)/RooCruijff.o
	@echo "--> Making PDF class: Cruijff_pdf_class"
	@g++ $(OPT) $(INCLUDE) -c -o $(OBJ_DIR)/Cruijff_pdf_class.o $(SRC)/Cruijff_pdf_class.cc 

ElectronCategory_class.o:  $(OBJ_DIR)/ElectronCategory_class.o 
$(OBJ_DIR)/ElectronCategory_class.o: $(SRC)/ElectronCategory_class.cc $(INCLUDEDIR)/ElectronCategory_class.hh $(OBJ_DIR)/RooCruijff.o
	@echo "--> Making class: ElectronCategory_class"
	@g++ $(OPT) $(INCLUDE) -c -o $(OBJ_DIR)/ElectronCategory_class.o $(SRC)/ElectronCategory_class.cc 

puWeights_class.o: $(OBJ_DIR)/puWeights_class.o
$(OBJ_DIR)/puWeights_class.o: $(SRC)/puWeights_class.cc $(INCLUDEDIR)/puWeights_class.hh
	@echo "--> Making class: puWeights_class"
	@g++ $(OPT) $(INCLUDE) -c -o $(OBJ_DIR)/puWeights_class.o $(SRC)/puWeights_class.cc 

r9Weights_class.o: $(OBJ_DIR)/r9Weights_class.o
$(OBJ_DIR)/r9Weights_class.o: $(SRC)/r9Weights_class.cc $(INCLUDEDIR)/r9Weights_class.hh
	@echo "--> Making class: r9Weights_class"
	@g++ $(OPT) $(INCLUDE) -c -o $(OBJ_DIR)/r9Weights_class.o $(SRC)/r9Weights_class.cc 

ZPtWeights_class.o: $(OBJ_DIR)/ZPtWeights_class.o
$(OBJ_DIR)/ZPtWeights_class.o: $(SRC)/ZPtWeights_class.cc $(INCLUDEDIR)/ZPtWeights_class.hh
	@echo "--> Making class: ZPtWeights_class"
	@g++ $(OPT) $(INCLUDE) -c -o $(OBJ_DIR)/ZPtWeights_class.o $(SRC)/ZPtWeights_class.cc 

runDivide_class.o: $(OBJ_DIR)/runDivide_class.o
$(OBJ_DIR)/runDivide_class.o: $(SRC)/runDivide_class.cc $(INCLUDEDIR)/runDivide_class.hh
	@g++ $(OPT) $(INCLUDE) -c -o $(OBJ_DIR)/runDivide_class.o $(SRC)/runDivide_class.cc

addBranch_class.o: $(OBJ_DIR)/addBranch_class.o
$(OBJ_DIR)/addBranch_class.o: $(SRC)/addBranch_class.cc $(INCLUDEDIR)/addBranch_class.hh
	@echo "-->  Making addBranch_class"
	@g++ $(OPT) $(INCLUDE) -c -o $(OBJ_DIR)/addBranch_class.o $(SRC)/addBranch_class.cc


EnergyScaleCorrection_class.o: $(OBJ_DIR)/EnergyScaleCorrection_class.o
$(OBJ_DIR)/EnergyScaleCorrection_class.o:  $(SRC)/EnergyScaleCorrection_class.cc $(INCLUDEDIR)/EnergyScaleCorrection_class.h
	@g++ $(OPT) $(INCLUDE) -c -o $(OBJ_DIR)/EnergyScaleCorrection_class.o $(SRC)/EnergyScaleCorrection_class.cc 

$(OBJ_DIR)/setTDRStyle.o: $(SRC)/setTDRStyle.C $(INCLUDEDIR)/setTDRStyle.hh
	@g++ $(OPT) $(INCLUDE) -c -o $(OBJ_DIR)/setTDRStyle.o $(SRC)/setTDRStyle.C 

SmearingImporter.o: $(OBJ_DIR)/SmearingImporter.o
smearing: $(OBJ_DIR)/SmearingImporter.o $(OBJ_DIR)/RooSmearer.o

$(OBJ_DIR)/SmearingImporter.o: $(SRC)/SmearingImporter.cc $(INCLUDEDIR)/SmearingImporter.hh
	@echo "--> Making class: SmearingImporter"
	@g++ $(OPT) $(INCLUDE) -c -o $(OBJ_DIR)/SmearingImporter.o $(SRC)/SmearingImporter.cc

$(OBJ_DIR)/RooSmearer.o: $(SRC)/RooSmearer.cc $(INCLUDEDIR)/RooSmearer.hh $(INCLUDEDIR)/SmearingImporter.hh
	@echo "--> Making class: RooSmearer"
	@g++ $(OPT) $(INCLUDE) -c -o $(OBJ_DIR)/RooSmearer.o $(SRC)/RooSmearer.cc 

proposal: $(OBJ_DIR)/TestProposal.o
$(OBJ_DIR)/TestProposal.o: $(SRC)/TestProposal.cc $(INCLUDEDIR)/TestProposal.h 
	@echo "--> Making class: TestProposal"
	@g++ $(OPT) $(INCLUDE) -c -o $(OBJ_DIR)/TestProposal.o $(SRC)/TestProposal.cc



#------------------------------ PRINCIPLE CLASS
ZFit_class.o: $(OBJ_DIR)/ZFit_class.o
$(OBJ_DIR)/ZFit_class.o: $(SRC)/ZFit_class.cc $(INCLUDEDIR)/ZFit_class.hh $(OBJ_DIR)/BW_CB_pdf_class.o $(OBJ_DIR)/Cruijff_pdf_class.o $(OBJ_DIR)/setTDRStyle.o
	@echo "---> Making ZFit_class"
	@g++ $(OPT) $(INCLUDE) -c -o $(OBJ_DIR)/ZFit_class.o $(SRC)/ZFit_class.cc


#------------------------------ PROGRAMS

test: $(BUILDDIR)/test.exe

ciao: 
	@echo "$(ROOFIT_LIB)"
	@echo "$(ROOFIT_INCLUDE)"

# $(BUILDDIR)/test.exe: $(BUILDDIR)/ZFitter.cpp $(OBJ_DIR)/ZFit_class.o $(OBJ_DIR)/ElectronCategory_class.o EnergyScaleCorrection_class.o $(OBJ_DIR)/puWeights_class.o $(OBJ_DIR)/runDivide_class.o  $(OBJ_DIR)/r9Weights_class.o $(OBJ_DIR)/addBranch_class.o $(OBJ_DIR)/RooSmearer.o $(OBJ_DIR)/SmearingImporter.o 
# 	@echo "---> Making Test ZFitter"
# 	@g++ $(OPT) $(INCLUDE) $(LIB) $(ROOT_LIB) $(ROOFIT_LIB) $(ROOSTAT_LIB) $(ROOT_FLAGS) \
# 	-o $(BUILDDIR)/test.exe $(BUILDDIR)/ZFitter.cpp  -lboost_program_options -lTreePlayer \
# 	$(OBJ_DIR)/*.o 

$(BUILDDIR)/test.exe: test/test.cpp $(OBJ_DIR)/EnergyScaleCorrection_class.o # quest'ultimo perche' compilo statico
	@echo "---> Making test"
	@g++ $(OPT) $(INCLUDE) $(LIB) $(ROOT_LIB) $(ROOFIT_LIB) $(ROOSTAT_LIB) \
	-o $(BUILDDIR)/test.exe test/test.cpp -lboost_program_options -lTreePlayer 	$(OBJ_DIR)/*.o 

ZFitter.exe: $(BUILDDIR)/ZFitter.exe


$(BUILDDIR)/ZFitter.exe: $(BUILDDIR)/ZFitter.cpp $(OBJ_DIR)/ZFit_class.o $(OBJ_DIR)/ElectronCategory_class.o EnergyScaleCorrection_class.o $(OBJ_DIR)/puWeights_class.o $(OBJ_DIR)/runDivide_class.o  $(OBJ_DIR)/r9Weights_class.o $(OBJ_DIR)/addBranch_class.o $(OBJ_DIR)/RooSmearer.o $(OBJ_DIR)/SmearingImporter.o $(OBJ_DIR)/ZPtWeights_class.o
	@echo "---> Making ZFitter"
	@g++ $(OPT) $(INCLUDE) $(LIB) $(ROOT_LIB) $(ROOFIT_LIB) $(ROOSTAT_LIB) $(ROOT_FLAGS) \
	-o $(BUILDDIR)/ZFitter.exe $(BUILDDIR)/ZFitter.cpp  -lboost_program_options -lTreePlayer \
	$(OBJ_DIR)/*.o 


clean:
	rm $(OBJ_DIR)/*.o
	rm $(BUILDDIR)/*.exe


