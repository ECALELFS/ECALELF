#include "Calibration/EOverPCalibration/interface/XtalAlphaEB.h"
#include "Calibration/EOverPCalibration/interface/GetHashedIndexEB.h"
#include <TH2.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TRandom.h>
#include <TRandom3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>


///==== Default constructor Contructor

XtalAlphaEB::XtalAlphaEB(TTree *tree, std::vector<TGraphErrors*> & inputMomentumScale, const std::string& typeEB, TString outEPDistribution):
	outEPDistribution_p(outEPDistribution)
{

	// if parameter tree is not specified (or zero), connect the file
	// used to generate this class and read the Tree.
	if (tree == 0) {
		TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/data1/dimatteo/Calibration/Ntuples/Run2011A/WZAnalysisSingleXtal/WZAnalysis_SingleElectron_Run2011A-WElectron-May10ReReco-v1.root");
		if (!f) {
			f = new TFile("/data1/dimatteo/Calibration/Ntuples/Run2011A/WZAnalysisSingleXtal/WZAnalysis_SingleElectron_Run2011A-WElectron-May10ReReco-v1.root");
		}
		tree = (TTree*)gDirectory->Get("ntu");

	}

	Init(tree);

	// Set my momentum scale using the input graphs
	myMomentumScale = inputMomentumScale;
	myTypeEB = typeEB;
}

///==== deconstructor

XtalAlphaEB::~XtalAlphaEB()
{

	if (!fChain) return;
	delete fChain->GetCurrentFile();
}


///==== Load information of input Ntupla

Long64_t XtalAlphaEB::LoadTree(Long64_t entry)
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

///==== Variables initialization

void XtalAlphaEB::Init(TTree *tree)
{
	/// Set object pointer

	ele1_recHit_E = 0;
	ele1_recHit_hashedIndex = 0;
	ele1_recHit_iphiORiy = 0;
	ele1_recHit_ietaORix = 0;
	ele1_recHit_flag = 0;
	ele1_recHit_LaserCorr = 0;
	ele1_recHit_Alpha = 0;


	ele2_recHit_E = 0;
	ele2_recHit_hashedIndex = 0;
	ele2_recHit_iphiORiy = 0;
	ele2_recHit_ietaORix = 0;
	ele2_recHit_flag = 0;
	ele2_recHit_LaserCorr = 0;
	ele2_recHit_Alpha = 0;

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
	fChain->SetBranchStatus("ele1_recHit_ietaORix", 1);
	fChain->SetBranchAddress("ele1_recHit_ietaORix", &ele1_recHit_ietaORix, &b_ele1_recHit_ietaORix);
	fChain->SetBranchStatus("ele1_recHit_iphiORiy", 1);
	fChain->SetBranchAddress("ele1_recHit_iphiORiy", &ele1_recHit_iphiORiy, &b_ele1_recHit_iphiORiy);
	fChain->SetBranchStatus("ele1_recHit_flag", 1);
	fChain->SetBranchAddress("ele1_recHit_flag", &ele1_recHit_flag, &b_ele1_recHit_flag);
	fChain->SetBranchStatus("ele1_recHit_laserCorrection", 1);
	fChain->SetBranchAddress("ele1_recHit_laserCorrection", &ele1_recHit_LaserCorr, &b_ele1_recHit_LaserCorr);
	fChain->SetBranchStatus("ele1_recHit_Alpha", 1);
	fChain->SetBranchAddress("ele1_recHit_Alpha", &ele1_recHit_Alpha, &b_ele1_recHit_Alpha);

	//fChain->SetBranchStatus("ele1_E_true", 1);             fChain->SetBranchAddress("ele1_E_true", &ele1_E_true, &b_ele1_E_true);
	//fChain->SetBranchStatus("ele1_DR", 1);                 fChain->SetBranchAddress("ele1_DR", &ele1_DR, &b_ele1_DR);

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
	fChain->SetBranchStatus("ele1_scE_regression", 1);
	fChain->SetBranchAddress("ele1_scE_regression", &ele1_scE_regression, &b_ele1_scE_regression);

	fChain->SetBranchStatus("ele1_es", 1);
	fChain->SetBranchAddress("ele1_es", &ele1_es, &b_ele1_es);
	fChain->SetBranchStatus("ele1_charge", 1);
	fChain->SetBranchAddress("ele1_charge", &ele1_charge, &b_ele1_charge);
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
	fChain->SetBranchStatus("ele1_isEBEtaGap", 1);
	fChain->SetBranchAddress("ele1_isEBEtaGap", &ele1_isEBEtaGap, &b_ele1_isEBEtaGap);
	fChain->SetBranchStatus("ele1_isEBPhiGap", 1);
	fChain->SetBranchAddress("ele1_isEBPhiGap", &ele1_isEBPhiGap, &b_ele1_isEBPhiGap);
	fChain->SetBranchStatus("ele1_isEEDeeGap", 1);
	fChain->SetBranchAddress("ele1_isEEDeeGap", &ele1_isEEDeeGap, &b_ele1_isEEDeeGap);
	fChain->SetBranchStatus("ele1_isEERingGap", 1);
	fChain->SetBranchAddress("ele1_isEERingGap", &ele1_isEERingGap, &b_ele1_isEERingGap);


	fChain->SetBranchStatus("ele1_seedLaserAlpha", 1);
	fChain->SetBranchAddress("ele1_seedLaserAlpha", &ele1_seedLaserAlpha, &b_ele1_seedLaserAlpha);
	fChain->SetBranchStatus("ele1_seedLaserCorr", 1);
	fChain->SetBranchAddress("ele1_seedLaserCorr", &ele1_seedLaserCorr, &b_ele1_seedLaserCorr);

	// ele2
	fChain->SetBranchStatus("ele2_recHit_E", 1);
	fChain->SetBranchAddress("ele2_recHit_E", &ele2_recHit_E, &b_ele2_recHit_E);
	fChain->SetBranchStatus("ele2_recHit_hashedIndex", 1);
	fChain->SetBranchAddress("ele2_recHit_hashedIndex", &ele2_recHit_hashedIndex, &b_ele2_recHit_hashedIndex);
	fChain->SetBranchStatus("ele2_recHit_iphiORiy", 1);
	fChain->SetBranchAddress("ele2_recHit_iphiORiy", &ele2_recHit_iphiORiy, &b_ele2_recHit_iphiORiy);
	fChain->SetBranchStatus("ele2_recHit_ietaORix", 1);
	fChain->SetBranchAddress("ele2_recHit_ietaORix", &ele2_recHit_ietaORix, &b_ele2_recHit_ietaORix);
	fChain->SetBranchStatus("ele2_recHit_flag", 1);
	fChain->SetBranchAddress("ele2_recHit_flag", &ele2_recHit_flag, &b_ele2_recHit_flag);
	fChain->SetBranchStatus("ele2_recHit_laserCorrection", 1);
	fChain->SetBranchAddress("ele2_recHit_laserCorrection", &ele2_recHit_LaserCorr, &b_ele2_recHit_LaserCorr);
	fChain->SetBranchStatus("ele2_recHit_Alpha", 1);
	fChain->SetBranchAddress("ele2_recHit_Alpha", &ele2_recHit_Alpha, &b_ele2_recHit_Alpha);

	//fChain->SetBranchStatus("ele2_E_true", 1);             fChain->SetBranchAddress("ele2_E_true", &ele2_E_true, &b_ele2_E_true);
	//fChain->SetBranchStatus("ele2_DR", 1);                 fChain->SetBranchAddress("ele2_DR", &ele2_DR, &b_ele2_DR);
	fChain->SetBranchStatus("ele2_charge", 1);
	fChain->SetBranchAddress("ele2_charge", &ele2_charge, &b_ele2_charge);

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
	fChain->SetBranchStatus("ele1_scE_regression", 1);
	fChain->SetBranchAddress("ele1_scE_regression", &ele1_scE_regression, &b_ele1_scE_regression);

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

	fChain->SetBranchStatus("ele2_seedLaserAlpha", 1);
	fChain->SetBranchAddress("ele2_seedLaserAlpha", &ele2_seedLaserAlpha, &b_ele2_seedLaserAlpha);
	fChain->SetBranchStatus("ele2_seedLaserCorr", 1);
	fChain->SetBranchAddress("ele2_seedLaserCorr", &ele2_seedLaserCorr, &b_ele2_seedLaserCorr);

}



