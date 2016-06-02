// classe per i fit alla Z
#ifndef ZFit_class_hh
#define ZFit_class_hh

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
#include "ElectronCategory_class.hh"
#include "BW_CB_pdf_class.hh"
#include "Cruijff_pdf_class.hh"

#include "setTDRStyle.h"
//#include <functions.h>


typedef std::map<TString, TChain* > chain_map_t;
typedef std::map<TString, TString > config_map_t;
typedef std::map<TString, TH1F *> hist_map_t;

class ZFit_class
{
public:

	ZFit_class(TChain *data_chain_,
	           TChain *signal_chain_,
	           TChain *bkg_chain_,
	           //	     config_map_t config_map_,
	           TString invMass_VarName = "invMass_SC", double invMass_min = 65, double invMass_max = 115, double bin_width = 0.5
	                   //int eleID_=7, TString commonCut="Et_25", bool odd=false
	          );

//   ZFit_class(chain_map_t data_chain_map_,
// 	     chain_map_t signal_chain_map_,
// 	     chain_map_t bkg_chain_map_, //config_map_t config_map_,
// 	     TString invMass_VarName="invMass_SC", double invMass_min=65, double invMass_max=115, int eleID_=7);
	~ZFit_class(void);


public:
	// options:
	bool _isDataUnbinned;    // to import in roodatahist
	bool _isDataSumW2;
	bool _isMCUnbinned;    // to import in roodatahist
	bool _isMCSumW2;


	bool _isToyUnbinned;
	bool _isToySumW2;

	bool _forceNewFit; // to force to redo the fit also for already existing MC fits
	bool _initFitMC;   // true if data fit parameter initialized to MC
	bool _updateOnly;
	bool _oddMC, _oddData;


	//int eleID;
	//TString commonCut;
	//std::set<TString> branchList;
	TString imgFormat; // eps
	TString outDirFitResMC;
	TString outDirFitResData;
	TString outDirImgMC;
	TString outDirImgData;
	//  TString outDirTable;

public:
	// if you want to change the pdf
	void SetPDF_signal(int pdf_index = 0); // associate an index to the pdf type
	void SetPDF_model(int signal_pdf_index = 0, int bkg_pdf_index = 0); // associate an index to the pdf type
	inline  void SetFitType(int fit_type)
	{
		std::cout << "[INFO] Setting fit type to " << fit_type << std::endl;
		_fit_type = fit_type;
	};
	inline  void SetInitFileMC(TString initFileName)
	{
		_initFileNameMC = initFileName;
	};
	inline  void SetInitFileData(TString initFileName)
	{
		_initFileNameData = initFileName;
	};




	TString GetDir(void);
	TString GetEnergyVarName(TString name);
	//std::map<TString, float> ReadInitValuesFromFile(TString fileName);

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

	double GetLastDeltaM(bool isMC = true);
	double GetLastWidth(bool isMC = true);


	inline  float GetRescaledWidth(float width, float peakShift)
	{
		return width / (91.187 + peakShift);
	}
	float GetRescaledWidthError(float width, float width_error, float peakShift, float peakShift_error);

	TString MCfitresFile;

	//--------------- class settings

private:
	// set by SetPDF methods
	int signal_pdf_type;
	int bkg_pdf_type;

	// set by FitType()
	int _fit_type;

	TString _initFileNameMC, _initFileNameData;

	double nEvents_region_MC, nEvents_region_data;

	// this are for fitting data from trees with also signal+bkg models
	//  chain_map_t data_chain_map;
	//  chain_map_t signal_chain_map;
	//  chain_map_t bkg_chain_map;
	TChain *data_chain;
	TChain *signal_chain;
	TChain *bkg_chain;

	//  config_map_t config_map;

	RooRealVar invMass;
	TString energyVar_name;
	TH1* invMass_highBinning;
	BW_CB_pdf_class convBwCbPdf;
	Cruijff_pdf_class cruijffPdf;

	//--------------- Variables for import from Tree and for constructing RooDataSet
	RooRealVar weight;
	RooRealVar runNumber;
	//  RooRealVar lumiBlock("lumiBlock","lumiBlock", -1, 10000000 );
	RooRealVar nPV;
	//RooRealVar r9weight("r9weight","r9weight", 0,100);

	RooArgSet Vars;    // variabili

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

	RooAbsData *invMass_dataRed;


	RooAbsPdf *signal_pdf;
	RooAbsPdf *model_pdf;

	float toy_deltaM;
	float toy_width;
	float toy_gamma;
	float toy_alpha;
	float init_deltaM;
	float init_width;
	float init_gamma;
	float init_alpha;



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

	ElectronCategory_class cutter;

	//  int _signal_pdf_index, _bkg_pdf_index;
	void SetInitParamsfromRead(RooArgSet* pars);
	TTree* InitTree();

	TEntryList *commonMC, *commonData, *reducedMC, *reducedData;
};

#endif

