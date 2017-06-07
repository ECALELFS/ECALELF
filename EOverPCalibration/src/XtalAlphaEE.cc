#include "Calibration/EOverPCalibration/interface/XtalAlphaEE.h"
#include "Calibration/EOverPCalibration/interface/GetHashedIndexEE.h"
#include <TH2.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <TRandom.h>


/// Default constructor
XtalAlphaEE::XtalAlphaEE(TTree *tree, std::vector<TGraphErrors*> & inputMomentumScale, const std::string& typeEE, TString outEPDistribution):
	outEPDistribution_p(outEPDistribution)
{

	if (tree == 0) {
		TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/data1/dimatteo/Calibration/Ntuples/Run2011A/WZAnalysisSingleXtal/WZAnalysis_SingleElectron_Run2011A-                                                            WElectron-May10ReReco-v1.root");
		if (!f) {
			f = new TFile("/data1/dimatteo/Calibration/Ntuples/Run2011A/WZAnalysisSingleXtal/WZAnalysis_SingleElectron_Run2011A-WElectron-May10ReReco-v1.root");
		}
		tree = (TTree*)gDirectory->Get("ntu");
	}

	// endcap geometry
	eRings = new TEndcapRings();
	SicCrystal = new TSicCrystals();

	Init(tree);

	// Set my momentum scale using the input graphs
	myMomentumScale = inputMomentumScale;
	myTypeEE = typeEE;
}

/// Deconstructor

XtalAlphaEE::~XtalAlphaEE()
{

	if (!fChain) return;
	delete fChain->GetCurrentFile();
}


Long64_t XtalAlphaEE::LoadTree(Long64_t entry)
{

	// Set the environment to read one entry
	if (!fChain) return -5;
	Long64_t centry = fChain->LoadTree(entry);
	if (centry < 0) return centry;
	if (!fChain->InheritsFrom(TChain::Class()))  return centry;

	TChain *chain = (TChain*)fChain;
	if (chain->GetTreeNumber() != fCurrent) {
		fCurrent = chain->GetTreeNumber();
	}
	return centry;
}


/// Intialize pointers
void XtalAlphaEE::Init(TTree *tree)
{

	/// Set object pointer

	ele1_recHit_E = 0;
	ele1_recHit_hashedIndex = 0;
	ele1_recHit_iphiORiy = 0;
	ele1_recHit_ietaORix = 0 ;
	ele1_recHit_laserCorr = 0 ;
	ele1_recHit_Alpha = 0 ;


	ele2_recHit_E = 0;
	ele2_recHit_hashedIndex = 0;
	ele2_recHit_iphiORiy = 0;
	ele2_recHit_ietaORix = 0;
	ele2_recHit_laserCorr = 0 ;
	ele2_recHit_Alpha = 0 ;

	/// Set branch addresses and branch pointers

	if (!tree) return;

	fChain = tree;
	fCurrent = -1;
	fChain->SetMakeClass(1);

	fChain->SetBranchStatus("*", 0);
	fChain->SetBranchStatus("runId", 1);
	fChain->SetBranchAddress("runId", &runId, &b_runId);
	fChain->SetBranchStatus("lumiId", 1);
	fChain->SetBranchAddress("lumiId", &lumiId, &b_lumiId);
	fChain->SetBranchStatus("eventId", 1);
	fChain->SetBranchAddress("eventId", &eventId, &b_eventId);
	fChain->SetBranchStatus("isW", 1);
	fChain->SetBranchAddress("isW", &isW, &b_isW);
	fChain->SetBranchStatus("isZ", 1);
	fChain->SetBranchAddress("isZ", &isZ, &b_isZ);

	// ele1
	fChain->SetBranchStatus("ele1_recHit_E", 1);
	fChain->SetBranchAddress("ele1_recHit_E", &ele1_recHit_E, &b_ele1_recHit_E);
	fChain->SetBranchStatus("ele1_recHit_hashedIndex", 1);
	fChain->SetBranchAddress("ele1_recHit_hashedIndex", &ele1_recHit_hashedIndex, &b_ele1_recHit_hashedIndex);
	fChain->SetBranchStatus("ele1_recHit_iphiORiy", 1);
	fChain->SetBranchAddress("ele1_recHit_iphiORiy", &ele1_recHit_iphiORiy, &b_ele1_recHit_iphiORiy);
	fChain->SetBranchStatus("ele1_recHit_ietaORix", 1);
	fChain->SetBranchAddress("ele1_recHit_ietaORix", &ele1_recHit_ietaORix, &b_ele1_recHit_ietaORix);
	fChain->SetBranchStatus("ele1_recHit_laserCorrection", 1);
	fChain->SetBranchAddress("ele1_recHit_laserCorrection", &ele1_recHit_laserCorr, &b_ele1_recHit_laserCorr);
	fChain->SetBranchStatus("ele1_recHit_Alpha", 1);
	fChain->SetBranchAddress("ele1_recHit_Alpha", &ele1_recHit_Alpha, &b_ele1_recHit_Alpha);

	fChain->SetBranchStatus("ele1_charge", 1);
	fChain->SetBranchAddress("ele1_charge", &ele1_charge, &b_ele1_charge);
	fChain->SetBranchStatus("ele1_eta", 1);
	fChain->SetBranchAddress("ele1_eta", &ele1_eta, &b_ele1_eta);
	fChain->SetBranchStatus("ele1_phi", 1);
	fChain->SetBranchAddress("ele1_phi", &ele1_phi, &b_ele1_phi);
	fChain->SetBranchStatus("ele1_scERaw", 1);
	fChain->SetBranchAddress("ele1_scERaw", &ele1_scERaw, &b_ele1_scERaw);
	fChain->SetBranchStatus("ele1_scE", 1);
	fChain->SetBranchAddress("ele1_scE", &ele1_scE, &b_ele1_scE);
	fChain->SetBranchStatus("ele1_scEta", 1);
	fChain->SetBranchAddress("ele1_scEta", &ele1_scEta, &b_ele1_scEta);
	fChain->SetBranchStatus("ele1_es", 1);
	fChain->SetBranchAddress("ele1_es", &ele1_es, &b_ele1_es);
	fChain->SetBranchStatus("ele1_e3x3", 1);
	fChain->SetBranchAddress("ele1_e3x3", &ele1_e3x3, &b_ele1_e3x3);
	fChain->SetBranchStatus("ele1_tkP", 1);
	fChain->SetBranchAddress("ele1_tkP", &ele1_tkP, &b_ele1_tkP);
	fChain->SetBranchStatus("ele1_fbrem", 1);
	fChain->SetBranchAddress("ele1_fbrem", &ele1_fbrem, &b_ele1_fbrem);
	fChain->SetBranchStatus("ele1_EOverP", 1);
	fChain->SetBranchAddress("ele1_EOverP", &ele1_EOverP, &b_ele1_EOverP);
	fChain->SetBranchStatus("ele1_isEB", 1);
	fChain->SetBranchAddress("ele1_isEB", &ele1_isEB, &b_ele1_isEB);
	fChain->SetBranchStatus("ele1_isEBEEGap", 1);
	fChain->SetBranchAddress("ele1_isEBEEGap", &ele1_isEBEEGap, &b_ele1_isEBEEGap);
	fChain->SetBranchStatus("ele1_scE_regression", 1);
	fChain->SetBranchAddress("ele1_scE_regression", &ele1_scE_regression, &b_ele1_scE_regression);

	fChain->SetBranchStatus("ele1_seedLaserAlpha", 1);
	fChain->SetBranchAddress("ele1_seedLaserAlpha", &ele1_seedLaserAlpha, &b_ele1_seedLaserAlpha);
	fChain->SetBranchStatus("ele1_seedLaserCorr", 1);
	fChain->SetBranchAddress("ele1_seedLaserCorr",  &ele1_seedLaserCorr,  &b_ele1_seedLaserCorr);

	fChain->SetBranchStatus("ele1_isEBEtaGap", 1);
	fChain->SetBranchAddress("ele1_isEBEtaGap", &ele1_isEBEtaGap, &b_ele1_isEBEtaGap);
	fChain->SetBranchStatus("ele1_isEBPhiGap", 1);
	fChain->SetBranchAddress("ele1_isEBPhiGap", &ele1_isEBPhiGap, &b_ele1_isEBPhiGap);
	fChain->SetBranchStatus("ele1_isEEDeeGap", 1);
	fChain->SetBranchAddress("ele1_isEEDeeGap", &ele1_isEEDeeGap, &b_ele1_isEEDeeGap);
	fChain->SetBranchStatus("ele1_isEERingGap", 1);
	fChain->SetBranchAddress("ele1_isEERingGap", &ele1_isEERingGap, &b_ele1_isEERingGap);

	// ele2
	fChain->SetBranchStatus("ele2_recHit_E", 1);
	fChain->SetBranchAddress("ele2_recHit_E", &ele2_recHit_E, &b_ele2_recHit_E);
	fChain->SetBranchStatus("ele2_recHit_hashedIndex", 1);
	fChain->SetBranchAddress("ele2_recHit_hashedIndex", &ele2_recHit_hashedIndex, &b_ele2_recHit_hashedIndex);
	fChain->SetBranchStatus("ele2_recHit_iphiORiy", 1);
	fChain->SetBranchAddress("ele2_recHit_iphiORiy", &ele2_recHit_iphiORiy, &b_ele2_recHit_iphiORiy);
	fChain->SetBranchStatus("ele2_recHit_ietaORix", 1);
	fChain->SetBranchAddress("ele2_recHit_ietaORix", &ele2_recHit_ietaORix, &b_ele2_recHit_ietaORix);
	fChain->SetBranchStatus("ele2_recHit_laserCorrection", 1);
	fChain->SetBranchAddress("ele2_recHit_laserCorrection", &ele2_recHit_laserCorr, &b_ele2_recHit_laserCorr);
	fChain->SetBranchStatus("ele2_recHit_Alpha", 1);
	fChain->SetBranchAddress("ele2_recHit_Alpha", &ele2_recHit_Alpha, &b_ele2_recHit_Alpha);


	fChain->SetBranchStatus("ele2_charge", 1);
	fChain->SetBranchAddress("ele2_charge", &ele2_charge, &b_ele2_charge);
	fChain->SetBranchStatus("ele2_eta", 1);
	fChain->SetBranchAddress("ele2_eta", &ele2_eta, &b_ele2_eta);
	fChain->SetBranchStatus("ele2_phi", 1);
	fChain->SetBranchAddress("ele2_phi", &ele2_phi, &b_ele2_phi);
	fChain->SetBranchStatus("ele2_scERaw", 1);
	fChain->SetBranchAddress("ele2_scERaw", &ele2_scERaw, &b_ele2_scERaw);
	fChain->SetBranchStatus("ele2_scE", 1);
	fChain->SetBranchAddress("ele2_scE", &ele2_scE, &b_ele2_scE);
	fChain->SetBranchStatus("ele2_scEta", 1);
	fChain->SetBranchAddress("ele2_scEta", &ele2_scEta, &b_ele2_scEta);
	fChain->SetBranchStatus("ele2_es", 1);
	fChain->SetBranchAddress("ele2_es", &ele2_es, &b_ele2_es);
	fChain->SetBranchStatus("ele2_e3x3", 1);
	fChain->SetBranchAddress("ele2_e3x3", &ele2_e3x3, &b_ele2_e3x3);
	fChain->SetBranchStatus("ele2_tkP", 1);
	fChain->SetBranchAddress("ele2_tkP", &ele2_tkP, &b_ele2_tkP);
	fChain->SetBranchStatus("ele2_fbrem", 1);
	fChain->SetBranchAddress("ele2_fbrem", &ele2_fbrem, &b_ele2_fbrem);
	fChain->SetBranchStatus("ele2_EOverP", 1);
	fChain->SetBranchAddress("ele2_EOverP", &ele2_EOverP, &b_ele2_EOverP);
	fChain->SetBranchStatus("ele2_isEB", 1);
	fChain->SetBranchAddress("ele2_isEB", &ele2_isEB, &b_ele2_isEB);
	fChain->SetBranchStatus("ele2_scE_regression", 1);
	fChain->SetBranchAddress("ele2_scE_regression", &ele2_scE_regression, &b_ele2_scE_regression);

	fChain->SetBranchStatus("ele2_seedLaserAlpha", 1);
	fChain->SetBranchAddress("ele2_seedLaserAlpha", &ele2_seedLaserAlpha, &b_ele2_seedLaserAlpha);
	fChain->SetBranchStatus("ele2_seedLaserCorr", 1);
	fChain->SetBranchAddress("ele2_seedLaserCorr",  &ele2_seedLaserCorr,  &b_ele2_seedLaserCorr);

	fChain->SetBranchStatus("ele2_isEBEEGap", 1);
	fChain->SetBranchAddress("ele2_isEBEEGap", &ele2_isEBEEGap, &b_ele2_isEBEEGap);
	fChain->SetBranchStatus("ele2_isEBEtaGap", 1);
	fChain->SetBranchAddress("ele2_isEBEtaGap", &ele2_isEBEtaGap, &b_ele2_isEBEtaGap);
	fChain->SetBranchStatus("ele2_isEBPhiGap", 1);
	fChain->SetBranchAddress("ele2_isEBPhiGap", &ele2_isEBPhiGap, &b_ele2_isEBPhiGap);
	fChain->SetBranchStatus("ele2_isEEDeeGap", 1);
	fChain->SetBranchAddress("ele2_isEEDeeGap", &ele2_isEEDeeGap, &b_ele2_isEEDeeGap);
	fChain->SetBranchStatus("ele2_isEERingGap", 1);
	fChain->SetBranchAddress("ele2_isEERingGap", &ele2_isEERingGap, &b_ele2_isEERingGap);

}


