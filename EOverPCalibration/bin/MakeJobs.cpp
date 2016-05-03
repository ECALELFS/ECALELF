#include <iostream>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <istream>
#include <sstream>

#include "ConfigParser.h"
#include "ntpleUtils.h"


int main (int argc, char** argv)
{

	if(argc < 2) {
		std::cout << " Not correct number of input parameter --> Need Just one cfg file exit " << std::endl;
		return -1;
	}

	parseConfigFile(argv[1]);

// Parse Info for cfg creation

// Dead xtal in EB

	int PhiStartDeadXtal ;
	try {
		PhiStartDeadXtal =  gConfigParser -> readIntOption("Input::PhiStartDeadXtal");
	} catch(char const* exceptionString ) {
		PhiStartDeadXtal = -999.;
	}

	int PhiStepDeadXtal ;
	try {
		PhiStepDeadXtal  = gConfigParser -> readIntOption("Input::PhiStepDeadXtal");
	} catch(char const* exceptionString ) {
		PhiStepDeadXtal = -999.;
	}

	int PhiNShiftDeadXtal ;
	try {
		PhiNShiftDeadXtal = gConfigParser -> readIntOption("Input::PhiNShiftDeadXtal");
	} catch(char const* exceptionString ) {
		PhiNShiftDeadXtal = -999.;
	}

	std::vector<int> EtaBinCenterDeadXtal ;
	try {
		EtaBinCenterDeadXtal = gConfigParser -> readIntListOption("Input::EtaBinCenterDeadXtal");
	} catch(char const* exceptionString ) {
		EtaBinCenterDeadXtal.push_back(-999.);
	}


	bool isDeadTriggerTower ;
	try {
		isDeadTriggerTower = gConfigParser -> readBoolOption("Input::isDeadTriggerTower");
	} catch(char const* exceptionString ) {
		isDeadTriggerTower = false ;
	}

// Dead xtal in EE

	std::vector<int> IxEEPBinCenterDeadXtal ;
	try {
		IxEEPBinCenterDeadXtal = gConfigParser -> readIntListOption("Input::IxEEPBinCenterDeadXtal");
	} catch(char const* exceptionString ) {
		IxEEPBinCenterDeadXtal.push_back(-999.);
	}

	std::vector<int> IxEEMBinCenterDeadXtal ;
	try {
		IxEEMBinCenterDeadXtal = gConfigParser -> readIntListOption("Input::IxEEMBinCenterDeadXtal");
	} catch(char const* exceptionString ) {
		IxEEMBinCenterDeadXtal.push_back(-999.);
	}

	std::vector<int> IyEEPBinCenterDeadXtal ;
	try {
		IyEEPBinCenterDeadXtal = gConfigParser -> readIntListOption("Input::IyEEPBinCenterDeadXtal");
	} catch(char const* exceptionString ) {
		IyEEPBinCenterDeadXtal.push_back(-999.);
	}

	std::vector<int> IyEEMBinCenterDeadXtal ;
	try {
		IyEEMBinCenterDeadXtal = gConfigParser -> readIntListOption("Input::IyEEMBinCenterDeadXtal");
	} catch(char const* exceptionString ) {
		IyEEMBinCenterDeadXtal.push_back(-999.);
	}

	int iXNShiftDeadXtal ;
	try {
		iXNShiftDeadXtal = gConfigParser -> readIntOption("Input::iXNShiftDeadXtal");
	} catch(char const* exceptionString ) {
		iXNShiftDeadXtal = -999.;
	}

	int iYNShiftDeadXtal ;
	try {
		iYNShiftDeadXtal = gConfigParser -> readIntOption("Input::iYNShiftDeadXtal");
	} catch(char const* exceptionString ) {
		iYNShiftDeadXtal = -999.;
	}

	std::string OutputDeadXtalPath ;
	try {
		OutputDeadXtalPath = gConfigParser -> readStringOption("Input::OutputDeadXtalPath");
		system(("mkdir -p " + OutputDeadXtalPath).c_str());
	} catch(char const* exceptionString ) {
		OutputDeadXtalPath = "cfg/InputDeadXtal/" ;
		system(("mkdir -p " + OutputDeadXtalPath).c_str());
	}

	std::string OutputDeadXtalFile ;
	try {
		OutputDeadXtalFile = gConfigParser -> readStringOption("Input::OutputDeadXtalFile");
	} catch(char const* exceptionString ) {
		OutputDeadXtalFile = "DeadXtalFile";
	}


	std::string inputCfgTemplate ;
	try {
		inputCfgTemplate = gConfigParser -> readStringOption("Input::inputCfgTemplate");
	} catch(char const* exceptionString ) {
		inputCfgTemplate = "cfg/FastCalibrator_Template.cfg";
	}


///// Common cfg setup

// txt file with the list of input root files
	std::string inputList = gConfigParser -> readStringOption("Input::inputList");

// input tree name
	std::string inputTree = "NULL";
	try {
		inputTree = gConfigParser -> readStringOption("Input::inputTree");
	} catch(char const* exceptionString ) {
		inputTree = "simpleNtupleEoverP/SimpleNtupleEoverP";
	}

// jsonFileName
	std::string jsonFileName = "NULL";
	try {
		jsonFileName  = gConfigParser -> readStringOption("Input::jsonFileName");
	} catch( char const* exceptionString ) {
		jsonFileName = "json/Cert_190456-196531_8TeV_13Jul2012ReReco_Collisions12_JSON_v2.txt";
	}

// Miscalibration --> scalib 5%
	bool isMiscalib ;
	try {
		isMiscalib = gConfigParser -> readBoolOption("Input::isMiscalib");
	} catch( char const* exceptionString ) {
		isMiscalib = false;
	}

// Save EoverP distribution
	bool isSaveEPDistribution ;
	try {
		isSaveEPDistribution = gConfigParser -> readBoolOption("Input::isSaveEPDistribution");
	} catch( char const* exceptionString ) {
		isSaveEPDistribution = false;
	}

// Do the E/P selection
	bool isEPselection ;
	try {
		isEPselection = gConfigParser -> readBoolOption("Input::isEPselection");
	} catch( char const* exceptionString ) {
		isEPselection = false;
	}


// Pt treshold bool and cut
	bool isPtCut ;
	try {
		isPtCut = gConfigParser -> readBoolOption("Input::isPtCut");
	} catch( char const* exceptionString ) {
		isPtCut = false;
	}

	float PtMin ;
	try {
		PtMin = gConfigParser -> readFloatOption("Input::PtMin");
	} catch( char const* exceptionString ) {
		PtMin = 0.;
	}

// fbrem treshold bool and cut
	bool isfbrem ;
	try {
		isfbrem = gConfigParser -> readBoolOption("Input::isfbrem");
	} catch( char const* exceptionString ) {
		isfbrem = false;
	}

	float fbremMax ;
	try {
		fbremMax = gConfigParser -> readFloatOption("Input::fbremMax");
	} catch( char const* exceptionString ) {
		fbremMax = 100.;
	}

// R9 treshold bool and cut
	bool isR9selection ;
	try {
		isR9selection = gConfigParser -> readBoolOption("Input::isR9selection");
	} catch( char const* exceptionString ) {
		isR9selection = false;
	}

	float R9Min ;
	try {
		R9Min = gConfigParser -> readFloatOption("Input::R9Min");
	} catch( char const* exceptionString ) {
		R9Min = 0.;
	}

// Run Calibration on E/Etrue instead of E/P --> MC only
	bool isMCTruth ;
	try {
		isMCTruth = gConfigParser -> readBoolOption("Input::isMCTruth");
	} catch( char const* exceptionString ) {
		isMCTruth = false;
	}

// Momentum scale file
	std::string inputMomentumScale ;
	try {
		inputMomentumScale =  gConfigParser -> readStringOption("Input::inputMomentumScale");
	} catch( char const* exceptionString ) {
		inputMomentumScale = "output/MomentumCalibrationCombined_2011AB-2012ABC.root";
	}

	std::string typeEB ;
	try {
		typeEB = gConfigParser -> readStringOption("Input::typeEB");
	} catch( char const* exceptionString ) {
		typeEB = "none" ;
	}


	std::string typeEE ;
	try {
		typeEE = gConfigParser -> readStringOption("Input::typeEE");
	} catch( char const* exceptionString ) {
		typeEE = "none" ;
	}

// Name of the output calib file
	std::string outputPath;
	try {
		outputPath = gConfigParser -> readStringOption("Output::outputPath");
		system(("mkdir -p " + outputPath).c_str());
	} catch( char const* exceptionString ) {
		outputPath = "output/Oct22_Run2012ABC_Cal_Dic2012/";
		system(("mkdir -p " + outputPath).c_str());
	}

	std::string outputFile ;
	try {
		outputFile = gConfigParser -> readStringOption("Output::outputFile");
	} catch( char const* exceptionString ) {
		outputFile = "FastCalibrator_Oct22_Run2012ABC_Cal_Dic2012" ;
	}

	std::string OutputCfgPath ;
	try {
		OutputCfgPath = gConfigParser -> readStringOption("Output::OutputCfgPath");
		system(("mkdir -p " + OutputCfgPath).c_str());
	} catch(char const* exceptionString ) {
		OutputCfgPath = "cfg/JOB/";
		system(("mkdir -p " + OutputCfgPath).c_str());
	}

// Other options for the L3 algo
	int numberOfEvents ;
	try {
		numberOfEvents = gConfigParser -> readIntOption("Options::numberOfEvents");
	} catch( char const* exceptionString ) {
		numberOfEvents = -1 ;
	}

	int useZ ;
	try {
		useZ = gConfigParser -> readIntOption("Options::useZ");
	} catch( char const* exceptionString ) {
		useZ = 1 ;
	}

	int useW ;
	try {
		useW = gConfigParser -> readIntOption("Options::useW");
	} catch( char const* exceptionString ) {
		useW = 1 ;
	}

	int splitStat ;
	try {
		splitStat = gConfigParser -> readIntOption("Options::splitStat");
	} catch( char const* exceptionString ) {
		splitStat = 0 ;
	}

	int nLoops ;
	try {
		nLoops = gConfigParser -> readIntOption("Options::nLoops");
	} catch( char const* exceptionString ) {
		nLoops = 20 ;
	}

// prepare the general cfg:
	std::string inputTemp1 = inputCfgTemplate + "_tmp1" ;
	std::string inputTemp2 = inputCfgTemplate + "_tmp2" ;

	system(("rm  " + OutputCfgPath + "/*").c_str());

	system (("cat " + inputCfgTemplate + " | sed -e s%INPUTLIST%" + inputList + "%g > " + inputTemp1).c_str()) ;
	system (("cat " + inputTemp1 + " | sed -e s%INPUTTREE%" + inputTree + "%g > " + inputTemp2).c_str()) ;
	system (("cat " + inputTemp2 + " | sed -e s%JSONFILENAME%" + jsonFileName + "%g > " + inputTemp1).c_str()) ;
	system (("cat " + inputTemp1 + " | sed -e s%MISCALIB%" + std::string(Form("%o", isMiscalib)) + "%g > " + inputTemp2).c_str()) ;
	system (("cat " + inputTemp2 + " | sed -e s%EOVERP%" + std::string(Form("%o", isSaveEPDistribution)) + "%g > " + inputTemp1).c_str()) ;
	system (("cat " + inputTemp1 + " | sed -e s%EPSELECTION%" + std::string(Form("%o", isEPselection)) + "%g > " + inputTemp2).c_str()) ;
	system (("cat " + inputTemp2 + " | sed -e s%MCTRUTH%" + std::string(Form("%o", isMCTruth)) + "%g > " + inputTemp1).c_str()) ;
	system (("cat " + inputTemp1 + " | sed -e s%PTCUT%" + std::string(Form("%o", isPtCut)) + "%g > " + inputTemp2).c_str()) ;
	system (("cat " + inputTemp2 + " | sed -e s%PTMIN%" + std::string(Form("%0.2f", PtMin)) + "%g > " + inputTemp1).c_str()) ;
	system (("cat " + inputTemp1 + " | sed -e s%ISFBREM%" + std::string(Form("%o", isfbrem)) + "%g > " + inputTemp2).c_str()) ;
	system (("cat " + inputTemp2 + " | sed -e s%FBREMMAX%" + std::string(Form("%0.2f", fbremMax)) + "%g > " + inputTemp1).c_str()) ;
	system (("cat " + inputTemp1 + " | sed -e s%R9CUT%" + std::string(Form("%o", isR9selection)) + "%g > " + inputTemp2).c_str()) ;
	system (("cat " + inputTemp2 + " | sed -e s%R9MIN%" + std::string(Form("%0.2f", R9Min)) + "%g > " + inputTemp1).c_str()) ;
	system (("cat " + inputTemp1 + " | sed -e s%INPUTMOMENTUMSCALE%" + std::string(inputMomentumScale) + "%g > " + inputTemp2).c_str()) ;
	system (("cat " + inputTemp2 + " | sed -e s%TYPEEB%" + std::string(typeEB) + "%g > " + inputTemp1).c_str()) ;
	system (("cat " + inputTemp1 + " | sed -e s%TYPEEE%" + std::string(typeEE) + "%g > " + inputTemp2).c_str()) ;
	system (("cat " + inputTemp2 + " | sed -e s%OUTPUTPATH%" + std::string(outputPath) + "%g > " + inputTemp1).c_str()) ;
	system (("cat " + inputTemp1 + " | sed -e s%NUMEVENTS%" + std::string(Form("%d", numberOfEvents)) + "%g > " + inputTemp2).c_str()) ;
	system (("cat " + inputTemp2 + " | sed -e s%USEZ%" + std::string(Form("%d", useZ)) + "%g > " + inputTemp1).c_str()) ;
	system (("cat " + inputTemp1 + " | sed -e s%USEW%" + std::string(Form("%d", useW)) + "%g > " + inputTemp2).c_str()) ;
	system (("cat " + inputTemp2 + " | sed -e s%SPLIT%" + std::string(Form("%d", splitStat)) + "%g > " + inputTemp1).c_str()) ;
	system (("cat " + inputTemp1 + " | sed -e s%LOOPS%" + std::string(Form("%d", nLoops)) + "%g > " + inputTemp2).c_str()) ;
	system (("cat " + inputTemp2 + " | sed -e s%DEADTRIGGERTOWER%" + std::string(Form("%o", isDeadTriggerTower)) + "%g > " + inputTemp1).c_str()) ;


	system(std::string(Form("touch %slancia_EB.sh", OutputCfgPath.c_str())).c_str());
	system(std::string(Form("touch %slancia_EE.sh", OutputCfgPath.c_str())).c_str());

	std::ofstream JobStringEB(std::string(OutputCfgPath + "lancia_EB.sh").c_str(), std::ios::out);
	std::ofstream JobStringEE(std::string(OutputCfgPath + "lancia_EE.sh").c_str(), std::ios::out);

	std::cout << " <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< " << std::endl;
	std::cout << "  Generate Calibration Barrel Jobs   " << std::endl;
	std::cout << " <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< " << std::endl;


	for( unsigned int iEta = 0; iEta < EtaBinCenterDeadXtal.size(); iEta ++)

		std::cout << " EtaBinCenterDeadXtal : iEta " << EtaBinCenterDeadXtal.at(iEta) << std::endl;

	std::cout << " PhiStartDeadXtal   " << PhiStartDeadXtal << std::endl;
	std::cout << " PhiStepDeadXtal    " << PhiStepDeadXtal << std::endl;
	std::cout << " PhiNShiftDeadXtal  " << PhiNShiftDeadXtal << std::endl;

	if(PhiNShiftDeadXtal == -999 || PhiStartDeadXtal == -999 || EtaBinCenterDeadXtal.at(0) == -999 ) {

		// lunch normal calibration
		TString NameCfg_EB     = Form("%sFastCalibrator_EB.cfg", OutputCfgPath.c_str());

		system (("cat " + inputTemp1 + " | sed -e s%OUTPUTFILE%" + std::string(outputFile) + "%g > " + inputTemp2).c_str()) ;
		system (("cat " + inputTemp2 + " | sed -e s%INPUTFILEDEADXTAL%" + "%g > " + std::string(NameCfg_EB)).c_str()) ;

		TString NameJobFile_EB =  Form("%sbjob_FastCalibrator_EB.sh", OutputCfgPath.c_str());
		TString NameOut_EB     = Form("%sout_EB.txt", OutputCfgPath.c_str());

		system(("touch " + std::string(NameJobFile_EB)).c_str());

		std::ofstream outJobEB(std::string(NameJobFile_EB).c_str(), std::ios::out);
		outJobEB << "#!/bin/sh" << std::endl;
		outJobEB << "cd /gwpool/users/gerosa/CMSSW_5_3_2_patch4/src " << std::endl;
		outJobEB << "eval `scramv1 runtime -sh`" << std::endl;
		outJobEB << "cd " + std::string(getenv("PWD")) << std::endl;
		outJobEB << "source scripts/setup.sh " << std::endl;
		outJobEB << "unbuffer bin/FastCalibratorEB.exe " + std::string(NameCfg_EB) + " >> " + std::string(NameOut_EB) << std::endl;

		JobStringEB << "qsub -V -d " + std::string(getenv("PWD")) + "/" + OutputCfgPath + " -q longcms " + std::string(getenv("PWD")) + "/" + NameJobFile_EB << std::endl;

	}

	else {

		// dead xtal study

		for( int nShift = 0; nShift < PhiNShiftDeadXtal; nShift ++) {

			TString Name1 = Form("%s%s_EB_p%d.txt", OutputDeadXtalPath.c_str(), OutputDeadXtalFile.c_str(), nShift);
			TString Name2 = Form("%s%s_EB_m%d.txt", OutputDeadXtalPath.c_str(), OutputDeadXtalFile.c_str(), nShift);

			std::ofstream outDeadXtalEB_1(std::string(Name1).c_str(), std::ios::out);

			for( unsigned int iEta = 0; iEta < EtaBinCenterDeadXtal.size(); iEta ++) {

				for( unsigned int iPhi = PhiStartDeadXtal + nShift ; iPhi < 360 ; iPhi = iPhi + PhiStepDeadXtal)  outDeadXtalEB_1 << EtaBinCenterDeadXtal.at(iEta) << "    " << iPhi << std::endl;

			}


			TString NameCfg_EB_1    = Form("%sFastCalibrator_EB_pX%d.cfg", OutputCfgPath.c_str(), nShift);
			system (("cat " + inputTemp1 + " | sed -e s%OUTPUTFILE%" + std::string(outputFile) + std::string(Form("_p%d", nShift)) + "%g > " + inputTemp2).c_str()) ;
			system (("cat " + inputTemp2 + " | sed -e s%INPUTFILEDEADXTAL%" + std::string(Name1) + "%g > " + std::string(NameCfg_EB_1)).c_str()) ;

			TString NameJobFile_EB_1 =  Form("%sbjob_FastCalibrator_EB_p%d.sh", OutputCfgPath.c_str(), nShift);
			TString NameOut_EB_1     = Form("%sout_EB_p%d.txt", OutputCfgPath.c_str(), nShift);
			system(("touch " + std::string(NameJobFile_EB_1)).c_str());

			std::ofstream outJobEB_1(std::string(NameJobFile_EB_1).c_str(), std::ios::out);
			outJobEB_1 << "#!/bin/sh" << std::endl;
			outJobEB_1 << "cd /gwpool/users/gerosa/CMSSW_5_3_2_patch4/src " << std::endl;
			outJobEB_1 << "eval `scramv1 runtime -sh`" << std::endl;
			outJobEB_1 << "cd " + std::string(getenv("PWD")) << std::endl;
			outJobEB_1 << "source scripts/setup.sh " << std::endl;
			outJobEB_1 << "unbuffer bin/FastCalibratorEB.exe " + std::string(NameCfg_EB_1) + " >> " + std::string(NameOut_EB_1) << std::endl;

			std::ofstream outDeadXtalEB_2(std::string(Name2).c_str(), std::ios::out);

			for( unsigned int iEta = 0; iEta < EtaBinCenterDeadXtal.size(); iEta ++) {

				for( unsigned int iPhi = fabs(PhiStartDeadXtal - nShift) ; iPhi < 360 ; iPhi = iPhi + PhiStepDeadXtal)  outDeadXtalEB_2 << EtaBinCenterDeadXtal.at(iEta) << "    " << iPhi << std::endl;

			}

			TString NameCfg_EB_2    = Form("%sFastCalibrator_EB_m%d.cfg", OutputCfgPath.c_str(), nShift);
			system (("cat " + inputTemp1 + " | sed -e s%OUTPUTFILE%" + std::string(outputFile) + std::string(Form("_m%d", nShift)) + "%g > " + inputTemp2).c_str()) ;
			system (("cat " + inputTemp2 + " | sed -e s%INPUTFILEDEADXTAL%" + std::string(Name2) + "%g > " + std::string(NameCfg_EB_2)).c_str()) ;

			TString NameJobFile_EB_2 =  Form("%sbjob_FastCalibrator_EB_m%d.sh", OutputCfgPath.c_str(), nShift);
			TString NameOut_EB_2     = Form("%sout_EB_m%d.txt", OutputCfgPath.c_str(), nShift);
			system(("touch " + std::string(NameJobFile_EB_2)).c_str());


			std::ofstream outJobEB_2(std::string(NameJobFile_EB_2).c_str(), std::ios::out);
			outJobEB_2 << "#!/bin/sh" << std::endl;
			outJobEB_2 << "cd /gwpool/users/gerosa/CMSSW_5_3_2_patch4/src " << std::endl;
			outJobEB_2 << "eval `scramv1 runtime -sh`" << std::endl;
			outJobEB_2 << "cd " + std::string(getenv("PWD")) << std::endl;
			outJobEB_2 << "source scripts/setup.sh " << std::endl;
			outJobEB_2 << "unbuffer bin/FastCalibratorEB.exe " + std::string(NameCfg_EB_2) + " >> " + std::string(NameOut_EB_2) << std::endl;


			TString Name = Form("%s%s_EB_%d.txt", OutputDeadXtalPath.c_str(), OutputDeadXtalFile.c_str(), nShift);
			TString NameCfg_EB     = Form("%sFastCalibrator_EB_%d.cfg", OutputCfgPath.c_str(), nShift);
			TString NameJobFile_EB = Form("%sbjob_FastCalibrator_EB_%d.sh", OutputCfgPath.c_str(), nShift);
			TString NameOut_EB     = Form("%sout_EE_%d.txt", OutputCfgPath.c_str(), nShift);

			JobStringEB << "qsub -V -d " + std::string(getenv("PWD")) + "/" + OutputCfgPath + " -q longcms " + std::string(getenv("PWD")) + "/" + NameJobFile_EB_1 << std::endl;
			JobStringEB << "qsub -V -d " + std::string(getenv("PWD")) + "/" + OutputCfgPath + " -q longcms " + std::string(getenv("PWD")) + "/" + NameJobFile_EB_2 << std::endl;

			if( nShift == 0 ) {

				system(("if  diff " + std::string(Name1) + " " + std::string(Name2) + "  ; then mv " + std::string(Name1) + " " + std::string(Name2) + " ; mv " + std::string(Name2) + " " + std::string(Name) +
				        " ; mv " + std::string(NameCfg_EB_1) + " " + std::string(NameCfg_EB_2) + " ; mv " + std::string(NameCfg_EB_2) + " " + std::string(NameCfg_EB) + " ; " + "mv " +
				        std::string(NameJobFile_EB_1) + " " + std::string(NameJobFile_EB_2) + " ; mv " + std::string(NameJobFile_EB_2) + " " + std::string(NameJobFile_EB) + " ; fi").c_str());

				system (std::string(" cp " + NameCfg_EB + " " + " temp.cfg ").c_str());
				system (("cat temp.cfg | sed -e s%" + std::string(outputFile) + std::string(Form("_p%d", nShift)) + "%" + std::string(outputFile) + std::string(Form("_%d", nShift)) + "%g > temp2.cfg").c_str()) ;
				system (("cat temp2.cfg  | sed -e s%" + std::string(Name1) + "%" + std::string(Name) + "%g > " + std::string(NameCfg_EB)).c_str()) ;
				system(std::string("rm temp.cfg temp2.cfg").c_str());


				system (std::string(" cp " + NameJobFile_EB + " " + " temp.sh").c_str());
				system (("cat temp.sh | sed -e s%" + std::string(NameCfg_EB_1) + "%" + std::string(NameCfg_EB) + "%g > temp2.sh").c_str()) ;
				system (("cat temp2.sh  | sed -e s%" + std::string(NameOut_EB_1) + "%" + std::string(NameOut_EB) + "%g > " + std::string(NameJobFile_EB)).c_str()) ;
				system(std::string("rm temp.sh temp2.sh").c_str());

				JobStringEB.close();
				system(("sed '$d' < " + std::string(OutputCfgPath + "lancia_EB.sh") + " > tmp.txt ; sed '$d' < tmp.txt > " + std::string(OutputCfgPath + "lancia_EB.sh")).c_str());
				system(std::string(" rm tmp.txt ").c_str());
				JobStringEB.open(std::string(OutputCfgPath + "lancia_EB.sh").c_str(), std::ios::out);
				JobStringEB << "qsub -V -d " + std::string(getenv("PWD")) + "/" + OutputCfgPath + " -q longcms " + std::string(getenv("PWD")) + "/" + NameJobFile_EB << std::endl;

			}
		}
	}

	std::cout << " <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< " << std::endl;
	std::cout << "  Generate Calibration Endcap Jobs   " << std::endl;
	std::cout << " <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< " << std::endl;

	if(IxEEPBinCenterDeadXtal.size() != IyEEPBinCenterDeadXtal.size() ) {
		std::cout << " Error in EEP Dead Xtal List --> exit " << std::endl;
		return -1 ;
	}
	if(IxEEMBinCenterDeadXtal.size() != IyEEMBinCenterDeadXtal.size() ) {
		std::cout << " Error in EEM Dead Xtal List --> exit " << std::endl;
		return -1 ;
	}

	for( unsigned int iX = 0, iY = 0; iX < IxEEPBinCenterDeadXtal.size() && iY < IyEEPBinCenterDeadXtal.size(); iX ++ , iY ++)

		std::cout << " EEPBinCenterDeadXtal : iX " << IxEEPBinCenterDeadXtal.at(iX) << " iY " << IyEEPBinCenterDeadXtal.at(iY) << std::endl;

	for( unsigned int iX = 0, iY = 0; iX < IxEEMBinCenterDeadXtal.size() && iY < IyEEMBinCenterDeadXtal.size(); iX ++ , iY ++)

		std::cout << " EEMBinCenterDeadXtal : iX " << IxEEMBinCenterDeadXtal.at(iX) << " iY " << IyEEMBinCenterDeadXtal.at(iY) << std::endl;


	if( iXNShiftDeadXtal == -999 || iYNShiftDeadXtal == -999 || IxEEPBinCenterDeadXtal.at(0) == -999 || IyEEPBinCenterDeadXtal.at(0) == -999 || IxEEMBinCenterDeadXtal.at(0) == -999
	        || IyEEMBinCenterDeadXtal.at(0) == -999) {


		TString NameCfg_EE    = Form("%sFastCalibrator_EE.cfg", OutputCfgPath.c_str());
		system (("cat " + inputTemp1 + " | sed -e s%OUTPUTFILE%" + std::string(outputFile) + "%g > " + inputTemp2).c_str()) ;
		system (("cat " + inputTemp2 + " | sed -e s%INPUTFILEDEADXTAL%" + "%g > " + std::string(NameCfg_EE)).c_str()) ;

		TString NameJobFile_EE =  Form("%sbjob_FastCalibrator_EE.sh", OutputCfgPath.c_str());
		TString NameOut_EE     = Form("%sout_EE.txt", OutputCfgPath.c_str());
		system(("touch " + std::string(NameJobFile_EE)).c_str());

		std::ofstream outJobEE(std::string(NameJobFile_EE).c_str(), std::ios::out);
		outJobEE << "#!/bin/sh" << std::endl;
		outJobEE << "cd /gwpool/users/gerosa/CMSSW_5_3_2_patch4/src " << std::endl;
		outJobEE << "eval `scramv1 runtime -sh`" << std::endl;
		outJobEE << "cd " + std::string(getenv("PWD")) << std::endl;
		outJobEE << "source scripts/setup.sh " << std::endl;
		outJobEE << "unbuffer bin/FastCalibratorEE.exe " + std::string(NameCfg_EE) + " >> " + std::string(NameOut_EE) << std::endl;

		JobStringEE << "qsub -V -d " + std::string(getenv("PWD")) + "/" + OutputCfgPath + " -q longcms " + std::string(getenv("PWD")) + "/" + NameJobFile_EE << std::endl;


	} else {

		for( int xShift = 0 ; xShift < iXNShiftDeadXtal ; xShift++) {

			TString Name1 = Form("%s%s_EE_pX%d.txt", OutputDeadXtalPath.c_str(), OutputDeadXtalFile.c_str(), xShift);
			TString Name2 = Form("%s%s_EE_mX%d.txt", OutputDeadXtalPath.c_str(), OutputDeadXtalFile.c_str(), xShift);

			std::ofstream outDeadXtalEE_1(std::string(Name1).c_str(), std::ios::out);

			for( unsigned int iX = 0, iY = 0; iX < IxEEPBinCenterDeadXtal.size() && iY < IyEEPBinCenterDeadXtal.size(); iX ++ , iY ++)
				outDeadXtalEE_1 << IxEEPBinCenterDeadXtal.at(iX) + xShift << "  " << IyEEPBinCenterDeadXtal.at(iY) << "  " << "1" << std::endl;

			for( unsigned int iX = 0, iY = 0; iX < IxEEMBinCenterDeadXtal.size() && iY < IyEEMBinCenterDeadXtal.size(); iX ++ , iY ++)
				outDeadXtalEE_1 << IxEEMBinCenterDeadXtal.at(iX) + xShift << "  " << IyEEMBinCenterDeadXtal.at(iY) << "  " << "-1" << std::endl;



			TString NameCfg_EE_1    = Form("%sFastCalibrator_EE_pX%d.cfg", OutputCfgPath.c_str(), xShift);
			system (("cat " + inputTemp1 + " | sed -e s%OUTPUTFILE%" + std::string(outputFile) + std::string(Form("_pX%d", xShift)) + "%g > " + inputTemp2).c_str()) ;
			system (("cat " + inputTemp2 + " | sed -e s%INPUTFILEDEADXTAL%" + std::string(Name1) + "%g > " + std::string(NameCfg_EE_1)).c_str()) ;

			TString NameJobFile_EE_1 =  Form("%sbjob_FastCalibrator_EE_pX%d.sh", OutputCfgPath.c_str(), xShift);
			TString NameOut_EE_1     = Form("%sout_EE_pX%d.txt", OutputCfgPath.c_str(), xShift);
			system(("touch " + std::string(NameJobFile_EE_1)).c_str());

			std::ofstream outJobEE_1(std::string(NameJobFile_EE_1).c_str(), std::ios::out);
			outJobEE_1 << "#!/bin/sh" << std::endl;
			outJobEE_1 << "cd /gwpool/users/gerosa/CMSSW_5_3_2_patch4/src " << std::endl;
			outJobEE_1 << "eval `scramv1 runtime -sh`" << std::endl;
			outJobEE_1 << "cd " + std::string(getenv("PWD")) << std::endl;
			outJobEE_1 << "source scripts/setup.sh " << std::endl;
			outJobEE_1 << "unbuffer bin/FastCalibratorEE.exe " + std::string(NameCfg_EE_1) + " >> " + std::string(NameOut_EE_1) << std::endl;

			std::ofstream outDeadXtalEE_2(std::string(Name2).c_str(), std::ios::out);

			for( unsigned int iX = 0, iY = 0; iX < IxEEPBinCenterDeadXtal.size() && iY < IyEEPBinCenterDeadXtal.size(); iX ++ , iY ++)
				outDeadXtalEE_2 << IxEEPBinCenterDeadXtal.at(iX) - xShift << "  " << IyEEPBinCenterDeadXtal.at(iY) << "  " << "1" << std::endl;

			for( unsigned int iX = 0, iY = 0; iX < IxEEMBinCenterDeadXtal.size() && iY < IyEEMBinCenterDeadXtal.size(); iX ++ , iY ++)
				outDeadXtalEE_2 << IxEEMBinCenterDeadXtal.at(iX) - xShift << "  " << IyEEMBinCenterDeadXtal.at(iY) << "  " << "-1" << std::endl;

			TString NameCfg_EE_2    = Form("%sFastCalibrator_EE_mX%d.cfg", OutputCfgPath.c_str(), xShift);
			system (("cat " + inputTemp1 + " | sed -e s%OUTPUTFILE%" + std::string(outputFile) + std::string(Form("_mX%d", xShift)) + "%g > " + inputTemp2).c_str()) ;
			system (("cat " + inputTemp2 + " | sed -e s%INPUTFILEDEADXTAL%" + std::string(Name2) + "%g > " + std::string(NameCfg_EE_2)).c_str()) ;

			TString NameJobFile_EE_2 =  Form("%sbjob_FastCalibrator_EE_mX%d.sh", OutputCfgPath.c_str(), xShift);
			TString NameOut_EE_2     = Form("%sout_EE_mX%d.txt", OutputCfgPath.c_str(), xShift);
			system(("touch " + std::string(NameJobFile_EE_2)).c_str());

			std::ofstream outJobEE_2(std::string(NameJobFile_EE_2).c_str(), std::ios::out);
			outJobEE_2 << "#!/bin/sh" << std::endl;
			outJobEE_2 << "cd /gwpool/users/gerosa/CMSSW_5_3_2_patch4/src " << std::endl;
			outJobEE_2 << "eval `scramv1 runtime -sh`" << std::endl;
			outJobEE_2 << "cd " + std::string(getenv("PWD")) << std::endl;
			outJobEE_2 << "source scripts/setup.sh " << std::endl;
			outJobEE_2 << "unbuffer bin/FastCalibratorEE.exe " + std::string(NameCfg_EE_2) + " >> " + std::string(NameOut_EE_2) << std::endl;


			TString Name = Form("%s%s_EE_X%d.txt", OutputDeadXtalPath.c_str(), OutputDeadXtalFile.c_str(), xShift);
			TString NameCfg_EE     = Form("%sFastCalibrator_EE_X%d.cfg", OutputCfgPath.c_str(), xShift);
			TString NameJobFile_EE = Form("%sbjob_FastCalibrator_EE_X%d.sh", OutputCfgPath.c_str(), xShift);
			TString NameOut_EE     = Form("%sout_EE_X%d.txt", OutputCfgPath.c_str(), xShift);

			JobStringEE << "qsub -V -d " + std::string(getenv("PWD")) + "/" + OutputCfgPath + " -q longcms " + std::string(getenv("PWD")) + "/" + NameJobFile_EE_1 << std::endl;
			JobStringEE << "qsub -V -d " + std::string(getenv("PWD")) + "/" + OutputCfgPath + " -q longcms " + std::string(getenv("PWD")) + "/" + NameJobFile_EE_2 << std::endl;

			if( xShift == 0 ) {

				system(("if  diff " + std::string(Name1) + " " + std::string(Name2) + "  ; then mv " + std::string(Name1) + " " + std::string(Name2) + " ; mv " + std::string(Name2) + " " + std::string(Name) +
				        " ; mv " + std::string(NameCfg_EE_1) + " " + std::string(NameCfg_EE_2) + " ; mv " + std::string(NameCfg_EE_2) + " " + std::string(NameCfg_EE) + " ; " + "mv " +
				        std::string(NameJobFile_EE_1) + " " + std::string(NameJobFile_EE_2) + " ; mv " + std::string(NameJobFile_EE_2) + " " + std::string(NameJobFile_EE) + " ; fi").c_str());

				system (std::string(" cp " + NameCfg_EE + " " + " temp.cfg ").c_str());
				system (("cat temp.cfg | sed -e s%" + std::string(outputFile) + std::string(Form("_pX%d", xShift)) + "%" + std::string(outputFile) + std::string(Form("_X%d", xShift)) + "%g > temp2.cfg").c_str()) ;
				system (("cat temp2.cfg  | sed -e s%" + std::string(Name1) + "%" + std::string(Name) + "%g > " + std::string(NameCfg_EE)).c_str()) ;
				system(std::string("rm temp.cfg temp2.cfg").c_str());


				system (std::string(" cp " + NameJobFile_EE + " " + " temp.sh").c_str());
				system (("cat temp.sh | sed -e s%" + std::string(NameCfg_EE_1) + "%" + std::string(NameCfg_EE) + "%g > temp2.sh").c_str()) ;
				system (("cat temp2.sh  | sed -e s%" + std::string(NameOut_EE_1) + "%" + std::string(NameOut_EE) + "%g > " + std::string(NameJobFile_EE)).c_str()) ;
				system(std::string("rm temp.sh temp2.sh").c_str());

				JobStringEE.close();
				system(("sed '$d' < " + std::string(OutputCfgPath + "lancia_EE.sh") + " > tmp.txt ; sed '$d' < tmp.txt > " + std::string(OutputCfgPath + "lancia_EE.sh")).c_str());
				system(std::string(" rm tmp.txt ").c_str());
				JobStringEE.open(std::string(OutputCfgPath + "lancia_EE.sh").c_str(), std::ios::out);
				JobStringEE << "qsub -V -d " + std::string(getenv("PWD")) + "/" + OutputCfgPath + " -q longcms " + std::string(getenv("PWD")) + "/" + NameJobFile_EE << std::endl;
			}

		}

		for( int yShift = 1 ; yShift < iYNShiftDeadXtal ; yShift++) {

			TString Name1 = Form("%s%s_EE_pY%d.txt", OutputDeadXtalPath.c_str(), OutputDeadXtalFile.c_str(), yShift);
			TString Name2 = Form("%s%s_EE_mY%d.txt", OutputDeadXtalPath.c_str(), OutputDeadXtalFile.c_str(), yShift);

			std::ofstream outDeadXtalEE_1(std::string(Name1).c_str(), std::ios::out);

			for( unsigned int iX = 0, iY = 0; iX < IxEEPBinCenterDeadXtal.size() && iY < IyEEPBinCenterDeadXtal.size(); iX ++ , iY ++)
				outDeadXtalEE_1 << IxEEPBinCenterDeadXtal.at(iX) << "  " << IyEEPBinCenterDeadXtal.at(iY) + yShift << "  " << "1" << std::endl;

			for( unsigned int iX = 0, iY = 0; iX < IxEEMBinCenterDeadXtal.size() && iY < IyEEMBinCenterDeadXtal.size(); iX ++ , iY ++)
				outDeadXtalEE_1 << IxEEMBinCenterDeadXtal.at(iX) << "  " << IyEEMBinCenterDeadXtal.at(iY) + yShift << "  " << "-1" << std::endl;



			TString NameCfg_EE_1    = Form("%sFastCalibrator_EE_pY%d.cfg", OutputCfgPath.c_str(), yShift);
			system (("cat " + inputTemp1 + " | sed -e s%OUTPUTFILE%" + std::string(outputFile) + std::string(Form("_pY%d", yShift)) + "%g > " + inputTemp2).c_str()) ;
			system (("cat " + inputTemp2 + " | sed -e s%INPUTFILEDEADXTAL%" + std::string(Name1) + "%g > " + std::string(NameCfg_EE_1)).c_str()) ;

			TString NameJobFile_EE_1 =  Form("%sbjob_FastCalibrator_EE_pY%d.sh", OutputCfgPath.c_str(), yShift);
			TString NameOut_EE_1     = Form("%sout_EE_pY%d.txt", OutputCfgPath.c_str(), yShift);
			system(("touch " + std::string(NameJobFile_EE_1)).c_str());

			std::ofstream outJobEE_1(std::string(NameJobFile_EE_1).c_str(), std::ios::out);
			outJobEE_1 << "#!/bin/sh" << std::endl;
			outJobEE_1 << "cd /gwpool/users/gerosa/CMSSW_5_3_2_patch4/src " << std::endl;
			outJobEE_1 << "eval `scramv1 runtime -sh`" << std::endl;
			outJobEE_1 << "cd " + std::string(getenv("PWD")) << std::endl;
			outJobEE_1 << "source scripts/setup.sh " << std::endl;
			outJobEE_1 << "unbuffer bin/FastCalibratorEE.exe " + std::string(NameCfg_EE_1) + " >> " + std::string(NameOut_EE_1) << std::endl;

			std::ofstream outDeadXtalEE_2(std::string(Name2).c_str(), std::ios::out);

			for( unsigned int iX = 0, iY = 0; iX < IxEEPBinCenterDeadXtal.size() && iY < IyEEPBinCenterDeadXtal.size(); iX ++ , iY ++)
				outDeadXtalEE_2 << IxEEPBinCenterDeadXtal.at(iX) << "  " << IyEEPBinCenterDeadXtal.at(iY) - yShift << "  " << "1" << std::endl;

			for( unsigned int iX = 0, iY = 0; iX < IxEEMBinCenterDeadXtal.size() && iY < IyEEMBinCenterDeadXtal.size(); iX ++ , iY ++)
				outDeadXtalEE_2 << IxEEMBinCenterDeadXtal.at(iX) << "  " << IyEEMBinCenterDeadXtal.at(iY) - yShift << "  " << "-1" << std::endl;

			TString NameCfg_EE_2    = Form("%sFastCalibrator_EE_mY%d.cfg", OutputCfgPath.c_str(), yShift);
			system (("cat " + inputTemp1 + " | sed -e s%OUTPUTFILE%" + std::string(outputFile) + std::string(Form("_mY%d", yShift)) + "%g > " + inputTemp2).c_str()) ;
			system (("cat " + inputTemp2 + " | sed -e s%INPUTFILEDEADXTAL%" + std::string(Name2) + "%g > " + std::string(NameCfg_EE_2)).c_str()) ;

			TString NameJobFile_EE_2 =  Form("%sbjob_FastCalibrator_EE_mY%d.sh", OutputCfgPath.c_str(), yShift);
			TString NameOut_EE_2     = Form("%sout_EE_mY%d.txt", OutputCfgPath.c_str(), yShift);
			system(("touch " + std::string(NameJobFile_EE_2)).c_str());

			std::ofstream outJobEE_2(std::string(NameJobFile_EE_2).c_str(), std::ios::out);
			outJobEE_2 << "#!/bin/sh" << std::endl;
			outJobEE_2 << "cd /gwpool/users/gerosa/CMSSW_5_3_2_patch4/src " << std::endl;
			outJobEE_2 << "eval `scramv1 runtime -sh`" << std::endl;
			outJobEE_2 << "cd " + std::string(getenv("PWD")) << std::endl;
			outJobEE_2 << "source scripts/setup.sh " << std::endl;
			outJobEE_2 << "unbuffer bin/FastCalibratorEE.exe " + std::string(NameCfg_EE_2) + " >> " + std::string(NameOut_EE_2) << std::endl;


			TString Name = Form("%s%s_EE_Y%d.txt", OutputDeadXtalPath.c_str(), OutputDeadXtalFile.c_str(), yShift);
			TString NameCfg_EE     = Form("%sFastCalibrator_EE_Y%d.cfg", OutputCfgPath.c_str(), yShift);
			TString NameJobFile_EE = Form("%sbjob_FastCalibrator_EE_Y%d.sh", OutputCfgPath.c_str(), yShift);
			TString NameOut_EE     = Form("%sout_EE_Y%d.txt", OutputCfgPath.c_str(), yShift);

			JobStringEE << "qsub -V -d " + std::string(getenv("PWD")) + "/" + OutputCfgPath + " -q longcms " + std::string(getenv("PWD")) + "/" + NameJobFile_EE_1 << std::endl;
			JobStringEE << "qsub -V -d " + std::string(getenv("PWD")) + "/" + OutputCfgPath + " -q longcms " + std::string(getenv("PWD")) + "/" + NameJobFile_EE_2 << std::endl;


		}


	}


	system(("rm " + inputTemp1).c_str());
	system(("rm " + inputTemp2).c_str());

	return 0;

}
