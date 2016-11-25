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

#include "../interface/ntpleUtils.h"

#include "FWCore/ParameterSet/interface/ProcessDesc.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

//#if not defined(__CINT__) || defined(__MAKECINT__)
#include "TMVA/Tools.h"
#include "TMVA/Factory.h"
#include "TMVA/Reader.h"
//#endif

#include "/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.00/x86_64-slc5-gcc46-opt/root/tmva/test/TMVARegGui.C"


int main(int argc, char**argv)
{

	if(argc != 2) {
		std::cerr << " >>>>> analysis.cpp::usage: " << argv[0] << " configFileName" << std::endl ;
		return 1;
	}

	std::string configFileName = argv[1];
	boost::shared_ptr<edm::ParameterSet> parameterSet = edm::readConfig(configFileName);
	edm::ParameterSet Options = parameterSet -> getParameter<edm::ParameterSet>("AddRegressionWeight");
	parameterSet.reset();

	std::string treeNameDATA  = Options.getParameter<std::string>("treeNameDATA");
	std::string UseMethodFlag  = Options.getParameter<std::string>("UseMethodFlag");

	std::string FilemethodXML_EB_E  = Options.getParameter<std::string>("FilemethodXML_EB_E");
	std::string FilemethodXML_EB_P  = Options.getParameter<std::string>("FilemethodXML_EB_P");

	std::string FilemethodXML_EE_E  = Options.getParameter<std::string>("FilemethodXML_EE_E");
	std::string FilemethodXML_EE_P  = Options.getParameter<std::string>("FilemethodXML_EE_P");

	std::string RegionOfTraining  = Options.getParameter<std::string>("RegionOfTraining");
	std::string inputFile =  Options.getParameter<std::string>("inputFile");

	bool useW = Options.getParameter<bool>("useW");
	bool isMC = Options.getParameter<bool>("isMC");


	TFile *File = new TFile(inputFile.c_str(), "UPDATE");

	TTree* treeDATA = (TTree*) File->Get(treeNameDATA.c_str());


	float input_variables_1[1000];
	float input_variables_2[1000];


	double MVA_ValueZ_1, MVA_ValueZ_2, MVA_ValueW_1, MVA_ValueW_2;

	float targetW_1, targetZ_1, targetW_2, targetZ_2;

	TBranch *weightBranchW_1 = new TBranch();
	TBranch *weightBranchW_2 = new TBranch();
	TBranch *weightBranchZ_1 = new TBranch();
	TBranch *weightBranchZ_2 = new TBranch();
	TBranch *btargetW_1 = new TBranch();
	TBranch *btargetZ_1 = new TBranch();
	TBranch *btargetW_2 = new TBranch();
	TBranch *btargetZ_2 = new TBranch();



	if(RegionOfTraining == "EB") {


		TMVA::Reader *TMVAreader_1 = new TMVA::Reader( "!Color:!Silent" );
		TMVA::Reader *TMVAreader_2 = new TMVA::Reader( "!Color:!Silent" );

		int ele1_isEB;
		float ele1_eRegrInput_nPV, ele1_eRegrInput_r9, ele1_fbrem, ele1_eta, ele1_DphiIn, ele1_DetaIn, ele1_sigmaIetaIeta, ele1_E_true, ele1_scE, ele1_tkP, ele1_eRegrInput_etaW, ele1_eRegrInput_phiW, ele1_scERaw;


		TMVAreader_1->AddVariable("ele1_scE/ele1_scERaw", &input_variables_1[0]);
		TMVAreader_1->AddVariable("ele1_eRegrInput_nPV", &input_variables_1[1]);
		TMVAreader_1->AddVariable("ele1_eRegrInput_r9", &input_variables_1[2]);
		TMVAreader_1->AddVariable("ele1_fbrem", &input_variables_1[3]);
		TMVAreader_1->AddVariable("ele1_eta", &input_variables_1[4]);
		TMVAreader_1->AddVariable("ele1_DphiIn", &input_variables_1[5]);
		TMVAreader_1->AddVariable("ele1_DetaIn", &input_variables_1[6]);
		TMVAreader_1->AddVariable("ele1_sigmaIetaIeta",  &input_variables_1[7]);
		TMVAreader_1->AddVariable("ele1_eRegrInput_etaW", &input_variables_1[8]);
		TMVAreader_1->AddVariable("ele1_eRegrInput_phiW",  &input_variables_1[9]);

		TMVAreader_2->AddVariable("ele1_scE/ele1_scERaw", &input_variables_2[0]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_nPV", &input_variables_2[1]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_r9", &input_variables_2[2]);
		TMVAreader_2->AddVariable("ele1_fbrem", &input_variables_2[3]);
		TMVAreader_2->AddVariable("ele1_eta", &input_variables_2[4]);
		TMVAreader_2->AddVariable("ele1_DphiIn", &input_variables_2[5]);
		TMVAreader_2->AddVariable("ele1_DetaIn", &input_variables_2[6]);
		TMVAreader_2->AddVariable("ele1_sigmaIetaIeta",  &input_variables_2[7]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_etaW", &input_variables_2[8]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_phiW",  &input_variables_2[9]);


		treeDATA -> SetBranchAddress("ele1_isEB",  &ele1_isEB);
		treeDATA -> SetBranchAddress("ele1_scE",  &ele1_scE);
		treeDATA -> SetBranchAddress("ele1_scERaw",  &ele1_scERaw);
		treeDATA -> SetBranchAddress("ele1_tkP",  &ele1_tkP);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_nPV",  &ele1_eRegrInput_nPV);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_r9",  &ele1_eRegrInput_r9);
		treeDATA -> SetBranchAddress("ele1_fbrem",  &ele1_fbrem);
		treeDATA -> SetBranchAddress("ele1_eta",  &ele1_eta);
		treeDATA -> SetBranchAddress("ele1_DphiIn",  &ele1_DphiIn);
		treeDATA -> SetBranchAddress("ele1_DetaIn",  &ele1_DetaIn);
		treeDATA -> SetBranchAddress("ele1_sigmaIetaIeta",  &ele1_sigmaIetaIeta);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_etaW",  &ele1_eRegrInput_etaW);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_phiW",  &ele1_eRegrInput_phiW);

		if(isMC == true) treeDATA -> SetBranchAddress("ele1_E_true",  &ele1_E_true);


		float ele1_eRegrInput_bCE_Over_sCE, ele1_eRegrInput_sigietaieta_bC1, ele1_eRegrInput_sigiphiiphi_bC1, ele1_eRegrInput_sigietaiphi_bC1, ele1_eRegrInput_e3x3_Over_bCE, ele1_eRegrInput_Deta_bC_sC, ele1_eRegrInput_Dphi_bC_sC, ele1_eRegrInput_bEMax_Over_bCE;


		TMVAreader_1->AddVariable("ele1_eRegrInput_bCE_Over_sCE", &input_variables_1[10]);
		TMVAreader_1->AddVariable("ele1_eRegrInput_sigietaieta_bC1", &input_variables_1[11]);
		TMVAreader_1->AddVariable("ele1_eRegrInput_sigiphiiphi_bC1", &input_variables_1[12]);
		TMVAreader_1->AddVariable("ele1_eRegrInput_sigietaiphi_bC1", &input_variables_1[13]);
		TMVAreader_1->AddVariable("ele1_eRegrInput_e3x3_Over_bCE",  &input_variables_1[14]);
		TMVAreader_1->AddVariable("ele1_eRegrInput_Deta_bC_sC",         &input_variables_1[15]);
		TMVAreader_1->AddVariable("ele1_eRegrInput_Dphi_bC_sC",         &input_variables_1[16]);
		TMVAreader_1->AddVariable("ele1_eRegrInput_bEMax_Over_bCE",         &input_variables_1[17]);

		TMVAreader_2->AddVariable("ele1_eRegrInput_bCE_Over_sCE", &input_variables_2[10]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_sigietaieta_bC1", &input_variables_2[11]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_sigiphiiphi_bC1", &input_variables_2[12]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_sigietaiphi_bC1", &input_variables_2[13]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_e3x3_Over_bCE",  &input_variables_2[14]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_Deta_bC_sC",         &input_variables_2[15]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_Dphi_bC_sC",         &input_variables_2[16]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_bEMax_Over_bCE",         &input_variables_2[17]);


		treeDATA -> SetBranchAddress("ele1_eRegrInput_bCE_Over_sCE",  &ele1_eRegrInput_bCE_Over_sCE);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_sigietaieta_bC1",  &ele1_eRegrInput_sigietaieta_bC1);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_sigiphiiphi_bC1",  &ele1_eRegrInput_sigiphiiphi_bC1);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_sigietaiphi_bC1",  &ele1_eRegrInput_sigietaiphi_bC1);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_e3x3_Over_bCE",  &ele1_eRegrInput_e3x3_Over_bCE);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_Deta_bC_sC",  &ele1_eRegrInput_Deta_bC_sC);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_Dphi_bC_sC",  &ele1_eRegrInput_Dphi_bC_sC);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_bEMax_Over_bCE",  &ele1_eRegrInput_bEMax_Over_bCE);


		float ele1_dxy_PV, ele1_dz_PV, ele1_sigmaP;

		TMVAreader_1->AddVariable( "ele1_dxy_PV" , &input_variables_1[18]);
		TMVAreader_1->AddVariable( "ele1_dz_PV" , &input_variables_1[19]);
		TMVAreader_1->AddVariable( "ele1_sigmaP/ele1_tkP" , &input_variables_1[20]);

		TMVAreader_2->AddVariable( "ele1_dxy_PV" , &input_variables_2[18]);
		TMVAreader_2->AddVariable( "ele1_dz_PV" , &input_variables_2[19]);
		TMVAreader_2->AddVariable( "ele1_sigmaP/ele1_tkP" , &input_variables_2[20]);


		treeDATA -> SetBranchAddress("ele1_dxy_PV",  &ele1_dxy_PV);
		treeDATA -> SetBranchAddress("ele1_dz_PV",  &ele1_dz_PV);
		treeDATA -> SetBranchAddress("ele1_sigmaP",  &ele1_sigmaP);

		float  ele1_eRegrInput_bCELow_Over_sCE, ele1_eRegrInput_sigietaieta_bCLow, ele1_eRegrInput_sigiphiiphi_bCLow, ele1_eRegrInput_sigietaiphi_bCLow, ele1_eRegrInput_e3x3_Over_bCELow, ele1_eRegrInput_Deta_bCLow_sC, ele1_eRegrInput_Dphi_bCLow_sC;

		TMVAreader_1->AddVariable("ele1_eRegrInput_bCELow_Over_sCE", &input_variables_1[21]);
		TMVAreader_1->AddVariable("ele1_eRegrInput_e3x3_Over_bCELow", &input_variables_1[22]);
		TMVAreader_1->AddVariable("ele1_eRegrInput_Deta_bCLow_sC", &input_variables_1[23]);
		TMVAreader_1->AddVariable("ele1_eRegrInput_Dphi_bCLow_sC", &input_variables_1[24]);


		TMVAreader_2->AddVariable("ele1_eRegrInput_bCELow_Over_sCE", &input_variables_2[21]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_e3x3_Over_bCELow", &input_variables_2[22]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_Deta_bCLow_sC", &input_variables_2[23]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_Dphi_bCLow_sC", &input_variables_2[24]);

		treeDATA -> SetBranchAddress("ele1_eRegrInput_bCELow_Over_sCE",  &ele1_eRegrInput_bCELow_Over_sCE);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_e3x3_Over_bCELow", &ele1_eRegrInput_e3x3_Over_bCELow);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_sigietaieta_bCLow",  &ele1_eRegrInput_sigietaieta_bCLow);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_sigiphiiphi_bCLow",  &ele1_eRegrInput_sigiphiiphi_bCLow);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_sigietaiphi_bCLow",  &ele1_eRegrInput_sigietaiphi_bCLow);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_Deta_bCLow_sC",  &ele1_eRegrInput_Deta_bCLow_sC);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_Dphi_bCLow_sC",  &ele1_eRegrInput_Dphi_bCLow_sC);

		float ele1_eRegrInput_seedbC_etacry, ele1_eRegrInput_seedbC_phicry;

		TMVAreader_1->AddVariable("ele1_eRegrInput_seedbC_etacry", &input_variables_1[25]);
		TMVAreader_1->AddVariable("ele1_eRegrInput_seedbC_phicry", &input_variables_1[26]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_seedbC_etacry", &input_variables_2[25]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_seedbC_phicry", &input_variables_2[26]);


		treeDATA -> SetBranchAddress("ele1_eRegrInput_seedbC_etacry",  &ele1_eRegrInput_seedbC_etacry);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_seedbC_phicry",  &ele1_eRegrInput_seedbC_phicry);