//! Declaration of the objects that are save in the output file

void XtalAlphaEE::bookHistos(int nLoops)
{

	///service histos

	h_Alpha_hashedIndex_EE        = new TH1F ("h_scale_Alpha_EE", "h_Alpha_hashedIndex_EE", kEEhalf * 2, 0, kEEhalf * 2 - 1 );
	h_occupancy_hashedIndex_EE    = new TH1F ("h_occupancy_hashedIndex_EE", "h_occupancy_hashedIndex_EE", kEEhalf * 2, 0, kEEhalf * 2 - 1 );
	hC_EoP                        = new hChain ("EoP", "EoP", 500, 0.2, 3.0, nLoops);

	///EE+

	hC_AlphaValues_BTCP_EEP            = new hChain ("AlphaValues_BTCP_EEP", "AlphaValues_BTCP_EEP", 400, 0.2, 1.9, nLoops);
	hC_AlphaSpreadVsLoop_BTCP_EEP      = new hChain ("hC_AlphaSpreadVsLoop_BTCP_EEP", "hC_AlphaSpreadVsLoop_BTCP_EEP", 40, 0, 40, nLoops);
	hC_Alpha_BTCP_EEP                  = new h2Chain("hC_Alpha_BTCP_EEP", "hC_Alpha_BTCP_EEP", 100, 1, 101, 100, 1, 101, nLoops );
	hC_AlphaValues_SIC_EEP            = new hChain ("AlphaValues_SIC_EEP", "AlphaValues_SIC_EEP", 400, 0.2, 1.9, nLoops);
	hC_AlphaSpreadVsLoop_SIC_EEP      = new hChain ("hC_AlphaSpreadVsLoop_SIC_EEP", "hC_AlphaSpreadVsLoop_SIC_EEP", 40, 0, 40, nLoops);
	hC_Alpha_SIC_EEP                  = new h2Chain("hC_Alpha_SIC_EEP", "hC_Alpha_SIC_EEP", 100, 1, 101, 100, 1, 101, nLoops );

	h_Alpha_EEP             = new TH2F("h_Alpha_EEP", "h_Alpha_EEP", 100, 1, 101, 100, 1, 101);
	h_scalib_EEP            = new TH2F("h_scalib_EEP", "h_scalib_EEP", 100, 1, 101, 100, 1, 101);

	h_occupancy_EEP         = new TH2F("h_occupancy_EEP", "h_occupancy_EEP", 100, 1, 101, 100, 1, 101 );
	h_map_Dead_Channels_EEP = new TH2F("h_map_Dead_Channels_EEP", "h_map_Dead_Channels_EEP", 100, 1, 101, 100, 1, 101);

	p_AlphaVsIeta_BTCP_EEP       = new TProfile("p_AlphaVsIeta_BTCP_EEP", "p_AlphaVsIeta_BTCP_EEP", 40, 0, 40);
	AlphaSpreadVsIeta_BTCP_EEP   = new TH1F("AlphaSpreadVsIeta_BTCP_EEP", "AlphaSpreadVsIeta_BTCP_EEP", 40, 0, 40);

	g_AlphameanVsLoop_BTCP_EEP   = new TGraphErrors();
	g_AlphameanVsLoop_BTCP_EEP   -> SetName("g_AlphameanVsLoop_BTCP_EEP");
	g_AlphameanVsLoop_BTCP_EEP   -> SetTitle("g_AlphameanVsLoop_BTCP_EEP");

	g_AlpharmsVsLoop_BTCP_EEP    = new TGraphErrors();
	g_AlpharmsVsLoop_BTCP_EEP    -> SetName("g_AlpharmsVsLoop_BTCP_EEP");
	g_AlpharmsVsLoop_BTCP_EEP    -> SetTitle("g_AlpharmsVsLoop_BTCP_EEP");

	g_AlphaSigmaVsLoop_BTCP_EEP  = new TGraphErrors();
	g_AlphaSigmaVsLoop_BTCP_EEP  -> SetName("g_AlpharmsVsLoop_BTCP_EEP");
	g_AlphaSigmaVsLoop_BTCP_EEP  -> SetTitle("g_AlpharmsVsLoop_BTCP_EEP");

	p_AlphaVsIeta_SIC_EEP       = new TProfile("p_AlphaVsIeta_SIC_EEP", "p_AlphaVsIeta_SIC_EEP", 40, 0, 40);
	AlphaSpreadVsIeta_SIC_EEP   = new TH1F("AlphaSpreadVsIeta_SIC_EEP", "AlphaSpreadVsIeta_SIC_EEP", 40, 0, 40);

	g_AlphameanVsLoop_SIC_EEP   = new TGraphErrors();
	g_AlphameanVsLoop_SIC_EEP   -> SetName("g_AlphameanVsLoop_SIC_EEP");
	g_AlphameanVsLoop_SIC_EEP   -> SetTitle("g_AlphameanVsLoop_SIC_EEP");

	g_AlpharmsVsLoop_SIC_EEP    = new TGraphErrors();
	g_AlpharmsVsLoop_SIC_EEP    -> SetName("g_AlpharmsVsLoop_SIC_EEP");
	g_AlpharmsVsLoop_SIC_EEP    -> SetTitle("g_AlpharmsVsLoop_SIC_EEP");

	g_AlphaSigmaVsLoop_SIC_EEP  = new TGraphErrors();
	g_AlphaSigmaVsLoop_SIC_EEP  -> SetName("g_AlpharmsVsLoop_SIC_EEP");
	g_AlphaSigmaVsLoop_SIC_EEP  -> SetTitle("g_AlpharmsVsLoop_SIC_EEP");

///EE+

	hC_AlphaValues_BTCP_EEM            = new hChain ("AlphaValues_BTCP_EEM", "AlphaValues_BTCP_EEM", 400, 0.2, 1.9, nLoops);
	hC_AlphaSpreadVsLoop_BTCP_EEM      = new hChain ("hC_AlphaSpreadVsLoop_BTCP_EEM", "hC_AlphaSpreadVsLoop_BTCP_EEM", 40, 0, 40, nLoops);
	hC_Alpha_BTCP_EEM                  = new h2Chain("hC_Alpha_BTCP_EEM", "hC_Alpha_BTCP_EEM", 100, 1, 101, 100, 1, 101, nLoops );
	hC_AlphaValues_SIC_EEM            = new hChain ("AlphaValues_SIC_EEM", "AlphaValues_SIC_EEM", 400, 0.2, 1.9, nLoops);
	hC_AlphaSpreadVsLoop_SIC_EEM      = new hChain ("hC_AlphaSpreadVsLoop_SIC_EEM", "hC_AlphaSpreadVsLoop_SIC_EEM", 40, 0, 40, nLoops);
	hC_Alpha_SIC_EEM                  = new h2Chain("hC_Alpha_SIC_EEM", "hC_Alpha_SIC_EEM", 100, 1, 101, 100, 1, 101, nLoops );

	h_Alpha_EEM             = new TH2F("h_Alpha_EEM", "h_Alpha_EEM", 100, 1, 101, 100, 1, 101);
	h_scalib_EEM            = new TH2F("h_scalib_EEM", "h_scalib_EEM", 100, 1, 101, 100, 1, 101);

	h_occupancy_EEM         = new TH2F("h_occupancy_EEM", "h_occupancy_EEM", 100, 1, 101, 100, 1, 101 );
	h_map_Dead_Channels_EEM = new TH2F("h_map_Dead_Channels_EEM", "h_map_Dead_Channels_EEM", 100, 1, 101, 100, 1, 101);

	p_AlphaVsIeta_BTCP_EEM       = new TProfile("p_AlphaVsIeta_BTCP_EEM", "p_AlphaVsIeta_BTCP_EEM", 40, 0, 40);
	AlphaSpreadVsIeta_BTCP_EEM   = new TH1F("AlphaSpreadVsIeta_BTCP_EEM", "AlphaSpreadVsIeta_BTCP_EEM", 40, 0, 40);

	g_AlphameanVsLoop_BTCP_EEM   = new TGraphErrors();
	g_AlphameanVsLoop_BTCP_EEM   -> SetName("g_AlphameanVsLoop_BTCP_EEM");
	g_AlphameanVsLoop_BTCP_EEM   -> SetTitle("g_AlphameanVsLoop_BTCP_EEM");

	g_AlpharmsVsLoop_BTCP_EEM    = new TGraphErrors();
	g_AlpharmsVsLoop_BTCP_EEM    -> SetName("g_AlpharmsVsLoop_BTCP_EEM");
	g_AlpharmsVsLoop_BTCP_EEM    -> SetTitle("g_AlpharmsVsLoop_BTCP_EEM");

	g_AlphaSigmaVsLoop_BTCP_EEM  = new TGraphErrors();
	g_AlphaSigmaVsLoop_BTCP_EEM  -> SetName("g_AlpharmsVsLoop_BTCP_EEM");
	g_AlphaSigmaVsLoop_BTCP_EEM  -> SetTitle("g_AlpharmsVsLoop_BTCP_EEM");

	p_AlphaVsIeta_SIC_EEM       = new TProfile("p_AlphaVsIeta_SIC_EEM", "p_AlphaVsIeta_SIC_EEM", 40, 0, 40);
	AlphaSpreadVsIeta_SIC_EEM   = new TH1F("AlphaSpreadVsIeta_SIC_EEM", "AlphaSpreadVsIeta_SIC_EEM", 40, 0, 40);

	g_AlphameanVsLoop_SIC_EEM   = new TGraphErrors();
	g_AlphameanVsLoop_SIC_EEM   -> SetName("g_AlphameanVsLoop_SIC_EEM");
	g_AlphameanVsLoop_SIC_EEM   -> SetTitle("g_AlphameanVsLoop_SIC_EEM");

	g_AlpharmsVsLoop_SIC_EEM    = new TGraphErrors();
	g_AlpharmsVsLoop_SIC_EEM    -> SetName("g_AlpharmsVsLoop_SIC_EEM");
	g_AlpharmsVsLoop_SIC_EEM    -> SetTitle("g_AlpharmsVsLoop_SIC_EEM");

	g_AlphaSigmaVsLoop_SIC_EEM  = new TGraphErrors();
	g_AlphaSigmaVsLoop_SIC_EEM  -> SetName("g_AlpharmsVsLoop_SIC_EEM");
	g_AlphaSigmaVsLoop_SIC_EEM  -> SetTitle("g_AlpharmsVsLoop_SIC_EEM");


	return;
}




