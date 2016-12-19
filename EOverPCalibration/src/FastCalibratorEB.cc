//adapting it to ECALELF ntuple

#include "../interface/FastCalibratorEB.h"
#include "../interface/GetHashedIndexEB.h"
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

FastCalibratorEB::FastCalibratorEB(TTree *tree, std::vector<TGraphErrors*> & inputMomentumScale, std::vector<TGraphErrors*> & inputEnergyScale, const std::string& typeEB, TString outEPDistribution):
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
	myEnergyScale = inputEnergyScale;
	myTypeEB = typeEB;
}

///==== deconstructor

FastCalibratorEB::~FastCalibratorEB()
{

	if (!fChain) return;
	delete fChain->GetCurrentFile();
}

///=== Acces to the entry information in the input tree or chain

Int_t FastCalibratorEB::GetEntry(Long64_t entry)
{

	if (!fChain) return 0;
	return fChain->GetEntry(entry);
}

///==== Load information of input Ntupla

Long64_t FastCalibratorEB::LoadTree(Long64_t entry)
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


//===== Fill Scalibration Map

void FastCalibratorEB::FillScalibMap(TString miscalibMap)
{

	std::ifstream scalibFile;
	scalibFile.open(miscalibMap.Data());

	if (!scalibFile) {
		std::cout << "miscalib map missing!!!" << std::endl;
		return;
	}

	float eta;
	float scalibValue;

	while (!scalibFile.eof()) {
		scalibFile >> eta >> scalibValue;
		scalibMap.insert(std::pair<float, float>(eta, scalibValue));
	}
	scalibFile.close();
}


///==== Variables initialization

void FastCalibratorEB::Init(TTree *tree)
{

	/// Set object pointer

	energyRecHitSCEle1 = 0;
	XRecHitSCEle1 = 0;  //ETA
	YRecHitSCEle1 = 0;  //PHI
	ZRecHitSCEle1 = 0;
	recoFlagRecHitSCEle1 = 0;

	energyRecHitSCEle2 = 0;
	XRecHitSCEle2 = 0;  //ETA
	YRecHitSCEle2 = 0;  //PHI
	ZRecHitSCEle2 = 0;
	recoFlagRecHitSCEle2 = 0;

	/// Set branch addresses and branch pointers

	if (!tree) return;
	fChain = tree;
	fCurrent = -1;
	fChain->SetMakeClass(1);

	fChain->SetBranchStatus("*", 0);

	fChain->SetBranchStatus("runNumber", 1);
	fChain->SetBranchAddress("runNumber", &runNumber, &b_runNumber);
	fChain->SetBranchStatus("lumiBlock", 1);
	fChain->SetBranchAddress("lumiBlock", &lumiBlock, &b_lumiBlock);
	fChain->SetBranchStatus("eventNumber", 1);
	fChain->SetBranchAddress("eventNumber", &eventNumber, &b_eventNumber);

	fChain->SetBranchStatus("chargeEle", 1);
	fChain->SetBranchAddress("chargeEle", chargeEle);
	fChain->SetBranchStatus("etaEle", 1);
	fChain->SetBranchAddress("etaEle", &etaEle, &b_etaEle);
	fChain->SetBranchStatus("PtEle", 1);
	fChain->SetBranchAddress("PtEle", &PtEle, &b_PtEle);
	fChain->SetBranchStatus("phiEle", 1);
	fChain->SetBranchAddress("phiEle", &phiEle, &b_phiEle);
	fChain->SetBranchStatus("rawEnergySCEle", 1);
	fChain->SetBranchAddress("rawEnergySCEle", &rawEnergySCEle, &b_rawEnergySCEle);
	fChain->SetBranchStatus("energySCEle_must", 1);
	fChain->SetBranchAddress("energySCEle_must", &energySCEle, &b_energySCEle);
	//  fChain->SetBranchStatus("energySCEle", 1);             fChain->SetBranchAddress("energySCEle", &energySCEle, &b_energySCEle);
	fChain->SetBranchStatus("etaSCEle", 1);
	fChain->SetBranchAddress("etaSCEle", &etaSCEle, &b_etaSCEle);
	fChain->SetBranchStatus("esEnergySCEle", 1);
	fChain->SetBranchAddress("esEnergySCEle", &esEnergySCEle, &b_esEnergySCEle);
	fChain->SetBranchStatus("pAtVtxGsfEle", 1);
	fChain->SetBranchAddress("pAtVtxGsfEle", &pAtVtxGsfEle, &b_pAtVtxGsfEle);
	fChain->SetBranchStatus("fbremEle", 1);
	fChain->SetBranchAddress("fbremEle", &fbremEle, &b_fbremEle);
	fChain->SetBranchStatus("energyMCEle", 1);
	fChain->SetBranchAddress("energyMCEle", &energyMCEle, &b_energyMCEle);
	fChain->SetBranchStatus("etaMCEle", 1);
	fChain->SetBranchAddress("etaMCEle", &etaMCEle, &b_etaMCEle);
	fChain->SetBranchStatus("phiMCEle", 1);
	fChain->SetBranchAddress("phiMCEle", &phiMCEle, &b_phiMCEle);

	// ele1
	fChain->SetBranchStatus("energyRecHitSCEle1", 1);
	fChain->SetBranchAddress("energyRecHitSCEle1", &energyRecHitSCEle1, &b_energyRecHitSCEle1);
	fChain->SetBranchStatus("XRecHitSCEle1", 1);
	fChain->SetBranchAddress("XRecHitSCEle1", &XRecHitSCEle1, &b_XRecHitSCEle1);
	fChain->SetBranchStatus("YRecHitSCEle1", 1);
	fChain->SetBranchAddress("YRecHitSCEle1", &YRecHitSCEle1, &b_YRecHitSCEle1);
	fChain->SetBranchStatus("ZRecHitSCEle1", 1);
	fChain->SetBranchAddress("ZRecHitSCEle1", &ZRecHitSCEle1, &b_ZRecHitSCEle1);
	fChain->SetBranchStatus("recoFlagRecHitSCEle1", 1);
	fChain->SetBranchAddress("recoFlagRecHitSCEle1", &recoFlagRecHitSCEle1, &b_recoFlagRecHitSCEle1);

	// ele2
	fChain->SetBranchStatus("energyRecHitSCEle2", 1);
	fChain->SetBranchAddress("energyRecHitSCEle2", &energyRecHitSCEle2, &b_energyRecHitSCEle2);
	fChain->SetBranchStatus("XRecHitSCEle2", 1);
	fChain->SetBranchAddress("XRecHitSCEle2", &XRecHitSCEle2, &b_XRecHitSCEle2);
	fChain->SetBranchStatus("YRecHitSCEle2", 1);
	fChain->SetBranchAddress("YRecHitSCEle2", &YRecHitSCEle2, &b_YRecHitSCEle2);
	fChain->SetBranchStatus("ZRecHitSCEle2", 1);
	fChain->SetBranchAddress("ZRecHitSCEle2", &ZRecHitSCEle2, &b_ZRecHitSCEle2);
	fChain->SetBranchStatus("recoFlagRecHitSCEle2", 1);
	fChain->SetBranchAddress("recoFlagRecHitSCEle2", &recoFlagRecHitSCEle2, &b_recoFlagRecHitSCEle2);

}

