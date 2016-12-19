//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Jul  2 02:30:39 2011 by ROOT version 5.27/06b
// from TTree ntu/ntu
// found on file: /data1/dimatteo/Calibration/Ntuples/Run2011A/WZAnalysisSingleXtal/WZAnalysis_SingleElectron_Run2011A-WElectron-May10ReReco-v1.root
//////////////////////////////////////////////////////////

#ifndef FastCalibratorEE_h
#define FastCalibratorEE_h

/* #include <TROOT.h> */
#include <TChain.h>
/* #include <TFile.h> */
/* #include <TH1F.h> */
/* #include <TMath.h> */
/* #include <TProfile.h> */
/* #include <vector> */
#include "../interface/hChain.h"
#include "../interface/h2Chain.h"
#include <TGraphErrors.h>

/* #include <TLorentzVector.h> */
/* /\* #include "Math/PtEtaPhiE4D.h" *\/ */
/* /\* #include "Math/PtEtaPhiM4D.h" *\/ */
/* /\* #include "Math/LorentzVector.h" *\/ */

/* #include "../interface/CalibrationUtils.h" */
/* #include "../interface/readJSONFile.h" */
#include "../interface/TEndcapRings.h"

class FastCalibratorEE
{

public :

	///! Class methods
	FastCalibratorEE(TTree *tree, std::vector<TGraphErrors*> & inputMomentumScale, std::vector<TGraphErrors*> & inputEnergyScale, const std::string& typeEE, TString outEPDistribution = "NULL");

	virtual ~FastCalibratorEE();

	virtual void     bookHistos(int);

	virtual void     saveHistos(TFile *f1);

	virtual Int_t    GetEntry(Long64_t entry);

	virtual Long64_t LoadTree(Long64_t entry);

	virtual void     Init(TTree *tree);

	virtual void     FillScalibMap (TString miscalibMap);

	virtual void     Loop(int, int, int, int, int, bool, bool, int, bool, bool, bool, bool, float, float, int, bool, float, bool, float, bool, float, TString);
	virtual void     BuildEoPeta_ele(int, int, int, int, bool, bool, int, std::vector<float> , bool, bool, float, bool, float, bool, float, bool);

	virtual void     saveEoPeta(TFile * f2);

	virtual void     AcquireDeadXtal(TString imputDeadXtal, const bool & isDeadTriggerTower = false);

	virtual bool     CheckDeadXtal(const int & iX, const int & iY, const int & iZ);


	std::vector<TGraphErrors*> myMomentumScale;
	std::vector<TGraphErrors*> myEnergyScale;
	std::string myTypeEE;

	TTree          *fChain;   //!pointer to the analyzed TTree or TChain
	Int_t           fCurrent; //!current Tree number in a TChain


	///! Declaration of leaf types

	Int_t           runNumber;
	Int_t           lumiBlock;
	Int_t           eventNumber;
	Int_t           isW;
	Int_t           isZ;

	Int_t           chargeEle[3];
	Float_t         etaEle[3];
	Float_t         PtEle[3];
	Float_t         phiEle[3];
	Float_t         rawEnergySCEle[3];
	Float_t         energySCEle[3];
	Float_t         etaSCEle[3];
	Float_t         esEnergySCEle[3];
	Float_t         e3x3SCEle[3];
	Float_t         pAtVtxGsfEle[3];
	Float_t         fbremEle[3];
	Int_t           isEBEle[3];
	Float_t         energyMCEle[3];
	Float_t         etaMCEle[3];
	Float_t         phiMCEle[3];


	///! Ele 1 variables

	std::vector<float>     *energyRecHitSCEle1;
	//    std::vector<float>     *ele1_recHit_hashedIndex;
	std::vector<int>     *XRecHitSCEle1;
	std::vector<int>     *YRecHitSCEle1;
	std::vector<int>     *ZRecHitSCEle1;
	std::vector<int>       *recoFlagRecHitSCEle1;

	Int_t           ele1_charge;
	Float_t         ele1_eta;
	Float_t         ele1_pt;
	Float_t         ele1_phi;
	Float_t         ele1_scERaw;
	Float_t         ele1_scE;
	Float_t         ele1_scEta;
	Float_t         ele1_es;
	Float_t         ele1_e3x3;
	Float_t         ele1_tkP;
	Float_t         ele1_fbrem;
	Int_t           ele1_isEB;
	Float_t         ele1_E_true;
	Float_t         ele1_DR;

	///! Ele 2 variables