///==== add new branches ====
		TString weightfile_1 = FilemethodXML_EB_E;
		TString weightfile_2 = FilemethodXML_EB_P;

		std::cout << " UseMethodFlag " << UseMethodFlag << " weightfile" << weightfile_1 << " : " << weightfile_1 << std::endl;

		TMVAreader_1->BookMVA( UseMethodFlag, weightfile_1 );
		TMVAreader_2->BookMVA( UseMethodFlag, weightfile_2 );

		TString methodName4TreeW_1  = Form ("%s_weightEB_W_1", UseMethodFlag.c_str());
		TString methodName4TreeZ_1  = Form ("%s_weightEB_Z_1", UseMethodFlag.c_str());
		TString methodName4TreeW_1_ = Form ("%s_weightEB_W_1/D", UseMethodFlag.c_str());
		TString methodName4TreeZ_1_ = Form ("%s_weightEB_Z_1/D", UseMethodFlag.c_str());

		TString methodName4TreeW_2  = Form ("%s_weightEB_W_2", UseMethodFlag.c_str());
		TString methodName4TreeZ_2  = Form ("%s_weightEB_Z_2", UseMethodFlag.c_str());
		TString methodName4TreeW_2_ = Form ("%s_weightEB_W_2/D", UseMethodFlag.c_str());
		TString methodName4TreeZ_2_ = Form ("%s_weightEB_Z_2/D", UseMethodFlag.c_str());

		TString methodtargetW_1;
		TString methodtargetZ_1;
		TString methodtargetW_1_;
		TString methodtargetZ_1_;
		TString methodtargetW_2;
		TString methodtargetZ_2;
		TString methodtargetW_2_;
		TString methodtargetZ_2_;

		if(isMC == true) {

			methodtargetW_1 = Form ("%s_targetEB_W_1", UseMethodFlag.c_str());
			methodtargetZ_1 = Form ("%s_targetEB_Z_1", UseMethodFlag.c_str());
			methodtargetW_1_ = Form ("%s_targetEB_W_1/F", UseMethodFlag.c_str());
			methodtargetZ_1_ = Form ("%s_targetEB_Z_1/F", UseMethodFlag.c_str());

			methodtargetW_2 = Form ("%s_targetEB_W_2", UseMethodFlag.c_str());
			methodtargetZ_2 = Form ("%s_targetEB_Z_2", UseMethodFlag.c_str());
			methodtargetW_2_ = Form ("%s_targetEB_W_2/F", UseMethodFlag.c_str());
			methodtargetZ_2_ = Form ("%s_targetEB_Z_2/F", UseMethodFlag.c_str());

		}

		if(useW) {
			weightBranchW_1 = treeDATA->Branch(methodName4TreeW_1, &MVA_ValueW_1, methodName4TreeW_1_);
			weightBranchW_2 = treeDATA->Branch(methodName4TreeW_2, &MVA_ValueW_2, methodName4TreeW_2_);
			if(isMC == true) {
				btargetW_1 =  treeDATA->Branch(methodtargetW_1, &targetW_1, methodtargetW_1);
				btargetW_2 =  treeDATA->Branch(methodtargetW_2, &targetW_2, methodtargetW_2);
			}
		}

		if(!useW) {
			weightBranchZ_1 = treeDATA->Branch(methodName4TreeZ_1, &MVA_ValueZ_1, methodName4TreeZ_1);
			weightBranchZ_2 = treeDATA->Branch(methodName4TreeZ_2, &MVA_ValueZ_2, methodName4TreeZ_2);

			if(isMC == true) {
				btargetZ_1 =  treeDATA->Branch(methodtargetZ_1, &targetZ_1, methodtargetZ_1);
				btargetZ_2 =  treeDATA->Branch(methodtargetZ_2, &targetZ_2, methodtargetZ_2);
			}
		}