//! Declaration of the objects that are save in the output file

void FastCalibratorEB::bookHistos(int nLoops)
{

	hC_IntercalibValues = new hChain ("IntercalibValues", "IntercalibValues", 2000, 0.5, 1.5, nLoops);

	hC_PullFromScalib   = new hChain ("hC_PullFromScalib", "hC_PullFromScalib", 2000, -0.5, 0.5, nLoops);

	hC_EoP              = new hChain ("EoP", "EoP", 100, 0.2, 1.9, nLoops);

	hC_scale_EB         = new h2Chain("hC_scale_EB", "hC_scale_EB", 360, 1, 361, 171, -85, 86, nLoops );

	h_scalib_EB         = new TH2F("h_scalib_EB", "h_scalib_EB", 360, 1, 361, 171, -85, 86 );

	h_Occupancy_hashedIndex = new TH1F ("h_Occupancy_hashedIndex", "h_Occupancy_hashedIndex", 61201, -0.5, 61199.5);

	p_IntercalibValues_iEta = new TProfile ("p_IntercalibValues_iEta", "p_IntercalibValues_iEta", 171, -85, 86, -0.1, 2.1);

	h_IntercalibSpread_iEta = new TH1F ("h_IntercalibSpread_iEta", "h_IntercalibSpread_iEta", 171, -85, 86);

	h_IntercalibValues_test = new TH1F ("h_IntercalibValues_test", "h_IntercalibValues_test", 400, -1, 1);

	h_scale_EB_hashedIndex  = new TH1F("h_scale_EB_hashedIndex", "h_scale_EB_hashedIndex", 61201, -0.5, 61999.5 );

	h_Init_IntercalibValues = new TH1F("h_Init_IntercalibValues", "h_Init_IntercalibValues", 2000, 0.5, 1.5);

	h_map_Dead_Channels     = new TH2F("h_map_Dead_Channels", "h_map_Dead_Channels", 360, 1, 361, 171, -85, 86);

	g_ICmeanVsLoop = new TGraphErrors();
	g_ICmeanVsLoop -> SetName("g_ICmeanVsLoop");
	g_ICmeanVsLoop -> SetTitle("g_ICmeanVsLoop");

	g_ICrmsVsLoop  = new TGraphErrors();
	g_ICrmsVsLoop -> SetName("g_ICrmsVsLoop");
	g_ICrmsVsLoop ->SetTitle("g_ICrmsVsLoop");

	h_scale_EB           = new TH2F("h_scale_EB", "h_scale_EB", 360, 1, 361, 171, -85, 86 );

	h_scale_EB_meanOnPhi = new TH2F("h_scale_EB_meanOnPhi", "h_scale_EB_meanOnPhi", 360, 1, 361, 171, -85, 86 );

	h_occupancy          = new TH2F("h_occupancy", "h_occupancy", 360, 1, 361, 171, -85, 86 );

	return;
}

//! Build E/p distribution for both ele1 and ele2

