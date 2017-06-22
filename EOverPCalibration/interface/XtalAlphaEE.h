//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Jul  2 02:30:39 2011 by ROOT version 5.27/06b
// from TTree ntu/ntu
// found on file: /data1/dimatteo/Calibration/Ntuples/Run2011A/WZAnalysisSingleXtal/WZAnalysis_SingleElectron_Run2011A-WElectron-May10ReReco-v1.root
//////////////////////////////////////////////////////////

#ifndef XtalAlphaEE_h
#define XtalAlphaEE_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>
#include <TProfile.h>
#include <vector>
#include "Calibration/EOverPCalibration/interface/hChain.h"
#include "Calibration/EOverPCalibration/interface/h2Chain.h"
#include <TGraphErrors.h>


#include "Calibration/EOverPCalibration/interface/CalibrationUtils.h"
#include "Calibration/EOverPCalibration/interface/readJSONFile.h"
#include "Calibration/EOverPCalibration/interface/TEndcapRings.h"
#include "Calibration/EOverPCalibration/interface/TSicCrystals.h"

class XtalAlphaEE
{

public :

	std::vector<TGraphErrors*> myMomentumScale;
	std::string myTypeEE;

	TTree          *fChain;   //!pointer to the analyzed TTree or TChain
	Int_t           fCurrent; //!current Tree number in a TChain


	///! Declaration of leaf types
	Int_t           runId;
	Int_t           lumiId;
	Int_t           eventId;
	Int_t           isW;
	Int_t           isZ;

	///! Ele1 Variables
	std::vector<float>    *ele1_recHit_E;
	std::vector<int>      *ele1_recHit_hashedIndex;
	std::vector<int>      *ele1_recHit_ietaORix;
	std::vector<int>      *ele1_recHit_iphiORiy;
	std::vector<float>    *ele1_recHit_laserCorr;
	std::vector<float>    *ele1_recHit_Alpha;

	Float_t         ele1_charge;
	Float_t         ele1_eta;
	Float_t         ele1_phi;
	Float_t         ele1_scERaw;
	Float_t         ele1_scE;
	Float_t         ele1_scEta;
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

	///! Ele Variables
	std::vector<float>    *ele2_recHit_E;
	std::vector<int>      *ele2_recHit_hashedIndex;
	std::vector<int>      *ele2_recHit_iphiORiy;
	std::vector<int>      *ele2_recHit_ietaORix;
	std::vector<float>    *ele2_recHit_laserCorr;
	std::vector<float>    *ele2_recHit_Alpha;

	Float_t         ele2_charge;
	Float_t         ele2_eta;
	Float_t         ele2_phi;
	Float_t         ele2_scERaw;
	Float_t         ele2_scE;
	Float_t         ele2_scEta;
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


	///!  List of input branches from ntu
	TBranch        *b_runId;   //!
	TBranch        *b_lumiId;  //!
	TBranch        *b_eventId; //!
	TBranch        *b_isW;   //!
	TBranch        *b_isZ;   //!
	TBranch        *b_ele1_recHit_E;   //!
	TBranch        *b_ele1_recHit_iphiORiy;
	TBranch        *b_ele1_recHit_ietaORix;
	TBranch        *b_ele1_recHit_hashedIndex;   //!
	TBranch        *b_ele1_recHit_laserCorr;   //!
	TBranch        *b_ele1_recHit_Alpha;

	TBranch        *b_ele1_charge;   //!
	TBranch        *b_ele1_eta;   //!
	TBranch        *b_ele1_phi;   //!
	TBranch        *b_ele1_scERaw;   //!
	TBranch        *b_ele1_scE;   //!
	TBranch        *b_ele1_scEta;   //!
	TBranch        *b_ele1_es;   //!
	TBranch        *b_ele1_E_true;   //!
	TBranch        *b_ele1_DR;   //!
	TBranch        *b_ele1_e3x3;   //!
	TBranch        *b_ele1_tkP;   //!
	TBranch        *b_ele1_fbrem;   //!
	TBranch        *b_ele1_EOverP;   //!
	TBranch        *b_ele1_isEB;   //!
	TBranch        *b_ele1_scE_regression;
	TBranch        *b_ele1_scERaw_PUcleaned;

	TBranch        *b_ele1_seedLaserAlpha;
	TBranch        *b_ele1_seedLaserCorr;

	TBranch        *b_ele1_isEBEEGap;   //!
	TBranch        *b_ele1_isEBEtaGap;   //!
	TBranch        *b_ele1_isEBPhiGap;   //!
	TBranch        *b_ele1_isEEDeeGap;   //!
	TBranch        *b_ele1_isEERingGap;   //!

	TBranch        *b_ele2_recHit_E;   //!
	TBranch        *b_ele2_recHit_hashedIndex;   //!
	TBranch        *b_ele2_recHit_iphiORiy;
	TBranch        *b_ele2_recHit_ietaORix;   //!
	TBranch        *b_ele2_recHit_laserCorr;   //!
	TBranch        *b_ele2_recHit_Alpha;

