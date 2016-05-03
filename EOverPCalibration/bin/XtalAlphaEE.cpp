#include "XtalAlphaEE.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "ConfigParser.h"
#include "ntpleUtils.h"
#include "CalibrationUtils.h"



int main (int argc, char ** argv)
{

	///Check if all nedeed arguments to parse are there
	if(argc != 2) {
		std::cerr << ">>>>> FastCalibrator::usage: " << argv[0] << " configFileName" << std::endl ;
		return 1;
	}

	/// Parse the config file
	parseConfigFile (argv[1]) ;

	//std::string inputFile = gConfigParser -> readStringOption("Input::inputFile");
	std::string inputList = gConfigParser -> readStringOption("Input::inputList");
	std::string inputTree = gConfigParser -> readStringOption("Input::inputTree");

	std::string inputFileDeadXtal = "NULL";
	try {
		inputFileDeadXtal = gConfigParser -> readStringOption("Input::inputFileDeadXtal");
	} catch( char const* exceptionString ) {
		std::cerr << " exception = " << exceptionString << std::endl;
	}

	std::string jsonFileName = gConfigParser -> readStringOption("Input::jsonFileName");
	std::map<int, std::vector<std::pair<int, int> > > jsonMap;
	jsonMap = readJSONFile(jsonFileName);

	bool isMiscalib = gConfigParser -> readBoolOption("Input::isMiscalib");
	bool isSaveEPDistribution = gConfigParser -> readBoolOption("Input::isSaveEPDistribution");
	bool isEPselection = gConfigParser -> readBoolOption("Input::isEPselection");
	bool isR9selection = gConfigParser -> readBoolOption("Input::isR9selection");
	float R9Min = gConfigParser -> readFloatOption("Input::R9Min");
	bool isMCTruth = gConfigParser -> readBoolOption("Input::isMCTruth");
	bool isfbrem = gConfigParser -> readBoolOption("Input::isfbrem");
	std::string inputMomentumScale =  gConfigParser -> readStringOption("Input::inputMomentumScale");

	std::string typeEB = gConfigParser -> readStringOption("Input::typeEB");
	std::string typeEE = gConfigParser -> readStringOption("Input::typeEE");

	int nRegionsEE = GetNRegionsEE(typeEE);

	std::string outputFile = gConfigParser -> readStringOption("Output::outputFile");

	int numberOfEvents = gConfigParser -> readIntOption("Options::numberOfEvents");
	int useZ = gConfigParser -> readIntOption("Options::useZ");
	int useW = gConfigParser -> readIntOption("Options::useW");
	int splitStat = gConfigParser -> readIntOption("Options::splitStat");
	int nLoops = gConfigParser -> readIntOption("Options::nLoops");

	/// Acquistion input ntuples
	TChain * albero = new TChain (inputTree.c_str());
	FillChain(*albero, inputList);

	/// open calibration momentum graph
	TFile* f4 = new TFile((inputMomentumScale + "_" + typeEB + "_" + typeEE + ".root").c_str());
	std::vector<TGraphErrors*> g_EoC_EE;

	for(int i = 0; i < nRegionsEE; ++i) {

		TString Name = Form("g_EoC_EE_%d", i);
		g_EoC_EE.push_back( (TGraphErrors*)(f4->Get(Name)) );
	}

	///Use the whole sample statistics if numberOfEvents < 0
	if ( numberOfEvents < 0 ) numberOfEvents = albero->GetEntries();

	/// run in normal mode: full statistics
	if ( splitStat == 0 ) {

		TString name ;
		TString outputTxtFile ;
		TString name_tmp;
		if(isMiscalib == true && useZ == 1 && isR9selection == true ) name_tmp = Form ("%s_WZ_R9_miscalib_EE", outputFile.c_str());
		if(isMiscalib == true && useZ == 1 && isEPselection == true ) name_tmp = Form ("%s_WZ_EP_miscalib_EE", outputFile.c_str());
		if(isMiscalib == true && useZ == 1 && isfbrem == true ) name_tmp = Form ("%s_WZ_fbrem_miscalib_EE", outputFile.c_str());
		if(isMiscalib == true && useZ == 1 && isEPselection == false && isR9selection == false && isfbrem == false ) name_tmp = Form ("%s_WZ_noEP_miscalib_EE", outputFile.c_str());

		if(isMiscalib == false && useZ == 1 && isR9selection == true ) name_tmp = Form ("%s_WZ_R9_EE", outputFile.c_str());
		if(isMiscalib == false && useZ == 1 && isEPselection == true ) name_tmp = Form ("%s_WZ_EP_EE", outputFile.c_str());
		if(isMiscalib == false && useZ == 1 && isfbrem == true ) name_tmp = Form ("%s_WZ_fbrem_EE", outputFile.c_str());
		if(isMiscalib == false && useZ == 1 && isEPselection == false && isR9selection == false && isfbrem == false ) name_tmp = Form ("%s_WZ_noEP_EE", outputFile.c_str());


		if(isMiscalib == true && useZ == 0 && isR9selection == true ) name_tmp = Form ("%s_W_R9_miscalib_EE", outputFile.c_str());
		if(isMiscalib == true && useZ == 0 && isEPselection == true ) name_tmp = Form ("%s_W_EP_miscalib_EE", outputFile.c_str());
		if(isMiscalib == true && useZ == 0 && isfbrem == true ) name_tmp = Form ("%s_W_fbrem_EE", outputFile.c_str());
		if(isMiscalib == true && useZ == 0 && isEPselection == false && isR9selection == false && isfbrem == false ) name_tmp = Form ("%s_W_noEP_miscalib_EE", outputFile.c_str());


		if(isMiscalib == false && useZ == 0 && isR9selection == true ) name_tmp = Form ("%s_W_R9_EE", outputFile.c_str());
		if(isMiscalib == false && useZ == 0 && isEPselection == true ) name_tmp = Form ("%s_W_EP_EE", outputFile.c_str());
		if(isMiscalib == false && useZ == 0 && isfbrem == true ) name_tmp = Form ("%s_W_fbrem_EE", outputFile.c_str());
		if(isMiscalib == false && useZ == 0 && isEPselection == false && isR9selection == false && isfbrem == false ) name_tmp = Form ("%s_W_noEP_EE", outputFile.c_str());

		name = Form("%s.root", name_tmp.Data());
		TFile *f1 = new TFile(name, "RECREATE");

		TString outEPDistribution = "Weight_" + name;

		TString DeadXtal = Form("%s", inputFileDeadXtal.c_str());


		if(isSaveEPDistribution == true) {

			XtalAlphaEE analyzer(albero, g_EoC_EE, typeEE, outEPDistribution);
			analyzer.bookHistos(nLoops);
			analyzer.AcquireDeadXtal(DeadXtal);
			analyzer.Loop(numberOfEvents, useZ, useW, splitStat, nLoops, isMiscalib, isSaveEPDistribution, isEPselection, isR9selection, R9Min, isMCTruth, isfbrem, jsonMap);
			analyzer.saveHistos(f1);
		} else {
			XtalAlphaEE analyzer(albero, g_EoC_EE, typeEE);
			analyzer.bookHistos(nLoops);
			analyzer.AcquireDeadXtal(DeadXtal);
			analyzer.Loop(numberOfEvents, useZ, useW, splitStat, nLoops, isMiscalib, isSaveEPDistribution, isEPselection, isR9selection, R9Min, isMCTruth, isfbrem, jsonMap);
			analyzer.saveHistos(f1);
		}

	}

	/// run in even-odd mode: half statistics
	else if ( splitStat == 1 ) {

		/// Prepare the outputs
		TString name;
		TString name2;

		if(isMiscalib == true && useZ == 1 && isR9selection == true) {
			name  = Form ("%s_WZ_R9_miscalib_EE_even.root", outputFile.c_str());
			name2 = Form ("%s_WZ_R9_miscalib_EE_odd.root", outputFile.c_str());
		}

		if(isMiscalib == true && useZ == 1 && isEPselection == true) {
			name  = Form ("%s_WZ_EP_miscalib_EE_even.root", outputFile.c_str());
			name2 = Form ("%s_WZ_EP_miscalib_EE.root", outputFile.c_str());
		}

		if(isMiscalib == true && useZ == 1 && isfbrem == true) {
			name  = Form ("%s_WZ_fbrem_miscalib_EE_even.root", outputFile.c_str());
			name2 = Form ("%s_WZ_fbrem_miscalib_EE_odd.root", outputFile.c_str());
		}

		if(isMiscalib == true && useZ == 1 && isR9selection == false && isEPselection == false && isfbrem == false) {
			name  = Form ("%s_WZ_noEP_miscalib_EE_even.root", outputFile.c_str());
			name2 = Form ("%s_WZ_noEP_miscalib_EE_odd.root", outputFile.c_str());
		}



		if(isMiscalib == false && useZ == 1 && isR9selection == true) {
			name  = Form ("%s_WZ_R9_EE_even.root", outputFile.c_str());
			name2 = Form ("%s_WZ_R9_EE_odd.root", outputFile.c_str());
		}

		if(isMiscalib == false && useZ == 1 && isEPselection == true) {
			name  = Form ("%s_WZ_EP_EE_even.root", outputFile.c_str());
			name2 = Form ("%s_WZ_EP_EE_odd.root", outputFile.c_str());
		}

		if(isMiscalib == false && useZ == 1 && isfbrem == true) {
			name  = Form ("%s_WZ_fbrem_EE_even.root", outputFile.c_str());
			name2 = Form ("%s_WZ_fbrem_EE_odd.root", outputFile.c_str());
		}
		if(isMiscalib == false && useZ == 1 && isR9selection == false && isEPselection == false && isfbrem == false) {
			name  = Form ("%s_WZ_noEP_EE_even.root", outputFile.c_str());
			name2 = Form ("%s_WZ_noEP_EE_odd.root", outputFile.c_str());
		}


		if(isMiscalib == true && useZ == 0 && isR9selection == true) {
			name  = Form ("%s_W_R9_miscalib_EE_even.root", outputFile.c_str());
			name2 = Form ("%s_W_R9_miscalib_EE_odd.root", outputFile.c_str());
		}

		if(isMiscalib == true && useZ == 0 && isEPselection == true) {
			name  = Form ("%s_W_EP_miscalib_EE_even.root", outputFile.c_str());
			name2 = Form ("%s_W_EP_miscalib_EE_odd.root", outputFile.c_str());
		}

		if(isMiscalib == true && useZ == 0 && isfbrem == true) {
			name  = Form ("%s_W_fbrem_miscalib_EE_even.root", outputFile.c_str());
			name2 = Form ("%s_W_fbrem_miscalib_EE_odd.root", outputFile.c_str());
		}

		if(isMiscalib == true && useZ == 0 && isR9selection == false && isEPselection == false && isfbrem == false) {
			name  = Form ("%s_W_noEP_miscalib_EE_even.root", outputFile.c_str());
			name2 = Form ("%s_W_noEP_miscalib_EE_odd.root", outputFile.c_str());
		}


		if(isMiscalib == false && useZ == 0 && isR9selection == true) {
			name  = Form ("%s_W_R9_EE_even.root", outputFile.c_str());
			name2 = Form ("%s_W_R9_EE_odd.root", outputFile.c_str());
		}

		if(isMiscalib == false && useZ == 0 && isEPselection == true) {
			name  = Form ("%s_W_EP_EE_even.root", outputFile.c_str());
			name2 = Form ("%s_W_EP_EE_odd.root", outputFile.c_str());
		}

		if(isMiscalib == false && useZ == 0 && isfbrem == true) {
			name  = Form ("%s_W_fbrem_miscalib_EE_even.root", outputFile.c_str());
			name2 = Form ("%s_W_fbrem_miscalib_EE_odd.root", outputFile.c_str());
		}

		if(isMiscalib == false && useZ == 0 && isR9selection == false && isEPselection == false && isfbrem == false) {
			name  = Form ("%s_W_noEP_EE_even.root", outputFile.c_str());
			name2 = Form ("%s_W_noEP_EE_odd.root", outputFile.c_str());
		}

		TFile *f1 = new TFile(name, "RECREATE");
		TFile *f2 = new TFile(name2, "RECREATE");
		TString DeadXtal = Form("%s", inputFileDeadXtal.c_str());


		/// Run on odd
		XtalAlphaEE analyzer_even(albero, g_EoC_EE, typeEE);
		analyzer_even.bookHistos(nLoops);
		analyzer_even.AcquireDeadXtal(DeadXtal);
		analyzer_even.Loop(numberOfEvents, useZ, useW, splitStat, nLoops, isMiscalib, isSaveEPDistribution, isEPselection, isR9selection, R9Min, isMCTruth, isfbrem, jsonMap);
		analyzer_even.saveHistos(f1);

		/// Run on even
		XtalAlphaEE analyzer_odd(albero, g_EoC_EE, typeEE);
		analyzer_odd.bookHistos(nLoops);
		analyzer_odd.AcquireDeadXtal(DeadXtal);
		analyzer_odd.Loop(numberOfEvents, useZ, useW, splitStat * (-1), nLoops, isMiscalib, isSaveEPDistribution, isEPselection, isR9selection, R9Min, isMCTruth, isfbrem, jsonMap);
		analyzer_odd.saveHistos(f2);

	}

	delete albero;
	return 0;
}