void FastCalibratorEB::BuildEoPeta_ele(int iLoop, int nentries , int useW, int useZ, bool applyMomentumCorrection, bool applyEnergyCorrection, int useRawEnergy, std::vector<float> theScalibration, bool isSaveEPDistribution, bool isR9selection,
                                       float R9Min, bool isfbrem, float fbremMax, bool isPtCut, float PtMin, bool isMCTruth)
{

	if(iLoop == 0) {
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
		//   if (runNumber>276243) continue; //LUCATEMP

		///! Tight electron from W or Z only barrel (if chargeEle[1]==-100: event from W)

		if ( fabs(etaSCEle[0]) < 1.479 && (( useW == 1 && chargeEle[1] == -100 ) || ( useZ == 1 && chargeEle[1] != -100 ))) {

			FdiEta = energySCEle[0] / rawEnergySCEle[0]; /// FEta approximation
			if (useRawEnergy == 1)
				FdiEta = 1.;
			if (applyEnergyCorrection)
				FdiEta /= myEnergyScale[0] -> Eval( phiEle[0] );

			float thisE = 0;
			int   iseed = 0 ;
			int   seed_hashedIndex = 0;
			float E_seed = 0;
			float thisE3x3 = 0;

			/// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy

			for (unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle1->size(); iRecHit++ ) {

				float thisIC = 1.;
				int thisIndex = GetHashedIndexEB(XRecHitSCEle1->at(iRecHit), YRecHitSCEle1->at(iRecHit), ZRecHitSCEle1->at(iRecHit));
				if (thisIndex < 0) continue;

				if(energyRecHitSCEle1 -> at(iRecHit) > E_seed &&  recoFlagRecHitSCEle1->at(iRecHit) < 4 ) { /// control if this recHit is good
					seed_hashedIndex = GetHashedIndexEB(XRecHitSCEle1->at(iRecHit), YRecHitSCEle1->at(iRecHit), ZRecHitSCEle1->at(iRecHit));
					iseed = iRecHit;
					E_seed = energyRecHitSCEle1 -> at(iRecHit); ///! Seed search
				}

				if (iLoop > 0) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex + 1);

				if(recoFlagRecHitSCEle1->at(iRecHit) < 4) ///! SC energy taking only good channels
					thisE += theScalibration[thisIndex] * energyRecHitSCEle1 -> at(iRecHit) * FdiEta * thisIC;

			}

			for (unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle1->size(); iRecHit++ ) {

				float thisIC = 1.;
				int thisIndex = GetHashedIndexEB(XRecHitSCEle1->at(iRecHit), YRecHitSCEle1->at(iRecHit), ZRecHitSCEle1->at(iRecHit));
				if (thisIndex < 0) continue;

				///! 3x3 matrix informations in order to apply R9 selection

				if (iLoop > 0) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex + 1);

				if(fabs(XRecHitSCEle1->at(iRecHit) - XRecHitSCEle1->at(iseed)) <= 1 &&
				        fabs(YRecHitSCEle1->at(iRecHit) - YRecHitSCEle1->at(iseed)) <= 1 && recoFlagRecHitSCEle1->at(iRecHit) < 4)
					thisE3x3 += theScalibration[thisIndex] * energyRecHitSCEle1 -> at(iRecHit) * FdiEta * thisIC;
			}

			///! Eta seed from hashed index
			int eta_seed = GetIetaFromHashedIndex(seed_hashedIndex);

			bool skipElectron = false;

			///! different E/p if I am using MCThruth informations or not
			if(!isMCTruth)  {
				pIn = pAtVtxGsfEle[0];
				//       int regionId = templIndexEB(myTypeEB,etaEle[0],chargeEle[0],thisE3x3/thisE);
				if (applyMomentumCorrection)
					pIn /= myMomentumScale[0] -> Eval( phiEle[0] );
			} else {
				pIn = energyMCEle[0];
				ele1_DR     = TMath::Sqrt((etaMCEle[0] - etaEle[0]) * (etaMCEle[0] - etaEle[0]) + (phiMCEle[0] - phiEle[0]) * (phiMCEle[0] - phiEle[0])) ;
				if(fabs(ele1_DR) > 0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
			}

			/// R9 Selection
			if( fabs(thisE3x3 / thisE) < R9Min && isR9selection == true ) skipElectron = true;

			/// fbrem Selection
			if( fabs(fbremEle[0]) > fbremMax  && isfbrem == true ) skipElectron = true;

			/// fbrem Selection
			if( PtEle[0] < PtMin  && isPtCut == true ) skipElectron = true;

			/// Save electron E/p in a chain of histogramm each for eta bin
			if(!skipElectron)    hC_EoP_eta_ele -> Fill(eta_seed + 85, thisE / pIn);


		}
		///=== Second medium electron from Z

		if ( fabs(etaSCEle[1]) < 1.479 && ( useZ == 1 && chargeEle[1] != -100 ) ) {

			FdiEta = energySCEle[1] / rawEnergySCEle[1]; /// FEta approximation
			if (useRawEnergy == 1)
				FdiEta = 1.;
			if (applyEnergyCorrection)
				FdiEta /= myEnergyScale[0] -> Eval( phiEle[1] );

			float thisE = 0;
			int   iseed = 0 ;
			int seed_hashedIndex = 0;
			float E_seed = 0;
			float thisE3x3 = 0;

			/// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy

			for (unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle2->size(); iRecHit++ ) {

				float thisIC = 1.;
				int thisIndex = GetHashedIndexEB(XRecHitSCEle2->at(iRecHit), YRecHitSCEle2->at(iRecHit), ZRecHitSCEle2->at(iRecHit));
				if (thisIndex < 0) continue;

				if(energyRecHitSCEle2 -> at(iRecHit) > E_seed && recoFlagRecHitSCEle2->at(iRecHit) < 4) {
					seed_hashedIndex = GetHashedIndexEB(XRecHitSCEle2->at(iRecHit), YRecHitSCEle2->at(iRecHit), ZRecHitSCEle2->at(iRecHit));
					iseed = iRecHit;
					E_seed = energyRecHitSCEle2 -> at(iRecHit); ///Seed informations
				}


				if (iLoop > 0) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex + 1);

				if (recoFlagRecHitSCEle2->at(iRecHit) < 4 ) /// SC Energy only for good channels
					thisE += theScalibration[thisIndex] * energyRecHitSCEle2 -> at(iRecHit) * FdiEta * thisIC;

			}

			for (unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle2->size(); iRecHit++ ) {

				float thisIC = 1.;
				int thisIndex = GetHashedIndexEB(XRecHitSCEle2->at(iRecHit), YRecHitSCEle2->at(iRecHit), ZRecHitSCEle2->at(iRecHit));
				if (thisIndex < 0) continue;

				if (iLoop > 0) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex + 1);

				if(fabs(XRecHitSCEle2->at(iRecHit) - XRecHitSCEle2->at(iseed)) <= 1 &&
				        fabs(YRecHitSCEle2->at(iRecHit) - YRecHitSCEle2->at(iseed)) <= 1 &&
				        recoFlagRecHitSCEle2->at(iRecHit) < 4)
					thisE3x3 += theScalibration[thisIndex] * energyRecHitSCEle2 -> at(iRecHit) * FdiEta * thisIC;
			}


			/// Eta seed info from hashed index
			int eta_seed = GetIetaFromHashedIndex(seed_hashedIndex);

			bool skipElectron = false;

			/// MCTruth analysis option
			if(!isMCTruth)  {
				pIn = pAtVtxGsfEle[1];
				//	 int regionId = templIndexEB(myTypeEB,etaEle[1],ele2_charge,thisE3x3/thisE);
				if (applyMomentumCorrection)
					pIn /= myMomentumScale[0] -> Eval( phiEle[0] );
				//         pIn /= myMomentumScale[regionId] -> Eval( phiEle[1] );
			} else {
				pIn = energyMCEle[1];
				ele2_DR     = TMath::Sqrt((etaMCEle[1] - etaEle[1]) * (etaMCEle[1] - etaEle[1]) + (phiMCEle[1] - phiEle[1]) * (phiMCEle[1] - phiEle[1])) ;
				if(fabs(ele2_DR) > 0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
			}

			///R9 Selection
			if( fabs(thisE3x3 / thisE) < R9Min && isR9selection == true ) skipElectron = true;

			/// fbrem Selection
			if( fabs(fbremEle[1]) > fbremMax  && isfbrem == true ) skipElectron = true;

			/// fbrem Selection
			if( PtEle[1] < PtMin  && isPtCut == true ) skipElectron = true;

			/// Save E/p electron information
			if(!skipElectron) hC_EoP_eta_ele -> Fill(eta_seed + 85, thisE / pIn);

		}
	}