//! Declaration of the objects that are save in the output file

void XtalAlphaEB::bookHistos(int nLoops)
{

	hC_AlphaValues          = new hChain ("AlphaValues", "AlphaValues", 2000, 0.5, 1.5, nLoops);

	hC_EoP                  = new hChain ("EoP", "EoP", 100, 0.2, 1.9, nLoops);

	hC_AlphaSpreadVsLoop    = new hChain ("hC_AlphaSpreadVsLoop", "hC_AlphaSpreadVsLoop", 171, -85, 86, nLoops);

	hC_Alpha_EB             = new h2Chain("hC_Alpha_EB", "hC_Alpha_EB", 360, 1, 361, 171, -85, 86, nLoops );

	h_Alpha_EB              = new TH2F("h_Alpha_EB", "h_Alpha_EB", 360, 1, 361, 171, -85, 86 );

	h_Intial_AlphaValues    = new TH2F("h_Intial_AlphaValues", "h_Intial_AlphaValues", 360, 1, 361, 171, -85, 86);

	h_Alpha_scalib_EB       = new TH2F("h_Alpha_scalib_EB", "h_Alpha_scalib_EB", 360, 1, 361, 171, -85, 86 );

	p_AlphaValues_iEta      = new TProfile ("p_AlphaValues_iEta", "p_AlphaValues_iEta", 171, -85, 86, -0.1, 2.1);

	h_AlphaSpread_iEta      = new TH1F ("h_AlphaSpread_iEta", "h_AlphaSpread_iEta", 171, -85, 86);

	h_Alpha_EB_hashedIndex  = new TH1F("h_Alpha_EB_hashedIndex", "h_Alpha_EB_hashedIndex", 61201, -0.5, 61999.5 );

	h_map_Dead_Channels     = new TH2F("h_map_Dead_Channels", "h_map_Dead_Channels", 360, 1, 361, 171, -85, 86);

	g_AlphameanVsLoop       = new TGraphErrors();
	g_AlphameanVsLoop       -> SetName("g_AlphameanVsLoop");
	g_AlphameanVsLoop       -> SetTitle("g_AlphameanVsLoop");

	g_AlpharmsVsLoop        = new TGraphErrors();
	g_AlpharmsVsLoop        ->SetName("g_AlpharmsVsLoop");
	g_AlpharmsVsLoop        ->SetTitle("g_AlpharmsVsLoop");

	g_AlphaSigmaVsLoop        = new TGraphErrors();
	g_AlphaSigmaVsLoop      ->SetName("g_AlphaSigmaVsLoop");
	g_AlphaSigmaVsLoop      ->SetTitle("g_AlphaSigmaVsLoop");

	h_Alpha_EB_meanOnPhi    = new TH2F("h_Alpha_EB_meanOnPhi", "h_Alpha_EB_meanOnPhi", 360, 1, 361, 171, -85, 86 );

	h_Occupancy_hashedIndex = new TH1F ("h_Occupancy_hashedIndex", "h_Occupancy_hashedIndex", 61201, -0.5, 61199.5);

	h_occupancy             = new TH2F("h_occupancy", "h_occupancy", 360, 1, 361, 171, -85, 86 );

	return;
}

