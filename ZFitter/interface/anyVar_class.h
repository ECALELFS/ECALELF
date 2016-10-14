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

#include "Stats.hh"

//*********************************
#include "ElectronCategory_class.hh"

//#include <functions.h>
#define NELE 2


#include "TECALChain.h"
class anyVar_class
{
public:

	typedef enum { kInt_t = 1, kUInt_t,  kULong64_t,  kFloat_t,  kArrayTypes = 10,   ///< simple variable types
	               kAInt_t,    kAUInt_t, kAULong64_t, kAFloat_t, kMaxType
	             } kType;  ///< array types

	anyVar_class(TChain *data_chain_,
	             std::vector<std::pair<TString, kType> > branchNames, ElectronCategory_class& cutter,
	             std::string massBranchName = "invMass_SC_must_regrCorr_ele"
	            );

	~anyVar_class(void);
	void Import(TString commonCut, TString eleID_, std::set<TString>& branchList); ///< to be called in the main
	RooDataSet *TreeToRooDataSet(TChain *chain, TCut cut, int iEle = 0); ///< returns a RooDataset with selected events and weight
	void TreeAnalyzeShervin(std::string region, TCut cut_ele1, TCut cut_ele2); ///<
	void TreeToTree(TChain *chain, TCut cut); ///< skim the input TChain with selected events, copying only active branches
	void TreeToTreeShervin(TChain *chain, TCut cut); ///< skim the input TChain with selected events, copying only active branches

private:
	TChain *data_chain; // pointer fixed in the constructor
	TTree *reduced_data;
	TDirectory dir;
	std::vector<std::pair<TString, kType> > _branchNames; //fixed in the constructor, these are the branches with the variables to study
	ElectronCategory_class _cutter; // this class provides the TCut for the selections given simple category names coded in the ElectronCategory_class header file

	std::vector<std::ofstream> _statfiles; ///< one file for each branch, here the stats are saved
	statsCollection _stats_vec;


	RooArgSet Vars; ///< argSet containing the RooRealVars of branches under study

	std::string massBranchName_; ///< branch name for the di-object invariant mass

	Double_t weight; ///< variable with the total event weight

	RooRealVar * idx, * mass, * smearMass;


	TChain *ImportTree(TChain *chain, TCut commonCut, std::set<TString>& branchList); ///< add to the chain the entry list with selected events, the returned pointer is the same as the one in input


	void FillStat(RooDataSet *dataset); ///< fills the "stats" struct


public:
	// define a struct saving the infos:



	double GetEffectiveSigma(RooAbsData *dataset, float quant);

	void PrintStats(std::ostream outScale);
	bool _exclusiveCategories;



};

#endif

