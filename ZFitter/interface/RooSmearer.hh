#ifndef roosmearer_hh
#define roosmearer_hh

#include <iomanip>

#include <TChain.h>
#include <TH1F.h>
#include <TString.h>
#include <TRandom3.h>
#include <TMath.h>
#include <Math/Util.h>
#include <TStopwatch.h>

#include <RooAbsReal.h>
#include <RooSetProxy.h>
#include <RooStats/MarkovChain.h>
#include "SmearingImporter.hh"
//#define DEBUG
/*



*/

class ZeeCategory
{
public:
	inline  ZeeCategory()
	{
		hist_data = NULL;
		smearHist_data = NULL;
		hist_mc = NULL;
		smearHist_mc = NULL;
	};

	inline ~ZeeCategory()
	{
		//std::cout << "[STATUS] Destroy ZeeCategory: " << categoryName1 << "\t" << categoryName2 << std::endl;
		// do not delete data_events;
		// do not delete mc_events;
#ifdef DEBUG
		if(hist_data != NULL)      hist_data->Print();
		//    if(smearHist_data!=NULL) smearHist_data->Print();
		if(hist_mc != NULL)        hist_mc->Print();
		if(smearHist_mc != NULL)   smearHist_mc->Print();
#endif
//     if(hist_data!=NULL) delete hist_data;
//     //    if(smearHist_data!=NULL) delete smearHist_data;
//     if(hist_mc!=NULL) delete hist_mc;
//     if(smearHist_mc!=NULL) delete smearHist_mc;
		data_events = NULL;
		mc_events = NULL;
	};


	//SetBinning(double min, double max, double nBins); // fixed

public:
	zee_events_t *data_events;
	zee_events_t *mc_events;

	int categoryIndex1, categoryIndex2;
	TString categoryName1;
	TString categoryName2;
	RooArgSet pars1;
	RooArgSet pars2;

	// old values
	double scale1, constant1, alpha1;
	double scale2, constant2, alpha2;

	int nBins;
	float invMass_min;
	float invMass_max;

	TH1F *hist_data;
	TH1F *smearHist_data;

	TH1F *hist_mc;
	TH1F *smearHist_mc;

	bool active;
	unsigned int nSmearToy;
	unsigned int nLLtoy;

	double nll, nllRMS;
};

// parameters params are provided externally to leave flexibiility in
// their definition and dependencies (can be  RooRealVar or
// RooFormulaVar)

/** \class RooSmearer
 * \brief Minimize NLL to get energy scale and smearings
 * \author Paolo Meridiani and Shervin Nourbakhsh
 * From an original idea of Paolo Meridiani
 */

class RooSmearer: public RooAbsReal
{

public:
	~RooSmearer(void);

	inline RooSmearer() {};
	RooSmearer(const RooSmearer& c, const char *newname);
	virtual TObject* clone(const char* newname) const
	{
		return new RooSmearer(*this, newname);
	};
//   virtual ~RooSmearer();
//  inline double eval() const { std::cout << "##############################" << std::endl;return evaluate(); } ;
	Double_t evaluate(void) const;

// inline  RooSmearer(const char *name,TChain *data_chain_,
// 	     TChain *signal_chain_,
// 	     TChain *bkg_chain_,
// 	     std::vector<TString> regionList,
// 	     std::vector<RooArgSet > params,
// 	     TString energyBranchName="energySCEle_regrCorr_ele"
// 		   ){};
	RooSmearer(const char *name, TChain *data_chain_,
	           TChain *signal_chain_,
	           TChain *bkg_chain_,
	           std::vector<TString> regionList,
	           std::vector<RooArgSet > params,
	           RooArgSet parset,
	           TString energyBranchName = "energySCEle_regrCorr_ele"
	          );


	// Settting options
	void AutoNSmear(ZeeCategory& category);
	void AutoNBins(ZeeCategory& category);

	// interface functions to SmearingImporter
	inline void SetPuWeight(bool usePuReweight)
	{
		importer.SetPuWeight(usePuReweight);
	};
	inline void SetR9Weight(bool useR9Reweight)
	{
		importer.SetR9Weight(useR9Reweight);
	};
	inline void SetPtWeight(bool usePtReweight)
	{
		importer.SetPtWeight(usePtReweight);
	};
	inline void SetFsrWeight(bool value)
	{
		importer.SetFsrWeight(value);
	};
	inline void SetWeakWeight(bool value)
	{
		importer.SetWeakWeight(value);
	};
	inline void SetZPtWeight(bool useZPtReweight)
	{
		importer.SetZPtWeight(useZPtReweight);
	};
	inline void SetOnlyDiagonal(bool value)
	{
		importer.SetOnlyDiagonal(value);
	};
	inline void SetSmearingEt(bool value)
	{
		importer.SetSmearingEt(value);
	};
	inline void SetPdfSystWeight(int value)
	{
		importer.SetPdfSystWeight(value);
	};