/// Histogramm Normalization
	for(unsigned int ieta = 0 ; ieta < hC_EoP_eta_ele->Size() ; ieta++) hC_EoP_eta_ele->Normalize(ieta);

/// Save E/p pdf if it is required
	if(isSaveEPDistribution == true) {
		TFile *f2 = new TFile(outEPDistribution_p.Data(), "UPDATE");
		saveEoPeta(f2);
	}

}


/// Calibration Loop over the ntu events
void FastCalibratorEB::Loop( int nentries, int useZ, int useW, int splitStat, int nLoops, bool applyMomentumCorrection, bool applyEnergyCorrection, int useRawEnergy, bool isMiscalib, bool isSaveEPDistribution,
                             bool isEPselection, bool isR9selection, float R9Min, float EPMin, int smoothCut, bool isfbrem, float fbremMax, bool isPtCut, float PtMin,
                             bool isMCTruth, float miscalibMethod, TString miscalibMap)
{
	if (fChain == 0) return;

	if(isMiscalib == true) {
		std::cout << "method used for the scalibration (1=from map, 0=linear): " << miscalibMethod << std::endl;
		if (miscalibMethod == 1) {  //miscalibration with a gaussian spread (eta-dependent)
			FillScalibMap(miscalibMap);  //fill the map with the scalib values
			std::cout << "Using miscalibration from map" << std::endl;
		}
	}

	/// Define the number of crystal you want to calibrate
	int m_regions = 0;

	//   float EPMin=0.15;

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
			if(isMiscalib == true) {
				if (miscalibMethod == 1) {  //miscalibration with a gaussian spread (eta-dependent)
					theScalibration[iIndex] = scalibMap.at(GetIetaFromHashedIndex(iIndex));   //take the values from the map filled before
				} else
					theScalibration[iIndex] = 1 + 0.1 * fabs(GetIetaFromHashedIndex(iIndex)) / 85.; //linear eta-dependent scalibration

			}

//       theScalibration[iIndex] = 1.+ 0.01*fabs(GetIetaFromHashedIndex(iIndex));//  genRand.Gaus(1.,0.01); ///! 5% of Miscalibration fixed
			if(isMiscalib == false) theScalibration[iIndex] = 1.;
			h_scalib_EB -> Fill ( GetIphiFromHashedIndex(iIndex), GetIetaFromHashedIndex(iIndex), theScalibration[iIndex] ); ///! Scalib map
		}
	}

	/// ----------------- Calibration Loops -----------------------------//

	float EPCutValue = 100.;

	for ( int iLoop = 0; iLoop < nLoops; iLoop++ ) {

		std::cout << "Starting iteration " << iLoop + 1 << std::endl;

		/// prepare the numerator and denominator for each Xtal

		std::vector<float> theNumerator(m_regions, 0.);
		std::vector<float> theDenominator(m_regions, 0.);

		if (iLoop == 0)  EPCutValue = 100.;
		//    else if (iLoop==1)  EPCutValue = 0.50;
		// else if (iLoop==7)  EPCutValue = 0.10;
		//else if (iLoop==13) EPCutValue = 0.05;
		//else if (iLoop==1) EPCutValue = EPMin;
		//else           EPCutValue = EPCutValue*0.82;
		else EPCutValue = EPMin;

		std::cout << "Number of analyzed events = " << nentries << std::endl;

		///==== build E/p distribution ele 1 and 2
		BuildEoPeta_ele(iLoop, nentries, useW, useZ, applyMomentumCorrection, applyEnergyCorrection, useRawEnergy, theScalibration, isSaveEPDistribution, isR9selection, R9Min, isfbrem, fbremMax, isPtCut, PtMin, isMCTruth);

		// define map with events
		std::map<std::pair<int, std::pair<int, int> >, int> eventsMap;

		/// Loop on each entry
		Long64_t nbytes = 0, nb = 0;
		for (Long64_t jentry = 0; jentry < nentries; jentry++) {

			if (!(jentry % 10000))std::cerr << jentry;
			if (!(jentry % 1000)) std::cerr << ".";

			Long64_t ientry = LoadTree(jentry);
			if (ientry < 0) break;
			nb = fChain->GetEntry(jentry);
			nbytes += nb;

			//*********************************
			// JSON FILE AND DUPLIACTES IN DATA
			//	if (runNumber>276243) continue; //LUCATEMP

			bool skipEvent = false;
			/*	if( isMCTruth == 0 )
			    {
			  //	  if(AcceptEventByRunAndLumiSection(runNumber,lumiBlock,jsonMap) == false) skipEvent = true;

			      std::pair<int,Long64_t> eventLSandID(lumiBlock,eventNumber);
			      std::pair<int,std::pair<int,Long64_t> > eventRUNandLSandID(runNumber,eventLSandID);
			      if( eventsMap[eventRUNandLSandID] == 1 ) skipEvent = true;
			      else eventsMap[eventRUNandLSandID] = 1;
			  }*/

			if( skipEvent == true ) continue;

			float pIn, FdiEta;

			std::map<int, double> map;
			bool skipElectron = false;

			/// Tight electron information from W and Z

			if ( fabs(etaSCEle[0]) < 1.479 && (( useW == 1 && chargeEle[1] == -100 ) || ( useZ == 1 && chargeEle[1] != -100 )) ) {

				/// SCL energy containment correction
				FdiEta = energySCEle[0] / rawEnergySCEle[0];
				if (useRawEnergy == 1)
					FdiEta = 1.;
				if (applyEnergyCorrection)
					FdiEta /= myEnergyScale[0] -> Eval( phiEle[0] );

				float thisE = 0;
				int iseed = 0 ;
				float E_seed = 0;
				int seed_hashedIndex = 0;
				float thisE3x3 = 0 ;

				bool skipElectron = false;

				/// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
				if (energyRecHitSCEle1->size() < 1) continue;
				for (unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle1->size(); iRecHit++ ) {

					float thisIC = 1.;
					int thisIndex = GetHashedIndexEB(XRecHitSCEle1->at(iRecHit), YRecHitSCEle1->at(iRecHit), ZRecHitSCEle1->at(iRecHit));
					if (thisIndex < 0) continue;

					if (iLoop > 0 ) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex + 1);

					if (recoFlagRecHitSCEle1->at(iRecHit) < 4) ///! SC Energy
						thisE += theScalibration[thisIndex] * energyRecHitSCEle1 -> at(iRecHit) * FdiEta * thisIC;

					if(theScalibration[thisIndex] == 0  && energyRecHitSCEle1 -> at(iRecHit) / energySCEle[0] >= 0.15 ) ///! not to introduce a bias in the Dead xtal study
						skipElectron = true;

					if(energyRecHitSCEle1 -> at(iRecHit) > E_seed && recoFlagRecHitSCEle1->at(iRecHit) < 4) {
						E_seed = energyRecHitSCEle1 -> at(iRecHit);
						iseed = iRecHit;
						seed_hashedIndex = GetHashedIndexEB(XRecHitSCEle1->at(iRecHit), YRecHitSCEle1->at(iRecHit), ZRecHitSCEle1->at(iRecHit)); //! Seed Infos
					}

				}

				for (unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle1->size(); iRecHit++ ) {

					float thisIC = 1.;
					int thisIndex = GetHashedIndexEB(XRecHitSCEle1->at(iRecHit), YRecHitSCEle1->at(iRecHit), ZRecHitSCEle1->at(iRecHit));
					if (thisIndex < 0) continue;

					if (iLoop > 0) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex + 1);

					if(fabs(XRecHitSCEle1->at(iRecHit) - XRecHitSCEle1->at(iseed)) <= 1 &&
					        fabs(YRecHitSCEle1->at(iRecHit) - YRecHitSCEle1->at(iseed)) <= 1 &&
					        recoFlagRecHitSCEle1->at(iRecHit) < 4 )
						thisE3x3 += theScalibration[thisIndex] * energyRecHitSCEle1 -> at(iRecHit) * FdiEta * thisIC;
				}


				///! if MCTruth Analysis
				if(!isMCTruth)  {
					pIn = pAtVtxGsfEle[0];
					//	    int regionId = templIndexEB(myTypeEB,etaEle[0],chargeEle[0],thisE3x3/thisE);
					if (applyMomentumCorrection)
						pIn /= myMomentumScale[0] -> Eval( phiEle[0] );
				} else {
					pIn = energyMCEle[0];
					ele1_DR     = TMath::Sqrt((etaMCEle[0] - etaEle[0]) * (etaMCEle[0] - etaEle[0]) + (phiMCEle[0] - phiEle[0]) * (phiMCEle[0] - phiEle[0]));
					if(fabs(ele1_DR) > 0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
				}

				/// Take the correct pdf for the ring in order to reweight the events in L3
				int eta_seed = GetIetaFromHashedIndex(seed_hashedIndex);
				TH1F* EoPHisto = hC_EoP_eta_ele->GetHisto(eta_seed + 85);


				/// Basic selection on E/p or R9 if you want to apply
				if( fabs(thisE / pIn  - 1) > 0.3 && isEPselection == true ) skipElectron = true;
				if( fabs(thisE3x3 / thisE) < R9Min && isR9selection == true ) skipElectron = true;
				if( fabs(fbremEle[0]) > fbremMax  && isfbrem == true ) skipElectron = true;
				if( PtEle[0] < PtMin  && isPtCut == true ) skipElectron = true;

				if( thisE / pIn < EoPHisto->GetXaxis()->GetXmin() || thisE / pIn > EoPHisto->GetXaxis()->GetXmax()) skipElectron = true;
				if( !skipElectron) {

					/// Now cycle on the all the recHits and update the numerator and denominator
					for ( unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle1->size(); iRecHit++ ) {

						if (recoFlagRecHitSCEle1->at(iRecHit) >= 4) continue ;

						int thisIndex = GetHashedIndexEB(XRecHitSCEle1->at(iRecHit), YRecHitSCEle1->at(iRecHit), ZRecHitSCEle1->at(iRecHit));
						float thisIC = 1.;
						if (thisIndex < 0) continue;

						if (iLoop > 0) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex + 1);

						/// Fill the occupancy map JUST for the first Loop
						if ( iLoop == 0 ) {
							h_Occupancy_hashedIndex -> Fill(thisIndex);
							h_occupancy -> Fill(GetIphiFromHashedIndex(thisIndex), GetIetaFromHashedIndex(thisIndex));
						}

						/// use full statistics
						if ( splitStat == 0 ) {
							float EoPweight = EoPHisto->GetBinContent(EoPHisto->FindBin(thisE / pIn));
							if (fabs(thisE / pIn - 1) < EPCutValue && smoothCut == 1) EoPweight = EoPHisto->GetBinContent(EoPHisto->FindBin(thisE / pIn));
							else if (fabs(thisE / pIn - 1) < EPCutValue) EoPweight = EoPHisto->GetBinContent(EoPHisto->FindBin(1));
							else EoPweight = 0.00000001;
							theNumerator[thisIndex] += theScalibration[thisIndex] * energyRecHitSCEle1 -> at(iRecHit) * FdiEta * thisIC / thisE * pIn / thisE * EoPweight;
							theDenominator[thisIndex] += theScalibration[thisIndex] * energyRecHitSCEle1 -> at(iRecHit) * FdiEta * thisIC / thisE * EoPweight;

						}
						/// Use Half Statistic only even
						else if ( splitStat == 1 && eventNumber % 2 == 0 ) {
							float EoPweight = EoPHisto->GetBinContent(EoPHisto->FindBin(thisE / pIn));
							if (fabs(thisE / pIn - 1) < EPCutValue && smoothCut == 1) EoPweight = EoPHisto->GetBinContent(EoPHisto->FindBin(thisE / pIn));
							else if (fabs(thisE / pIn - 1) < EPCutValue) EoPweight = EoPHisto->GetBinContent(EoPHisto->FindBin(1));
							else EoPweight = 0.00000001;
							theNumerator[thisIndex] += theScalibration[thisIndex] * energyRecHitSCEle1 -> at(iRecHit) * FdiEta * thisIC / thisE * pIn / thisE * EoPweight;
							theDenominator[thisIndex] += theScalibration[thisIndex] * energyRecHitSCEle1 -> at(iRecHit) * FdiEta * thisIC / thisE * EoPweight;
						}
						/// use odd event
						else if ( splitStat == -1 && eventNumber % 2 != 0 ) {
							float EoPweight = EoPHisto->GetBinContent(EoPHisto->FindBin(thisE / pIn));
							if (fabs(thisE / pIn - 1) < EPCutValue && smoothCut == 1) EoPweight = EoPHisto->GetBinContent(EoPHisto->FindBin(thisE / pIn));
							else if (fabs(thisE / pIn - 1) < EPCutValue) EoPweight = EoPHisto->GetBinContent(EoPHisto->FindBin(1));
							else EoPweight = 0.00000001;
							theNumerator[thisIndex] += theScalibration[thisIndex] * energyRecHitSCEle1 -> at(iRecHit) * FdiEta * thisIC / thisE * pIn / thisE * EoPweight;
							theDenominator[thisIndex] += theScalibration[thisIndex] * energyRecHitSCEle1 -> at(iRecHit) * FdiEta * thisIC / thisE * EoPweight;
						}

					}

				}


				//Fill EoP
				hC_EoP -> Fill(iLoop, thisE / pIn);

			}

			skipElectron = false;

			/// Ele2 medium from Z only Barrel
			if ( fabs(etaSCEle[1]) < 1.479 && ( useZ == 1 && chargeEle[1] != -100 ) ) {

				FdiEta = energySCEle[1] / rawEnergySCEle[1];
				if (useRawEnergy == 1)
					FdiEta = 1.;
				if (applyEnergyCorrection)
					FdiEta /= myEnergyScale[0] -> Eval( phiEle[1] );

				// Electron energy
				float thisE = 0;
				int iseed = 0 ;
				float E_seed = 0;
				int seed_hashedIndex = 0;
				float thisE3x3 = 0;

				/// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
				if (energyRecHitSCEle2->size() < 1) continue;
				for (unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle2->size(); iRecHit++ ) {

					float thisIC = 1.;
					int thisIndex = GetHashedIndexEB(XRecHitSCEle2->at(iRecHit), YRecHitSCEle2->at(iRecHit), ZRecHitSCEle2->at(iRecHit));
					if (thisIndex < 0) continue;

					if (iLoop > 0) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex + 1);

					if(recoFlagRecHitSCEle2->at(iRecHit) < 4)
						thisE += theScalibration[thisIndex] * energyRecHitSCEle2 -> at(iRecHit) * FdiEta * thisIC;

					if(theScalibration[thisIndex] == 0  && energyRecHitSCEle2 -> at(iRecHit) / energySCEle[1] >= 0.15 ) ///! not to introduce a bias in the Dead xtal study
						skipElectron = true;

					if(energyRecHitSCEle2 -> at(iRecHit) > E_seed && recoFlagRecHitSCEle2->at(iRecHit) < 4) {
						E_seed = energyRecHitSCEle2 -> at(iRecHit);
						iseed = iRecHit;
						seed_hashedIndex = GetHashedIndexEB(XRecHitSCEle2->at(iRecHit), YRecHitSCEle2->at(iRecHit), ZRecHitSCEle2->at(iRecHit)); /// Seed information
					}


				}

				for (unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle2->size(); iRecHit++ ) {

					float thisIC = 1.;
					int thisIndex = GetHashedIndexEB(XRecHitSCEle2->at(iRecHit), YRecHitSCEle2->at(iRecHit), ZRecHitSCEle2->at(iRecHit));
					if (thisIndex < 0) continue;

					// IC obtained from previous Loops
					if (iLoop > 0) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex + 1);


					if(fabs(XRecHitSCEle2->at(iRecHit) - XRecHitSCEle2->at(iseed)) <= 1 &&
					        fabs(YRecHitSCEle2->at(iRecHit) - YRecHitSCEle2->at(iseed)) <= 1 &&
					        recoFlagRecHitSCEle2->at(iRecHit) < 4)
						thisE3x3 += theScalibration[thisIndex] * energyRecHitSCEle2 -> at(iRecHit) * FdiEta * thisIC;


				}

				///! Option for MCTruth analysis
				if(!isMCTruth) {
					pIn = pAtVtxGsfEle[1];
					//	    int regionId = templIndexEB(myTypeEB,etaEle[1],ele2_charge,thisE3x3/thisE);
					if (applyMomentumCorrection)
						pIn /= myMomentumScale[0] -> Eval( phiEle[1] );
				} else {
					pIn = energyMCEle[1];
					ele2_DR     = TMath::Sqrt((etaMCEle[1] - etaEle[1]) * (etaMCEle[1] - etaEle[1]) + (phiMCEle[1] - phiEle[1]) * (phiMCEle[1] - phiEle[1]));
					if(fabs(ele2_DR) > 0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
				}
				int eta_seed = GetIetaFromHashedIndex(seed_hashedIndex);
				TH1F* EoPHisto = hC_EoP_eta_ele->GetHisto(eta_seed + 85);

				/// discard electrons with bad E/P or R9
				if( thisE / pIn  < EoPHisto->GetXaxis()->GetXmin() || thisE / pIn  > EoPHisto->GetXaxis()->GetXmax()) skipElectron = true;
				if( fabs(thisE / pIn  - 1) > 0.3 && isEPselection == true ) skipElectron = true;
				if( fabs(thisE3x3 / thisE) < R9Min && isR9selection == true ) skipElectron = true;
				if( fabs(fbremEle[1]) > fbremMax  && isfbrem == true ) skipElectron = true;
				if( PtEle[1] < PtMin  && isPtCut == true ) skipElectron = true;

				if( !skipElectron ) {

					/// Now cycle on the all the recHits and update the numerator and denominator
					for ( unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle2->size(); iRecHit++ ) {

						if (recoFlagRecHitSCEle2->at(iRecHit) >= 4) continue ;

						int thisIndex = GetHashedIndexEB(XRecHitSCEle2->at(iRecHit), YRecHitSCEle2->at(iRecHit), ZRecHitSCEle2->at(iRecHit));
						if (thisIndex < 0) continue;
						//  std::cout<<"DEBUG: "<<thisIndex<<" "<<XRecHitSCEle2->at(iRecHit)<<" "<<YRecHitSCEle2->at(iRecHit)<<" "<<ZRecHitSCEle2->at(iRecHit)<<std::endl;
						float thisIC = 1.;

						if (iLoop > 0) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex + 1);

						/// Fill the occupancy map JUST for the first Loop
						if ( iLoop == 0 ) {
							h_Occupancy_hashedIndex -> Fill(thisIndex);
							h_occupancy -> Fill(GetIphiFromHashedIndex(thisIndex), GetIetaFromHashedIndex(thisIndex));
						}

						/// use full statistics
						if ( splitStat == 0 ) {

							float EoPweight = EoPHisto->GetBinContent(EoPHisto->FindBin(thisE / pIn));
							if (fabs(thisE / pIn - 1) < EPCutValue && smoothCut == 1) EoPweight = EoPHisto->GetBinContent(EoPHisto->FindBin(thisE / pIn));
							else if (fabs(thisE / pIn - 1) < EPCutValue) EoPweight = EoPHisto->GetBinContent(EoPHisto->FindBin(1));
							else EoPweight = 0.00000001;
							theNumerator[thisIndex] += theScalibration[thisIndex] * energyRecHitSCEle2 -> at(iRecHit) * FdiEta * thisIC / thisE * pIn / thisE * EoPweight;
							theDenominator[thisIndex] += theScalibration[thisIndex] * energyRecHitSCEle2 -> at(iRecHit) * FdiEta * thisIC / thisE * EoPweight;
						}
						/// use evens
						else if ( splitStat == 1 && eventNumber % 2 == 0 ) {
							float EoPweight = EoPHisto->GetBinContent(EoPHisto->FindBin(thisE / pIn));
							if (fabs(thisE / pIn - 1) < EPCutValue && smoothCut == 1) EoPweight = EoPHisto->GetBinContent(EoPHisto->FindBin(thisE / pIn));
							else if (fabs(thisE / pIn - 1) < EPCutValue) EoPweight = EoPHisto->GetBinContent(EoPHisto->FindBin(1));
							else EoPweight = 0.00000001;
							theNumerator[thisIndex] += theScalibration[thisIndex] * energyRecHitSCEle2 -> at(iRecHit) * FdiEta * thisIC / thisE * pIn / thisE * EoPweight;
							theDenominator[thisIndex] += theScalibration[thisIndex] * energyRecHitSCEle2 -> at(iRecHit) * FdiEta * thisIC / thisE * EoPweight;
						}
						/// use odds
						else if ( splitStat == -1 && eventNumber % 2 != 0 ) {
							float EoPweight = EoPHisto->GetBinContent(EoPHisto->FindBin(thisE / pIn));
							if (fabs(thisE / pIn - 1) < EPCutValue && smoothCut == 1) EoPweight = EoPHisto->GetBinContent(EoPHisto->FindBin(thisE / pIn));
							else if (fabs(thisE / pIn - 1) < EPCutValue) EoPweight = EoPHisto->GetBinContent(EoPHisto->FindBin(1));
							else EoPweight = 0.00000001;
							theNumerator[thisIndex] += theScalibration[thisIndex] * energyRecHitSCEle2 -> at(iRecHit) * FdiEta * (thisIC / thisE) * pIn / thisE * EoPweight;
							theDenominator[thisIndex] += theScalibration[thisIndex] * energyRecHitSCEle2 -> at(iRecHit) * FdiEta * thisIC / thisE * EoPweight;
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

		TH1F auxiliary_IC("auxiliary_IC", "auxiliary_IC", 50, 0.2, 1.9);

		///Fill the histo of IntercalibValues before the solve
		for ( int iIndex = 0; iIndex < 61200; iIndex++ ) {

			if ( h_Occupancy_hashedIndex -> GetBinContent(iIndex + 1) > 0 ) {

				float thisIntercalibConstant = 1.;
				/// Solve the cases where the recHit energy is always 0 (dead Xtal?)
				bool isDeadXtal = false ;
				if(DeadXtal_HashedIndex.at(0) != -9999) isDeadXtal = CheckDeadXtal(GetIetaFromHashedIndex(iIndex), GetIphiFromHashedIndex(iIndex));
				if(isDeadXtal == true ) continue;


				if ( theDenominator[iIndex] != 0. ) thisIntercalibConstant = theNumerator[iIndex] / theDenominator[iIndex];
				float oldIntercalibConstant = 1.;
				if ( iLoop > 0 ) oldIntercalibConstant = h_scale_EB_hashedIndex -> GetBinContent (iIndex + 1);

				h_scale_EB_hashedIndex -> SetBinContent(iIndex + 1, thisIntercalibConstant * oldIntercalibConstant); /// IC product useful for L3 methods
				hC_IntercalibValues -> Fill(iLoop, thisIntercalibConstant); /// IC distribution at each loop
				hC_PullFromScalib -> Fill(iLoop, (thisIntercalibConstant * oldIntercalibConstant - 1. / theScalibration[iIndex]));
				hC_scale_EB -> Fill(iLoop, GetIphiFromHashedIndex(iIndex), GetIetaFromHashedIndex(iIndex), thisIntercalibConstant * oldIntercalibConstant); ///IC Map

				///Save the new IC coefficient
				auxiliary_IC.Fill(thisIntercalibConstant);

			}

		}
		/// Info in order to test convergence
		g_ICmeanVsLoop -> SetPoint(iLoop, iLoop, auxiliary_IC . GetMean());
		g_ICmeanVsLoop -> SetPointError(iLoop, 0., auxiliary_IC . GetMeanError());

		g_ICrmsVsLoop -> SetPoint(iLoop, iLoop, auxiliary_IC . GetRMS());
		g_ICrmsVsLoop -> SetPointError(iLoop, 0., auxiliary_IC . GetRMSError());
	}/// end calibration loop


	int myPhiIndex = 0;

	for ( int iabseta = MIN_IETA; iabseta < MAX_IETA + 1; iabseta++ ) {
		for ( int theZside = -1; theZside < 2; theZside = theZside + 2 ) {

			///Setup the histo for fit
			TH1F histoAuxiliary("histoAuxiliary", "histoAuxiliary", 400, 0.2, 1.9);
			TF1 f1("f1", "gaus", 0.2, 1.9);

			int totIphi = 0;
			float meanICforPhiRing = 0.;

			for ( int iphi = MIN_IPHI; iphi <= MAX_IPHI; iphi++ ) {

				int thisHashedIndex = GetHashedIndexEB(iabseta * theZside, iphi, theZside);
				if ( h_Occupancy_hashedIndex -> GetBinContent(thisHashedIndex + 1) == 0 ) continue;
				float thisIntercalibConstant = h_scale_EB_hashedIndex -> GetBinContent (thisHashedIndex + 1);
				//	 std::cout<<iabseta*theZside<<" "<<iphi<<" "<<thisIntercalibConstant<<" "<<h_Occupancy_hashedIndex -> GetBinContent(thisHashedIndex+1)<<std::endl;
				h_scale_EB -> Fill(iphi, iabseta * theZside, thisIntercalibConstant); ///Fill with Last IC Value

				if (GetIetaFromHashedIndex(thisHashedIndex) == 85) h_IntercalibValues_test -> Fill (thisIntercalibConstant);

				p_IntercalibValues_iEta -> Fill(GetIetaFromHashedIndex(thisHashedIndex), thisIntercalibConstant);

				histoAuxiliary . Fill (thisIntercalibConstant);


				///Vectors
				IetaValues.push_back(iabseta * theZside);
				IphiValues.push_back(iphi);
				ICValues.push_back(thisIntercalibConstant);

				meanICforPhiRing += thisIntercalibConstant;
				totIphi++;

			}

			for ( int uu = 0; uu < totIphi; uu++ )
				meanICforPhiRingValues.push_back(meanICforPhiRing / totIphi);
			/// Note this info are not used furhter because channels near to the dead ones are not skipped


			for ( int iphi = MIN_IPHI; iphi <= MAX_IPHI; iphi++ ) {

				int thisHashedIndex = GetHashedIndexEB(iabseta * theZside, iphi, theZside);
				if ( h_Occupancy_hashedIndex -> GetBinContent(thisHashedIndex + 1) == 0 ) continue;

				h_scale_EB_meanOnPhi -> Fill(iphi, iabseta * theZside, ICValues.at(myPhiIndex) / meanICforPhiRingValues.at(myPhiIndex));
				myPhiIndex++; /// Normalization IC with the mean of each ring

			}
			f1.SetParameters(histoAuxiliary.GetEntries(), histoAuxiliary.GetMean(), histoAuxiliary.GetRMS());
			f1.SetRange(1 - 5 * histoAuxiliary.GetRMS(), 1 + 5 * histoAuxiliary.GetRMS());
			histoAuxiliary . Fit("f1", "QR+");

			if ( f1.GetParError(2) > 0.5 ) continue;
			h_IntercalibSpread_iEta -> SetBinContent( iabseta * theZside + 85 + 1, f1.GetParameter(2) );
			h_IntercalibSpread_iEta -> SetBinError( iabseta * theZside + 85 + 1, f1.GetParError(2) );

		}

	}

}
/// Save infos in the output
void FastCalibratorEB::saveHistos(TFile * f1)
{

	f1->cd();
	hC_IntercalibValues -> Write(*f1);
	hC_PullFromScalib -> Write(*f1);
	hC_EoP -> Write(*f1);
	hC_scale_EB -> Write("", *f1);
	h_scalib_EB -> Write();

	h_IntercalibValues_test -> Write();
	h_Occupancy_hashedIndex -> Write();
	p_IntercalibValues_iEta -> Write();
	h_Init_IntercalibValues -> Write();

	h_IntercalibSpread_iEta -> Write();
	h_scale_EB -> Write();
	h_scale_EB_meanOnPhi -> Write("h_scale_map");
	h_scale_EB_hashedIndex -> Write();

	h_occupancy -> Write();

	g_ICmeanVsLoop -> Write();
	g_ICrmsVsLoop -> Write();

	h_map_Dead_Channels -> Write() ;

	f1->Close();

	return;
}
/// Save E/0 distribution
void FastCalibratorEB::saveEoPeta(TFile * f2)
{
	f2->cd();
	hC_EoP_eta_ele ->Write(*f2);
	f2->Close();
}

///! Acquire fake dead channel list on order to evaluate the effected of IC near to them
void FastCalibratorEB::AcquireDeadXtal(TString inputDeadXtal, const bool & isDeadTriggerTower)
{

	if(inputDeadXtal != "NULL") {

		std::ifstream DeadXtal (inputDeadXtal.Data(), std::ios::binary);

		std::string buffer;
		int iEta, iPhi ;

		while(!DeadXtal.eof()) {
			getline(DeadXtal, buffer);
			std::stringstream line( buffer );
			line >> iEta >> iPhi ;

			if(iEta >= 0) {
				DeadXtal_HashedIndex.push_back(GetHashedIndexEB(iEta, iPhi, 1)) ;
				if(isDeadTriggerTower) {
					for(int iphi = -2 ; iphi <= 2 ; iphi ++) {
						for(int ieta = -2 ; ieta <= 2 ; ieta ++) {
							if(iphi == 0 && ieta == 0) continue ;
							DeadXtal_HashedIndex.push_back(GetHashedIndexEB(iEta + ieta, iPhi + iphi, 1)) ;
						}
					}
				}
			}

			else {

				DeadXtal_HashedIndex.push_back(GetHashedIndexEB(iEta, iPhi, -1)) ;
				if(isDeadTriggerTower) {
					for(int iphi = -2 ; iphi <= 2 ; iphi ++) {
						for(int ieta = -2 ; ieta <= 2 ; ieta ++) {
							if(iphi == 0 && ieta == 0) continue ;
							DeadXtal_HashedIndex.push_back(GetHashedIndexEB(iEta + ieta, iPhi + iphi, -1)) ;
						}
					}
				}
			}
		}

		sort(DeadXtal_HashedIndex.begin(), DeadXtal_HashedIndex.end());
	}

	else DeadXtal_HashedIndex.push_back(-9999);

}

///! Check if the channel is dead or not
bool FastCalibratorEB::CheckDeadXtal(const int & iEta, const int & iPhi)
{
	int hashed_Index;
	if(iEta >= 0) hashed_Index = GetHashedIndexEB(iEta, iPhi, 1);
	else hashed_Index = GetHashedIndexEB(iEta, iPhi, -1);

	std::vector<int>::iterator iter = find(DeadXtal_HashedIndex.begin(), DeadXtal_HashedIndex.end(), hashed_Index);

	if(iter != DeadXtal_HashedIndex.end())
		return true;
	else return false;
}
