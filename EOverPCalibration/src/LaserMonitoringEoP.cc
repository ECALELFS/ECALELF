//Class for the monitoring of laser corrections with E/p

#include "../interface/LaserMonitoringEoP.h"
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

LaserMonitoringEoP::LaserMonitoringEoP(TTree *tree, TTree *treeMC, int useRegression)
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
	Init(tree, treeMC, useRegression);
	//  Init(treeMC);

}

///==== deconstructor

LaserMonitoringEoP::~LaserMonitoringEoP()
{

	if (!fChain) return;
	delete fChain->GetCurrentFile();
	if (!fChainMC) return;
	delete fChainMC->GetCurrentFile();
}


///==== Variables initialization

void LaserMonitoringEoP::Init(TTree *tree, TTree *treeMC, int useRegression)
{

	/// Set branch addresses and branch pointers

	if (!tree) return;
	fChain = tree;
	fCurrent = -1;
	fChain->SetMakeClass(1);

	fChain->SetBranchStatus("*", 0);

	fChain->SetBranchStatus("*", 0);
	fChain->SetBranchStatus("runNumber", 1);
	fChain->SetBranchStatus("runTime", 1);
	fChain->SetBranchStatus("nPU", 1);
	fChain->SetBranchStatus("nPV", 1);
	fChain->SetBranchStatus("avgLCSCEle", 1);
	//  fChain->SetBranchStatus("seedLaserAlphaSCEle1",1);
	//  fChain->SetBranchStatus("ele1_EOverP",1);
	fChain->SetBranchStatus("etaSCEle", 1);
	fChain->SetBranchStatus("phiSCEle", 1);
	fChain->SetBranchStatus("energySCEle_must", 1);
	fChain->SetBranchStatus("energySCEle_corr", 1);
	fChain->SetBranchStatus("esEnergySCEle", 1);
	fChain->SetBranchStatus("pAtVtxGsfEle", 1);
	fChain->SetBranchStatus("seedXSCEle", 1);
	fChain->SetBranchStatus("seedYSCEle", 1);
	//  fChain->SetBranchStatus("ele1_seedZside",1);

	fChain->SetBranchAddress("runNumber", &runNumber);
	fChain->SetBranchAddress("runTime", &runTime);
	fChain->SetBranchAddress("nPU", &nPU);
	fChain->SetBranchAddress("nPV", &nPV);
	fChain->SetBranchAddress("avgLCSCEle", &avgLCSCEle[0]);
	//fChain->SetBranchAddress("seedLaserAlphaSCEle1", &seedLaserAlphaSCEle1);
	//  fChain->SetBranchAddress("ele1_EOverP", &EoP);
	fChain->SetBranchAddress("etaSCEle", &etaSCEle);
	fChain->SetBranchAddress("phiSCEle", &phiSCEle);
	if( useRegression < 1 )
		fChain->SetBranchAddress("energySCEle_must", &energySCEle);
	else
		fChain->SetBranchAddress("energySCEle_corr", &energySCEle_corr);
	//  fChain->SetBranchAddress("energySCEle_corr", &energySCEle);
	fChain->SetBranchAddress("esEnergySCEle", &esEnergySCEle);
	fChain->SetBranchAddress("pAtVtxGsfEle", &pAtVtxGsfEle);
	fChain->SetBranchAddress("seedXSCEle", &seedXSCEle);
	fChain->SetBranchAddress("seedYSCEle", &seedYSCEle);
	//  fChain->SetBranchAddress("ele1_seedZside", &seedZside);


	/// Set branch addresses and branch pointers for MC

	if (!treeMC) return;
	fChainMC = treeMC;
	fCurrentMC = -1;
	fChainMC->SetMakeClass(1);

	fChainMC->SetBranchStatus("*", 0);


	fChainMC->SetBranchStatus("*", 0);
	fChainMC->SetBranchStatus("runNumber", 1);
	fChainMC->SetBranchStatus("runTime", 1);
	fChainMC->SetBranchStatus("nPU", 1);
	fChainMC->SetBranchStatus("nPV", 1);
	fChainMC->SetBranchStatus("avgLCSCEle", 1);
	//  fChainMC->SetBranchStatus("seedLaserAlphaSCEle1",1);
	//  fChainMC->SetBranchStatus("ele1_EOverP",1);
	fChainMC->SetBranchStatus("etaSCEle", 1);
	fChainMC->SetBranchStatus("phiSCEle", 1);
	fChainMC->SetBranchStatus("energySCEle_must", 1);
	fChainMC->SetBranchStatus("energySCEle_corr", 1);
	fChainMC->SetBranchStatus("esEnergySCEle", 1);
	fChainMC->SetBranchStatus("pAtVtxGsfEle", 1);
	fChainMC->SetBranchStatus("seedXSCEle", 1);
	fChainMC->SetBranchStatus("seedYSCEle", 1);
	//  fChainMC->SetBranchStatus("ele1_seedZside",1);

	fChainMC->SetBranchAddress("runNumber", &runNumber);
	fChainMC->SetBranchAddress("runTime", &runTime);
	fChainMC->SetBranchAddress("nPU", &nPU);
	fChainMC->SetBranchAddress("nPV", &nPV);
	fChainMC->SetBranchAddress("avgLCSCEle", &avgLCSCEle[0]);
	//fChainMC->SetBranchAddress("seedLaserAlphaSCEle1", &seedLaserAlphaSCEle1);
	//  fChainMC->SetBranchAddress("ele1_EOverP", &EoP);
	fChainMC->SetBranchAddress("etaSCEle", &etaSCEle);
	fChainMC->SetBranchAddress("phiSCEle", &phiSCEle);
	if( useRegression < 1 )
		fChainMC->SetBranchAddress("energySCEle_must", &energySCEle);
	else
		fChainMC->SetBranchAddress("energySCEle_corr", &energySCEle_corr);
	//  fChainMC->SetBranchAddress("energySCEle_corr", &energySCEle);
	fChainMC->SetBranchAddress("esEnergySCEle", &esEnergySCEle);
	fChainMC->SetBranchAddress("pAtVtxGsfEle", &pAtVtxGsfEle);
	fChainMC->SetBranchAddress("seedXSCEle", &seedXSCEle);
	fChainMC->SetBranchAddress("seedYSCEle", &seedYSCEle);
	//  fChainMC->SetBranchAddress("ele1_seedZside", &seedZside);

}


