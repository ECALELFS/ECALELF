// @(#)root/tmva $Id: MVARegression.cpp,v 1.3 2012/07/17 08:31:45 rgerosa Exp $
/**********************************************************************************
 * Project   : TMVA - a Root-integrated toolkit for multivariate data analysis    *
 * Package   : TMVA                                                               *
 * Root Macro: TMVARegression                                                     *
 *                                                                                *
 * This macro provides examples for the training and testing of the               *
 * TMVA classifiers.                                                              *
 *                                                                                *
 * As input data is used a toy-MC sample consisting of four Gaussian-distributed  *
 * and linearly correlated input variables.                                       *
 *                                                                                *
 * The methods to be used can be switched on and off by means of booleans, or     *
 * via the prompt command, for example:                                           *
 *                                                                                *
 *    root -l TMVARegression.C\(\"LD,MLP\"\)                                      *
 *                                                                                *
 * (note that the backslashes are mandatory)                                      *
 * If no method given, a default set is used.                                     *
 *                                                                                *
 * The output file "TMVAReg.root" can be analysed with the use of dedicated       *
 * macros (simply say: root -l <macro.C>), which can be conveniently              *
 * invoked through a GUI that will appear at the end of the run of this macro.    *
 **********************************************************************************/

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TApplication.h"
#include "TMVA/MsgLogger.h"
#include "TMVA/Config.h"

#include "ConfigParser.h"
#include "ntpleUtils.h"

#if not defined(__CINT__) || defined(__MAKECINT__)
#include "TMVA/Tools.h"
#include "TMVA/Factory.h"
#endif

#include "/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.00/x86_64-slc5-gcc46-opt/root/tmva/test/TMVARegGui.C"

using namespace TMVA;