	std::vector<float>     *energyRecHitSCEle2;
	//    std::vector<float>     *ele1_recHit_hashedIndex;
	std::vector<int>     *XRecHitSCEle2;
	std::vector<int>     *YRecHitSCEle2;
	std::vector<int>     *ZRecHitSCEle2;
	std::vector<int>       *recoFlagRecHitSCEle2;

	Int_t         ele2_charge;
	Float_t         ele2_eta;
	Float_t         ele2_pt;
	Float_t         ele2_phi;
	Float_t         ele2_scERaw;
	Float_t         ele2_scE;
	Float_t         ele2_scEta;
	Float_t         ele2_es;
	Float_t         ele2_e3x3;
	Float_t         ele2_tkP;
	Float_t         ele2_fbrem;
	Int_t           ele2_isEB;
	Float_t         ele2_E_true;
	Float_t         ele2_DR;




	///! List of input branches from ntu

	TBranch        *b_runNumber;   //!
	TBranch        *b_lumiBlock;  //!
	TBranch        *b_eventNumber; //!
	TBranch        *b_isW;   //!
	TBranch        *b_isZ;   //!

	TBranch        *b_chargeEle;
	TBranch        *b_etaEle;
	TBranch        *b_PtEle;
	TBranch        *b_phiEle;
	TBranch        *b_rawEnergySCEle;
	TBranch        *b_energySCEle;
	TBranch        *b_etaSCEle;
	TBranch        *b_esEnergySCEle;
	TBranch        *b_e3x3SCEle;
	TBranch        *b_pAtVtxGsfEle;
	TBranch        *b_fbremEle;
	TBranch        *b_isEBEle;
	TBranch        *b_energyMCEle;
	TBranch        *b_etaMCEle;
	TBranch        *b_phiMCEle;

	TBranch        *b_energyRecHitSCEle1;   //!
	TBranch        *b_XRecHitSCEle1;
	TBranch        *b_YRecHitSCEle1;
	TBranch        *b_ZRecHitSCEle1;
	TBranch        *b_recoFlagRecHitSCEle1;

	TBranch        *b_energyRecHitSCEle2;   //!
	TBranch        *b_XRecHitSCEle2;
	TBranch        *b_YRecHitSCEle2;
	TBranch        *b_ZRecHitSCEle2;
	TBranch        *b_recoFlagRecHitSCEle2;


	/// Output informations
	hChain     *hC_EoP_ir_ele;

	TH1F       *h_scale_hashedIndex_EE;
	TH1F       *h_occupancy_hashedIndex_EE;
	hChain     *hC_EoP;

	/// EE+
	hChain     *hC_IntercalibValues_EEP;
	hChain     *hC_PullFromScalib_EEP;
	h2Chain    *hC_scale_EEP;


	TH2F       *h_occupancy_EEP;
	TH2F       *h_scale_EEP;
	TH2F       *h_scalib_EEP;
	TH2F       *h_scale_meanOnring_EEP;

	TGraphErrors *g_ICmeanVsLoop_EEP;
	TGraphErrors *g_ICrmsVsLoop_EEP;

	std::vector<int>   IxValues_EEP;
	std::vector<int>   IyValues_EEP;
	std::vector<float> ICValues_EEP;

	std::vector<float> SumIC_Ring_EEP;
	std::vector<int> Sumxtal_Ring_EEP;


	/// EE-
	hChain     *hC_IntercalibValues_EEM;
	hChain     *hC_PullFromScalib_EEM;
	h2Chain    *hC_scale_EEM;

	TH2F       *h_occupancy_EEM;
	TH2F       *h_scale_EEM;
	TH2F       *h_scalib_EEM;
	TH2F       *h_scale_meanOnring_EEM;

	TGraphErrors *g_ICmeanVsLoop_EEM;
	TGraphErrors *g_ICrmsVsLoop_EEM;

	std::vector<int>   IxValues_EEM;
	std::vector<int>   IyValues_EEM;
	std::vector<float> ICValues_EEM;

	std::vector<float> SumIC_Ring_EEM;
	std::vector<int> Sumxtal_Ring_EEM;

	/// Dead Channel infos
	std::vector<int> DeadXtal_HashedIndex;

	TH2F       *h_map_Dead_Channels_EEP ;
	TH2F       *h_map_Dead_Channels_EEM ;

	std::map<int, float> scalibMap;


private :

	TString outEPDistribution_p;

	/// Essential values to get EE geometry
	TEndcapRings* eRings;

	static const int IX_MIN = 1;
	static const int IY_MIN = 1;
	static const int IX_MAX = 100;
	static const int IY_MAX = 100;
	static const int kEEhalf = 7324;

	static const int kxf[200];

	static const int kdi[200];
};

#endif