/// Calibration Loop over the ntu events
void LaserMonitoringEoP::Loop( float yMIN, float yMAX, std::string EBEE, int evtsPerPoint, int useRegression, std::string dayMin, std::string dayMax, std::string dayZOOM,
                               std::string LUMI, int vsPU)
{
	if (fChain == 0) return;
	if (fChainMC == 0) return;

	/// ----------------- Calibration Loops -----------------------------//

	float timeLapse = 24.; // in hours
	//    int t1 = 1267401600;   //  1 Mar 2010
	//int t2 = 1325289600;   // 31 Dec 2011
	//int  t1 = 1400000000;
	//int  t2 = 1600000000;

	// float yMIN = 0.65;
	//float yMAX = 1.10;


	// Set style options
	setLaserPlotStyle();
	gStyle->SetPadTickX(1);
	gStyle->SetPadTickY(1);
	gStyle->SetOptTitle(0);
	gStyle->SetOptStat(1110);
	gStyle->SetOptFit(1);

	// Set fitting options
	TVirtualFitter::SetDefaultFitter("Fumili2");



	//-----------------
	// Input parameters


	std::cout << "\n***************************************************************************************************************************" << std::endl;

	//  std::string dayMin = "";
	//std::string dayMax = "";
	std::string dayMinLabel = "";
	std::string dayMaxLabel = "";
	std::string dayZOOMLabel = "";
	float absEtaMin = -1.;
	float absEtaMax = -1.;
	int IetaMin = -1;
	int IetaMax = -1;
	int IphiMin = -1;
	int IphiMax = -1;


	int t1 = dateToInt(dayMin);
	int t2 = dateToInt(dayMax);
	int t3 = dateToInt(dayZOOM);

	if (vsPU) {
		t1 = 0;
		t2 = 60;
		t3 = 0;
	}

	/*
	if(argc >= 5)
	{
	  dayMin = std::string(argv[4])+" "+std::string(argv[5])+" "+std::string(argv[6]);
	  dayMax = std::string(argv[7])+" "+std::string(argv[8])+" "+std::string(argv[9]);
	  dayMinLabel = std::string(argv[4])+"_"+std::string(argv[5])+"_"+std::string(argv[6]);
	  dayMaxLabel = std::string(argv[7])+"_"+std::string(argv[8])+"_"+std::string(argv[9]);

	}
	if(argc >= 11)
	{
	  yMIN = atof(argv[10]);
	  yMAX = atof(argv[11]);
	}
	if(argc >= 13)
	{
	  absEtaMin = atof(argv[12]);
	  absEtaMax = atof(argv[13]);
	}
	if(argc >= 15)
	{
	  IetaMin = atoi(argv[14]);
	  IetaMax = atoi(argv[15]);
	  IphiMin = atoi(argv[16]);
	  IphiMax = atoi(argv[17]);
	}
	*/

	std::cout << "EBEE: "          << EBEE          << std::endl;
	std::cout << "evtsPerPoint: "  << evtsPerPoint  << std::endl;
	std::cout << "useRegression: " << useRegression << std::endl;
	std::cout << "dayMin: "        << dayMin        << std::endl;
	std::cout << "dayZOOM: "       << dayZOOM       << std::endl;
	std::cout << "dayMax: "        << dayMax        << std::endl;
	std::cout << "yMin: "          << yMIN          << std::endl;
	std::cout << "yMax: "          << yMAX          << std::endl;
	std::cout << "absEtaMin: "     << absEtaMin     << std::endl;
	std::cout << "absEtaMax: "     << absEtaMax     << std::endl;
	std::cout << "IetaMin: "       << IetaMin       << std::endl;
	std::cout << "IetaMax: "       << IetaMax       << std::endl;
	std::cout << "IphiMin: "       << IphiMin       << std::endl;
	std::cout << "IphiMax: "       << IphiMax       << std::endl;
	std::cout << "t1: "            << t1            << std::endl;
	std::cout << "t2: "            << t2            << std::endl;
	std::cout << "t3"              << t3            << std::endl;

	dayZOOM = "";
	dayMin = "";
	dayMax = "";


	//-------------------
	// Define in/outfiles

	std::string folderName = std::string(EBEE) + "_" + dayMinLabel + "_" + dayMaxLabel;
	if( (absEtaMin != -1.) && (absEtaMax != -1.) ) {
		char absEtaBuffer[50];
		sprintf(absEtaBuffer, "_%.2f-%.2f", absEtaMin, absEtaMax);
		folderName += std::string(absEtaBuffer);
	}

	if( (IetaMin != -1.) && (IetaMax != -1.) && (IphiMin != -1.) && (IphiMax != -1.) ) {
		char absEtaBuffer[50];
		sprintf(absEtaBuffer, "_Ieta_%d-%d_Iphi_%d-%d", IetaMin, IetaMax, IphiMin, IphiMax);
		folderName += std::string(absEtaBuffer);
	}

	gSystem->mkdir(folderName.c_str());
	TFile* o = new TFile((folderName + "/" + folderName + "_histos.root").c_str(), "RECREATE");



	// Get trees
	std::cout << std::endl;

	/*
	TChain* ntu_DA = new TChain("simpleNtupleEoverP/SimpleNtupleEoverP");
	FillChain(ntu_DA,"inputDATA.txt");
	std::cout << "     DATA: " << std::setw(8) << ntu_DA->GetEntries() << " entries" << std::endl;

	TChain* ntu_MC = new TChain("simpleNtupleEoverP/SimpleNtupleEoverP");
	FillChain(ntu_MC,"inputMC.txt");
	std::cout << "REFERENCE: " << std::setw(8) << ntu_MC->GetEntries() << " entries" << std::endl;
	*/

	if (fChain->GetEntries() == 0 || fChainMC->GetEntries() == 0 ) {
		std::cout << "Error: At least one file is empty" << std::endl;
		return;
	}






	//--------------------------------------------------------
	// Define PU correction (to be used if useRegression == 0)

	// corr = p0 + p1 * nPU
	float p0_EB;
	float p1_EB;
	float p0_EE;
	float p1_EE;

	if( useRegression == 0 ) {
		//2012 EB
		p0_EB = 0.9991;
		p1_EB = 0.0001635;
		//2012 EE
		p0_EE = 0.9968;
		p1_EE = 0.001046;
	} else {
		//2012 EB
		p0_EB = 1.001;
		p1_EB = -0.000143;
		//2012 EE
		p0_EE = 1.00327;
		p1_EE = -0.000432;
	}

	float p0 = -1.;
	float p1 = -1.;

	if( strcmp(EBEE.c_str(), "EB") == 0 ) {
		p0 = p0_EB;
		p1 = p1_EB;
	} else {
		p0 = p0_EE;
		p1 = p1_EE;
	}

	//2015
	p0 = 1.;
	p1 = 0.;




	//---------------------------------
	// Build the reference distribution

	std::cout << std::endl;
	std::cout << "***** Build reference for " << EBEE << " *****" << std::endl;

	TH1F* h_template = new TH1F("template", "", 2000, 0., 5.);

	for(int ientry = 0; ientry < fChainMC->GetEntries(); ++ientry) {
		if( (ientry % 10000 == 0) ) std::cout << "reading MC entry " << ientry << "/" << fChainMC->GetEntries() << "\r" << std::endl; //std::flush;
		fChainMC->GetEntry(ientry);

		// selections
		if( (strcmp(EBEE.c_str(), "EB") == 0) && (fabs(etaSCEle[0]) > 1.479) )                    continue; // barrel
		if( (strcmp(EBEE.c_str(), "EB_0_1") == 0) && (fabs(etaSCEle[0]) > 1.0)) continue; // inner barrel
		if( (strcmp(EBEE.c_str(), "EB_1_1479") == 0) && (fabs(etaSCEle[0]) < 1.0 || fabs(etaSCEle[0]) > 1.479) )                     continue; // barrel
		if( (strcmp(EBEE.c_str(), "EE") == 0) && (fabs(etaSCEle[0]) < 1.479 || fabs(etaSCEle[0]) > 2.5) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EE_1479_2") == 0) && (fabs(etaSCEle[0]) < 1.479 || fabs(etaSCEle[0]) > 2.0) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EE_2_25") == 0) && (fabs(etaSCEle[0]) < 2.0 || fabs(etaSCEle[0]) > 2.5) ) continue; // endcap

		if( (strcmp(EBEE.c_str(), "EEp") == 0) && (etaSCEle[0] < 1.479 || etaSCEle[0] > 2.5 ) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EEp_1479_2") == 0) && (etaSCEle[0] < 1.479 || etaSCEle[0] > 2.0) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EEp_2_25") == 0) && (etaSCEle[0] < 2.0 || etaSCEle[0] > 2.5) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EEp_2_225") == 0) && (etaSCEle[0] < 2.0 || etaSCEle[0] > 2.25) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EEp_225_25") == 0) && (etaSCEle[0] < 2.25 || etaSCEle[0] > 2.25) ) continue; // endcap

		if( (strcmp(EBEE.c_str(), "EEm") == 0) && (etaSCEle[0] > -1.479 || etaSCEle[0] < -2.5 ) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EEm_1479_2") == 0) && (etaSCEle[0] > -1.479 || etaSCEle[0] < -2.0) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EEm_2_25") == 0) && (etaSCEle[0] > -2.0 || etaSCEle[0] < -2.5) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EEm_2_225") == 0) && (etaSCEle[0] > -2.0 || etaSCEle[0] < -2.25) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EEm_225_25") == 0) && (etaSCEle[0] > -2.25 || etaSCEle[0] < -2.5) ) continue; // endcap

		if( (absEtaMin != -1.) && (absEtaMax != -1.) ) {
			if( (fabs(etaSCEle[0]) < absEtaMin) || (fabs(etaSCEle[0]) > absEtaMax) ) continue;
		}

		if( (IetaMin != -1.) && (IetaMax != -1.) && (IphiMin != -1.) && (IphiMax != -1.) ) {
			if( (seedXSCEle[0] < IetaMin) || (seedXSCEle[0] > IetaMax) ) continue;
			if( (seedYSCEle[0] < IphiMin) || (seedYSCEle[0] > IphiMax) ) continue;
		}

		// PU correction
		float PUCorr = (p0 + p1 * nPU);
		//std::cout << "p0: " << p0  << "   p1: " << p1 << "   nPU: " << nPU << std::endl;

		// fill the template histogram
		h_template -> Fill( (energySCEle[0] - esEnergySCEle[0]) / (pAtVtxGsfEle[0] - esEnergySCEle[0]) / PUCorr );
	}

	std::cout << "Reference built for " << EBEE << " - " << h_template->GetEntries() << " events" << std::endl;






	//---------------------
	// Loop and sort events

	std::cout << std::endl;
	std::cout << "***** Sort events and define bins *****" << std::endl;

	int nEntries = fChain -> GetEntriesFast();
	int nSavePts = 0;
	std::vector<bool> isSavedEntries(nEntries);
	std::vector<Sorter> sortedEntries;
	std::vector<int> timeStampFirst;

	for(int ientry = 0; ientry < nEntries; ++ientry) {
		fChain -> GetEntry(ientry);
		isSavedEntries.at(ientry) = false;

		if( (ientry % 10000 == 0) ) std::cout << "reading data entry " << ientry << "\r" << std::endl; //std::flush;

		// selections
		if( (strcmp(EBEE.c_str(), "EB") == 0) && (fabs(etaSCEle[0]) > 1.479) )                    continue; // barrel
		if( (strcmp(EBEE.c_str(), "EB_0_1") == 0) && (fabs(etaSCEle[0]) > 1.0)) continue; // inner barrel
		if( (strcmp(EBEE.c_str(), "EB_1_1479") == 0) && (fabs(etaSCEle[0]) < 1.0 || fabs(etaSCEle[0]) > 1.479) )                     continue; // barrel
		if( (strcmp(EBEE.c_str(), "EE") == 0) && (fabs(etaSCEle[0]) < 1.479 || fabs(etaSCEle[0]) > 2.5) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EE_1479_2") == 0) && (fabs(etaSCEle[0]) < 1.479 || fabs(etaSCEle[0]) > 2.0) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EE_2_25") == 0) && (fabs(etaSCEle[0]) < 2.0 || fabs(etaSCEle[0]) > 2.5) ) continue; // endcap

		if( (strcmp(EBEE.c_str(), "EEp") == 0) && (etaSCEle[0] < 1.479 || etaSCEle[0] > 2.5 ) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EEp_1479_2") == 0) && (etaSCEle[0] < 1.479 || etaSCEle[0] > 2.0) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EEp_2_25") == 0) && (etaSCEle[0] < 2.0 || etaSCEle[0] > 2.5) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EEp_2_225") == 0) && (etaSCEle[0] < 2.0 || etaSCEle[0] > 2.25) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EEp_225_25") == 0) && (etaSCEle[0] < 2.25 || etaSCEle[0] > 2.25) ) continue; // endcap

		if( (strcmp(EBEE.c_str(), "EEm") == 0) && (etaSCEle[0] > -1.479 || etaSCEle[0] < -2.5 ) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EEm_1479_2") == 0) && (etaSCEle[0] > -1.479 || etaSCEle[0] < -2.0) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EEm_2_25") == 0) && (etaSCEle[0] > -2.0 || etaSCEle[0] < -2.5) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EEm_2_225") == 0) && (etaSCEle[0] > -2.0 || etaSCEle[0] < -2.25) ) continue; // endcap
		if( (strcmp(EBEE.c_str(), "EEm_225_25") == 0) && (etaSCEle[0] > -2.25 || etaSCEle[0] < -2.5) ) continue; // endcap

		if( (absEtaMin != -1.) && (absEtaMax != -1.) ) {
			if( (fabs(etaSCEle[0]) < absEtaMin) || (fabs(etaSCEle[0]) > absEtaMax) ) continue;
		}

		if( (IetaMin != -1.) && (IetaMax != -1.) && (IphiMin != -1.) && (IphiMax != -1.) ) {
			if( (seedXSCEle[0] < IetaMin) || (seedXSCEle[0] > IetaMax) ) continue;
			if( (seedYSCEle[0] < IphiMin) || (seedYSCEle[0] > IphiMax) ) continue;
		}

		if (vsPU == 0) {
			if( runTime < t1 ) continue;
			if( runTime > t2 ) continue;
		} else {
			if( nPV < t1 ) continue;
			if( nPV > t2 ) continue;
		}

		if( avgLCSCEle[0] <= 0. ) continue;

		isSavedEntries.at(ientry) = true;


		// fill sorter
		Sorter dummy;
		if (vsPU == 0) dummy.time = runTime;
		else         dummy.time = nPV;
		dummy.entry = ientry;
		sortedEntries.push_back(dummy);

		++nSavePts;
	}

	// sort events
	std::sort(sortedEntries.begin(), sortedEntries.end(), Sorter());
	std::cout << "Data sorted in " << EBEE << " - " << nSavePts << " events" << std::endl;

	std::map<int, int> antiMap;
	for(unsigned int iSaved = 0; iSaved < sortedEntries.size(); ++iSaved)
		antiMap[sortedEntries.at(iSaved).entry] = iSaved;


	//---------------------
	// Loop and define bins

	// "wide" bins - find events with time separation bigger than 1 day
	int nWideBins = 1;
	std::vector<int> wideBinEntryMax;
	int timeStampOld = -1;

	//  TEventList* evlist=new TEventList("events");
	//  data->Draw(">>events","","goff");
	//  TEventList* evlist = (TEventList*) gDirectory->Get("events");

	wideBinEntryMax.push_back(0);

	if (vsPU == 0) {
		for(int iSaved = 0; iSaved < nSavePts; ++iSaved) {
			if( iSaved % 10000 == 0 ) std::cout << "reading saved entry " << iSaved << "\r" << std::endl; //std::flush;
			fChain->GetEntry(sortedEntries[iSaved].entry);
			//    data->GetEntry(evlist->GetEntry(sortedEntries[iSaved].entry));

			if( iSaved == 0 ) {
				timeStampOld = runTime;
				continue;
			}

			if( (runTime - timeStampOld) / 3600. > timeLapse ) {
				++nWideBins;
				wideBinEntryMax.push_back(iSaved - 1);
			}

			timeStampOld = runTime;
		}
	}
	std::cout << std::endl;
	wideBinEntryMax.push_back(nSavePts);


	// bins with approximatively evtsPerPoint events per bin
	int nBins = 0;
	std::vector<int> binEntryMax;

	binEntryMax.push_back(0);
	for(int wideBin = 0; wideBin < nWideBins; ++wideBin) {
		int nTempBins = std::max(1, int( (wideBinEntryMax.at(wideBin + 1) - wideBinEntryMax.at(wideBin)) / evtsPerPoint ));
		int nTempBinEntries = int( (wideBinEntryMax.at(wideBin + 1) - wideBinEntryMax.at(wideBin)) / nTempBins );

		for(int tempBin = 0; tempBin < nTempBins; ++tempBin) {
			++nBins;
			if( tempBin < nTempBins - 1 )
				binEntryMax.push_back( wideBinEntryMax.at(wideBin) + (tempBin + 1)*nTempBinEntries );
			else
				binEntryMax.push_back( wideBinEntryMax.at(wideBin + 1) );
		}
	}

	std::cout << "nBins = " << nBins << std::endl;
	//for(int bin = 0; bin < nBins; ++bin)
	//  std::cout << "bin: " << bin
	//            << "   entry min: " << setw(6) << binEntryMax.at(bin)
	//            << "   entry max: " << setw(6) << binEntryMax.at(bin+1)
	//            << "   events: "    << setw(6) << binEntryMax.at(bin+1)-binEntryMax.at(bin)
	//            << std::endl;


	// histogram definition

	TH1F* h_scOccupancy_eta  = new TH1F("h_scOccupancy_eta", "", 298, -2.6, 2.6);
	TH1F* h_scOccupancy_phi  = new TH1F("h_scOccupancy_phi", "", 363, -3.1765, 3.159);
	SetHistoStyle(h_scOccupancy_eta);
	SetHistoStyle(h_scOccupancy_phi);

	TH2F* h_seedOccupancy_EB  = new TH2F("h_seedOccupancy_EB", "",  171, -86., 85., 361,   0., 361.);
	TH2F* h_seedOccupancy_EEp = new TH2F("h_seedOccupancy_EEp", "", 101,   0., 101., 100,   0., 101.);
	TH2F* h_seedOccupancy_EEm = new TH2F("h_seedOccupancy_EEm", "", 101,   0., 101., 100,   0., 101.);
	SetHistoStyle(h_seedOccupancy_EB);
	SetHistoStyle(h_seedOccupancy_EEp);
	SetHistoStyle(h_seedOccupancy_EEm);

	TH1F* h_EoP_spread = new TH1F("h_EoP_spread", "", 100, yMIN, yMAX);
	TH1F* h_EoC_spread = new TH1F("h_EoC_spread", "", 100, yMIN, yMAX);
	TH1F* h_EoP_spread_run = new TH1F("h_EoP_spread_run", "", 100, yMIN, yMAX);
	TH1F* h_EoC_spread_run = new TH1F("h_EoC_spread_run", "", 100, yMIN, yMAX);
	SetHistoStyle(h_EoP_spread, "EoP");
	SetHistoStyle(h_EoC_spread, "EoC");
	SetHistoStyle(h_EoP_spread_run, "EoP");
	SetHistoStyle(h_EoC_spread_run, "EoC");

	TH1F* h_EoP_chi2 = new TH1F("h_EoP_chi2", "", 50, 0., 5.);
	TH1F* h_EoC_chi2 = new TH1F("h_EoC_chi2", "", 50, 0., 5.);
	SetHistoStyle(h_EoP_chi2, "EoP");
	SetHistoStyle(h_EoC_chi2, "EoC");

	TH1F** h_EoP = new TH1F*[nBins];
	TH1F** h_EoC = new TH1F*[nBins];
	TH1F** h_Las = new TH1F*[nBins];
	TH1F** h_Tsp = new TH1F*[nBins];
	TH1F** h_Cvl = new TH1F*[nBins];

	for(int i = 0; i < nBins; ++i) {
		char histoName[80];

		sprintf(histoName, "EoP_%d", i);
		h_EoP[i] = new TH1F(histoName, histoName, 2000, 0., 5.);
		SetHistoStyle(h_EoP[i], "EoP");

		sprintf(histoName, "EoC_%d", i);
		h_EoC[i] = new TH1F(histoName, histoName, 2000, 0., 5.);
		SetHistoStyle(h_EoC[i], "EoC");

		sprintf(histoName, "Las_%d", i);
		h_Las[i] = new TH1F(histoName, histoName, 500, 0.5, 1.5);

		sprintf(histoName, "Tsp_%d", i);
		h_Tsp[i] = new TH1F(histoName, histoName, 500, 0.5, 1.5);

	}


	// function definition
	TF1** f_EoP = new TF1*[nBins];
	TF1** f_EoC = new TF1*[nBins];


	// graphs definition
	TGraphAsymmErrors* g_fit   = new TGraphAsymmErrors();
	TGraphAsymmErrors* g_c_fit = new TGraphAsymmErrors();

	TGraphAsymmErrors* g_fit_run   = new TGraphAsymmErrors();
	TGraphAsymmErrors* g_c_fit_run = new TGraphAsymmErrors();

	TGraph* g_las = new TGraph();

	TGraphErrors* g_LT = new TGraphErrors();

	g_fit->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
	g_fit->GetXaxis()->SetTimeDisplay(1);
	g_c_fit->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
	g_c_fit->GetXaxis()->SetTimeDisplay(1);
	g_las->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
	g_las->GetXaxis()->SetTimeDisplay(1);
	g_LT->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
	g_LT->GetXaxis()->SetTimeDisplay(1);



	//------------------------------------
	// loop on the saved and sorted events

	std::cout << std::endl;
	std::cout << "***** Fill and fit histograms *****" << std::endl;

	std::vector<int> Entries(nBins);
	std::vector<double> AveTime(nBins);
	std::vector<int> MinTime(nBins);
	std::vector<int> MaxTime(nBins);
	std::vector<double> AveRun(nBins);
	std::vector<int> MinRun(nBins);
	std::vector<int> MaxRun(nBins);
	std::vector<double> AveLT(nBins);
	std::vector<double> AveLT2(nBins);

	int iSaved = -1;
	for(int ientry = 0; ientry < nEntries; ++ientry) {
		if( (ientry % 100000 == 0) ) std::cout << "reading entry " << ientry << "\r" << std::endl; //std::flush;

		if( isSavedEntries.at(ientry) == false ) continue;

		++iSaved;

		int iSaved = antiMap[ientry];
		int bin = -1;
		for(bin = 0; bin < nBins; ++bin)
			if( iSaved >= binEntryMax.at(bin) && iSaved < binEntryMax.at(bin + 1) )
				break;

		//std::cout << "bin = " << bin << "   iSaved = "<< iSaved << std::endl;
		fChain->GetEntry(ientry);



		Entries[bin] += 1;

		if(vsPU == 0) {
			if( iSaved == binEntryMax.at(bin) + 1 )   MinTime[bin] = runTime;
			if( iSaved == binEntryMax.at(bin + 1) - 1 ) MaxTime[bin] = runTime;
			AveTime[bin] += runTime;
		} else {
			if( iSaved == binEntryMax.at(bin) + 1 )   MinTime[bin] = nPV;
			if( iSaved == binEntryMax.at(bin + 1) - 1 ) MaxTime[bin] = nPV;
			AveTime[bin] += nPV;
		}

		if( iSaved == binEntryMax.at(bin) + 1 )   MinRun[bin] = runNumber;
		if( iSaved == binEntryMax.at(bin + 1) - 1 ) MaxRun[bin] = runNumber;
		AveRun[bin] += runNumber;

		//    float LT = (-1. / seedLaserAlphaSCEle1 * log(avgLCSCEle[0]));
		float LT = 1.;
		AveLT[bin] += LT;
		AveLT2[bin] += LT * LT;

		// PU correction
		float PUCorr = (p0 + p1 * nPU);

		// fill the histograms
		(h_EoP[bin]) -> Fill( (energySCEle[0] - esEnergySCEle[0]) / (pAtVtxGsfEle[0] - esEnergySCEle[0]) / avgLCSCEle[0] / PUCorr);
		(h_EoC[bin]) -> Fill( (energySCEle[0] - esEnergySCEle[0]) / (pAtVtxGsfEle[0] - esEnergySCEle[0]) / PUCorr );

		(h_Las[bin]) -> Fill(avgLCSCEle[0]);
		(h_Tsp[bin]) -> Fill(1. / avgLCSCEle[0]);

		h_scOccupancy_eta -> Fill(etaSCEle[0]);
		h_scOccupancy_phi -> Fill(phiSCEle[0]);
		if(fabs(etaSCEle[0]) < 1.449)
			h_seedOccupancy_EB -> Fill(seedXSCEle[0], seedYSCEle[0]);
		else if(etaSCEle[0] > 1.449)
			h_seedOccupancy_EEp -> Fill(seedXSCEle[0], seedYSCEle[0]);
		else if(etaSCEle[0] < -1.449)
			h_seedOccupancy_EEm -> Fill(seedXSCEle[0], seedYSCEle[0]);
	}

	for(int bin = 0; bin < nBins; ++bin) {
		AveTime[bin] = 1. * AveTime[bin] / (binEntryMax.at(bin + 1) - binEntryMax.at(bin));
		AveRun[bin]  = 1. * AveRun[bin]  / (binEntryMax.at(bin + 1) - binEntryMax.at(bin));
		AveLT[bin]   = 1. * AveLT[bin]   / (binEntryMax.at(bin + 1) - binEntryMax.at(bin));
		AveLT2[bin]  = 1. * AveLT2[bin]  / (binEntryMax.at(bin + 1) - binEntryMax.at(bin));
		//std::cout << date << " " << AveTime[i] << " " << MinTime[i] << " " << MaxTime[i] << std::endl;
	}






	int rebin = 2;
	if( strcmp(EBEE.c_str(), "EE") == 0 ) rebin *= 2;

	h_template -> Rebin(rebin);



	float EoP_scale = 0.;
	float EoP_err = 0.;
	int   EoP_nActiveBins = 0;

	float EoC_scale = 0.;
	float EoC_err = 0.;
	int   EoC_nActiveBins = 0;

	float LCInv_scale = 0;

	std::vector<int> validBins;
	for(int i = 0; i < nBins; ++i) {
		bool isValid = true;

		h_EoP[i] -> Rebin(rebin);
		h_EoC[i] -> Rebin(rebin);



		//------------------------------------
		// Fill the graph for uncorrected data

		// define the fitting function
		// N.B. [0] * ( [1] * f( [1]*(x-[2]) ) )

		//o -> cd();
		char convolutionName[50];
		sprintf(convolutionName, "h_convolution_%d", i);
		//h_Cvl[i] = ConvoluteTemplate(std::string(convolutionName),h_template,h_Las[i],32768,-5.,5.);
		h_Cvl[i] = MellinConvolution(std::string(convolutionName), h_template, h_Tsp[i]);

		histoFunc* templateHistoFunc = new histoFunc(h_template);
		histoFunc* templateConvolutedHistoFunc = new histoFunc(h_Cvl[i]);
		char funcName[50];

		sprintf(funcName, "f_EoP_%d", i);

		if( strcmp(EBEE.c_str(), "EB") == 0 )
			f_EoP[i] = new TF1(funcName, templateConvolutedHistoFunc, 0.8 * (h_Tsp[i]->GetMean()), 1.4 * (h_Tsp[i]->GetMean()), 3, "histoFunc");
		else
			f_EoP[i] = new TF1(funcName, templateConvolutedHistoFunc, 0.75 * (h_Tsp[i]->GetMean()), 1.5 * (h_Tsp[i]->GetMean()), 3, "histoFunc");

		f_EoP[i] -> SetParName(0, "Norm");
		f_EoP[i] -> SetParName(1, "Scale factor");
		f_EoP[i] -> SetLineWidth(1);
		f_EoP[i] -> SetNpx(10000);

		double xNorm = h_EoP[i]->GetEntries() / h_template->GetEntries() *
		               h_EoP[i]->GetBinWidth(1) / h_template->GetBinWidth(1);

		f_EoP[i] -> FixParameter(0, xNorm);
		f_EoP[i] -> SetParameter(1, 1.);
		f_EoP[i] -> FixParameter(2, 0.);
		f_EoP[i] -> SetLineColor(kRed + 2);

		int fStatus = 0;
		int nTrials = 0;
		TFitResultPtr rp;

		rp = h_EoP[i] -> Fit(funcName, "ERLS+");
		while( (fStatus != 0) && (nTrials < 10) ) {
			rp = h_EoP[i] -> Fit(funcName, "ERLS+");
			fStatus = rp;
			if(fStatus == 0) break;
			++nTrials;
		}

		// fill the graph
		double eee = f_EoP[i]->GetParError(1);
		//float k    = f_EoP[i]->GetParameter(1);
		float k    = f_EoP[i]->GetParameter(1) / h_Tsp[i]->GetMean(); //needed when using mellin's convolution

		/*
		std::cout << i <<"--nocorr---- "<< 1./k << std::endl;
		std::cout <<" condizione 1: " << h_EoP[i]->GetEntries() << "  fStatus: " << fStatus << " eee: " << eee << "con eee che ci piace essere maggiore di : " <<  0.05*h_template->GetRMS()/sqrt(evtsPerPoint) << std::endl ;
		getchar();
		*/


		//    if( (h_EoP[i]->GetEntries() > 3) && (fStatus == 0) && (eee > 0.05*h_template->GetRMS()/sqrt(evtsPerPoint)) )
		if( (h_EoP[i]->GetEntries() > 500) && (fStatus == 0) ) {
			float date = (float)AveTime[i];
			float dLow = (float)(AveTime[i] - MinTime[i]);
			float dHig = (float)(MaxTime[i] - AveTime[i]);
			float run = (float)AveRun[i];
			float rLow = (float)(AveRun[i] - MinRun[i]);
			float rHig = (float)(MaxRun[i] - AveRun[i]);

			g_fit -> SetPoint(i,  date , 1. / k);
			g_fit -> SetPointError(i, dLow , dHig, eee / k / k, eee / k / k);

			g_fit_run -> SetPoint(i,  run , 1. / k);
			g_fit_run -> SetPointError(i, rLow , rHig, eee / k / k, eee / k / k);

			std::cout << "************-------------------*****************" << std::endl;


			h_EoP_chi2 -> Fill(f_EoP[i]->GetChisquare() / f_EoP[i]->GetNDF());

			EoP_scale += 1. / k;
			EoP_err += eee / k / k;
			++EoP_nActiveBins;
		} else {
			std::cout << "Fitting uncorrected time bin: " << i << "   Fail status: " << fStatus << "   sigma: " << eee << std::endl;
			isValid = false;
		}

		//----------------------------------
		// Fill the graph for corrected data

		// define the fitting function
		// N.B. [0] * ( [1] * f( [1]*(x-[2]) ) )

		sprintf(funcName, "f_EoC_%d", i);
		if( strcmp(EBEE.c_str(), "EB") == 0 )
			f_EoC[i] = new TF1(funcName, templateHistoFunc, 0.8, 1.4, 3, "histoFunc");
		else
			f_EoC[i] = new TF1(funcName, templateHistoFunc, 0.75, 1.5, 3, "histoFunc");
		f_EoC[i] -> SetParName(0, "Norm");
		f_EoC[i] -> SetParName(1, "Scale factor");
		f_EoC[i] -> SetLineWidth(1);
		f_EoC[i] -> SetNpx(10000);

		xNorm = h_EoC[i]->GetEntries() / h_template->GetEntries() *
		        h_EoC[i]->GetBinWidth(1) / h_template->GetBinWidth(1);

		f_EoC[i] -> FixParameter(0, xNorm);
		f_EoC[i] -> SetParameter(1, 0.99);
		f_EoC[i] -> FixParameter(2, 0.);
		f_EoC[i] -> SetLineColor(kGreen + 2);


		rp = h_EoC[i] -> Fit(funcName, "ERLS+");
		fStatus = rp;
		nTrials = 0;
		while( (fStatus != 0) && (nTrials < 10) ) {
			rp = h_EoC[i] -> Fit(funcName, "ERLS+");
			fStatus = rp;
			if(fStatus == 0) break;
			++nTrials;
		}


		// fill the graph
		k   = f_EoC[i]->GetParameter(1);
		eee = f_EoC[i]->GetParError(1);

		/* std::cout << i <<"--corr---- "<< 1./k << std::endl;
		std::cout <<" condizione 1: " << h_EoP[i]->GetEntries() << "  fStatus: " << fStatus << " eee: " << eee << "con eee che ci piace essere maggiore di : " <<  0.05*h_template->GetRMS()/sqrt(evtsPerPoint) << std::endl ;
		getchar();
		*/


		if( (h_EoC[i]->GetEntries() > 500) && (fStatus == 0) )
			//  if( (h_EoC[i]->GetEntries() > 10) && (fStatus == 0) && (eee > 0.05*h_template->GetRMS()/sqrt(evtsPerPoint)) )
		{
			float date = (float)AveTime[i];
			float dLow = (float)(AveTime[i] - MinTime[i]);
			float dHig = (float)(MaxTime[i] - AveTime[i]);
			float run = (float)AveRun[i];
			float rLow = (float)(AveRun[i] - MinRun[i]);
			float rHig = (float)(MaxRun[i] - AveRun[i]);

			g_c_fit -> SetPoint(i,  date , 1. / k);
			g_c_fit -> SetPointError(i, dLow , dHig , eee / k / k, eee / k / k);

			g_c_fit_run -> SetPoint(i,  run , 1. / k);
			g_c_fit_run -> SetPointError(i, rLow , rHig, eee / k / k, eee / k / k);
			std::cout << "************-------------------*****************" << std::endl;


			h_EoC_chi2 -> Fill(f_EoC[i]->GetChisquare() / f_EoP[i]->GetNDF());

			EoC_scale += 1. / k;
			EoC_err += eee / k / k;
			++EoC_nActiveBins;
		} else {
			std::cout << "Fitting corrected time bin: " << i << "   Fail status: " << fStatus << "   sigma: " << eee << std::endl;
			isValid = false;
		}

		if( isValid == true ) validBins.push_back(i);
	}

	EoP_scale /= EoP_nActiveBins;
	EoP_err   /= EoP_nActiveBins;

	EoC_scale /= EoC_nActiveBins;
	EoC_err   /= EoC_nActiveBins;






	//----------------------------------------
	// Fill the graph for avg laser correction

	//fede
	for(unsigned int itr = 0; itr < validBins.size(); ++itr) {
		int i = validBins.at(itr);
		g_las -> SetPoint(itr, (float)AveTime[i], h_Tsp[i]->GetMean());
		g_LT  -> SetPoint(itr, (float)AveTime[i], AveLT[i] );
		g_LT  -> SetPointError(itr, 0., sqrt(AveLT2[i] - AveLT[i]*AveLT[i]) / sqrt(Entries[i]) );

		LCInv_scale += h_Tsp[i]->GetMean();
	}

	LCInv_scale /= validBins.size();






	//---------------
	// Rescale graphs

	float yscale = 1.;
	//float yscale = 1./EoC_scale;

	for(unsigned int itr = 0; itr < validBins.size(); ++itr) {
		double x, y;
		g_fit -> GetPoint(itr, x, y);
		g_fit -> SetPoint(itr, x, y * yscale);
		if ( (x > t1) && (x < t2) ) h_EoP_spread -> Fill(y * yscale);

		g_c_fit -> GetPoint(itr, x, y);
		g_c_fit -> SetPoint(itr, x, y * yscale);
		if ( (x > t1) && (x < t2) ) h_EoC_spread -> Fill(y * yscale);

		g_fit_run -> GetPoint(itr, x, y);
		g_fit_run -> SetPoint(itr, x, y * yscale);
		if ( (x > t1) && (x < t2) ) h_EoP_spread_run -> Fill(y * yscale);

		g_c_fit_run -> GetPoint(itr, x, y);
		g_c_fit_run -> SetPoint(itr, x, y * yscale);
		if ( (x > t1) && (x < t2) ) h_EoC_spread_run -> Fill(y * yscale);

		g_las -> GetPoint(itr, x, y);
		g_las -> SetPoint(itr, x, y * yscale * EoP_scale / LCInv_scale);
	}
	TF1 EoC_pol0("EoC_pol0", "pol0", t1, t2);
	EoC_pol0.SetLineColor(kGreen + 2);
	EoC_pol0.SetLineWidth(2);
	EoC_pol0.SetLineStyle(2);
	g_c_fit -> Fit("EoC_pol0", "QNR");








	//----------------------------
	// Print out global quantities

	std::cout << std::endl;
	std::cout << "***** Mean scales and errors *****" << std::endl;
	std::cout << std::fixed;
	std::cout << std::setprecision(4);
	std::cout << "Mean EoP scale: "  << std::setw(6) << EoP_scale   << "   mean EoP error: " << std::setw(8) << EoP_err << std::endl;
	std::cout << "Mean EoC scale: "  << std::setw(6) << EoC_scale   << "   mean EoC error: " << std::setw(8) << EoC_err << std::endl;
	std::cout << "Mean 1/LC scale: " << std::setw(6) << LCInv_scale << std::endl;






	//-----------------
	// Occupancy plots
	//-----------------

	TCanvas* c_scOccupancy = new TCanvas("c_scOccupancy", "SC occupancy", 0, 0, 1000, 500);
	c_scOccupancy -> Divide(2, 1);

	c_scOccupancy -> cd(1);
	h_scOccupancy_eta -> GetXaxis() -> SetTitle("sc #eta");
	h_scOccupancy_eta -> GetYaxis() -> SetTitle("events");
	h_scOccupancy_eta -> Draw();

	c_scOccupancy -> cd(2);
	h_scOccupancy_phi -> GetXaxis() -> SetTitle("sc #phi");
	h_scOccupancy_phi -> GetYaxis() -> SetTitle("events");
	h_scOccupancy_phi -> Draw();

	TCanvas* c_seedOccupancy = new TCanvas("c_seedOccupancy", "seed occupancy", 0, 0, 1500, 500);
	c_seedOccupancy -> Divide(3, 1);

	c_seedOccupancy -> cd(1);
	h_seedOccupancy_EB -> GetXaxis() -> SetTitle("seed i#eta");
	h_seedOccupancy_EB -> GetYaxis() -> SetTitle("seed i#phi");
	h_seedOccupancy_EB -> Draw("COLZ");

	c_seedOccupancy -> cd(2);
	h_seedOccupancy_EEp -> GetXaxis() -> SetTitle("seed ix");
	h_seedOccupancy_EEp -> GetYaxis() -> SetTitle("seed iy");
	h_seedOccupancy_EEp -> Draw("COLZ");

	c_seedOccupancy -> cd(3);
	h_seedOccupancy_EEm -> GetXaxis() -> SetTitle("seed ix");
	h_seedOccupancy_EEm -> GetYaxis() -> SetTitle("seed iy");
	h_seedOccupancy_EEm -> Draw("COLZ");



	//-----------
	// Chi2 plots
	//-----------

	TCanvas* c_chi2 = new TCanvas("c_chi2", "fit chi2", 0, 0, 500, 500);
	c_chi2 -> cd();

	h_EoC_chi2 -> GetXaxis() -> SetTitle("#chi^{2}/N_{dof}");
	h_EoC_chi2 -> Draw("");
	gPad -> Update();

	TPaveStats* s_EoC = new TPaveStats;
	s_EoC = (TPaveStats*)(h_EoC_chi2->GetListOfFunctions()->FindObject("stats"));
	s_EoC -> SetStatFormat("1.4g");
	s_EoC -> SetTextColor(kGreen + 2);
	s_EoC->SetY1NDC(0.59);
	s_EoC->SetY2NDC(0.79);
	s_EoC -> Draw("sames");
	gPad -> Update();

	h_EoP_chi2 -> GetXaxis() -> SetTitle("#chi^{2}/N_{dof}");
	h_EoP_chi2 -> Draw("sames");
	gPad -> Update();

	TPaveStats* s_EoP = new TPaveStats;
	s_EoP = (TPaveStats*)(h_EoP_chi2->GetListOfFunctions()->FindObject("stats"));
	s_EoP -> SetStatFormat("1.4g");
	s_EoP -> SetTextColor(kRed + 2);
	s_EoP->SetY1NDC(0.79);
	s_EoP->SetY2NDC(0.99);
	s_EoP -> Draw("sames");
	gPad -> Update();

	//ciao
	//-------------------
	// RMS vs Num evts -BARREL
	//-------------------
	double x[13] = {2., 4., 6., 8., 10., 12., 14., 16., 18., 20., 22., 24., 30.};
	double y[13] = {0.001049, 0.001114, 0.0009367, 0.0008480, 0.0007669, 0.0007892, 0.0006699, 0.0006473, 0.0006235, 0.0005903, 0.0005815, 0.0005459, 0.0005506};

	TCanvas* RMSeb = new TCanvas("plot", "plot", 0, 0, 500, 500);
	TGraph* gRMSeb = new TGraph(13, x, y);

	gRMSeb->Draw("APC");
	gRMSeb -> SetMarkerColor(38);
	gRMSeb -> SetLineColor(38);
	gRMSeb->GetXaxis()->SetTitle("Number of Events - Barrel");
	gRMSeb->GetYaxis()->SetTitle("RMS");

	RMSeb -> Print((folderName + "/" + folderName + "_RMSeb" + ".png").c_str(), "png");
	RMSeb -> Print((folderName + "/" + folderName + "_RMSeb" + ".pdf").c_str(), "pdf");

	//-------------------
	// RMS vs Num evts -ENDCAP
	//-------------------

	double xx[11] = {2., 4., 6., 8., 10., 12., 14., 16., 18., 20., 22.};
	double yy[11] = {0.007234, 0.005759, 0.004174, 0.004255, 0.003833, 0.004037, 0.003912, 0.004251, 0.003598, 0.004067, 0.004138};

	TCanvas* RMSee = new TCanvas("plot", "plot", 0, 0, 500, 500);
	TGraph* gRMSee = new TGraph(11, xx, yy);

	gRMSee->Draw("APC");
	gRMSee -> SetMarkerColor(38);
	gRMSee -> SetLineColor(38);
	gRMSee->GetXaxis()->SetTitle("Number of Events - Endcap");
	gRMSee->GetYaxis()->SetTitle("RMS");


	RMSee -> Print((folderName + "/" + folderName + "_RMSee" + ".png").c_str(), "png");
	RMSee -> Print((folderName + "/" + folderName + "_RMSee" + ".pdf").c_str(), "pdf");


	//ciao
	//-------------------
	// histos
	//-------------------


	/*
	for ( int i = 0; i < nBins; ++i)
	  {

	    TCanvas* histoEoP = new TCanvas("histo","histo",0,0,500,500);
	    histoEoP -> cd();

	    h_EoP[i] -> Draw();
	    f_EoP[i] -> SetLineWidth(2);
	    f_EoP[i] -> SetLineColor(4);
	    f_EoP[i] -> Draw("same");
	    // histoEoP -> Update();

	    histoEoP -> Print((folderName+"/"+folderName+"_histoEoP"+std::to_string(i)+".png").c_str(),"png");
	    histoEoP -> Print((folderName+"/"+folderName+"_histoEoP"+std::to_string(i)+".pdf").c_str(),"pdf");


	    TCanvas* histoEoC = new TCanvas("histo","histo",0,0,500,500);
	    histoEoC -> cd();

	    h_EoC[i] -> Draw();
	    f_EoC[i] -> SetLineWidth(2);
	    f_EoC[i] -> SetLineColor(4);
	    f_EoC[i] -> Draw("same");
	    //histoEoC -> Update();

	    histoEoC -> Print((folderName+"/"+folderName+"_histoEoC"+to_string(i)+".png").c_str(),"png");
	    histoEoC -> Print((folderName+"/"+folderName+"_histoEoC"+to_string(i)+".pdf").c_str(),"pdf");

	  }
	*/

	//-------------------
	// Final Plot vs date
	//-------------------

	TCanvas* cplot = new TCanvas("cplot", "history plot vs date", 100, 100, 1000, 500);
	cplot->cd();

	TPad *cLeft  = new TPad("pad_0", "pad_0", 0.00, 0.00, 0.75, 1.00);
	TPad *cRight = new TPad("pad_1", "pad_1", 0.75, 0.00, 1.00, 1.00);

	cLeft->SetLeftMargin(0.15);
	cLeft->SetRightMargin(0.025);
	cRight->SetLeftMargin(0.025);

	cLeft->Draw();
	cRight->Draw();

	float tYoffset = 1.0;
	float labSize = 0.05;
	float labSize2 = 0.06;

	cLeft->cd();

	cLeft->SetGridx();
	cLeft->SetGridy();

	TH1F *hPad = (TH1F*)gPad->DrawFrame(t1, 0.9, t2, 1.05);

	//hPad->GetXaxis()->SetLimits(t3,t2);
	hPad->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
	hPad->GetXaxis()->SetTimeDisplay(1);
	hPad->GetXaxis() -> SetRangeUser(MinTime[0] - 43200, MaxTime[nBins - 1] + 43200);
	hPad->GetXaxis()->SetTitle("date (day/month)");
	//ciao
	//hPad->GetXaxis()->SetLabelSize(0.025);

	if( strcmp(EBEE.c_str(), "EB") == 0 )
		hPad->GetYaxis()->SetTitle("Relative E/p scale");
	else
		hPad->GetYaxis()->SetTitle("Relative E/p scale");
	hPad->GetYaxis()->SetTitleOffset(tYoffset);
	hPad->GetXaxis()->SetTitleOffset(0.8);
	hPad->GetXaxis()->SetLabelSize(0.03);
	hPad->GetXaxis()->SetTitleSize(labSize2);
	hPad->GetYaxis()->SetLabelSize(labSize);
	hPad->GetYaxis()->SetTitleSize(labSize2);
	hPad -> SetMinimum(yMIN);
	hPad -> SetMaximum(yMAX);

	// draw history plot
	g_fit -> SetMarkerStyle(24);
	g_fit -> SetMarkerSize(0.7);
	g_fit -> SetMarkerColor(kRed + 2);
	g_fit -> SetLineColor(kRed + 2);
	g_fit -> Draw("P");
	g_c_fit -> SetMarkerStyle(20);
	g_c_fit -> SetMarkerColor(kGreen + 2);
	g_c_fit -> SetLineColor(kGreen + 2);
	g_c_fit -> SetMarkerSize(0.7);
	g_c_fit -> Draw("EP,same");
	g_las -> SetLineColor(kAzure - 2);
	g_las -> SetLineWidth(2);
	//  g_las -> Draw("L,same");

	TLegend* legend = new TLegend(0.60, 0.78, 0.90, 0.94);
	legend -> SetLineColor(kWhite);
	legend -> SetLineWidth(0);
	legend -> SetFillColor(kWhite);
	legend -> SetFillStyle(0);
	legend -> SetTextFont(42);
	legend -> SetTextSize(0.04);
	legend -> AddEntry(g_c_fit, "with LM correction", "PL");
	legend -> AddEntry(g_fit,  "without LM correction", "PL");
	//  legend -> AddEntry(g_las,  "1 / LM correction","L");
	legend -> Draw("same");

	char latexBuffer[250];

	sprintf(latexBuffer, "CMS 2016 Preliminary");
	TLatex* latex = new TLatex(0.18, 0.89, latexBuffer);
	latex -> SetNDC();
	latex -> SetTextFont(62);
	latex -> SetTextSize(0.05);
	latex -> Draw("same");

	//sprintf(latexBuffer,"#sqrt{s} = 8 TeV   L = 3.95 fb^{-1}");
	sprintf(latexBuffer, "#sqrt{s} = 13 TeV, L =%s fb^{-1} ", LUMI.c_str());

	// sprintf(latexBuffer, LUMI.c_str());
	TLatex* latex2 = new TLatex(0.18, 0.84, latexBuffer);
	latex2 -> SetNDC();
	latex2 -> SetTextFont(42);
	latex2 -> SetTextSize(0.05);
	latex2 -> Draw("same");

	if( strcmp(EBEE.c_str(), "EB") == 0 || strcmp(EBEE.c_str(), "EB_0_1") == 0 || strcmp(EBEE.c_str(), "EB_1_1479") == 0)
		sprintf(latexBuffer, "ECAL Barrel");
	else
		sprintf(latexBuffer, "ECAL Endcap");
	TLatex* latex3 = new TLatex(0.18, 0.19, latexBuffer);
	latex3 -> SetNDC();
	latex3 -> SetTextFont(42);
	latex3 -> SetTextSize(0.05);
	latex3 -> Draw("same");

	//sprintf(latexBuffer,"%.2E events",1.*nSavePts);
	//TLatex* latex4 = new TLatex(0.18,0.24,latexBuffer);
	//latex4 -> SetNDC();
	//latex4 -> SetTextFont(42);
	//latex4 -> SetTextSize(0.04);
	//latex4 -> Draw("same");
	//
	//sprintf(latexBuffer,"%d events/bin - %d bins",evtsPerPoint,nBins);
	//TLatex* latex5 = new TLatex(0.18,0.19,latexBuffer);
	//latex5 -> SetNDC();
	//latex5 -> SetTextFont(42);
	//latex5 -> SetTextSize(0.04);
	//latex5 -> Draw("same");


	cRight -> cd();

	TPaveStats* s_EoP_spread = new TPaveStats();
	TPaveStats* s_EoC_spread = new TPaveStats();


	h_EoC_spread -> SetFillStyle(3001);
	h_EoC_spread -> SetFillColor(kGreen + 2);
	h_EoC_spread->GetYaxis()->SetLabelSize(0.09);
	h_EoC_spread->GetYaxis()->SetLabelOffset(-0.03);
	h_EoC_spread->GetYaxis()->SetTitleSize(0.08);
	h_EoC_spread->GetYaxis()->SetNdivisions(505);
	h_EoC_spread->GetXaxis()->SetLabelOffset(1000);

	h_EoC_spread -> Draw("hbar");
	gPad -> Update();

	s_EoC_spread = (TPaveStats*)(h_EoC_spread->GetListOfFunctions()->FindObject("stats"));
	s_EoC_spread -> SetStatFormat("1.4g");
	s_EoC_spread->SetX1NDC(0.06); //new x start position
	s_EoC_spread->SetX2NDC(0.71); //new x end position
	s_EoC_spread->SetY1NDC(0.43); //new x start position
	s_EoC_spread->SetY2NDC(0.34); //new x end position
	s_EoC_spread -> SetOptStat(1100);
	s_EoC_spread ->SetTextColor(kGreen + 2);
	s_EoC_spread ->SetTextSize(0.08);
	s_EoC_spread -> Draw("sames");


	h_EoP_spread -> SetFillStyle(3001);
	h_EoP_spread -> SetFillColor(kRed + 2);
	h_EoP_spread -> Draw("hbarsames");
	gPad -> Update();
	s_EoP_spread = (TPaveStats*)(h_EoP_spread->GetListOfFunctions()->FindObject("stats"));
	s_EoP_spread -> SetStatFormat("1.4g");
	s_EoP_spread->SetX1NDC(0.06); //new x start position
	s_EoP_spread->SetX2NDC(0.71); //new x end position
	s_EoP_spread->SetY1NDC(0.33); //new x start position
	s_EoP_spread->SetY2NDC(0.24); //new x end position
	s_EoP_spread ->SetOptStat(1100);
	s_EoP_spread ->SetTextColor(kRed + 2);
	s_EoP_spread ->SetTextSize(0.08);
	s_EoP_spread -> Draw("sames");

	/*
	h_EoP_spread -> SetFillStyle(3001);
	h_EoP_spread -> SetFillColor(kRed+2);
	h_EoP_spread -> Draw("hbarsame");
	gPad -> Update();
	*/



	//------------------
	// Final plot vs run
	//------------------
	if (vsPU == 0) {

		TCanvas* cplot_run = new TCanvas("cplot_run", "history plot vs run", 100, 100, 1000, 500);
		cplot_run->cd();

		cLeft  = new TPad("pad_0_run", "pad_0_run", 0.00, 0.00, 0.75, 1.00);
		cRight = new TPad("pad_1_run", "pad_1_run", 0.75, 0.00, 1.00, 1.00);

		cLeft->SetLeftMargin(0.15);
		cLeft->SetRightMargin(0.025);
		cRight->SetLeftMargin(0.025);

		cLeft->Draw();
		cRight->Draw();

		tYoffset = 1.5;
		labSize = 0.04;
		labSize2 = 0.07;

		cLeft->cd();

		cLeft->SetGridx();
		cLeft->SetGridy();

		hPad = (TH1F*)gPad->DrawFrame(MinRun[0] - 1000, 0.9, MaxRun[nBins - 1] + 1000, 1.05);
		hPad->GetXaxis()->SetTitle("run");
		if( strcmp(EBEE.c_str(), "EB") == 0 )
			hPad->GetYaxis()->SetTitle("Relative E/p scale");
		else
			hPad->GetYaxis()->SetTitle("Relative E/p scale");
		hPad->GetYaxis()->SetTitleOffset(tYoffset);
		hPad->GetYaxis()->SetTitleOffset(0.8);
		hPad->GetXaxis()->SetLabelSize(labSize);
		hPad->GetXaxis()->SetTitleSize(labSize);
		hPad->GetYaxis()->SetLabelSize(labSize);
		hPad->GetYaxis()->SetTitleSize(labSize);
		hPad -> SetMinimum(yMIN);
		hPad -> SetMaximum(yMAX);

		// draw history plot
		g_fit_run -> SetMarkerStyle(20);
		g_fit_run -> SetMarkerSize(0.7);
		g_fit_run -> SetMarkerColor(kRed + 2);
		g_fit_run -> SetLineColor(kRed + 2);
		g_fit_run -> Draw("P");
		g_c_fit_run -> SetMarkerStyle(20);
		g_c_fit_run -> SetMarkerColor(kGreen + 2);
		g_c_fit_run -> SetLineColor(kGreen + 2);
		g_c_fit_run -> SetMarkerSize(0.7);
		g_c_fit_run -> Draw("P,same");


		cRight -> cd();

		s_EoP_spread = new TPaveStats();
		s_EoC_spread = new TPaveStats();


		h_EoC_spread_run -> SetFillStyle(3001);
		h_EoC_spread_run -> SetFillColor(kGreen + 2);
		h_EoC_spread_run->GetYaxis()->SetLabelSize(labSize2);
		h_EoC_spread_run->GetYaxis()->SetTitleSize(labSize2);
		h_EoC_spread_run->GetYaxis()->SetNdivisions(505);
		h_EoC_spread_run->GetYaxis()->SetLabelOffset(-0.02);
		h_EoC_spread_run->GetXaxis()->SetLabelOffset(1000);

		h_EoC_spread_run -> Draw("hbar");
		gPad -> Update();

		s_EoC_spread = (TPaveStats*)(h_EoC_spread_run->GetListOfFunctions()->FindObject("stats"));
		s_EoC_spread ->SetTextColor(kGreen + 2);
		s_EoC_spread ->SetTextSize(0.06);
		s_EoC_spread->SetX1NDC(0.49); //new x start position

		s_EoC_spread->SetX2NDC(0.99); //new x end position
		s_EoC_spread->SetY1NDC(0.475); //new x start position
		s_EoC_spread->SetY2NDC(0.590); //new x end position
		s_EoC_spread -> SetOptStat(1100);
		s_EoC_spread -> Draw("sames");

		h_EoP_spread_run -> SetFillStyle(3001);
		h_EoP_spread_run -> SetFillColor(kRed + 2);
		h_EoP_spread_run -> Draw("hbarsames");
		gPad -> Update();

		s_EoP_spread = (TPaveStats*)(h_EoP_spread_run->GetListOfFunctions()->FindObject("stats"));
		s_EoP_spread->SetX1NDC(0.49); //new x start position
		s_EoP_spread->SetX2NDC(0.99); //new x end position
		s_EoP_spread->SetY1NDC(0.350); //new x start position
		s_EoP_spread->SetY2NDC(0.475); //new x end position
		s_EoP_spread ->SetOptStat(1100);
		s_EoP_spread ->SetTextColor(kRed + 2);
		s_EoP_spread ->SetTextSize(0.06);
		s_EoP_spread -> Draw("sames");




		c_chi2 -> Print((folderName + "/" + folderName + "_fitChi2.png").c_str(), "png");
		c_scOccupancy -> Print((folderName + "/" + folderName + "_scOccupancy.png").c_str(), "png");
		c_seedOccupancy -> Print((folderName + "/" + folderName + "_seedOccupancy.png").c_str(), "png");
		cplot -> Print((folderName + "/" + folderName + "_history_vsTime.png").c_str(), "png");
		cplot_run -> Print((folderName + "/" + folderName + "_history_vsRun.png").c_str(), "png");

		c_chi2 -> Print((folderName + "/" + folderName + "_fitChi2.pdf").c_str(), "pdf");
		c_scOccupancy -> Print((folderName + "/" + folderName + "_scOccupancy.pdf").c_str(), "pdf");
		c_seedOccupancy -> Print((folderName + "/" + folderName + "_seedOccupancy.pdf").c_str(), "pdf");
		cplot -> Print((folderName + "/" + folderName + "_history_vsTime.pdf").c_str(), "pdf");
		cplot_run -> Print((folderName + "/" + folderName + "_history_vsRun.pdf").c_str(), "pdf");

		cplot -> SaveAs((folderName + "/" + folderName + "_history_vsTime.C").c_str());
		cplot_run -> SaveAs((folderName + "/" + folderName + "_history_vsRun.C").c_str());

		cplot -> SaveAs((folderName + "/" + folderName + "_history_vsTime.root").c_str());
		cplot_run -> SaveAs((folderName + "/" + folderName + "_history_vsRun.root").c_str());

	}


	else {

		/////////////////////////PLOT
		TCanvas* cplot = new TCanvas("cplot", "history plot vs Vertex", 100, 100, 1000, 500);
		cplot->cd();

		TPad *cLeft  = new TPad("pad_0", "pad_0", 0.00, 0.00, 0.75, 1.00);
		TPad *cRight = new TPad("pad_1", "pad_1", 0.75, 0.00, 1.00, 1.00);

		cLeft->SetLeftMargin(0.15);
		cLeft->SetRightMargin(0.025);
		cRight->SetLeftMargin(0.025);

		cLeft->Draw();
		cRight->Draw();

		float tYoffset = 1.0;
		float labSize = 0.05;
		float labSize2 = 0.06;

		cLeft->cd();

		cLeft->SetGridx();
		cLeft->SetGridy();

		TH1F *hPad = (TH1F*)gPad->DrawFrame(t1, 0.9, t2, 1.05);

		hPad->GetXaxis()->SetLimits(0, 46);
		//hPad->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
		//hPad->GetXaxis()->SetTimeDisplay(1);
		//hPad->GetXaxis() -> SetRangeUser(MinTime[0]-43200,MaxTime[nBins-1]+43200);
		hPad->GetXaxis()->SetTitle(" Number of Vertices");
		hPad->GetXaxis()->SetTitleOffset(0.8);

		//ciao


		if( strcmp(EBEE.c_str(), "EB") == 0 )
			hPad->GetYaxis()->SetTitle("Relative E/p scale");
		else
			hPad->GetYaxis()->SetTitle("Relative E/p scale");
		hPad->GetYaxis()->SetTitleOffset(tYoffset);
		hPad->GetXaxis()->SetLabelSize(labSize);
		hPad->GetXaxis()->SetTitleSize(labSize2);
		hPad->GetYaxis()->SetLabelSize(labSize);
		hPad->GetYaxis()->SetTitleSize(labSize2);
		hPad -> SetMinimum(yMIN);
		hPad -> SetMaximum(yMAX);

		// draw history plot
		g_fit -> SetMarkerStyle(24);
		g_fit -> SetMarkerSize(0.7);
		g_fit -> SetMarkerColor(kRed + 2);
		g_fit -> SetLineColor(kRed + 2);
		//g_fit -> Draw("P");
		g_c_fit -> SetMarkerStyle(20);
		g_c_fit -> SetMarkerColor(kGreen + 2);
		g_c_fit -> SetLineColor(kGreen + 2);
		g_c_fit -> SetMarkerSize(0.7);
		g_c_fit -> Draw("EP");
		g_las -> SetLineColor(kAzure - 2);
		g_las -> SetLineWidth(2);
		//g_las -> Draw("L,same");

		TLegend* legend = new TLegend(0.60, 0.78, 0.90, 0.94);
		legend -> SetLineColor(kWhite);
		legend -> SetLineWidth(0);
		legend -> SetFillColor(kWhite);
		legend -> SetFillStyle(0);
		legend -> SetTextFont(42);
		legend -> SetTextSize(0.04);
		legend -> AddEntry(g_c_fit, "with LM correction", "PL");
		//legend -> AddEntry(g_fit,  "without LM correction","PL");
		//legend -> AddEntry(g_las,  "1 / LM correction","L");
		legend -> Draw("same");

		char latexBuffer[250];

		sprintf(latexBuffer, "CMS 2015 Preliminary");
		TLatex* latex = new TLatex(0.18, 0.89, latexBuffer);
		latex -> SetNDC();
		latex -> SetTextFont(62);
		latex -> SetTextSize(0.05);
		latex -> Draw("same");

		//sprintf(latexBuffer,"#sqrt{s} = 8 TeV   L = 3.95 fb^{-1}");
		sprintf(latexBuffer, "#sqrt{s} = 13 TeV");
		TLatex* latex2 = new TLatex(0.18, 0.84, latexBuffer);
		latex2 -> SetNDC();
		latex2 -> SetTextFont(42);
		latex2 -> SetTextSize(0.05);
		latex2 -> Draw("same");

		if( strcmp(EBEE.c_str(), "EB") == 0 || strcmp(EBEE.c_str(), "EB_0_1") == 0 || strcmp(EBEE.c_str(), "EB_1_1479") == 0)
			sprintf(latexBuffer, "ECAL Barrel");
		else
			sprintf(latexBuffer, "ECAL Endcap");
		TLatex* latex3 = new TLatex(0.18, 0.19, latexBuffer);
		latex3 -> SetNDC();
		latex3 -> SetTextFont(42);
		latex3 -> SetTextSize(0.05);
		latex3 -> Draw("same");

		//sprintf(latexBuffer,"%.2E events",1.*nSavePts);
		//TLatex* latex4 = new TLatex(0.18,0.24,latexBuffer);
		//latex4 -> SetNDC();
		//latex4 -> SetTextFont(42);
		//latex4 -> SetTextSize(0.04);
		//latex4 -> Draw("same");
		//
		//sprintf(latexBuffer,"%d events/bin - %d bins",evtsPerPoint,nBins);
		//TLatex* latex5 = new TLatex(0.18,0.19,latexBuffer);
		//latex5 -> SetNDC();
		//latex5 -> SetTextFont(42);
		//latex5 -> SetTextSize(0.04);
		//latex5 -> Draw("same");


		cRight -> cd();

		TPaveStats* s_EoP_spread = new TPaveStats();
		TPaveStats* s_EoC_spread = new TPaveStats();


		h_EoC_spread -> SetFillStyle(3001);
		h_EoC_spread -> SetFillColor(kGreen + 2);
		h_EoC_spread->GetYaxis()->SetLabelSize(0.09);
		h_EoC_spread->GetYaxis()->SetLabelOffset(-0.03);
		h_EoC_spread->GetYaxis()->SetTitleSize(0.08);
		h_EoC_spread->GetYaxis()->SetNdivisions(505);
		h_EoC_spread->GetXaxis()->SetLabelOffset(1000);

		h_EoC_spread -> Draw("hbar");
		gPad -> Update();

		s_EoC_spread = (TPaveStats*)(h_EoC_spread->GetListOfFunctions()->FindObject("stats"));
		s_EoC_spread -> SetStatFormat("1.4g");
		s_EoC_spread->SetX1NDC(0.06); //new x start position
		s_EoC_spread->SetX2NDC(0.71); //new x end position
		s_EoC_spread->SetY1NDC(0.43); //new x start position
		s_EoC_spread->SetY2NDC(0.34); //new x end position
		s_EoC_spread -> SetOptStat(1100);
		s_EoC_spread ->SetTextColor(kGreen + 2);
		s_EoC_spread ->SetTextSize(0.08);
		s_EoC_spread -> Draw("sames");


		h_EoP_spread -> SetFillStyle(3001);
		h_EoP_spread -> SetFillColor(kRed + 2);
		h_EoP_spread -> Draw("hbarsames");
		gPad -> Update();
		s_EoP_spread = (TPaveStats*)(h_EoP_spread->GetListOfFunctions()->FindObject("stats"));
		s_EoP_spread -> SetStatFormat("1.4g");
		s_EoP_spread->SetX1NDC(0.06); //new x start position
		s_EoP_spread->SetX2NDC(0.71); //new x end position
		s_EoP_spread->SetY1NDC(0.33); //new x start position
		s_EoP_spread->SetY2NDC(0.24); //new x end position
		s_EoP_spread ->SetOptStat(1100);
		s_EoP_spread ->SetTextColor(kRed + 2);
		s_EoP_spread ->SetTextSize(0.08);
		s_EoP_spread -> Draw("sames");

		/*
		h_EoP_spread -> SetFillStyle(3001);
		h_EoP_spread -> SetFillColor(kRed+2);
		h_EoP_spread -> Draw("hbarsame");
		gPad -> Update();
		*/





		cplot -> Print((folderName + "/" + folderName + "_history_vsVertex.png").c_str(), "png");

		cplot -> Print((folderName + "/" + folderName + "_history_vsVertex.pdf").c_str(), "pdf");

		cplot -> SaveAs((folderName + "/" + folderName + "_history_vsVertex.C").c_str());

	}





	///////////////////////////

	o -> cd();

	h_template -> Write();

	h_scOccupancy_eta   -> Write();
	h_scOccupancy_phi   -> Write();
	h_seedOccupancy_EB  -> Write();
	h_seedOccupancy_EEp -> Write();
	h_seedOccupancy_EEm -> Write();

	g_fit   -> Write("g_fit");
	g_c_fit -> Write("g_c_fit");
	g_fit_run   -> Write("g_fit_run");
	g_c_fit_run -> Write("g_c_fit_run");
	g_las -> Write("g_las");
	g_LT -> Write("g_LT");

	h_EoP_chi2 -> Write();
	h_EoC_chi2 -> Write();

	//ciao

	for(int i = 0; i < nBins; ++i) {
		gStyle->SetOptFit(1111);

		h_EoP[i] -> Write();
		h_EoC[i] -> Write();
		f_EoP[i] -> Write();
		f_EoC[i] -> Write();
		//  h_Tsp[i] -> Write();
		//
		//  h_Cvl[i] -> Write();
	}

	o -> Close();

}