	void SetNSmear(unsigned int n_smear = 0, unsigned int nlltoy = 0);

	inline void SetToyScale(float scaleToy = 1.01, float constTermToy = 0.01)
	{
		importer._scaleToy = scaleToy;
		importer._constTermToy = constTermToy;
	}
	inline void SetEleID(TString value)
	{
		importer.SetEleID(value);
	};
	inline void SetCommonCut(TString cut)
	{
		importer.SetCommonCut(cut);
	};

	inline void SetHistBinning(double min, double max, double width)
	{
		invMass_min_ = min;
		invMass_max_ = max;
		invMass_bin_ = width;
		nBins_ = (int) ((invMass_max_ - invMass_min_) / invMass_bin_);
		return;
	}

	/// Initialize the categories: import from the tree
	void Init(TString commonCut, TString eleID, Long64_t nEvents = 0, bool mcToy = false, bool externToy = true, TString initFile = "");
	//  TH1F *GetSmearedHisto(TString categoryName,
	//			bool smearEnergy=false, TString histoName="") const;
	//  TH1F *GetSmearedHisto(int categoryIndex,
	//			bool smearEnergy=false, TString histoName="") const;


	// category manipulation
	//  void EnableCategory(TString categoryName);
	//  void EnableCategory(int categoryIndex);
	//  void DisableCategory(TString categoryName);
	//  void DisableCategory(int categoryIndex);
	inline RooDataSet *GetMarkovChainAsDataSet()
	{
		return _markov.GetAsDataSet();
	};
	inline RooDataSet *SetDataSet(TString name = "profile", TString title = "", double nllMin_ = 0)
	{
		if(dataset != NULL) {
			std::cerr << "[WARNING] Removing last dataset: " << std::endl;
			dataset->Print();
			delete dataset;
		}
		//nllMin=nllMin_;
		dataset = new RooDataSet(name, title, RooArgSet(_paramSet, nllVar));
		return dataset;
	};
	inline RooDataSet *GetDataSet(void)
	{
		return dataset;
	}
private:
	TChain *_data_chain, *_signal_chain;
	SmearingImporter importer;
	std::vector<zee_events_t> mc_events_cache;
	std::vector<zee_events_t> data_events_cache;
	std::vector<RooArgSet> _params_vec; //, _truth_params_vec;

	RooSetProxy _paramSet;
	RooArgSet *truthSet, pullArgs;

	double invMass_min_;
	double invMass_max_;
	double invMass_bin_;
	int nBins_;

public:
	float deltaNLLMaxSmearToy;
	unsigned int _deactive_minEventsDiag;
	unsigned int _deactive_minEventsOffDiag;
	double nllMin;
	unsigned int _nSmearToy;
private:

	unsigned int _nLLtoy;
	TRandom3* rgen_;
	TStopwatch *myClock;

	double lastNLL;
	double lastNLLrms;
	double nllBase;
	RooRealVar nllVar;

public:
	bool _isDataSmeared;
	bool _autoBin;
	bool _autoNsmear;
	bool smearscan;
	RooDataSet *dataset;

	RooStats::MarkovChain _markov;
private:
	void SetCache(Long64_t nEvents = 0, bool cacheToy = false, bool externToy = true);
	void InitCategories(bool mcToy = false);

	//double smearedEnergy(float ene,float scale,float alpha,float
	//constant) const;
	double smearedEnergy(double *smear, unsigned int nGen, float ene, float scale, float alpha, float constant, const float *fixedSmearings = NULL) const;
	void SetSmearedHisto(const zee_events_t& cache,
	                     RooArgSet pars1, RooArgSet pars2,
	                     TString categoryName1, TString categoryName2, unsigned int nSmearToy,
	                     TH1F *hist) const;

	void SetHisto(const zee_events_t& cache, TH1F *hist) const;
	void SetAutoBin(ZeeCategory& category, double min, double max); // set using statistics
	void ResetBinning(ZeeCategory& category);
	bool isCategoryChanged(ZeeCategory& category, bool updateVar = true) const;


	double getLogLikelihood(TH1F* data, TH1F* prob) const;
	void UpdateCategoryNLL(ZeeCategory& cat, unsigned int nLLtoy, bool multiSmearToy = true);


	int Trag_eq(int row, int col, int N) const;

public:
	std::vector<ZeeCategory> ZeeCategories;
	typedef std::vector<ZeeCategory> ZeeCategoryCollection;

	TH1F *GetSmearedHisto(ZeeCategory& category, bool isMC,
	                      bool smearEnergy, bool forceNew = false, bool multiSmearToy = true);


	double getCompatibility(bool forceUpdate = false) const;
	void DumpNLL(void) const;
	//  float getCompatibility(const RooSmearer *ptr);
	inline RooArgSet GetParams(void)
	{
		return _paramSet;
	};

	inline double GetNllRMS()
	{
		return lastNLLrms;
	};
};

#endif
