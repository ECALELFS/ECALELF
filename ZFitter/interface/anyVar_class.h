// classe per i fit alla Z
#ifndef anyVar_class_hh
#define anyVar_class_hh

/*
 * FIXME mettere i throw exception e sistemare gli errori
 */

#include <iostream>
#include <stdlib.h>
// root headers
#include <TChain.h>
#include <TString.h>
#include <TCut.h>
#include <TCanvas.h>
#include <TAttText.h>
#include <TH1F.h>
#include <TDirectory.h>
#include <THStack.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TDirectory.h>
// roofit headers
#include <RooRealVar.h>
#include <RooDataSet.h>
#include <RooDataHist.h>
#include <RooFitResult.h>
#include <RooPlot.h>
#include <TMath.h>
#include <TString.h>
// needed for TEntryList fix
#include <TEntryList.h>
#include <TChainElement.h>

//*********************************
#include "ElectronCategory_class.hh"

//#include <functions.h>



#include "TECALChain.h"
class anyVar_class
{
public:


	anyVar_class(TChain *data_chain_,
	             std::vector<TString> branchNames, ElectronCategory_class& cutter
	            );

	~anyVar_class(void);
	void Import(TString commonCut, TString eleID_, std::set<TString>& branchList); ///< to be called in the main
	RooDataSet *TreeToRooDataSet(TChain *chain, TCut cut); ///< returns a RooDataset with selected events and weight

private:
	TChain *data_chain; // pointer fixed in the constructor
	std::vector<TString> _branchNames; //fixed in the constructor, these are the branches with the variables to study
	ElectronCategory_class _cutter; // this class provides the TCut for the selections given simple category names coded in the ElectronCategory_class header file

	RooArgSet Vars; ///< argSet containing the RooRealVars of branches under study
	RooRealVar weight; ///< variable with the total event weight

	TEntryList *commonData, *reducedData;



	TChain *ImportTree(TChain *chain, TCut commonCut, std::set<TString>& branchList); ///< add to the chain the entry list with selected events, the returned pointer is the same as the one in input


	void FillStat(RooDataSet *dataset); ///< fills the "stats" struct


public:
	// define a struct saving the infos:
	struct stats {
		double mode;
		double mean;
		double stdDev;
		double skewness;
		double curtosis;
		double sigmaEff30, sigmaEff68, sigmaEff90, sigmaEff95;
	};



	double GetEffectiveSigma(RooAbsData *dataset, float quant);

	void PrintStats(std::ostream outScale);




};

#endif

