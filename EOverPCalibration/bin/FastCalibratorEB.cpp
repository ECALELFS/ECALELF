#include "../interface/FastCalibratorEB.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "../interface/ntpleUtils.h"
#include "../interface/CalibrationUtils.h"

#include "FWCore/ParameterSet/interface/ProcessDesc.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

int main (int argc, char ** argv)
{

	///Check if all nedeed arguments to parse are there
	if(argc != 2) {
		std::cerr << ">>>>> FastCalibrator::usage: " << argv[0] << " configFileName" << std::endl ;
		return 1;
	}

	std::string configFileName = argv[1];
	boost::shared_ptr<edm::ParameterSet> parameterSet = edm::readConfig(configFileName);
	edm::ParameterSet Options = parameterSet -> getParameter<edm::ParameterSet>("Options");
	//  parameterSet.reset();

	std::string inputList = "NULL";
	if(Options.existsAs<std::string>("inputList"))
		inputList = Options.getParameter<std::string>("inputList");
	else {
		std::cout << " Exit from code, no input list found" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	std::string inputFileDeadXtal = "NULL";
	if(Options.existsAs<std::string>("inputFileDeadXtal"))
		inputFileDeadXtal = Options.getParameter<std::string>("inputFileDeadXtal");
	else {
		std::cout << " No input File Dead Xtal found! " << std::endl;
	}

	std::string inputTree = "NULL";
	if(Options.existsAs<std::string>("inputTree"))
		inputTree = Options.getParameter<std::string>("inputTree");
	else {
		std::cout << " Exit from code, no input tree found" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	std::string jsonFileName = "NULL";
	if(Options.existsAs<std::string>("jsonFileName"))
		jsonFileName = Options.getParameter<std::string>("jsonFileName");
	else {
		std::cout << " Exit from code, no jsonFile found" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	std::map<int, std::vector<std::pair<int, int> > > jsonMap;
	jsonMap = readJSONFile(jsonFileName);

	std::string miscalibMap = "NULL";
	if(Options.existsAs<std::string>("miscalibMap"))
		miscalibMap = Options.getParameter<std::string>("miscalibMap");
	else {
		std::cout << " no miscalib map found" << std::endl;
	}

	bool isMiscalib = false;
	if(Options.existsAs<bool>("isMiscalib"))
		isMiscalib = Options.getParameter<bool>("isMiscalib");

	bool isSaveEPDistribution = false;
	if(Options.existsAs<bool>("isSaveEPDistribution"))
		isSaveEPDistribution = Options.getParameter<bool>("isSaveEPDistribution");

	bool isMCTruth = false;
	if(Options.existsAs<bool>("isMCTruth"))
		isMCTruth = Options.getParameter<bool>("isMCTruth");

	bool isEPselection = false;
	if(Options.existsAs<bool>("isEPselection"))
		isEPselection = Options.getParameter<bool>("isEPselection");

	bool isPtCut = false;
	if(Options.existsAs<bool>("isPtCut"))
		isPtCut = Options.getParameter<bool>("isPtCut");

	double PtMin = 0.;
	if(Options.existsAs<double>("PtMin"))
		PtMin = Options.getParameter<double>("PtMin");

	bool isfbrem = false;
	if(Options.existsAs<bool>("isfbrem"))
		isfbrem = Options.getParameter<bool>("isfbrem");

	double fbremMax = 100.;
	if(Options.existsAs<double>("fbremMax"))
		fbremMax = Options.getParameter<double>("fbremMax");

	bool isR9selection = false;
	if(Options.existsAs<bool>("isR9selection"))
		isR9selection = Options.getParameter<bool>("isR9selection");

	double R9Min = -1.;
	if(Options.existsAs<double>("R9Min"))
		R9Min = Options.getParameter<double>("R9Min");

	int miscalibMethod = 1;
	if(Options.existsAs<int>("miscalibMethod"))
		miscalibMethod = Options.getParameter<int>("miscalibMethod");

	std::string inputMomentumScale = "NULL";
	if(Options.existsAs<std::string>("inputMomentumScale"))
		inputMomentumScale = Options.getParameter<std::string>("inputMomentumScale");

	std::string typeEB = "NULL";
	if(Options.existsAs<std::string>("typeEB"))
		typeEB = Options.getParameter<std::string>("typeEB");
	int nRegionsEB = GetNRegionsEB(typeEB);

	std::string typeEE = "NULL";
	if(Options.existsAs<std::string>("typeEE"))
		typeEE = Options.getParameter<std::string>("typeEE");

	std::string outputPath = "output/Oct22_Run2012ABC_Cal_Dic2012/";
	if(Options.existsAs<std::string>("outputPath"))
		outputPath = Options.getParameter<std::string>("outputPath");
	system(("mkdir -p " + outputPath).c_str());

	std::string outputFile = "FastCalibrator_Oct22_Run2012ABC_Cal_Dic2012";
	if(Options.existsAs<std::string>("outputFile"))
		outputFile = Options.getParameter<std::string>("outputFile");

	int numberOfEvents = -1;
	if(Options.existsAs<int>("numberOfEvents"))
		numberOfEvents = Options.getParameter<int>("numberOfEvents");

	int useZ = 1;
	if(Options.existsAs<int>("useZ"))
		useZ = Options.getParameter<int>("useZ");

	int useW = 1;
	if(Options.existsAs<int>("useW"))
		useW = Options.getParameter<int>("useW");

	int splitStat = 0;
	if(Options.existsAs<int>("splitStat"))
		splitStat = Options.getParameter<int>("splitStat");

	int nLoops = 20;
	if(Options.existsAs<int>("nLoops"))
		nLoops = Options.getParameter<int>("nLoops");

	bool isDeadTriggerTower = false;
	if(Options.existsAs<bool>("isDeadTriggerTower"))
		isDeadTriggerTower = Options.getParameter<bool>("isDeadTriggerTower");


	/// open ntupla of data or MC
	TChain * tree = new TChain (inputTree.c_str());
	FillChain(*tree, inputList);

	/// open calibration momentum graph
	TFile* momentumscale = new TFile((inputMomentumScale + "_" + typeEB + "_" + typeEE + ".root").c_str());
	std::vector<TGraphErrors*> g_EoC_EB;

	for(int i = 0; i < nRegionsEB; ++i) {
		TString Name = Form("g_EoC_EB_%d", i);
		g_EoC_EB.push_back( (TGraphErrors*)(momentumscale->Get(Name)) );
	}

	///Use the whole sample statistics if numberOfEvents < 0
	if ( numberOfEvents < 0 ) numberOfEvents = tree->GetEntries();

	/// run in normal mode: full statistics
	if ( splitStat == 0 ) {

		TString name ;
		TString name_tmp;

		if(isMiscalib == true && useZ == 1 && isR9selection == true && isEPselection == false && isfbrem == false && isPtCut == false )
			name_tmp = Form ("%s_WZ_R9_miscalib_EB", outputFile.c_str());
		else if(isMiscalib == true && useZ == 1 && isR9selection == false && isEPselection == false && isfbrem == true && isPtCut == false )
			name_tmp = Form ("%s_WZ_Fbrem_miscalib_EB", outputFile.c_str());
		else if(isMiscalib == true && useZ == 1 && isR9selection == true && isEPselection == false && isfbrem == false && isPtCut == true )
			name_tmp = Form ("%s_WZ_PT_miscalib_EB", outputFile.c_str());
		else if(isMiscalib == true && useZ == 1 && isR9selection == true && isEPselection == true && isfbrem == false && isPtCut == false )
			name_tmp = Form ("%s_WZ_EP_miscalib_EB", outputFile.c_str());
		else if(isMiscalib == true && useZ == 1 && isEPselection == false && isR9selection == false && isPtCut == false && isfbrem == false  )
			name_tmp = Form ("%s_WZ_noEP_miscalib_EB", outputFile.c_str());

		else if(isMiscalib == false && useZ == 1 && isR9selection == true && isEPselection == false && isfbrem == false && isPtCut == false )
			name_tmp = Form ("%s_WZ_R9_EB", outputFile.c_str());
		else if(isMiscalib == false && useZ == 1 && isR9selection == false && isEPselection == false && isfbrem == true && isPtCut == false )
			name_tmp = Form ("%s_WZ_Fbrem_EB", outputFile.c_str());
		else if(isMiscalib == false && useZ == 1 && isR9selection == true && isEPselection == false && isfbrem == false && isPtCut == true )
			name_tmp = Form ("%s_WZ_PT_EB", outputFile.c_str());
		else if(isMiscalib == false && useZ == 1 && isR9selection == true && isEPselection == true && isfbrem == false && isPtCut == false )
			name_tmp = Form ("%s_WZ_EP_EB", outputFile.c_str());
		else if(isMiscalib == false && useZ == 1 && isEPselection == false && isR9selection == false && isPtCut == false && isfbrem == false  )
			name_tmp = Form ("%s_WZ_noEP_EB", outputFile.c_str());

		else if(isMiscalib == true && useZ == 0 && isR9selection == true && isEPselection == false && isfbrem == false && isPtCut == false )
			name_tmp = Form ("%s_W_R9_miscalib_EB", outputFile.c_str());
		else if(isMiscalib == true && useZ == 0 && isR9selection == false && isEPselection == false && isfbrem == true && isPtCut == false )
			name_tmp = Form ("%s_W_Fbrem_miscalib_EB", outputFile.c_str());
		else if(isMiscalib == true && useZ == 0 && isR9selection == true && isEPselection == false && isfbrem == false && isPtCut == true )
			name_tmp = Form ("%s_W_PT_miscalib_EB", outputFile.c_str());
		else if(isMiscalib == true && useZ == 0 && isR9selection == true && isEPselection == true && isfbrem == false && isPtCut == false )
			name_tmp = Form ("%s_W_EP_miscalib_EB", outputFile.c_str());
		else if(isMiscalib == true && useZ == 0 && isEPselection == false && isR9selection == false && isPtCut == false && isfbrem == false  )
			name_tmp = Form ("%s_W_noEP_miscalib_EB", outputFile.c_str());

		else if(isMiscalib == false && useZ == 0 && isR9selection == true && isEPselection == false && isfbrem == false && isPtCut == false )
			name_tmp = Form ("%s_WZ_R9_EB", outputFile.c_str());
		else if(isMiscalib == false && useZ == 0 && isR9selection == false && isEPselection == false && isfbrem == true && isPtCut == false )
			name_tmp = Form ("%s_W_Fbrem_EB", outputFile.c_str());
		else if(isMiscalib == false && useZ == 0 && isR9selection == true && isEPselection == false && isfbrem == false && isPtCut == true )
			name_tmp = Form ("%s_W_PT_EB", outputFile.c_str());
		else if(isMiscalib == false && useZ == 0 && isR9selection == true && isEPselection == true && isfbrem == false && isPtCut == false )
			name_tmp = Form ("%s_W_EP_EB", outputFile.c_str());
		else if(isMiscalib == false && useZ == 0 && isEPselection == false && isR9selection == false && isPtCut == false && isfbrem == false  )
			name_tmp = Form ("%s_W_noEP_EB", outputFile.c_str());
		else {
			std::cout << " Option not considered --> exit " << std::endl;
			return -1 ;
		}

		name = Form("%s%s.root", outputPath.c_str(), name_tmp.Data());
		TFile *outputName = new TFile(name, "RECREATE");

		TString outEPDistribution = "Weight_" + name;

		TString DeadXtal = Form("%s", inputFileDeadXtal.c_str());

		if(isSaveEPDistribution == true) {
			FastCalibratorEB analyzer(tree, g_EoC_EB, typeEB, outEPDistribution);
			analyzer.bookHistos(nLoops);
			analyzer.AcquireDeadXtal(DeadXtal, isDeadTriggerTower);
			analyzer.Loop(numberOfEvents, useZ, useW, splitStat, nLoops, isMiscalib, isSaveEPDistribution, isEPselection, isR9selection, R9Min, isfbrem, fbremMax, isPtCut, PtMin, isMCTruth, jsonMap, miscalibMethod, miscalibMap);
			analyzer.saveHistos(outputName);
		} else {
			FastCalibratorEB analyzer(tree, g_EoC_EB, typeEB);
			analyzer.bookHistos(nLoops);
			analyzer.AcquireDeadXtal(DeadXtal, isDeadTriggerTower);
			analyzer.Loop(numberOfEvents, useZ, useW, splitStat, nLoops, isMiscalib, isSaveEPDistribution, isEPselection, isR9selection, R9Min, isfbrem, fbremMax, isPtCut, PtMin, isMCTruth, jsonMap, miscalibMethod, miscalibMap);
			analyzer.saveHistos(outputName);
		}

	}

	/// run in even-odd mode: half statistics
	else if ( splitStat == 1 ) {

		/// Prepare the outputs
		TString name;
		TString name2;

		if(isMiscalib == true && useZ == 1 && isR9selection == true && isEPselection == false && isfbrem == false && isPtCut == false) {
			name  = Form ("%s_WZ_R9_miscalib_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_WZ_R9_miscalib_EB_odd.root", outputFile.c_str());
		} else if(isMiscalib == true && useZ == 1 && isR9selection == false && isEPselection == true && isfbrem == false && isPtCut == false) {
			name  = Form ("%s_WZ_EP_miscalib_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_WZ_EP_miscalib_EB_odd.root", outputFile.c_str());
		} else if(isMiscalib == true && useZ == 1 && isR9selection == true && isEPselection == false && isfbrem == true && isPtCut == false) {
			name  = Form ("%s_WZ_Fbrem_miscalib_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_WZ_Fbrem_miscalib_EB_odd.root", outputFile.c_str());
		} else if(isMiscalib == true && useZ == 1 && isR9selection == true && isEPselection == false && isfbrem == false && isPtCut == true) {
			name  = Form ("%s_WZ_PT_miscalib_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_WZ_PT_miscalib_EB_odd.root", outputFile.c_str());
		} else if(isMiscalib == true && useZ == 1 && isR9selection == false && isEPselection == false && isfbrem == false && isPtCut == false) {
			name  = Form ("%s_WZ_noEP_miscalib_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_WZ_noEP_miscalib_EB_odd.root", outputFile.c_str());
		}



		else if(isMiscalib == false && useZ == 1 && isR9selection == true && isEPselection == false && isfbrem == false && isPtCut == false) {
			name  = Form ("%s_WZ_R9_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_WZ_R9_EB_odd.root", outputFile.c_str());
		} else if(isMiscalib == false && useZ == 1 && isR9selection == false && isEPselection == true && isfbrem == false && isPtCut == false) {
			name  = Form ("%s_WZ_EP_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_WZ_EP_EB_odd.root", outputFile.c_str());
		} else if(isMiscalib == false && useZ == 1 && isR9selection == false && isEPselection == false && isfbrem == true && isPtCut == false) {
			name  = Form ("%s_WZ_Fbrem_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_WZ_Fbrem_EB_odd.root", outputFile.c_str());
		} else if(isMiscalib == false && useZ == 1 && isR9selection == false && isEPselection == false && isfbrem == false && isPtCut == true) {
			name  = Form ("%s_WZ_PT_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_WZ_PT_EB_odd.root", outputFile.c_str());
		} else if(isMiscalib == false && useZ == 1 && isR9selection == false && isEPselection == false && isfbrem == false && isPtCut == false) {
			name  = Form ("%s_WZ_noEP_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_WZ_noEP_EB_odd.root", outputFile.c_str());
		}


		else if(isMiscalib == true && useZ == 0 && isR9selection == true && isEPselection == false && isfbrem == false && isPtCut == false) {
			name  = Form ("%s_W_R9_miscalib_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_W_R9_miscalib_EB_odd.root", outputFile.c_str());
		}

		else if(isMiscalib == true && useZ == 0 && isR9selection == false && isEPselection == true && isfbrem == false && isPtCut == false) {
			name  = Form ("%s_W_EP_miscalib_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_W_EP_miscalib_EB_odd.root", outputFile.c_str());
		} else if(isMiscalib == true && useZ == 0 && isR9selection == false && isEPselection == false && isfbrem == true && isPtCut == false) {
			name  = Form ("%s_W_Fbrem_miscalib_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_W_Fbrem_miscalib_EB_odd.root", outputFile.c_str());
		} else if(isMiscalib == true && useZ == 0 && isR9selection == false && isEPselection == false && isfbrem == false && isPtCut == true) {
			name  = Form ("%s_W_PT_miscalib_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_W_PT_miscalib_EB_odd.root", outputFile.c_str());
		} else if(isMiscalib == true && useZ == 0 && isR9selection == false && isEPselection == false && isfbrem == false && isPtCut == false) {
			name  = Form ("%s_W_noEP_miscalib_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_W_noEP_miscalib_EB_odd.root", outputFile.c_str());
		}

		else if(isMiscalib == false && useZ == 0 && isR9selection == true && isEPselection == false && isfbrem == false && isPtCut == false) {
			name  = Form ("%s_W_R9_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_W_R9_EB_odd.root", outputFile.c_str());
		}


		else if(isMiscalib == false && useZ == 0 && isR9selection == true && isEPselection == false && isfbrem == false && isPtCut == false) {
			name  = Form ("%s_W_EP_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_W_EP_EB_odd.root", outputFile.c_str());
		} else if(isMiscalib == false && useZ == 0 && isR9selection == false && isEPselection == true && isfbrem == false && isPtCut == false) {
			name  = Form ("%s_W_EP_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_W_EP_EB_odd.root", outputFile.c_str());
		} else if(isMiscalib == false && useZ == 0 && isR9selection == false && isEPselection == false && isfbrem == true && isPtCut == false) {
			name  = Form ("%s_W_Fbrem_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_W_Fbrem_EB_odd.root", outputFile.c_str());
		} else if(isMiscalib == false && useZ == 0 && isR9selection == false && isEPselection == false && isfbrem == false && isPtCut == true) {
			name  = Form ("%s_W_PT_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_W_PT_EB_odd.root", outputFile.c_str());
		} else if(isMiscalib == false && useZ == 0 && isR9selection == true && isEPselection == false && isfbrem == false && isPtCut == false) {
			name  = Form ("%s_W_noEP_EB_even.root", outputFile.c_str());
			name2 = Form ("%s_W_noEP_EB_odd.root", outputFile.c_str());
		} else {
			std::cout << " Option not considered --> exit " << std::endl;
			return -1 ;
		}

		TFile *outputName1 = new TFile(outputPath + name, "RECREATE");
		TFile *outputName2 = new TFile(outputPath + name2, "RECREATE");

		TString DeadXtal = Form("%s", inputFileDeadXtal.c_str());

		/// Run on odd
		FastCalibratorEB analyzer_even(tree, g_EoC_EB, typeEB);
		analyzer_even.bookHistos(nLoops);
		analyzer_even.AcquireDeadXtal(DeadXtal, isDeadTriggerTower);
		analyzer_even.Loop(numberOfEvents, useZ, useW, splitStat, nLoops, isMiscalib, isSaveEPDistribution, isEPselection, isR9selection, R9Min, isfbrem, fbremMax, isPtCut, PtMin, isMCTruth, jsonMap, miscalibMethod, miscalibMap);
		analyzer_even.saveHistos(outputName1);

		/// Run on even
		FastCalibratorEB analyzer_odd(tree, g_EoC_EB, typeEB);
		analyzer_odd.bookHistos(nLoops);
		analyzer_odd.AcquireDeadXtal(DeadXtal, isDeadTriggerTower);
		analyzer_odd.Loop(numberOfEvents, useZ, useW, splitStat * (-1), nLoops, isMiscalib, isSaveEPDistribution, isEPselection, isR9selection, R9Min, isfbrem, fbremMax, isPtCut, PtMin, isMCTruth, jsonMap, miscalibMethod, miscalibMap);
		analyzer_odd.saveHistos(outputName2);

	}

	delete tree;
	return 0;
}