///===== Build E/p for electron 1 and 2

void XtalAlphaEE::BuildEoPeta_ele(int iLoop, int nentries , int useW, int useZ, std::vector<float> theScalibration, bool isSaveEPDistribution, bool isR9selection,                                  float R9Min, bool isMCTruth, bool isfbrem)
{

	if(iLoop == 0) {
		TString name = Form ("hC_EoP_eta_%d", iLoop);
		hC_EoP_ir_ele = new hChain (name, name, 250, 0.1, 3.0, 41);
	} else {
		hC_EoP_ir_ele -> Reset();
		TString name = Form ("hC_EoP_eta_%d", iLoop);
		hC_EoP_ir_ele = new hChain (name, name, 250, 0.1, 3.0, 41);
	}

	//  Long64_t nb = 0;

	for(Long64_t jentry = 0; jentry < nentries; jentry++) {

		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;

		//    nb = fChain->GetEntry(jentry);

		if (!(jentry % 1000000))std::cerr << "building E/p distribution ----> " << jentry << " vs " << nentries << std::endl;

		float pIn, FdiEta;

		///=== electron tight W or Z only Endcap
		if ( ele1_isEB == 0 && (( useW == 1 && isW == 1 ) ||  ( useZ == 1 && isZ == 1 ))) {

			FdiEta = ele1_scE / (ele1_scERaw + ele1_es); /// Cluster containment approximation using ps infos

			float thisE = 0;
			int   iseed = 0 ;
			int   seed_hashedIndex = 0;
			float E_seed = 0;
			float thisE3x3 = 0;
			/// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy

			for (unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {

				float thisAlpha = 1.;
				int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);

				if(ele1_recHit_E -> at(iRecHit) > E_seed && ele1_recHit_laserCorr -> at(iRecHit) > 0. && ele1_recHit_Alpha -> at(iRecHit) > 0.) {
					seed_hashedIndex = ele1_recHit_hashedIndex -> at(iRecHit);
					iseed = iRecHit;
					E_seed = ele1_recHit_E -> at(iRecHit); ///Seed infos

				}

				if (iLoop > 0) thisAlpha = h_Alpha_hashedIndex_EE -> GetBinContent(thisIndex + 1);
				if(ele1_recHit_laserCorr -> at(iRecHit) > 0. && ele1_recHit_Alpha -> at(iRecHit) > 0.)
					thisE += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta * TMath::Power(ele1_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.); /// SC energy

			}
			for (unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {

				float thisAlpha = 1.;
				int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);

				if (iLoop > 0) thisAlpha = h_Alpha_hashedIndex_EE -> GetBinContent(thisIndex + 1);

				if(fabs(ele1_recHit_ietaORix->at(iRecHit) - ele1_recHit_ietaORix->at(iseed)) <= 1 &&
				        fabs(ele1_recHit_iphiORiy->at(iRecHit) - ele1_recHit_iphiORiy->at(iseed)) <= 1 && ele1_recHit_laserCorr -> at(iRecHit) > 0. &&
				        ele1_recHit_Alpha -> at(iRecHit) > 0.)
					thisE3x3 += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta * TMath::Power(ele1_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.);
			}


			int ix_seed   = GetIxFromHashedIndex(seed_hashedIndex);
			int iy_seed   = GetIyFromHashedIndex(seed_hashedIndex);
			int iz_seed   = GetZsideFromHashedIndex(seed_hashedIndex);
			int ir_seed   = eRings -> GetEndcapRing(ix_seed, iy_seed, iz_seed); /// Seed ring

			bool skipElectron = false;

			/// Option for MCTruth analysis
			if(!isMCTruth) {

				pIn = ele1_tkP;
				int regionId = templIndexEE(myTypeEE, ele1_eta, ele1_charge, thisE3x3 / thisE);
				pIn /= myMomentumScale[regionId] -> Eval( ele1_phi );
			} else {
				pIn = ele1_E_true;
				if(fabs(ele1_DR) > 0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
			}

			/// R9, fbrem selection before E/p distribution
			if( fabs(thisE3x3 / thisE) < 0.80 && isR9selection == true && fabs(ele1_scEta) <= 1.75 )                             skipElectron = true;
			if( fabs(thisE3x3 / thisE) < 0.88 && isR9selection == true && fabs(ele1_scEta) >  1.75 && fabs(ele1_scEta) <= 2.00 ) skipElectron = true;
			if( fabs(thisE3x3 / thisE) < 0.92 && isR9selection == true && fabs(ele1_scEta) >  2.00 && fabs(ele1_scEta) <= 2.15 ) skipElectron = true;
			if( fabs(thisE3x3 / thisE) < 0.94 && isR9selection == true && fabs(ele1_scEta) >  2.15 )                             skipElectron = true;

			if( fabs(thisE3x3 / thisE) < R9Min && isR9selection == true ) skipElectron = true;

			if( fabs(ele1_fbrem) > 0.4 && isfbrem == true ) skipElectron = true;

			if( !skipElectron ) hC_EoP_ir_ele -> Fill(ir_seed, thisE / (pIn - ele1_es)); // Fill E/p ditribution
		}

		///=== Second medium electron from Z only Endcaps
		if ( ele2_isEB == 0 && (( useW == 1 && isW == 1 ) || ( useZ == 1 && isZ == 1 )) ) {

			FdiEta = ele2_scE / (ele2_scERaw + ele2_es);

			float thisE = 0;
			int   iseed = 0 ;
			int   seed_hashedIndex = 0;
			float E_seed = 0;
			float thisE3x3 = 0;

			/// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
			for (unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {

				float thisAlpha = 1.;
				int thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);

				if(ele2_recHit_E -> at(iRecHit) > E_seed && ele2_recHit_laserCorr -> at(iRecHit) > 0. && ele2_recHit_Alpha -> at(iRecHit) > 0. ) {
					seed_hashedIndex = ele2_recHit_hashedIndex -> at(iRecHit);
					iseed = iRecHit;
					E_seed = ele2_recHit_E -> at(iRecHit);
				}

				if (iLoop > 0) thisAlpha = h_Alpha_hashedIndex_EE -> GetBinContent(thisIndex + 1);

				if(ele2_recHit_laserCorr -> at(iRecHit) > 0. && ele2_recHit_Alpha -> at(iRecHit) > 0.)
					thisE += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta * TMath::Power(ele2_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.);

			}

			for (unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {

				float thisAlpha = 1.;
				int thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);
				// IC obtained from previous Loops
				if (iLoop > 0) thisAlpha = h_Alpha_hashedIndex_EE -> GetBinContent(thisIndex + 1);

				if(fabs(ele2_recHit_ietaORix->at(iRecHit) - ele2_recHit_ietaORix->at(iseed)) <= 1 &&
				        fabs(ele2_recHit_iphiORiy->at(iRecHit) - ele2_recHit_iphiORiy->at(iseed)) <= 1 && ele2_recHit_laserCorr -> at(iRecHit) > 0. &&
				        ele2_recHit_Alpha -> at(iRecHit) > 0.)
					thisE3x3 += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta * TMath::Power(ele2_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.);
			}


			int ix_seed = GetIxFromHashedIndex(seed_hashedIndex);
			int iy_seed = GetIyFromHashedIndex(seed_hashedIndex);
			int iz_seed = GetZsideFromHashedIndex(seed_hashedIndex);
			int ir_seed = eRings -> GetEndcapRing(ix_seed, iy_seed, iz_seed); /// Seed ring

			bool skipElectron = false;
			/// Option for MCTruth Analysis
			if(!isMCTruth) {

				pIn = ele2_tkP;
				int regionId = templIndexEE(myTypeEE, ele2_eta, ele2_charge, thisE3x3 / thisE);
				pIn /= myMomentumScale[regionId] -> Eval( ele2_phi );
			} else {

				pIn = ele2_E_true;
				if(fabs(ele2_DR) > 0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
			}

			/// R9 and fbrem selection
			if( fabs(thisE3x3 / thisE) < 0.80 && isR9selection == true && fabs(ele2_scEta) <= 1.75 )                             skipElectron = true;
			if( fabs(thisE3x3 / thisE) < 0.88 && isR9selection == true && fabs(ele2_scEta) >  1.75 && fabs(ele2_scEta) <= 2.00 ) skipElectron = true;
			if( fabs(thisE3x3 / thisE) < 0.92 && isR9selection == true && fabs(ele2_scEta) >  2.00 && fabs(ele2_scEta) <= 2.15 ) skipElectron = true;
			if( fabs(thisE3x3 / thisE) < 0.94 && isR9selection == true && fabs(ele2_scEta) >  2.15 )                             skipElectron = true;

			if( fabs(thisE3x3 / thisE) < R9Min && isR9selection == true ) skipElectron = true;

			if( fabs(ele2_fbrem) > 0.4 && isfbrem == true ) skipElectron = true;

			if(!skipElectron) hC_EoP_ir_ele -> Fill(ir_seed, thisE / (pIn - ele2_es));

		}
	}


/// Normalization E/p distribution
	for(unsigned int ir = 0 ; ir < hC_EoP_ir_ele->Size() ; ir++) hC_EoP_ir_ele->Normalize(ir);


/// Save E/p distributions
	if(isSaveEPDistribution == true && outEPDistribution_p != "NULL" ) {

		if(iLoop == 0) {
			TFile *f2 = new TFile(outEPDistribution_p.Data(), "RECREATE");
			saveEoPeta(f2);
		} else {
			TFile *f2 = new TFile(outEPDistribution_p.Data(), "UPDATE");
			saveEoPeta(f2);
		}

	}

}


/// L3 Loop method ----> Calibration Loop function
void XtalAlphaEE::Loop(int nentries, int useZ, int useW, int splitStat, int nLoops, bool isMiscalib, bool isSaveEPDistribution,
                       bool isEPselection, bool isR9selection, float R9Min, bool isMCTruth, bool isfbrem, std::map<int, std::vector<std::pair<int, int> > > jsonMap)
{

	if (fChain == 0) return;

	/// Define the number of crystal you want to calibrate
	int m_regions = kEEhalf;

	std::cout << "m_regions " << m_regions << std::endl;

	/// build up scalibration map
	std::vector<float> theScalibration(m_regions * 2, 0.);
	TRandom genRand;

	for ( int iIndex = 0; iIndex < m_regions * 2; iIndex++ ) {

		bool isDeadXtal = false ;

		/// Check if the xtal has to be considered dead or not ---> >Fake dead list given by user
		if(DeadXtal_HashedIndex.at(0) != -9999) isDeadXtal = CheckDeadXtal(GetIxFromHashedIndex(iIndex), GetIyFromHashedIndex(iIndex), GetZsideFromHashedIndex(iIndex));
		if(isDeadXtal == true ) {

			theScalibration[iIndex] = 0;

			if(GetZsideFromHashedIndex(iIndex) > 0)
				h_map_Dead_Channels_EEP->Fill(GetIxFromHashedIndex(iIndex), GetIyFromHashedIndex(iIndex));
			else h_map_Dead_Channels_EEM->Fill(GetIxFromHashedIndex(iIndex), GetIyFromHashedIndex(iIndex));
		} else {
			if(isMiscalib == true) theScalibration[iIndex] = genRand.Gaus(1., 0.03); /// Miscalibration fixed at 5%
			if(isMiscalib == false) theScalibration[iIndex] = 1.;
			if(GetZsideFromHashedIndex(iIndex) > 0)
				h_scalib_EEP -> Fill ( GetIxFromHashedIndex(iIndex), GetIyFromHashedIndex(iIndex), theScalibration[iIndex] ); /// scalibration map for EE+ and EE-
			else  h_scalib_EEM-> Fill ( GetIxFromHashedIndex(iIndex), GetIyFromHashedIndex(iIndex), theScalibration[iIndex] );

		}
	}

	/// ----------------- Calibration Loops -----------------------------//
	for ( int iLoop = 0; iLoop < nLoops; iLoop++ ) {

		std::cout << "Starting iteration " << iLoop + 1 << std::endl;

		/// L3 numerator and denominator for EE+ and EE-
		std::vector<float> theNumerator_EEP(m_regions * 2 + 1, 0.);
		std::vector<float> theDenominator_EEP(m_regions * 2 + 1, 0.);
		std::vector<float> theNumerator_EEM(m_regions + 1, 0.);
		std::vector<float> theDenominator_EEM(m_regions + 1, 0.);

		///==== build E/p distribution ele 1 and 2

		BuildEoPeta_ele(iLoop, nentries, useW, useZ, theScalibration, isSaveEPDistribution, isR9selection, R9Min, isMCTruth, isfbrem);

		// define map with events
		std::map<std::pair<int, std::pair<int, int> >, int> eventsMap;

		/// Loop over events
		std::cout << "Number of analyzed events = " << nentries << std::endl;

		for (Long64_t jentry = 0; jentry < nentries; jentry++) {

			if (!(jentry % 100000))std::cerr << jentry;
			if (!(jentry % 10000)) std::cerr << ".";

			//*********************************
			// JSON FILE AND DUPLIACTES IN DATA

			fChain->GetEntry(jentry);

			bool skipEvent = false;

			if( isMCTruth == 0 ) {

				if(AcceptEventByRunAndLumiSection(runId, lumiId, jsonMap) == false) skipEvent = true;

				std::pair<int, Long64_t> eventLSandID(lumiId, eventId);
				std::pair<int, std::pair<int, Long64_t> > eventRUNandLSandID(runId, eventLSandID);
				if( eventsMap[eventRUNandLSandID] == 1 ) skipEvent = true;
				else eventsMap[eventRUNandLSandID] = 1;
			}

			if( skipEvent == true ) continue;

			float pIn, FdiEta;
			std::map<int, double> map;
			bool skipElectron = false;

			/// Only tight electron from W and Z, only Endcap

			if ( ele1_isEB == 0 && (( useW == 1 && isW == 1 ) || ( useZ == 1 && isZ == 1 )) ) {

				/// SCL energy containment correction
				FdiEta = ele1_scE / (ele1_scERaw + ele1_es);

				float thisE = 0;
				float thisE3x3 = 0 ;
				int   iseed = 0 ;
				int   seed_hashedIndex = 0 ;
				float E_seed = 0;

				/// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
				for (unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {

					float thisAlpha = 1.;
					int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);

					if (iLoop > 0) thisAlpha = h_Alpha_hashedIndex_EE -> GetBinContent(thisIndex + 1);

					if(ele1_recHit_laserCorr -> at(iRecHit) > 0. && ele1_recHit_Alpha -> at(iRecHit) > 0.)
						thisE += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta * TMath::Power(ele1_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.);

					if(ele1_recHit_E -> at(iRecHit) > E_seed && ele1_recHit_laserCorr -> at(iRecHit) > 0. && ele1_recHit_Alpha -> at(iRecHit) > 0.) {

						E_seed = ele1_recHit_E -> at(iRecHit);
						iseed = iRecHit;
						seed_hashedIndex = ele1_recHit_hashedIndex -> at(iRecHit); /// Seed infos
					}

				}

				for (unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {

					float thisAlpha = 1.;
					int thisIndex   = ele1_recHit_hashedIndex -> at(iRecHit);

					if (iLoop > 0) thisAlpha = h_Alpha_hashedIndex_EE -> GetBinContent(thisIndex + 1);

					if(fabs(ele1_recHit_ietaORix->at(iRecHit) - ele1_recHit_ietaORix->at(iseed)) <= 1 &&
					        fabs(ele1_recHit_iphiORiy->at(iRecHit) - ele1_recHit_iphiORiy->at(iseed)) <= 1 && ele1_recHit_laserCorr -> at(iRecHit) > 0. &&
					        ele1_recHit_Alpha -> at(iRecHit) > 0.)
						thisE3x3 += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta * TMath::Power(ele1_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.);

				}

				/// find the zside
				int thisCaliBlock = -1;
				if (GetZsideFromHashedIndex(ele1_recHit_hashedIndex -> at(iseed)) < 0) thisCaliBlock = 0;
				else thisCaliBlock = 1;

				int ix_seed = GetIxFromHashedIndex(seed_hashedIndex);
				int iy_seed = GetIyFromHashedIndex(seed_hashedIndex);
				int iz_seed = GetZsideFromHashedIndex(seed_hashedIndex);
				int ir_seed = eRings -> GetEndcapRing(ix_seed, iy_seed, iz_seed);

				/// MCTruth option
				if(!isMCTruth) {

					pIn = ele1_tkP;
					int regionId = templIndexEE(myTypeEE, ele1_eta, ele1_charge, thisE3x3 / thisE);
					pIn /= myMomentumScale[regionId] -> Eval( ele1_phi );
				} else {
					pIn = ele1_E_true;
					if(fabs(ele1_DR) > 0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
				}

				TH1F* EoPHisto = hC_EoP_ir_ele->GetHisto(ir_seed);

				if ( fabs(thisE / (ele1_tkP - ele1_es) - 1) > 0.7 && isEPselection == true) skipElectron = true; /// Take the correct E/p pdf to weight events in the calib procedure

				/// R9 and fbrem selection
				if( fabs(thisE3x3 / thisE) < 0.80 && isR9selection == true && fabs(ele1_scEta) <= 1.75 )                             skipElectron = true;
				if( fabs(thisE3x3 / thisE) < 0.88 && isR9selection == true && fabs(ele1_scEta) >  1.75 && fabs(ele1_scEta) <= 2.00 ) skipElectron = true;
				if( fabs(thisE3x3 / thisE) < 0.92 && isR9selection == true && fabs(ele1_scEta) >  2.00 && fabs(ele1_scEta) <= 2.15 ) skipElectron = true;
				if( fabs(thisE3x3 / thisE) < 0.94 && isR9selection == true && fabs(ele1_scEta) >  2.15 )                             skipElectron = true;

				if( fabs(thisE3x3 / thisE) < R9Min && isR9selection == true ) skipElectron = true;

				if( fabs(ele1_fbrem) > 0.4 && isfbrem == true ) skipElectron = true;

				if( thisE / (pIn - ele1_es) < EoPHisto->GetXaxis()->GetXmin() ||
				        thisE / (pIn - ele1_es) > EoPHisto->GetXaxis()->GetXmax() ) skipElectron = true;

				if( !skipElectron ) {

					for( unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++) {

						int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);
						float thisAlpha = 1.;

						/// Fill the occupancy map JUST for the first Loop
						if( iLoop == 0 ) {
							h_occupancy_hashedIndex_EE -> Fill(thisIndex);
							if ( GetZsideFromHashedIndex(thisIndex) < 0 ) h_occupancy_EEM -> Fill(GetIxFromHashedIndex(thisIndex), GetIyFromHashedIndex(thisIndex) );
							else                                          h_occupancy_EEP -> Fill(GetIxFromHashedIndex(thisIndex), GetIyFromHashedIndex(thisIndex) );
						}

						if( iLoop > 0 ) thisAlpha = h_Alpha_hashedIndex_EE -> GetBinContent(thisIndex + 1);


						///Use full statistic
						if( splitStat == 0 ) {

							if(thisCaliBlock == 0) {

								int EoPbin = EoPHisto->FindBin(thisE / (pIn - ele1_es));
								theNumerator_EEM[thisIndex]   += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele1_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * (pIn - ele1_es) / thisE * EoPHisto->GetBinContent(EoPbin);
								theDenominator_EEM[thisIndex] += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele1_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * EoPHisto->GetBinContent(EoPbin);
							}

							if( thisCaliBlock == 1 ) {

								int EoPbin = EoPHisto->FindBin(thisE / (pIn - ele1_es));
								theNumerator_EEP[thisIndex]   += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele1_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * (pIn - ele1_es) / thisE * EoPHisto->GetBinContent(EoPbin);
								theDenominator_EEP[thisIndex] += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele1_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * EoPHisto->GetBinContent(EoPbin);
							}
						}

						/// use evens
						if( splitStat == 1 && jentry % 2 == 0 ) {

							if( thisCaliBlock == 0 ) {

								int EoPbin = EoPHisto->FindBin(thisE / (pIn - ele1_es));
								theNumerator_EEM[thisIndex]   += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele1_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * (pIn - ele1_es) / thisE * EoPHisto->GetBinContent(EoPbin);
								theDenominator_EEM[thisIndex] += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele1_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * EoPHisto->GetBinContent(EoPbin);
							}

							if(thisCaliBlock == 1) {

								int EoPbin = EoPHisto->FindBin(thisE / (pIn - ele1_es));
								theNumerator_EEP[thisIndex]   += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele1_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * (pIn - ele1_es) / thisE * EoPHisto->GetBinContent(EoPbin);
								theDenominator_EEP[thisIndex] += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele1_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * EoPHisto->GetBinContent(EoPbin);
							}
						}

						/// use odd
						if( splitStat == -1 && jentry % 2 != 0 ) {

							if(thisCaliBlock == 0) {

								int EoPbin = EoPHisto->FindBin(thisE / (pIn - ele1_es));
								theNumerator_EEM[thisIndex]   += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele1_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * (pIn - ele1_es) / thisE * EoPHisto->GetBinContent(EoPbin);
								theDenominator_EEM[thisIndex] += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele1_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * EoPHisto->GetBinContent(EoPbin);
							}

							if(thisCaliBlock == 1) {

								int EoPbin = EoPHisto->FindBin(thisE / (pIn - ele1_es));
								theNumerator_EEP[thisIndex]   += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele1_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * (pIn - ele1_es) / thisE * EoPHisto->GetBinContent(EoPbin);
								theDenominator_EEP[thisIndex] += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele1_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * EoPHisto->GetBinContent(EoPbin);
							}
						}
					}
				}

				///Fill EoP
				if( thisCaliBlock != -1 ) hC_EoP -> Fill(iLoop, thisE / (pIn - ele1_es));
			}

			skipElectron = false;

			/// Medium ele from Z only Endcap
			if( ele2_isEB == 0 && ( useZ == 1 && isZ == 1 ) ) {
				/// SCL energy containment correction
				FdiEta = ele2_scE / (ele2_scERaw + ele2_es);

				float thisE = 0;
				float thisE3x3 = 0 ;
				int   iseed = 0 ;
				int   seed_hashedIndex = 0;
				float E_seed = 0;


				/// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
				for(unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {

					float thisAlpha = 1.;
					int   thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);

					if (iLoop > 0) thisAlpha = h_Alpha_hashedIndex_EE -> GetBinContent(thisIndex + 1);

					if(ele2_recHit_laserCorr -> at(iRecHit) > 0. && ele2_recHit_Alpha -> at(iRecHit) > 0.)
						thisE += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta * TMath::Power(ele2_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.);

					if(ele2_recHit_E -> at(iRecHit) > E_seed) {
						E_seed = ele2_recHit_E -> at(iRecHit);
						iseed = iRecHit;
						seed_hashedIndex = ele2_recHit_hashedIndex -> at(iRecHit);
					}
				}

				for (unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {

					float thisAlpha = 1.;
					int thisIndex   = ele2_recHit_hashedIndex -> at(iRecHit);

					if (iLoop > 0) thisAlpha = h_Alpha_hashedIndex_EE -> GetBinContent(thisIndex + 1);

					if(fabs(ele2_recHit_ietaORix->at(iRecHit) - ele2_recHit_ietaORix->at(iseed)) <= 1 &&
					        fabs(ele2_recHit_iphiORiy->at(iRecHit) - ele2_recHit_iphiORiy->at(iseed)) <= 1 && ele2_recHit_laserCorr -> at(iRecHit) > 0.
					        && ele2_recHit_Alpha -> at(iRecHit) > 0.)
						thisE3x3 += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta * TMath::Power(ele2_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.);
				}

				/// find the zside
				int thisCaliBlock = -1;
				if (GetZsideFromHashedIndex(ele2_recHit_hashedIndex -> at(iseed)) < 0) thisCaliBlock = 0;
				else thisCaliBlock = 1;

				int ix_seed = GetIxFromHashedIndex(seed_hashedIndex);
				int iy_seed = GetIyFromHashedIndex(seed_hashedIndex);
				int iz_seed = GetZsideFromHashedIndex(seed_hashedIndex);
				int ir_seed = eRings -> GetEndcapRing(ix_seed, iy_seed, iz_seed);

				/// Option for MCTruth Analysis
				if(!isMCTruth) {
					pIn = ele2_tkP;
					int regionId = templIndexEE(myTypeEE, ele2_eta, ele2_charge, thisE3x3 / thisE);
					pIn /= myMomentumScale[regionId] -> Eval( ele2_phi );
				} else {
					pIn = ele2_E_true;
					if(fabs(ele2_DR) > 0.1) skipElectron = true ; /// No macthing beetween gen ele and reco ele
				}

				TH1F* EoPHisto = hC_EoP_ir_ele->GetHisto(ir_seed); /// Use correct pdf for reweight events in the L3 procedure

				/// E/p and R9 selections
				if ( fabs(thisE / (pIn - ele2_es) - 1) > 0.7 && isEPselection == true) skipElectron = true;

				/// R9 and fbrem selection
				if( fabs(thisE3x3 / thisE) < 0.80 && isR9selection == true && fabs(ele2_scEta) <= 1.75 )                             skipElectron = true;
				if( fabs(thisE3x3 / thisE) < 0.88 && isR9selection == true && fabs(ele2_scEta) >  1.75 && fabs(ele2_scEta) <= 2.00 ) skipElectron = true;
				if( fabs(thisE3x3 / thisE) < 0.92 && isR9selection == true && fabs(ele2_scEta) >  2.00 && fabs(ele2_scEta) <= 2.15 ) skipElectron = true;
				if( fabs(thisE3x3 / thisE) < 0.94 && isR9selection == true && fabs(ele2_scEta) >  2.15 )                             skipElectron = true;

				if( fabs(thisE3x3 / thisE) < R9Min && isR9selection == true ) skipElectron = true;

				if( fabs(ele2_fbrem) > 0.4 && isfbrem == true) skipElectron = true;

				if( thisE / (pIn - ele2_es) < EoPHisto->GetXaxis()->GetXmin() ||
				        thisE / (pIn - ele2_es) > EoPHisto->GetXaxis()->GetXmax() ) skipElectron = true;

				if( !skipElectron ) {
					for( unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {

						int thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);
						float thisAlpha = 1.;

						/// Fill the occupancy map JUST for the first Loop
						if ( iLoop == 0 ) {
							h_occupancy_hashedIndex_EE -> Fill(thisIndex);
							if( GetZsideFromHashedIndex(thisIndex) < 0 ) h_occupancy_EEM -> Fill(GetIxFromHashedIndex(thisIndex), GetIyFromHashedIndex(thisIndex) );
							else                                         h_occupancy_EEP -> Fill(GetIxFromHashedIndex(thisIndex), GetIyFromHashedIndex(thisIndex) );
						}

						if(iLoop > 0) thisAlpha = h_Alpha_hashedIndex_EE -> GetBinContent(thisIndex + 1);

						/// Use full statistic
						if( splitStat == 0) {
							if(thisCaliBlock == 0) {
								int EoPbin = EoPHisto->FindBin(thisE / (pIn - ele2_es));
								theNumerator_EEM[thisIndex]   += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele2_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * (pIn - ele2_es) / thisE * EoPHisto->GetBinContent(EoPbin);
								theDenominator_EEM[thisIndex] += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele2_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * EoPHisto->GetBinContent(EoPbin);
							}

							if( thisCaliBlock == 1 ) {
								int EoPbin = EoPHisto->FindBin(thisE / (pIn - ele2_es));
								theNumerator_EEP[thisIndex]   += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele2_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * (pIn - ele2_es) / thisE * EoPHisto->GetBinContent(EoPbin);
								theDenominator_EEP[thisIndex] += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele2_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * EoPHisto->GetBinContent(EoPbin);
							}
						}


						/// use evens
						if( splitStat == 1 && jentry % 2 == 0 ) {
							if( thisCaliBlock == 0 ) {
								int EoPbin = EoPHisto->FindBin(thisE / (pIn - ele2_es));
								theNumerator_EEM[thisIndex]   += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele2_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * (pIn - ele2_es) / thisE * EoPHisto->GetBinContent(EoPbin);
								theDenominator_EEM[thisIndex] += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele2_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * EoPHisto->GetBinContent(EoPbin);
							}

							if( thisCaliBlock == 1 ) {
								int EoPbin = EoPHisto->FindBin(thisE / (pIn - ele2_es));
								theNumerator_EEP[thisIndex]   += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele2_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * (pIn - ele2_es) / thisE * EoPHisto->GetBinContent(EoPbin);
								theDenominator_EEP[thisIndex] += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele2_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * EoPHisto->GetBinContent(EoPbin);
							}
						}

						/// use odd
						if( splitStat == -1 && jentry % 2 != 0 ) {
							if(thisCaliBlock == 0) {
								int EoPbin = EoPHisto->FindBin(thisE / (pIn - ele2_es));
								theNumerator_EEM[thisIndex]   += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele2_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * (pIn - ele2_es) / thisE * EoPHisto->GetBinContent(EoPbin);
								theDenominator_EEM[thisIndex] += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele2_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * EoPHisto->GetBinContent(EoPbin);
							}

							if( thisCaliBlock == 1 ) {
								int EoPbin = EoPHisto->FindBin(thisE / (pIn - ele2_es));
								theNumerator_EEP[thisIndex]   += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele2_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * (pIn - ele2_es) / thisE * EoPHisto->GetBinContent(EoPbin);
								theDenominator_EEP[thisIndex] += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta *
								                                 TMath::Power(ele2_recHit_laserCorr -> at(iRecHit), thisAlpha - 1.) /
								                                 thisE * EoPHisto->GetBinContent(EoPbin);
							}
						}
					}
				}

				//Fill EoP
				if( thisCaliBlock != -1 ) hC_EoP -> Fill(iLoop, thisE / (pIn - ele2_es));
			}
		} ///  End Cycle on the events

		std::cout << ">>>>> [L3][endOfLoop] entering..." << std::endl;

		TH1F* auxiliary_Alpha_BTCP_EEM = new TH1F("auxiliary_Alpha_BTCP_EEM", "auxiliary_Alpha_BTCP_EEM", 400, 0., 3.);
		TH1F* auxiliary_Alpha_BTCP_EEP = new TH1F("auxiliary_Alpha_BTCP_EEP", "auxiliary_Alpha_BTCP_EEP", 400, 0., 3.);
		TH1F* auxiliary_Alpha_SIC_EEM = new TH1F("auxiliary_Alpha_SIC_EEM", "auxiliary_Alpha_SIC_EEM", 400, 0., 3.);
		TH1F* auxiliary_Alpha_SIC_EEP = new TH1F("auxiliary_Alpha_SIC_EEP", "auxiliary_Alpha_SIC_EEP", 400, 0., 3.);

		TF1* f_BTCP_EEP = new TF1("f_BTCP_EEP", "gaus", 0., 3.);
		TF1* f_BTCP_EEM = new TF1("f_BTCP_EEM", "gaus", 0., 3.);
		TF1* f_SIC_EEP = new TF1("f_SIC_EEP", "gaus", 0., 3.);
		TF1* f_SIC_EEM = new TF1("f_SIC_EEM", "gaus", 0., 3.);

		///Fill the histo of IntercalibValues before the solve
		for( int iIndex = 0; iIndex < kEEhalf * 2; iIndex++ ) {
			if( h_occupancy_hashedIndex_EE -> GetBinContent(iIndex + 1) > 0 ) {

				int thisCaliBlock = -1;
				if( GetZsideFromHashedIndex(iIndex) < 0 ) thisCaliBlock = 0;
				else thisCaliBlock = 1;

				float thisAlphaConstant = 1.;
				float oldAlphaConstant = 1.;
				if( iLoop > 0 ) oldAlphaConstant = h_Alpha_hashedIndex_EE -> GetBinContent (iIndex + 1);


				if( thisCaliBlock == 0 && theDenominator_EEM[iIndex] != 0. ) thisAlphaConstant = theNumerator_EEM[iIndex] / theDenominator_EEM[iIndex];
				if( thisCaliBlock == 1 && theDenominator_EEP[iIndex] != 0. ) thisAlphaConstant = theNumerator_EEP[iIndex] / theDenominator_EEP[iIndex];

				h_Alpha_hashedIndex_EE -> SetBinContent(iIndex + 1, thisAlphaConstant * oldAlphaConstant);

				if(SicCrystal->isSic(GetIxFromHashedIndex(iIndex), GetIyFromHashedIndex(iIndex), GetZsideFromHashedIndex(iIndex))) {
					if( thisCaliBlock == 0 ) {

						hC_AlphaValues_SIC_EEM -> Fill (iLoop, thisAlphaConstant);
						hC_Alpha_SIC_EEM       -> Fill (iLoop, GetIxFromHashedIndex(iIndex), GetIyFromHashedIndex(iIndex), thisAlphaConstant * oldAlphaConstant);

						auxiliary_Alpha_SIC_EEM->Fill(thisAlphaConstant);
					}
					if( thisCaliBlock == 1) {
						hC_AlphaValues_SIC_EEP -> Fill (iLoop, thisAlphaConstant);
						hC_Alpha_SIC_EEP       -> Fill(iLoop, GetIxFromHashedIndex(iIndex), GetIyFromHashedIndex(iIndex), thisAlphaConstant * oldAlphaConstant);

						auxiliary_Alpha_SIC_EEP->Fill(thisAlphaConstant);
					}
				} else {
					if( thisCaliBlock == 0 ) {

						hC_AlphaValues_BTCP_EEM -> Fill (iLoop, thisAlphaConstant);
						hC_Alpha_BTCP_EEM       -> Fill (iLoop, GetIxFromHashedIndex(iIndex), GetIyFromHashedIndex(iIndex), thisAlphaConstant * oldAlphaConstant);

						auxiliary_Alpha_BTCP_EEM->Fill(thisAlphaConstant);
					}
					if( thisCaliBlock == 1) {
						hC_AlphaValues_BTCP_EEP -> Fill (iLoop, thisAlphaConstant);
						hC_Alpha_BTCP_EEP       -> Fill(iLoop, GetIxFromHashedIndex(iIndex), GetIyFromHashedIndex(iIndex), thisAlphaConstant * oldAlphaConstant);

						auxiliary_Alpha_BTCP_EEP->Fill(thisAlphaConstant);
					}
				}

			}
		}
		//******************************* EE- ***************************

		g_AlphameanVsLoop_SIC_EEM -> SetPoint(iLoop, iLoop,  auxiliary_Alpha_SIC_EEM->GetMean());
		g_AlphameanVsLoop_SIC_EEM -> SetPointError(iLoop, 0., auxiliary_Alpha_SIC_EEM->GetMeanError());

		g_AlpharmsVsLoop_SIC_EEM -> SetPoint(iLoop, iLoop,   auxiliary_Alpha_SIC_EEM -> GetRMS());
		g_AlpharmsVsLoop_SIC_EEM -> SetPointError(iLoop, 0., auxiliary_Alpha_SIC_EEM -> GetRMSError());

		f_SIC_EEM -> SetParameters (auxiliary_Alpha_SIC_EEM->GetEntries(), auxiliary_Alpha_SIC_EEM->GetMean(), auxiliary_Alpha_SIC_EEM->GetRMS());
		f_SIC_EEM -> SetRange      (auxiliary_Alpha_SIC_EEM->GetMean() - 8 * auxiliary_Alpha_SIC_EEM->GetRMS(), auxiliary_Alpha_SIC_EEM->GetMean() +
		                            8 * auxiliary_Alpha_SIC_EEM->GetRMS());

		auxiliary_Alpha_SIC_EEM->Fit("f_SIC_EEM", "QR");

		g_AlphaSigmaVsLoop_SIC_EEM -> SetPoint(iLoop, iLoop,   f_SIC_EEM -> GetParameter(2));
		g_AlphaSigmaVsLoop_SIC_EEM -> SetPointError(iLoop, 0., f_SIC_EEM -> GetParError(2));

		g_AlphameanVsLoop_BTCP_EEM -> SetPoint(iLoop, iLoop,  auxiliary_Alpha_BTCP_EEM->GetMean());
		g_AlphameanVsLoop_BTCP_EEM -> SetPointError(iLoop, 0., auxiliary_Alpha_BTCP_EEM->GetMeanError());

		g_AlpharmsVsLoop_BTCP_EEM -> SetPoint(iLoop, iLoop,   auxiliary_Alpha_BTCP_EEM -> GetRMS());
		g_AlpharmsVsLoop_BTCP_EEM -> SetPointError(iLoop, 0., auxiliary_Alpha_BTCP_EEM -> GetRMSError());

		f_BTCP_EEM -> SetParameters (auxiliary_Alpha_BTCP_EEM->GetEntries(), auxiliary_Alpha_BTCP_EEM->GetMean(), auxiliary_Alpha_BTCP_EEM->GetRMS());
		f_BTCP_EEM -> SetRange      (auxiliary_Alpha_BTCP_EEM->GetMean() - 8 * auxiliary_Alpha_BTCP_EEM->GetRMS(), auxiliary_Alpha_BTCP_EEM->GetMean() +
		                             8 * auxiliary_Alpha_BTCP_EEM->GetRMS());

		auxiliary_Alpha_BTCP_EEM->Fit("f_BTCP_EEM", "QR");

		g_AlphaSigmaVsLoop_BTCP_EEM -> SetPoint(iLoop, iLoop,   f_BTCP_EEM -> GetParameter(2));
		g_AlphaSigmaVsLoop_BTCP_EEM -> SetPointError(iLoop, 0., f_BTCP_EEM -> GetParError(2));

		//******************************* EE+ ***************************

		g_AlphameanVsLoop_SIC_EEP -> SetPoint(iLoop, iLoop,   auxiliary_Alpha_SIC_EEP -> GetMean());
		g_AlphameanVsLoop_SIC_EEP -> SetPointError(iLoop, 0., auxiliary_Alpha_SIC_EEP -> GetMeanError());

		g_AlpharmsVsLoop_SIC_EEP -> SetPoint(iLoop, iLoop,   auxiliary_Alpha_SIC_EEP -> GetRMS());
		g_AlpharmsVsLoop_SIC_EEP -> SetPointError(iLoop, 0., auxiliary_Alpha_SIC_EEP -> GetRMSError());

		f_SIC_EEP -> SetParameters (auxiliary_Alpha_SIC_EEP->GetEntries(), auxiliary_Alpha_SIC_EEP->GetMean(), auxiliary_Alpha_SIC_EEP->GetRMS());
		f_SIC_EEP -> SetRange      (auxiliary_Alpha_SIC_EEP->GetMean() - 8 * auxiliary_Alpha_SIC_EEP->GetRMS(), auxiliary_Alpha_SIC_EEP->GetMean() +
		                            8 * auxiliary_Alpha_SIC_EEP->GetRMS());

		auxiliary_Alpha_SIC_EEP->Fit("f_SIC_EEP", "QR");

		g_AlphaSigmaVsLoop_SIC_EEP -> SetPoint(iLoop, iLoop,   f_SIC_EEP -> GetParameter(2));
		g_AlphaSigmaVsLoop_SIC_EEP -> SetPointError(iLoop, 0., f_SIC_EEP -> GetParError(2));

		g_AlphameanVsLoop_BTCP_EEP -> SetPoint(iLoop, iLoop,   auxiliary_Alpha_BTCP_EEP -> GetMean());
		g_AlphameanVsLoop_BTCP_EEP -> SetPointError(iLoop, 0., auxiliary_Alpha_BTCP_EEP -> GetMeanError());

		g_AlpharmsVsLoop_BTCP_EEP -> SetPoint(iLoop, iLoop,   auxiliary_Alpha_BTCP_EEP -> GetRMS());
		g_AlpharmsVsLoop_BTCP_EEP -> SetPointError(iLoop, 0., auxiliary_Alpha_BTCP_EEP -> GetRMSError());

		f_BTCP_EEP -> SetParameters (auxiliary_Alpha_BTCP_EEP->GetEntries(), auxiliary_Alpha_BTCP_EEP->GetMean(), auxiliary_Alpha_BTCP_EEP->GetRMS());
		f_BTCP_EEP -> SetRange      (auxiliary_Alpha_BTCP_EEP->GetMean() - 8 * auxiliary_Alpha_BTCP_EEP->GetRMS(), auxiliary_Alpha_BTCP_EEP->GetMean() +
		                             8 * auxiliary_Alpha_BTCP_EEP->GetRMS());

		auxiliary_Alpha_BTCP_EEP->Fit("f_BTCP_EEP", "QR");

		g_AlphaSigmaVsLoop_BTCP_EEP -> SetPoint(iLoop, iLoop,   f_BTCP_EEP -> GetParameter(2));
		g_AlphaSigmaVsLoop_BTCP_EEP -> SetPointError(iLoop, 0., f_BTCP_EEP -> GetParError(2));

		delete auxiliary_Alpha_SIC_EEP ;
		delete auxiliary_Alpha_SIC_EEM ;
		delete auxiliary_Alpha_BTCP_EEP ;
		delete auxiliary_Alpha_BTCP_EEM ;
		delete f_SIC_EEP;
		delete f_SIC_EEM;
		delete f_BTCP_EEP;
		delete f_BTCP_EEM;

		//Alpha spread in each ring
		std::map <int, TH1F*> *histoTemp_BTCP_EEP = new std::map <int, TH1F*>();
		std::map <int, TH1F*> *histoTemp_BTCP_EEM = new std::map <int, TH1F*>();
		std::map <int, TH1F*> *histoTemp_SIC_EEP = new std::map <int, TH1F*>();
		std::map <int, TH1F*> *histoTemp_SIC_EEM = new std::map <int, TH1F*>();

		TString Name ;

		for ( int iIndex = 0; iIndex < kEEhalf * 2; iIndex++ ) {

			if ( h_occupancy_hashedIndex_EE -> GetBinContent(iIndex + 1) == 0) continue;

			int thisCaliBlock = -1;
			if (GetZsideFromHashedIndex(iIndex) < 0) thisCaliBlock = 0;
			else thisCaliBlock = 1;

			int thisIx = GetIxFromHashedIndex(iIndex);
			int thisIy = GetIyFromHashedIndex(iIndex);
			int thisIz = GetZsideFromHashedIndex(iIndex);

			int thisIr = eRings -> GetEndcapRing(thisIx, thisIy, thisIz);

			if(SicCrystal->isSic(thisIx, thisIy, thisIz)) {
				if(!((*histoTemp_SIC_EEP)[thisIr]) && thisCaliBlock == 1) {
					Name = Form("histoTemp_SIC_EEP_%d", thisIr);
					(*histoTemp_SIC_EEP)[thisIr] = new TH1F(Name.Data(), Name.Data(), 400, 0., 3.);
				}

				if(!((*histoTemp_SIC_EEM)[thisIr]) && thisCaliBlock == 0) {
					Name = Form("histoTemp_SIC_EEM_%d", thisIr);
					(*histoTemp_SIC_EEM)[thisIr] = new TH1F(Name.Data(), Name.Data(), 400, 0., 3.);
				}

				float thisAlphaConstant = h_Alpha_hashedIndex_EE -> GetBinContent (iIndex + 1); /// Alpha value

				if(thisCaliBlock == 0)      (*histoTemp_SIC_EEM)[thisIr]->Fill(thisAlphaConstant);
				else if(thisCaliBlock == 1) (*histoTemp_SIC_EEP)[thisIr]->Fill(thisAlphaConstant);
			}

			else {
				if(!((*histoTemp_BTCP_EEP)[thisIr]) && thisCaliBlock == 1) {
					Name = Form("histoTemp_BTCP_EEP_%d", thisIr);
					(*histoTemp_BTCP_EEP)[thisIr] = new TH1F(Name.Data(), Name.Data(), 400, 0., 3.);
				}

				if(!((*histoTemp_BTCP_EEM)[thisIr]) && thisCaliBlock == 0) {
					Name = Form("histoTemp_BTCP_EEM_%d", thisIr);
					(*histoTemp_BTCP_EEM)[thisIr] = new TH1F(Name.Data(), Name.Data(), 400, 0., 3.);
				}

				float thisAlphaConstant = h_Alpha_hashedIndex_EE -> GetBinContent (iIndex + 1); /// Alpha value

				if(thisCaliBlock == 0)      (*histoTemp_BTCP_EEM)[thisIr]->Fill(thisAlphaConstant);
				else if(thisCaliBlock == 1) (*histoTemp_BTCP_EEP)[thisIr]->Fill(thisAlphaConstant);
			}

		}



		for( unsigned int iRing = 0; iRing < histoTemp_SIC_EEP->size(); iRing++) {

			if(!(*histoTemp_SIC_EEP)[iRing]) continue;

			TF1* funz_SIC_EEP = new TF1("funz_SIC_EEP", "gaus", 0., 3.);

			funz_SIC_EEP->SetParameters((*histoTemp_SIC_EEP)[iRing]->GetEntries(), (*histoTemp_SIC_EEP)[iRing]->GetMean(), (*histoTemp_SIC_EEP)[iRing]->GetRMS());
			funz_SIC_EEP->SetRange((*histoTemp_SIC_EEP)[iRing]->GetMean() - 8 * (*histoTemp_SIC_EEP)[iRing]->GetRMS(), (*histoTemp_SIC_EEP)[iRing]->GetMean()
			                       + 8 * (*histoTemp_SIC_EEP)[iRing]->GetRMS());

			(*histoTemp_SIC_EEP)[iRing]->Fit("funz_SIC_EEP", "QR");

			hC_AlphaSpreadVsLoop_SIC_EEP->SetBinContent(iLoop, iRing, funz_SIC_EEP->GetParameter(2));
			hC_AlphaSpreadVsLoop_SIC_EEP->SetBinError(iLoop, iRing, funz_SIC_EEP->GetParError(2));

			delete funz_SIC_EEP;
		}

		for( unsigned int iRing = 0; iRing < histoTemp_BTCP_EEP->size(); iRing++) {

			if(!(*histoTemp_BTCP_EEP)[iRing]) continue;

			TF1* funz_BTCP_EEP = new TF1("funz_BTCP_EEP", "gaus", 0., 3.);

			funz_BTCP_EEP->SetParameters((*histoTemp_BTCP_EEP)[iRing]->GetEntries(), (*histoTemp_BTCP_EEP)[iRing]->GetMean(), (*histoTemp_BTCP_EEP)[iRing]->GetRMS());
			funz_BTCP_EEP->SetRange((*histoTemp_BTCP_EEP)[iRing]->GetMean() - 8 * (*histoTemp_BTCP_EEP)[iRing]->GetRMS(), (*histoTemp_BTCP_EEP)[iRing]->GetMean()
			                        + 8 * (*histoTemp_BTCP_EEP)[iRing]->GetRMS());

			(*histoTemp_BTCP_EEP)[iRing]->Fit("funz_BTCP_EEP", "QR");

			hC_AlphaSpreadVsLoop_BTCP_EEP->SetBinContent(iLoop, iRing, funz_BTCP_EEP->GetParameter(2));
			hC_AlphaSpreadVsLoop_BTCP_EEP->SetBinError(iLoop, iRing, funz_BTCP_EEP->GetParError(2));

			delete funz_BTCP_EEP;
		}


		for( unsigned int iRing = 0; iRing < histoTemp_SIC_EEM->size(); iRing++) {

			if(!(*histoTemp_SIC_EEM)[iRing]) continue;

			TF1* funz_SIC_EEM = new TF1("funz_SIC_EEM", "gaus", 0., 3.);
			funz_SIC_EEM->SetParameters((*histoTemp_SIC_EEM)[iRing]->GetEntries(), (*histoTemp_SIC_EEM)[iRing]->GetMean(), (*histoTemp_SIC_EEM)[iRing]->GetRMS());
			funz_SIC_EEM->SetRange((*histoTemp_SIC_EEM)[iRing]->GetMean() - 8 * (*histoTemp_SIC_EEM)[iRing]->GetRMS(), (*histoTemp_SIC_EEM)[iRing]->GetMean()
			                       + 8 * (*histoTemp_SIC_EEM)[iRing]->GetRMS());

			(*histoTemp_SIC_EEM)[iRing]->Fit("funz_SIC_EEM", "QR");

			hC_AlphaSpreadVsLoop_SIC_EEM->SetBinContent(iLoop, iRing, funz_SIC_EEM->GetParameter(2));
			hC_AlphaSpreadVsLoop_SIC_EEM->SetBinError(iLoop, iRing, funz_SIC_EEM->GetParError(2));
			delete funz_SIC_EEM;

		}

		for( unsigned int iRing = 0; iRing < histoTemp_BTCP_EEM->size(); iRing++) {

			if(!(*histoTemp_SIC_EEM)[iRing]) continue;

			TF1* funz_BTCP_EEM = new TF1("funz_BTCP_EEM", "gaus", 0., 3.);
			funz_BTCP_EEM->SetParameters((*histoTemp_BTCP_EEM)[iRing]->GetEntries(), (*histoTemp_BTCP_EEM)[iRing]->GetMean(), (*histoTemp_BTCP_EEM)[iRing]->GetRMS());
			funz_BTCP_EEM->SetRange((*histoTemp_BTCP_EEM)[iRing]->GetMean() - 8 * (*histoTemp_BTCP_EEM)[iRing]->GetRMS(), (*histoTemp_BTCP_EEM)[iRing]->GetMean()
			                        + 8 * (*histoTemp_BTCP_EEM)[iRing]->GetRMS());

			(*histoTemp_BTCP_EEM)[iRing]->Fit("funz_BTCP_EEM", "QR");

			hC_AlphaSpreadVsLoop_BTCP_EEM->SetBinContent(iLoop, iRing, funz_BTCP_EEM->GetParameter(2));
			hC_AlphaSpreadVsLoop_BTCP_EEM->SetBinError(iLoop, iRing, funz_BTCP_EEM->GetParError(2));
			delete funz_BTCP_EEM;

		}

		for( std::map<int, TH1F*>::const_iterator itrechit = histoTemp_SIC_EEM->begin(); itrechit != histoTemp_SIC_EEM->end(); itrechit++) delete itrechit->second;
		for( std::map<int, TH1F*>::const_iterator itrechit = histoTemp_SIC_EEP->begin(); itrechit != histoTemp_SIC_EEP->end(); itrechit++) delete itrechit->second;
		for( std::map<int, TH1F*>::const_iterator itrechit = histoTemp_BTCP_EEM->begin(); itrechit != histoTemp_BTCP_EEM->end(); itrechit++) delete itrechit->second;
		for( std::map<int, TH1F*>::const_iterator itrechit = histoTemp_BTCP_EEP->begin(); itrechit != histoTemp_BTCP_EEP->end(); itrechit++) delete itrechit->second;

		delete histoTemp_BTCP_EEP;
		delete histoTemp_BTCP_EEM;
		delete histoTemp_SIC_EEP;
		delete histoTemp_SIC_EEM;


	} /// End of Calibration Loops

	std::map<int, TH1F*> *histo_SIC_EEP = new std::map<int, TH1F*>();
	std::map<int, TH1F*> *histo_SIC_EEM = new std::map<int, TH1F*>();
	std::map<int, TH1F*> *histo_BTCP_EEP = new std::map<int, TH1F*>();
	std::map<int, TH1F*> *histo_BTCP_EEM = new std::map<int, TH1F*>();

	TString Name ;

	///Fill the histo of IntercalibValues after the loops at last step
	for( int iIndex = 0; iIndex < kEEhalf * 2; iIndex++ ) {

		if( h_occupancy_hashedIndex_EE -> GetBinContent(iIndex + 1) == 0 ) continue;

		int thisCaliBlock = -1;
		if (GetZsideFromHashedIndex(iIndex) < 0) thisCaliBlock = 0;
		else thisCaliBlock = 1;

		int thisIx = GetIxFromHashedIndex(iIndex);
		int thisIy = GetIyFromHashedIndex(iIndex);
		int thisIz = GetZsideFromHashedIndex(iIndex); /// Ix, Iy and Iz info for each xtal

		int thisIr = eRings -> GetEndcapRing(thisIx, thisIy, thisIz); /// Endcap ring  xtal belongs to

		float thisAlphaConstant = h_Alpha_hashedIndex_EE -> GetBinContent (iIndex + 1); /// Final IC value

		if ( thisCaliBlock == 0 ) h_Alpha_EEM -> Fill (thisIx, thisIy, thisAlphaConstant);
		else                      h_Alpha_EEP -> Fill (thisIx, thisIy, thisAlphaConstant);


		if(SicCrystal->isSic(thisIx, thisIy, thisIz)) {

			if ( thisCaliBlock == 0 ) p_AlphaVsIeta_SIC_EEM -> Fill (thisIr, thisAlphaConstant);
			else                      p_AlphaVsIeta_SIC_EEP -> Fill (thisIr, thisAlphaConstant);


			if((!(*histo_SIC_EEP)[thisIr]) && thisCaliBlock == 1) {
				Name = Form("histo_SIC_EEP_%d", thisIr);
				(*histo_SIC_EEP)[thisIr] = new TH1F(Name.Data(), Name.Data(), 400, 0., 3.);
			}
			if((!(*histo_SIC_EEM)[thisIr]) && thisCaliBlock == 0) {
				Name = Form("histo_SIC_EEM_%d", thisIr);
				(*histo_SIC_EEM)[thisIr] = new TH1F(Name.Data(), Name.Data(), 400, 0., 3.);
			}

			if(thisCaliBlock == 0)  (*histo_SIC_EEM)[thisIr]->Fill(thisAlphaConstant);
			else if(thisCaliBlock == 1) (*histo_SIC_EEP)[thisIr]->Fill(thisAlphaConstant);
		} else {

			if ( thisCaliBlock == 0 ) p_AlphaVsIeta_BTCP_EEM -> Fill (thisIr, thisAlphaConstant);
			else                      p_AlphaVsIeta_BTCP_EEP -> Fill (thisIr, thisAlphaConstant);

			if((!(*histo_BTCP_EEP)[thisIr]) && thisCaliBlock == 1) {
				Name = Form("histo_BTCP_EEP_%d", thisIr);
				(*histo_BTCP_EEP)[thisIr] = new TH1F(Name.Data(), Name.Data(), 400, 0., 3.);
			}
			if((!(*histo_BTCP_EEM)[thisIr]) && thisCaliBlock == 0) {
				Name = Form("histo_BTCP_EEM_%d", thisIr);
				(*histo_BTCP_EEM)[thisIr] = new TH1F(Name.Data(), Name.Data(), 400, 0., 3.);
			}

			if(thisCaliBlock == 0)  (*histo_BTCP_EEM)[thisIr]->Fill(thisAlphaConstant);
			else if(thisCaliBlock == 1) (*histo_BTCP_EEP)[thisIr]->Fill(thisAlphaConstant);
		}

	}

	// spread and mean value vs iRing

	for( unsigned int iRing = 0; iRing < histo_SIC_EEP->size(); iRing++) {
		if(!(*histo_SIC_EEP)[iRing]) continue;

		TF1* f_SIC_EEP = new TF1("f_SIC_EEP", "gaus", 0., 3.);
		f_SIC_EEP->SetParameters((*histo_SIC_EEP)[iRing]->GetEntries(), (*histo_SIC_EEP)[iRing]->GetMean(), (*histo_SIC_EEP)[iRing]->GetRMS());
		f_SIC_EEP->SetRange((*histo_SIC_EEP)[iRing]->GetMean() - 8 * (*histo_SIC_EEP)[iRing]->GetRMS(), (*histo_SIC_EEP)[iRing]->GetMean()
		                    + 8 * (*histo_SIC_EEP)[iRing]->GetRMS());

		(*histo_SIC_EEP)[iRing]->Fit("f_SIC_EEP", "QR");

		AlphaSpreadVsIeta_SIC_EEP->SetBinContent(iRing, f_SIC_EEP->GetParameter(2));
		AlphaSpreadVsIeta_SIC_EEP->SetBinError(iRing, f_SIC_EEP->GetParError(2));

		delete f_SIC_EEP;

	}

	for( unsigned int iRing = 0; iRing < histo_BTCP_EEP->size(); iRing++) {
		if(!(*histo_BTCP_EEP)[iRing]) continue;

		TF1* f_BTCP_EEP = new TF1("f_BTCP_EEP", "gaus", 0., 3.);
		f_BTCP_EEP->SetParameters((*histo_BTCP_EEP)[iRing]->GetEntries(), (*histo_BTCP_EEP)[iRing]->GetMean(), (*histo_BTCP_EEP)[iRing]->GetRMS());
		f_BTCP_EEP->SetRange((*histo_BTCP_EEP)[iRing]->GetMean() - 8 * (*histo_BTCP_EEP)[iRing]->GetRMS(), (*histo_BTCP_EEP)[iRing]->GetMean()
		                     + 8 * (*histo_BTCP_EEP)[iRing]->GetRMS());

		(*histo_BTCP_EEP)[iRing]->Fit("f_BTCP_EEP", "QR");

		AlphaSpreadVsIeta_BTCP_EEP->SetBinContent(iRing, f_BTCP_EEP->GetParameter(2));
		AlphaSpreadVsIeta_BTCP_EEP->SetBinError(iRing, f_BTCP_EEP->GetParError(2));

		delete f_BTCP_EEP;

	}

	for( unsigned int iRing = 0; iRing < histo_SIC_EEM->size(); iRing++) {
		if(!(*histo_SIC_EEM)[iRing]) continue;

		TF1* f_SIC_EEM = new TF1("f_SIC_EEM", "gaus", 0., 3.);
		f_SIC_EEM->SetParameters((*histo_SIC_EEM)[iRing]->GetEntries(), (*histo_SIC_EEM)[iRing]->GetMean(), (*histo_SIC_EEM)[iRing]->GetRMS());
		f_SIC_EEM->SetRange((*histo_SIC_EEM)[iRing]->GetMean() - 8 * (*histo_SIC_EEM)[iRing]->GetRMS(), (*histo_SIC_EEM)[iRing]->GetMean()
		                    + 8 * (*histo_SIC_EEM)[iRing]->GetRMS());

		(*histo_SIC_EEM)[iRing]->Fit("f_SIC_EEM", "QR");

		AlphaSpreadVsIeta_SIC_EEM->SetBinContent(iRing, f_SIC_EEM->GetParameter(2));
		AlphaSpreadVsIeta_SIC_EEM->SetBinError(iRing, f_SIC_EEM->GetParError(2));

		delete f_SIC_EEM;

	}

	for( unsigned int iRing = 0; iRing < histo_BTCP_EEM->size(); iRing++) {
		if(!(*histo_BTCP_EEM)[iRing]) continue;

		TF1* f_BTCP_EEM = new TF1("f_BTCP_EEM", "gaus", 0., 3.);
		f_BTCP_EEM->SetParameters((*histo_BTCP_EEM)[iRing]->GetEntries(), (*histo_BTCP_EEM)[iRing]->GetMean(), (*histo_BTCP_EEM)[iRing]->GetRMS());
		f_BTCP_EEM->SetRange((*histo_BTCP_EEM)[iRing]->GetMean() - 8 * (*histo_BTCP_EEM)[iRing]->GetRMS(), (*histo_BTCP_EEM)[iRing]->GetMean()
		                     + 8 * (*histo_BTCP_EEM)[iRing]->GetRMS());

		(*histo_BTCP_EEM)[iRing]->Fit("f_BTCP_EEM", "QR");

		AlphaSpreadVsIeta_BTCP_EEM->SetBinContent(iRing, f_BTCP_EEM->GetParameter(2));
		AlphaSpreadVsIeta_BTCP_EEM->SetBinError(iRing, f_BTCP_EEM->GetParError(2));

		delete f_BTCP_EEM;

	}


	for( std::map<int, TH1F*>::const_iterator itrechit = histo_SIC_EEP->begin(); itrechit != histo_SIC_EEP->end(); itrechit++) delete itrechit->second;
	for( std::map<int, TH1F*>::const_iterator itrechit = histo_SIC_EEM->begin(); itrechit != histo_SIC_EEM->end(); itrechit++) delete itrechit->second;
	for( std::map<int, TH1F*>::const_iterator itrechit = histo_BTCP_EEP->begin(); itrechit != histo_BTCP_EEP->end(); itrechit++) delete itrechit->second;
	for( std::map<int, TH1F*>::const_iterator itrechit = histo_BTCP_EEM->begin(); itrechit != histo_BTCP_EEM->end(); itrechit++) delete itrechit->second;

	delete histo_SIC_EEP;
	delete histo_SIC_EEM;
	delete histo_BTCP_EEP;
	delete histo_BTCP_EEM;
}


/// Save in the output
void XtalAlphaEE::saveHistos(TFile * f1)
{

	f1->cd();

	///service histos
	h_Alpha_hashedIndex_EE     ->Write();
	h_occupancy_hashedIndex_EE ->Write();
	hC_EoP                     ->Write(*f1);

	///EE+

	hC_AlphaValues_SIC_EEP         ->Write(*f1);
	hC_AlphaSpreadVsLoop_SIC_EEP   ->Write(*f1);
	hC_Alpha_SIC_EEP               ->Write("", *f1);
	hC_AlphaValues_BTCP_EEP         ->Write(*f1);
	hC_AlphaSpreadVsLoop_BTCP_EEP   ->Write(*f1);
	hC_Alpha_BTCP_EEP               ->Write("", *f1);

	h_Alpha_EEP             ->Write();
	h_scalib_EEP            ->Write();

	h_occupancy_EEP         ->Write();
	h_map_Dead_Channels_EEP ->Write();

	p_AlphaVsIeta_SIC_EEP       ->Write();
	AlphaSpreadVsIeta_SIC_EEP   ->Write();

	g_AlphameanVsLoop_SIC_EEP   ->Write("g_AlphameanVsLoop_SIC_EEP");
	g_AlpharmsVsLoop_SIC_EEP    ->Write("g_AlpharmsVsLoop_SIC_EEP");
	g_AlphaSigmaVsLoop_SIC_EEP  ->Write("g_AlphaSigmaVsLoop_SIC_EEP");

	p_AlphaVsIeta_BTCP_EEP       ->Write();
	AlphaSpreadVsIeta_BTCP_EEP   ->Write();

	g_AlphameanVsLoop_BTCP_EEP   ->Write("g_AlphameanVsLoop_BTCP_EEP");
	g_AlpharmsVsLoop_BTCP_EEP    ->Write("g_AlpharmsVsLoop_BTCP_EEP");
	g_AlphaSigmaVsLoop_BTCP_EEP  ->Write("g_AlphaSigmaVsLoop_BTCP_EEP");

///EE-

	hC_AlphaValues_SIC_EEM         ->Write(*f1);
	hC_AlphaSpreadVsLoop_SIC_EEM   ->Write(*f1);
	hC_Alpha_SIC_EEM               ->Write("", *f1);
	hC_AlphaValues_BTCP_EEM         ->Write(*f1);
	hC_AlphaSpreadVsLoop_BTCP_EEM   ->Write(*f1);
	hC_Alpha_BTCP_EEM               ->Write("", *f1);

	h_Alpha_EEM             ->Write();
	h_scalib_EEM            ->Write();

	h_occupancy_EEM         ->Write();
	h_map_Dead_Channels_EEM ->Write();

	p_AlphaVsIeta_SIC_EEM       ->Write();
	AlphaSpreadVsIeta_SIC_EEM   ->Write();

	g_AlphameanVsLoop_SIC_EEM   ->Write("g_AlphameanVsLoop_SIC_EEM");
	g_AlpharmsVsLoop_SIC_EEM    ->Write("g_AlpharmsVsLoop_SIC_EEM");
	g_AlphaSigmaVsLoop_SIC_EEM  ->Write("g_AlphaSigmaVsLoop_SIC_EEM");

	p_AlphaVsIeta_BTCP_EEM       ->Write();
	AlphaSpreadVsIeta_BTCP_EEM   ->Write();

	g_AlphameanVsLoop_BTCP_EEM   ->Write("g_AlphameanVsLoop_BTCP_EEM");
	g_AlpharmsVsLoop_BTCP_EEM    ->Write("g_AlpharmsVsLoop_BTCP_EEM");
	g_AlphaSigmaVsLoop_BTCP_EEM  ->Write("g_AlphaSigmaVsLoop_BTCP_EEM");

	f1->Close();

	return;
}

/// Save E/p distributions
void XtalAlphaEE::saveEoPeta(TFile * f2)
{

	f2->cd();

	hC_EoP_ir_ele ->Write(*f2);

	f2->Close();
}

/// Acquire fake Dead Xtal in order to study the effect of IC near them
void XtalAlphaEE::AcquireDeadXtal(TString inputDeadXtal)
{

	if(inputDeadXtal != "NULL") {
		std::ifstream DeadXtal (inputDeadXtal.Data(), std::ios::binary);

		std::string buffer;
		int iX, iY , iZ;

		while(!DeadXtal.eof()) {
			getline(DeadXtal, buffer);
			std::stringstream line( buffer );
			line >> iX >> iY >> iZ ;
			DeadXtal_HashedIndex.push_back(GetHashedIndexEE(iX, iY, iZ)) ;

		}

		sort(DeadXtal_HashedIndex.begin(), DeadXtal_HashedIndex.end());
	} else {
		DeadXtal_HashedIndex.push_back(-9999);
	}

}
/// Check if the channel considered is in the list of dead or not
bool XtalAlphaEE::CheckDeadXtal(const int & iX, const int & iY, const int & iZ)
{

	int hashed_Index;
	hashed_Index = GetHashedIndexEE(iX, iY, iZ);

	std::vector<int>::iterator iter = find(DeadXtal_HashedIndex.begin(), DeadXtal_HashedIndex.end(), hashed_Index);

	if(iter != DeadXtal_HashedIndex.end())
		return true;
	else return false;
}