void LaserMonitoringEoP::setLaserPlotStyle()
{
	TStyle* laserPlotStyle = new TStyle("laserPlotStyle", "Style for P-TDR");

	// For the canvas:
	laserPlotStyle->SetCanvasBorderMode(0);
	laserPlotStyle->SetCanvasColor(kWhite);
	laserPlotStyle->SetCanvasDefH(600); //Height of canvas
	laserPlotStyle->SetCanvasDefW(700); //Width of canvas
	//laserPlotStyle->SetCanvasDefW(800); //Width of canvas
	laserPlotStyle->SetCanvasDefX(0);   //POsition on screen
	laserPlotStyle->SetCanvasDefY(0);

	// For the Pad:
	laserPlotStyle->SetPadBorderMode(0);
	// laserPlotStyle->SetPadBorderSize(Width_t size = 1);
	laserPlotStyle->SetPadColor(kWhite);
	laserPlotStyle->SetPadGridX(false);
	laserPlotStyle->SetPadGridY(false);
	laserPlotStyle->SetGridColor(0);
	laserPlotStyle->SetGridStyle(3);
	laserPlotStyle->SetGridWidth(1);

	// For the frame:
	laserPlotStyle->SetFrameBorderMode(0);
	laserPlotStyle->SetFrameBorderSize(1);
	laserPlotStyle->SetFrameFillColor(0);
	laserPlotStyle->SetFrameFillStyle(0);
	laserPlotStyle->SetFrameLineColor(1);
	laserPlotStyle->SetFrameLineStyle(1);
	laserPlotStyle->SetFrameLineWidth(1);

	// For the histo:
	// laserPlotStyle->SetHistFillColor(1);
	// laserPlotStyle->SetHistFillStyle(0);
	laserPlotStyle->SetHistLineColor(1);
	laserPlotStyle->SetHistLineStyle(0);
	laserPlotStyle->SetHistLineWidth(1);
	// laserPlotStyle->SetLegoInnerR(Float_t rad = 0.5);
	// laserPlotStyle->SetNumberContours(Int_t number = 20);

	laserPlotStyle->SetEndErrorSize(2);
	//  laserPlotStyle->SetErrorMarker(20);
	//laserPlotStyle->SetErrorX(0.);

	laserPlotStyle->SetMarkerStyle(20);

	//For the fit/function:
	laserPlotStyle->SetOptFit(0);
	laserPlotStyle->SetFitFormat("5.4g");
	laserPlotStyle->SetFuncColor(2);
	laserPlotStyle->SetFuncStyle(1);
	laserPlotStyle->SetFuncWidth(1);

	//For the date:
	laserPlotStyle->SetOptDate(0);
	// laserPlotStyle->SetDateX(Float_t x = 0.01);
	// laserPlotStyle->SetDateY(Float_t y = 0.01);

	// For the statistics box:
	laserPlotStyle->SetOptFile(0);
	laserPlotStyle->SetOptStat(0); // To display the mean and RMS:   SetOptStat("mr");
	laserPlotStyle->SetStatColor(kWhite);
	laserPlotStyle->SetStatFont(42);
	laserPlotStyle->SetStatFontSize(0.025);
	laserPlotStyle->SetStatTextColor(1);
	laserPlotStyle->SetStatFormat("6.4g");
	laserPlotStyle->SetStatBorderSize(1);
	laserPlotStyle->SetStatH(0.1);
	laserPlotStyle->SetStatW(0.15);
	// laserPlotStyle->SetStatStyle(Style_t style = 1001);
	// laserPlotStyle->SetStatX(Float_t x = 0);
	// laserPlotStyle->SetStatY(Float_t y = 0);

	// Margins:
	laserPlotStyle->SetPadTopMargin(0.05);
	laserPlotStyle->SetPadBottomMargin(0.13);
	laserPlotStyle->SetPadLeftMargin(0.13);
	laserPlotStyle->SetPadRightMargin(0.17);
	//laserPlotStyle->SetPadRightMargin(0.25);

	// For the Global title:
	laserPlotStyle->SetOptTitle(0);
	laserPlotStyle->SetTitleFont(42);
	laserPlotStyle->SetTitleColor(1);
	laserPlotStyle->SetTitleTextColor(1);
	laserPlotStyle->SetTitleFillColor(10);
	laserPlotStyle->SetTitleFontSize(0.05);
	// laserPlotStyle->SetTitleH(0); // Set the height of the title box
	// laserPlotStyle->SetTitleW(0); // Set the width of the title box
	// laserPlotStyle->SetTitleX(0); // Set the position of the title box
	// laserPlotStyle->SetTitleY(0.985); // Set the position of the title box
	// laserPlotStyle->SetTitleStyle(Style_t style = 1001);
	// laserPlotStyle->SetTitleBorderSize(2);

	// For the axis titles:
	laserPlotStyle->SetTitleColor(1, "XYZ");
	laserPlotStyle->SetTitleFont(42, "XYZ");
	laserPlotStyle->SetTitleSize(0.05, "XYZ");
	// laserPlotStyle->SetTitleXSize(Float_t size = 0.02); // Another way to set the size?
	// laserPlotStyle->SetTitleYSize(Float_t size = 0.02);
	laserPlotStyle->SetTitleXOffset(1.20);
	laserPlotStyle->SetTitleYOffset(1.40);
	laserPlotStyle->SetTitleOffset(1.30, "Z");
	// laserPlotStyle->SetTitleOffset(1.1, "Y"); // Another way to set the Offset

	// For the axis labels:
	laserPlotStyle->SetLabelColor(1, "XYZ");
	laserPlotStyle->SetLabelFont(42, "XYZ");
	laserPlotStyle->SetLabelOffset(0.005, "XYZ");
	laserPlotStyle->SetLabelSize(0.04, "XYZ");

	// For the axis:
	laserPlotStyle->SetAxisColor(1, "XYZ");
	laserPlotStyle->SetStripDecimals(kTRUE);
	laserPlotStyle->SetTickLength(0.03, "XYZ");
	laserPlotStyle->SetNdivisions(510, "XYZ");
	laserPlotStyle->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
	laserPlotStyle->SetPadTickY(1);

	// Change for log plots:
	laserPlotStyle->SetOptLogx(0);
	laserPlotStyle->SetOptLogy(0);
	laserPlotStyle->SetOptLogz(0);

	// Postscript options:
	laserPlotStyle->SetPaperSize(20., 20.);
	// laserPlotStyle->SetLineScalePS(Float_t scale = 3);
	// laserPlotStyle->SetLineStyleString(Int_t i, const char* text);
	// laserPlotStyle->SetHeaderPS(const char* header);
	// laserPlotStyle->SetTitlePS(const char* pstitle);

	//laserPlotStyle->SetBarOffset(Float_t baroff = 0.5);
	//laserPlotStyle->SetBarWidth(Float_t barwidth = 0.5);
	//laserPlotStyle->SetPaintTextFormat(const char* format = "g");
	laserPlotStyle->SetPalette(1);
	//laserPlotStyle->SetTimeOffset(Double_t toffset);
	//laserPlotStyle->SetHistMinimumZero(kTRUE);



	const Int_t NRGBs = 5;
	const Int_t NCont = 255;

	Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
	Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
	Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
	Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
	TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
	TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
	laserPlotStyle->SetNumberContours(NCont);

	//TLatex *lab = new TLatex(0.70,0.85, "CMS 2008");
	//lab->SetNDC();
	//lab->SetTextFont(42);
	//lab->SetTextSize(0.05);
	//lab->Draw("same");



	gROOT -> ForceStyle();

	laserPlotStyle->cd();
}
