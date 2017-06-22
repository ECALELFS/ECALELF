//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Jul  2 02:30:39 2011 by ROOT version 5.27/06b
// from TTree ntu/ntu
// found on file: /data1/dimatteo/Calibration/Ntuples/Run2011A/WZAnalysisSingleXtal/WZAnalysis_SingleElectron_Run2011A-WElectron-May10ReReco-v1.root
//////////////////////////////////////////////////////////

#ifndef XtalAlphaEB_h
#define XtalAlphaEB_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>
#include <TProfile.h>
#include <vector>
#include "Calibration/EOverPCalibration/interface/hChain.h"
#include "Calibration/EOverPCalibration/interface/h2Chain.h"
#include <TGraphErrors.h>

#include <TLorentzVector.h>

#include "Calibration/EOverPCalibration/interface/CalibrationUtils.h"
#include "Calibration/EOverPCalibration/interface/readJSONFile.h"

class XtalAlphaEB
{

public :

	std::vector<TGraphErrors*> myMomentumScale;
	std::string myTypeEB;

	TTree          *fChain;    //!pointer to the analyzed TTree or TChain
	Int_t           fCurrent;  //!current Tree number in a TChain

	///! Declaration of leaf types

	Int_t           runId;
	Int_t           lumiId;
	Int_t           eventId;
	Int_t           isW;
	Int_t           isZ;

	///! Ele 1 variables

	std::vector<float>   *ele1_recHit_E;
	std::vector<int>     *ele1_recHit_hashedIndex;
	std::vector<int>     *ele1_recHit_ietaORix;
	std::vector<int>     *ele1_recHit_iphiORiy;
	std::vector<int>     *ele1_recHit_flag;
	std::vector<float>   *ele1_recHit_LaserCorr;
	std::vector<float>   *ele1_recHit_Alpha;

	Float_t         ele1_charge;
	Float_t         ele1_eta;
	Float_t         ele1_phi;
	Float_t         ele1_scERaw;
	Float_t         ele1_scE;
	Float_t         ele1_es;
	Float_t         ele1_e3x3;
	Float_t         ele1_tkP;
	Float_t         ele1_fbrem;
	Float_t         ele1_EOverP;
	Int_t           ele1_isEB;
	Int_t           ele1_isEBEEGap;
	Int_t           ele1_isEBEtaGap;
	Int_t           ele1_isEBPhiGap;
	Int_t           ele1_isEEDeeGap;
	Int_t           ele1_isEERingGap;
	Float_t         ele1_E_true;
	Float_t         ele1_DR;
	Float_t         ele1_scE_regression;
	Float_t         ele1_scERaw_PUcleaned;
	Float_t         ele1_seedLaserAlpha;
	Float_t         ele1_seedLaserCorr;

	///! Ele 2 variables

	std::vector<float>   *ele2_recHit_E;
	std::vector<int>     *ele2_recHit_hashedIndex;
	std::vector<int>     *ele2_recHit_iphiORiy;
	std::vector<int>     *ele2_recHit_ietaORix;
	std::vector<int>     *ele2_recHit_flag;
	std::vector<float>   *ele2_recHit_LaserCorr;
	std::vector<float>   *ele2_recHit_Alpha;

	Float_t         ele2_charge;
	Float_t         ele2_eta;
	Float_t         ele2_phi;
	Float_t         ele2_scERaw;
	Float_t         ele2_scE;
	Float_t         ele2_es;
	Float_t         ele2_e3x3;
	Float_t         ele2_tkP;
	Float_t         ele2_fbrem;
	Float_t         ele2_EOverP;
	Int_t           ele2_isEB;
	Int_t           ele2_isEBEEGap;
	Int_t           ele2_isEBEtaGap;
	Int_t           ele2_isEBPhiGap;
	Int_t           ele2_isEEDeeGap;
	Int_t           ele2_isEERingGap;
	Float_t         ele2_E_true;
	Float_t         ele2_DR;
	Float_t         ele2_scE_regression;
	Float_t         ele2_scERaw_PUcleaned;
	Float_t         ele2_seedLaserAlpha;
	Float_t         ele2_seedLaserCorr;

	///! List of input branches from ntu

	TBranch        *b_runId;   //!
	TBranch        *b_lumiId;  //!
	TBranch        *b_eventId; //!
	TBranch        *b_isW;   //!
	TBranch        *b_isZ;   //!

	TBranch        *b_ele1_recHit_E;   //!
	TBranch        *b_ele1_recHit_hashedIndex;
	TBranch        *b_ele1_recHit_iphiORiy;
	TBranch        *b_ele1_recHit_ietaORix;
	TBranch        *b_ele1_recHit_flag;
	TBranch        *b_ele1_recHit_LaserCorr;
	TBranch        *b_ele1_recHit_Alpha;

	TBranch        *b_ele1_scERaw_PUcleaned;
	TBranch        *b_ele1_scE_regression;

