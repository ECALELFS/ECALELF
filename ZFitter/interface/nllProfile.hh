#include <TGraphErrors.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TString.h>
#include <TFile.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TCanvas.h>
#include <TH2F.h>
#include <TROOT.h>
#include <TStyle.h>
#include <RooRealVar.h>
#include "RooRealVar.h"
#include "RooMinuit.h"
#include "RooBinning.h"
#include "RooFitResult.h"

#include "TCanvas.h"
#include "TPostScript.h"
#include "TStopwatch.h"

#include "TMath.h"
#include "TMinuit.h"
#include "TCut.h"
#include <TList.h>
#include <TObject.h>
#include <TKey.h>
#include <TPaveText.h>
#include <TSystem.h>

#include <iostream>
#include <vector>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include <RooMinuit.h>
#include <RooStats/UniformProposal.h>
#include <RooStats/PdfProposal.h>
//#include <RooStats/SequentialProposal.h>
#include <RooGaussian.h>
#include <RooStats/MetropolisHastings.h>
#include "RooBinning.h"
//#include "../src/ShervinMinuit.cc"
#include <RooStats/ProposalHelper.h>
#include <RooMultiVarGaussian.h>

#include <TRandom3.h>

#include "../interface/RooSmearer.hh"


#define FIT_LIMIT 0.01
#define ITER_MAX 20
//#define DEBUG
#define MEM_DEBUG
#define PROFILE_NBINS 2000

using namespace RooStats;



//Get Profile after smearing
TGraph *GetProfile(RooRealVar *var, RooSmearer& compatibility, int level = -1, bool warningEnable = false, bool trueEval = true, float rho = 0, float Emean = 0, float phi = 0);
TGraph *GetProfile(RooRealVar *var1, RooRealVar *var2, RooSmearer& smearer, bool trueEval = true, double rho = 0, double Emean = 0, float phi = 0);
TGraph *GetProfile(RooRealVar *var, RooSmearer& compatibility, int level, bool warningEnable, bool trueEval, float rho, float Emean, float phi);
TTree *dataset2tree(RooDataSet *dataset);
TMatrixDSym* GetCovariance( RooStats::MarkovChain *chain, TString var1, TString var2);
bool stopFindMin1D(Int_t i, Int_t iLocMin, Double_t chi2, Double_t min, Double_t locmin, float phiMin = 2);
Int_t FindMin1D(RooRealVar *var, Double_t *X, Int_t N, Int_t iMinStart, Double_t min, RooSmearer& smearer, bool update = true, Double_t *Y = NULL, RooRealVar *var2 = NULL, Double_t *X2 = NULL);
void MinimizationProfile(RooSmearer& smearer, RooArgSet args, long unsigned int nIterMCMC, bool mcmc = false);

bool MinProfile(RooRealVar *var, RooSmearer& smearer, int iProfile,
                Double_t min_old, Double_t& min, double rho = 0, double Emean = 0, bool update = true, bool dscan = false);

bool MinProfile2D(RooRealVar *var1, RooRealVar *var2, RooSmearer& smearer, int iProfile,
                  Double_t min_old, Double_t& min, double& rho, double& Emean, bool update = true, bool dscan = false);



class ShervinMinuit: public PdfProposal
{

public:
	//inline ShervinMinuit(){};
	//ShervinMinuit(int nBurnSteps);
	ShervinMinuit(int nBurnSteps, RooSmearer& smearer);

	ProposalHelper *ph;
	ProposalFunction* proposal;
	void Propose(RooArgSet& xPrime, RooArgSet& x);
	inline Bool_t IsSymmetric(RooArgSet& xPrime, RooArgSet& x)
	{
		return true;
	}; //proposal->IsSymmetric(xPrime,x);};
	inline Double_t GetProposalDensity(RooArgSet& xPrime, RooArgSet& x)
	{
		return 0;
	}; //proposal->GetProposalDensity(xPrime, x);};

	inline void SetMinuit(RooMinuit& m)
	{
		_m = &m;
	};
private:
	int _nBurnSteps;
	int _iStep;
	int _iBurnStep;
	RooMinuit* _m;


	RooSmearer& _smearer;
	//RooArgSet& _params;
	RooArgSet _paramSet;
	RooStats::UniformProposal propUnif;
	RooMultiVarGaussian *gausPdf;
	TRandom3 gen;
	std::vector<TH1F*> rmsHist;
	float fSigmaRangeDivisor;
};