	TBranch        *b_ele2_charge;   //!
	TBranch        *b_ele2_eta;   //!
	TBranch        *b_ele2_phi;   //!
	TBranch        *b_ele2_scERaw;   //!
	TBranch        *b_ele2_scE;   //!
	TBranch        *b_ele2_scEta;   //!
	TBranch        *b_ele2_es;   //!
	TBranch        *b_ele2_E_true;   //!
	TBranch        *b_ele2_DR;   //!
	TBranch        *b_ele2_e3x3;   //!
	TBranch        *b_ele2_tkP;   //!
	TBranch        *b_ele2_fbrem;   //!
	TBranch        *b_ele2_EOverP;   //!
	TBranch        *b_ele2_isEB;   //!
	TBranch        *b_ele2_isEBEEGap;   //!
	TBranch        *b_ele2_isEBEtaGap;   //!
	TBranch        *b_ele2_isEBPhiGap;   //!
	TBranch        *b_ele2_isEEDeeGap;   //!
	TBranch        *b_ele2_isEERingGap;   //!
	TBranch        *b_ele2_scE_regression;
	TBranch        *b_ele2_scERaw_PUcleaned;

	TBranch        *b_ele2_seedLaserAlpha;
	TBranch        *b_ele2_seedLaserCorr;

	///! Class methods
	XtalAlphaEE(TTree *tree, std::vector<TGraphErrors*> & inputMomentumScale, const std::string& typeEE, TString outEPDistribution = "NULL");

	virtual ~XtalAlphaEE();

	virtual void     bookHistos(int);

	virtual void     saveHistos(TFile *f1);

	virtual void     Init(TTree *tree);

	virtual Long64_t LoadTree(Long64_t entry);

	virtual void     Loop(int, int, int, int, int, bool, bool, bool, bool, float, bool, bool, std::map<int, std::vector<std::pair<int, int> > >);

	virtual void     BuildEoPeta_ele(int, int, int, int, std::vector<float>, bool, bool, float, bool, bool);

	virtual void     saveEoPeta(TFile * f2);

	virtual void     AcquireDeadXtal(TString imputDeadXtal);

	virtual bool     CheckDeadXtal(const int & iX, const int & iY, const int & iZ);

	/// Output informations
	TH1F       *h_occupancy_hashedIndex_EE;
	TH1F       *h_Alpha_hashedIndex_EE;

	hChain     *hC_EoP_ir_ele;
	hChain     *hC_EoP;

	/// EE+
	hChain     *hC_AlphaValues_BTCP_EEP;
	h2Chain    *hC_Alpha_BTCP_EEP;
	hChain     *hC_AlphaSpreadVsLoop_BTCP_EEP;

	hChain     *hC_AlphaValues_SIC_EEP;
	h2Chain    *hC_Alpha_SIC_EEP;
	hChain     *hC_AlphaSpreadVsLoop_SIC_EEP;

	TProfile   *p_AlphaVsIeta_BTCP_EEP;
	TH1F       *AlphaSpreadVsIeta_BTCP_EEP;
	TProfile   *p_AlphaVsIeta_SIC_EEP;
	TH1F       *AlphaSpreadVsIeta_SIC_EEP;

	TH2F       *h_occupancy_EEP;

	TH2F       *h_Alpha_EEP;
	TH2F       *h_scalib_EEP;
	TH2F       *h_Alpha_Original_EEP;


	TGraphErrors *g_AlphameanVsLoop_BTCP_EEP;
	TGraphErrors *g_AlpharmsVsLoop_BTCP_EEP;
	TGraphErrors *g_AlphaSigmaVsLoop_BTCP_EEP;
	TGraphErrors *g_AlphameanVsLoop_SIC_EEP;
	TGraphErrors *g_AlpharmsVsLoop_SIC_EEP;
	TGraphErrors *g_AlphaSigmaVsLoop_SIC_EEP;

	/// EE-
	hChain     *hC_AlphaValues_BTCP_EEM;
	h2Chain    *hC_Alpha_BTCP_EEM;
	hChain     *hC_AlphaSpreadVsLoop_BTCP_EEM;

	hChain     *hC_AlphaValues_SIC_EEM;
	h2Chain    *hC_Alpha_SIC_EEM;
	hChain     *hC_AlphaSpreadVsLoop_SIC_EEM;

	TProfile   *p_AlphaVsIeta_BTCP_EEM;
	TH1F       *AlphaSpreadVsIeta_BTCP_EEM;
	TProfile   *p_AlphaVsIeta_SIC_EEM;
	TH1F       *AlphaSpreadVsIeta_SIC_EEM;

	TH2F       *h_occupancy_EEM;

	TH2F       *h_Alpha_EEM;
	TH2F       *h_scalib_EEM;
	TH2F       *h_Alpha_Original_EEM;


	TGraphErrors *g_AlphameanVsLoop_BTCP_EEM;
	TGraphErrors *g_AlpharmsVsLoop_BTCP_EEM;
	TGraphErrors *g_AlphaSigmaVsLoop_BTCP_EEM;
	TGraphErrors *g_AlphameanVsLoop_SIC_EEM;
	TGraphErrors *g_AlpharmsVsLoop_SIC_EEM;
	TGraphErrors *g_AlphaSigmaVsLoop_SIC_EEM;


	/// Dead Channel infos
	std::vector<int> DeadXtal_HashedIndex;

	TH2F       *h_map_Dead_Channels_EEP ;
	TH2F       *h_map_Dead_Channels_EEM ;


private :

	TString outEPDistribution_p;

	/// Essential values to get EE geometry
	TEndcapRings* eRings;
	TSicCrystals* SicCrystal;

	static const int IX_MIN = 1;
	static const int IY_MIN = 1;
	static const int IX_MAX = 100;
	static const int IY_MAX = 100;
	static const int kEEhalf = 7324;

	static const int kxf[200];

	static const int kdi[200];
};

#endif