	TBranch        *b_ele1_eta;   //!
	TBranch        *b_ele1_phi;   //!
	TBranch        *b_ele1_scERaw;   //!
	TBranch        *b_ele1_scE;   //!
	TBranch        *b_ele1_es;   //!
	TBranch        *b_ele1_e3x3;   //!
	TBranch        *b_ele1_tkP;   //!
	TBranch        *b_ele1_fbrem;   //!
	TBranch        *b_ele1_EOverP;   //!
	TBranch        *b_ele1_isEB;   //!
	TBranch        *b_ele1_E_true;   //!
	TBranch        *b_ele1_DR;   //!
	TBranch        *b_ele1_seedLaserAlpha;
	TBranch        *b_ele1_seedLaserCorr;
	TBranch        *b_ele1_charge;



	TBranch        *b_ele1_isEBEEGap;   //!
	TBranch        *b_ele1_isEBEtaGap;   //!
	TBranch        *b_ele1_isEBPhiGap;   //!
	TBranch        *b_ele1_isEEDeeGap;   //!
	TBranch        *b_ele1_isEERingGap;   //!

	TBranch        *b_ele2_recHit_E;   //!
	TBranch        *b_ele2_recHit_hashedIndex;
	TBranch        *b_ele2_recHit_iphiORiy;
	TBranch        *b_ele2_recHit_ietaORix;   //!
	TBranch        *b_ele2_recHit_flag;
	TBranch        *b_ele2_recHit_LaserCorr;
	TBranch        *b_ele2_recHit_Alpha;

	TBranch        *b_ele2_eta;   //!
	TBranch        *b_ele2_phi;   //!
	TBranch        *b_ele2_scERaw;   //!
	TBranch        *b_ele2_scE;   //!
	TBranch        *b_ele2_es;   //!
	TBranch        *b_ele2_e3x3;   //!
	TBranch        *b_ele2_tkP;   //!
	TBranch        *b_ele2_fbrem;   //!
	TBranch        *b_ele2_E_true;   //!
	TBranch        *b_ele2_DR;   //!

	TBranch        *b_ele2_scERaw_PUcleaned;
	TBranch        *b_ele2_scE_regression;

	TBranch        *b_ele2_EOverP;   //!
	TBranch        *b_ele2_isEB;   //!
	TBranch        *b_ele2_isEBEEGap;   //!
	TBranch        *b_ele2_isEBEtaGap;   //!
	TBranch        *b_ele2_isEBPhiGap;   //!
	TBranch        *b_ele2_isEEDeeGap;   //!
	TBranch        *b_ele2_isEERingGap;   //!
	TBranch        *b_ele2_seedLaserAlpha;
	TBranch        *b_ele2_seedLaserCorr;
	TBranch        *b_ele2_charge;

	///! List of class methods

	XtalAlphaEB(TTree *tree, std::vector<TGraphErrors*> & inputMomentumScale, const std::string& typeEB, TString outEPDistribution = "NULL");

	virtual ~XtalAlphaEB();

	virtual void     bookHistos(int);

	virtual void     saveHistos(TFile *f1);

	virtual Long64_t LoadTree(Long64_t entry);

	virtual void     Init(TTree *tree);

	virtual void     Loop(int, int, int, int, int, bool, bool, bool, bool, float, bool, std::map<int, std::vector<std::pair<int, int> > >);

	virtual void     BuildEoPeta_ele(int, int, int, int, std::vector<float>, bool, bool, float, bool);

	virtual void     saveEoPeta(TFile * f2);

	virtual void     AcquireDeadXtal(TString imputDeadXtal);

	virtual bool     CheckDeadXtal(const int & iEta, const int & iPhi);

	///! Output information

	std::vector<int>   IetaValues;
	std::vector<int>   IphiValues;
	std::vector<float> AlphaValues;
	std::vector<float> meanAlphaforPhiRingValues;
	std::vector<int>   DeadXtal_HashedIndex;


	hChain     *hC_EoP_eta_ele;
	hChain     *hC_AlphaValues;
	hChain     *hC_EoP;
	hChain     *hC_AlphaSpreadVsLoop;

	h2Chain    *hC_Alpha_EB;

	TH1F       *h_Occupancy_hashedIndex;
	TH2F       *h_occupancy;
	TProfile   *p_AlphaValues_iEta;
	TH2F       *h_Alpha_scalib_EB;
	TH2F       *h_Alpha_EB;
	TH2F       *h_Alpha_EB_meanOnPhi;
	TH1F       *h_Alpha_EB_hashedIndex;
	TH1F       *h_AlphaSpread_iEta;
	TH2F       *h_Intial_AlphaValues;

	TH2F       *h_map_Dead_Channels ;

	TGraphErrors *g_AlphameanVsLoop;
	TGraphErrors *g_AlpharmsVsLoop;
	TGraphErrors *g_AlphaSigmaVsLoop;

private:

	TString outEPDistribution_p;

};

#endif