///==== loop ====
		Long64_t nentries = treeDATA->GetEntries();

		for (Long64_t iEntry = 0; iEntry < nentries; iEntry++) {
			if((iEntry % 100000) == 0) std::cout << ">>>>> analysis::GetEntry " << iEntry << " : " << nentries << std::endl;

			treeDATA->GetEntry(iEntry);

			if(ele1_isEB == 1 && ele1_sigmaP / ele1_tkP < 0.4 && ele1_fbrem > 0. && abs(ele1_dxy_PV) < 0.05 && abs(ele1_dz_PV) < 0.05 &&
			        ele1_eRegrInput_etaW > 0.006 && ele1_eRegrInput_phiW < 0.08 && ele1_eRegrInput_sigietaieta_bC1 > 0.006 && ele1_eRegrInput_sigiphiiphi_bC1 > 0.008  && abs(ele1_eRegrInput_Deta_bC_sC) < 0.004 &&
			        abs(ele1_eRegrInput_Dphi_bC_sC) < 0.04 && abs(ele1_eRegrInput_seedbC_etacry) < 0.6 && abs(ele1_eRegrInput_seedbC_phicry) < 0.6 && ele1_scE / ele1_scERaw < 1.2) {

				input_variables_1[0]  = static_cast<float>(ele1_scE / ele1_scERaw);
				input_variables_1[1]  = static_cast<float>(ele1_eRegrInput_nPV);
				input_variables_1[2]  = static_cast<float>(ele1_eRegrInput_r9);
				input_variables_1[3]  = static_cast<float>(ele1_fbrem);
				input_variables_1[4]  = static_cast<float>(ele1_eta);
				input_variables_1[5]  = static_cast<float>(ele1_DphiIn);
				input_variables_1[6]  = static_cast<float>(ele1_DetaIn);
				input_variables_1[7]  = static_cast<float>(ele1_sigmaIetaIeta);
				input_variables_1[8]  = static_cast<float>(ele1_eRegrInput_etaW);
				input_variables_1[9]  = static_cast<float>(ele1_eRegrInput_phiW);
				input_variables_1[10]  = static_cast<float>(ele1_eRegrInput_bCE_Over_sCE);
				input_variables_1[11]  = static_cast<float>(ele1_eRegrInput_sigietaieta_bC1);
				input_variables_1[12]  = static_cast<float>(ele1_eRegrInput_sigiphiiphi_bC1);
				input_variables_1[13]  = static_cast<float>(ele1_eRegrInput_sigietaiphi_bC1);
				input_variables_1[14]  = static_cast<float>(ele1_eRegrInput_e3x3_Over_bCE);
				input_variables_1[15]  = static_cast<float>(ele1_eRegrInput_Deta_bC_sC);
				input_variables_1[16]  = static_cast<float>(ele1_eRegrInput_Dphi_bC_sC);
				input_variables_1[17]  = static_cast<float>(ele1_eRegrInput_bEMax_Over_bCE);
				input_variables_1[18]  = static_cast<float>(ele1_dxy_PV);
				input_variables_1[19]  = static_cast<float>(ele1_dz_PV);
				input_variables_1[29]  = static_cast<float>(ele1_sigmaP / ele1_tkP);
				input_variables_1[21]  = static_cast<float>(ele1_eRegrInput_bCELow_Over_sCE);
				input_variables_1[22]  = static_cast<float>(ele1_eRegrInput_e3x3_Over_bCELow);
				input_variables_1[23]  = static_cast<float>(ele1_eRegrInput_Deta_bCLow_sC);
				input_variables_1[24]  = static_cast<float>(ele1_eRegrInput_Dphi_bCLow_sC);
				input_variables_1[25]  = static_cast<float>(ele1_eRegrInput_seedbC_etacry);
				input_variables_1[26]  = static_cast<float>(ele1_eRegrInput_seedbC_phicry);


				input_variables_2[0]  = static_cast<float>(ele1_scE / ele1_scERaw);
				input_variables_2[1]  = static_cast<float>(ele1_eRegrInput_nPV);
				input_variables_2[2]  = static_cast<float>(ele1_eRegrInput_r9);
				input_variables_2[3]  = static_cast<float>(ele1_fbrem);
				input_variables_2[4]  = static_cast<float>(ele1_eta);
				input_variables_2[5]  = static_cast<float>(ele1_DphiIn);
				input_variables_2[6]  = static_cast<float>(ele1_DetaIn);
				input_variables_2[7]  = static_cast<float>(ele1_sigmaIetaIeta);
				input_variables_2[8]  = static_cast<float>(ele1_eRegrInput_etaW);
				input_variables_2[9]  = static_cast<float>(ele1_eRegrInput_phiW);
				input_variables_2[10]  = static_cast<float>(ele1_eRegrInput_bCE_Over_sCE);
				input_variables_2[11]  = static_cast<float>(ele1_eRegrInput_sigietaieta_bC1);
				input_variables_2[12]  = static_cast<float>(ele1_eRegrInput_sigiphiiphi_bC1);
				input_variables_2[13]  = static_cast<float>(ele1_eRegrInput_sigietaiphi_bC1);
				input_variables_2[14]  = static_cast<float>(ele1_eRegrInput_e3x3_Over_bCE);
				input_variables_2[15]  = static_cast<float>(ele1_eRegrInput_Deta_bC_sC);
				input_variables_2[16]  = static_cast<float>(ele1_eRegrInput_Dphi_bC_sC);
				input_variables_2[17]  = static_cast<float>(ele1_eRegrInput_bEMax_Over_bCE);
				input_variables_2[18]  = static_cast<float>(ele1_dxy_PV);
				input_variables_2[19]  = static_cast<float>(ele1_dz_PV);
				input_variables_2[29]  = static_cast<float>(ele1_sigmaP / ele1_tkP);
				input_variables_2[21]  = static_cast<float>(ele1_eRegrInput_bCELow_Over_sCE);
				input_variables_2[22]  = static_cast<float>(ele1_eRegrInput_e3x3_Over_bCELow);
				input_variables_2[23]  = static_cast<float>(ele1_eRegrInput_Deta_bCLow_sC);
				input_variables_2[24]  = static_cast<float>(ele1_eRegrInput_Dphi_bCLow_sC);
				input_variables_2[25]  = static_cast<float>(ele1_eRegrInput_seedbC_etacry);
				input_variables_2[26]  = static_cast<float>(ele1_eRegrInput_seedbC_phicry);


				if(useW) {

					MVA_ValueW_1 = TMVAreader_1->EvaluateRegression(0, UseMethodFlag);
					MVA_ValueW_2 = TMVAreader_2->EvaluateRegression(0, UseMethodFlag);


					if(isMC == true) {
						targetW_1 = ele1_scE / ele1_E_true;
						targetW_2 = ele1_tkP / ele1_E_true;
						btargetW_1 -> Fill();
						btargetW_2 -> Fill();
					}

					weightBranchW_1 -> Fill();
					weightBranchW_2 -> Fill();

				}

				if(!useW) {

					MVA_ValueZ_1 = TMVAreader_1->EvaluateRegression(0, UseMethodFlag);
					MVA_ValueZ_2 = TMVAreader_2->EvaluateRegression(0, UseMethodFlag);

					if(isMC == true) {
						targetZ_1 = ele1_scE / ele1_E_true;
						targetZ_2 = ele1_tkP / ele1_E_true;
						btargetZ_1 -> Fill();
						btargetZ_2 -> Fill();
					}

					weightBranchZ_1 -> Fill();
					weightBranchZ_2 -> Fill();
				}

			} else {
				if( useW == true) {
					MVA_ValueW_1 = -99. ;
					MVA_ValueW_2 = -99.;
					if(isMC == true) {
						targetW_1 = -99. ;
						targetW_2 = -99. ;
						btargetW_1 -> Fill();
						btargetW_2 -> Fill();
					}

					weightBranchW_1 -> Fill();
					weightBranchW_2 -> Fill();
				}

				if( useW == false) {
					MVA_ValueZ_1 = -99.;
					MVA_ValueZ_2 = -99.;
					if(isMC == true) {
						targetZ_1 = -99. ;
						targetZ_2 = -99. ;
						btargetZ_1 -> Fill();
						btargetZ_2 -> Fill();
					}
					weightBranchZ_1 -> Fill();
					weightBranchZ_2 -> Fill();
				}

			}
		}
	}

	if(RegionOfTraining == "EE") {

		TMVA::Reader *TMVAreader_1 = new TMVA::Reader( "!Color:!Silent" );
		TMVA::Reader *TMVAreader_2 = new TMVA::Reader( "!Color:!Silent" );


		int ele1_isEB;
		float ele1_eRegrInput_nPV, ele1_eRegrInput_r9, ele1_fbrem, ele1_eta, ele1_DphiIn, ele1_DetaIn, ele1_sigmaIetaIeta, ele1_E_true, ele1_scE, ele1_tkP, ele1_eRegrInput_etaW, ele1_eRegrInput_phiW, ele1_scERaw;


		TMVAreader_1->AddVariable("ele1_scE/ele1_scERaw", &input_variables_1[0]);
		TMVAreader_1->AddVariable("ele1_eRegrInput_nPV", &input_variables_1[1]);
		TMVAreader_1->AddVariable("ele1_eRegrInput_r9", &input_variables_1[2]);
		TMVAreader_1->AddVariable("ele1_fbrem", &input_variables_1[3]);
		TMVAreader_1->AddVariable("ele1_eta", &input_variables_1[4]);
		TMVAreader_1->AddVariable("ele1_DphiIn", &input_variables_1[5]);
		TMVAreader_1->AddVariable("ele1_DetaIn", &input_variables_1[6]);
		TMVAreader_1->AddVariable("ele1_sigmaIetaIeta",  &input_variables_1[7]);
		TMVAreader_1->AddVariable("ele1_eRegrInput_etaW", &input_variables_1[8]);
		TMVAreader_1->AddVariable("ele1_eRegrInput_phiW",  &input_variables_1[9]);


		TMVAreader_2->AddVariable("ele1_scE/ele1_scERaw", &input_variables_2[0]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_nPV", &input_variables_2[1]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_r9", &input_variables_2[2]);
		TMVAreader_2->AddVariable("ele1_fbrem", &input_variables_2[3]);
		TMVAreader_2->AddVariable("ele1_eta", &input_variables_2[4]);
		TMVAreader_2->AddVariable("ele1_DphiIn", &input_variables_2[5]);
		TMVAreader_2->AddVariable("ele1_DetaIn", &input_variables_2[6]);
		TMVAreader_2->AddVariable("ele1_sigmaIetaIeta",  &input_variables_2[7]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_etaW", &input_variables_2[8]);
		TMVAreader_2->AddVariable("ele1_eRegrInput_phiW",  &input_variables_2[9]);


		treeDATA -> SetBranchAddress("ele1_isEB",  &ele1_isEB);

		if(isMC == true) treeDATA -> SetBranchAddress("ele1_E_true",  &ele1_E_true);
		treeDATA -> SetBranchAddress("ele1_scE",  &ele1_scE);
		treeDATA -> SetBranchAddress("ele1_scERaw",  &ele1_scERaw);
		treeDATA -> SetBranchAddress("ele1_tkP",  &ele1_tkP);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_nPV",  &ele1_eRegrInput_nPV);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_r9",  &ele1_eRegrInput_r9);
		treeDATA -> SetBranchAddress("ele1_fbrem",  &ele1_fbrem);
		treeDATA -> SetBranchAddress("ele1_eta",  &ele1_eta);
		treeDATA -> SetBranchAddress("ele1_DphiIn",  &ele1_DphiIn);
		treeDATA -> SetBranchAddress("ele1_DetaIn",  &ele1_DetaIn);
		treeDATA -> SetBranchAddress("ele1_sigmaIetaIeta",  &ele1_sigmaIetaIeta);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_etaW",  &ele1_eRegrInput_etaW);
		treeDATA -> SetBranchAddress("ele1_eRegrInput_phiW",  &ele1_eRegrInput_phiW);


		float ele1_dxy_PV, ele1_dz_PV, ele1_sigmaP;

		TMVAreader_1->AddVariable( "ele1_dxy_PV" , &input_variables_1[18]);
		TMVAreader_1->AddVariable( "ele1_dz_PV" , &input_variables_1[19]);
		TMVAreader_1->AddVariable( "ele1_sigmaP/ele1_tkP" , &input_variables_1[20]);

		TMVAreader_2->AddVariable( "ele1_dxy_PV" , &input_variables_2[18]);
		TMVAreader_2->AddVariable( "ele1_dz_PV" , &input_variables_2[19]);
		TMVAreader_2->AddVariable( "ele1_sigmaP/ele1_tkP" , &input_variables_2[20]);

		treeDATA -> SetBranchAddress("ele1_dxy_PV",  &ele1_dxy_PV);
		treeDATA -> SetBranchAddress("ele1_dz_PV",  &ele1_dz_PV);
		treeDATA -> SetBranchAddress("ele1_sigmaP",  &ele1_sigmaP);


///==== add new branches ====
		TString weightfile_1 = FilemethodXML_EE_E;
		TString weightfile_2 = FilemethodXML_EE_P;

		std::cout << " UseMethodFlag " << UseMethodFlag << " weightfile" << weightfile_1 << " : " << weightfile_2 << std::endl;

		TMVAreader_1->BookMVA( UseMethodFlag, weightfile_1 );
		TMVAreader_2->BookMVA( UseMethodFlag, weightfile_2 );

		TString methodName4TreeW_1  = Form ("%s_weightEE_W_1", UseMethodFlag.c_str());
		TString methodName4TreeZ_1  = Form ("%s_weightEE_Z_1", UseMethodFlag.c_str());
		TString methodName4TreeW_1_ = Form ("%s_weightEE_W_1/D", UseMethodFlag.c_str());
		TString methodName4TreeZ_1_ = Form ("%s_weightEE_Z_1/D", UseMethodFlag.c_str());

		TString methodName4TreeW_2  = Form ("%s_weightEE_W_2", UseMethodFlag.c_str());
		TString methodName4TreeZ_2  = Form ("%s_weightEE_Z_2", UseMethodFlag.c_str());
		TString methodName4TreeW_2_ = Form ("%s_weightEE_W_2/D", UseMethodFlag.c_str());
		TString methodName4TreeZ_2_ = Form ("%s_weightEE_Z_2/D", UseMethodFlag.c_str());

		TString methodtargetW_1;
		TString methodtargetZ_1;
		TString methodtargetW_1_;
		TString methodtargetZ_1_;
		TString methodtargetW_2;
		TString methodtargetZ_2;
		TString methodtargetW_2_;
		TString methodtargetZ_2_;

		if(isMC == true) {
			methodtargetW_1 = Form ("%s_targetEE_W_1", UseMethodFlag.c_str());
			methodtargetZ_1 = Form ("%s_targetEE_Z_1", UseMethodFlag.c_str());
			methodtargetW_1_ = Form ("%s_targetEE_W_1/F", UseMethodFlag.c_str());
			methodtargetZ_1_ = Form ("%s_targetEE_Z_1/F", UseMethodFlag.c_str());

			methodtargetW_2 = Form ("%s_targetEE_W_2", UseMethodFlag.c_str());
			methodtargetZ_2 = Form ("%s_targetEE_Z_2", UseMethodFlag.c_str());
			methodtargetW_2_ = Form ("%s_targetEE_W_2/F", UseMethodFlag.c_str());
			methodtargetZ_2_ = Form ("%s_targetEE_Z_2/F", UseMethodFlag.c_str());
		}

		if(useW) {
			weightBranchW_1 = treeDATA->Branch(methodName4TreeW_1, &MVA_ValueW_1, methodName4TreeW_1_);
			weightBranchW_2 = treeDATA->Branch(methodName4TreeW_2, &MVA_ValueW_2, methodName4TreeW_2_);
			if(isMC == true) {
				btargetW_1 =  treeDATA->Branch(methodtargetW_1, &targetW_1, methodtargetW_1);
				btargetW_2 =  treeDATA->Branch(methodtargetW_2, &targetW_2, methodtargetW_2);
			}
		}

		if(!useW) {
			weightBranchZ_1 = treeDATA->Branch(methodName4TreeZ_1, &MVA_ValueZ_1, methodName4TreeZ_1);
			weightBranchZ_2 = treeDATA->Branch(methodName4TreeZ_2, &MVA_ValueZ_2, methodName4TreeZ_2);
			if(isMC == true) {
				btargetZ_1 =  treeDATA->Branch(methodtargetZ_1, &targetZ_1, methodtargetZ_1);
				btargetZ_2 =  treeDATA->Branch(methodtargetZ_2, &targetZ_2, methodtargetZ_2);
			}
		}

///==== loop ====
		Long64_t nentries = treeDATA->GetEntries();

		for (Long64_t iEntry = 0; iEntry < nentries; iEntry++) {
			if((iEntry % 100000) == 0) std::cout << ">>>>> analysis::GetEntry " << iEntry << " : " << nentries << std::endl;

			treeDATA->GetEntry(iEntry);

			if(ele1_isEB == 0 && ele1_sigmaP / ele1_tkP < 0.4 && ele1_fbrem > 0 && abs(ele1_dxy_PV) < 0.05 && abs(ele1_dz_PV) < 0.05) {

				input_variables_1[0]  = static_cast<float>(ele1_scE / ele1_scERaw);
				input_variables_1[1]  = static_cast<float>(ele1_eRegrInput_nPV);
				input_variables_1[2]  = static_cast<float>(ele1_eRegrInput_r9);
				input_variables_1[3]  = static_cast<float>(ele1_fbrem);
				input_variables_1[4]  = static_cast<float>(ele1_eta);
				input_variables_1[5]  = static_cast<float>(ele1_DphiIn);
				input_variables_1[6]  = static_cast<float>(ele1_DetaIn);
				input_variables_1[7]  = static_cast<float>(ele1_sigmaIetaIeta);
				input_variables_1[8]  = static_cast<float>(ele1_eRegrInput_etaW);
				input_variables_1[9]  = static_cast<float>(ele1_eRegrInput_phiW);
				input_variables_1[10]  = static_cast<float>(ele1_dxy_PV);
				input_variables_1[11]  = static_cast<float>(ele1_dz_PV);
				input_variables_1[12]  = static_cast<float>(ele1_sigmaP / ele1_tkP);

				input_variables_2[0]  = static_cast<float>(ele1_scE / ele1_scERaw);
				input_variables_2[1]  = static_cast<float>(ele1_eRegrInput_nPV);
				input_variables_2[2]  = static_cast<float>(ele1_eRegrInput_r9);
				input_variables_2[3]  = static_cast<float>(ele1_fbrem);
				input_variables_2[4]  = static_cast<float>(ele1_eta);
				input_variables_2[5]  = static_cast<float>(ele1_DphiIn);
				input_variables_2[6]  = static_cast<float>(ele1_DetaIn);
				input_variables_2[7]  = static_cast<float>(ele1_sigmaIetaIeta);
				input_variables_2[8]  = static_cast<float>(ele1_eRegrInput_etaW);
				input_variables_2[9]  = static_cast<float>(ele1_eRegrInput_phiW);
				input_variables_2[10]  = static_cast<float>(ele1_dxy_PV);
				input_variables_2[11]  = static_cast<float>(ele1_dz_PV);
				input_variables_2[12]  = static_cast<float>(ele1_sigmaP / ele1_tkP);


				if(useW) {

					MVA_ValueW_1 = TMVAreader_1->EvaluateRegression(0, UseMethodFlag) ;
					MVA_ValueW_2 = TMVAreader_2->EvaluateRegression(0, UseMethodFlag) ;

					if(isMC == true) {
						targetW_1 = ele1_scE / ele1_E_true;
						targetW_2 = ele1_tkP / ele1_E_true;

						btargetW_1 -> Fill();
						btargetW_2 -> Fill();
					}
					weightBranchW_1 -> Fill();
					weightBranchW_2 -> Fill();

				}

				if(!useW) {

					MVA_ValueZ_1 = TMVAreader_1->EvaluateRegression(0, UseMethodFlag) ;
					MVA_ValueZ_2 = TMVAreader_2->EvaluateRegression(0, UseMethodFlag) ;

					if(isMC == true) {
						targetZ_1 = ele1_scE / ele1_E_true;
						targetZ_2 = ele1_tkP / ele1_E_true;

						btargetZ_1 -> Fill();
						btargetZ_2 -> Fill();
					}

					weightBranchZ_1 -> Fill();
					weightBranchZ_2 -> Fill();
				}

			} else {
				if( useW == true) {
					MVA_ValueW_1 = -99. ;
					MVA_ValueW_2 = -99.;
					if(isMC == true) {
						targetW_1 = -99. ;
						targetW_2 = -99. ;
						btargetW_1 -> Fill();
						btargetW_2 -> Fill();
					}
					weightBranchW_1 -> Fill();
					weightBranchW_2 -> Fill();
				}

				if( useW == false) {
					MVA_ValueZ_1 = -99.;
					MVA_ValueZ_2 = -99.;
					if(isMC == true) {
						targetZ_1 = -99. ;
						targetZ_2 = -99. ;
						btargetZ_1 -> Fill();
						btargetZ_2 -> Fill();
					}
					weightBranchZ_1 -> Fill();
					weightBranchZ_2 -> Fill();
				}

			}
		}

	}

// save only the new version of the tree
	treeDATA->Write("", TObject::kOverwrite);

	return 0;

}




