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
				 std::vector<TString> branchNames
		);

	~anyVar_class(void);


private:
	TChain *data_chain; // pointer fixed in the constructor
	std::vector<TString> _branchNames; //fixed in the constructor, these are the branches with the variables to study
	ElectronCategory_class cutter; // this class provides the TCut for the selections given simple category names coded in the ElectronCategory_class header file

	TChain *ImportTree(TChain *chain, TString commonCut, std::set<TString>& branchList); ///< add to the chain the entry list with selected events, the returned pointer is the same as the one in input
	
	RooArgSet Vars; ///< argSet containing the RooRealVars of branches under study
	RooRealVar weight; ///< variable with the total event weight
public:
	// define a struct saving the infos:
	struct {
		double mode;
		double mean;
		double stdDev;
		double skewness;
		double curtosis;
		double sigmaEff30, sigmaEff68, sigmaEff90, sigmaEff95;
	};




	// options:
	bool _isDataUnbinned;    // to import in roodatahist


	TString imgFormat; // eps
	TString outDirFitResData;
	TString outDirImgData;




















public:

	TString GetDir(void);

	void Import(TString commonCut, TString eleID_, std::set<TString>& branchList);

	double GetEffectiveSigma(RooAbsData *dataset, float quant);

	// this method makes the fit on data and MC
	void Fit(TString region = "", bool doPlot = true);
	void Fit(TH1F *hist, bool isMC = true);

	void FitToy(TString region, int nToys, int nEvents = 10000, bool doPlot = true);


	void SaveFitPlot(TString fileName, bool isMC = true);
	void PlotInvMass(TString region);
	void PrintScale(std::ostream outScale);
	void PrintRes(std::ostream outRes);



	//--------------- class settings

private:


	TString energyVar_name;
	TH1* invMass_highBinning;


	//public:
	//to be private, public just for testing
	RooPlot *plot_data;
	RooPlot *plot_MC;
	float chi2_data, chi2_MC;
	float sigmaeff_data, sigmaeff_MC;
	std::map< float, float> sigmaeff_data_map, sigmaeff_MC_map;
private:
	//-------------------- Pointers to the objects
	//--------------- Z parameters to be declared depending on the signal pdf
	//  RooRealVar& Z_deltaM; // parametri da prendere dai fit
	//  RooRealVar& Z_width;  // parametri da prendere dai fit


	RooArgSet* params;

	TChain *data;
	TChain *signal;




	std::vector<TString> region_map;
	std::vector<std::pair<double, double> > deltaM_data_map;
	std::vector<std::pair<double, double> > width_data_map;

	std::vector<std::pair<double, double> > deltaM_MC_map;
	std::vector<std::pair<double, double> > width_MC_map;

	TChain *ImportTree(TChain *chain, TString commonCut, std::set<TString>& branchList);
	RooDataSet *TreeToRooDataSet(TChain *chain, TEntryList *entryList); // import only invMass and weight
	RooDataSet *TreeToRooDataSet(TChain *chain, TCut cut); // import only invMass and weight
	//  RooDataSet *ImportTree(TChain *chain, int eleID, bool odd=false);
	RooDataHist *ImportHist(TH1F *hist);

	TCut GetCut(TString region, bool RooFit = true);
	RooAbsData *ReduceDataset(TChain *data, TString region, bool isMC, bool isUnbinned);
	void SetFitPar(RooFitResult *fitres_MC = NULL);
	void PlotFit(RooAbsData *signal_red, RooAbsData *data_red);
	void PlotFit(RooAbsData *data_red, bool isMC = true);

	void SaveFitRes(RooFitResult *fitres, TString fileName, float chi2, double nEvents, std::map<float, float> sigmaeff);

	//  std::map<TString, TH1F *> MakeHistMap(const chain_map_t& map, TString region);

	RooFitResult *FitData(TString region, bool doPlot = true, RooFitResult *fitres_MC = NULL);
	RooFitResult *FitMC(TString region, bool doPlot = true);


	//  int _signal_pdf_index, _bkg_pdf_index;
	void SetInitParamsfromRead(RooArgSet* pars);
	TTree* InitTree();

	TEntryList *commonMC, *commonData, *reducedMC, *reducedData;
};

#endif

