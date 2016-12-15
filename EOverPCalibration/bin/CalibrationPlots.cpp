/// Standalone program to mormalize IC EB by the mean on a eta ring + skipping xtal near dead channels and TT
/// in the normalization procedure
/// Folded Plots for Spread IC, Statistical Precision and spread
/// Correct IC near cracks and for momentum scale and produce txt IC values

#include <vector>
#include <utility>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "TH2F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TF1.h"
#include "TLegend.h"
#include "TGraphErrors.h"
#include "TApplication.h"
#include "TLatex.h"

#include "../interface/TEndcapRings.h"
#include "../interface/ntpleUtils.h"
#include "../interface/CalibrationUtils.h"
#include "../interface/DrawingUtils.h"
#include "../../ZFitter/interface/setTDRStyle.h"

#include "FWCore/ParameterSet/interface/ProcessDesc.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"


int main(int argc, char **argv)
{
	//------------------
	// Set style options

	setTDRStyle();
	gStyle->SetPadTopMargin(0.05);
	gStyle->SetPadBottomMargin(0.13);
	gStyle->SetPadLeftMargin(0.13);
	gStyle->SetPadRightMargin(0.17);
	gStyle->SetLabelSize(0.04, "XYZ");
	gStyle->SetTitleSize(0.05, "XYZ");
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(0);



	//----------------
	// parse  cfg file

	if( argc != 2 ) {
		std::cerr << ">>>>> Calibration.cpp::usage: " << argv[0] << " configFileName" << std::endl;
		return 1;
	}

	std::string configFileName = argv[1];
	std::shared_ptr<edm::ParameterSet> parameterSet = edm::readConfig(configFileName);
	edm::ParameterSet Options = parameterSet -> getParameter<edm::ParameterSet>("Options");


	std::string inFileName = "NULL";
	if(Options.existsAs<std::string>("inFileName"))
		inFileName = Options.getParameter<std::string>("inFileName");
	else {
		std::cout << " Exit from code, no input file" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	bool is2012Calib = true;
	if(Options.existsAs<bool>("is2012Calib"))
		is2012Calib = Options.getParameter<bool>("is2012Calib");

	bool isEB = true;
	if(Options.existsAs<bool>("isEB"))
		isEB = Options.getParameter<bool>("isEB");

	std::string outputFolder = "output/";
	if(Options.existsAs<std::string>("outputFolder"))
		outputFolder = Options.getParameter<std::string>("outputFolder");

	std::string outputTxt = "IC_Run2012ABC_22JanuaryRereco_WZ_Fbrem_EB";
	if(Options.existsAs<std::string>("outputTxt"))
		outputTxt = Options.getParameter<std::string>("outputTxt");

	std::string outFileName = "calibrationEB_Run2012ABC_22JanuaryRereco_WZ_Fbrem.root";
	if(Options.existsAs<std::string>("outFileName"))
		outFileName = Options.getParameter<std::string>("outFileName");

	std::string fileType = "cxx";
	if(Options.existsAs<std::string>("fileType"))
		fileType = Options.getParameter<std::string>("fileType");

	int evalStat = true;
	if(Options.existsAs<int>("evalStat"))
		evalStat = Options.getParameter<int>("evalStat");

	std::string inFileNameEven = "NULL", inFileNameOdd = "NULL";
	if( evalStat ) {
		if(Options.existsAs<std::string>("inFileNameEven"))
			inFileNameEven = Options.getParameter<std::string>("inFileNameEven");
		else {
			std::cout << " Exit from code, no even input file" << std::endl;
			std::exit(EXIT_FAILURE);
		}

		if(Options.existsAs<std::string>("inFileNameOdd"))
			inFileNameOdd = Options.getParameter<std::string>("inFileNameOdd");
		else {
			std::cout << " Exit from code, no odd input file" << std::endl;
			std::exit(EXIT_FAILURE);
		}
	}


	TFile* f4 = new TFile("momentumCalibration2015_EE_ptK.root");
	std::vector<std::vector<TGraphErrors*> > corrMomentum(1);

	for (int k = 0; k < 1; k++) {
		for(int i = 0; i < 2; ++i) {
			std::cout << k << " " << i << std::endl;
			TString Name = Form("g_pData_EE_%d_%d", k, i);
			(corrMomentum.at(k)).push_back( (TGraphErrors*)(f4->Get(Name)) );
		}
	}


	//------------
	// check files

	if( inFileName.empty() ) {
		std::cout << ">>> No input file specified!" << std::endl;
		return 1;
	}
	std::cout << ">>> Making calibration plots for: " << inFileName << std::endl;

	if( evalStat ) {
		if( inFileNameEven.empty() || inFileNameOdd.empty() ) {
			std::cout << ">>> No input files to evaluate statistical precision specified!" << std::endl;
			return 1;
		}
		std::cout << ">>> Evaluating statistical precision from: " << inFileNameEven << std::endl;
		std::cout << ">>> Evaluating statistical precision from: " << inFileNameOdd << std::endl;
	}



	//---------------------
	// endcap ring geometry

	TEndcapRings* eRings = new TEndcapRings();
	TFile* exisistingEEFile = new TFile("/afs/cern.ch/user/l/lbrianza/work/public/CommonTools/existingEE.root", "READ");
	TH2F* existingEE = (TH2F*)( exisistingEEFile->Get("endcap") );

	TFile* MCSystematicFile = new TFile("/afs/cern.ch/user/l/lbrianza/work/public/CommonTools/ResidualForSystematic_EE_MC.root", "READ");
	//TGraphErrors* MCSystematic_EEM = (TGraphErrors*)( MCSystematicFile->Get("residual_EEM") );
	TGraphErrors* MCSystematic_EE  = (TGraphErrors*)( MCSystematicFile->Get("residual_All") );
	//TGraphErrors* MCSystematic_EEP = (TGraphErrors*)( MCSystematicFile->Get("residual_EEP") );



	//-----------------------
	// map for dead TT centre

	std::cout << "debug" << std::endl;

	std::map<int, std::vector< std::pair<int, int> > > TT_centre;
	if( isEB == true ) {
		std::vector< std::pair<int, int> > TT_centre_EB;

		if( !is2012Calib ) InitializeDeadTT_EB(TT_centre_EB);
		if(  is2012Calib )  InitializeDeadTT_EB2012(TT_centre_EB);

		TT_centre[0] = TT_centre_EB;
	} else {
		std::vector< std::pair<int, int> > TT_centre_EEM;
		std::vector< std::pair<int, int> > TT_centre_EEP;

		if( !is2012Calib ) InitializeDeadTTEEM(TT_centre_EEM);
		if(  is2012Calib ) InitializeDeadTTEEM2012(TT_centre_EEM);

		if( !is2012Calib ) InitializeDeadTTEEP(TT_centre_EEP);
		if(  is2012Calib ) InitializeDeadTTEEP2012(TT_centre_EEP);

		TT_centre[-1] = TT_centre_EEM;
		TT_centre[1]  = TT_centre_EEP;
	}


	std::cout << "debug" << std::endl;

	//---------------
	// define outfile

	TCanvas* c1 = new TCanvas();
	c1 -> cd();

	TFile* outFile = new TFile((outputFolder + "/" + outFileName).c_str(), "RECREATE");




	std::cout << "debug" << std::endl;


	//--------------------------------------------------------------
	// Build the precision vs ieta plot starting from the TH2F of IC
	//--------------------------------------------------------------



	// open files
	TFile* inFile = new TFile(inFileName.c_str(), "READ");
	TFile* inFileEven = new TFile();
	TFile* inFileOdd = new TFile();
	if( evalStat ) {
		inFileEven = new TFile(inFileNameEven.c_str(), "READ");
		inFileOdd  = new TFile(inFileNameOdd.c_str(), "READ");
	}

	std::cout << "debug" << std::endl;

	std::map<int, TH2F*> h2_corrP;
	std::map<int, TH2F*> h2_IC_corr;

	// get the IC maps as they come from the algorithm
	std::map<int, TH2F*> h2_IC_raw;
	std::map<int, TH2F*> h2_IC_raw_phiNorm;
	std::map<int, TH2F*> h2_IC_crackCorr;
	std::map<int, TH2F*> h2_IC_crackCorr_phiNorm;

	std::map<int, TH2F*> h2_ICEven_raw;
	std::map<int, TH2F*> h2_IC_raw_phiNorm_even;
	std::map<int, TH2F*> h2_IC_crackCorr_phiNorm_even;

	std::map<int, TH2F*> h2_ICOdd_raw;
	std::map<int, TH2F*> h2_IC_raw_phiNorm_odd;
	std::map<int, TH2F*> h2_IC_crackCorr_phiNorm_odd;

	if( isEB == true ) {
		h2_IC_raw[0] = (TH2F*)( inFile->Get("h_scale_EB") );

		h2_IC_raw_phiNorm[0] = (TH2F*)( h2_IC_raw[0]->Clone("h2_IC_raw_phiNorm_EB") );
		h2_IC_raw_phiNorm[0] -> Reset("ICEMS");
		h2_IC_raw_phiNorm[0] -> ResetStats();

		h2_IC_crackCorr[0] = (TH2F*)( h2_IC_raw[0]->Clone("h2_IC_crackCorr_EB") );
		h2_IC_crackCorr[0] -> Reset("ICEMS");
		h2_IC_crackCorr[0] -> ResetStats();

		h2_IC_crackCorr_phiNorm[0] = (TH2F*)( h2_IC_raw[0]->Clone("h2_IC_crackCorr_phiNorm_EB") );
		h2_IC_crackCorr_phiNorm[0] -> Reset("ICEMS");
		h2_IC_crackCorr_phiNorm[0] -> ResetStats();

		if( evalStat ) {
			h2_ICEven_raw[0] = (TH2F*)( inFileEven->Get("h_scale_EB") );
			h2_ICOdd_raw[0]  = (TH2F*)( inFileOdd ->Get("h_scale_EB") );

			h2_IC_raw_phiNorm_even[0] = (TH2F*)( h2_ICEven_raw[0]->Clone("h2_IC_raw_phiNorm_even_EB") );
			h2_IC_raw_phiNorm_even[0] -> Reset("ICEMS");
			h2_IC_raw_phiNorm_even[0] -> ResetStats();

			h2_IC_raw_phiNorm_odd[0] = (TH2F*)( h2_ICOdd_raw[0]->Clone("h2_IC_raw_phiNorm_odd_EB") );
			h2_IC_raw_phiNorm_odd[0] -> Reset("ICEMS");
			h2_IC_raw_phiNorm_odd[0] -> ResetStats();

			h2_IC_crackCorr_phiNorm_even[0] = (TH2F*)( h2_ICEven_raw[0]->Clone("h2_IC_crackCorr_phiNorm_even_EB") );
			h2_IC_crackCorr_phiNorm_even[0] -> Reset("ICEMS");
			h2_IC_crackCorr_phiNorm_even[0] -> ResetStats();

			h2_IC_crackCorr_phiNorm_odd[0] = (TH2F*)( h2_ICOdd_raw[0]->Clone("h2_IC_crackCorr_phiNorm_odd_EB") );
			h2_IC_crackCorr_phiNorm_odd[0] -> Reset("ICEMS");
			h2_IC_crackCorr_phiNorm_odd[0] -> ResetStats();
		}
	} else {
		std::cout << "debug" << std::endl;
		h2_IC_raw[-1] = (TH2F*)( inFile->Get("h_scale_EEM") );
		h2_IC_raw[1]  = (TH2F*)( inFile->Get("h_scale_EEP") );
		std::cout << "debug" << std::endl;

		h2_IC_raw_phiNorm[-1] = (TH2F*)( h2_IC_raw[-1]->Clone("h2_IC_raw_phiNorm_EEM") );
		h2_IC_raw_phiNorm[1]  = (TH2F*)( h2_IC_raw[1] ->Clone("h2_IC_raw_phiNorm_EEP") );
		h2_IC_raw_phiNorm[-1] -> Reset("ICEMS");
		h2_IC_raw_phiNorm[1]  -> Reset("ICEMS");
		h2_IC_raw_phiNorm[-1] -> ResetStats();
		h2_IC_raw_phiNorm[1]  -> ResetStats();

		h2_corrP[-1] = (TH2F*)( h2_IC_raw[-1]->Clone("h2_corrP_EEM") );
		h2_corrP[1]  = (TH2F*)( h2_IC_raw[1] ->Clone("h2_corrP_EEP") );
		h2_corrP[-1] -> Reset("ICEMS");
		h2_corrP[1]  -> Reset("ICEMS");
		h2_corrP[-1] -> ResetStats();
		h2_corrP[1]  -> ResetStats();

		h2_IC_corr[-1] = (TH2F*)( h2_IC_raw[-1]->Clone("h2_IC_corr_EEM") );
		h2_IC_corr[1]  = (TH2F*)( h2_IC_raw[1] ->Clone("h2_IC_corr_EEP") );
		h2_IC_corr[-1] -> Reset("ICEMS");
		h2_IC_corr[1]  -> Reset("ICEMS");
		h2_IC_corr[-1] -> ResetStats();
		h2_IC_corr[1]  -> ResetStats();


		std::cout << "debug" << std::endl;
		if( evalStat ) {
			std::cout << "debug" << std::endl;
			h2_ICEven_raw[-1] = (TH2F*)( inFileEven->Get("h_scale_EEM") );
			h2_ICEven_raw[1]  = (TH2F*)( inFileEven->Get("h_scale_EEP") );
			h2_ICOdd_raw[-1 ] = (TH2F*)( inFileOdd ->Get("h_scale_EEM") );
			h2_ICOdd_raw[1]   = (TH2F*)( inFileOdd ->Get("h_scale_EEP") );
			std::cout << "debug" << std::endl;

			h2_IC_raw_phiNorm_even[-1] = (TH2F*)( h2_ICEven_raw[-1]->Clone("h2_IC_raw_phiNorm_even_EEM") );
			h2_IC_raw_phiNorm_even[1]  = (TH2F*)( h2_ICEven_raw[1] ->Clone("h2_IC_raw_phiNorm_even_EEP") );
			h2_IC_raw_phiNorm_even[-1] -> Reset("ICEMS");
			h2_IC_raw_phiNorm_even[1]  -> Reset("ICEMS");
			h2_IC_raw_phiNorm_even[-1] -> ResetStats();
			h2_IC_raw_phiNorm_even[1]  -> ResetStats();
			std::cout << "debug" << std::endl;

			h2_IC_raw_phiNorm_odd[-1] = (TH2F*)( h2_ICOdd_raw[-1]->Clone("h2_IC_raw_phiNorm_odd_EEM") );
			h2_IC_raw_phiNorm_odd[1]  = (TH2F*)( h2_ICOdd_raw[1] ->Clone("h2_IC_raw_phiNorm_odd_EEP") );
			h2_IC_raw_phiNorm_odd[-1] -> Reset("ICEMS");
			h2_IC_raw_phiNorm_odd[1]  -> Reset("ICEMS");
			h2_IC_raw_phiNorm_odd[-1] -> ResetStats();
			h2_IC_raw_phiNorm_odd[1]  -> ResetStats();
		}
	}


	std::cout << "debug" << std::endl;

	// normalize each ring to the average IC of that eta ring
	if( isEB == true ) {
		NormalizeIC_EB(h2_IC_raw[0], h2_IC_raw_phiNorm[0], TT_centre[0]);
		if( evalStat ) {
			NormalizeIC_EB(h2_ICEven_raw[0], h2_IC_raw_phiNorm_even[0], TT_centre[0]);
			NormalizeIC_EB(h2_ICOdd_raw[0], h2_IC_raw_phiNorm_odd[0], TT_centre[0]);
		}
	} else {
		NormalizeIC_EE(h2_IC_raw[-1], h2_IC_raw[1], h2_IC_raw_phiNorm[-1], h2_IC_raw_phiNorm[1], TT_centre[-1], TT_centre[1], eRings);
		//    DrawCorr_EE(h2_IC_raw[-1],h2_IC_raw[1],h2_corrP[-1],h2_corrP[1],TT_centre[-1],TT_centre[1],corrMomentum,eRings,true,1);
		//    DrawICCorr_EE(h2_IC_raw[-1],h2_IC_raw[1],h2_IC_corr[-1],h2_IC_corr[1],TT_centre[-1],TT_centre[1],corrMomentum,eRings,true);
		if( evalStat ) {
			NormalizeIC_EE(h2_ICEven_raw[-1], h2_ICEven_raw[1], h2_IC_raw_phiNorm_even[-1], h2_IC_raw_phiNorm_even[1], TT_centre[-1], TT_centre[1], eRings);
			NormalizeIC_EE(h2_ICOdd_raw[-1], h2_ICOdd_raw[1], h2_IC_raw_phiNorm_odd[-1], h2_IC_raw_phiNorm_odd[1], TT_centre[-1], TT_centre[1], eRings);
		}
	}






	/////////////////////////////////////////////////////////////
	// plots for raw IC (only ring normalization)
	/////////////////////////////////////////////////////////////
	std::cout << ">>> Draw plots for raw IC" << std::endl;

	int etaRingWidth   = 1;
	int phiRegionWidth = 1;

	int nBins_spread = 2000;
	float spreadMin = 0.;
	float spreadMax = 2.;

	int nBins_stat = 1000;
	float statMin = -1.;
	float statMax = 1.;

	outFile -> mkdir("raw");
	outFile -> cd("raw");



	//--------------
	// spread histos
	std::cout << ">>>>>> spread histos" << std::endl;

	std::map<int, TH1F*> h_spread;
	std::map<int, std::vector<TH1F*> > h_spread_vsEta;
	std::map<int, TGraphErrors*> g_spread_vsEta;

	if( isEB == true ) {
		h_spread[0] = new TH1F("h_spread_EB", "", nBins_spread, spreadMin, spreadMax);
		g_spread_vsEta[0] = new TGraphErrors();

		BookSpreadHistos_EB(h_spread[0], h_spread_vsEta[0], g_spread_vsEta[0], etaRingWidth,
		                    "EB_spread_vsEta", nBins_spread, spreadMin, spreadMax,
		                    h2_IC_raw_phiNorm[0]);

		h_spread[0] -> Write();
		//for(unsigned int i = 0; i < h_spread_vsEta[0].size(); ++i)
		//  h_spread_vsEta[0].at(i) -> Write();
		g_spread_vsEta[0] -> Write("g_spread_vsEta_EB");
	} else {
		h_spread[-1] = new TH1F("h_spread_EEM", "", nBins_spread, spreadMin, spreadMax);
		h_spread[0]  = new TH1F("h_spread_EE", "", nBins_spread, spreadMin, spreadMax);
		h_spread[+1] = new TH1F("h_spread_EEP", "", nBins_spread, spreadMin, spreadMax);
		g_spread_vsEta[-1] = new TGraphErrors();
		g_spread_vsEta[0]  = new TGraphErrors();
		g_spread_vsEta[+1] = new TGraphErrors();

		std::map<int, TH2F*> dummy;
		BookSpreadHistos_EE(h_spread, h_spread_vsEta, g_spread_vsEta,
		                    eRings, etaRingWidth,
		                    "EE_spread_vsEta", nBins_spread, spreadMin, spreadMax,
		                    h2_IC_raw_phiNorm, dummy);

		h_spread[-1] -> Write();
		h_spread[0]  -> Write();
		h_spread[+1] -> Write();
		//for(unsigned int i = 0; i < h_spread_vsEta[0].size(); ++i)
		//{
		//  h_spread_vsEta[-1].at(i) -> Write();
		//  h_spread_vsEta[0].at(i)  -> Write();
		//  h_spread_vsEta[+1].at(i) -> Write();
		//}
		g_spread_vsEta[-1] -> Write("g_spread_vsEta_EEM");
		g_spread_vsEta[0]  -> Write("g_spread_vsEta_EE");
		g_spread_vsEta[+1] -> Write("g_spread_vsEta_EEP");
	}



	//-------------------
	// phi profile histos
	std::cout << ">>>>>> phi profile histos" << std::endl;

	std::map<int, TH1F*> h_phiAvgICSpread;
	std::map<int, TGraphErrors*> g_avgIC_vsPhi;

	if( isEB == true ) {
		h_phiAvgICSpread[0] = new TH1F("h_phiAvgICSpread_EB", "", nBins_spread, spreadMin, spreadMax);
		g_avgIC_vsPhi[0] = new TGraphErrors();

		PhiProfile(h_phiAvgICSpread[0], g_avgIC_vsPhi[0], phiRegionWidth, h2_IC_raw_phiNorm[0]);

		h_phiAvgICSpread[0] -> Write();
		g_avgIC_vsPhi[0] -> Write("g_avgIC_vsPhi_EB");
	} else {
		h_phiAvgICSpread[-1] = new TH1F("h_phiAvgICSpread_EEM", "", nBins_spread, spreadMin, spreadMax);
		h_phiAvgICSpread[+1] = new TH1F("h_phiAvgICSpread_EEP", "", nBins_spread, spreadMin, spreadMax);
		g_avgIC_vsPhi[-1] = new TGraphErrors();
		g_avgIC_vsPhi[+1] = new TGraphErrors();

		PhiProfile(h_phiAvgICSpread[-1], g_avgIC_vsPhi[-1], phiRegionWidth, h2_IC_raw_phiNorm[-1], eRings);
		PhiProfile(h_phiAvgICSpread[+1], g_avgIC_vsPhi[+1], phiRegionWidth, h2_IC_raw_phiNorm[+1], eRings);

		h_phiAvgICSpread[-1] -> Write();
		h_phiAvgICSpread[+1] -> Write();
		g_avgIC_vsPhi[-1] -> Write("g_avgIC_vsPhi_EEM");
		g_avgIC_vsPhi[+1] -> Write("g_avgIC_vsPhi_EEP");
	}



	//----------------------------------
	// phi-fold profile histos (EB only)
	std::cout << ">>>>>> phi-fold profile histos" << std::endl;

	TGraphErrors* g_avgIC_vsPhiFold_EBM = new TGraphErrors();
	TGraphErrors* g_avgIC_vsPhiFold_EBP = new TGraphErrors();

	if( isEB == true ) {
		PhiFoldProfile_EB(g_avgIC_vsPhiFold_EBM, g_avgIC_vsPhiFold_EBP, phiRegionWidth, h2_IC_raw_phiNorm[0]);

		g_avgIC_vsPhiFold_EBM -> Write("g_avgIC_vsPhiFold_EBM");
		g_avgIC_vsPhiFold_EBP -> Write("g_avgIC_vsPhiFold_EBP");
	}



	//-----------------------------
	// statistical precision histos
	std::cout << ">>>>>> stat histos" << std::endl;

	std::map<int, TH1F*> h_stat;
	std::map<int, std::vector<TH1F*> > h_stat_vsEta;
	std::map<int, TGraphErrors*> g_stat_vsEta;
	std::map<int, TGraphErrors*> g_residual_vsEta;

	if( isEB == true ) {
		h_stat[0] = new TH1F("h_stat_EB", "", nBins_stat, statMin, statMax);
		g_stat_vsEta[0] = new TGraphErrors();
		g_residual_vsEta[0] = new TGraphErrors();

		if( evalStat ) {
			BookSpreadHistos_EB(h_stat[0], h_stat_vsEta[0], g_stat_vsEta[0], etaRingWidth,
			                    "EB_stat_vsEta", nBins_stat, statMin, statMax,
			                    h2_IC_raw_phiNorm_even[0], h2_IC_raw_phiNorm_odd[0]);

			h_stat[0] -> Write();
			//for(unsigned int i = 0; i < h_stat_vsEta[0].size(); ++i)
			//  h_stat_vsEta[0].at(i) -> Write();
			g_stat_vsEta[0] -> Write("g_stat_vsEta_EB");

			ResidualSpread(g_stat_vsEta[0], g_spread_vsEta[0], g_residual_vsEta[0]);
		}
	} else {
		h_stat[-1] = new TH1F("h_stat_EEM", "", nBins_stat, statMin, statMax);
		h_stat[0]  = new TH1F("h_stat_EE", "", nBins_stat, statMin, statMax);
		h_stat[+1] = new TH1F("h_stat_EEP", "", nBins_stat, statMin, statMax);
		g_stat_vsEta[-1] = new TGraphErrors();
		g_stat_vsEta[0]  = new TGraphErrors();
		g_stat_vsEta[+1] = new TGraphErrors();
		g_residual_vsEta[-1] = new TGraphErrors();
		g_residual_vsEta[0]  = new TGraphErrors();
		g_residual_vsEta[+1] = new TGraphErrors();

		if( evalStat ) {
			BookSpreadHistos_EE(h_stat, h_stat_vsEta, g_stat_vsEta,
			                    eRings, etaRingWidth,
			                    "EE_stat_vsEta", nBins_stat, statMin, statMax,
			                    h2_IC_raw_phiNorm_even, h2_IC_raw_phiNorm_odd);

			h_stat[-1] -> Write();
			h_stat[0]  -> Write();
			h_stat[+1] -> Write();
			//for(unsigned int i = 0; i < h_stat_vsEta[0].size(); ++i)
			//{
			//  h_stat_vsEta[-1].at(i) -> Write();
			//  h_stat_vsEta[0].at(i)  -> Write();
			//  h_stat_vsEta[+1].at(i) -> Write();
			//}
			g_stat_vsEta[-1] -> Write("g_stat_vsEta_EEM");
			g_stat_vsEta[0]  -> Write("g_stat_vsEta_EE");
			g_stat_vsEta[+1] -> Write("g_stat_vsEta_EEP");

			ResidualSpread(g_stat_vsEta[-1], g_spread_vsEta[-1], g_residual_vsEta[-1]);
			ResidualSpread(g_stat_vsEta[0], g_spread_vsEta[0], g_residual_vsEta[0]);
			ResidualSpread(g_stat_vsEta[+1], g_spread_vsEta[+1], g_residual_vsEta[+1]);
		}
	}

	outFile -> cd();






	////////////////////////////////////////
	// apply corrections for crack structure
	////////////////////////////////////////
	std::cout << ">>> Draw plots for crack-corrected IC" << std::endl;

	TF1* pol0_EBM = new TF1("pol0_EBM", "pol0", 4., 16.);
	TF1* pol0_EBP = new TF1("pol0_EBP", "pol0", 4., 16.);

	if( isEB == true ) {
		pol0_EBP -> SetLineColor(kRed + 2);
		pol0_EBM -> SetLineColor(kRed + 2);
		pol0_EBP -> SetLineStyle(7);
		pol0_EBM -> SetLineStyle(7);

		g_avgIC_vsPhiFold_EBM -> Fit("pol0_EBM", "QRS+");
		g_avgIC_vsPhiFold_EBP -> Fit("pol0_EBP", "QRS+");

		outFile -> cd("raw");

		pol0_EBM -> Write();
		pol0_EBP -> Write();
	}

	outFile -> cd();



	if( isEB == true ) {
		for(int ibin = 1; ibin <= h2_IC_crackCorr[0]->GetNbinsX(); ++ibin)
			for(int jbin = 1; jbin <= h2_IC_crackCorr[0]->GetNbinsY(); ++jbin) {
				float IC = h2_IC_raw_phiNorm[0] -> GetBinContent(ibin, jbin);

				float phiRegionMin = h2_IC_crackCorr[0]->GetXaxis()->GetBinLowEdge(ibin);
				int phiRegion = int( (fabs(phiRegionMin) - 1.) / phiRegionWidth ) % 20;

				float etaBinCenter = h2_IC_crackCorr[0]->GetYaxis()->GetBinCenter(jbin);

				double phiFold, ICPhiFoldAvg;
				if( etaBinCenter < 0. ) {
					g_avgIC_vsPhiFold_EBM -> GetPoint(phiRegion, phiFold, ICPhiFoldAvg);
					h2_IC_crackCorr[0] -> SetBinContent(ibin, jbin, IC * pol0_EBM->GetParameter(0) / ICPhiFoldAvg);
				}
				if( etaBinCenter > 0. ) {
					g_avgIC_vsPhiFold_EBP -> GetPoint(phiRegion, phiFold, ICPhiFoldAvg);
					h2_IC_crackCorr[0] -> SetBinContent(ibin, jbin, IC * pol0_EBM->GetParameter(0) / ICPhiFoldAvg);
				}
			}

		NormalizeIC_EB(h2_IC_crackCorr[0], h2_IC_crackCorr_phiNorm[0], TT_centre[0], false);

		outFile -> mkdir("crackCorr");
		outFile -> cd("crackCorr");
	}



	//--------------
	// spread histos
	std::cout << ">>>>>> spread histos" << std::endl;

	TH1F* h_spread_crackCorr = new TH1F("h_spread_crackCorr", "", nBins_spread, spreadMin, spreadMax);
	std::vector<TH1F*> h_spread_vsEta_crackCorr;
	TGraphErrors* g_spread_vsEta_crackCorr = new TGraphErrors();

	if( isEB == true ) {
		BookSpreadHistos_EB(h_spread_crackCorr, h_spread_vsEta_crackCorr, g_spread_vsEta_crackCorr, etaRingWidth,
		                    "EB_spread_vsEta_crackCorr", nBins_spread, spreadMin, spreadMax,
		                    h2_IC_crackCorr_phiNorm[0]);

		h_spread_crackCorr -> Write();
		//for(unsigned int i = 0; i < h_spread_vsEta_crackCorr.size(); ++i)
		//  h_spread_vsEta_crackCorr.at(i) -> Write();
		g_spread_vsEta_crackCorr -> Write("g_spread_vsEta_crackCorr");
	}



	//-------------------
	// phi profile histos
	std::cout << ">>>>>> phi profile histos" << std::endl;

	TH1F* h_phiAvgICSpread_crackCorr = new TH1F("h_phiAvgICSpread_crackCorr", "", nBins_spread, spreadMin, spreadMax);
	TGraphErrors* g_avgIC_vsPhi_crackCorr = new TGraphErrors();

	if( isEB == true ) {
		PhiProfile(h_phiAvgICSpread_crackCorr, g_avgIC_vsPhi_crackCorr, phiRegionWidth, h2_IC_crackCorr_phiNorm[0]);

		h_phiAvgICSpread_crackCorr -> Write();
		g_avgIC_vsPhi_crackCorr -> Write("g_avgIC_vsPhi_crackCorr");
	}



	//------------------------
	// phi-fold profile histos
	std::cout << ">>>>>> phi-fold profile histos" << std::endl;

	TGraphErrors* g_avgIC_vsPhiFold_crackCorr_EBM = new TGraphErrors();
	TGraphErrors* g_avgIC_vsPhiFold_crackCorr_EBP = new TGraphErrors();

	if( isEB == true ) {
		PhiFoldProfile_EB(g_avgIC_vsPhiFold_crackCorr_EBM, g_avgIC_vsPhiFold_crackCorr_EBP, phiRegionWidth, h2_IC_crackCorr_phiNorm[0]);

		g_avgIC_vsPhiFold_crackCorr_EBM -> Write("g_avgIC_vsPhiFold_crackCorr_EBM");
		g_avgIC_vsPhiFold_crackCorr_EBP -> Write("g_avgIC_vsPhiFold_crackCorr_EBP");
	}



	//-----------------------------
	// statistical precision histos
	std::cout << ">>>>>> stat histos" << std::endl;

	std::vector<TH1F*> h_stat_vsEta_crackCorr;

	TH1F* h_stat_crackCorr = new TH1F("h_stat_crackCorr", "", nBins_stat, statMin, statMax);
	TGraphErrors* g_stat_vsEta_crackCorr = new TGraphErrors();
	TGraphErrors* g_residual_vsEta_crackCorr = new TGraphErrors();

	if( isEB == true) {

		if( evalStat ) {
			BookSpreadHistos_EB(h_stat_crackCorr, h_stat_vsEta_crackCorr, g_stat_vsEta_crackCorr, etaRingWidth,
			                    "EB_stat_vsEta_crackCorr", nBins_stat, statMin, statMax,
			                    h2_IC_raw_phiNorm_even[0], h2_IC_raw_phiNorm_odd[0]);

			h_stat_crackCorr -> Write();
			//for(unsigned int i = 0; i < h_stat_vsEta_crackCorr.size(); ++i)
			//  h_stat_vsEta_crackCorr.at(i) -> Write();
			g_stat_vsEta_crackCorr -> Write("g_stat_vsEta_crackCorr");


			ResidualSpread(g_stat_vsEta_crackCorr, g_spread_vsEta_crackCorr, g_residual_vsEta_crackCorr);
			g_residual_vsEta_crackCorr -> Write("g_residual_vsEta_crackCorr");
		}
	}
	outFile -> cd();






	/////////////////////////////////////////////////////////////
	// draw plots
	/////////////////////////////////////////////////////////////

	if( isEB == true ) {
		DrawICMap(h2_IC_raw_phiNorm[0],      outputFolder + "/EB_h2_IC_raw_phiNorm",      "png", isEB);
		DrawICMap(h2_IC_crackCorr_phiNorm[0], outputFolder + "/EB_h2_IC_crackCorr_phiNorm", "png", isEB);
		DrawICMap(h2_IC_raw_phiNorm[0],      outputFolder + "/EB_h2_IC_raw_phiNorm",      "pdf", isEB);
		DrawICMap(h2_IC_crackCorr_phiNorm[0], outputFolder + "/EB_h2_IC_crackCorr_phiNorm", "pdf", isEB);

		DrawSpreadHisto(h_spread[0],       outputFolder + "/EB_h_spread",          "f_EB_spread_vsEta",          "png", isEB);
		DrawSpreadHisto(h_spread_crackCorr, outputFolder + "/EB_h_spread_crackCorr", "f_EB_spread_vsEta_crackCorr", "png", isEB);
		DrawSpreadHisto(h_spread[0],       outputFolder + "/EB_h_spread",          "f_EB_spread_vsEta",          "pdf", isEB);
		DrawSpreadHisto(h_spread_crackCorr, outputFolder + "/EB_h_spread_crackCorr", "f_EB_spread_vsEta_crackCorr", "pdf", isEB);

		DrawSpreadGraph(g_spread_vsEta[0],       outputFolder + "/EB_g_spread_vsEta",          "png", isEB, g_stat_vsEta[0]);
		DrawSpreadGraph(g_spread_vsEta_crackCorr, outputFolder + "/EB_g_spread_vsEta_crackCorr", "png", isEB, g_stat_vsEta_crackCorr);
		DrawSpreadGraph(g_spread_vsEta[0],       outputFolder + "/EB_g_spread_vsEta",          "pdf", isEB, g_stat_vsEta[0]);
		DrawSpreadGraph(g_spread_vsEta_crackCorr, outputFolder + "/EB_g_spread_vsEta_crackCorr", "pdf", isEB, g_stat_vsEta_crackCorr);

		DrawResidualGraph(g_residual_vsEta[0],       outputFolder + "/EB_g_residual_vsEta",          "png", isEB);
		DrawResidualGraph(g_residual_vsEta_crackCorr, outputFolder + "/EB_g_residual_vsEta_crackCorr", "png", isEB);
		DrawResidualGraph(g_residual_vsEta[0],       outputFolder + "/EB_g_residual_vsEta",          "pdf", isEB);
		DrawResidualGraph(g_residual_vsEta_crackCorr, outputFolder + "/EB_g_residual_vsEta_crackCorr", "pdf", isEB);

		DrawPhiAvgICSpread(h_phiAvgICSpread[0],       outputFolder + "/EB_h_phiAvgICSpread",          "png", isEB);
		DrawPhiAvgICSpread(h_phiAvgICSpread_crackCorr, outputFolder + "/EB_h_phiAvgICSpread_crackCorr", "png", isEB);
		DrawPhiAvgICSpread(h_phiAvgICSpread[0],       outputFolder + "/EB_h_phiAvgICSpread",          "pdf", isEB);
		DrawPhiAvgICSpread(h_phiAvgICSpread_crackCorr, outputFolder + "/EB_h_phiAvgICSpread_crackCorr", "pdf", isEB);

		DrawAvgICVsPhiGraph(g_avgIC_vsPhi[0],       outputFolder + "/EB_g_avgIC_vsPhi",          "png", kRed + 2,  isEB);
		DrawAvgICVsPhiGraph(g_avgIC_vsPhi_crackCorr, outputFolder + "/EB_g_avgIC_vsPhi_crackCorr", "png", kGreen + 2, isEB);
		DrawAvgICVsPhiGraph(g_avgIC_vsPhi[0],       outputFolder + "/EB_g_avgIC_vsPhi",          "pdf", kRed + 2,  isEB);
		DrawAvgICVsPhiGraph(g_avgIC_vsPhi_crackCorr, outputFolder + "/EB_g_avgIC_vsPhi_crackCorr", "pdf", kGreen + 2, isEB);

		DrawAvgICVsPhiFoldGraph(g_avgIC_vsPhiFold_EBM, g_avgIC_vsPhiFold_crackCorr_EBM, outputFolder + "/EBM_g_avgIC_vsPhiFold", "png", isEB);
		DrawAvgICVsPhiFoldGraph(g_avgIC_vsPhiFold_EBP, g_avgIC_vsPhiFold_crackCorr_EBP, outputFolder + "/EBP_g_avgIC_vsPhiFold", "png", isEB);
		DrawAvgICVsPhiFoldGraph(g_avgIC_vsPhiFold_EBM, g_avgIC_vsPhiFold_crackCorr_EBM, outputFolder + "/EBM_g_avgIC_vsPhiFold", "pdf", isEB);
		DrawAvgICVsPhiFoldGraph(g_avgIC_vsPhiFold_EBP, g_avgIC_vsPhiFold_crackCorr_EBP, outputFolder + "/EBP_g_avgIC_vsPhiFold", "pdf", isEB);
	} else {
		DrawICMap(h2_IC_raw_phiNorm[-1], outputFolder + "/EEM_h2_IC_raw_phiNorm", "png", isEB);
		DrawICMap(h2_IC_raw_phiNorm[+1], outputFolder + "/EEP_h2_IC_raw_phiNorm", "png", isEB);
		DrawICMap(h2_IC_raw_phiNorm[-1], outputFolder + "/EEM_h2_IC_raw_phiNorm", "pdf", isEB);
		DrawICMap(h2_IC_raw_phiNorm[+1], outputFolder + "/EEP_h2_IC_raw_phiNorm", "pdf", isEB);

		//    DrawICMap(h2_corrP[-1],outputFolder+"/EEM_h2_corrP","png",isEB);
		//    DrawICMap(h2_corrP[+1],outputFolder+"/EEP_h2_corrP","png",isEB);

		//    DrawICMap(h2_IC_corr[-1],outputFolder+"/EEM_h2_IC_corr","png",isEB);
		//DrawICMap(h2_IC_corr[+1],outputFolder+"/EEP_h2_IC_corr","png",isEB);

		DrawSpreadHisto(h_spread[-1], outputFolder + "/EEM_h_spread", "f_EE_spread_vsEta_EEM", "png", isEB);
		DrawSpreadHisto(h_spread[0],  outputFolder + "/EE_h_spread", "f_EE_spread_vsEta_EE", "png", isEB);
		DrawSpreadHisto(h_spread[+1], outputFolder + "/EEP_h_spread", "f_EE_spread_vsEta_EEP", "png", isEB);
		DrawSpreadHisto(h_spread[-1], outputFolder + "/EEM_h_spread", "f_EE_spread_vsEta_EEM", "pdf", isEB);
		DrawSpreadHisto(h_spread[0],  outputFolder + "/EE_h_spread", "f_EE_spread_vsEta_EE", "pdf", isEB);
		DrawSpreadHisto(h_spread[+1], outputFolder + "/EEP_h_spread", "f_EE_spread_vsEta_EEP", "pdf", isEB);

		DrawSpreadGraph(g_spread_vsEta[-1], outputFolder + "/EEM_g_spread_vsEta", "png", isEB, g_stat_vsEta[-1]);
		DrawSpreadGraph(g_spread_vsEta[0],  outputFolder + "/EE_g_spread_vsEta", "png", isEB, g_stat_vsEta[0]);
		DrawSpreadGraph(g_spread_vsEta[+1], outputFolder + "/EEP_g_spread_vsEta", "png", isEB, g_stat_vsEta[+1]);
		DrawSpreadGraph(g_spread_vsEta[-1], outputFolder + "/EEM_g_spread_vsEta", "pdf", isEB, g_stat_vsEta[-1]);
		DrawSpreadGraph(g_spread_vsEta[0],  outputFolder + "/EE_g_spread_vsEta", "pdf", isEB, g_stat_vsEta[0]);
		DrawSpreadGraph(g_spread_vsEta[+1], outputFolder + "/EEP_g_spread_vsEta", "pdf", isEB, g_stat_vsEta[+1]);

		DrawResidualGraph(g_residual_vsEta[-1], outputFolder + "/EEM_g_residual_vsEta", "png", isEB);
		DrawResidualGraph(g_residual_vsEta[0],  outputFolder + "/EE_g_residual_vsEta", "png", isEB);
		DrawResidualGraph(g_residual_vsEta[+1], outputFolder + "/EEP_g_residual_vsEta", "png", isEB);
		DrawResidualGraph(g_residual_vsEta[-1], outputFolder + "/EEM_g_residual_vsEta", "pdf", isEB);
		DrawResidualGraph(g_residual_vsEta[0],  outputFolder + "/EE_g_residual_vsEta", "pdf", isEB);
		DrawResidualGraph(g_residual_vsEta[+1], outputFolder + "/EEP_g_residual_vsEta", "pdf", isEB);

		DrawPhiAvgICSpread(h_phiAvgICSpread[-1], outputFolder + "/EEM_h_phiAvgICSpread", "png", isEB);
		DrawPhiAvgICSpread(h_phiAvgICSpread[+1], outputFolder + "/EEP_h_phiAvgICSpread", "png", isEB);
		DrawPhiAvgICSpread(h_phiAvgICSpread[-1], outputFolder + "/EEM_h_phiAvgICSpread", "pdf", isEB);
		DrawPhiAvgICSpread(h_phiAvgICSpread[+1], outputFolder + "/EEP_h_phiAvgICSpread", "pdf", isEB);

		DrawAvgICVsPhiGraph(g_avgIC_vsPhi[-1], outputFolder + "/EEM_g_avgIC_vsPhi", "png", kRed + 2, isEB);
		DrawAvgICVsPhiGraph(g_avgIC_vsPhi[+1], outputFolder + "/EEP_g_avgIC_vsPhi", "png", kRed + 2, isEB);
		DrawAvgICVsPhiGraph(g_avgIC_vsPhi[-1], outputFolder + "/EEM_g_avgIC_vsPhi", "pdf", kRed + 2, isEB);
		DrawAvgICVsPhiGraph(g_avgIC_vsPhi[+1], outputFolder + "/EEP_g_avgIC_vsPhi", "pdf", kRed + 2, isEB);
	}

	outFile -> Close();






	//------------------------------------------------------
	// Dump IC in a txt file ---> IC from isolated electrons

	std::ofstream outTxt((outputFolder + "/" + outputTxt + "_relative.txt").c_str(), std::ios::out);

	//outTxt << "---------------------------------------------------------------" << std::endl;
	//if( isEB == true )
	//  outTxt << std::fixed << std::setprecision(0) << std::setw(10) << "iEta"
	//         << std::fixed << std::setprecision(0) << std::setw(10) << "iPhi"
	//         << std::fixed << std::setprecision(0) << std::setw(10) << "iZ"
	//         << std::fixed << std::setprecision(6) << std::setw(15) << "IC"
	//         << std::fixed << std::setprecision(6) << std::setw(15) << "error"
	//         << std::endl;
	//else
	//  outTxt << std::fixed << std::setprecision(0) << std::setw(10) << "iX"
	//         << std::fixed << std::setprecision(0) << std::setw(10) << "iY"
	//         << std::fixed << std::setprecision(0) << std::setw(10) << "iZ"
	//         << std::fixed << std::setprecision(6) << std::setw(15) << "IC"
	//         << std::fixed << std::setprecision(6) << std::setw(15) << "error"
	//         << std::endl;
	//outTxt << "---------------------------------------------------------------" << std::endl;


	std::map<int, TH2F*> h2_final;

	if( isEB == true) {
		h2_final[0] = h2_IC_crackCorr_phiNorm[0];
	} else {
		h2_final[-1] = h2_IC_raw_phiNorm[-1];
		h2_final[+1] = h2_IC_raw_phiNorm[+1];
	}


	if( isEB == true ) {
		for(int jbin = 1; jbin < h2_final[0]->GetNbinsY() + 1; ++jbin) {
			float iEta = h2_final[0] -> GetYaxis() -> GetBinLowEdge(jbin);

			if( iEta == 0. ) continue; // skip ieta=0

			double x, statPrec;
			if( iEta < 0 )
				g_stat_vsEta[0] -> GetPoint(int(fabs(iEta + 1)), x, statPrec); //mirroring of the folded precision
			else
				g_stat_vsEta[0] -> GetPoint(int(fabs(iEta - 1)), x, statPrec); //mirroring of the folded precision

			for(int ibin = 1; ibin < h2_final[0]->GetNbinsX() + 1; ++ibin) {
				float IC = h2_final[0] -> GetBinContent(ibin, jbin);

				outTxt << std::fixed << std::setprecision(0) << std::setw(10) << h2_final[0]->GetYaxis()->GetBinLowEdge(jbin)
				       << std::fixed << std::setprecision(0) << std::setw(10) << h2_final[0]->GetXaxis()->GetBinLowEdge(ibin)
				       << std::fixed << std::setprecision(0) << std::setw(10) << "0"; //iz for the barrel

				if( IC == 0. ) {
					outTxt << std::fixed << std::setprecision(6) << std::setw(15) << "-1."
					       << std::fixed << std::setprecision(6) << std::setw(15) << "999."
					       << std::endl;
				} else {
					outTxt << std::fixed << std::setprecision(6) << std::setw(15) << IC
					       << std::fixed << std::setprecision(6) << std::setw(15) << statPrec
					       << std::endl;
				}
			}
		}
	} else {
		for(int iz = -1; iz <= 1; ++iz) {
			if( iz == 0 ) continue;

			for(int ix = 1; ix < h2_final[iz]->GetNbinsX() + 1; ++ix) {
				for (int iy = 1; iy < h2_final[iz] -> GetNbinsY() + 1; ++iy) {
					if( existingEE->GetBinContent(ix, iy) != 1 ) continue;

					float IC = h2_final[iz]->GetBinContent(ix, iy);

					double x, statPrec, y, sysPrec;
					g_stat_vsEta[iz] -> GetPoint(int(eRings->GetEndcapRing(ix, iy, iz)), x, statPrec);
					MCSystematic_EE  -> GetPoint(int(eRings->GetEndcapRing(ix, iy, iz)), y, sysPrec);

					if( IC > 0. && IC < 2. ) {
						outTxt << std::fixed << std::setprecision(0) << std::setw(10) << h2_final[iz]->GetXaxis()->GetBinLowEdge(ix)
						       << std::fixed << std::setprecision(0) << std::setw(10) << h2_final[iz]->GetYaxis()->GetBinLowEdge(iy)
						       << std::fixed << std::setprecision(0) << std::setw(10) << iz
						       << std::fixed << std::setprecision(6) << std::setw(15) << IC
						       << std::fixed << std::setprecision(6) << std::setw(15) << sqrt( statPrec * statPrec ) //+ sysPrec*sysPrec )
						       << std::endl;
					} else {
						outTxt << std::fixed << std::setprecision(0) << std::setw(10) << h2_final[iz]->GetXaxis()->GetBinLowEdge(ix)
						       << std::fixed << std::setprecision(0) << std::setw(10) << h2_final[iz]->GetYaxis()->GetBinLowEdge(iy)
						       << std::fixed << std::setprecision(0) << std::setw(10) << iz
						       << std::fixed << std::setprecision(6) << std::setw(15) << "-1."
						       << std::fixed << std::setprecision(6) << std::setw(15) << "999."
						       << std::endl;
					}
				}
			}
		}
	}



	return 0;
}