//! Build E/p distribution for both ele1 and ele2

void XtalAlphaEB::BuildEoPeta_ele(int iLoop, int nentries , int useW, int useZ, std::vector<float> theScalibration, bool isSaveEPDistribution, bool isR9selection, float R9Min, bool isMCTruth)
{

	if(iLoop == 0)  { // Set Chain fo Histogram for E/p distribution

		TString name = Form ("hC_EoP_eta_%d", iLoop);
		hC_EoP_eta_ele = new hChain (name, name, 100, 0.2, 1.9, 171);
	} else {
		hC_EoP_eta_ele -> Reset();
		TString name = Form ("hC_EoP_eta_%d", iLoop);
		hC_EoP_eta_ele = new hChain (name, name, 100, 0.2, 1.9, 171);
	}


	Long64_t nbytes = 0, nb = 0;

	for (Long64_t jentry = 0; jentry < nentries; jentry++) {

		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;

		nb = fChain->GetEntry(jentry);
		nbytes += nb;
		if (!(jentry % 1000000))std::cerr << "building E/p distribution ----> " << jentry << " vs " << nentries << std::endl;

		float pIn, FdiEta;

		///! Tight electron from W or Z only barrel

		if ( ele1_isEB == 1 && (( useW == 1 && isW == 1 ) ||  ( useZ == 1 && isZ == 1 ))) {

			FdiEta = ele1_scE / ele1_scERaw; /// FEta approximation

			float thisE = 0;
			int   iseed = 0 ;
			int   seed_hashedIndex = 0;
			float E_seed = 0;
			float thisE3x3 = 0;

			/// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy

			for (unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {

				float thisAlpha = 1.;
				int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);

				if(ele1_recHit_E -> at(iRecHit) > E_seed && ele1_recHit_LaserCorr->at(iRecHit) > 0. && ele1_recHit_Alpha -> at(iRecHit) > 0.) { /// control if this recHit is good
					seed_hashedIndex = ele1_recHit_hashedIndex -> at(iRecHit);
					iseed = iRecHit;
					E_seed = ele1_recHit_E -> at(iRecHit); ///! Seed search
				}

				if (iLoop > 0) thisAlpha = h_Alpha_EB_hashedIndex -> GetBinContent(thisIndex + 1);

				thisE += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta * TMath::Power(ele1_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.);

			}

			for (unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {

				float thisAlpha = 1.;
				int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);

				///! 3x3 matrix informations in order to apply R9 selection

				if (iLoop > 0) thisAlpha = h_Alpha_EB_hashedIndex -> GetBinContent(thisIndex + 1);

				if(fabs(ele1_recHit_ietaORix->at(iRecHit) - ele1_recHit_ietaORix->at(iseed)) <= 1 &&
				        fabs(ele1_recHit_iphiORiy->at(iRecHit) - ele1_recHit_iphiORiy->at(iseed)) <= 1 && ele1_recHit_LaserCorr -> at(iRecHit) > 0. &&
				        ele1_recHit_Alpha -> at(iRecHit) > 0.
				  )
					thisE3x3 += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta * TMath::Power(ele1_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.);
			}

			///! Eta seed from hashed index
			int eta_seed = GetIetaFromHashedIndex(seed_hashedIndex);

			bool skipElectron = false;

			///! different E/p if I am using MCThruth informations or not
			if(!isMCTruth)  {

				pIn = ele1_tkP;
				int regionId = templIndexEB(myTypeEB, ele1_eta, ele1_charge, thisE3x3 / thisE);
				pIn /= myMomentumScale[regionId] -> Eval( ele1_phi );
			} else {
				pIn = ele1_E_true;
				if(fabs(ele1_DR) > 0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
			}

			/// R9 Selection
			if( fabs(thisE3x3 / thisE) < R9Min && isR9selection == true ) skipElectron = true;

			/// Save electron E/p in a chain of histogramm each for eta bin
			if(!skipElectron)    hC_EoP_eta_ele -> Fill(eta_seed + 85, thisE / pIn);


		}
		///=== Second medium electron from Z

		if ( ele2_isEB == 1 && (( useW == 1 && isW == 1 ) || ( useZ == 1 && isZ == 1 )) ) {

			FdiEta = ele2_scE / ele2_scERaw; /// FEta approximation

			float thisE = 0;
			int   iseed = 0 ;
			int seed_hashedIndex = 0;
			float E_seed = 0;
			float thisE3x3 = 0;

			/// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy

			for (unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {

				float thisAlpha = 1.;
				int thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);

				if(ele2_recHit_E -> at(iRecHit) > E_seed && ele2_recHit_LaserCorr -> at(iRecHit) > 0. && ele2_recHit_Alpha -> at(iRecHit) > 0.) {
					seed_hashedIndex = ele2_recHit_hashedIndex -> at(iRecHit);
					iseed = iRecHit;
					E_seed = ele2_recHit_E -> at(iRecHit); ///Seed informations
				}


				if (iLoop > 0) thisAlpha = h_Alpha_EB_hashedIndex -> GetBinContent(thisIndex + 1);

				thisE += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta * TMath::Power(ele2_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.);

			}

			for (unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {

				float thisAlpha = 1.;
				int thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);

				if (iLoop > 0) thisAlpha = h_Alpha_EB_hashedIndex -> GetBinContent(thisIndex + 1);

				if(fabs(ele2_recHit_ietaORix->at(iRecHit) - ele2_recHit_ietaORix->at(iseed)) <= 1 &&
				        fabs(ele2_recHit_iphiORiy->at(iRecHit) - ele2_recHit_iphiORiy->at(iseed)) <= 1 && ele2_recHit_LaserCorr -> at(iRecHit) > 0. &&
				        ele2_recHit_Alpha -> at(iRecHit) > 0.)
					thisE3x3 += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta * TMath::Power(ele2_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.);
			}


			/// Eta seed info from hashed index
			int eta_seed = GetIetaFromHashedIndex(seed_hashedIndex);

			bool skipElectron = false;

			/// MCTruth analysis option
			if(!isMCTruth)  {

				pIn = ele2_tkP;
				int regionId = templIndexEB(myTypeEB, ele2_eta, ele2_charge, thisE3x3 / thisE);
				pIn /= myMomentumScale[regionId] -> Eval( ele2_phi );
			} else {
				pIn = ele2_E_true;
				if(fabs(ele2_DR) > 0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
			}

			///R9 Selection
			if( fabs(thisE3x3 / thisE) < R9Min && isR9selection == true ) skipElectron = true;
			/// Save E/p electron information
			if(!skipElectron) hC_EoP_eta_ele -> Fill(eta_seed + 85, thisE / pIn);

		}


	}

/// Histogramm Normalization
	for(unsigned int ieta = 0 ; ieta < hC_EoP_eta_ele->Size() ; ieta++) {

		hC_EoP_eta_ele->Normalize(ieta);
	}

/// Save E/p pdf if it is required
	if(isSaveEPDistribution == true) {

		TFile *f2 = new TFile(outEPDistribution_p.Data(), "UPDATE");
		saveEoPeta(f2);
	}

}


/// Calibration Loop over the ntu events

void XtalAlphaEB::Loop(int nentries, int useZ, int useW, int splitStat, int nLoops, bool isMiscalib, bool isSaveEPDistribution,
                       bool isEPselection, bool isR9selection, float R9Min, bool isMCTruth, std::map<int, std::vector<std::pair<int, int> > > jsonMap)
{
	if (fChain == 0) return;

	/// Define the number of crystal you want to calibrate
	int m_regions = 0;


	/// Define useful numbers
	static const int MIN_IETA = 1;
	static const int MIN_IPHI = 1;
	static const int MAX_IETA = 85;
	static const int MAX_IPHI = 360;

	for ( int iabseta = MIN_IETA; iabseta <= MAX_IETA; iabseta++ ) {
		for ( int iphi = MIN_IPHI; iphi <= MAX_IPHI; iphi++ ) {
			for ( int theZside = -1; theZside < 2; theZside = theZside + 2 ) {

				m_regions++;

			}
		}
	}

	/// Barrel region = Barrel xtal
	std::cout << "m_regions " << m_regions << std::endl;

	/// Build the scalibration Map for MC Analysis

	std::vector<float> theScalibration(m_regions, 0.);
	TRandom3 genRand;

	for ( int iIndex = 0; iIndex < m_regions; iIndex++ )  {

		bool isDeadXtal = false ;

		/// Save Map of DeadXtal and put the scalibration value = 0 in order to skip them in the calibration procedure -> Fake dead list given by user

		if(DeadXtal_HashedIndex.at(0) != -9999) isDeadXtal = CheckDeadXtal(GetIetaFromHashedIndex(iIndex), GetIphiFromHashedIndex(iIndex));
		if(isDeadXtal == true ) {
			theScalibration[iIndex] = 0;
			h_map_Dead_Channels->Fill(GetIphiFromHashedIndex(iIndex), GetIetaFromHashedIndex(iIndex));
		} else {

			if(isMiscalib == true)  theScalibration[iIndex] = genRand.Gaus(1., 0.01); ///! 1% of Miscalibration fixed
			if(isMiscalib == false) theScalibration[iIndex] = 1.;
			h_Alpha_scalib_EB -> Fill ( GetIphiFromHashedIndex(iIndex), GetIetaFromHashedIndex(iIndex), theScalibration[iIndex] ); ///! Scalib map
		}
	}

	/// ----------------- Calibration Loops -----------------------------//

	for ( int iLoop = 0; iLoop < nLoops; iLoop++ ) {

		std::cout << "Starting iteration " << iLoop + 1 << std::endl;

		/// prepare the numerator and denominator for each Xtal

		std::vector<float> theNumerator(m_regions, 0.);
		std::vector<float> theDenominator(m_regions, 0.);

		std::cout << "Number of analyzed events = " << nentries << std::endl;

		///==== build E/p distribution ele 1 and 2
		BuildEoPeta_ele(iLoop, nentries, useW, useZ, theScalibration, isSaveEPDistribution, isR9selection, R9Min, isMCTruth);

		// define map with events
		std::map<std::pair<int, std::pair<int, int> >, int> eventsMap;

		/// Loop on each entry
		Long64_t nbytes = 0, nb = 0;
		for (Long64_t jentry = 0; jentry < nentries; jentry++) {

			if (!(jentry % 100000))std::cerr << jentry;
			if (!(jentry % 10000)) std::cerr << ".";

			Long64_t ientry = LoadTree(jentry);
			if (ientry < 0) break;
			nb = fChain->GetEntry(jentry);
			nbytes += nb;


			//*********************************
			// JSON FILE AND DUPLIACTES IN DATA

			bool skipEvent = false;
			if( isMCTruth == 0 ) {

				if(AcceptEventByRunAndLumiSection(runId, lumiId, jsonMap) == false) skipEvent = true;

				std::pair<int, Long64_t> eventLSandID(lumiId, eventId);
				std::pair<int, std::pair<int, Long64_t> > eventRUNandLSandID(runId, eventLSandID);
				if( eventsMap[eventRUNandLSandID] == 1 ) skipEvent = true;
				else eventsMap[eventRUNandLSandID] = 1;
			}

			if( skipEvent == true ) continue;



			float pIn, pSub, FdiEta;

			std::map<int, double> map;
			bool skipElectron = false;

			/// Tight electron information from W and Z, it depends on the flag variable isW, isZ

			if ( ele1_isEB == 1 && (( useW == 1 && isW == 1 ) || ( useZ == 1 && isZ == 1 )) ) {

				/// SCL energy containment correction
				FdiEta = ele1_scE / ele1_scERaw;

				float thisE = 0;
				int iseed = 0 ;
				float E_seed = 0;
				int seed_hashedIndex = 0;
				float thisE3x3 = 0 ;

				/// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
				for (unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {

					float thisAlpha = 1.;
					int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);

					if (iLoop > 0 ) thisAlpha = h_Alpha_EB_hashedIndex -> GetBinContent(thisIndex + 1);

					if(ele1_recHit_LaserCorr -> at(iRecHit) > 0. && ele1_recHit_Alpha -> at(iRecHit) > 0.)
						thisE += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta * TMath::Power(ele1_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.);

					if(ele1_recHit_E -> at(iRecHit) > E_seed && ele1_recHit_LaserCorr -> at(iRecHit) > 0. && ele1_recHit_Alpha -> at(iRecHit) > 0.) {

						E_seed = ele1_recHit_E -> at(iRecHit);
						iseed = iRecHit;
						seed_hashedIndex = ele1_recHit_hashedIndex -> at(iRecHit); //! Seed Infos
					}

				}

				for (unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {

					float thisAlpha = 1.;
					int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);

					if (iLoop > 0) thisAlpha = h_Alpha_EB_hashedIndex -> GetBinContent(thisIndex + 1);

					if(fabs(ele1_recHit_ietaORix->at(iRecHit) - ele1_recHit_ietaORix->at(iseed)) <= 1 &&
					        fabs(ele1_recHit_iphiORiy->at(iRecHit) - ele1_recHit_iphiORiy->at(iseed)) <= 1 && ele1_recHit_LaserCorr -> at(iRecHit) > 0. &&
					        ele1_recHit_Alpha -> at(iRecHit) > 0.)
						thisE3x3 += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * FdiEta * TMath::Power(ele1_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.);
				}

				// Iniatila Map of Alpha Values

				if(iLoop == 0) {
					for ( unsigned int iRecHit = 0; iRecHit < ele1_recHit_Alpha->size(); iRecHit++) {
						int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);
						int ieta = GetIetaFromHashedIndex(thisIndex);
						int iphi = GetIphiFromHashedIndex(thisIndex);
						if(h_Intial_AlphaValues->GetBinContent(iphi, fabs(ieta + 85)) != 0) continue;
						h_Intial_AlphaValues -> SetBinContent(iphi, fabs(ieta + 85), ele1_recHit_Alpha->at(iRecHit));
					}
				}



				pSub = 0.; //NOTALEO : test dummy
				bool skipElectron = false;

				///! if MCTruth Analysis
				if(!isMCTruth)  {

					pIn = ele1_tkP;
					int regionId = templIndexEB(myTypeEB, ele1_eta, ele1_charge, thisE3x3 / thisE);
					pIn /= myMomentumScale[regionId] -> Eval( ele1_phi );
				} else {
					pIn = ele1_E_true;
					if(fabs(ele1_DR) > 0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
				}

				/// Take the correct pdf for the ring in order to reweight the events in L3
				int eta_seed = GetIetaFromHashedIndex(seed_hashedIndex);
				TH1F* EoPHisto = hC_EoP_eta_ele->GetHisto(eta_seed + 85);

				/// Basic selection on E/p or R9 if you want to apply
				if( fabs(thisE / pIn  - 1) > 0.3 && isEPselection == true ) skipElectron = true;
				if( fabs(thisE3x3 / thisE) < R9Min && isR9selection == true ) skipElectron = true;
				if( thisE / pIn < EoPHisto->GetXaxis()->GetXmin() || thisE / pIn > EoPHisto->GetXaxis()->GetXmax()) skipElectron = true;
				if( !skipElectron) {

					/// Now cycle on the all the recHits and update the numerator and denominator
					for ( unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {

						int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);
						float thisAlpha = 1.;

						if (iLoop > 0) thisAlpha = h_Alpha_EB_hashedIndex -> GetBinContent(thisIndex + 1);

						/// Fill the occupancy map JUST for the first Loop
						if ( iLoop == 0 ) {
							h_Occupancy_hashedIndex -> Fill(thisIndex);
							h_occupancy -> Fill(GetIphiFromHashedIndex(thisIndex), GetIetaFromHashedIndex(thisIndex));
						}

						/// use full statistics
						if ( splitStat == 0 ) {

							int EoPbin = EoPHisto->FindBin(thisE / (pIn - pSub)); /// factor use to reweight the evemts
							theNumerator[thisIndex] += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * TMath::Power(ele1_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.) * FdiEta * 1 / thisE * (pIn - pSub) / thisE * EoPHisto->GetBinContent(EoPbin);
							theDenominator[thisIndex] += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * TMath::Power(ele1_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.) * FdiEta * 1 / thisE * EoPHisto->GetBinContent(EoPbin);

						}
						/// Use Half Statistic only even
						else if ( splitStat == 1 && jentry % 2 == 0 ) {
							int EoPbin = EoPHisto->FindBin(thisE / (pIn - pSub));
							theNumerator[thisIndex] += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * TMath::Power(ele1_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.) * FdiEta * 1 / thisE * (pIn - pSub) / thisE * EoPHisto->GetBinContent(EoPbin);
							theDenominator[thisIndex] += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * TMath::Power(ele1_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.) * FdiEta * 1 / thisE * EoPHisto->GetBinContent(EoPbin);
						}
						/// use odd event
						else if ( splitStat == -1 && jentry % 2 != 0 ) {
							int EoPbin = EoPHisto->FindBin(thisE / (pIn - pSub));
							theNumerator[thisIndex] += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * TMath::Power(ele1_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.) * FdiEta * 1 / thisE * (pIn - pSub) / thisE * EoPHisto->GetBinContent(EoPbin);
							theDenominator[thisIndex] += theScalibration[thisIndex] * ele1_recHit_E -> at(iRecHit) * TMath::Power(ele1_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.) * FdiEta * 1 / thisE * EoPHisto->GetBinContent(EoPbin);
						}

					}

				}
				//Fill EoP
				hC_EoP -> Fill(iLoop, thisE / pIn);

			}

			skipElectron = false;

			/// Ele2 medium from Z only Barrel
			if ( ele2_isEB == 1 && (( useW == 1 && isW == 1 ) || ( useZ == 1 && isZ == 1 )) ) {

				FdiEta = ele2_scE / ele2_scERaw;
				// Electron energy
				float thisE = 0;
				int iseed = 0 ;
				float E_seed = 0;
				int seed_hashedIndex = 0;
				float thisE3x3 = 0;

				/// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
				for (unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {

					float thisAlpha = 1.;
					int thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);
					if (iLoop > 0) thisAlpha = h_Alpha_EB_hashedIndex -> GetBinContent(thisIndex + 1);

					if( ele2_recHit_LaserCorr -> at(iRecHit) > 0. && ele2_recHit_Alpha -> at(iRecHit) > 0.)
						thisE += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta * TMath::Power(ele2_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.);


					if(ele2_recHit_E -> at(iRecHit) > E_seed && ele2_recHit_LaserCorr -> at(iRecHit) > 0. && ele2_recHit_Alpha -> at(iRecHit) > 0.) {
						E_seed = ele2_recHit_E -> at(iRecHit);
						iseed = iRecHit;
						seed_hashedIndex = ele2_recHit_hashedIndex -> at(iRecHit); /// Seed information
					}


				}

				for (unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {

					float thisAlpha = 1.;
					int thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);
					// IC obtained from previous Loops
					if (iLoop > 0) thisAlpha = h_Alpha_EB_hashedIndex -> GetBinContent(thisIndex + 1);


					if(fabs(ele2_recHit_ietaORix->at(iRecHit) - ele2_recHit_ietaORix->at(iseed)) <= 1 &&
					        fabs(ele2_recHit_iphiORiy->at(iRecHit) - ele2_recHit_iphiORiy->at(iseed)) <= 1 && ele2_recHit_LaserCorr -> at(iRecHit) > 0. &&
					        ele2_recHit_Alpha -> at(iRecHit) > 0.)

						thisE3x3 += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * FdiEta * TMath::Power(ele2_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.);

				}

				if(iLoop == 0) {
					for ( unsigned int iRecHit = 0; iRecHit < ele2_recHit_Alpha->size(); iRecHit++) {
						int thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);
						int ieta = GetIetaFromHashedIndex(thisIndex);
						int iphi = GetIphiFromHashedIndex(thisIndex);

						if(h_Intial_AlphaValues->GetBinContent(iphi, fabs(ieta + 85)) != 0) continue;
						h_Intial_AlphaValues   -> SetBinContent(iphi, fabs(ieta + 85), ele2_recHit_Alpha->at(iRecHit));

					}
				}

				pSub = 0.; //NOTALEO : test dummy

				///! Option for MCTruth analysis
				if(!isMCTruth) {
					pIn = ele2_tkP;
					int regionId = templIndexEB(myTypeEB, ele2_eta, ele2_charge, thisE3x3 / thisE);
					pIn /= myMomentumScale[regionId] -> Eval( ele2_phi );
				} else {
					pIn = ele2_E_true;
					if(fabs(ele2_DR) > 0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
				}
				int eta_seed = GetIetaFromHashedIndex(seed_hashedIndex);
				TH1F* EoPHisto = hC_EoP_eta_ele->GetHisto(eta_seed + 85);

				/// discard electrons with bad E/P or R9
				if( thisE / pIn  < EoPHisto->GetXaxis()->GetXmin() || thisE / pIn  > EoPHisto->GetXaxis()->GetXmax()) skipElectron = true;
				if( fabs(thisE / pIn  - 1) > 0.3 && isEPselection == true ) skipElectron = true;
				if( fabs(thisE3x3 / thisE) < R9Min && isR9selection == true ) skipElectron = true;

				if( !skipElectron ) {

					/// Now cycle on the all the recHits and update the numerator and denominator
					for ( unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {


						int thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);
						float thisAlpha = 1.;

						if (iLoop > 0) thisAlpha = h_Alpha_EB_hashedIndex -> GetBinContent(thisIndex + 1);

						/// Fill the occupancy map JUST for the first Loop
						if ( iLoop == 0 ) {
							h_Occupancy_hashedIndex -> Fill(thisIndex);
							h_occupancy -> Fill(GetIphiFromHashedIndex(thisIndex), GetIetaFromHashedIndex(thisIndex));
						}

						/// use full statistics
						if ( splitStat == 0 ) {

							int EoPbin = EoPHisto->FindBin(thisE / (pIn - pSub));
							theNumerator[thisIndex] += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * TMath::Power(ele2_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.) * FdiEta * 1 / thisE * (pIn - pSub) / thisE * EoPHisto->GetBinContent(EoPbin);
							theDenominator[thisIndex] += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * TMath::Power(ele2_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.) * FdiEta * 1 / thisE * EoPHisto->GetBinContent(EoPbin);
						}
						/// use evens
						else if ( splitStat == 1 && jentry % 2 == 0 ) {
							int EoPbin = EoPHisto->FindBin(thisE / (pIn - pSub));
							theNumerator[thisIndex] += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * TMath::Power(ele2_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.) * FdiEta * 1 / thisE * (pIn - pSub) / thisE * EoPHisto->GetBinContent(EoPbin);
							theDenominator[thisIndex] += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * TMath::Power(ele2_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.) * FdiEta * 1 / thisE * EoPHisto->GetBinContent(EoPbin);
						}
						/// use odds
						else if ( splitStat == -1 && jentry % 2 != 0 ) {
							int EoPbin = EoPHisto->FindBin(thisE / (pIn - pSub));
							theNumerator[thisIndex] += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * TMath::Power(ele2_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.) * FdiEta * 1 / thisE * (pIn - pSub) / thisE * EoPHisto->GetBinContent(EoPbin);
							theDenominator[thisIndex] += theScalibration[thisIndex] * ele2_recHit_E -> at(iRecHit) * TMath::Power(ele2_recHit_LaserCorr->at(iRecHit), thisAlpha - 1.) * FdiEta * 1 / thisE * EoPHisto->GetBinContent(EoPbin);
						}

					}

				}
				//Fill EoP
				hC_EoP -> Fill(iLoop, thisE / pIn);

			}

		}

		///! End Cycle on the events

		///New Loop cycle + Save info
		std::cout << ">>>>> [L3][endOfLoop] entering..." << std::endl;

		TH1F auxiliary_Alpha("auxiliary_Alpha", "auxiliary_Alpha", 50, 0.2, 1.9);

		TF1* f1 = new TF1("f1", "gaus", 0, 5);

		///Fill the histo of IntercalibValues before the solve
		for ( int iIndex = 0; iIndex < m_regions; iIndex++ ) {

			if ( h_Occupancy_hashedIndex -> GetBinContent(iIndex + 1) > 0 ) {

				float thisAlphaConstant = 1.;
				/// Solve the cases where the recHit energy is always 0 (dead Xtal?)
				bool isDeadXtal = false ;
				if(DeadXtal_HashedIndex.at(0) != -9999) isDeadXtal = CheckDeadXtal(GetIetaFromHashedIndex(iIndex), GetIphiFromHashedIndex(iIndex));
				if(isDeadXtal == true ) continue;


				if (theNumerator[iIndex] / theDenominator[iIndex] > 0 && theDenominator[iIndex] != 0) thisAlphaConstant = theNumerator[iIndex] / theDenominator[iIndex];
				float oldAlphaConstant = 1.;
				if ( iLoop > 0 ) oldAlphaConstant = h_Alpha_EB_hashedIndex -> GetBinContent (iIndex + 1);

				h_Alpha_EB_hashedIndex -> SetBinContent(iIndex + 1, thisAlphaConstant * oldAlphaConstant); /// Alpha product useful for L3 methods
				hC_AlphaValues         -> Fill(iLoop, thisAlphaConstant); /// Alpha correction distribution at each loop
				hC_Alpha_EB            -> Fill(iLoop, GetIphiFromHashedIndex(iIndex), GetIetaFromHashedIndex(iIndex), thisAlphaConstant * oldAlphaConstant);
				///Save the new IC coefficient
				auxiliary_Alpha.Fill(thisAlphaConstant);
			}

		}
		/// Info in order to test convergence
		g_AlphameanVsLoop -> SetPoint(iLoop, iLoop, auxiliary_Alpha . GetMean());
		g_AlphameanVsLoop -> SetPointError(iLoop, 0., auxiliary_Alpha . GetMeanError());

		g_AlpharmsVsLoop -> SetPoint(iLoop, iLoop, auxiliary_Alpha . GetRMS());
		g_AlpharmsVsLoop -> SetPointError(iLoop, 0., auxiliary_Alpha . GetRMSError());

		auxiliary_Alpha.Fit("f1", "QR");

		g_AlphaSigmaVsLoop -> SetPoint(iLoop, iLoop,   f1->GetParameter(2));
		g_AlphaSigmaVsLoop -> SetPointError(iLoop, 0., f1->GetParError(2));

		for ( int iabseta = MIN_IETA; iabseta < MAX_IETA + 1; iabseta++ ) {
			for ( int theZside = -1; theZside < 2; theZside = theZside + 2 ) {

				TH1F* histoTemp = new TH1F("histoTemp", "histoTemp", 800, 0., 3.);
				///Setup the histo for fit
				for ( int iphi = MIN_IPHI; iphi <= MAX_IPHI; iphi++ ) {
					int hashedIndex = GetHashedIndexEB(iabseta * theZside, iphi, theZside);
					if ( h_Occupancy_hashedIndex -> GetBinContent(hashedIndex + 1) == 0 ) continue;
					histoTemp->Fill(h_Alpha_EB_hashedIndex->GetBinContent(hashedIndex + 1));
				}

				TF1* f2 = new TF1("f2", "gaus", 0, 5);
				f2->SetParameters(histoTemp->GetEntries(), histoTemp->GetMean(), histoTemp->GetRMS());
				f2->SetRange(histoTemp->GetMean() - 8 * histoTemp->GetRMS(), histoTemp->GetMean() + 8 * histoTemp->GetRMS());
				histoTemp -> Fit("f2", "QR");
				hC_AlphaSpreadVsLoop -> SetBinContent(iLoop, iabseta * theZside + 85 + 1, f2->GetParameter(2));
				hC_AlphaSpreadVsLoop -> SetBinError(iLoop,  iabseta * theZside + 85 + 1, f2->GetParError(2) );

				delete histoTemp;
				delete f2;
			}
		}

		delete f1;
	}/// end calibration loop

	int myPhiIndex = 0;

	for ( int iabseta = MIN_IETA; iabseta < MAX_IETA + 1; iabseta++ ) {
		for ( int theZside = -1; theZside < 2; theZside = theZside + 2 ) {

			///Setup the histo for fit
			TH1F histoAuxiliary("histoAuxiliary", "histoAuxiliary", 800, 0., 3.);
			TF1 f1("f1", "gaus", 0., 5.);

			int totIphi = 0;
			float meanAlphaforPhiRing = 0.;

			for ( int iphi = MIN_IPHI; iphi <= MAX_IPHI; iphi++ ) {


				int thisHashedIndex = GetHashedIndexEB(iabseta * theZside, iphi, theZside);
				if ( h_Occupancy_hashedIndex -> GetBinContent(thisHashedIndex + 1) == 0 ) continue;
				float thisAlphaConstant = h_Alpha_EB_hashedIndex -> GetBinContent (thisHashedIndex + 1);

				h_Alpha_EB -> Fill(iphi, iabseta * theZside, thisAlphaConstant); ///Fill with Last Alpha correction Value

				p_AlphaValues_iEta -> Fill(GetIetaFromHashedIndex(thisHashedIndex), thisAlphaConstant);

				histoAuxiliary . Fill (thisAlphaConstant);

				///Vectors
				IetaValues.push_back(iabseta * theZside);
				IphiValues.push_back(iphi);
				AlphaValues.push_back(thisAlphaConstant);

				meanAlphaforPhiRing += thisAlphaConstant;
				totIphi++;

			}

			for ( int uu = 0; uu < totIphi; uu++ ) meanAlphaforPhiRingValues.push_back(meanAlphaforPhiRing / totIphi);
			/// Note this info are not used furhter because channels near to the dead ones are not skipped


			for ( int iphi = MIN_IPHI; iphi <= MAX_IPHI; iphi++ ) {

				int thisHashedIndex = GetHashedIndexEB(iabseta * theZside, iphi, theZside);
				if ( h_Occupancy_hashedIndex -> GetBinContent(thisHashedIndex + 1) == 0 ) continue;

				h_Alpha_EB_meanOnPhi -> Fill(iphi, iabseta * theZside, AlphaValues.at(myPhiIndex) / meanAlphaforPhiRingValues.at(myPhiIndex));
				myPhiIndex++; /// Normalization IC with the mean of each ring

			}

			f1.SetParameters(histoAuxiliary.GetEntries(), histoAuxiliary.GetMean(), histoAuxiliary.GetRMS());
			f1.SetRange(histoAuxiliary.GetMean() - 8 * histoAuxiliary.GetRMS(), histoAuxiliary.GetMean() + 8 * histoAuxiliary.GetRMS());
			histoAuxiliary . Fit("f1", "QR");

			h_AlphaSpread_iEta -> SetBinContent( iabseta * theZside + 85 + 1, f1.GetParameter(2) );
			h_AlphaSpread_iEta -> SetBinError( iabseta * theZside + 85 + 1, f1.GetParError(2) );

		}

	}
}
/// Save infos in the output
void XtalAlphaEB::saveHistos(TFile * f1)
{

	f1->cd();

	h_occupancy             -> Write();
	h_Occupancy_hashedIndex -> Write();
	h_Intial_AlphaValues    -> Write();

	hC_AlphaValues          -> Write(*f1);
	hC_Alpha_EB             -> Write("", *f1);
	hC_EoP                  -> Write(*f1);

	g_AlphameanVsLoop       -> Write();
	g_AlpharmsVsLoop        -> Write();
	g_AlphaSigmaVsLoop      -> Write();

	hC_AlphaSpreadVsLoop    -> Write(*f1);

	h_Alpha_EB_hashedIndex -> Write();
	h_Alpha_EB             -> Write();
	h_Alpha_EB_meanOnPhi   -> Write();


	p_AlphaValues_iEta     -> Write();
	h_AlphaSpread_iEta     -> Write();


	h_map_Dead_Channels    -> Write() ;


	f1->Close();

	return;
}
/// Save E/0 distribution
void XtalAlphaEB::saveEoPeta(TFile * f2)
{

	f2->cd();

	hC_EoP_eta_ele ->Write(*f2);

	f2->Close();
}

///! Acquire fake dead channel list on order to evaluate the effected of IC near to them
void XtalAlphaEB::AcquireDeadXtal(TString inputDeadXtal)
{

	if(inputDeadXtal != "NULL") {
		std::ifstream DeadXtal (inputDeadXtal.Data(), std::ios::binary);

		std::string buffer;
		int iEta, iPhi ;


		while(!DeadXtal.eof()) {
			getline(DeadXtal, buffer);
			std::stringstream line( buffer );
			line >> iEta >> iPhi ;

			if(iEta >= 0) DeadXtal_HashedIndex.push_back(GetHashedIndexEB(iEta, iPhi, 1)) ;
			else DeadXtal_HashedIndex.push_back(GetHashedIndexEB(iEta, iPhi, -1)) ;


		}

		sort(DeadXtal_HashedIndex.begin(), DeadXtal_HashedIndex.end());
	} else {
		DeadXtal_HashedIndex.push_back(-9999);
	}

}
///! Check if the channel is dead or not
bool XtalAlphaEB::CheckDeadXtal(const int & iEta, const int & iPhi)
{
	int hashed_Index;
	if(iEta >= 0) hashed_Index = GetHashedIndexEB(iEta, iPhi, 1);
	else hashed_Index = GetHashedIndexEB(iEta, iPhi, -1);

	std::vector<int>::iterator iter = find(DeadXtal_HashedIndex.begin(), DeadXtal_HashedIndex.end(), hashed_Index);

	if(iter != DeadXtal_HashedIndex.end())
		return true;
	else return false;
}