int main(int argc, char**argv)
{

	if(argc != 2) {
		std::cerr << " >>>>> analysis.cpp::usage: " << argv[0] << " configFileName" << std::endl ;
		return 1;
	}

	parseConfigFile (argv[1]) ;

// The explicit loading of the shared libTMVA is done in TMVAlogon.C, defined in .rootrc
// if you use your private .rootrc, or run from a different directory, please copy the
// corresponding lines from .rootrc
// methods to be processed can be given as an argument; use format:
//
// mylinux~> root -l TMVARegression.C\(\"myMethod1,myMethod2,myMethod3\"\)
//
//---------------------------------------------------------------
// This loads the library
	TMVA::Tools::Instance();

	std::vector<std::string> UseMethodName;
	UseMethodName = gConfigParser -> readStringListOption("Input::UseMethodName");

	std::cout << " >>>>> Input::UseMethodName size = " << UseMethodName.size() << std::endl;
	std::cout << " >>>>> >>>>>  ";
	for (unsigned int iCat = 0; iCat < UseMethodName.size(); iCat++) {
		std::cout << " " << UseMethodName.at(iCat) << ", ";
	}
	std::cout << std::endl;



// ---------------------------------------------------------------

	std::cout << std::endl;
	std::cout << "==> Start TMVARegression" << std::endl;

	std::map<std::string, int> Use;

	for(std::vector<std::string>::iterator it = UseMethodName.begin(); it != UseMethodName.end(); ++it) Use[*it] = 0;

	std::string UseMethodFlag;
	try {
		UseMethodFlag = gConfigParser -> readStringOption("Input::UseMethodFlag");
		std::cout << UseMethodFlag << std::endl;
		std::vector<TString> mlist = gTools().SplitString( UseMethodFlag, '/' );
		for (UInt_t i = 0; i < mlist.size(); i++) {

			std::string regMethod(mlist[i]);
			if (Use.find(regMethod) == Use.end()) {
				std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
				for (std::map<std::string, int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";
				std::cout << std::endl;
				return -1;
			}
			Use[regMethod] = 1;
			std::cout << "regMethod= " << regMethod << " 1 " << std::endl;

		}
	} catch (char * exception) {

		std::cerr << " exception =  Use All method " << std::endl;
		for(std::vector<std::string>::iterator it = UseMethodName.begin() ; it != UseMethodName.end(); it++) Use[*it] = 1;

	}


// --------------------------------------------------------------------------------------------------
// --- Here the preparation phase begins
// Create a new root output file

	std::string outputFileName =  gConfigParser -> readStringOption("Output::outputFileName");
	std::cout << " Output Data File = " << outputFileName << std::endl;

	TFile* outputFile = TFile::Open( outputFileName.c_str(), "RECREATE" );

// Create the factory object. Later you can choose the methods
// whose performance you'd like to investigate. The factory will
// then run the performance analysis for you.
// The first argument is the base of the name of all the
// weightfiles in the directory weight/
// All TMVA output can be suppressed by removing the "!" (not) in
// front of the "Silent" argument in the option string

// Read training and test data (see TMVAClassification for reading ASCII files)
// load the signal and background event samples from ROOT trees
	std::string inputFileList =  gConfigParser -> readStringOption("Input::inputFileList");
	std::string treeNameDATA =  gConfigParser -> readStringOption("Input::treeNameDATA");

	std::cout << " Input Data List = " << inputFileList << std::endl;

	TChain* treeDATA = new TChain(treeNameDATA.c_str());

	FillChain(*treeDATA, inputFileList.c_str());


	TMVA::Factory *factory = new TMVA::Factory( "TMVARegression", outputFile,
	        "!V:!Silent:Color:DrawProgressBar" );

// If you wish to modify default settings
// (please check "src/Config.h" to see all available global options)
//    (TMVA::gConfig().GetVariablePlotting()).fTimesRMS = 8.0;
//    (TMVA::gConfig().GetIONames()).fWeightFileDir = "myWeightDirectory";
// Define the input variables that shall be used for the MVA training
// note that you may also use variable expressions, such as: "3*var1/var2*abs(var3)"
// [all types of expressions that can also be parsed by TTree::Draw( "expression" )]
	std::string RegionOfTraining =  gConfigParser -> readStringOption("Input::RegionOfTraining");
	std::cout << " RegionOfTraining = " << RegionOfTraining << std::endl;

	if(RegionOfTraining == "EB") {


		factory->AddVariable( "ele1_scE/ele1_scERaw" , 'F');
		factory->AddVariable( "ele1_eRegrInput_nPV" , 'F');
		factory->AddVariable( "ele1_eRegrInput_r9" , 'F');
		factory->AddVariable( "ele1_fbrem" , 'F');
		factory->AddVariable( "ele1_eta" , 'F');
		factory->AddVariable( "ele1_DphiIn" , 'F');
		factory->AddVariable( "ele1_DetaIn" , 'F');
		factory->AddVariable( "ele1_sigmaIetaIeta" , 'F');

		factory->AddVariable( "ele1_eRegrInput_etaW" , 'F');
		factory->AddVariable( "ele1_eRegrInput_phiW" , 'F');

		factory->AddVariable( "ele1_eRegrInput_bCE_Over_sCE", 'F');
		factory->AddVariable( "ele1_eRegrInput_sigietaieta_bC1" , 'F');
		factory->AddVariable( "ele1_eRegrInput_sigiphiiphi_bC1" , 'F');
		factory->AddVariable( "ele1_eRegrInput_sigietaiphi_bC1" , 'F');
		factory->AddVariable( "ele1_eRegrInput_e3x3_Over_bCE" , 'F');
		factory->AddVariable( "ele1_eRegrInput_Deta_bC_sC" , 'F');
		factory->AddVariable( "ele1_eRegrInput_Dphi_bC_sC" , 'F');
		factory->AddVariable( "ele1_eRegrInput_bEMax_Over_bCE" , 'F');


		factory->AddVariable( "ele1_dxy_PV" , 'F');
		factory->AddVariable( "ele1_dz_PV" , 'F');
		factory->AddVariable( "ele1_sigmaP/ele1_tkP" , 'F');

		factory->AddVariable( "ele1_eRegrInput_bCELow_Over_sCE", 'F');
		factory->AddVariable( "ele1_eRegrInput_e3x3_Over_bCELow" , 'F');
		factory->AddVariable( "ele1_eRegrInput_Deta_bCLow_sC" , 'F');
		factory->AddVariable( "ele1_eRegrInput_Dphi_bCLow_sC" , 'F');

		factory->AddVariable( "ele1_eRegrInput_seedbC_etacry" , 'F');
		factory->AddVariable( "ele1_eRegrInput_seedbC_phicry" , 'F');

// You can add so-called "Spectator variables", which are not used in the MVA training,
// but will appear in the final "TestTree" produced by TMVA. This TestTree will contain the
// input variables, the response values of all trained MVAs, and the spectator variables
// factory->AddSpectator( "spec1:=var1*2",  "Spectator 1", "units", 'F' );
// factory->AddSpectator( "spec2:=var1*3",  "Spectator 2", "units", 'F' );
// Add the variable carrying the regression target
//   factory->AddTarget("ele1_scE/ele1_E_true" );
		factory->AddTarget("ele1_tkP/ele1_E_true" );


// It is also possible to declare additional targets for multi-dimensional regression, ie:
// -- factory->AddTarget( "fvalue2" );
// BUT: this is currently ONLY implemented for MLP

// global event weights per tree (see below for setting event-wise weights)
		Double_t regWeight  = 1.0;

// You can add an arbitrary number of regression trees
		factory->AddRegressionTree( treeDATA, regWeight );

// This would set individual event weights (the variables defined in the
// expression need to exist in the original TTree)
// factory->SetWeightExpression( "var1", "Regression" );

//  TCut mycut = "ele1_isEB==1 && ele1_sigmaP/ele1_tkP<0.4 && ele1_fbrem>0 && abs(ele1_dxy_PV)<0.05 && abs(ele1_dz_PV)<0.05 && ele1_eRegrInput_etaW > 0.006 && ele1_eRegrInput_phiW<0.08 && ele1_eRegrInput_sigietaieta_bC1>0.006 && ele1_eRegrInput_sigiphiiphi_bC1>0.008  && abs(ele1_eRegrInput_Deta_bC_sC)<0.004 && abs(ele1_eRegrInput_Dphi_bC_sC)<0.04 && abs(ele1_eRegrInput_seedbC_etacry)<0.6 && abs(ele1_eRegrInput_seedbC_phicry)<0.6 && ele1_scE/ele1_scERaw<1.2 && (ele1_scE/ele1_E_true)<1.4 && (ele1_scE/ele1_E_true)>0.3"; // for example: TCut mycut = "abs(var1)<0.5 && abs(var2-0.5)<1";

		TCut mycut = "ele1_isEB==1 && ele1_sigmaP/ele1_tkP<0.4 && ele1_fbrem>0 && abs(ele1_dxy_PV)<0.05 && abs(ele1_dz_PV)<0.05 && ele1_eRegrInput_etaW > 0.006 && ele1_eRegrInput_phiW<0.08 && ele1_eRegrInput_sigietaieta_bC1>0.006 && ele1_eRegrInput_sigiphiiphi_bC1>0.008  && abs(ele1_eRegrInput_Deta_bC_sC)<0.004 && abs(ele1_eRegrInput_Dphi_bC_sC)<0.04 && abs(ele1_eRegrInput_seedbC_etacry)<0.6 && abs(ele1_eRegrInput_seedbC_phicry)<0.6 && ele1_scE/ele1_scERaw<1.2 && ele1_tkP/ele1_E_true<1.8 && ele1_tkP/ele1_E_true>0.2"; // for example: TCut mycut = "abs(var1)<0.5 && abs(var2-0.5)<1";


// tell the factory to use all remaining events in the trees after training for testing:
		factory->PrepareTrainingAndTestTree( mycut,
		                                     "nTrain_Regression=2500000:nTest_Regression=2500000:SplitMode=Random:NormMode=NumEvents:!V" );

		TString Name = Form("weight_%s_%s_P_W", RegionOfTraining.c_str(), UseMethodFlag.c_str());
		(TMVA::gConfig().GetIONames()).fWeightFileDir = Name;
	}

	if(RegionOfTraining == "EE") {

		factory->AddVariable( "ele1_scE/ele1_scERaw" , 'F');
		factory->AddVariable( "ele1_eRegrInput_nPV", 'F');
		factory->AddVariable( "ele1_eRegrInput_r9", 'F');
		factory->AddVariable( "ele1_fbrem", 'F');
		factory->AddVariable( "ele1_eta", 'F');
		factory->AddVariable( "ele1_DphiIn", 'F');
		factory->AddVariable( "ele1_DetaIn", 'F');
		factory->AddVariable( "ele1_sigmaIetaIeta", 'F');

		factory->AddVariable( "ele1_eRegrInput_etaW", 'F');
		factory->AddVariable( "ele1_eRegrInput_phiW", 'F');

		factory->AddVariable( "ele1_dxy_PV", 'F');
		factory->AddVariable( "ele1_dz_PV", 'F');
		factory->AddVariable( "ele1_sigmaP/ele1_tkP", 'F');


// You can add so-called "Spectator variables", which are not used in the MVA training,
// but will appear in the final "TestTree" produced by TMVA. This TestTree will contain the
// input variables, the response values of all trained MVAs, and the spectator variables
// factory->AddSpectator( "spec1:=var1*2",  "Spectator 1", "units", 'F' );
// factory->AddSpectator( "spec2:=var1*3",  "Spectator 2", "units", 'F' );
// Add the variable carrying the regression target

//  factory->AddTarget("ele1_scE/ele1_E_true" );
		factory->AddTarget("ele1_tkP/ele1_E_true" );

// It is also possible to declare additional targets for multi-dimensional regression, ie:
// -- factory->AddTarget( "fvalue2" );
// BUT: this is currently ONLY implemented for MLP

// global event weights per tree (see below for setting event-wise weights)
		Double_t regWeight  = 1.0;

// You can add an arbitrary number of regression trees
		factory->AddRegressionTree( treeDATA, regWeight );

// This would set individual event weights (the variables defined in the
// expression need to exist in the original TTree)
// factory->SetWeightExpression( "var1", "Regression" );
//  TCut mycut = "ele1_isEB==0 && ele1_sigmaP/ele1_tkP<0.4 && ele1_fbrem>0 && abs(ele1_dxy_PV)<0.05 && abs(ele1_dz_PV)<0.05 &&(ele1_scE/ele1_E_true)<1.4 && (ele1_scE/ele1_E_true)>0.3";
		TCut mycut = "ele1_isEB==0 && ele1_sigmaP/ele1_tkP<0.4 && ele1_fbrem>0 && abs(ele1_dxy_PV)<0.05 && abs(ele1_dz_PV)<0.05 && (ele1_tkP/ele1_E_true)<1.6";

// for example: TCut mycut = "abs(var1)<0.5 &&
// tell the factory to use all remaining events in the trees after training for testing:
		factory->PrepareTrainingAndTestTree( mycut,
		                                     "nTrain_Regression=3000000:nTest_Regression=3000000:SplitMode=Random:NormMode=NumEvents:!V" );

		TString Name = Form("weight_%s_%s_P_W", RegionOfTraining.c_str(), UseMethodFlag.c_str());
		(TMVA::gConfig().GetIONames()).fWeightFileDir = Name;

	}
// Apply additional cuts on the signal and background samples (can be different)

//  // If no numbers of events are given, half of the events in the tree are used
// for training, and the other half for testing:
//    factory->PrepareTrainingAndTestTree( mycut, "SplitMode=random:!V" );

// ---- Book MVA methods
//
// please lookup the various method configuration options in the corresponding cxx files, eg:
// src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html
// it is possible to preset ranges in the option string in which the cut optimisation should be done:
// "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable

// PDE - RS method
	if (Use["PDERS"])
		factory->BookMethod( TMVA::Types::kPDERS, "PDERS", "!H:!V:Normthree=T:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=40:NEventsMax=60:VarTransform=None" );
	// And the options strings for the MinMax and RMS methods, respectively:
	//      "!H:!V:VolumeRangeMode=MinMax:DeltaFrac=0.2:KernelEstimator=Gauss:GaussSigma=0.3" );
	//      "!H:!V:VolumeRangeMode=RMS:DeltaFrac=3:KernelEstimator=Gauss:GaussSigma=0.3" );

	if (Use["PDEFoam"])
		factory->BookMethod( TMVA::Types::kPDEFoam, "PDEFoam", "!H:!V:MultiTargetRegression=F:TargetSelection=Mpv:TailCut=0.001:VolFrac=0.3:nActiveCells=500:nSampl=2000:nBin=5:Compress=T:Kernel=None:Nmin=10:VarTransform=None" );

// K-Nearest Neighbour classifier (KNN)
	if (Use["KNN"])
		factory->BookMethod( TMVA::Types::kKNN, "KNN", "nkNN=20:ScaleFrac=0.8:SigmaFact=1.0:Kernel=Gaus:UseKernel=F:UseWeight=T:!Trim" );

// Linear discriminant
	if (Use["LD"])  factory->BookMethod( TMVA::Types::kLD, "LD", "!H:!V:VarTransform=G,U,D" );

// Function discrimination analysis (FDA) -- test of various fitters - the recommended one is Minuit (or GA or SA)
	if (Use["FDA_MC"])
		factory->BookMethod( TMVA::Types::kFDA, "FDA_MC",
		                     "!H:!V:Formula=(0)+(1)*x0+(2)*x1:ParRanges=(-100,100);(-100,100);(-100,100):FitMethod=MC:SampleSize=100000:Sigma=0.1:VarTransform=D" );

	if (Use["FDA_GA"]) // can also use Simulated Annealing (SA) algorithm (see Cuts_SA options) .. the formula of this example is good for parabolas
		factory->BookMethod( TMVA::Types::kFDA, "FDA_GA",
		                     "!H:!V:Formula=(0)+(1)*x0+(2)*x1:ParRanges=(-100,100);(-100,100);(-100,100):FitMethod=GA:PopSize=100:Cycles=3:Steps=30:Trim=True:SaveBestGen=1:VarTransform=Norm" );

	if (Use["FDA_MT"])
		factory->BookMethod( TMVA::Types::kFDA, "FDA_MT",
		                     "!H:!V:Formula=(0)+(1)*x0+(2)*x1:ParRanges=(-100,100);(-100,100);(-100,100);(-10,10):FitMethod=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=2:UseImprove:UseMinos:SetBatch" );

	if (Use["FDA_GAMT"])
		factory->BookMethod( TMVA::Types::kFDA, "FDA_GAMT",
		                     "!H:!V:Formula=(0)+(1)*x0+(2)*x1:ParRanges=(-100,100);(-100,100);(-100,100):FitMethod=GA:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:Cycles=1:PopSize=5:Steps=5:Trim" );

	// Neural network (MLP)
	if (Use["MLP"])
//       factory->BookMethod( TMVA::Types::kMLP, "MLP", "!H:!V:VarTransform=Norm:NeuronType=tanh:NCycles=20000:HiddenLayers=N+20:TestRate=6:TrainingMethod=BFGS:Sampling=0.3:SamplingEpoch=0.8:ConvergenceImprove=1e-6:ConvergenceTests=15:!UseRegulator" );
//         factory->BookMethod( TMVA::Types::kMLP, "MLP", "!H:!V:VarTransform=Norm:NeuronType=tanh:NCycles=200:HiddenLayers=N+20:TestRate=6:TrainingMethod=BFGS:Sampling=0.3:SamplingEpoch=0.8:ConvergenceImprove=1e-6:ConvergenceTests=15:!UseRegulator" );
// 	factory->BookMethod( TMVA::Types::kMLP, "MLP", "!H:!V:VarTransform=Norm:NeuronType=tanh:NCycles=400:HiddenLayers=N+10:TestRate=6:TrainingMethod=BFGS:Sampling=0.3:SamplingEpoch=0.8:ConvergenceImprove=1e-6:ConvergenceTests=15" );
// 	factory->BookMethod( TMVA::Types::kMLP, "MLP", "!H:!V:VarTransform=N:NeuronType=tanh:NCycles=200:HiddenLayers=N+10:TestRate=6:TrainingMethod=BFGS:Sampling=0.3:SamplingEpoch=0.8:ConvergenceImprove=1e-6:ConvergenceTests=15" );
// 	factory->BookMethod( TMVA::Types::kMLP, "MLP", "!H:!V:VarTransform=G,N:NeuronType=tanh:NCycles=200:HiddenLayers=N+5:TestRate=6:TrainingMethod=BFGS:Sampling=0.3:SamplingEpoch=0.8:ConvergenceImprove=1e-6:ConvergenceTests=15" );
		factory->BookMethod( TMVA::Types::kMLP, "MLP", "!H:!V:NeuronType=tanh:NCycles=250:HiddenLayers=N+5:TrainingMethod=BFGS:Sampling=0.3:SamplingEpoch=0.8:ConvergenceImprove=1e-6:ConvergenceTests=15:TestRate=10");

	// Support Vector Machine
	if (Use["SVM"])
		factory->BookMethod( TMVA::Types::kSVM, "SVM", "Gamma=0.25:Tol=0.001:VarTransform=N" );
//     factory->BookMethod( TMVA::Types::kSVM, "SVM", "Gamma=0.25:Tol=0.001:VarTransform=N,G" );

	// Boosted Decision Trees
	if (Use["BDT"])
//      factory->BookMethod( TMVA::Types::kBDT, "BDT","!H:!V:NTrees=100:nEventsMin=5:BoostType=AdaBoostR2:SeparationType=RegressionVariance:nCuts=20:PruneMethod=CostComplexity:PruneStrength=30" );
//         factory->BookMethod( TMVA::Types::kBDT, "BDT","!H:!V:NTrees=200:nEventsMin=5:BoostType=AdaBoostR2:SeparationType=RegressionVariance:PruneMethod=CostComplexity:PruneStrength=30" );
//         factory->BookMethod( TMVA::Types::kBDT, "BDT","!H:!V:NTrees=300:nEventsMin=5:BoostType=AdaBoostR2:SeparationType=RegressionVariance:PruneMethod=CostComplexity:PruneStrength=30" );
//  	factory->BookMethod( TMVA::Types::kBDT, "BDT","!H:!V:NTrees=100:nEventsMin=5:BoostType=AdaBoostR2:SeparationType=RegressionVariance:PruneMethod=CostComplexity:PruneStrength=30" );
		factory->BookMethod( TMVA::Types::kBDT, "BDT", "!H:!V:NTrees=100:nEventsMin=20:BoostType=AdaBoostR2:SeparationType=RegressionVariance:PruneMethod=CostComplexity:PruneStrength=30");

	if (Use["BDTG"])
//      factory->BookMethod( TMVA::Types::kBDT, "BDTG","!H:!V:NTrees=2000::BoostType=Grad:Shrinkage=0.1:UseBaggedGrad:GradBaggingFraction=0.5:nCuts=20:MaxDepth=3:NNodesMax=15" );
		factory->BookMethod( TMVA::Types::kBDT, "BDTG", "!H:!V:NTrees=1000::BoostType=Grad:Shrinkage=0.1:UseBaggedGrad:GradBaggingFraction=0.5:MaxDepth=5:NNodesMax=25:PruneMethod=CostComplexity:PruneStrength=30");
	// --------------------------------------------------------------------------------------------------
	// ---- Now you can tell the factory to train, test, and evaluate the MVAs

// Train MVAs using the set of training events
	factory->TrainAllMethods();

// ---- Evaluate all MVAs using the set of test events
	factory->TestAllMethods();

// ----- Evaluate and compare performance of all configured MVAs
	factory->EvaluateAllMethods();

// --------------------------------------------------------------

// Save the output
	outputFile->Close();

	std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
	std::cout << "==> TMVARegression is done!" << std::endl;

	delete factory;

// Launch the GUI for the root macros
//  if (!gROOT->IsBatch()) TMVARegGui( outputFileName.c_str() );

	return 0;
}
