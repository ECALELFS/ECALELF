#include "../interface/TEndcapRings.h"
#include "../interface/ntpleUtils.h"
#include "../interface/treeReader.h"
#include "../interface/CalibrationUtils.h"
#include "../CommonTools/histoFunc.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <ctime>
#include <map>
#include <algorithm>
#include <math.h>
#include <vector>

#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TProfile.h"
#include "TGraphErrors.h"
#include "TPaveStats.h"
#include "TLegend.h"
#include "TChain.h"
#include "TVirtualFitter.h"
#include "TMath.h"

#include "FWCore/ParameterSet/interface/ProcessDesc.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

using namespace std;


bool IsEtaGap(float eta)
{
	float feta = fabs(eta);
	if( fabs(feta - 0 ) < 3 ) return true;
	if( fabs(feta - 25) < 3 ) return true;
	if( fabs(feta - 45) < 3 ) return true;
	if( fabs(feta - 65) < 3 ) return true;
	if( fabs(feta - 85) < 3 ) return true;
	return false;
}






//**************  MAIN PROGRAM **************************************************************
int main(int argc, char** argv)
{
	// Acquisition from cfg file
	if(argc != 2) {
		std::cerr << ">>>>> CalibrationMomentum.cpp::usage:   " << argv[0] << " configFileName" << std::endl;
		return 1;
	}


	std::string configFileName = argv[1];
	std::shared_ptr<edm::ParameterSet> parameterSet = edm::readConfig(configFileName);
	edm::ParameterSet Options = parameterSet -> getParameter<edm::ParameterSet>("Options");
	//  parameterSet.reset();

	std::string TreeName = "NULL";
	if(Options.existsAs<std::string>("TreeName"))
		TreeName = Options.getParameter<std::string>("TreeName");

	std::string infileDATA = "NULL";
	if(Options.existsAs<std::string>("infileDATA"))
		infileDATA = Options.getParameter<std::string>("infileDATA");

	std::string infileMC = "NULL";
	if(Options.existsAs<std::string>("infileMC"))
		infileMC = Options.getParameter<std::string>("infileMC");

	std::string WeightforMC = "NULL";
	if(Options.existsAs<std::string>("WeightforMC"))
		WeightforMC = Options.getParameter<std::string>("WeightforMC");

	std::string typeEB = "NULL";
	if(Options.existsAs<std::string>("typeEB"))
		typeEB = Options.getParameter<std::string>("typeEB");

	std::string typeEE = "NULL";
	if(Options.existsAs<std::string>("typeEE"))
		typeEE = Options.getParameter<std::string>("typeEE");

	int nPhiBinsEB = 1;
	if(Options.existsAs<int>("nPhiBinsEB"))
		nPhiBinsEB = Options.getParameter<int>("nPhiBinsEB");

	int nPhiBinsEE = 1;
	if(Options.existsAs<int>("nPhiBinsEE"))
		nPhiBinsEE = Options.getParameter<int>("nPhiBinsEE");

	int nPhiBinsTempEB = 1;
	if(Options.existsAs<int>("nPhiBinsTempEB"))
		nPhiBinsTempEB = Options.getParameter<int>("nPhiBinsTempEB");

	int nPhiBinsTempEE = 1;
	if(Options.existsAs<int>("nPhiBinsTempEE"))
		nPhiBinsTempEE = Options.getParameter<int>("nPhiBinsTempEE");

	int rebinEB = 1;
	if(Options.existsAs<int>("rebinEB"))
		rebinEB = Options.getParameter<int>("rebinEB");

	int rebinEE = 1;
	if(Options.existsAs<int>("rebinEE"))
		rebinEE = Options.getParameter<int>("rebinEE");

	int nRegionsEB = 1;
	if(Options.existsAs<int>("nRegionsEB"))
		nRegionsEB = Options.getParameter<int>("nRegionsEB");

	int nRegionsEE = 1;
	if(Options.existsAs<int>("nRegionsEE"))
		nRegionsEE = Options.getParameter<int>("nRegionsEE");

	std::string outputFile = "NULL";
	if(Options.existsAs<std::string>("outputFile"))
		outputFile = Options.getParameter<std::string>("outputFile");

	bool usePUweights = false;
	if(Options.existsAs<bool>("usePUweights"))
		usePUweights = Options.getParameter<bool>("usePUweights");

	cout << " Basic Configuration " << endl;
	cout << " Tree Name = " << TreeName << endl;
	cout << " infileDATA = " << infileDATA << endl;
	cout << " infileMC = " << infileMC << endl;
	cout << " WeightforMC = " << WeightforMC << endl;
	cout << " nRegionsEB = " << nRegionsEB << endl;
	cout << " nRegionsEE = " << nRegionsEE << endl;
	cout << " nPhiBinsEB = " << nPhiBinsEB << endl;
	cout << " nPhiBinsEE = " << nPhiBinsEE << endl;
	cout << " nPhiBinsTempEB = " << nPhiBinsTempEB << endl;
	cout << " nPhiBinsTempEE = " << nPhiBinsTempEE << endl;
	cout << " rebinEB = " << rebinEB << endl;
	cout << " rebinEE = " << rebinEE << endl;
	cout << " usePUweights = " << usePUweights << endl;

	cout << "Making calibration plots for Momentum scale studies " << endl;



	//---- variables for selection
	float etaMax  = 2.5;
	float eta2Max = 2.5;

	//--- weights for MC
	TFile weightsFile (WeightforMC.c_str(), "READ");
	TH1F* hweights = (TH1F*)weightsFile.Get("pileup");

	float w[100];
	for (int ibin = 1; ibin < hweights->GetNbinsX() + 1; ibin++) {
		w[ibin - 1] = hweights->GetBinContent(ibin); // bin 1 --> nvtx = 0
	}
	weightsFile.Close();

	//histos to get the bin in phi given the electron phi
	TH1F* hPhiBinEB = new TH1F("hphiEB", "hphiEB", nPhiBinsEB, -1.*TMath::Pi(), 1.*TMath::Pi());
	TH1F* hPhiBinEE = new TH1F("hphiEE", "hphiEE", nPhiBinsEE, -1.*TMath::Pi(), 1.*TMath::Pi());

	//----- NTUPLES--------------------
	TChain *ntu_DA = new TChain(TreeName.c_str());
	TChain *ntu_MC = new TChain(TreeName.c_str());

	if(!FillChain(*ntu_DA, infileDATA.c_str())) return 1;
	if(!FillChain(*ntu_MC, infileMC.c_str()))   return 1;

	std::cout << "     DATA: " << ntu_DA->GetEntries() << " entries in Data sample" << std::endl;
	std::cout << "     MC  : " << ntu_MC->GetEntries() << " entries in  MC  sample" << std::endl;

	// observables
	//  int isW;
	float invMass_SC;
	float etaSCEle[3], phiSCEle[3];
	float etaEle[3], phiEle[3];
	float energySCEle[3], rawEnergySCEle[3], R9Ele[3];
	int chargeEle[3];
	float pAtVtxGsfEle[3];
	float seedXSCEle[3], seedYSCEle[3];
	int NPU[1];
	float PtEle[3];
	int ele1_iz, ele2_iz;

	// Set branch addresses for Data
	ntu_DA->SetBranchStatus("*", 0);
	//  ntu_DA->SetBranchStatus("isW", 1);                 ntu_DA->SetBranchAddress("isW", &isW);
	ntu_DA->SetBranchStatus("etaEle", 1);
	ntu_DA->SetBranchAddress("etaEle", &etaEle);
	ntu_DA->SetBranchStatus("phiEle", 1);
	ntu_DA->SetBranchAddress("phiEle", &phiEle);
	ntu_DA->SetBranchStatus("etaSCEle", 1);
	ntu_DA->SetBranchAddress("etaSCEle", &etaSCEle);
	ntu_DA->SetBranchStatus("phiSCEle", 1);
	ntu_DA->SetBranchAddress("phiSCEle", &phiSCEle);
	ntu_DA->SetBranchStatus("energySCEle", 1);
	ntu_DA->SetBranchAddress("energySCEle", &energySCEle);
	ntu_DA->SetBranchStatus("rawEnergySCEle", 1);
	ntu_DA->SetBranchAddress("rawEnergySCEle", &rawEnergySCEle);
	ntu_DA->SetBranchStatus("PtEle", 1);
	ntu_DA->SetBranchAddress("PtEle", &PtEle);
	ntu_DA->SetBranchStatus("invMass_SC", 1);
	ntu_DA->SetBranchAddress("invMass_SC", &invMass_SC);
	ntu_DA->SetBranchStatus("chargeEle", 1);
	ntu_DA->SetBranchAddress("chargeEle", &chargeEle);
	ntu_DA->SetBranchStatus("pAtVtxGsfEle", 1);
	ntu_DA->SetBranchAddress("pAtVtxGsfEle", &pAtVtxGsfEle);
	ntu_DA->SetBranchStatus("seedXSCEle", 1);
	ntu_DA->SetBranchAddress("seedXSCEle", &seedXSCEle);
	ntu_DA->SetBranchStatus("seedYSCEle", 1);
	ntu_DA->SetBranchAddress("seedYSCEle", &seedYSCEle);


	// Set branch addresses for MC
	ntu_MC->SetBranchStatus("*", 0);
	//  ntu_MC->SetBranchStatus("isW", 1);                  ntu_MC->SetBranchAddress("isW", &isW);
	ntu_MC->SetBranchStatus("etaEle", 1);
	ntu_MC->SetBranchAddress("etaEle", &etaEle);
	ntu_MC->SetBranchStatus("phiEle", 1);
	ntu_MC->SetBranchAddress("phiEle", &phiEle);
	ntu_MC->SetBranchStatus("etaSCEle", 1);
	ntu_MC->SetBranchAddress("etaSCEle", &etaSCEle);
	ntu_MC->SetBranchStatus("phiSCEle", 1);
	ntu_MC->SetBranchAddress("phiSCEle", &phiSCEle);
	ntu_MC->SetBranchStatus("energySCEle", 1);
	ntu_MC->SetBranchAddress("energySCEle", &energySCEle);
	ntu_MC->SetBranchStatus("rawEnergySCEle", 1);
	ntu_MC->SetBranchAddress("rawEnergySCEle", &rawEnergySCEle);
	ntu_MC->SetBranchStatus("PtEle", 1);
	ntu_MC->SetBranchAddress("PtEle", &PtEle);
	ntu_MC->SetBranchStatus("invMass_SC", 1);
	ntu_MC->SetBranchAddress("invMass_SC", &invMass_SC);
	ntu_MC->SetBranchStatus("chargeEle", 1);
	ntu_MC->SetBranchAddress("chargeEle", &chargeEle);
	ntu_MC->SetBranchStatus("pAtVtxGsfEle", 1);
	ntu_MC->SetBranchAddress("pAtVtxGsfEle", &pAtVtxGsfEle);
	ntu_MC->SetBranchStatus("seedXSCEle", 1);
	ntu_MC->SetBranchAddress("seedXSCEle", &seedXSCEle);
	ntu_MC->SetBranchStatus("seedYSCEle", 1);
	ntu_MC->SetBranchAddress("seedYSCEle", &seedYSCEle);

	if(usePUweights) {
		ntu_MC->SetBranchStatus("NPU", 1);
		ntu_MC->SetBranchAddress("NPU", &NPU);
	}



	// histogram definition in EB and fit functions
	std::vector<std::vector<TH1F*> > h_Phi_EB(nPhiBinsEB); // used to map iEta (as defined for Barrel and Endcap geom) into eta
	std::vector<std::vector<TH1F*> > h_EoP_EB(nPhiBinsEB);
	std::vector<std::vector<TH1F*> > h_EoC_EB(nPhiBinsEB);
	std::vector<std::vector<TF1*> > f_EoP_EB(nPhiBinsEB);
	std::vector<std::vector<TF1*> > f_EoC_EB(nPhiBinsEB);

	// histogram definition in EE and fit functions
	std::vector<std::vector<TH1F*> > h_Phi_EE(nPhiBinsEE); // used to map iEta (as defined for Barrel and Endcap geom) into eta
	std::vector<std::vector<TH1F*> > h_EoP_EE(nPhiBinsEE);
	std::vector<std::vector<TH1F*> > h_EoC_EE(nPhiBinsEE);
	std::vector<std::vector<TF1*> > f_EoP_EE(nPhiBinsEE);
	std::vector<std::vector<TF1*> > f_EoC_EE(nPhiBinsEE);



	// Initializate histos in EB
	std::cout << ">>> Initialize EB histos" << std::endl;
	for(int i = 0; i < nPhiBinsEB; ++i) {
		for(int j = 0; j < nRegionsEB; ++j) {
			TString histoName;
			histoName = Form("EB_EoP_%d_%d", i, j);
			TH1F* temp = new TH1F (histoName, histoName, 2200, 0.2, 1.6);
			temp->Sumw2();
			temp->SetFillColor(kRed + 2);
			temp->SetLineColor(kRed + 2);
			temp->SetFillStyle(3004);
			(h_EoP_EB.at(i)).push_back(temp);

			histoName = Form("EB_EoC_%d_%d", i, j);
			temp = new TH1F(histoName, histoName, 2200, 0.2, 1.6);
			temp->Sumw2();
			temp->SetFillColor(kGreen + 2);
			temp->SetLineColor(kGreen + 2);
			temp->SetFillStyle(3004);
			(h_EoC_EB.at(i)).push_back(temp);

			histoName = Form("EB_Phi_%d_%d", i, j);
			temp = new TH1F(histoName, histoName, 360, 0., 360.);
			(h_Phi_EB.at(i)).push_back(temp);
		}
	}

	// Initializate histos in EE
	std::cout << ">>> Initialize EE histos" << std::endl;
	for(int i = 0; i < nPhiBinsEE; ++i) {
		for(int j = 0; j < nRegionsEE; ++j) {
			TString histoName;
			histoName = Form("EE_EoP_%d_%d", i, j);
			TH1F* temp = new TH1F (histoName, histoName, 2200, 0., 2.);
			temp->Sumw2();
			temp->SetFillColor(kRed + 2);
			temp->SetLineColor(kRed + 2);
			temp->SetFillStyle(3004);
			(h_EoP_EE.at(i)).push_back(temp);

			histoName = Form("EE_EoC_%d_%d", i, j);
			temp = new TH1F(histoName, histoName, 2200, 0., 2.);
			temp->Sumw2();
			temp->SetFillColor(kGreen + 2);
			temp->SetLineColor(kGreen + 2);
			temp->SetFillStyle(3004);
			(h_EoC_EE.at(i)).push_back(temp);

			histoName = Form("EE_Phi_%d_%d", i, j);
			temp = new TH1F(histoName, histoName, 360, 0., 360.);
			(h_Phi_EE.at(i)).push_back(temp);
		}
	}



	// Template in EE and EB
	std::vector<std::vector<TH1F*> > h_template_EB(nPhiBinsTempEB);
	std::vector<std::vector<TH1F*> > h_template_EE(nPhiBinsTempEE);

	std::cout << ">>> Initialize EB template" << std::endl;
	for(int mod = 0; mod < nPhiBinsTempEB; ++mod) {
		for(int j = 0; j < nRegionsEB; ++j) {
			TString histoName;
			histoName = Form("EB_template_%d_%d", mod, j);
			TH1F* temp = new TH1F(histoName, "", 2200, 0.2, 1.6);
			(h_template_EB.at(mod)).push_back(temp);
		}
	}

	std::cout << ">>> Initialize EE template" << std::endl;
	for(int mod = 0; mod < nPhiBinsTempEE; ++mod) {
		for(int j = 0; j < nRegionsEE; ++j) {
			TString histoName;
			histoName = Form("EE_template_%d_%d", mod, j);
			TH1F* temp = new TH1F(histoName, "", 2200, 0., 2.);
			(h_template_EE.at(mod)).push_back(temp);
		}
	}


	TH1F** h_phi_data_EB = new TH1F*[nRegionsEB];
	TH1F** h_phi_mc_EB   = new TH1F*[nRegionsEB];
	TH1F** h_phi_data_EE = new TH1F*[nRegionsEE];
	TH1F** h_phi_mc_EE   = new TH1F*[nRegionsEE];

	for(int index = 0; index < nRegionsEB; ++index) {
		TString name;
		name = Form("EB_h_phi_data_%d", index);
		h_phi_data_EB[index] = new TH1F(name, "h_phi_data", 100, -TMath::Pi(), TMath::Pi());
		name = Form("EB_h_phi_mc_%d", index);
		h_phi_mc_EB[index] = new TH1F(name, "h_phi_mc", 100, -TMath::Pi(), TMath::Pi());
	}

	for(int index = 0; index < nRegionsEE; ++index) {
		TString name;
		name = Form("EE_h_phi_data_%d", index);
		h_phi_data_EE[index] = new TH1F(name, "h_phi_data", 100, -TMath::Pi(), TMath::Pi());
		name = Form("EE_h_phi_mc_%d", index);
		h_phi_mc_EE[index] =  new TH1F(name, "h_phi_mc", 100, -TMath::Pi(), TMath::Pi());
	}

	TH1F* h_et_data = new TH1F("h_et_data", "h_et_data", 100, 0., 100.);
	TH1F* h_et_mc   = new TH1F("h_et_mc",  "h_et_mc",  100, 0., 100.);



	// Initialize endcap geometry
	TEndcapRings *eRings = new TEndcapRings();

	// Map for conversion (ix,iy) into Eta for EE
	TH2F * mapConversionEEp = new TH2F ("mapConversionEEp", "mapConversionEEp", 101, 1, 101, 101, 1, 101);
	TH2F * mapConversionEEm = new TH2F ("mapConversionEEm", "mapConversionEEm", 101, 1, 101, 101, 1, 101);

	for(int ix = 0; ix < mapConversionEEp->GetNbinsX(); ix++)
		for(int iy = 0; iy < mapConversionEEp->GetNbinsY(); iy++) {
			mapConversionEEp->SetBinContent(ix + 1, iy + 1, 0);
			mapConversionEEm->SetBinContent(ix + 1, iy + 1, 0);
		}



	// fill MC templates
	std::vector<int> refIdEB;
	refIdEB.assign(nPhiBinsEB, 0);

	std::vector<int> refIdEE;
	refIdEE.assign(nPhiBinsEE, 0);

	for(int iphi = 0; iphi < nPhiBinsEB; ++iphi) {
		float phi = hPhiBinEB->GetBinCenter(iphi + 1);

		phi = 2.*TMath::Pi() + phi + TMath::Pi() * 10. / 180.;
		phi -= int(phi / 2. / TMath::Pi()) * 2.*TMath::Pi();

		int modPhi = int(phi / (2.*TMath::Pi() / nPhiBinsTempEB));
		if( modPhi == nPhiBinsTempEB ) modPhi = 0;
		refIdEB.at(iphi) = modPhi;
	}

	for(int iphi = 0; iphi < nPhiBinsEE; ++iphi) {
		float phi = hPhiBinEE->GetBinCenter(iphi + 1);

		phi = 2.*TMath::Pi() + phi + TMath::Pi() * 10. / 180.;
		phi -= int(phi / 2. / TMath::Pi()) * 2.*TMath::Pi();

		int modPhi = int(phi / (2.*TMath::Pi() / nPhiBinsTempEE));
		if( modPhi == nPhiBinsTempEE ) modPhi = 0;
		refIdEE.at(iphi) = modPhi;
	}






	//**************************** loop on MC, make refernce and fit dist

	float var = 0.;
	std::cout << "Loop in MC events " << endl;
	for(int entry = 0; entry < ntu_MC->GetEntries(); ++entry) {
		if( entry % 10000 == 0 ) std::cout << "reading saved entry " << entry << "\r" << std::flush;
		//if( entry > 1000 ) break;

		ntu_MC->GetEntry(entry);

		//    if( isW == 1 )               continue;
		if( fabs(etaSCEle[0])  > etaMax )  continue;
		if( fabs(etaSCEle[1]) > eta2Max ) continue;
		if( PtEle[0]  < 20. ) continue;
		if( PtEle[1] < 20. ) continue;

		if ( fabs(etaEle[0]) < 1.479) ele1_iz = 0;
		else if (etaEle[0] > 0) ele1_iz = 1;
		else ele1_iz = -1;

		if ( fabs(etaEle[1]) < 1.479) ele2_iz = 0;
		else if (etaEle[1] > 0) ele2_iz = 1;
		else ele2_iz = -1;

		//--- PU weights
		float ww = 1.;
		if( usePUweights ) ww *= w[NPU[0]];

		//--- set the mass for ele1
		//    var = ( invMass_SC * sqrt(pAtVtxGsfEle[0]/energySCEle[0]) * sqrt(scEneReg2/energySCEle[1]) ) / 91.19;
		var = ( invMass_SC * sqrt(pAtVtxGsfEle[0] / energySCEle[0]) ) / 91.19;
		// simulate e+/e- asymmetry
		//if( chargeEle[0] > 0 ) ww *= 1.*(6/5);
		//else             ww *= 1.*(4/5);

		// MC - BARREL - ele1
		if(  ele1_iz == 0 ) {
			// fill MC templates
			int modPhi = int(seedYSCEle[0] / (360. / nPhiBinsTempEB));
			if( modPhi == nPhiBinsTempEB ) modPhi = 0;

			int regionId = templIndexEB(typeEB, etaEle[0], chargeEle[0], R9Ele[0]);
			if( regionId == -1 ) continue;

			(h_template_EB.at(modPhi)).at(regionId) -> Fill(var * var, ww);


			// fill MC histos in eta bins
			int PhibinEB = hPhiBinEB->FindBin(phiEle[0]) - 1;
			if( PhibinEB == nPhiBinsEB ) PhibinEB = 0;

			(h_EoP_EB.at(PhibinEB)).at(regionId) -> Fill(var * var, ww); // This is MC
			h_phi_mc_EB[regionId] -> Fill(phiSCEle[0], ww);
		}

		// MC - ENDCAP - ele1
		else {
			int iphi = eRings->GetEndcapIphi(seedXSCEle[0], seedYSCEle[0], ele1_iz);

			if( ele1_iz ==  1 )mapConversionEEp -> SetBinContent(seedXSCEle[0], seedYSCEle[0], etaSCEle[0]);
			if( ele1_iz == -1 )mapConversionEEm -> SetBinContent(seedXSCEle[0], seedYSCEle[0], etaSCEle[0]);


			// fill MC templates
			int modPhi = int (iphi / (360. / nPhiBinsTempEE));
			if( modPhi == nPhiBinsTempEE ) modPhi = 0;

			int regionId =  templIndexEE(typeEE, etaEle[0], chargeEle[0], R9Ele[0]);
			if( regionId == -1 ) continue;

			(h_template_EE.at(modPhi)).at(regionId) -> Fill(var * var, ww);


			// fill MC histos in eta bins
			int PhibinEE = hPhiBinEE->FindBin(phiEle[0]) - 1;
			if( PhibinEE == nPhiBinsEE ) PhibinEE = 0;

			(h_EoP_EE.at(PhibinEE)).at(regionId) -> Fill(var * var, ww); // This is MC
			h_phi_mc_EE[regionId] -> Fill(phiSCEle[0], ww);
		}



		//--- set the mass for ele2
		//    var = ( invMass_SC * sqrt(pAtVtxGsfEle[1]/energySCEle[1]) * sqrt(scEneReg/energySCEle[0]) ) / 91.19;
		var = ( invMass_SC * sqrt(pAtVtxGsfEle[1] / energySCEle[1]) ) / 91.19;
		// simulate e+/e- asymmetry
		//if( chargeEle[0] > 0 ) ww *= 1.*(6/5);
		//else             ww *= 1.*(4/5);

		// MC - BARREL - ele2
		if( ele2_iz == 0) {
			// fill MC templates
			int modPhi = int (seedYSCEle[1] / (360. / nPhiBinsTempEB));
			if( modPhi == nPhiBinsTempEB ) modPhi = 0;

			int regionId  = templIndexEB(typeEB, etaEle[1], chargeEle[1], R9Ele[1]);
			if(regionId == -1) continue;

			(h_template_EB.at(modPhi)).at(regionId)->Fill(var * var, ww);


			// fill MC histos in eta bins
			int PhibinEB = hPhiBinEB->FindBin(phiEle[1]) - 1;
			if( PhibinEB == nPhiBinsEB ) PhibinEB = 0;

			(h_EoP_EB.at(PhibinEB)).at(regionId) -> Fill(var * var, ww); // This is MC
			h_phi_mc_EB[regionId]->Fill(phiSCEle[1], ww);
		}

		// MC - ENDCAP - ele2
		else {
			if( ele2_iz ==  1 ) mapConversionEEp -> SetBinContent(seedXSCEle[1], seedYSCEle[1], etaSCEle[1]);
			if( ele2_iz == -1 ) mapConversionEEm -> SetBinContent(seedXSCEle[1], seedYSCEle[1], etaSCEle[1]);

			int iphi = eRings->GetEndcapIphi(seedXSCEle[1], seedYSCEle[1], ele2_iz);


			// fill MC templates
			int modPhi = int (iphi / (360. / nPhiBinsTempEE));
			if( modPhi == nPhiBinsTempEE ) modPhi = 0;

			int regionId =  templIndexEE(typeEE, etaEle[1], chargeEle[1], R9Ele[1]);
			if(regionId == -1) continue;

			(h_template_EE.at(modPhi)).at(regionId) ->  Fill(var * var, ww);


			// fill MC histos in eta bins
			int PhibinEE = hPhiBinEE->FindBin(phiEle[1]) - 1;
			if(PhibinEE == nPhiBinsEE) PhibinEE = 0;

			(h_EoP_EE.at(PhibinEE)).at(regionId) -> Fill(var * var, ww); // This is MC
			h_phi_mc_EE[regionId]->Fill(phiSCEle[1], ww);
		}

		h_et_mc ->Fill(PtEle[0], ww);
		h_et_mc ->Fill(PtEle[1], ww);
	}






	//**************************** loop on DATA

	std::cout << "Loop in Data events " << endl;

	for(int entry = 0; entry < ntu_DA->GetEntries(); ++entry) {
		if( entry % 10000 == 0 ) std::cout << "reading saved entry " << entry << "\r" << std::flush;

		ntu_DA->GetEntry(entry);

		//    if( isW == 1 )               continue;
		if( fabs(etaSCEle[0])  > etaMax )  continue;
		if( fabs(etaSCEle[1]) > eta2Max ) continue;
		if( PtEle[0]  < 20. ) continue;
		if( PtEle[1] < 20. ) continue;

		float ww = 1.;

		if ( fabs(etaEle[0]) < 1.479) ele1_iz = 0;
		else if (etaEle[0] > 0) ele1_iz = 1;
		else ele1_iz = -1;

		if ( fabs(etaEle[1]) < 1.479) ele2_iz = 0;
		else if (etaEle[1] > 0) ele2_iz = 1;
		else ele2_iz = -1;


		//--- set the mass for ele1
		//    if( ele1_iz == 0 ) var = ( invMass_SC * sqrt(pAtVtxGsfEle[0]/energySCEle[0]) * sqrt(scEneReg2/energySCEle[1]) ) / 91.19;
		//    else               var = ( invMass_SC * sqrt(pAtVtxGsfEle[0]/energySCEle[0]) * sqrt(scEneReg2/energySCEle[1]) ) / 91.19;
		if( ele1_iz == 0 ) var = ( invMass_SC * sqrt(pAtVtxGsfEle[0] / energySCEle[0]) ) / 91.19;
		else               var = ( invMass_SC * sqrt(pAtVtxGsfEle[0] / energySCEle[0]) ) / 91.19;
		// simulate e+/e- asymmetry
		//if( chargeEle[0] > 0 ) ww *= 1.*(6/5);
		//else             ww *= 1.*(4/5);

		// DATA - BARREL - ele1
		if( ele1_iz == 0 ) {
			int PhibinEB = hPhiBinEB->FindBin(phiEle[0]) - 1;
			if( PhibinEB == nPhiBinsEB ) PhibinEB = 0;

			int regionId = templIndexEB(typeEB, etaEle[0], chargeEle[0], R9Ele[0]);
			if(regionId == -1) continue;

			(h_EoC_EB.at(PhibinEB)).at(regionId) -> Fill(var * var, ww); // This is DATA
			(h_Phi_EB.at(PhibinEB)).at(regionId) -> Fill(phiEle[0]);
			h_phi_data_EB[regionId]->Fill(phiEle[0]);
		}

		// DATA - ENDCAP - ele1
		else {
			if( ele1_iz ==  1 ) mapConversionEEp -> SetBinContent(seedXSCEle[0], seedYSCEle[0], etaSCEle[0]);
			if( ele1_iz == -1 ) mapConversionEEm -> SetBinContent(seedXSCEle[0], seedYSCEle[0], etaSCEle[0]);

			int PhibinEE = hPhiBinEE->FindBin(phiEle[0]) - 1;
			if( PhibinEE == nPhiBinsEE ) PhibinEE = 0;

			int regionId = templIndexEE(typeEE, etaEle[0], chargeEle[0], R9Ele[0]);
			if( regionId == -1 ) continue;

			(h_EoC_EE.at(PhibinEE)).at(regionId) -> Fill(var * var, ww); // This is DATA
			(h_Phi_EE.at(PhibinEE)).at(regionId) -> Fill(phiEle[0]);
			h_phi_data_EE[regionId] -> Fill(phiEle[0]);
		}



		//--- set the mass for ele2
		//    if( ele2_iz == 0 ) var = ( invMass_SC * sqrt(pAtVtxGsfEle[1]/energySCEle[1]) * sqrt(scEneReg/energySCEle[0]) ) / 91.19;
		//    else               var = ( invMass_SC * sqrt(pAtVtxGsfEle[1]/energySCEle[1]) * sqrt(scEneReg/energySCEle[0]) ) / 91.19;
		if( ele2_iz == 0 ) var = ( invMass_SC * sqrt(pAtVtxGsfEle[1] / energySCEle[1]) ) / 91.19;
		else               var = ( invMass_SC * sqrt(pAtVtxGsfEle[1] / energySCEle[1]) ) / 91.19;
		// simulate e+/e- asymmetry
		//if( chargeEle[0]Ele[1] > 0 ) ww *= 1.*(6/5);
		//else              ww *= 1.*(4/5);

		// DATA - BARREL - ele2
		if( ele2_iz == 0 ) {
			int PhibinEB = hPhiBinEB->FindBin(phiEle[1]) - 1;
			if( PhibinEB == nPhiBinsEB ) PhibinEB = 0;

			int regionId = templIndexEB(typeEB, etaEle[1], chargeEle[1], R9Ele[1]);
			if( regionId == -1 ) continue;

			(h_EoC_EB.at(PhibinEB)).at(regionId) -> Fill(var * var, ww); // This is DATA
			(h_Phi_EB.at(PhibinEB)).at(regionId) -> Fill(phiEle[1]);
			h_phi_data_EB[regionId] -> Fill(phiEle[1]);
		} else {
			if( ele2_iz ==  1 ) mapConversionEEp -> SetBinContent(seedXSCEle[1], seedYSCEle[1], etaSCEle[1]);
			if( ele2_iz == -1 ) mapConversionEEm -> SetBinContent(seedXSCEle[1], seedYSCEle[1], etaSCEle[1]);

			int PhibinEE = hPhiBinEE->FindBin(phiEle[1]) - 1;
			if( PhibinEE == nPhiBinsEE ) PhibinEE = 0;

			int regionId = templIndexEE(typeEE, etaEle[1], chargeEle[1], R9Ele[1]);
			if( regionId == -1 ) continue;

			(h_EoC_EE.at(PhibinEE)).at(regionId) -> Fill(var * var, ww); // This is DATA
			(h_Phi_EE.at(PhibinEE)).at(regionId) -> Fill(phiEle[1]);
			h_phi_data_EE[regionId] ->Fill(phiEle[1]);
		}

		h_et_data ->Fill(PtEle[0]);
		h_et_data ->Fill(PtEle[1]);
	}

	std::cout << "End loop: Analyze events " << endl;






	//----------------
	// Initializations

	// initialize TGraphs
	TFile* o = new TFile((outputFile + "_" + typeEB + "_" + typeEE + ".root").c_str(), "RECREATE");

	TGraphErrors** g_EoP_EB = new TGraphErrors*[nRegionsEB];
	TGraphErrors** g_EoC_EB = new TGraphErrors*[nRegionsEB];
	TGraphErrors** g_Rat_EB = new TGraphErrors*[nRegionsEB];

	for(int j = 0; j < nRegionsEB; ++j) {
		g_EoP_EB[j] = new TGraphErrors();
		g_EoC_EB[j] = new TGraphErrors();
		g_Rat_EB[j] = new TGraphErrors();
	}

	TGraphErrors** g_EoP_EE = new TGraphErrors*[nRegionsEB];
	TGraphErrors** g_EoC_EE = new TGraphErrors*[nRegionsEB];
	TGraphErrors** g_Rat_EE = new TGraphErrors*[nRegionsEB];

	for(int j = 0; j < nRegionsEE; ++j) {
		g_EoP_EE[j] = new TGraphErrors();
		g_EoC_EE[j] = new TGraphErrors();
		g_Rat_EE[j] = new TGraphErrors();
	}

	// initialize template functions
	std::vector<std::vector<histoFunc*> > templateHistoFuncEB(nPhiBinsTempEB);
	std::vector<std::vector<histoFunc*> > templateHistoFuncEE(nPhiBinsTempEE);

	for(int mod = 0; mod < nPhiBinsTempEB; ++mod) {
		for(int j = 0; j < nRegionsEB; ++j) {
			(h_template_EB.at(mod)).at(j) -> Rebin(rebinEB);
			(templateHistoFuncEB.at(mod)).push_back( new histoFunc((h_template_EB.at(mod)).at(j)) );
		}
	}

	for(int mod = 0; mod < nPhiBinsTempEE; ++mod) {
		for(int j = 0; j < nRegionsEE; ++j) {
			(h_template_EE.at(mod)).at(j) -> Rebin(rebinEE);
			(templateHistoFuncEE.at(mod)).push_back( new histoFunc((h_template_EE.at(mod)).at(j)) );
		}
	}






	//-------------------
	// Template Fit in EB

	if( typeEB != "none" ) {
		for(int i = 0; i < nPhiBinsEB; ++i) {
			for(int j = 0; j < nRegionsEB; ++j) {
				float flPhi = hPhiBinEB->GetXaxis()->GetBinCenter(i + 1);

				(h_EoP_EB.at(i)).at(j) -> Rebin(rebinEB);
				(h_EoC_EB.at(i)).at(j) -> Rebin(rebinEB);


				// define the fitting function
				// N.B. [0] * ( [1] * f( [1]*(x-[2]) ) )
				char funcName[50];
				sprintf(funcName, "f_EoP_%d_%d_Ref_%d_%d_EB", i, j, refIdEB.at(i), j);
				(f_EoP_EB.at(i)).push_back( new TF1(funcName, (templateHistoFuncEB.at(refIdEB.at(i))).at(j), 0.85, 1.1, 3, "histoFunc") );

				(f_EoP_EB.at(i)).at(j) -> SetParName(0, "Norm");
				(f_EoP_EB.at(i)).at(j) -> SetParName(1, "Scale factor");

				(f_EoP_EB.at(i)).at(j) -> SetLineWidth(1);
				(f_EoP_EB.at(i)).at(j) -> SetLineColor(kRed + 2);
				(f_EoP_EB.at(i)).at(j) -> SetNpx(10000);

				(h_EoP_EB.at(i)).at(j) -> Scale(1 * (h_EoC_EB.at(i)).at(j)->GetEntries() / (h_EoP_EB.at(i)).at(j)->GetEntries());

				// uncorrected
				double xNorm = (h_EoP_EB.at(i)).at(j)->Integral() / (h_template_EB.at(refIdEB.at(i))).at(j)->Integral() *
				               (h_EoP_EB.at(i)).at(j)->GetBinWidth(1) / (h_template_EB.at(refIdEB.at(i))).at(j)->GetBinWidth(1);


				(f_EoP_EB.at(i)).at(j) -> FixParameter(0, xNorm);
				(f_EoP_EB.at(i)).at(j) -> FixParameter(2, 0.);


				std::cout << "***** Fitting MC EB " << flPhi << " (" << i << "," << j << "):   ";
				TFitResultPtr rp;
				int fStatus;
				std::cout << "entries: " << (h_EoP_EB.at(i)).at(j)->GetEntries() << std::endl;
				for(int trial = 0; trial < 10; ++trial) {
					(f_EoP_EB.at(i)).at(j) -> SetParameter(1, 0.99);
					rp = (h_EoP_EB.at(i)).at(j) -> Fit(funcName, "QRWL+");
					fStatus = rp;
					if(fStatus != 4 && (f_EoP_EB.at(i)).at(j)->GetParError(1) != 0. ) {
						std::cout << "fit OK    ";

						double k = (f_EoP_EB.at(i)).at(j)->GetParameter(1);
						double eee = (f_EoP_EB.at(i)).at(j)->GetParError(1);
						g_EoP_EB[j] -> SetPoint(i, flPhi, 1. / k);
						g_EoP_EB[j] -> SetPointError(i, 0., eee / k / k);

						break;
					} else if( trial == 9 ) {
						std::cout << "fit BAD   ";

						g_EoP_EB[j] -> SetPoint(i, flPhi, 1.);
						g_EoP_EB[j] -> SetPointError(i, 0., 0.01);
					}
				}


				//ratio preparation
				float rat = (f_EoP_EB.at(i)).at(j)->GetParameter(1);
				float era = (f_EoP_EB.at(i)).at(j)->GetParError(1);

				xNorm = (h_EoC_EB.at(i)).at(j)->Integral() / (h_template_EB.at(refIdEB.at(i))).at(j)->Integral() *
				        (h_EoC_EB.at(i)).at(j)->GetBinWidth(1) / (h_template_EB.at(refIdEB.at(i))).at(j)->GetBinWidth(1);

				sprintf(funcName, "f_EoC_%d_%d_Ref_%d_%d_EB", i, j, refIdEB.at(i), i);

				(f_EoC_EB.at(i)).push_back( new TF1(funcName, (templateHistoFuncEB.at(refIdEB.at(i))).at(j), 0.85, 1.1, 3, "histoFunc") );

				(f_EoC_EB.at(i)).at(j) -> SetParName(0, "Norm");
				(f_EoC_EB.at(i)).at(j) -> SetParName(1, "Scale factor");

				(f_EoC_EB.at(i)).at(j) -> SetLineWidth(1);
				(f_EoC_EB.at(i)).at(j) -> SetLineColor(kGreen + 2);
				(f_EoC_EB.at(i)).at(j) -> SetNpx(10000);

				(f_EoC_EB.at(i)).at(j) -> FixParameter(0, xNorm);
				(f_EoC_EB.at(i)).at(j) -> FixParameter(2, 0.);


				std::cout << "***** Fitting DATA EB (" << i << "," << j << "):   ";
				for(int trial = 0; trial < 10; ++trial) {
					(f_EoC_EB.at(i)).at(j) -> SetParameter(1, 0.99);
					rp = (h_EoC_EB.at(i)).at(j) -> Fit(funcName, "QR+");
					fStatus = rp;
					if( fStatus != 4 && (f_EoC_EB.at(i)).at(j)->GetParError(1) != 0 ) {
						std::cout << "fit OK    ";

						double k = (f_EoC_EB.at(i)).at(j)->GetParameter(1);
						double eee = (f_EoC_EB.at(i)).at(j)->GetParError(1);
						g_EoC_EB[j] -> SetPoint(i, flPhi, 1. / k);
						g_EoC_EB[j] -> SetPointError(i, 0., eee / k / k);

						break;
					} else if( trial == 9 ) {
						std::cout << "fit BAD   ";

						g_EoC_EB[j] -> SetPoint(i, flPhi, 1.);
						g_EoC_EB[j] -> SetPointError(i, 0., 0.01);
					}
				}


				//ratio finalization
				rat /= (f_EoC_EB.at(i)).at(j)->GetParameter(1);
				era = rat * sqrt(era * era + (f_EoC_EB.at(i)).at(j)->GetParError(1) * (f_EoC_EB.at(i)).at(j)->GetParError(1));

				if(i == 0) g_Rat_EB[j] -> SetPoint(i, 0.,    rat);
				else     g_Rat_EB[j] -> SetPoint(i, flPhi, rat);

				g_Rat_EB[j] -> SetPointError(i,  0. , era);
				g_Rat_EB[j]->SetLineColor(kBlue + 2);

				std::cout << std::endl;
			}
		}
	} else {
		for(int i = 0; i < nPhiBinsEB; ++i) {
			for(int j = 0; j < nRegionsEB; ++j) {
				float flPhi = hPhiBinEB->GetXaxis()->GetBinCenter(i + 1);
				g_EoP_EB[j] -> SetPoint(i, flPhi, 1.);
				g_EoC_EB[j] -> SetPoint(i, flPhi, 1.);
				g_Rat_EB[j] -> SetPoint(i, flPhi, 1.);
			}
		}
	}






	//-------------------
	// Template Fit in EE

	if( typeEE != "none" ) {
		for(int i = 0; i < nPhiBinsEE; ++i) {
			for(int j = 0; j < nRegionsEE; ++j) {
				float flPhi = hPhiBinEE->GetXaxis()->GetBinCenter(i);

				(h_EoP_EE.at(i)).at(j) -> Rebin(rebinEE);
				(h_EoC_EE.at(i)).at(j) -> Rebin(rebinEE);


				// define the fitting function
				// N.B. [0] * ( [1] * f( [1]*(x-[2]) ) )

				char funcName[50];
				sprintf(funcName, "f_EoP_%d_%d_Ref_%d_%d_EE", i, j, refIdEE.at(i), j);
				(f_EoP_EE.at(i)).push_back( new TF1(funcName, (templateHistoFuncEE.at(refIdEE.at(i))).at(j), 0.7, 1.1, 3, "histoFunc") );

				(f_EoP_EE.at(i)).at(j) -> SetParName(0, "Norm");
				(f_EoP_EE.at(i)).at(j) -> SetParName(1, "Scale factor");

				(f_EoP_EE.at(i)).at(j) -> SetLineWidth(1);
				(f_EoP_EE.at(i)).at(j) -> SetLineColor(kRed + 2);
				(f_EoP_EE.at(i)).at(j) -> SetNpx(10000);
				(f_EoP_EE.at(i)).at(j) -> SetNpx(10000);

				(h_EoP_EE.at(i)).at(j) -> Scale(1 * (h_EoC_EE.at(i)).at(j)->GetEntries() / (h_EoP_EE.at(i)).at(j)->GetEntries());

				// uncorrected
				double xNorm = (h_EoP_EE.at(i)).at(j)->Integral() / (h_template_EE.at(refIdEE.at(i))).at(j)->Integral() *
				               (h_EoP_EE.at(i)).at(j)->GetBinWidth(1) / (h_template_EE.at(refIdEE.at(i))).at(j)->GetBinWidth(1);

				(f_EoP_EE.at(i)).at(j) -> FixParameter(0, xNorm);
				(f_EoP_EE.at(i)).at(j) -> FixParameter(2, 0.);


				std::cout << "***** Fitting MC EE " << flPhi << " (" << i << "," << j << "):   ";
				TFitResultPtr rp;
				int fStatus;
				for(int trial = 0; trial < 10; ++trial) {
					(f_EoP_EE.at(i)).at(j) -> SetParameter(1, 0.99);
					rp = (h_EoP_EE.at(i)).at(j) -> Fit(funcName, "QRWL+");
					fStatus = rp;

					if( fStatus != 4 && (f_EoP_EE.at(i)).at(j)->GetParError(1) != 0. ) {
						std::cout << "fit OK    ";

						double k = (f_EoP_EE.at(i)).at(j)->GetParameter(1);
						double eee = (f_EoP_EE.at(i)).at(j)->GetParError(1);
						g_EoP_EE[j] -> SetPoint(i, flPhi, 1. / k);
						g_EoP_EE[j] -> SetPointError(i, 0., eee / k / k);

						break;
					} else if( trial == 9 ) {
						std::cout << "fit BAD   ";

						g_EoP_EE[j] -> SetPoint(i, flPhi, 1.);
						g_EoP_EE[j] -> SetPointError(i, 0., 0.03);
					}
				}

				//ratio preparation
				float rat = (f_EoP_EE.at(i)).at(j)->GetParameter(1);
				float era = (f_EoP_EE.at(i)).at(j)->GetParError(1);

				// corrected
				xNorm = (h_EoC_EE.at(i)).at(j)->Integral() / (h_template_EE.at(refIdEE.at(i))).at(j)->Integral() *
				        (h_EoC_EE.at(i)).at(j)->GetBinWidth(1) / (h_template_EE.at(refIdEE.at(i))).at(j)->GetBinWidth(1);

				sprintf(funcName, "f_EoC_%d_%d_Ref_%d_%d_EE", i, j, refIdEE.at(i), j);
				(f_EoC_EE.at(i)).push_back( new TF1(funcName, (templateHistoFuncEE.at(refIdEE.at(i))).at(j), 0.7, 1.1, 3, "histoFunc") );

				(f_EoC_EE.at(i)).at(j) -> SetParName(0, "Norm");
				(f_EoC_EE.at(i)).at(j) -> SetParName(1, "Scale factor");

				(f_EoC_EE.at(i)).at(j) -> SetLineWidth(1);
				(f_EoC_EE.at(i)).at(j) -> SetLineColor(kGreen + 2);
				(f_EoC_EE.at(i)).at(j) -> SetNpx(10000);

				(f_EoC_EE.at(i)).at(j) -> FixParameter(0, xNorm);
				(f_EoC_EE.at(i)).at(j) -> FixParameter(2, 0.);


				std::cout << "***** Fitting DATA EE " << flPhi << " (" << i << "," << j << "):   ";
				for(int trial = 0; trial < 10; ++trial) {
					(f_EoC_EE.at(i)).at(j) -> SetParameter(1, 0.99);
					rp = (h_EoC_EE.at(i)).at(j) -> Fit(funcName, "QR+");
					if( fStatus != 4 && (f_EoC_EE.at(i)).at(j)->GetParError(1) != 0. ) {
						std::cout << "fit OK    ";

						double k = (f_EoC_EE.at(i)).at(j)->GetParameter(1);
						double eee = (f_EoC_EE.at(i)).at(j)->GetParError(1);
						g_EoC_EE[j] -> SetPoint(i, flPhi, 1. / k);
						g_EoC_EE[j] -> SetPointError(i, 0., eee / k / k);

						break;
					} else if( trial == 9 ) {
						g_EoC_EE[j] -> SetPoint(i, flPhi, 1.);
						g_EoC_EE[j] -> SetPointError(i, 0., 0.03);
					}
				}


				//ratio finalization
				rat /= (f_EoC_EE.at(i)).at(j)->GetParameter(1);
				era = rat * sqrt(era * era + (f_EoC_EE.at(i)).at(j)->GetParError(1) * (f_EoC_EE.at(i)).at(j)->GetParError(1));

				g_Rat_EE[j] -> SetPoint(i, flPhi, rat);
				g_Rat_EE[j] -> SetPointError(i,  0. , era);

				g_Rat_EE[j]->SetLineColor(kBlue + 2);
			}

			std::cout << std::endl;
		}
	} else {
		for(int i = 0; i < nPhiBinsEE; ++i) {
			for(int j = 0; j < nRegionsEE; ++j) {
				float flPhi = hPhiBinEE->GetXaxis()->GetBinCenter(i + 1);
				g_EoP_EE[j] -> SetPoint(i, flPhi, 1.);
				g_EoC_EE[j] -> SetPoint(i, flPhi, 1.);
				g_Rat_EE[j] -> SetPoint(i, flPhi, 1.);
			}
		}
	}






	//-------
	// Output

	o -> cd();

	for(int j = 0; j < nRegionsEB; ++j) {
		TString Name;
		//Name = Form("g_EoP_EB_%d",j);
		//if(g_EoP_EB[j]->GetN()!=0) g_EoP_EB[j] -> Write(Name);
		Name = Form("g_EoC_EB_%d", j);
		if(g_EoC_EB[j]->GetN() != 0) g_EoC_EB[j] -> Write(Name);
		//Name = Form("g_Rat_EB_%d",j);
		//if(g_Rat_EB[j]->GetN()!=0) g_Rat_EB[j] -> Write(Name);
	}

	for(int j = 0; j < nRegionsEE; ++j) {
		TString Name;
		//Name = Form("g_EoP_EE_%d",j);
		//if(g_EoP_EE[j]->GetN()!=0) g_EoP_EE[j] -> Write(Name);
		Name = Form("g_EoC_EE_%d", j);
		if(g_EoC_EE[j]->GetN() != 0) g_EoC_EE[j] -> Write(Name);
		//Name = Form("g_Rat_EE_%d",j);
		//if(g_Rat_EE[j]->GetN()!=0) g_Rat_EE[j] -> Write(Name);
	}

	for(int mod = 0; mod < nPhiBinsTempEB; ++mod)
		for(int j = 0; j < nRegionsEB; ++j) {
			if( h_template_EB[mod][j] -> GetEntries() != 0 ) h_template_EB[mod][j] -> Write();
		}

	for(int mod = 0; mod < nPhiBinsTempEE; ++mod)
		for(int j = 0; j < nRegionsEE; ++j) {
			if( h_template_EE[mod][j] -> GetEntries() != 0 ) h_template_EE[mod][j] -> Write();
		}

	for(int i = 0; i < nPhiBinsEB; ++i)
		for(int j = 0; j < nRegionsEB; ++j) {
			//if( (h_EoP_EB.at(i)).at(j) -> GetEntries() != 0 ) (h_EoP_EB.at(i)).at(j) -> Write();
			//if( (h_EoC_EB.at(i)).at(j) -> GetEntries() != 0 ) (h_EoC_EB.at(i)).at(j) -> Write();
		}

	for(int i = 0; i < nPhiBinsEE; ++i)
		for(int j = 0; j < nRegionsEE; ++j) {
			//if( (h_EoP_EE.at(i)).at(j) -> GetEntries() != 0 ) (h_EoP_EE.at(i)).at(j) -> Write();
			//if( (h_EoC_EE.at(i)).at(j) -> GetEntries() != 0 ) (h_EoC_EE.at(i)).at(j) -> Write();
		}

	for(int j = 0; j < nRegionsEB; ++j) {
		if( h_phi_mc_EB[j]   -> GetEntries() != 0 ) h_phi_mc_EB[j]   -> Write();
		if( h_phi_data_EB[j] -> GetEntries() != 0 ) h_phi_data_EB[j] -> Write();
	}

	for(int j = 0; j < nRegionsEE; ++j) {
		if( h_phi_mc_EE[j]   -> GetEntries() != 0 ) h_phi_mc_EE[j]   -> Write();
		if( h_phi_data_EE[j] -> GetEntries() != 0 ) h_phi_data_EE[j] -> Write();
	}

	h_et_mc->Write();
	h_et_data->Write();

	mapConversionEEp -> Write();
	mapConversionEEm -> Write();

	o -> Close();



	return 0;
}
