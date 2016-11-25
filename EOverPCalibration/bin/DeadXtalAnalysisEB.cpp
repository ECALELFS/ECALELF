#include <vector>
#include <utility>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <string>

#include "TFile.h"
#include "TStyle.h"
#include "TMath.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TROOT.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "TApplication.h"
#include "TEndcapRings.h"

#include "ConfigParser.h"
#include "ntpleUtils.h"


int main (int argc, char** argv)
{

	/*  std::string ROOTStyle =  getenv ("ROOTStyle");

	gROOT->ProcessLine((".x "+ROOTStyle+"/rootLogon.C").c_str());
	gROOT->ProcessLine((".x "+ROOTStyle+"/rootPalette.C").c_str());
	gROOT->ProcessLine((".x "+ROOTStyle+"/rootColors.C").c_str());
	gROOT->ProcessLine((".x "+ROOTStyle+"/setTDRStyle.C").c_str());
	*/

	if(argc < 2) {
		std::cout << " Not correct number of input parameter --> Need Just one cfg file exit " << std::endl;
		return -1;
	}

	parseConfigFile(argv[1]);

	// txt file with the list of input root files
	std::string inputList = gConfigParser -> readStringOption("Input::inputList");
	std::string inputStandardMap = gConfigParser -> readStringOption("Input::inputStandardMap");

	std::string DeadChannelMapName ;
	try {
		DeadChannelMapName = gConfigParser -> readStringOption("Input::DeadChannelMapName");
	} catch(char const* exceptionString ) {
		DeadChannelMapName = "h_map_Dead_Channels";
	}

	std::string ICMapName ;
	try {
		ICMapName = gConfigParser -> readStringOption("Input::ICMapName");
	} catch(char const* exceptionString ) {
		ICMapName = "h_scale_EB";
	}

	bool isDeadTriggerTower ;
	try {
		isDeadTriggerTower = gConfigParser -> readBoolOption("Input::isDeadTriggerTower");
	} catch(char const* exceptionString ) {
		isDeadTriggerTower = false;
	}

	std::vector<double> EtaBinCenterDeadTT ;
	try {
		EtaBinCenterDeadTT = gConfigParser -> readDoubleListOption("Input::EtaBinCenterDeadTT");
	} catch(char const* exceptionString ) {
		EtaBinCenterDeadTT.push_back(-999);
	}

	int NPhiShift ;
	try {
		NPhiShift = gConfigParser -> readIntOption("Input::NPhiShift");
	} catch(char const* exceptionString ) {
		NPhiShift = 14;
	}

	int PhiOffset ;
	try {
		PhiOffset = gConfigParser -> readIntOption("Input::PhiOffset");
	} catch(char const* exceptionString ) {
		PhiOffset = 4;
	}

	bool icMapRatio ;
	try {
		icMapRatio = gConfigParser -> readBoolOption("Input::icMapRatio");
	} catch(char const* exceptionString ) {
		icMapRatio = true;
	}

	std::vector<double> iEtaBinDivision ;
	try {
		iEtaBinDivision = gConfigParser -> readDoubleListOption("Input::iEtaBinDivision");
	} catch(char const* exceptionString ) {
		iEtaBinDivision.push_back(0);
		iEtaBinDivision.push_back(20);
		iEtaBinDivision.push_back(40);
		iEtaBinDivision.push_back(60);
		iEtaBinDivision.push_back(85);
	}

	std::vector<double> iPhiBinDivision ;
	try {
		iPhiBinDivision = gConfigParser -> readDoubleListOption("Input::iPhiBinDivision");
	} catch(char const* exceptionString ) {
		iPhiBinDivision.push_back(0);
		iPhiBinDivision.push_back(90);
		iPhiBinDivision.push_back(180);
		iPhiBinDivision.push_back(270);
		iPhiBinDivision.push_back(360);
	}

	int NPhiSMBinDivision ;
	try {
		NPhiSMBinDivision = gConfigParser -> readIntOption("Input::NPhiSMBinDivision");
	} catch(char const* exceptionString ) {
		NPhiSMBinDivision = 5 ;
	}


	int IPhiWindow ;
	try {
		IPhiWindow = gConfigParser -> readIntOption("Input::IPhiWindow");
	} catch(char const* exceptionString ) {
		IPhiWindow = 3;
	}

	int IEtaWindow ;
	try {
		IEtaWindow = gConfigParser -> readIntOption("Input::IEtaWindow");
	} catch(char const* exceptionString ) {
		IEtaWindow = 3;
	}

	std::string outputCanvasPlot ;
	try {
		outputCanvasPlot = gConfigParser -> readStringOption("Output::outputCanvasPlot");
	} catch(char const* exceptionString ) {
		outputCanvasPlot = "output/outDeadXtalPlots/";
	}

	std::cout << "inputList          : " << inputList << std::endl;
	std::cout << "inputStandardMap   : " << inputStandardMap << std::endl;
	std::cout << "DeadChannelMapName : " << DeadChannelMapName << std::endl;
	std::cout << "ICMapName          : " << ICMapName << std::endl;
	std::cout << "IPhiWindow         : " << IPhiWindow << std::endl;
	std::cout << "IEtaWindow         : " << IEtaWindow << std::endl;
	std::cout << "isDeadTriggerTower : " << isDeadTriggerTower << std::endl;
	std::cout << "NPhiShift          : " << NPhiShift << std::endl;
	std::cout << "PhiOffset          : " << PhiOffset << std::endl;
	std::cout << "icMapRatio         : " << icMapRatio << std::endl;

	std::cout << " Dead Trigger Tower " << std::endl;
	for(size_t iEta = 0 ; iEta < EtaBinCenterDeadTT.size() ; iEta++)
		std::cout << "Eta Bin : " << iEta << " ieta : " << EtaBinCenterDeadTT.at(iEta) << std::endl;

	Double_t *EtaBin = new double [int(iEtaBinDivision.size())];
	Double_t *PhiBin = new double [int(iPhiBinDivision.size())];

	std::cout << " Eta Binning " << std::endl;
	for(size_t iEta = 0 ; iEta < iEtaBinDivision.size() ; iEta++) {
		std::cout << "Eta Bin : " << iEta << " ieta : " << iEtaBinDivision.at(iEta) << std::endl;
		EtaBin[iEta] = iEtaBinDivision.at(iEta);
	}


	std::cout << " Phi Binning " << std::endl;
	for(size_t iPhi = 0 ; iPhi < iPhiBinDivision.size() ; iPhi++) {
		std::cout << "Phi Bin : " << iPhi << " iphi : " << iPhiBinDivision.at(iPhi) << std::endl;
		PhiBin[iPhi] = iPhiBinDivision.at(iPhi);
	}

	std::cout << "NPhiSMBinDivision : " << NPhiSMBinDivision << std::endl;
	std::cout << "outputCanvasPlot   : " << outputCanvasPlot << std::endl;


	system(("mkdir -p " + outputCanvasPlot).c_str());
	system(("rm " + outputCanvasPlot + "*").c_str());

	gStyle->SetOptFit(111);
	if(isDeadTriggerTower)gStyle->SetOptStat(111110);
	else gStyle->SetOptStat(11110);


	// ic map running a standard calibration without any dead channel

	TFile* inputFileStandardMap = new TFile(inputStandardMap.c_str(), "READ");
	inputFileStandardMap->cd();
	TH2F* ICMapEBStandard =  (TH2F*) inputFileStandardMap->Get(ICMapName.c_str()) ;


	std::ifstream inFile(inputList.c_str());
	std::string buffer;

	std::vector<TFile*> inputFileList ;

	if(!inFile.is_open()) {
		std::cout << "** ERROR: Can't open '" << inputList << "' for input" << std::endl;
		return -1;
	}

	std::cout << "Input Files : " << std::endl;

	while(!inFile.eof()) {

		inFile >> buffer;
		if( buffer.at(0) == '#' ) continue;
		std::cout << buffer << std::endl;
		inputFileList.push_back(new TFile(buffer.c_str(), "READ"));
	}

	std::vector<std::vector<TH1F*> > ICCrystalEB (IPhiWindow, std::vector<TH1F*> (IEtaWindow) ); // for inclusive analysis
	std::vector<std::vector<TH1F*> > ICCrystalEBP(IPhiWindow, std::vector<TH1F*> (IEtaWindow) ); // for inclusive analysis in EB+
	std::vector<std::vector<TH1F*> > ICCrystalEBM(IPhiWindow, std::vector<TH1F*> (IEtaWindow) ); // for inclusive analysis in EB-

	// for differential eta and phi analysis
	std::vector<std::vector<std::vector<TH1F*> > > ICCrystalEB_EtaBinned(iEtaBinDivision.size() - 1, std::vector<std::vector<TH1F*> >(IPhiWindow, std::vector<TH1F*> (IEtaWindow)));
	std::vector<std::vector<std::vector<TH1F*> > > ICCrystalEB_PhiBinned(iPhiBinDivision.size() - 1, std::vector<std::vector<TH1F*> >(IPhiWindow, std::vector<TH1F*> (IEtaWindow)));
	std::vector<std::vector<std::vector<TH1F*> > > ICCrystalEB_SMBinned(int(20 / NPhiSMBinDivision), std::vector<std::vector<TH1F*> >(IPhiWindow, std::vector<TH1F*> (IEtaWindow)));


	std::vector<TH2F*> DeadCrystalEB(inputFileList.size());
	std::vector<TH2F*> ICMapEB(inputFileList.size());

	for( int iPhi = 0 ; iPhi < IPhiWindow ; iPhi ++) {

		for( int iEta = 0 ; iEta < IEtaWindow ; iEta ++) {

			if(isDeadTriggerTower) (ICCrystalEB.at(iPhi)).at(iEta)       = new TH1F(std::string(Form("ICCrystalEB_%d_%d", iPhi, iEta)).c_str(), "", 10000, 0, 100);
			else (ICCrystalEB.at(iPhi)).at(iEta)       = new TH1F(std::string(Form("ICCrystalEB_%d_%d", iPhi, iEta)).c_str(), "", 100, 0.95, 1.4);
			(ICCrystalEB.at(iPhi)).at(iEta)->Sumw2();
			if(isDeadTriggerTower) (ICCrystalEBP.at(iPhi)).at(iEta)      = new TH1F(std::string(Form("ICCrystalEBP_%d_%d", iPhi, iEta)).c_str(), "", 500, 0, 5);
			else (ICCrystalEBP.at(iPhi)).at(iEta)      = new TH1F(std::string(Form("ICCrystalEBP_%d_%d", iPhi, iEta)).c_str(), "", 100, 0.95, 1.4);
			(ICCrystalEBP.at(iPhi)).at(iEta)->Sumw2();
			if(isDeadTriggerTower) (ICCrystalEBM.at(iPhi)).at(iEta)      = new TH1F(std::string(Form("ICCrystalEBM_%d_%d", iPhi, iEta)).c_str(), "", 10000, 0, 100);
			else (ICCrystalEBM.at(iPhi)).at(iEta)      = new TH1F(std::string(Form("ICCrystalEBM_%d_%d", iPhi, iEta)).c_str(), "", 100, 0.95, 1.4);
			(ICCrystalEBM.at(iPhi)).at(iEta)->Sumw2();

			for( int iEtaBin = 0; iEtaBin < int(iEtaBinDivision.size() - 1) ; iEtaBin++) {

				if(isDeadTriggerTower) ((ICCrystalEB_EtaBinned.at(iEtaBin)).at(iPhi)).at(iEta) = new TH1F(std::string(Form("ICCrystalEB_EtaBin_%f_%f_iEta_%d_iPhi_%d", iEtaBinDivision.at(iEtaBin),
					        iEtaBinDivision.at(iEtaBin + 1), iPhi, iEta)).c_str(), "", 10000, 0, 100);
				else ((ICCrystalEB_EtaBinned.at(iEtaBin)).at(iPhi)).at(iEta) = new TH1F(std::string(Form("ICCrystalEB_EtaBin_%f_%f_iEta_%d_iPhi_%d", iEtaBinDivision.at(iEtaBin),
					        iEtaBinDivision.at(iEtaBin + 1), iPhi, iEta)).c_str(), "", 100, 0.95, 1.4);
				((ICCrystalEB_EtaBinned.at(iEtaBin)).at(iPhi)).at(iEta)->Sumw2();
			}

			for( int iPhiBin = 0; iPhiBin < int(iPhiBinDivision.size() - 1) ; iPhiBin++) {

				if(isDeadTriggerTower) ((ICCrystalEB_PhiBinned.at(iPhiBin)).at(iPhi)).at(iEta) = new TH1F(std::string(Form("ICCrystalEB_PhiBin_%f_%f_iEta_%d_iPhi_%d", iPhiBinDivision.at(iPhiBin),
					        iPhiBinDivision.at(iPhiBin + 1), iPhi, iEta)).c_str(), "", 10000, 0, 100);
				else ((ICCrystalEB_PhiBinned.at(iPhiBin)).at(iPhi)).at(iEta) = new TH1F(std::string(Form("ICCrystalEB_PhiBin_%f_%f_iEta_%d_iPhi_%d", iPhiBinDivision.at(iPhiBin),
					        iPhiBinDivision.at(iPhiBin + 1), iPhi, iEta)).c_str(), "", 100, 0.95, 1.4);
				((ICCrystalEB_PhiBinned.at(iPhiBin)).at(iPhi)).at(iEta)->Sumw2();

			}

			for( int iSMBin = 0; iSMBin < int(20 / NPhiSMBinDivision) ; iSMBin++) {

				if(isDeadTriggerTower)  ((ICCrystalEB_SMBinned.at(iSMBin)).at(iPhi)).at(iEta) = new TH1F(std::string(Form("ICCrystalEB_SMBin_%d_%d_iEta_%d_iPhi_%d", iSMBin * NPhiSMBinDivision,
					        (iSMBin + 1)*NPhiSMBinDivision, iPhi, iEta)).c_str(), "", 10000, 0, 100);
				else  ((ICCrystalEB_SMBinned.at(iSMBin)).at(iPhi)).at(iEta) = new TH1F(std::string(Form("ICCrystalEB_SMBin_%d_%d_iEta_%d_iPhi_%d", iSMBin * NPhiSMBinDivision,
					        (iSMBin + 1)*NPhiSMBinDivision, iPhi, iEta)).c_str(), "", 100, 0.95, 1.4);
				((ICCrystalEB_SMBinned.at(iSMBin)).at(iPhi)).at(iEta)->Sumw2();

			}
		}
	}

	for( unsigned int iFile = 0 ; iFile < inputFileList.size() ; iFile ++) {

		DeadCrystalEB.push_back((TH2F*) inputFileList.at(iFile)->Get(DeadChannelMapName.c_str()));
		ICMapEB.push_back((TH2F*) inputFileList.at(iFile)->Get(ICMapName.c_str()));

		if(isDeadTriggerTower) {

			int PhiShift = 0 ;
			if(TString(inputFileList.at(iFile)->GetName()).Contains("p1")) PhiShift = 1 ;
			else if(TString(inputFileList.at(iFile)->GetName()).Contains("m1")) PhiShift = -1 ;
			else if(TString(inputFileList.at(iFile)->GetName()).Contains("p2")) PhiShift = 2 ;
			else if(TString(inputFileList.at(iFile)->GetName()).Contains("m2")) PhiShift = -2 ;

			for(int iEta = 0 ; iEta < int(EtaBinCenterDeadTT.size()) ; iEta ++ ) {
				for( int iPhi = PhiOffset + PhiShift ; iPhi < 360 ; iPhi = iPhi + NPhiShift) {

					if(DeadCrystalEB.back()->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86) != 0 && EtaBinCenterDeadTT.at(iEta) != 0) {

						if(icMapRatio) {
							ICCrystalEB[int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86) /
							        ICMapEBStandard->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86));
							if(fabs(EtaBinCenterDeadTT.at(iEta) + 85) <= 85) ICCrystalEBM[int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86) /
								        ICMapEBStandard->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86));
							else ICCrystalEBP[int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86) /
								        ICMapEBStandard->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86));
						} else {
							ICCrystalEB[int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86));
							if(fabs(EtaBinCenterDeadTT.at(iEta) + 85) <= 85) ICCrystalEBM[int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86));
							else ICCrystalEBP[int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86));
						}

						int iEtaBin = 0 ;
						int iPhiBin = 0 ;

						for( ; iEtaBin < int(iEtaBinDivision.size() - 1) ; iEtaBin ++) {
							if(fabs(EtaBinCenterDeadTT.at(iEta)) > iEtaBinDivision.at(iEtaBin) && fabs(EtaBinCenterDeadTT.at(iEta)) <= iEtaBinDivision.at(iEtaBin + 1))  break ;
						}

						for( ; iPhiBin < int(iPhiBinDivision.size() - 1) ; iPhiBin ++) {
							if(iPhi == 0) {
								iPhiBin = 0 ;
								break ;
							} else if (iPhi == 360) {
								iPhiBin = int(iPhiBinDivision.size() - 1) - 1 ;
								break;
							} else if(iPhi > iPhiBinDivision.at(iPhiBin) && iPhi <= iPhiBinDivision.at(iPhiBin + 1))  break ;
						}

						if(icMapRatio) {
							if(ICMapEBStandard->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86) != 0) {
								ICCrystalEB_EtaBinned[iEtaBin][int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86) /
								        ICMapEBStandard->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86));
								ICCrystalEB_PhiBinned[iPhiBin][int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86) /
								        ICMapEBStandard->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86));
								ICCrystalEB_SMBinned[int((iPhi % 20) / NPhiSMBinDivision)][int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill(ICMapEB.back()->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86) /
								        ICMapEBStandard->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86));
							}
						} else {
							ICCrystalEB_EtaBinned[iEtaBin][int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86));
							ICCrystalEB_PhiBinned[iPhiBin][int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86));
							ICCrystalEB_SMBinned[int((iPhi % 20) / NPhiSMBinDivision)][int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill(ICMapEB.back()->GetBinContent(iPhi + 1, EtaBinCenterDeadTT.at(iEta) + 86));
						}

						for( int IPHI = iPhi - int((IPhiWindow - 1) / 2) ; IPHI <= iPhi + int((IPhiWindow - 1) / 2) ; IPHI ++) {
							for( int IETA = EtaBinCenterDeadTT.at(iEta) + 85 - int((IEtaWindow - 1) / 2) ; IETA <= EtaBinCenterDeadTT.at(iEta) + 85 + int((IEtaWindow - 1) / 2) ; IETA ++) {

								if(icMapRatio) {
									ICCrystalEB[int(IPhiWindow / 2) + (IPHI - iPhi)][int((IEtaWindow) / 2) + (IETA - (EtaBinCenterDeadTT.at(iEta) + 85))]->Fill(ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1) /
									        ICMapEBStandard->GetBinContent(IPHI + 1, IETA + 1));

									if(fabs(EtaBinCenterDeadTT.at(iEta) + 85) > 85) ICCrystalEBP[int(IPhiWindow / 2) + (IPHI - iPhi)][int((IEtaWindow) / 2) + (IETA - (EtaBinCenterDeadTT.at(iEta) + 85))]->Fill(ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1) / ICMapEBStandard->GetBinContent(IPHI + 1, IETA + 1));

									else ICCrystalEBM[int(IPhiWindow / 2) + (IPHI - iPhi)][int((IEtaWindow) / 2) + (IETA - (EtaBinCenterDeadTT.at(iEta) + 85))]->Fill(ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1) /
										        ICMapEBStandard->GetBinContent(IPHI + 1, IETA + 1));

									ICCrystalEB_EtaBinned[iEtaBin][int(IPhiWindow / 2) + (IPHI - iPhi)][int(IEtaWindow / 2) + (IETA - (EtaBinCenterDeadTT.at(iEta) + 85))]->Fill( ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1) / ICMapEBStandard->GetBinContent(IPHI + 1, IETA + 1));

									ICCrystalEB_PhiBinned[iPhiBin][int(IPhiWindow / 2) + (IPHI - iPhi)][int(IEtaWindow / 2) + (IETA - (EtaBinCenterDeadTT.at(iEta) + 85))]->Fill( ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1) / ICMapEBStandard->GetBinContent(IPHI + 1, IETA + 1));

									ICCrystalEB_SMBinned[int((iPhi % 20) / NPhiSMBinDivision)][int(IPhiWindow / 2) + (IPHI - iPhi)][int(IEtaWindow / 2) + (IETA - (EtaBinCenterDeadTT.at(iEta) + 85))]->Fill(ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1) / ICMapEBStandard->GetBinContent(IPHI + 1, IETA + 1));
								} else {

									ICCrystalEB[int(IPhiWindow / 2) + (IPHI - iPhi)][int((IEtaWindow) / 2) + (IETA - (EtaBinCenterDeadTT.at(iEta) + 85))]->Fill(ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1));

									if(fabs(EtaBinCenterDeadTT.at(iEta) + 85) > 85) ICCrystalEBP[int(IPhiWindow / 2) + (IPHI - iPhi)][int((IEtaWindow) / 2) + (IETA - (EtaBinCenterDeadTT.at(iEta) + 85))]->Fill(ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1));

									else ICCrystalEBM[int(IPhiWindow / 2) + (IPHI - iPhi)][int((IEtaWindow) / 2) + (IETA - (EtaBinCenterDeadTT.at(iEta) + 85))]->Fill(ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1));

									ICCrystalEB_EtaBinned[iEtaBin][int(IPhiWindow / 2) + (IPHI - iPhi)][int(IEtaWindow / 2) + (IETA - (EtaBinCenterDeadTT.at(iEta) + 85))]->Fill( ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1));

									ICCrystalEB_PhiBinned[iPhiBin][int(IPhiWindow / 2) + (IPHI - iPhi)][int(IEtaWindow / 2) + (IETA - (EtaBinCenterDeadTT.at(iEta) + 85))]->Fill( ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1));

									ICCrystalEB_SMBinned[int((iPhi % 20) / NPhiSMBinDivision)][int(IPhiWindow / 2) + (IPHI - iPhi)][int(IEtaWindow / 2) + (IETA - (EtaBinCenterDeadTT.at(iEta) + 85))]->Fill(ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1));
								}
							}
						}
					}
				}
			}
		}

		else {
			for( int iPhi = 0 ; iPhi < 360 ; iPhi ++) {
				for( int iEta = 0 ; iEta < 170 ; iEta ++) {

					if(DeadCrystalEB.back()->GetBinContent(iPhi + 1, iEta + 1) != 0 && iEta != 0) {

						if(icMapRatio) {
							ICCrystalEB[int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, iEta + 1) / ICMapEBStandard->GetBinContent(iPhi + 1, iEta + 1));
							if(iEta <= 85) ICCrystalEBM[int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, iEta + 1) / ICMapEBStandard->GetBinContent(iPhi + 1, iEta + 1));
							else ICCrystalEBP[int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, iEta + 1) / ICMapEBStandard->GetBinContent(iPhi + 1, iEta + 1));
						} else {
							ICCrystalEB[int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, iEta + 1));
							if(iEta <= 85) ICCrystalEBM[int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, iEta + 1));
							else ICCrystalEBP[int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, iEta + 1));
						}

						int iEtaBin = 0 ;
						int iPhiBin = 0 ;

						for( ; iEtaBin < int(iEtaBinDivision.size() - 1) ; iEtaBin ++) {
							if(fabs(iEta - 85) > iEtaBinDivision.at(iEtaBin) && fabs(iEta - 85) <= iEtaBinDivision.at(iEtaBin + 1))  break ;
						}

						for( ; iPhiBin < int(iPhiBinDivision.size() - 1) ; iPhiBin ++) {
							if(iPhi == 0) {
								iPhiBin = 0 ;
								break ;
							} else if (iPhi == 360) {
								iPhiBin = int(iPhiBinDivision.size() - 1) - 1 ;
								break;
							} else if(iPhi > iPhiBinDivision.at(iPhiBin) && iPhi <= iPhiBinDivision.at(iPhiBin + 1))  break ;
						}

						if(icMapRatio) {
							if(ICMapEBStandard->GetBinContent(iPhi + 1, iEta + 1) != 0) {
								ICCrystalEB_EtaBinned[iEtaBin][int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, iEta + 1) / ICMapEBStandard->GetBinContent(iPhi + 1, iEta + 1));
								ICCrystalEB_PhiBinned[iPhiBin][int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, iEta + 1) / ICMapEBStandard->GetBinContent(iPhi + 1, iEta + 1));
								ICCrystalEB_SMBinned[int((iPhi % 20) / NPhiSMBinDivision)][int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill(ICMapEB.back()->GetBinContent(iPhi + 1, iEta + 1) /
								        ICMapEBStandard->GetBinContent(iPhi + 1, iEta + 1));
							}
						} else {
							ICCrystalEB_EtaBinned[iEtaBin][int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, iEta + 1));
							ICCrystalEB_PhiBinned[iPhiBin][int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill( ICMapEB.back()->GetBinContent(iPhi + 1, iEta + 1));
							ICCrystalEB_SMBinned[int((iPhi % 20) / NPhiSMBinDivision)][int(IPhiWindow / 2)][int(IEtaWindow / 2)]->Fill(ICMapEB.back()->GetBinContent(iPhi + 1, iEta + 1));
						}

						for( int IPHI = iPhi - int((IPhiWindow - 1) / 2) ; IPHI <= iPhi + int((IPhiWindow - 1) / 2) ; IPHI ++) {
							for( int IETA = iEta - int((IEtaWindow - 1) / 2) ; IETA <= iEta + int((IEtaWindow - 1) / 2) ; IETA ++) {

								if(icMapRatio) {
									ICCrystalEB[int(IPhiWindow / 2) + (IPHI - iPhi)][int((IEtaWindow) / 2) + (IETA - iEta)]->Fill(ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1) /
									        ICMapEBStandard->GetBinContent(IPHI + 1, IETA + 1));

									if(iEta > 85) ICCrystalEBP[int(IPhiWindow / 2) + (IPHI - iPhi)][int((IEtaWindow) / 2) + (IETA - iEta)]->Fill(ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1) /
										        ICMapEBStandard->GetBinContent(IPHI + 1, IETA + 1));

									else ICCrystalEBM[int(IPhiWindow / 2) + (IPHI - iPhi)][int((IEtaWindow) / 2) + (IETA - iEta)]->Fill(ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1) /
										        ICMapEBStandard->GetBinContent(IPHI + 1, IETA + 1));

									ICCrystalEB_EtaBinned[iEtaBin][int(IPhiWindow / 2) + (IPHI - iPhi)][int(IEtaWindow / 2) + (IETA - iEta)]->Fill( ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1) /
									        ICMapEBStandard->GetBinContent(IPHI + 1, IETA + 1));

									ICCrystalEB_PhiBinned[iPhiBin][int(IPhiWindow / 2) + (IPHI - iPhi)][int(IEtaWindow / 2) + (IETA - iEta)]->Fill( ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1) /
									        ICMapEBStandard->GetBinContent(IPHI + 1, IETA + 1));

									ICCrystalEB_SMBinned[int((iPhi % 20) / NPhiSMBinDivision)][int(IPhiWindow / 2) + (IPHI - iPhi)][int(IEtaWindow / 2) + (IETA - iEta)]->Fill(ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1) /
									        ICMapEBStandard->GetBinContent(IPHI + 1, IETA + 1));
								} else {

									ICCrystalEB[int(IPhiWindow / 2) + (IPHI - iPhi)][int((IEtaWindow) / 2) + (IETA - iEta)]->Fill(ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1));

									if(iEta > 85) ICCrystalEBP[int(IPhiWindow / 2) + (IPHI - iPhi)][int((IEtaWindow) / 2) + (IETA - iEta)]->Fill(ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1));

									else ICCrystalEBM[int(IPhiWindow / 2) + (IPHI - iPhi)][int((IEtaWindow) / 2) + (IETA - iEta)]->Fill(ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1));

									ICCrystalEB_EtaBinned[iEtaBin][int(IPhiWindow / 2) + (IPHI - iPhi)][int(IEtaWindow / 2) + (IETA - iEta)]->Fill( ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1));

									ICCrystalEB_PhiBinned[iPhiBin][int(IPhiWindow / 2) + (IPHI - iPhi)][int(IEtaWindow / 2) + (IETA - iEta)]->Fill( ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1));

									ICCrystalEB_SMBinned[int((iPhi % 20) / NPhiSMBinDivision)][int(IPhiWindow / 2) + (IPHI - iPhi)][int(IEtaWindow / 2) + (IETA - iEta)]->Fill(ICMapEB.back()->GetBinContent(IPHI + 1, IETA + 1));
								}
							}
						}
					}
				}
			}
		}
	}

	std::vector<std::vector<TCanvas*> > Can (IPhiWindow, std::vector<TCanvas*> (IEtaWindow));
	std::vector<std::vector<TCanvas*> > Can_EBP (IPhiWindow, std::vector<TCanvas*> (IEtaWindow));
	std::vector<std::vector<TCanvas*> > Can_EBM (IPhiWindow, std::vector<TCanvas*> (IEtaWindow));
	std::vector<std::vector<std::vector<TCanvas*> > > Can_EtaBinned (iEtaBinDivision.size() - 1, std::vector<std::vector<TCanvas*> >(IPhiWindow, std::vector<TCanvas*> (IEtaWindow)));
	std::vector<std::vector<std::vector<TCanvas*> > > Can_PhiBinned (iPhiBinDivision.size() - 1, std::vector<std::vector<TCanvas*> >(IPhiWindow, std::vector<TCanvas*> (IEtaWindow)));
	std::vector<std::vector<std::vector<TCanvas*> > > Can_SMBinned  (int(20 / NPhiSMBinDivision)     , std::vector<std::vector<TCanvas*> >(IPhiWindow, std::vector<TCanvas*> (IEtaWindow)));

	for( int iPhi = 0 ; iPhi < IPhiWindow ; iPhi ++) {

		for(int iEta = 0 ; iEta < IPhiWindow ; iEta ++) {
			(Can.at(iPhi)).at(iEta) = new TCanvas(std::string(Form("Can_%d_%d", iPhi, iEta)).c_str(), "", 500, 500);
			(Can_EBP.at(iPhi)).at(iEta) = new TCanvas(std::string(Form("Can_EBP_%d_%d", iPhi, iEta)).c_str(), "", 500, 500);
			(Can_EBM.at(iPhi)).at(iEta) = new TCanvas(std::string(Form("Can_EBM_%d_%d", iPhi, iEta)).c_str(), "", 500, 500);

			for( int iEtaBin = 0 ; iEtaBin < int(iEtaBinDivision.size() - 1) ; iEtaBin ++)
				((Can_EtaBinned.at(iEtaBin)).at(iPhi)).at(iEta) = new TCanvas(std::string(Form("Can_EtaBin_%d_%d_iPhi_%d_iEta_%d", int(iEtaBinDivision.at(iEtaBin)),
				        int(iEtaBinDivision.at(iEtaBin + 1)), iPhi, iEta)).c_str(), "", 500, 500);

			for( int iPhiBin = 0 ; iPhiBin < int(iPhiBinDivision.size() - 1) ; iPhiBin ++)
				((Can_PhiBinned.at(iPhiBin)).at(iPhi)).at(iEta) = new TCanvas(std::string(Form("Can_PhiBin_%d_%d_iPhi_%d_iEta_%d", int(iPhiBinDivision.at(iPhiBin)),
				        int(iPhiBinDivision.at(iPhiBin + 1)), iPhi, iEta)).c_str(), "", 500, 500);
			for( int iSMBin = 0 ; iSMBin < int(NPhiSMBinDivision - 1) ; iSMBin ++)
				((Can_SMBinned.at(iSMBin)).at(iPhi)).at(iEta) = new TCanvas(std::string(Form("Can_SMBin_%d_%d_iPhi_%d_iEta_%d", iSMBin * NPhiSMBinDivision,
				        (iSMBin + 1)*NPhiSMBinDivision, iPhi, iEta)).c_str(), "", 500, 500);
		}
	}

	std::vector<std::vector<TF1*> > GaussianFits                         (IPhiWindow, std::vector<TF1*> (IEtaWindow)); // inclusive fits
	std::vector<std::vector<TF1*> > GaussianFits_EBP                     (IPhiWindow, std::vector<TF1*> (IEtaWindow)); // inclusive fits
	std::vector<std::vector<TF1*> > GaussianFits_EBM                     (IPhiWindow, std::vector<TF1*> (IEtaWindow)); // inclusive fits
	std::vector<std::vector<std::vector<TF1*> > > GaussianFits_EtaBinned (iEtaBinDivision.size() - 1, std::vector<std::vector<TF1*> >(IPhiWindow, std::vector<TF1*> (IEtaWindow)));
	std::vector<std::vector<std::vector<TF1*> > > GaussianFits_PhiBinned (iPhiBinDivision.size() - 1, std::vector<std::vector<TF1*> >(IPhiWindow, std::vector<TF1*> (IEtaWindow)));
	std::vector<std::vector<std::vector<TF1*> > > GaussianFits_SMBinned  (int(20 / NPhiSMBinDivision), std::vector<std::vector<TF1*> >(IPhiWindow, std::vector<TF1*> (IEtaWindow)));

	std::vector<std::vector<TH1F*> >  MeanIC_EtaBinned      (IPhiWindow, std::vector<TH1F*> (IEtaWindow));
	std::vector<std::vector<TH1F*> >  Gaus_MeanIC_EtaBinned (IPhiWindow, std::vector<TH1F*> (IEtaWindow));

	std::vector<std::vector<TH1F*> >  RMSIC_EtaBinned      (IPhiWindow, std::vector<TH1F*> (IEtaWindow));
	std::vector<std::vector<TH1F*> >  Gaus_RMSIC_EtaBinned (IPhiWindow, std::vector<TH1F*> (IEtaWindow));

	std::vector<std::vector<TH1F*> >  MeanIC_PhiBinned      (IPhiWindow, std::vector<TH1F*> (IEtaWindow));
	std::vector<std::vector<TH1F*> >  Gaus_MeanIC_PhiBinned (IPhiWindow, std::vector<TH1F*> (IEtaWindow));

	std::vector<std::vector<TH1F*> >  RMSIC_PhiBinned      (IPhiWindow, std::vector<TH1F*> (IEtaWindow));
	std::vector<std::vector<TH1F*> >  Gaus_RMSIC_PhiBinned (IPhiWindow, std::vector<TH1F*> (IEtaWindow));

	std::vector<std::vector<TH1F*> >  MeanIC_SMBinned      (IPhiWindow, std::vector<TH1F*> (IEtaWindow));
	std::vector<std::vector<TH1F*> >  Gaus_MeanIC_SMBinned (IPhiWindow, std::vector<TH1F*> (IEtaWindow));

	std::vector<std::vector<TH1F*> >  RMSIC_SMBinned      (IPhiWindow, std::vector<TH1F*> (IEtaWindow));
	std::vector<std::vector<TH1F*> >  Gaus_RMSIC_SMBinned (IPhiWindow, std::vector<TH1F*> (IEtaWindow));

	std::vector<std::vector<TF1*> >  Pol0_MeanIC_EtaBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol1_MeanIC_EtaBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol2_MeanIC_EtaBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol0_RMSIC_EtaBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol1_RMSIC_EtaBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol2_RMSIC_EtaBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));

	std::vector<std::vector<TF1*> >  Pol0_Gaus_MeanIC_EtaBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol1_Gaus_MeanIC_EtaBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol2_Gaus_MeanIC_EtaBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol0_Gaus_RMSIC_EtaBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol1_Gaus_RMSIC_EtaBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol2_Gaus_RMSIC_EtaBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));

	std::vector<std::vector<TF1*> >  Pol0_MeanIC_PhiBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol1_MeanIC_PhiBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol2_MeanIC_PhiBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol0_RMSIC_PhiBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol1_RMSIC_PhiBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol2_RMSIC_PhiBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));

	std::vector<std::vector<TF1*> >  Pol0_Gaus_MeanIC_PhiBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol1_Gaus_MeanIC_PhiBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol2_Gaus_MeanIC_PhiBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol0_Gaus_RMSIC_PhiBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol1_Gaus_RMSIC_PhiBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol2_Gaus_RMSIC_PhiBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));

	std::vector<std::vector<TF1*> >  Pol0_MeanIC_SMBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol1_MeanIC_SMBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol2_MeanIC_SMBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol0_RMSIC_SMBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol1_RMSIC_SMBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol2_RMSIC_SMBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));

	std::vector<std::vector<TF1*> >  Pol0_Gaus_MeanIC_SMBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol1_Gaus_MeanIC_SMBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol2_Gaus_MeanIC_SMBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol0_Gaus_RMSIC_SMBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol1_Gaus_RMSIC_SMBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));
	std::vector<std::vector<TF1*> >  Pol2_Gaus_RMSIC_SMBinned (IPhiWindow, std::vector<TF1*> (IEtaWindow));


	std::vector<std::vector<TCanvas*> > Can_MeanIC_EtaBinned(IPhiWindow, std::vector<TCanvas*> (IEtaWindow));
	std::vector<std::vector<TCanvas*> > Can_Gaus_MeanIC_EtaBinned(IPhiWindow, std::vector<TCanvas*> (IEtaWindow));
	std::vector<std::vector<TCanvas*> > Can_RMSIC_EtaBinned(IPhiWindow, std::vector<TCanvas*> (IEtaWindow));
	std::vector<std::vector<TCanvas*> > Can_Gaus_RMSIC_EtaBinned(IPhiWindow, std::vector<TCanvas*> (IEtaWindow));

	std::vector<std::vector<TCanvas*> > Can_MeanIC_PhiBinned(IPhiWindow, std::vector<TCanvas*> (IEtaWindow));
	std::vector<std::vector<TCanvas*> > Can_Gaus_MeanIC_PhiBinned(IPhiWindow, std::vector<TCanvas*> (IEtaWindow));
	std::vector<std::vector<TCanvas*> > Can_RMSIC_PhiBinned(IPhiWindow, std::vector<TCanvas*> (IEtaWindow));
	std::vector<std::vector<TCanvas*> > Can_Gaus_RMSIC_PhiBinned(IPhiWindow, std::vector<TCanvas*> (IEtaWindow));

	std::vector<std::vector<TCanvas*> > Can_MeanIC_SMBinned(IPhiWindow, std::vector<TCanvas*> (IEtaWindow));
	std::vector<std::vector<TCanvas*> > Can_Gaus_MeanIC_SMBinned(IPhiWindow, std::vector<TCanvas*> (IEtaWindow));
	std::vector<std::vector<TCanvas*> > Can_RMSIC_SMBinned(IPhiWindow, std::vector<TCanvas*> (IEtaWindow));
	std::vector<std::vector<TCanvas*> > Can_Gaus_RMSIC_SMBinned(IPhiWindow, std::vector<TCanvas*> (IEtaWindow));

	TH1F* htemp1;
	TH1F* htemp2;
	TPaveStats* pave1, *pave2, *pave3;

	for( int iPhi = 0 ; iPhi < IPhiWindow ; iPhi ++) {
		for( int iEta = 0 ; iEta < IEtaWindow ; iEta ++) {

			if(iPhi == int(IPhiWindow / 2) && iEta == int(IEtaWindow / 2)) continue ;
			if(isDeadTriggerTower && ( int(fabs(iPhi - IPhiWindow / 2)) < int(IPhiWindow / 2 - 1) && int(fabs(iEta - IEtaWindow / 2)) < int(IPhiWindow / 2 - 1))) continue ;

			if(isDeadTriggerTower) GaussianFits[iPhi][iEta] = new TF1(std::string(Form("Gaus_%d_%d", iPhi, iEta)).c_str(), "gaus", 0., 5);
			else GaussianFits[iPhi][iEta] = new TF1(std::string(Form("Gaus_%d_%d", iPhi, iEta)).c_str(), "gaus", 0.9, 1.5);
			if(isDeadTriggerTower) GaussianFits_EBP[iPhi][iEta] = new TF1(std::string(Form("Gaus_EBP_%d_%d", iPhi, iEta)).c_str(), "gaus", 0., 5);
			else GaussianFits_EBP[iPhi][iEta] = new TF1(std::string(Form("Gaus_EBP_%d_%d", iPhi, iEta)).c_str(), "gaus", 0.9, 1.5);
			if(isDeadTriggerTower) GaussianFits_EBM[iPhi][iEta] = new TF1(std::string(Form("Gaus_EBM_%d_%d", iPhi, iEta)).c_str(), "gaus", 0., 5);
			else GaussianFits_EBM[iPhi][iEta] = new TF1(std::string(Form("Gaus_EBM_%d_%d", iPhi, iEta)).c_str(), "gaus", 0.9, 1.5);

			for(int iEtaBin = 0 ; iEtaBin < int(iEtaBinDivision.size() - 1) ; iEtaBin++) {
				if(isDeadTriggerTower) GaussianFits_EtaBinned[iEtaBin][iPhi][iEta] = new TF1(std::string(Form("Gaus_EtaBin_%d_%d_iPhi_%d_iEta_%d",
					        int(iEtaBinDivision.at(iEtaBin)), int(iEtaBinDivision.at(iEtaBin + 1)), iPhi, iEta)).c_str(), "gaus", 0., 100);
				else GaussianFits_EtaBinned[iEtaBin][iPhi][iEta] = new TF1(std::string(Form("Gaus_EtaBin_%d_%d_iPhi_%d_iEta_%d",
					        int(iEtaBinDivision.at(iEtaBin)), int(iEtaBinDivision.at(iEtaBin + 1)), iPhi, iEta)).c_str(), "gaus", 0.9, 1.4);
			}

			for(int iPhiBin = 0 ; iPhiBin < int(iPhiBinDivision.size() - 1) ; iPhiBin++) {

				if(isDeadTriggerTower) GaussianFits_PhiBinned[iPhiBin][iPhi][iEta] = new TF1(std::string(Form("Gaus_PhiBin_%d_%d_iPhi_%d_iEta_%d",
					        int(iPhiBinDivision.at(iPhiBin)), int(iPhiBinDivision.at(iPhiBin + 1)), iPhi, iEta)).c_str(), "gaus", 0., 100);
				else GaussianFits_PhiBinned[iPhiBin][iPhi][iEta] = new TF1(std::string(Form("Gaus_PhiBin_%d_%d_iPhi_%d_iEta_%d",
					        int(iPhiBinDivision.at(iPhiBin)), int(iPhiBinDivision.at(iPhiBin + 1)), iPhi, iEta)).c_str(), "gaus", 0.9, 1.5);
			}

			for(int iSMBin = 0 ; iSMBin < int(20 / NPhiSMBinDivision) ; iSMBin++) {

				if(isDeadTriggerTower) GaussianFits_SMBinned[iSMBin][iPhi][iEta] = new TF1(std::string(Form("Gaus_SMBin_%d_%d_iPhi_%d_iEta_%d",
					        iSMBin * NPhiSMBinDivision, NPhiSMBinDivision * (iSMBin + 1), iPhi, iEta)).c_str(), "gaus", 0., 100);
				else GaussianFits_SMBinned[iSMBin][iPhi][iEta] = new TF1(std::string(Form("Gaus_SMBin_%d_%d_iPhi_%d_iEta_%d",
					        iSMBin * NPhiSMBinDivision, NPhiSMBinDivision * (iSMBin + 1), iPhi, iEta)).c_str(), "gaus", 0.9, 1.5);
			}

			MeanIC_EtaBinned[iPhi][iEta]     = new TH1F(std::string(Form("MeanIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "", int(iEtaBinDivision.size() - 1), EtaBin);
			Can_MeanIC_EtaBinned[iPhi][iEta] = new TCanvas(std::string(Form("Can_MeanIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "", 500, 500);

			RMSIC_EtaBinned[iPhi][iEta]      = new TH1F(std::string(Form("RMSIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "", int(iEtaBinDivision.size() - 1), EtaBin);
			Can_RMSIC_EtaBinned[iPhi][iEta]  = new TCanvas(std::string(Form("Can_RMSIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "", 500, 500);

			MeanIC_PhiBinned[iPhi][iEta]     = new TH1F(std::string(Form("MeanIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "", int(iPhiBinDivision.size() - 1), PhiBin);
			Can_MeanIC_PhiBinned[iPhi][iEta] = new TCanvas(std::string(Form("Can_MeanIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "", 500, 500);

			RMSIC_PhiBinned[iPhi][iEta]      = new TH1F(std::string(Form("RMSIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "", int(iPhiBinDivision.size() - 1), PhiBin);
			Can_RMSIC_PhiBinned[iPhi][iEta]  = new TCanvas(std::string(Form("Can_RMSIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "", 500, 500);

			MeanIC_SMBinned[iPhi][iEta]      = new TH1F(std::string(Form("MeanIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "", int(20 / NPhiSMBinDivision), 0, int(20 / NPhiSMBinDivision));
			Can_MeanIC_SMBinned[iPhi][iEta]  = new TCanvas(std::string(Form("Can_MeanIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "", 500, 500);

			RMSIC_SMBinned[iPhi][iEta]     = new TH1F(std::string(Form("RMSIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "", int(20 / NPhiSMBinDivision), 0, int(20 / NPhiSMBinDivision));
			Can_RMSIC_SMBinned[iPhi][iEta] = new TCanvas(std::string(Form("Can_RMSIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "", 500, 500);

			Gaus_MeanIC_EtaBinned[iPhi][iEta]     = new TH1F(std::string(Form("Gaus_MeanIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "", int(iEtaBinDivision.size() - 1), EtaBin);
			Can_Gaus_MeanIC_EtaBinned[iPhi][iEta] = new TCanvas(std::string(Form("Can_Gaus_MeanIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "", 500, 500);

			Gaus_RMSIC_EtaBinned[iPhi][iEta]     = new TH1F(std::string(Form("Gaus_RMSIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "", int(iEtaBinDivision.size() - 1), EtaBin);
			Can_Gaus_RMSIC_EtaBinned[iPhi][iEta] = new TCanvas(std::string(Form("Can_Gaus_RMSIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "", 500, 500);

			Gaus_MeanIC_PhiBinned[iPhi][iEta]     = new TH1F(std::string(Form("Gaus_MeanIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "", int(iPhiBinDivision.size() - 1), PhiBin);
			Can_Gaus_MeanIC_PhiBinned[iPhi][iEta] = new TCanvas(std::string(Form("Can_Gaus_MeanIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "", 500, 500);

			Gaus_RMSIC_PhiBinned[iPhi][iEta]     = new TH1F(std::string(Form("Gaus_RMSIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "", int(iPhiBinDivision.size() - 1), PhiBin);
			Can_Gaus_RMSIC_PhiBinned[iPhi][iEta] = new TCanvas(std::string(Form("Can_Gaus_RMSIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "", 500, 500);

			Gaus_MeanIC_SMBinned[iPhi][iEta]     = new TH1F(std::string(Form("Gaus_MeanIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "", int(20 / NPhiSMBinDivision), 0, int(20 / NPhiSMBinDivision));
			Can_Gaus_MeanIC_SMBinned[iPhi][iEta] = new TCanvas(std::string(Form("Can_Gaus_MeanIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "", 500, 500);

			Gaus_RMSIC_SMBinned[iPhi][iEta]     = new TH1F(std::string(Form("Gaus_RMSIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "", int(20 / NPhiSMBinDivision), 0, int(20 / NPhiSMBinDivision));
			Can_Gaus_RMSIC_SMBinned[iPhi][iEta] = new TCanvas(std::string(Form("Can_Gaus_RMSIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "", 500, 500);

			Pol0_MeanIC_EtaBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol0_MeanIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "pol0", iEtaBinDivision.front(), iEtaBinDivision.back());
			Pol1_MeanIC_EtaBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol1_MeanIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "pol1", iEtaBinDivision.front(), iEtaBinDivision.back());
			Pol2_MeanIC_EtaBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol2_MeanIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "pol2", iEtaBinDivision.front(), iEtaBinDivision.back());

			Pol0_RMSIC_EtaBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol0_RMSIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "pol0", iEtaBinDivision.front(), iEtaBinDivision.back());
			Pol1_RMSIC_EtaBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol1_RMSIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "pol1", iEtaBinDivision.front(), iEtaBinDivision.back());
			Pol2_RMSIC_EtaBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol2_RMSIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "pol2", iEtaBinDivision.front(), iEtaBinDivision.back());

			Pol0_Gaus_MeanIC_EtaBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol0_Gaus_MeanIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "pol0", iEtaBinDivision.front(), iEtaBinDivision.back());
			Pol1_Gaus_MeanIC_EtaBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol1_Gaus_MeanIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "pol1", iEtaBinDivision.front(), iEtaBinDivision.back());
			Pol2_Gaus_MeanIC_EtaBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol2_Gaus_MeanIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "pol2", iEtaBinDivision.front(), iEtaBinDivision.back());

			Pol0_Gaus_RMSIC_EtaBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol0_Gaus_RMSIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "pol0", iEtaBinDivision.front(), iEtaBinDivision.back());
			Pol1_Gaus_RMSIC_EtaBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol1_Gaus_RMSIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "pol1", iEtaBinDivision.front(), iEtaBinDivision.back());
			Pol2_Gaus_RMSIC_EtaBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol2_Gaus_RMSIC_EtaBinned_%d_%d", iPhi, iEta)).c_str(), "pol2", iEtaBinDivision.front(), iEtaBinDivision.back());

			Pol0_MeanIC_PhiBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol0_MeanIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "pol0", iPhiBinDivision.front(), iPhiBinDivision.back());
			Pol1_MeanIC_PhiBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol1_MeanIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "pol1", iPhiBinDivision.front(), iPhiBinDivision.back());
			Pol2_MeanIC_PhiBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol2_MeanIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "pol2", iPhiBinDivision.front(), iPhiBinDivision.back());

			Pol0_RMSIC_PhiBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol0_RMSIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "pol0", iPhiBinDivision.front(), iPhiBinDivision.back());
			Pol1_RMSIC_PhiBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol1_RMSIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "pol1", iPhiBinDivision.front(), iPhiBinDivision.back());
			Pol2_RMSIC_PhiBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol2_RMSIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "pol2", iPhiBinDivision.front(), iPhiBinDivision.back());

			Pol0_Gaus_MeanIC_PhiBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol0_Gaus_MeanIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "pol0", iPhiBinDivision.front(), iPhiBinDivision.back());
			Pol1_Gaus_MeanIC_PhiBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol1_Gaus_MeanIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "pol1", iPhiBinDivision.front(), iPhiBinDivision.back());
			Pol2_Gaus_MeanIC_PhiBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol2_Gaus_MeanIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "pol2", iPhiBinDivision.front(), iPhiBinDivision.back());

			Pol0_Gaus_RMSIC_PhiBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol0_Gaus_RMSIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "pol0", iPhiBinDivision.front(), iPhiBinDivision.back());
			Pol1_Gaus_RMSIC_PhiBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol1_Gaus_RMSIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "pol1", iPhiBinDivision.front(), iPhiBinDivision.back());
			Pol2_Gaus_RMSIC_PhiBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol2_Gaus_RMSIC_PhiBinned_%d_%d", iPhi, iEta)).c_str(), "pol2", iPhiBinDivision.front(), iPhiBinDivision.back());

			Pol0_MeanIC_SMBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol0_MeanIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "pol0", 0, int(20 / NPhiSMBinDivision));
			Pol1_MeanIC_SMBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol1_MeanIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "pol1", 0, int(20 / NPhiSMBinDivision));
			Pol2_MeanIC_SMBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol2_MeanIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "pol2", 0, int(20 / NPhiSMBinDivision));

			Pol0_RMSIC_SMBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol0_RMSIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "pol0", 0, int(20 / NPhiSMBinDivision));
			Pol1_RMSIC_SMBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol1_RMSIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "pol1", 0, int(20 / NPhiSMBinDivision));
			Pol2_RMSIC_SMBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol2_RMSIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "pol2", 0, int(20 / NPhiSMBinDivision));

			Pol0_Gaus_MeanIC_SMBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol0_Gaus_MeanIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "pol0", 0, int(20 / NPhiSMBinDivision));
			Pol1_Gaus_MeanIC_SMBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol1_Gaus_MeanIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "pol1", 0, int(20 / NPhiSMBinDivision));
			Pol2_Gaus_MeanIC_SMBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol2_Gaus_MeanIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "pol2", 0, int(20 / NPhiSMBinDivision));

			Pol0_Gaus_RMSIC_SMBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol0_Gaus_RMSIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "pol0", 0, int(20 / NPhiSMBinDivision));
			Pol1_Gaus_RMSIC_SMBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol1_Gaus_RMSIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "pol1", 0, int(20 / NPhiSMBinDivision));
			Pol2_Gaus_RMSIC_SMBinned[iPhi][iEta]  = new TF1(std::string(Form("Pol2_Gaus_RMSIC_SMBinned_%d_%d", iPhi, iEta)).c_str(), "pol2", 0, int(20 / NPhiSMBinDivision));

			/// Original IC distribution
			(Can.at(iPhi)).at(iEta)->cd();
			(Can.at(iPhi)).at(iEta)->SetGridx();
			(Can.at(iPhi)).at(iEta)->SetGridy();
			ICCrystalEB[iPhi][iEta]->SetLineColor(kBlack);
			ICCrystalEB[iPhi][iEta]->SetLineWidth(2);
			ICCrystalEB[iPhi][iEta]->SetMarkerStyle(20);
			ICCrystalEB[iPhi][iEta]->SetMarkerSize(1.);
			ICCrystalEB[iPhi][iEta]->SetMarkerColor(kRed);
			if(!isDeadTriggerTower) {
				GaussianFits[iPhi][iEta]->SetLineColor(kBlue);
				GaussianFits[iPhi][iEta]->SetLineWidth(2);
				ICCrystalEB[iPhi][iEta]->Fit(GaussianFits[iPhi][iEta], "RMEQ");
			}
			ICCrystalEB[iPhi][iEta]->Draw("E");
			ICCrystalEB[iPhi][iEta]->GetXaxis()->SetTitle("IC");
			ICCrystalEB[iPhi][iEta]->GetYaxis()->SetTitle("Entries");
			(Can.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can.at(iPhi)).at(iEta)->GetName()) + ".pdf").c_str(), "pdf");
			(Can.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can.at(iPhi)).at(iEta)->GetName()) + ".png").c_str(), "png");

			(Can_EBP.at(iPhi)).at(iEta)->cd();
			(Can_EBP.at(iPhi)).at(iEta)->SetGridx();
			(Can_EBP.at(iPhi)).at(iEta)->SetGridy();
			ICCrystalEBP[iPhi][iEta]->SetLineColor(kBlack);
			ICCrystalEBP[iPhi][iEta]->SetLineWidth(2);
			ICCrystalEBP[iPhi][iEta]->SetMarkerStyle(20);
			ICCrystalEBP[iPhi][iEta]->SetMarkerSize(1.);
			ICCrystalEBP[iPhi][iEta]->SetMarkerColor(kRed);
			if(!isDeadTriggerTower) {
				GaussianFits_EBP[iPhi][iEta]->SetLineColor(kBlue);
				GaussianFits_EBP[iPhi][iEta]->SetLineWidth(2);
				ICCrystalEBP[iPhi][iEta]->Fit(GaussianFits_EBP[iPhi][iEta], "RMEQ");
			}
			ICCrystalEBP[iPhi][iEta]->Draw("E");
			ICCrystalEBP[iPhi][iEta]->GetXaxis()->SetTitle("IC");
			ICCrystalEBP[iPhi][iEta]->GetYaxis()->SetTitle("Entries");
			(Can_EBP.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_EBP.at(iPhi)).at(iEta)->GetName()) + ".pdf").c_str(), "pdf");
			(Can_EBP.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_EBP.at(iPhi)).at(iEta)->GetName()) + ".png").c_str(), "png");

			(Can_EBM.at(iPhi)).at(iEta)->cd();
			(Can_EBM.at(iPhi)).at(iEta)->SetGridx();
			(Can_EBM.at(iPhi)).at(iEta)->SetGridy();
			ICCrystalEBM[iPhi][iEta]->SetLineColor(kBlack);
			ICCrystalEBM[iPhi][iEta]->SetLineWidth(2);
			ICCrystalEBM[iPhi][iEta]->SetMarkerStyle(20);
			ICCrystalEBM[iPhi][iEta]->SetMarkerSize(1.);
			ICCrystalEBM[iPhi][iEta]->SetMarkerColor(kRed);
			if(!isDeadTriggerTower) {
				GaussianFits_EBM[iPhi][iEta]->SetLineColor(kBlue);
				GaussianFits_EBM[iPhi][iEta]->SetLineWidth(2);
				ICCrystalEBM[iPhi][iEta]->Fit(GaussianFits_EBM[iPhi][iEta], "RMEQ");
			}
			ICCrystalEBM[iPhi][iEta]->Draw("E");
			ICCrystalEBM[iPhi][iEta]->GetXaxis()->SetTitle("IC");
			ICCrystalEBM[iPhi][iEta]->GetYaxis()->SetTitle("Entries");
			(Can_EBM.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_EBM.at(iPhi)).at(iEta)->GetName()) + ".pdf").c_str(), "pdf");
			(Can_EBM.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_EBM.at(iPhi)).at(iEta)->GetName()) + ".png").c_str(), "png");

			/// Original IC distribution vs the chosen eta binning
			for(int iEtaBin = 0 ; iEtaBin < int(iEtaBinDivision.size() - 1) ; iEtaBin++) {
				((Can_EtaBinned.at(iEtaBin)).at(iPhi)).at(iEta)->cd();
				((Can_EtaBinned.at(iEtaBin)).at(iPhi)).at(iEta)->SetGridx();
				((Can_EtaBinned.at(iEtaBin)).at(iPhi)).at(iEta)->SetGridy();
				ICCrystalEB_EtaBinned[iEtaBin][iPhi][iEta]->SetLineColor(kBlack);
				ICCrystalEB_EtaBinned[iEtaBin][iPhi][iEta]->SetLineWidth(2);
				ICCrystalEB_EtaBinned[iEtaBin][iPhi][iEta]->SetMarkerStyle(20);
				ICCrystalEB_EtaBinned[iEtaBin][iPhi][iEta]->SetMarkerSize(1.);
				ICCrystalEB_EtaBinned[iEtaBin][iPhi][iEta]->SetMarkerColor(kRed);
				if(!isDeadTriggerTower) {
					GaussianFits_EtaBinned[iEtaBin][iPhi][iEta]->SetLineColor(kBlue);
					GaussianFits_EtaBinned[iEtaBin][iPhi][iEta]->SetLineWidth(2);
					ICCrystalEB_EtaBinned[iEtaBin][iPhi][iEta]->Fit(GaussianFits_EtaBinned[iEtaBin][iPhi][iEta], "RMEQ");
				}
				ICCrystalEB_EtaBinned[iEtaBin][iPhi][iEta]->Draw("E");
				ICCrystalEB_EtaBinned[iEtaBin][iPhi][iEta]->GetXaxis()->SetTitle("IC");
				ICCrystalEB_EtaBinned[iEtaBin][iPhi][iEta]->GetYaxis()->SetTitle("Entries");
				((Can_EtaBinned.at(iEtaBin)).at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string(((Can_EtaBinned.at(iEtaBin)).at(iPhi)).at(iEta)->GetName()) + ".pdf").c_str(), "pdf");
				((Can_EtaBinned.at(iEtaBin)).at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string(((Can_EtaBinned.at(iEtaBin)).at(iPhi)).at(iEta)->GetName()) + ".png").c_str(), "png");
			}

			/// Mean IC vs Eta

			(Can_MeanIC_EtaBinned.at(iPhi)).at(iEta)->cd();
			(Can_MeanIC_EtaBinned.at(iPhi)).at(iEta)->SetGridx();
			(Can_MeanIC_EtaBinned.at(iPhi)).at(iEta)->SetGridy();

			MeanIC_EtaBinned[iPhi][iEta]->SetLineColor(kBlack);
			MeanIC_EtaBinned[iPhi][iEta]->SetLineWidth(2);
			MeanIC_EtaBinned[iPhi][iEta]->SetMarkerStyle(20);
			MeanIC_EtaBinned[iPhi][iEta]->SetMarkerSize(1.);

			for(int iBin = 0; iBin < MeanIC_EtaBinned[iPhi][iEta]->GetNbinsX() ; iBin++) {
				MeanIC_EtaBinned[iPhi][iEta]->SetBinContent(iBin + 1, ICCrystalEB_EtaBinned[iBin][iPhi][iEta]->GetMean());
				MeanIC_EtaBinned[iPhi][iEta]->SetBinError(iBin + 1, ICCrystalEB_EtaBinned[iBin][iPhi][iEta]->GetMeanError());
			}


			Pol0_MeanIC_EtaBinned[iPhi][iEta] -> SetLineColor(kBlue);
			Pol1_MeanIC_EtaBinned[iPhi][iEta] -> SetLineColor(kGreen + 1);
			Pol2_MeanIC_EtaBinned[iPhi][iEta] -> SetLineColor(kRed);
			Pol0_MeanIC_EtaBinned[iPhi][iEta] -> SetLineWidth(2);
			Pol1_MeanIC_EtaBinned[iPhi][iEta] -> SetLineWidth(2);
			Pol2_MeanIC_EtaBinned[iPhi][iEta] -> SetLineWidth(2);

			MeanIC_EtaBinned[iPhi][iEta]->Fit(Pol2_MeanIC_EtaBinned[iPhi][iEta], "RMEQ0");
			htemp1 = (TH1F*) MeanIC_EtaBinned[iPhi][iEta]->Clone("htemp1");
			htemp1->Fit(Pol1_MeanIC_EtaBinned[iPhi][iEta], "RMEQ0");
			htemp2 = (TH1F*) MeanIC_EtaBinned[iPhi][iEta]->Clone("htemp2");
			htemp2->Fit(Pol0_MeanIC_EtaBinned[iPhi][iEta], "RMEQ0");

			MeanIC_EtaBinned[iPhi][iEta]->Draw("E");
			MeanIC_EtaBinned[iPhi][iEta]->GetXaxis()->SetTitle("i#eta ");
			MeanIC_EtaBinned[iPhi][iEta]->GetYaxis()->SetTitle("IC Mean");
			MeanIC_EtaBinned[iPhi][iEta]->GetYaxis()->SetRangeUser(MeanIC_EtaBinned[iPhi][iEta]->GetMinimum() * 0.95, MeanIC_EtaBinned[iPhi][iEta]->GetMaximum() * 1.15);

			(Can_MeanIC_EtaBinned.at(iPhi)).at(iEta)->Update();
			pave1 = (TPaveStats*)(MeanIC_EtaBinned[iPhi][iEta]->GetListOfFunctions()->FindObject("stats"));
			pave1->SetTextColor(kRed);
			pave1->SetBorderSize(2);
			pave1->SetLineColor(kRed);
			pave1->SetFillStyle(0);
			pave1->SetY1NDC(0.6);
			pave1->SetY2NDC(0.9);
			(Can_MeanIC_EtaBinned.at(iPhi)).at(iEta)->Modified();

			htemp1->Draw("Esames");
			(Can_MeanIC_EtaBinned.at(iPhi)).at(iEta)->Update();
			pave2 = (TPaveStats*)(htemp1->GetListOfFunctions()->FindObject("stats"));
			pave2->SetOptStat(000);
			pave2->SetBorderSize(2);
			pave2->SetLineColor(kGreen + 1);
			pave2->SetFillStyle(0);
			pave2->SetTextSize(pave2->GetTextSize() * 1.7);
			pave2->SetTextColor(kGreen + 1);
			pave2->SetX1NDC(0.35);
			pave2->SetX2NDC(0.6);
			pave2->SetY1NDC(0.6);
			pave2->SetY2NDC(0.9);
			(Can_MeanIC_EtaBinned.at(iPhi)).at(iEta)->Modified();

			htemp2->Draw("Esames");
			(Can_MeanIC_EtaBinned.at(iPhi)).at(iEta)->Update();
			pave3 = (TPaveStats*)(htemp2->GetListOfFunctions()->FindObject("stats"));
			pave3->SetOptStat(000);
			pave3->SetBorderSize(2);
			pave3->SetLineColor(kBlue);
			pave3->SetFillStyle(0);
			pave3->SetTextSize(pave3->GetTextSize() * 1.7);
			pave3->SetTextColor(kBlue);
			pave3->SetX1NDC(0.11);
			pave3->SetX2NDC(0.33);
			pave3->SetY1NDC(0.6);
			pave3->SetY2NDC(0.9);
			(Can_MeanIC_EtaBinned.at(iPhi)).at(iEta)->Modified();

			Pol0_MeanIC_EtaBinned[iPhi][iEta] -> Draw("Lsame");
			Pol1_MeanIC_EtaBinned[iPhi][iEta] -> Draw("Lsame");
			Pol2_MeanIC_EtaBinned[iPhi][iEta] -> Draw("Lsame");

			(Can_MeanIC_EtaBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_MeanIC_EtaBinned.at(iPhi)).at(iEta)->GetName()) + ".pdf").c_str(), "pdf");
			(Can_MeanIC_EtaBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_MeanIC_EtaBinned.at(iPhi)).at(iEta)->GetName()) + ".png").c_str(), "png");

			if(htemp1 != 0) delete htemp1 ;
			if(htemp2 != 0) delete htemp2 ;

			/// RMS IC vs Eta

			(Can_RMSIC_EtaBinned.at(iPhi)).at(iEta)->cd();
			(Can_RMSIC_EtaBinned.at(iPhi)).at(iEta)->SetGridx();
			(Can_RMSIC_EtaBinned.at(iPhi)).at(iEta)->SetGridy();

			RMSIC_EtaBinned[iPhi][iEta]->SetLineColor(kBlack);
			RMSIC_EtaBinned[iPhi][iEta]->SetLineWidth(2);
			RMSIC_EtaBinned[iPhi][iEta]->SetMarkerStyle(20);
			RMSIC_EtaBinned[iPhi][iEta]->SetMarkerSize(1.);

			for(int iBin = 0; iBin < RMSIC_EtaBinned[iPhi][iEta]->GetNbinsX() ; iBin++) {
				RMSIC_EtaBinned[iPhi][iEta]->SetBinContent(iBin + 1, ICCrystalEB_EtaBinned[iBin][iPhi][iEta]->GetRMS());
				RMSIC_EtaBinned[iPhi][iEta]->SetBinError(iBin + 1, ICCrystalEB_EtaBinned[iBin][iPhi][iEta]->GetRMSError());
			}


			Pol0_RMSIC_EtaBinned[iPhi][iEta] -> SetLineColor(kBlue);
			Pol1_RMSIC_EtaBinned[iPhi][iEta] -> SetLineColor(kGreen + 1);
			Pol2_RMSIC_EtaBinned[iPhi][iEta] -> SetLineColor(kRed);
			Pol0_RMSIC_EtaBinned[iPhi][iEta] -> SetLineWidth(2);
			Pol1_RMSIC_EtaBinned[iPhi][iEta] -> SetLineWidth(2);
			Pol2_RMSIC_EtaBinned[iPhi][iEta] -> SetLineWidth(2);

			RMSIC_EtaBinned[iPhi][iEta]->Fit(Pol2_RMSIC_EtaBinned[iPhi][iEta], "RMEQ0");
			htemp1 = (TH1F*) RMSIC_EtaBinned[iPhi][iEta]->Clone("htemp1");
			htemp1->Fit(Pol1_RMSIC_EtaBinned[iPhi][iEta], "RMEQ0");
			htemp2 = (TH1F*) RMSIC_EtaBinned[iPhi][iEta]->Clone("htemp2");
			htemp2->Fit(Pol0_RMSIC_EtaBinned[iPhi][iEta], "RMEQ0");

			RMSIC_EtaBinned[iPhi][iEta]->Draw("E");
			RMSIC_EtaBinned[iPhi][iEta]->GetXaxis()->SetTitle("i#eta ");
			RMSIC_EtaBinned[iPhi][iEta]->GetYaxis()->SetTitle("IC RMS");
			RMSIC_EtaBinned[iPhi][iEta]->GetYaxis()->SetRangeUser(RMSIC_EtaBinned[iPhi][iEta]->GetMinimum() * 0.85, RMSIC_EtaBinned[iPhi][iEta]->GetMaximum() * 1.3);

			(Can_RMSIC_EtaBinned.at(iPhi)).at(iEta)->Update();
			pave1 = (TPaveStats*)(RMSIC_EtaBinned[iPhi][iEta]->GetListOfFunctions()->FindObject("stats"));
			pave1->SetTextColor(kRed);
			pave1->SetBorderSize(2);
			pave1->SetLineColor(kRed);
			pave1->SetFillStyle(0);
			pave1->SetY1NDC(0.6);
			pave1->SetY2NDC(0.9);
			(Can_RMSIC_EtaBinned.at(iPhi)).at(iEta)->Modified();

			htemp1->Draw("Esames");
			(Can_RMSIC_EtaBinned.at(iPhi)).at(iEta)->Update();
			pave2 = (TPaveStats*)(htemp1->GetListOfFunctions()->FindObject("stats"));
			pave2->SetOptStat(000);
			pave2->SetBorderSize(2);
			pave2->SetLineColor(kGreen + 1);
			pave2->SetFillStyle(0);
			pave2->SetTextSize(pave2->GetTextSize() * 1.7);
			pave2->SetTextColor(kGreen + 1);
			pave2->SetX1NDC(0.35);
			pave2->SetX2NDC(0.6);
			pave2->SetY1NDC(0.6);
			pave2->SetY2NDC(0.9);
			(Can_RMSIC_EtaBinned.at(iPhi)).at(iEta)->Modified();

			htemp2->Draw("Esames");
			(Can_RMSIC_EtaBinned.at(iPhi)).at(iEta)->Update();
			pave3 = (TPaveStats*)(htemp2->GetListOfFunctions()->FindObject("stats"));
			pave3->SetOptStat(000);
			pave3->SetBorderSize(2);
			pave3->SetLineColor(kBlue);
			pave3->SetFillStyle(0);
			pave3->SetTextSize(pave3->GetTextSize() * 1.7);
			pave3->SetTextColor(kBlue);
			pave3->SetX1NDC(0.11);
			pave3->SetX2NDC(0.33);
			pave3->SetY1NDC(0.6);
			pave3->SetY2NDC(0.9);
			(Can_RMSIC_EtaBinned.at(iPhi)).at(iEta)->Modified();

			Pol0_RMSIC_EtaBinned[iPhi][iEta] -> Draw("Lsame");
			Pol1_RMSIC_EtaBinned[iPhi][iEta] -> Draw("Lsame");
			Pol2_RMSIC_EtaBinned[iPhi][iEta] -> Draw("Lsame");

			(Can_RMSIC_EtaBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_RMSIC_EtaBinned.at(iPhi)).at(iEta)->GetName()) + ".pdf").c_str(), "pdf");
			(Can_RMSIC_EtaBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_RMSIC_EtaBinned.at(iPhi)).at(iEta)->GetName()) + ".png").c_str(), "png");

			if(htemp1 != 0) delete htemp1 ;
			if(htemp2 != 0) delete htemp2 ;

			/// Gauss Mean  vs Eta
			if(!isDeadTriggerTower) {
				(Can_Gaus_MeanIC_EtaBinned.at(iPhi)).at(iEta)->cd();
				(Can_Gaus_MeanIC_EtaBinned.at(iPhi)).at(iEta)->SetGridx();
				(Can_Gaus_MeanIC_EtaBinned.at(iPhi)).at(iEta)->SetGridy();

				Gaus_MeanIC_EtaBinned[iPhi][iEta]->SetLineColor(kBlack);
				Gaus_MeanIC_EtaBinned[iPhi][iEta]->SetLineWidth(2);
				Gaus_MeanIC_EtaBinned[iPhi][iEta]->SetMarkerStyle(20);
				Gaus_MeanIC_EtaBinned[iPhi][iEta]->SetMarkerSize(1.);

				for(int iBin = 0; iBin < Gaus_MeanIC_EtaBinned[iPhi][iEta]->GetNbinsX() ; iBin++) {
					Gaus_MeanIC_EtaBinned[iPhi][iEta]->SetBinContent(iBin + 1, GaussianFits_EtaBinned[iBin][iPhi][iEta]->GetParameter(1));
					Gaus_MeanIC_EtaBinned[iPhi][iEta]->SetBinError(iBin + 1, GaussianFits_EtaBinned[iBin][iPhi][iEta]->GetParError(1));
				}

				Pol0_Gaus_MeanIC_EtaBinned[iPhi][iEta] -> SetLineColor(kBlue);
				Pol1_Gaus_MeanIC_EtaBinned[iPhi][iEta] -> SetLineColor(kGreen + 1);
				Pol2_Gaus_MeanIC_EtaBinned[iPhi][iEta] -> SetLineColor(kRed);
				Pol0_Gaus_MeanIC_EtaBinned[iPhi][iEta] -> SetLineWidth(2);
				Pol1_Gaus_MeanIC_EtaBinned[iPhi][iEta] -> SetLineWidth(2);
				Pol2_Gaus_MeanIC_EtaBinned[iPhi][iEta] -> SetLineWidth(2);

				Gaus_MeanIC_EtaBinned[iPhi][iEta]->Fit(Pol2_Gaus_MeanIC_EtaBinned[iPhi][iEta], "RMEQ0");
				htemp1 = (TH1F*) Gaus_MeanIC_EtaBinned[iPhi][iEta]->Clone("htemp1");
				htemp1->Fit(Pol1_Gaus_MeanIC_EtaBinned[iPhi][iEta], "RMEQ0");
				htemp2 = (TH1F*) Gaus_MeanIC_EtaBinned[iPhi][iEta]->Clone("htemp2");
				htemp2->Fit(Pol0_Gaus_MeanIC_EtaBinned[iPhi][iEta], "RMEQ0");

				Gaus_MeanIC_EtaBinned[iPhi][iEta]->Draw("E");
				Gaus_MeanIC_EtaBinned[iPhi][iEta]->GetXaxis()->SetTitle("i#eta");
				Gaus_MeanIC_EtaBinned[iPhi][iEta]->GetYaxis()->SetTitle("IC Gaussian Mean");
				Gaus_MeanIC_EtaBinned[iPhi][iEta]->GetYaxis()->SetRangeUser(Gaus_MeanIC_EtaBinned[iPhi][iEta]->GetMinimum() * 0.95, Gaus_MeanIC_EtaBinned[iPhi][iEta]->GetMaximum() * 1.15);

				(Can_Gaus_MeanIC_EtaBinned.at(iPhi)).at(iEta)->Update();
				pave1 = (TPaveStats*)(Gaus_MeanIC_EtaBinned[iPhi][iEta]->GetListOfFunctions()->FindObject("stats"));
				pave1->SetTextColor(kRed);
				pave1->SetBorderSize(2);
				pave1->SetLineColor(kRed);
				pave1->SetFillStyle(0);
				pave1->SetY1NDC(0.6);
				pave1->SetY2NDC(0.9);
				(Can_Gaus_MeanIC_EtaBinned.at(iPhi)).at(iEta)->Modified();

				htemp1->Draw("Esames");
				(Can_Gaus_MeanIC_EtaBinned.at(iPhi)).at(iEta)->Update();
				pave2 = (TPaveStats*)(htemp1->GetListOfFunctions()->FindObject("stats"));
				pave2->SetOptStat(000);
				pave2->SetBorderSize(2);
				pave2->SetLineColor(kGreen + 1);
				pave2->SetFillStyle(0);
				pave2->SetTextSize(pave2->GetTextSize() * 1.7);
				pave2->SetTextColor(kGreen + 1);
				pave2->SetX1NDC(0.35);
				pave2->SetX2NDC(0.6);
				pave2->SetY1NDC(0.6);
				pave2->SetY2NDC(0.9);
				(Can_Gaus_MeanIC_EtaBinned.at(iPhi)).at(iEta)->Modified();

				htemp2->Draw("Esames");
				(Can_Gaus_MeanIC_EtaBinned.at(iPhi)).at(iEta)->Update();
				pave3 = (TPaveStats*)(htemp2->GetListOfFunctions()->FindObject("stats"));
				pave3->SetOptStat(000);
				pave3->SetBorderSize(2);
				pave3->SetLineColor(kBlue);
				pave3->SetFillStyle(0);
				pave3->SetTextSize(pave3->GetTextSize() * 1.7);
				pave3->SetTextColor(kBlue);
				pave3->SetX1NDC(0.11);
				pave3->SetX2NDC(0.33);
				pave3->SetY1NDC(0.6);
				pave3->SetY2NDC(0.9);
				(Can_Gaus_MeanIC_EtaBinned.at(iPhi)).at(iEta)->Modified();

				Pol0_Gaus_MeanIC_EtaBinned[iPhi][iEta] -> Draw("Lsame");
				Pol1_Gaus_MeanIC_EtaBinned[iPhi][iEta] -> Draw("Lsame");
				Pol2_Gaus_MeanIC_EtaBinned[iPhi][iEta] -> Draw("Lsame");

				(Can_Gaus_MeanIC_EtaBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_Gaus_MeanIC_EtaBinned.at(iPhi)).at(iEta)->GetName()) + ".pdf").c_str(), "pdf");
				(Can_Gaus_MeanIC_EtaBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_Gaus_MeanIC_EtaBinned.at(iPhi)).at(iEta)->GetName()) + ".png").c_str(), "png");
				(Can_Gaus_MeanIC_EtaBinned.at(iPhi)).at(iEta)->Close();

				delete htemp1 ;
				delete htemp2 ;

				/// Gauss Sigma  vs Eta

				(Can_Gaus_RMSIC_EtaBinned.at(iPhi)).at(iEta)->cd();
				(Can_Gaus_RMSIC_EtaBinned.at(iPhi)).at(iEta)->SetGridx();
				(Can_Gaus_RMSIC_EtaBinned.at(iPhi)).at(iEta)->SetGridy();

				Gaus_RMSIC_EtaBinned[iPhi][iEta]->SetLineColor(kBlack);
				Gaus_RMSIC_EtaBinned[iPhi][iEta]->SetLineWidth(2);
				Gaus_RMSIC_EtaBinned[iPhi][iEta]->SetMarkerStyle(20);
				Gaus_RMSIC_EtaBinned[iPhi][iEta]->SetMarkerSize(1.);

				for(int iBin = 0; iBin < Gaus_RMSIC_EtaBinned[iPhi][iEta]->GetNbinsX() ; iBin++) {
					Gaus_RMSIC_EtaBinned[iPhi][iEta]->SetBinContent(iBin + 1, GaussianFits_EtaBinned[iBin][iPhi][iEta]->GetParameter(2));
					Gaus_RMSIC_EtaBinned[iPhi][iEta]->SetBinError(iBin + 1, GaussianFits_EtaBinned[iBin][iPhi][iEta]->GetParError(2));
				}

				Pol0_Gaus_RMSIC_EtaBinned[iPhi][iEta] -> SetLineColor(kBlue);
				Pol1_Gaus_RMSIC_EtaBinned[iPhi][iEta] -> SetLineColor(kGreen + 1);
				Pol2_Gaus_RMSIC_EtaBinned[iPhi][iEta] -> SetLineColor(kRed);
				Pol0_Gaus_RMSIC_EtaBinned[iPhi][iEta] -> SetLineWidth(2);
				Pol1_Gaus_RMSIC_EtaBinned[iPhi][iEta] -> SetLineWidth(2);
				Pol2_Gaus_RMSIC_EtaBinned[iPhi][iEta] -> SetLineWidth(2);

				Gaus_RMSIC_EtaBinned[iPhi][iEta]->Fit(Pol2_Gaus_RMSIC_EtaBinned[iPhi][iEta], "RMEQ0");
				htemp1 = (TH1F*) Gaus_RMSIC_EtaBinned[iPhi][iEta]->Clone("htemp1");
				htemp1->Fit(Pol1_Gaus_RMSIC_EtaBinned[iPhi][iEta], "RMEQ0");
				htemp2 = (TH1F*) Gaus_RMSIC_EtaBinned[iPhi][iEta]->Clone("htemp2");
				htemp2->Fit(Pol0_Gaus_RMSIC_EtaBinned[iPhi][iEta], "RMEQ0");

				Gaus_RMSIC_EtaBinned[iPhi][iEta]->Draw("E");
				Gaus_RMSIC_EtaBinned[iPhi][iEta]->GetXaxis()->SetTitle("i#eta");
				Gaus_RMSIC_EtaBinned[iPhi][iEta]->GetYaxis()->SetTitle("IC Gaussian RMS");
				Gaus_RMSIC_EtaBinned[iPhi][iEta]->GetYaxis()->SetRangeUser(Gaus_RMSIC_EtaBinned[iPhi][iEta]->GetMinimum() * 0.85, Gaus_RMSIC_EtaBinned[iPhi][iEta]->GetMaximum() * 1.3);

				(Can_Gaus_RMSIC_EtaBinned.at(iPhi)).at(iEta)->Update();
				pave1 = (TPaveStats*)(Gaus_RMSIC_EtaBinned[iPhi][iEta]->GetListOfFunctions()->FindObject("stats"));
				pave1->SetTextColor(kRed);
				pave1->SetBorderSize(2);
				pave1->SetLineColor(kRed);
				pave1->SetFillStyle(0);
				pave1->SetY1NDC(0.6);
				pave1->SetY2NDC(0.9);
				(Can_Gaus_RMSIC_EtaBinned.at(iPhi)).at(iEta)->Modified();

				htemp1->Draw("Esames");
				(Can_Gaus_RMSIC_EtaBinned.at(iPhi)).at(iEta)->Update();
				pave2 = (TPaveStats*)(htemp1->GetListOfFunctions()->FindObject("stats"));
				pave2->SetOptStat(000);
				pave2->SetBorderSize(2);
				pave2->SetLineColor(kGreen + 1);
				pave2->SetFillStyle(0);
				pave2->SetTextSize(pave2->GetTextSize() * 1.7);
				pave2->SetTextColor(kGreen + 1);
				pave2->SetX1NDC(0.35);
				pave2->SetX2NDC(0.6);
				pave2->SetY1NDC(0.6);
				pave2->SetY2NDC(0.9);
				(Can_Gaus_RMSIC_EtaBinned.at(iPhi)).at(iEta)->Modified();

				htemp2->Draw("Esames");
				(Can_Gaus_RMSIC_EtaBinned.at(iPhi)).at(iEta)->Update();
				pave3 = (TPaveStats*)(htemp2->GetListOfFunctions()->FindObject("stats"));
				pave3->SetOptStat(000);
				pave3->SetBorderSize(2);
				pave3->SetLineColor(kBlue);
				pave3->SetFillStyle(0);
				pave3->SetTextSize(pave3->GetTextSize() * 1.7);
				pave3->SetTextColor(kBlue);
				pave3->SetX1NDC(0.11);
				pave3->SetX2NDC(0.33);
				pave3->SetY1NDC(0.6);
				pave3->SetY2NDC(0.9);
				(Can_Gaus_RMSIC_EtaBinned.at(iPhi)).at(iEta)->Modified();

				Pol0_Gaus_RMSIC_EtaBinned[iPhi][iEta] -> Draw("Lsame");
				Pol1_Gaus_RMSIC_EtaBinned[iPhi][iEta] -> Draw("Lsame");
				Pol2_Gaus_RMSIC_EtaBinned[iPhi][iEta] -> Draw("Lsame");

				(Can_Gaus_RMSIC_EtaBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_Gaus_RMSIC_EtaBinned.at(iPhi)).at(iEta)->GetName()) + ".pdf").c_str(), "pdf");
				(Can_Gaus_RMSIC_EtaBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_Gaus_RMSIC_EtaBinned.at(iPhi)).at(iEta)->GetName()) + ".png").c_str(), "png");
				(Can_Gaus_RMSIC_EtaBinned.at(iPhi)).at(iEta)->Close();

				delete htemp1 ;
				delete htemp2 ;
			}
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


			/// Original IC distribution vs the chosen eta binning

			for(int iPhiBin = 0 ; iPhiBin < int(iPhiBinDivision.size() - 1) ; iPhiBin++) {
				((Can_PhiBinned.at(iPhiBin)).at(iPhi)).at(iEta)->cd();
				((Can_PhiBinned.at(iPhiBin)).at(iPhi)).at(iEta)->SetGridx();
				((Can_PhiBinned.at(iPhiBin)).at(iPhi)).at(iEta)->SetGridy();
				ICCrystalEB_PhiBinned[iPhiBin][iPhi][iEta]->SetLineColor(kBlack);
				ICCrystalEB_PhiBinned[iPhiBin][iPhi][iEta]->SetLineWidth(2);
				ICCrystalEB_PhiBinned[iPhiBin][iPhi][iEta]->SetMarkerStyle(20);
				ICCrystalEB_PhiBinned[iPhiBin][iPhi][iEta]->SetMarkerSize(1.);
				ICCrystalEB_PhiBinned[iPhiBin][iPhi][iEta]->SetMarkerColor(kRed);
				if(!isDeadTriggerTower) {
					GaussianFits_PhiBinned[iPhiBin][iPhi][iEta]->SetLineColor(kBlue);
					GaussianFits_PhiBinned[iPhiBin][iPhi][iEta]->SetLineWidth(2);
					ICCrystalEB_PhiBinned[iPhiBin][iPhi][iEta]->Fit(GaussianFits_PhiBinned[iPhiBin][iPhi][iEta], "RMEQ");
				}
				ICCrystalEB_PhiBinned[iPhiBin][iPhi][iEta]->Draw("E");
				ICCrystalEB_PhiBinned[iPhiBin][iPhi][iEta]->GetXaxis()->SetTitle("IC");
				ICCrystalEB_PhiBinned[iPhiBin][iPhi][iEta]->GetYaxis()->SetTitle("Entries");
				((Can_PhiBinned.at(iPhiBin)).at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string(((Can_PhiBinned.at(iPhiBin)).at(iPhi)).at(iEta)->GetName()) + ".pdf").c_str(), "pdf");
				((Can_PhiBinned.at(iPhiBin)).at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string(((Can_PhiBinned.at(iPhiBin)).at(iPhi)).at(iEta)->GetName()) + ".png").c_str(), "png");
			}


			/// Mean IC vs Phi

			(Can_MeanIC_PhiBinned.at(iPhi)).at(iEta)->cd();
			(Can_MeanIC_PhiBinned.at(iPhi)).at(iEta)->SetGridx();
			(Can_MeanIC_PhiBinned.at(iPhi)).at(iEta)->SetGridy();

			MeanIC_PhiBinned[iPhi][iEta]->SetLineColor(kBlack);
			MeanIC_PhiBinned[iPhi][iEta]->SetLineWidth(2);
			MeanIC_PhiBinned[iPhi][iEta]->SetMarkerStyle(20);
			MeanIC_PhiBinned[iPhi][iEta]->SetMarkerSize(1.);

			for(int iBin = 0; iBin < MeanIC_PhiBinned[iPhi][iEta]->GetNbinsX() ; iBin++) {
				MeanIC_PhiBinned[iPhi][iEta]->SetBinContent(iBin + 1, ICCrystalEB_PhiBinned[iBin][iPhi][iEta]->GetMean());
				MeanIC_PhiBinned[iPhi][iEta]->SetBinError(iBin + 1, ICCrystalEB_PhiBinned[iBin][iPhi][iEta]->GetMeanError());
			}


			Pol0_MeanIC_PhiBinned[iPhi][iEta] -> SetLineColor(kBlue);
			Pol1_MeanIC_PhiBinned[iPhi][iEta] -> SetLineColor(kGreen + 1);
			Pol2_MeanIC_PhiBinned[iPhi][iEta] -> SetLineColor(kRed);
			Pol0_MeanIC_PhiBinned[iPhi][iEta] -> SetLineWidth(2);
			Pol1_MeanIC_PhiBinned[iPhi][iEta] -> SetLineWidth(2);
			Pol2_MeanIC_PhiBinned[iPhi][iEta] -> SetLineWidth(2);

			MeanIC_PhiBinned[iPhi][iEta]->Fit(Pol2_MeanIC_PhiBinned[iPhi][iEta], "RMEQ0");
			htemp1 = (TH1F*) MeanIC_PhiBinned[iPhi][iEta]->Clone("htemp1");
			htemp1->Fit(Pol1_MeanIC_PhiBinned[iPhi][iEta], "RMEQ0");
			htemp2 = (TH1F*) MeanIC_PhiBinned[iPhi][iEta]->Clone("htemp2");
			htemp2->Fit(Pol0_MeanIC_PhiBinned[iPhi][iEta], "RMEQ0");

			MeanIC_PhiBinned[iPhi][iEta]->Draw("E");
			MeanIC_PhiBinned[iPhi][iEta]->GetXaxis()->SetTitle("i#eta ");
			MeanIC_PhiBinned[iPhi][iEta]->GetYaxis()->SetTitle("IC Mean");
			MeanIC_PhiBinned[iPhi][iEta]->GetYaxis()->SetRangeUser(MeanIC_PhiBinned[iPhi][iEta]->GetMinimum() * 0.98, MeanIC_PhiBinned[iPhi][iEta]->GetMaximum() * 1.05);

			(Can_MeanIC_PhiBinned.at(iPhi)).at(iEta)->Update();
			pave1 = (TPaveStats*)(MeanIC_PhiBinned[iPhi][iEta]->GetListOfFunctions()->FindObject("stats"));
			pave1->SetTextColor(kRed);
			pave1->SetBorderSize(2);
			pave1->SetLineColor(kRed);
			pave1->SetFillStyle(0);
			pave1->SetY1NDC(0.6);
			pave1->SetY2NDC(0.9);
			(Can_MeanIC_PhiBinned.at(iPhi)).at(iEta)->Modified();

			htemp1->Draw("Esames");
			(Can_MeanIC_PhiBinned.at(iPhi)).at(iEta)->Update();
			pave2 = (TPaveStats*)(htemp1->GetListOfFunctions()->FindObject("stats"));
			pave2->SetOptStat(000);
			pave2->SetBorderSize(2);
			pave2->SetLineColor(kGreen + 1);
			pave2->SetFillStyle(0);
			pave2->SetTextSize(pave2->GetTextSize() * 1.7);
			pave2->SetTextColor(kGreen + 1);
			pave2->SetX1NDC(0.35);
			pave2->SetX2NDC(0.6);
			pave2->SetY1NDC(0.6);
			pave2->SetY2NDC(0.9);
			(Can_MeanIC_PhiBinned.at(iPhi)).at(iEta)->Modified();

			htemp2->Draw("Esames");
			(Can_MeanIC_PhiBinned.at(iPhi)).at(iEta)->Update();
			pave3 = (TPaveStats*)(htemp2->GetListOfFunctions()->FindObject("stats"));
			pave3->SetOptStat(000);
			pave3->SetBorderSize(2);
			pave3->SetLineColor(kBlue);
			pave3->SetFillStyle(0);
			pave3->SetTextSize(pave3->GetTextSize() * 1.7);
			pave3->SetTextColor(kBlue);
			pave3->SetX1NDC(0.11);
			pave3->SetX2NDC(0.33);
			pave3->SetY1NDC(0.6);
			pave3->SetY2NDC(0.9);
			(Can_MeanIC_PhiBinned.at(iPhi)).at(iEta)->Modified();

			Pol0_MeanIC_PhiBinned[iPhi][iEta] -> Draw("Lsame");
			Pol1_MeanIC_PhiBinned[iPhi][iEta] -> Draw("Lsame");
			Pol2_MeanIC_PhiBinned[iPhi][iEta] -> Draw("Lsame");

			(Can_MeanIC_PhiBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_MeanIC_PhiBinned.at(iPhi)).at(iEta)->GetName()) + ".pdf").c_str(), "pdf");
			(Can_MeanIC_PhiBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_MeanIC_PhiBinned.at(iPhi)).at(iEta)->GetName()) + ".png").c_str(), "png");

			if(htemp1 != 0) delete htemp1 ;
			if(htemp2 != 0) delete htemp2 ;

			/// RMS IC vs Phi

			(Can_RMSIC_PhiBinned.at(iPhi)).at(iEta)->cd();
			(Can_RMSIC_PhiBinned.at(iPhi)).at(iEta)->SetGridx();
			(Can_RMSIC_PhiBinned.at(iPhi)).at(iEta)->SetGridy();

			RMSIC_PhiBinned[iPhi][iEta]->SetLineColor(kBlack);
			RMSIC_PhiBinned[iPhi][iEta]->SetLineWidth(2);
			RMSIC_PhiBinned[iPhi][iEta]->SetMarkerStyle(20);
			RMSIC_PhiBinned[iPhi][iEta]->SetMarkerSize(1.);

			for(int iBin = 0; iBin < RMSIC_PhiBinned[iPhi][iEta]->GetNbinsX() ; iBin++) {
				RMSIC_PhiBinned[iPhi][iEta]->SetBinContent(iBin + 1, ICCrystalEB_PhiBinned[iBin][iPhi][iEta]->GetRMS());
				RMSIC_PhiBinned[iPhi][iEta]->SetBinError(iBin + 1, ICCrystalEB_PhiBinned[iBin][iPhi][iEta]->GetRMSError());
			}


			Pol0_RMSIC_PhiBinned[iPhi][iEta] -> SetLineColor(kBlue);
			Pol1_RMSIC_PhiBinned[iPhi][iEta] -> SetLineColor(kGreen + 1);
			Pol2_RMSIC_PhiBinned[iPhi][iEta] -> SetLineColor(kRed);
			Pol0_RMSIC_PhiBinned[iPhi][iEta] -> SetLineWidth(2);
			Pol1_RMSIC_PhiBinned[iPhi][iEta] -> SetLineWidth(2);
			Pol2_RMSIC_PhiBinned[iPhi][iEta] -> SetLineWidth(2);

			RMSIC_PhiBinned[iPhi][iEta]->Fit(Pol2_RMSIC_PhiBinned[iPhi][iEta], "RMEQ0");
			htemp1 = (TH1F*) RMSIC_PhiBinned[iPhi][iEta]->Clone("htemp1");
			htemp1->Fit(Pol1_RMSIC_PhiBinned[iPhi][iEta], "RMEQ0");
			htemp2 = (TH1F*) RMSIC_PhiBinned[iPhi][iEta]->Clone("htemp2");
			htemp2->Fit(Pol0_RMSIC_PhiBinned[iPhi][iEta], "RMEQ0");

			RMSIC_PhiBinned[iPhi][iEta]->Draw("E");
			RMSIC_PhiBinned[iPhi][iEta]->GetXaxis()->SetTitle("i#eta ");
			RMSIC_PhiBinned[iPhi][iEta]->GetYaxis()->SetTitle("IC RMS");
			RMSIC_PhiBinned[iPhi][iEta]->GetYaxis()->SetRangeUser(RMSIC_PhiBinned[iPhi][iEta]->GetMinimum() * 0.85, RMSIC_PhiBinned[iPhi][iEta]->GetMaximum() * 1.3);

			(Can_RMSIC_PhiBinned.at(iPhi)).at(iEta)->Update();
			pave1 = (TPaveStats*)(RMSIC_PhiBinned[iPhi][iEta]->GetListOfFunctions()->FindObject("stats"));
			pave1->SetTextColor(kRed);
			pave1->SetBorderSize(2);
			pave1->SetLineColor(kRed);
			pave1->SetFillStyle(0);
			pave1->SetY1NDC(0.6);
			pave1->SetY2NDC(0.9);
			(Can_RMSIC_PhiBinned.at(iPhi)).at(iEta)->Modified();

			htemp1->Draw("Esames");
			(Can_RMSIC_PhiBinned.at(iPhi)).at(iEta)->Update();
			pave2 = (TPaveStats*)(htemp1->GetListOfFunctions()->FindObject("stats"));
			pave2->SetOptStat(000);
			pave2->SetBorderSize(2);
			pave2->SetLineColor(kGreen + 1);
			pave2->SetFillStyle(0);
			pave2->SetTextSize(pave2->GetTextSize() * 1.7);
			pave2->SetTextColor(kGreen + 1);
			pave2->SetX1NDC(0.35);
			pave2->SetX2NDC(0.6);
			pave2->SetY1NDC(0.6);
			pave2->SetY2NDC(0.9);
			(Can_RMSIC_PhiBinned.at(iPhi)).at(iEta)->Modified();

			htemp2->Draw("Esames");
			(Can_RMSIC_PhiBinned.at(iPhi)).at(iEta)->Update();
			pave3 = (TPaveStats*)(htemp2->GetListOfFunctions()->FindObject("stats"));
			pave3->SetOptStat(000);
			pave3->SetBorderSize(2);
			pave3->SetLineColor(kBlue);
			pave3->SetFillStyle(0);
			pave3->SetTextSize(pave3->GetTextSize() * 1.7);
			pave3->SetTextColor(kBlue);
			pave3->SetX1NDC(0.11);
			pave3->SetX2NDC(0.33);
			pave3->SetY1NDC(0.6);
			pave3->SetY2NDC(0.9);
			(Can_RMSIC_PhiBinned.at(iPhi)).at(iEta)->Modified();

			Pol0_RMSIC_PhiBinned[iPhi][iEta] -> Draw("Lsame");
			Pol1_RMSIC_PhiBinned[iPhi][iEta] -> Draw("Lsame");
			Pol2_RMSIC_PhiBinned[iPhi][iEta] -> Draw("Lsame");

			(Can_RMSIC_PhiBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_RMSIC_PhiBinned.at(iPhi)).at(iEta)->GetName()) + ".pdf").c_str(), "pdf");
			(Can_RMSIC_PhiBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_RMSIC_PhiBinned.at(iPhi)).at(iEta)->GetName()) + ".png").c_str(), "png");

			if(htemp1 != 0) delete htemp1 ;
			if(htemp2 != 0) delete htemp2 ;

			/// Gauss Mean  vs Phi
			if(!isDeadTriggerTower) {
				(Can_Gaus_MeanIC_PhiBinned.at(iPhi)).at(iEta)->cd();
				(Can_Gaus_MeanIC_PhiBinned.at(iPhi)).at(iEta)->SetGridx();
				(Can_Gaus_MeanIC_PhiBinned.at(iPhi)).at(iEta)->SetGridy();

				Gaus_MeanIC_PhiBinned[iPhi][iEta]->SetLineColor(kBlack);
				Gaus_MeanIC_PhiBinned[iPhi][iEta]->SetLineWidth(2);
				Gaus_MeanIC_PhiBinned[iPhi][iEta]->SetMarkerStyle(20);
				Gaus_MeanIC_PhiBinned[iPhi][iEta]->SetMarkerSize(1.);

				for(int iBin = 0; iBin < Gaus_MeanIC_PhiBinned[iPhi][iEta]->GetNbinsX() ; iBin++) {
					Gaus_MeanIC_PhiBinned[iPhi][iEta]->SetBinContent(iBin + 1, GaussianFits_PhiBinned[iBin][iPhi][iEta]->GetParameter(1));
					Gaus_MeanIC_PhiBinned[iPhi][iEta]->SetBinError(iBin + 1, GaussianFits_PhiBinned[iBin][iPhi][iEta]->GetParError(1));
				}

				Pol0_Gaus_MeanIC_PhiBinned[iPhi][iEta] -> SetLineColor(kBlue);
				Pol1_Gaus_MeanIC_PhiBinned[iPhi][iEta] -> SetLineColor(kGreen + 1);
				Pol2_Gaus_MeanIC_PhiBinned[iPhi][iEta] -> SetLineColor(kRed);
				Pol0_Gaus_MeanIC_PhiBinned[iPhi][iEta] -> SetLineWidth(2);
				Pol1_Gaus_MeanIC_PhiBinned[iPhi][iEta] -> SetLineWidth(2);
				Pol2_Gaus_MeanIC_PhiBinned[iPhi][iEta] -> SetLineWidth(2);

				Gaus_MeanIC_PhiBinned[iPhi][iEta]->Fit(Pol2_Gaus_MeanIC_PhiBinned[iPhi][iEta], "RMEQ0");
				htemp1 = (TH1F*) Gaus_MeanIC_PhiBinned[iPhi][iEta]->Clone("htemp1");
				htemp1->Fit(Pol1_Gaus_MeanIC_PhiBinned[iPhi][iEta], "RMEQ0");
				htemp2 = (TH1F*) Gaus_MeanIC_PhiBinned[iPhi][iEta]->Clone("htemp2");
				htemp2->Fit(Pol0_Gaus_MeanIC_PhiBinned[iPhi][iEta], "RMEQ0");

				Gaus_MeanIC_PhiBinned[iPhi][iEta]->Draw("E");
				Gaus_MeanIC_PhiBinned[iPhi][iEta]->GetXaxis()->SetTitle("i#eta");
				Gaus_MeanIC_PhiBinned[iPhi][iEta]->GetYaxis()->SetTitle("IC Gaussian Mean");
				Gaus_MeanIC_PhiBinned[iPhi][iEta]->GetYaxis()->SetRangeUser(Gaus_MeanIC_PhiBinned[iPhi][iEta]->GetMinimum() * 0.98, Gaus_MeanIC_PhiBinned[iPhi][iEta]->GetMaximum() * 1.05);

				(Can_Gaus_MeanIC_PhiBinned.at(iPhi)).at(iEta)->Update();
				pave1 = (TPaveStats*)(Gaus_MeanIC_PhiBinned[iPhi][iEta]->GetListOfFunctions()->FindObject("stats"));
				pave1->SetTextColor(kRed);
				pave1->SetBorderSize(2);
				pave1->SetLineColor(kRed);
				pave1->SetFillStyle(0);
				pave1->SetY1NDC(0.6);
				pave1->SetY2NDC(0.9);
				(Can_Gaus_MeanIC_PhiBinned.at(iPhi)).at(iEta)->Modified();

				htemp1->Draw("Esames");
				(Can_Gaus_MeanIC_PhiBinned.at(iPhi)).at(iEta)->Update();
				pave2 = (TPaveStats*)(htemp1->GetListOfFunctions()->FindObject("stats"));
				pave2->SetOptStat(000);
				pave2->SetBorderSize(2);
				pave2->SetLineColor(kGreen + 1);
				pave2->SetFillStyle(0);
				pave2->SetTextSize(pave2->GetTextSize() * 1.7);
				pave2->SetTextColor(kGreen + 1);
				pave2->SetX1NDC(0.35);
				pave2->SetX2NDC(0.6);
				pave2->SetY1NDC(0.6);
				pave2->SetY2NDC(0.9);
				(Can_Gaus_MeanIC_PhiBinned.at(iPhi)).at(iEta)->Modified();

				htemp2->Draw("Esames");
				(Can_Gaus_MeanIC_PhiBinned.at(iPhi)).at(iEta)->Update();
				pave3 = (TPaveStats*)(htemp2->GetListOfFunctions()->FindObject("stats"));
				pave3->SetOptStat(000);
				pave3->SetBorderSize(2);
				pave3->SetLineColor(kBlue);
				pave3->SetFillStyle(0);
				pave3->SetTextSize(pave3->GetTextSize() * 1.7);
				pave3->SetTextColor(kBlue);
				pave3->SetX1NDC(0.11);
				pave3->SetX2NDC(0.33);
				pave3->SetY1NDC(0.6);
				pave3->SetY2NDC(0.9);
				(Can_Gaus_MeanIC_PhiBinned.at(iPhi)).at(iEta)->Modified();

				Pol0_Gaus_MeanIC_PhiBinned[iPhi][iEta] -> Draw("Lsame");
				Pol1_Gaus_MeanIC_PhiBinned[iPhi][iEta] -> Draw("Lsame");
				Pol2_Gaus_MeanIC_PhiBinned[iPhi][iEta] -> Draw("Lsame");

				(Can_Gaus_MeanIC_PhiBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_Gaus_MeanIC_PhiBinned.at(iPhi)).at(iEta)->GetName()) + ".pdf").c_str(), "pdf");
				(Can_Gaus_MeanIC_PhiBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_Gaus_MeanIC_PhiBinned.at(iPhi)).at(iEta)->GetName()) + ".png").c_str(), "png");
				(Can_Gaus_MeanIC_PhiBinned.at(iPhi)).at(iEta)->Close();

				delete htemp1 ;
				delete htemp2 ;

				/// Gauss Sigma  vs Eta

				(Can_Gaus_RMSIC_PhiBinned.at(iPhi)).at(iEta)->cd();
				(Can_Gaus_RMSIC_PhiBinned.at(iPhi)).at(iEta)->SetGridx();
				(Can_Gaus_RMSIC_PhiBinned.at(iPhi)).at(iEta)->SetGridy();

				Gaus_RMSIC_PhiBinned[iPhi][iEta]->SetLineColor(kBlack);
				Gaus_RMSIC_PhiBinned[iPhi][iEta]->SetLineWidth(2);
				Gaus_RMSIC_PhiBinned[iPhi][iEta]->SetMarkerStyle(20);
				Gaus_RMSIC_PhiBinned[iPhi][iEta]->SetMarkerSize(1.);

				for(int iBin = 0; iBin < Gaus_RMSIC_PhiBinned[iPhi][iEta]->GetNbinsX() ; iBin++) {
					Gaus_RMSIC_PhiBinned[iPhi][iEta]->SetBinContent(iBin + 1, GaussianFits_PhiBinned[iBin][iPhi][iEta]->GetParameter(2));
					Gaus_RMSIC_PhiBinned[iPhi][iEta]->SetBinError(iBin + 1, GaussianFits_PhiBinned[iBin][iPhi][iEta]->GetParError(2));
				}

				Pol0_Gaus_RMSIC_PhiBinned[iPhi][iEta] -> SetLineColor(kBlue);
				Pol1_Gaus_RMSIC_PhiBinned[iPhi][iEta] -> SetLineColor(kGreen + 1);
				Pol2_Gaus_RMSIC_PhiBinned[iPhi][iEta] -> SetLineColor(kRed);
				Pol0_Gaus_RMSIC_PhiBinned[iPhi][iEta] -> SetLineWidth(2);
				Pol1_Gaus_RMSIC_PhiBinned[iPhi][iEta] -> SetLineWidth(2);
				Pol2_Gaus_RMSIC_PhiBinned[iPhi][iEta] -> SetLineWidth(2);

				Gaus_RMSIC_PhiBinned[iPhi][iEta]->Fit(Pol2_Gaus_RMSIC_PhiBinned[iPhi][iEta], "RMEQ0");
				htemp1 = (TH1F*) Gaus_RMSIC_PhiBinned[iPhi][iEta]->Clone("htemp1");
				htemp1->Fit(Pol1_Gaus_RMSIC_PhiBinned[iPhi][iEta], "RMEQ0");
				htemp2 = (TH1F*) Gaus_RMSIC_PhiBinned[iPhi][iEta]->Clone("htemp2");
				htemp2->Fit(Pol0_Gaus_RMSIC_PhiBinned[iPhi][iEta], "RMEQ0");

				Gaus_RMSIC_PhiBinned[iPhi][iEta]->Draw("E");
				Gaus_RMSIC_PhiBinned[iPhi][iEta]->GetXaxis()->SetTitle("i#eta");
				Gaus_RMSIC_PhiBinned[iPhi][iEta]->GetYaxis()->SetTitle("IC Gaussian RMS");
				Gaus_RMSIC_PhiBinned[iPhi][iEta]->GetYaxis()->SetRangeUser(Gaus_RMSIC_PhiBinned[iPhi][iEta]->GetMinimum() * 0.85, Gaus_RMSIC_PhiBinned[iPhi][iEta]->GetMaximum() * 1.3);

				(Can_Gaus_RMSIC_PhiBinned.at(iPhi)).at(iEta)->Update();
				pave1 = (TPaveStats*)(Gaus_RMSIC_PhiBinned[iPhi][iEta]->GetListOfFunctions()->FindObject("stats"));
				pave1->SetTextColor(kRed);
				pave1->SetBorderSize(2);
				pave1->SetLineColor(kRed);
				pave1->SetFillStyle(0);
				pave1->SetY1NDC(0.6);
				pave1->SetY2NDC(0.9);
				(Can_Gaus_RMSIC_PhiBinned.at(iPhi)).at(iEta)->Modified();

				htemp1->Draw("Esames");
				(Can_Gaus_RMSIC_PhiBinned.at(iPhi)).at(iEta)->Update();
				pave2 = (TPaveStats*)(htemp1->GetListOfFunctions()->FindObject("stats"));
				pave2->SetOptStat(000);
				pave2->SetBorderSize(2);
				pave2->SetLineColor(kGreen + 1);
				pave2->SetFillStyle(0);
				pave2->SetTextSize(pave2->GetTextSize() * 1.7);
				pave2->SetTextColor(kGreen + 1);
				pave2->SetX1NDC(0.35);
				pave2->SetX2NDC(0.6);
				pave2->SetY1NDC(0.6);
				pave2->SetY2NDC(0.9);
				(Can_Gaus_RMSIC_PhiBinned.at(iPhi)).at(iEta)->Modified();

				htemp2->Draw("Esames");
				(Can_Gaus_RMSIC_PhiBinned.at(iPhi)).at(iEta)->Update();
				pave3 = (TPaveStats*)(htemp2->GetListOfFunctions()->FindObject("stats"));
				pave3->SetOptStat(000);
				pave3->SetBorderSize(2);
				pave3->SetLineColor(kBlue);
				pave3->SetFillStyle(0);
				pave3->SetTextSize(pave3->GetTextSize() * 1.7);
				pave3->SetTextColor(kBlue);
				pave3->SetX1NDC(0.11);
				pave3->SetX2NDC(0.33);
				pave3->SetY1NDC(0.6);
				pave3->SetY2NDC(0.9);
				(Can_Gaus_RMSIC_PhiBinned.at(iPhi)).at(iEta)->Modified();

				Pol0_Gaus_RMSIC_PhiBinned[iPhi][iEta] -> Draw("Lsame");
				Pol1_Gaus_RMSIC_PhiBinned[iPhi][iEta] -> Draw("Lsame");
				Pol2_Gaus_RMSIC_PhiBinned[iPhi][iEta] -> Draw("Lsame");

				(Can_Gaus_RMSIC_PhiBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_Gaus_RMSIC_PhiBinned.at(iPhi)).at(iEta)->GetName()) + ".pdf").c_str(), "pdf");
				(Can_Gaus_RMSIC_PhiBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_Gaus_RMSIC_PhiBinned.at(iPhi)).at(iEta)->GetName()) + ".png").c_str(), "png");
				(Can_Gaus_RMSIC_PhiBinned.at(iPhi)).at(iEta)->Close();

				delete htemp1 ;
				delete htemp2 ;
			}
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			/// Original IC distribution vs SM folding

			for(int iSMBin = 0 ; iSMBin < int(20 / NPhiSMBinDivision) ; iSMBin++) {
				((Can_SMBinned.at(iSMBin)).at(iPhi)).at(iEta)->cd();
				((Can_SMBinned.at(iSMBin)).at(iPhi)).at(iEta)->SetGridx();
				((Can_SMBinned.at(iSMBin)).at(iPhi)).at(iEta)->SetGridy();
				ICCrystalEB_SMBinned[iSMBin][iPhi][iEta]->SetLineColor(kBlack);
				ICCrystalEB_SMBinned[iSMBin][iPhi][iEta]->SetLineWidth(2);
				ICCrystalEB_SMBinned[iSMBin][iPhi][iEta]->SetMarkerStyle(20);
				ICCrystalEB_SMBinned[iSMBin][iPhi][iEta]->SetMarkerSize(1.);
				ICCrystalEB_SMBinned[iSMBin][iPhi][iEta]->SetMarkerColor(kRed);
				if(!isDeadTriggerTower) {
					GaussianFits_SMBinned[iSMBin][iPhi][iEta]->SetLineColor(kBlue);
					GaussianFits_SMBinned[iSMBin][iPhi][iEta]->SetLineWidth(2);
					ICCrystalEB_SMBinned[iSMBin][iPhi][iEta]->Fit(GaussianFits_SMBinned[iSMBin][iPhi][iEta], "RMEQ");
				}
				ICCrystalEB_SMBinned[iSMBin][iPhi][iEta]->Draw("E");
				ICCrystalEB_SMBinned[iSMBin][iPhi][iEta]->GetXaxis()->SetTitle("IC");
				ICCrystalEB_SMBinned[iSMBin][iPhi][iEta]->GetYaxis()->SetTitle("Entries");
				((Can_SMBinned.at(iSMBin)).at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string(((Can_SMBinned.at(iSMBin)).at(iPhi)).at(iEta)->GetName()) + ".pdf").c_str(), "pdf");
				((Can_SMBinned.at(iSMBin)).at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string(((Can_SMBinned.at(iSMBin)).at(iPhi)).at(iEta)->GetName()) + ".png").c_str(), "png");
			}

			/// Mean IC vs Eta

			(Can_MeanIC_SMBinned.at(iPhi)).at(iEta)->cd();
			(Can_MeanIC_SMBinned.at(iPhi)).at(iEta)->SetGridx();
			(Can_MeanIC_SMBinned.at(iPhi)).at(iEta)->SetGridy();

			MeanIC_SMBinned[iPhi][iEta]->SetLineColor(kBlack);
			MeanIC_SMBinned[iPhi][iEta]->SetLineWidth(2);
			MeanIC_SMBinned[iPhi][iEta]->SetMarkerStyle(20);
			MeanIC_SMBinned[iPhi][iEta]->SetMarkerSize(1.);

			for(int iBin = 0; iBin < MeanIC_SMBinned[iPhi][iEta]->GetNbinsX() ; iBin++) {
				MeanIC_SMBinned[iPhi][iEta]->SetBinContent(iBin + 1, ICCrystalEB_SMBinned[iBin][iPhi][iEta]->GetMean());
				MeanIC_SMBinned[iPhi][iEta]->SetBinError(iBin + 1, ICCrystalEB_SMBinned[iBin][iPhi][iEta]->GetMeanError());
			}


			Pol0_MeanIC_SMBinned[iPhi][iEta] -> SetLineColor(kBlue);
			Pol1_MeanIC_SMBinned[iPhi][iEta] -> SetLineColor(kGreen + 1);
			Pol2_MeanIC_SMBinned[iPhi][iEta] -> SetLineColor(kRed);
			Pol0_MeanIC_SMBinned[iPhi][iEta] -> SetLineWidth(2);
			Pol1_MeanIC_SMBinned[iPhi][iEta] -> SetLineWidth(2);
			Pol2_MeanIC_SMBinned[iPhi][iEta] -> SetLineWidth(2);

			MeanIC_SMBinned[iPhi][iEta]->Fit(Pol2_MeanIC_SMBinned[iPhi][iEta], "RMEQ0");
			htemp1 = (TH1F*) MeanIC_SMBinned[iPhi][iEta]->Clone("htemp1");
			htemp1->Fit(Pol1_MeanIC_SMBinned[iPhi][iEta], "RMEQ0");
			htemp2 = (TH1F*) MeanIC_SMBinned[iPhi][iEta]->Clone("htemp2");
			htemp2->Fit(Pol0_MeanIC_SMBinned[iPhi][iEta], "RMEQ0");

			MeanIC_SMBinned[iPhi][iEta]->Draw("E");
			MeanIC_SMBinned[iPhi][iEta]->GetXaxis()->SetTitle("i#eta ");
			MeanIC_SMBinned[iPhi][iEta]->GetYaxis()->SetTitle("IC Mean");
			MeanIC_SMBinned[iPhi][iEta]->GetYaxis()->SetRangeUser(MeanIC_SMBinned[iPhi][iEta]->GetMinimum() * 0.95, MeanIC_SMBinned[iPhi][iEta]->GetMaximum() * 1.15);

			(Can_MeanIC_SMBinned.at(iPhi)).at(iEta)->Update();
			pave1 = (TPaveStats*)(MeanIC_SMBinned[iPhi][iEta]->GetListOfFunctions()->FindObject("stats"));
			pave1->SetTextColor(kRed);
			pave1->SetBorderSize(2);
			pave1->SetLineColor(kRed);
			pave1->SetFillStyle(0);
			pave1->SetY1NDC(0.6);
			pave1->SetY2NDC(0.9);
			(Can_MeanIC_SMBinned.at(iPhi)).at(iEta)->Modified();

			htemp1->Draw("Esames");
			(Can_MeanIC_SMBinned.at(iPhi)).at(iEta)->Update();
			pave2 = (TPaveStats*)(htemp1->GetListOfFunctions()->FindObject("stats"));
			pave2->SetOptStat(000);
			pave2->SetBorderSize(2);
			pave2->SetLineColor(kGreen + 1);
			pave2->SetFillStyle(0);
			pave2->SetTextSize(pave2->GetTextSize() * 1.7);
			pave2->SetTextColor(kGreen + 1);
			pave2->SetX1NDC(0.35);
			pave2->SetX2NDC(0.6);
			pave2->SetY1NDC(0.6);
			pave2->SetY2NDC(0.9);
			(Can_MeanIC_SMBinned.at(iPhi)).at(iEta)->Modified();

			htemp2->Draw("Esames");
			(Can_MeanIC_SMBinned.at(iPhi)).at(iEta)->Update();
			pave3 = (TPaveStats*)(htemp2->GetListOfFunctions()->FindObject("stats"));
			pave3->SetOptStat(000);
			pave3->SetBorderSize(2);
			pave3->SetLineColor(kBlue);
			pave3->SetFillStyle(0);
			pave3->SetTextSize(pave3->GetTextSize() * 1.7);
			pave3->SetTextColor(kBlue);
			pave3->SetX1NDC(0.11);
			pave3->SetX2NDC(0.33);
			pave3->SetY1NDC(0.6);
			pave3->SetY2NDC(0.9);
			(Can_MeanIC_SMBinned.at(iPhi)).at(iEta)->Modified();

			Pol0_MeanIC_SMBinned[iPhi][iEta] -> Draw("Lsame");
			Pol1_MeanIC_SMBinned[iPhi][iEta] -> Draw("Lsame");
			Pol2_MeanIC_SMBinned[iPhi][iEta] -> Draw("Lsame");

			(Can_MeanIC_SMBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_MeanIC_SMBinned.at(iPhi)).at(iEta)->GetName()) + ".pdf").c_str(), "pdf");
			(Can_MeanIC_SMBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_MeanIC_SMBinned.at(iPhi)).at(iEta)->GetName()) + ".png").c_str(), "png");

			if(htemp1 != 0) delete htemp1 ;
			if(htemp2 != 0) delete htemp2 ;

			/// RMS IC vs SM
			(Can_RMSIC_SMBinned.at(iPhi)).at(iEta)->cd();
			(Can_RMSIC_SMBinned.at(iPhi)).at(iEta)->SetGridx();
			(Can_RMSIC_SMBinned.at(iPhi)).at(iEta)->SetGridy();

			RMSIC_SMBinned[iPhi][iEta]->SetLineColor(kBlack);
			RMSIC_SMBinned[iPhi][iEta]->SetLineWidth(2);
			RMSIC_SMBinned[iPhi][iEta]->SetMarkerStyle(20);
			RMSIC_SMBinned[iPhi][iEta]->SetMarkerSize(1.);

			for(int iBin = 0; iBin < RMSIC_SMBinned[iPhi][iEta]->GetNbinsX() ; iBin++) {
				RMSIC_SMBinned[iPhi][iEta]->SetBinContent(iBin + 1, ICCrystalEB_SMBinned[iBin][iPhi][iEta]->GetRMS());
				RMSIC_SMBinned[iPhi][iEta]->SetBinError(iBin + 1, ICCrystalEB_SMBinned[iBin][iPhi][iEta]->GetRMSError());
			}


			Pol0_RMSIC_SMBinned[iPhi][iEta] -> SetLineColor(kBlue);
			Pol1_RMSIC_SMBinned[iPhi][iEta] -> SetLineColor(kGreen + 1);
			Pol2_RMSIC_SMBinned[iPhi][iEta] -> SetLineColor(kRed);
			Pol0_RMSIC_SMBinned[iPhi][iEta] -> SetLineWidth(2);
			Pol1_RMSIC_SMBinned[iPhi][iEta] -> SetLineWidth(2);
			Pol2_RMSIC_SMBinned[iPhi][iEta] -> SetLineWidth(2);

			RMSIC_SMBinned[iPhi][iEta]->Fit(Pol2_RMSIC_SMBinned[iPhi][iEta], "RMEQ0");
			htemp1 = (TH1F*) RMSIC_SMBinned[iPhi][iEta]->Clone("htemp1");
			htemp1->Fit(Pol1_RMSIC_SMBinned[iPhi][iEta], "RMEQ0");
			htemp2 = (TH1F*) RMSIC_SMBinned[iPhi][iEta]->Clone("htemp2");
			htemp2->Fit(Pol0_RMSIC_SMBinned[iPhi][iEta], "RMEQ0");

			RMSIC_SMBinned[iPhi][iEta]->Draw("E");
			RMSIC_SMBinned[iPhi][iEta]->GetXaxis()->SetTitle("i#eta ");
			RMSIC_SMBinned[iPhi][iEta]->GetYaxis()->SetTitle("IC RMS");
			RMSIC_SMBinned[iPhi][iEta]->GetYaxis()->SetRangeUser(RMSIC_SMBinned[iPhi][iEta]->GetMinimum() * 0.85, RMSIC_SMBinned[iPhi][iEta]->GetMaximum() * 1.30);

			(Can_RMSIC_SMBinned.at(iPhi)).at(iEta)->Update();
			pave1 = (TPaveStats*)(RMSIC_SMBinned[iPhi][iEta]->GetListOfFunctions()->FindObject("stats"));
			pave1->SetTextColor(kRed);
			pave1->SetBorderSize(2);
			pave1->SetLineColor(kRed);
			pave1->SetFillStyle(0);
			pave1->SetY1NDC(0.6);
			pave1->SetY2NDC(0.9);
			(Can_RMSIC_SMBinned.at(iPhi)).at(iEta)->Modified();

			htemp1->Draw("Esames");
			(Can_RMSIC_SMBinned.at(iPhi)).at(iEta)->Update();
			pave2 = (TPaveStats*)(htemp1->GetListOfFunctions()->FindObject("stats"));
			pave2->SetOptStat(000);
			pave2->SetBorderSize(2);
			pave2->SetLineColor(kGreen + 1);
			pave2->SetFillStyle(0);
			pave2->SetTextSize(pave2->GetTextSize() * 1.7);
			pave2->SetTextColor(kGreen + 1);
			pave2->SetX1NDC(0.35);
			pave2->SetX2NDC(0.6);
			pave2->SetY1NDC(0.6);
			pave2->SetY2NDC(0.9);
			(Can_RMSIC_SMBinned.at(iPhi)).at(iEta)->Modified();

			htemp2->Draw("Esames");
			(Can_RMSIC_SMBinned.at(iPhi)).at(iEta)->Update();
			pave3 = (TPaveStats*)(htemp2->GetListOfFunctions()->FindObject("stats"));
			pave3->SetOptStat(000);
			pave3->SetBorderSize(2);
			pave3->SetLineColor(kBlue);
			pave3->SetFillStyle(0);
			pave3->SetTextSize(pave3->GetTextSize() * 1.7);
			pave3->SetTextColor(kBlue);
			pave3->SetX1NDC(0.11);
			pave3->SetX2NDC(0.33);
			pave3->SetY1NDC(0.6);
			pave3->SetY2NDC(0.9);
			(Can_RMSIC_SMBinned.at(iPhi)).at(iEta)->Modified();

			Pol0_RMSIC_SMBinned[iPhi][iEta] -> Draw("Lsame");
			Pol1_RMSIC_SMBinned[iPhi][iEta] -> Draw("Lsame");
			Pol2_RMSIC_SMBinned[iPhi][iEta] -> Draw("Lsame");

			(Can_RMSIC_SMBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_RMSIC_SMBinned.at(iPhi)).at(iEta)->GetName()) + ".pdf").c_str(), "pdf");
			(Can_RMSIC_SMBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_RMSIC_SMBinned.at(iPhi)).at(iEta)->GetName()) + ".png").c_str(), "png");

			if(htemp1 != 0) delete htemp1 ;
			if(htemp2 != 0) delete htemp2 ;

			/// Gauss Mean  vs SM
			if(isDeadTriggerTower) {
				(Can_Gaus_MeanIC_SMBinned.at(iPhi)).at(iEta)->cd();
				(Can_Gaus_MeanIC_SMBinned.at(iPhi)).at(iEta)->SetGridx();
				(Can_Gaus_MeanIC_SMBinned.at(iPhi)).at(iEta)->SetGridy();

				Gaus_MeanIC_SMBinned[iPhi][iEta]->SetLineColor(kBlack);
				Gaus_MeanIC_SMBinned[iPhi][iEta]->SetLineWidth(2);
				Gaus_MeanIC_SMBinned[iPhi][iEta]->SetMarkerStyle(20);
				Gaus_MeanIC_SMBinned[iPhi][iEta]->SetMarkerSize(1.);

				for(int iBin = 0; iBin < Gaus_MeanIC_SMBinned[iPhi][iEta]->GetNbinsX() ; iBin++) {
					Gaus_MeanIC_SMBinned[iPhi][iEta]->SetBinContent(iBin + 1, GaussianFits_SMBinned[iBin][iPhi][iEta]->GetParameter(1));
					Gaus_MeanIC_SMBinned[iPhi][iEta]->SetBinError(iBin + 1, GaussianFits_SMBinned[iBin][iPhi][iEta]->GetParError(1));
				}

				Pol0_Gaus_MeanIC_SMBinned[iPhi][iEta] -> SetLineColor(kBlue);
				Pol1_Gaus_MeanIC_SMBinned[iPhi][iEta] -> SetLineColor(kGreen + 1);
				Pol2_Gaus_MeanIC_SMBinned[iPhi][iEta] -> SetLineColor(kRed);
				Pol0_Gaus_MeanIC_SMBinned[iPhi][iEta] -> SetLineWidth(2);
				Pol1_Gaus_MeanIC_SMBinned[iPhi][iEta] -> SetLineWidth(2);
				Pol2_Gaus_MeanIC_SMBinned[iPhi][iEta] -> SetLineWidth(2);

				Gaus_MeanIC_SMBinned[iPhi][iEta]->Fit(Pol2_Gaus_MeanIC_SMBinned[iPhi][iEta], "RMEQ0");
				htemp1 = (TH1F*) Gaus_MeanIC_SMBinned[iPhi][iEta]->Clone("htemp1");
				htemp1->Fit(Pol1_Gaus_MeanIC_SMBinned[iPhi][iEta], "RMEQ0");
				htemp2 = (TH1F*) Gaus_MeanIC_SMBinned[iPhi][iEta]->Clone("htemp2");
				htemp2->Fit(Pol0_Gaus_MeanIC_SMBinned[iPhi][iEta], "RMEQ0");

				Gaus_MeanIC_SMBinned[iPhi][iEta]->Draw("E");
				Gaus_MeanIC_SMBinned[iPhi][iEta]->GetXaxis()->SetTitle("i#eta");
				Gaus_MeanIC_SMBinned[iPhi][iEta]->GetYaxis()->SetTitle("IC Gaussian Mean");
				Gaus_MeanIC_SMBinned[iPhi][iEta]->GetYaxis()->SetRangeUser(Gaus_MeanIC_SMBinned[iPhi][iEta]->GetMinimum() * 0.95, Gaus_MeanIC_SMBinned[iPhi][iEta]->GetMaximum() * 1.10);

				(Can_Gaus_MeanIC_SMBinned.at(iPhi)).at(iEta)->Update();
				pave1 = (TPaveStats*)(Gaus_MeanIC_SMBinned[iPhi][iEta]->GetListOfFunctions()->FindObject("stats"));
				pave1->SetTextColor(kRed);
				pave1->SetBorderSize(2);
				pave1->SetLineColor(kRed);
				pave1->SetFillStyle(0);
				pave1->SetY1NDC(0.6);
				pave1->SetY2NDC(0.9);
				(Can_Gaus_MeanIC_SMBinned.at(iPhi)).at(iEta)->Modified();

				htemp1->Draw("Esames");
				(Can_Gaus_MeanIC_SMBinned.at(iPhi)).at(iEta)->Update();
				pave2 = (TPaveStats*)(htemp1->GetListOfFunctions()->FindObject("stats"));
				pave2->SetOptStat(000);
				pave2->SetBorderSize(2);
				pave2->SetLineColor(kGreen + 1);
				pave2->SetFillStyle(0);
				pave2->SetTextSize(pave2->GetTextSize() * 1.7);
				pave2->SetTextColor(kGreen + 1);
				pave2->SetX1NDC(0.35);
				pave2->SetX2NDC(0.6);
				pave2->SetY1NDC(0.6);
				pave2->SetY2NDC(0.9);
				(Can_Gaus_MeanIC_SMBinned.at(iPhi)).at(iEta)->Modified();

				htemp2->Draw("Esames");
				(Can_Gaus_MeanIC_SMBinned.at(iPhi)).at(iEta)->Update();
				pave3 = (TPaveStats*)(htemp2->GetListOfFunctions()->FindObject("stats"));
				pave3->SetOptStat(000);
				pave3->SetBorderSize(2);
				pave3->SetLineColor(kBlue);
				pave3->SetFillStyle(0);
				pave3->SetTextSize(pave3->GetTextSize() * 1.7);
				pave3->SetTextColor(kBlue);
				pave3->SetX1NDC(0.11);
				pave3->SetX2NDC(0.33);
				pave3->SetY1NDC(0.6);
				pave3->SetY2NDC(0.9);
				(Can_Gaus_MeanIC_SMBinned.at(iPhi)).at(iEta)->Modified();

				Pol0_Gaus_MeanIC_SMBinned[iPhi][iEta] -> Draw("Lsame");
				Pol1_Gaus_MeanIC_SMBinned[iPhi][iEta] -> Draw("Lsame");
				Pol2_Gaus_MeanIC_SMBinned[iPhi][iEta] -> Draw("Lsame");

				(Can_Gaus_MeanIC_SMBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_Gaus_MeanIC_SMBinned.at(iPhi)).at(iEta)->GetName()) + ".pdf").c_str(), "pdf");
				(Can_Gaus_MeanIC_SMBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_Gaus_MeanIC_SMBinned.at(iPhi)).at(iEta)->GetName()) + ".png").c_str(), "png");
				(Can_Gaus_MeanIC_SMBinned.at(iPhi)).at(iEta)->Close();

				delete htemp1 ;
				delete htemp2 ;

				/// Gauss Sigma  vs SM

				(Can_Gaus_RMSIC_SMBinned.at(iPhi)).at(iEta)->cd();
				(Can_Gaus_RMSIC_SMBinned.at(iPhi)).at(iEta)->SetGridx();
				(Can_Gaus_RMSIC_SMBinned.at(iPhi)).at(iEta)->SetGridy();

				Gaus_RMSIC_SMBinned[iPhi][iEta]->SetLineColor(kBlack);
				Gaus_RMSIC_SMBinned[iPhi][iEta]->SetLineWidth(2);
				Gaus_RMSIC_SMBinned[iPhi][iEta]->SetMarkerStyle(20);
				Gaus_RMSIC_SMBinned[iPhi][iEta]->SetMarkerSize(1.);

				for(int iBin = 0; iBin < Gaus_RMSIC_SMBinned[iPhi][iEta]->GetNbinsX() ; iBin++) {
					Gaus_RMSIC_SMBinned[iPhi][iEta]->SetBinContent(iBin + 1, GaussianFits_SMBinned[iBin][iPhi][iEta]->GetParameter(2));
					Gaus_RMSIC_SMBinned[iPhi][iEta]->SetBinError(iBin + 1, GaussianFits_SMBinned[iBin][iPhi][iEta]->GetParError(2));
				}

				Pol0_Gaus_RMSIC_SMBinned[iPhi][iEta] -> SetLineColor(kBlue);
				Pol1_Gaus_RMSIC_SMBinned[iPhi][iEta] -> SetLineColor(kGreen + 1);
				Pol2_Gaus_RMSIC_SMBinned[iPhi][iEta] -> SetLineColor(kRed);
				Pol0_Gaus_RMSIC_SMBinned[iPhi][iEta] -> SetLineWidth(2);
				Pol1_Gaus_RMSIC_SMBinned[iPhi][iEta] -> SetLineWidth(2);
				Pol2_Gaus_RMSIC_SMBinned[iPhi][iEta] -> SetLineWidth(2);

				Gaus_RMSIC_SMBinned[iPhi][iEta]->Fit(Pol2_Gaus_RMSIC_SMBinned[iPhi][iEta], "RMEQ0");
				htemp1 = (TH1F*) Gaus_RMSIC_SMBinned[iPhi][iEta]->Clone("htemp1");
				htemp1->Fit(Pol1_Gaus_RMSIC_SMBinned[iPhi][iEta], "RMEQ0");
				htemp2 = (TH1F*) Gaus_RMSIC_SMBinned[iPhi][iEta]->Clone("htemp2");
				htemp2->Fit(Pol0_Gaus_RMSIC_SMBinned[iPhi][iEta], "RMEQ0");

				Gaus_RMSIC_SMBinned[iPhi][iEta]->Draw("E");
				Gaus_RMSIC_SMBinned[iPhi][iEta]->GetXaxis()->SetTitle("i#eta");
				Gaus_RMSIC_SMBinned[iPhi][iEta]->GetYaxis()->SetTitle("IC Gaussian RMS");
				Gaus_RMSIC_SMBinned[iPhi][iEta]->GetYaxis()->SetRangeUser(Gaus_RMSIC_SMBinned[iPhi][iEta]->GetMinimum() * 0.85, Gaus_RMSIC_SMBinned[iPhi][iEta]->GetMaximum() * 1.30);

				(Can_Gaus_RMSIC_SMBinned.at(iPhi)).at(iEta)->Update();
				pave1 = (TPaveStats*)(Gaus_RMSIC_SMBinned[iPhi][iEta]->GetListOfFunctions()->FindObject("stats"));
				pave1->SetTextColor(kRed);
				pave1->SetBorderSize(2);
				pave1->SetLineColor(kRed);
				pave1->SetFillStyle(0);
				pave1->SetY1NDC(0.6);
				pave1->SetY2NDC(0.9);
				(Can_Gaus_RMSIC_SMBinned.at(iPhi)).at(iEta)->Modified();

				htemp1->Draw("Esames");
				(Can_Gaus_RMSIC_SMBinned.at(iPhi)).at(iEta)->Update();
				pave2 = (TPaveStats*)(htemp1->GetListOfFunctions()->FindObject("stats"));
				pave2->SetOptStat(000);
				pave2->SetBorderSize(2);
				pave2->SetLineColor(kGreen + 1);
				pave2->SetFillStyle(0);
				pave2->SetTextSize(pave2->GetTextSize() * 1.7);
				pave2->SetTextColor(kGreen + 1);
				pave2->SetX1NDC(0.35);
				pave2->SetX2NDC(0.6);
				pave2->SetY1NDC(0.6);
				pave2->SetY2NDC(0.9);
				(Can_Gaus_RMSIC_SMBinned.at(iPhi)).at(iEta)->Modified();

				htemp2->Draw("Esames");
				(Can_Gaus_RMSIC_SMBinned.at(iPhi)).at(iEta)->Update();
				pave3 = (TPaveStats*)(htemp2->GetListOfFunctions()->FindObject("stats"));
				pave3->SetOptStat(000);
				pave3->SetBorderSize(2);
				pave3->SetLineColor(kBlue);
				pave3->SetFillStyle(0);
				pave3->SetTextSize(pave3->GetTextSize() * 1.7);
				pave3->SetTextColor(kBlue);
				pave3->SetX1NDC(0.11);
				pave3->SetX2NDC(0.33);
				pave3->SetY1NDC(0.6);
				pave3->SetY2NDC(0.9);
				(Can_Gaus_RMSIC_SMBinned.at(iPhi)).at(iEta)->Modified();

				Pol0_Gaus_RMSIC_SMBinned[iPhi][iEta] -> Draw("Lsame");
				Pol1_Gaus_RMSIC_SMBinned[iPhi][iEta] -> Draw("Lsame");
				Pol2_Gaus_RMSIC_SMBinned[iPhi][iEta] -> Draw("Lsame");

				(Can_Gaus_RMSIC_SMBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_Gaus_RMSIC_SMBinned.at(iPhi)).at(iEta)->GetName()) + ".pdf").c_str(), "pdf");
				(Can_Gaus_RMSIC_SMBinned.at(iPhi)).at(iEta)->Print((outputCanvasPlot + std::string((Can_Gaus_RMSIC_SMBinned.at(iPhi)).at(iEta)->GetName()) + ".png").c_str(), "png");
				(Can_Gaus_RMSIC_SMBinned.at(iPhi)).at(iEta)->Close();

				delete htemp1 ;
				delete htemp2 ;
			}
		}

	}

	// Final way to represent the results

	TH2F* inclusiveMeanIC = new TH2F("inclusiveMeanIC", "", IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow) ;
	TH2F* inclusiveRMSIC  = new TH2F("inclusiveRMSIC", "", IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow) ;

	TH2F* inclusiveGausMeanIC = new TH2F("inclusiveGausMeanIC", "", IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow) ;
	TH2F* inclusiveGausRMSIC  = new TH2F("inclusiveGausRMSIC", "", IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow) ;

	TH2F* inclusiveMeanIC_EBP = new TH2F("inclusiveMeanIC_EBP", "", IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow) ;
	TH2F* inclusiveRMSIC_EBP  = new TH2F("inclusiveRMSIC_EBP", "", IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow) ;

	TH2F* inclusiveGausMeanIC_EBP = new TH2F("inclusiveGausMeanIC_EBP", "", IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow) ;
	TH2F* inclusiveGausRMSIC_EBP  = new TH2F("inclusiveGausRMSIC_EBP", "", IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow) ;

	TH2F* inclusiveMeanIC_EBM = new TH2F("inclusiveMeanIC_EBM", "", IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow) ;
	TH2F* inclusiveRMSIC_EBM  = new TH2F("inclusiveRMSIC_EBM", "", IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow) ;

	TH2F* inclusiveGausMeanIC_EBM = new TH2F("inclusiveGausMeanIC_EBM", "", IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow) ;
	TH2F* inclusiveGausRMSIC_EBM  = new TH2F("inclusiveGausRMSIC_EBM", "", IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow) ;

	std::vector<TH2F*> EtaBinned_MeanIC (iEtaBinDivision.size() - 1);
	std::vector<TH2F*> PhiBinned_MeanIC (iPhiBinDivision.size() - 1);
	std::vector<TH2F*> SMBinned_MeanIC  (int(20 / NPhiSMBinDivision));

	std::vector<TH2F*> EtaBinned_GausMeanIC (iEtaBinDivision.size() - 1);
	std::vector<TH2F*> PhiBinned_GausMeanIC (iPhiBinDivision.size() - 1);
	std::vector<TH2F*> SMBinned_GausMeanIC  (int(20 / NPhiSMBinDivision));

	std::vector<TH2F*> EtaBinned_RMSIC (iEtaBinDivision.size() - 1);
	std::vector<TH2F*> PhiBinned_RMSIC (iPhiBinDivision.size() - 1);
	std::vector<TH2F*> SMBinned_RMSIC  (int(20 / NPhiSMBinDivision));

	std::vector<TH2F*> EtaBinned_GausRMSIC (iEtaBinDivision.size() - 1);
	std::vector<TH2F*> PhiBinned_GausRMSIC (iPhiBinDivision.size() - 1);
	std::vector<TH2F*> SMBinned_GausRMSIC  (int(20 / NPhiSMBinDivision));

	for(int iEtaBin =  0; iEtaBin < int(EtaBinned_MeanIC.size()) ; iEtaBin++) {
		EtaBinned_MeanIC[iEtaBin] = new TH2F(std::string(Form("EtaBinned_MeanIC_%d_%d", int(iEtaBinDivision.at(iEtaBin)), int(iEtaBinDivision.at(iEtaBin + 1)))).c_str(), "",
		                                     IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow);
		EtaBinned_RMSIC[iEtaBin]  = new TH2F(std::string(Form("EtaBinned_RMSIC_%d_%d", int(iEtaBinDivision.at(iEtaBin)), int(iEtaBinDivision.at(iEtaBin + 1)))).c_str(), "",
		                                     IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow);
		EtaBinned_GausMeanIC[iEtaBin] = new TH2F(std::string(Form("EtaBinned_GausMeanIC_%d_%d", int(iEtaBinDivision.at(iEtaBin)), int(iEtaBinDivision.at(iEtaBin + 1)))).c_str(), "",
		        IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow);
		EtaBinned_GausRMSIC[iEtaBin]  = new TH2F(std::string(Form("EtaBinned_GausRMSIC_%d_%d", int(iEtaBinDivision.at(iEtaBin)), int(iEtaBinDivision.at(iEtaBin + 1)))).c_str(), "",
		        IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow);
	}

	for(int iPhiBin =  0; iPhiBin < int(PhiBinned_MeanIC.size()) ; iPhiBin++) {
		PhiBinned_MeanIC[iPhiBin] = new TH2F(std::string(Form("PhiBinned_MeanIC_%d_%d", int(iPhiBinDivision.at(iPhiBin)), int(iPhiBinDivision.at(iPhiBin + 1)))).c_str(), "",
		                                     IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow);
		PhiBinned_RMSIC[iPhiBin]  = new TH2F(std::string(Form("PhiBinned_RMSIC_%d_%d", int(iPhiBinDivision.at(iPhiBin)), int(iPhiBinDivision.at(iPhiBin + 1)))).c_str(), "",
		                                     IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow);
		PhiBinned_GausMeanIC[iPhiBin] = new TH2F(std::string(Form("PhiBinned_GausMeanIC_%d_%d", int(iPhiBinDivision.at(iPhiBin)), int(iPhiBinDivision.at(iPhiBin + 1)))).c_str(), "",
		        IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow);
		PhiBinned_GausRMSIC[iPhiBin]  = new TH2F(std::string(Form("PhiBinned_GausRMSIC_%d_%d", int(iPhiBinDivision.at(iPhiBin)), int(iPhiBinDivision.at(iPhiBin + 1)))).c_str(), "",
		        IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow);
	}

	for(int iSMBin =  0; iSMBin < int(SMBinned_MeanIC.size()) ; iSMBin++) {
		SMBinned_MeanIC[iSMBin] = new TH2F(std::string(Form("SMBinned_MeanIC_%d_%d", iSMBin * NPhiSMBinDivision, (iSMBin + 1)*NPhiSMBinDivision)).c_str(), "",
		                                   IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow);
		SMBinned_RMSIC[iSMBin]  = new TH2F(std::string(Form("SMBinned_RMSIC_%d_%d", iSMBin * NPhiSMBinDivision, (iSMBin + 1)*NPhiSMBinDivision)).c_str(), "",
		                                   IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow);
		SMBinned_GausMeanIC[iSMBin] = new TH2F(std::string(Form("SMBinned_GausMeanIC_%d_%d", iSMBin * NPhiSMBinDivision, (iSMBin + 1)*NPhiSMBinDivision)).c_str(), "",
		                                       IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow);
		SMBinned_GausRMSIC[iSMBin]  = new TH2F(std::string(Form("SMBinned_GausRMSIC_%d_%d", iSMBin * NPhiSMBinDivision, (iSMBin + 1)*NPhiSMBinDivision)).c_str(), "",
		                                       IPhiWindow, 0, IPhiWindow, IEtaWindow, 0, IEtaWindow);
	}


	for( int iPhi = 0 ; iPhi < IPhiWindow ; iPhi ++) {
		for( int iEta = 0 ; iEta < IEtaWindow ; iEta ++) {

			if(iPhi == int(IPhiWindow / 2) && iEta == int(IEtaWindow / 2)) continue ;
			if(isDeadTriggerTower && ( int(fabs(iPhi - IPhiWindow / 2)) < int(IPhiWindow / 2 - 1) && int(fabs(iEta - IEtaWindow / 2)) < int(IPhiWindow / 2 - 1))) continue ;

			inclusiveMeanIC->SetBinContent(iPhi + 1, iEta + 1, ICCrystalEB[iPhi][iEta]->GetMean());
			inclusiveMeanIC->SetBinError(iPhi + 1, iEta + 1, ICCrystalEB[iPhi][iEta]->GetMeanError());

			if(!isDeadTriggerTower) {
				inclusiveGausMeanIC->SetBinContent(iPhi + 1, iEta + 1, GaussianFits[iPhi][iEta]->GetParameter(1));
				inclusiveGausMeanIC->SetBinError(iPhi + 1, iEta + 1, GaussianFits[iPhi][iEta]->GetParError(1));
			}

			inclusiveRMSIC->SetBinContent(iPhi + 1, iEta + 1, ICCrystalEB[iPhi][iEta]->GetRMS());
			inclusiveRMSIC->SetBinError(iPhi + 1, iEta + 1, ICCrystalEB[iPhi][iEta]->GetRMSError());

			if(!isDeadTriggerTower) {
				inclusiveGausRMSIC->SetBinContent(iPhi + 1, iEta + 1, GaussianFits[iPhi][iEta]->GetParameter(2));
				inclusiveGausRMSIC->SetBinError(iPhi + 1, iEta + 1, GaussianFits[iPhi][iEta]->GetParError(2));
			}

			inclusiveMeanIC_EBP->SetBinContent(iPhi + 1, iEta + 1, ICCrystalEBP[iPhi][iEta]->GetMean());
			inclusiveMeanIC_EBP->SetBinError(iPhi + 1, iEta + 1, ICCrystalEBP[iPhi][iEta]->GetMeanError());

			if(!isDeadTriggerTower) {
				inclusiveGausMeanIC_EBP->SetBinContent(iPhi + 1, iEta + 1, GaussianFits_EBP[iPhi][iEta]->GetParameter(1));
				inclusiveGausMeanIC_EBP->SetBinError(iPhi + 1, iEta + 1, GaussianFits_EBP[iPhi][iEta]->GetParError(1));
			}

			inclusiveRMSIC_EBP->SetBinContent(iPhi + 1, iEta + 1, ICCrystalEBP[iPhi][iEta]->GetRMS());
			inclusiveRMSIC_EBP->SetBinError(iPhi + 1, iEta + 1, ICCrystalEBP[iPhi][iEta]->GetRMSError());

			if(!isDeadTriggerTower) {
				inclusiveGausRMSIC_EBP->SetBinContent(iPhi + 1, iEta + 1, GaussianFits_EBP[iPhi][iEta]->GetParameter(2));
				inclusiveGausRMSIC_EBP->SetBinError(iPhi + 1, iEta + 1, GaussianFits_EBP[iPhi][iEta]->GetParError(2));
			}

			inclusiveMeanIC_EBM->SetBinContent(iPhi + 1, iEta + 1, ICCrystalEBM[iPhi][iEta]->GetMean());
			inclusiveMeanIC_EBM->SetBinError(iPhi + 1, iEta + 1, ICCrystalEBM[iPhi][iEta]->GetMeanError());

			if(!isDeadTriggerTower) {
				inclusiveGausMeanIC_EBM->SetBinContent(iPhi + 1, iEta + 1, GaussianFits_EBM[iPhi][iEta]->GetParameter(1));
				inclusiveGausMeanIC_EBM->SetBinError(iPhi + 1, iEta + 1, GaussianFits_EBM[iPhi][iEta]->GetParError(1));
			}

			inclusiveRMSIC_EBM->SetBinContent(iPhi + 1, iEta + 1, ICCrystalEBM[iPhi][iEta]->GetRMS());
			inclusiveRMSIC_EBM->SetBinError(iPhi + 1, iEta + 1, ICCrystalEBM[iPhi][iEta]->GetRMSError());

			if(!isDeadTriggerTower) {
				inclusiveGausRMSIC_EBM->SetBinContent(iPhi + 1, iEta + 1, GaussianFits_EBM[iPhi][iEta]->GetParameter(2));
				inclusiveGausRMSIC_EBM->SetBinError(iPhi + 1, iEta + 1, GaussianFits_EBM[iPhi][iEta]->GetParError(2));
			}

			for(int iEtaBin = 0 ; iEtaBin < int(iEtaBinDivision.size() - 1) ; iEtaBin++) {

				EtaBinned_MeanIC[iEtaBin]->SetBinContent(iPhi + 1, iEta + 1, ICCrystalEB_EtaBinned[iEtaBin][iPhi][iEta]->GetMean());
				EtaBinned_MeanIC[iEtaBin]->SetBinError(iPhi + 1, iEta + 1, ICCrystalEB_EtaBinned[iEtaBin][iPhi][iEta]->GetMeanError());

				EtaBinned_RMSIC[iEtaBin]->SetBinContent(iPhi + 1, iEta + 1, ICCrystalEB_EtaBinned[iEtaBin][iPhi][iEta]->GetRMS());
				EtaBinned_RMSIC[iEtaBin]->SetBinError(iPhi + 1, iEta + 1, ICCrystalEB_EtaBinned[iEtaBin][iPhi][iEta]->GetRMSError());

				if(!isDeadTriggerTower) {
					EtaBinned_GausMeanIC[iEtaBin]->SetBinContent(iPhi + 1, iEta + 1, GaussianFits_EtaBinned[iEtaBin][iPhi][iEta]->GetParameter(1));
					EtaBinned_GausMeanIC[iEtaBin]->SetBinError(iPhi + 1, iEta + 1, GaussianFits_EtaBinned[iEtaBin][iPhi][iEta]->GetParError(1));

					EtaBinned_GausRMSIC[iEtaBin]->SetBinContent(iPhi + 1, iEta + 1, GaussianFits_EtaBinned[iEtaBin][iPhi][iEta]->GetParameter(2));
					EtaBinned_GausRMSIC[iEtaBin]->SetBinError(iPhi + 1, iEta + 1, GaussianFits_EtaBinned[iEtaBin][iPhi][iEta]->GetParError(2));
				}
			}

			for(int iPhiBin = 0 ; iPhiBin < int(iPhiBinDivision.size() - 1) ; iPhiBin++) {

				PhiBinned_MeanIC[iPhiBin]->SetBinContent(iPhi + 1, iEta + 1, ICCrystalEB_PhiBinned[iPhiBin][iPhi][iEta]->GetMean());
				PhiBinned_MeanIC[iPhiBin]->SetBinError(iPhi + 1, iEta + 1, ICCrystalEB_PhiBinned[iPhiBin][iPhi][iEta]->GetMeanError());

				PhiBinned_RMSIC[iPhiBin]->SetBinContent(iPhi + 1, iEta + 1, ICCrystalEB_PhiBinned[iPhiBin][iPhi][iEta]->GetRMS());
				PhiBinned_RMSIC[iPhiBin]->SetBinError(iPhi + 1, iEta + 1, ICCrystalEB_PhiBinned[iPhiBin][iPhi][iEta]->GetRMSError());

				if(!isDeadTriggerTower) {
					PhiBinned_GausMeanIC[iPhiBin]->SetBinContent(iPhi + 1, iEta + 1, GaussianFits_PhiBinned[iPhiBin][iPhi][iEta]->GetParameter(1));
					PhiBinned_GausMeanIC[iPhiBin]->SetBinError(iPhi + 1, iEta + 1, GaussianFits_PhiBinned[iPhiBin][iPhi][iEta]->GetParError(1));

					PhiBinned_GausRMSIC[iPhiBin]->SetBinContent(iPhi + 1, iEta + 1, GaussianFits_PhiBinned[iPhiBin][iPhi][iEta]->GetParameter(2));
					PhiBinned_GausRMSIC[iPhiBin]->SetBinError(iPhi + 1, iEta + 1, GaussianFits_PhiBinned[iPhiBin][iPhi][iEta]->GetParError(2));
				}
			}

			for(int iSMBin = 0 ; iSMBin < int(20 / NPhiSMBinDivision) ; iSMBin++) {

				SMBinned_MeanIC[iSMBin]->SetBinContent(iPhi + 1, iEta + 1, ICCrystalEB_SMBinned[iSMBin][iPhi][iEta]->GetMean());
				SMBinned_MeanIC[iSMBin]->SetBinError(iPhi + 1, iEta + 1, ICCrystalEB_SMBinned[iSMBin][iPhi][iEta]->GetMeanError());

				SMBinned_RMSIC[iSMBin]->SetBinContent(iPhi + 1, iEta + 1, ICCrystalEB_SMBinned[iSMBin][iPhi][iEta]->GetRMS());
				SMBinned_RMSIC[iSMBin]->SetBinError(iPhi + 1, iEta + 1, ICCrystalEB_SMBinned[iSMBin][iPhi][iEta]->GetRMSError());

				if(!isDeadTriggerTower) {
					SMBinned_GausMeanIC[iSMBin]->SetBinContent(iPhi + 1, iEta + 1, GaussianFits_SMBinned[iSMBin][iPhi][iEta]->GetParameter(1));
					SMBinned_GausMeanIC[iSMBin]->SetBinError(iPhi + 1, iEta + 1, GaussianFits_SMBinned[iSMBin][iPhi][iEta]->GetParError(1));

					SMBinned_GausRMSIC[iSMBin]->SetBinContent(iPhi + 1, iEta + 1, GaussianFits_SMBinned[iSMBin][iPhi][iEta]->GetParameter(2));
					SMBinned_GausRMSIC[iSMBin]->SetBinError(iPhi + 1, iEta + 1, GaussianFits_SMBinned[iSMBin][iPhi][iEta]->GetParError(2));
				}
			}
		}
	}

	TCanvas* Can_inclusiveMeanIC = new TCanvas("Can_inclusiveMeanIC", "", 600, 500);
	TCanvas* Can_inclusiveRMSIC  = new TCanvas("Can_inclusiveRMSIC", "", 600, 500);

	TCanvas* Can_inclusiveGausMeanIC = new TCanvas("Can_inclusiveGausMeanIC", "", 600, 500);
	TCanvas* Can_inclusiveGausRMSIC  = new TCanvas("Can_inclusiveGausRMSIC", "", 600, 500);

	TCanvas* Can_inclusiveMeanIC_EBP = new TCanvas("Can_inclusiveMeanIC_EBP", "", 600, 500);
	TCanvas* Can_inclusiveRMSIC_EBP  = new TCanvas("Can_inclusiveRMSIC_EBP", "", 600, 500);

	TCanvas* Can_inclusiveGausMeanIC_EBP = new TCanvas("Can_inclusiveGausMeanIC_EBP", "", 600, 500);
	TCanvas* Can_inclusiveGausRMSIC_EBP  = new TCanvas("Can_inclusiveGausRMSIC_EBP", "", 600, 500);

	TCanvas* Can_inclusiveMeanIC_EBM = new TCanvas("Can_inclusiveMeanIC_EBM", "", 600, 500);
	TCanvas* Can_inclusiveRMSIC_EBM  = new TCanvas("Can_inclusiveRMSIC_EBM", "", 600, 500);

	TCanvas* Can_inclusiveGausMeanIC_EBM = new TCanvas("Can_inclusiveGausMeanIC_EBM", "", 600, 500);
	TCanvas* Can_inclusiveGausRMSIC_EBM  = new TCanvas("Can_inclusiveGausRMSIC_EBM", "", 600, 500);

	std::vector<TCanvas*> Can_EtaBinned_MeanIC (iEtaBinDivision.size() - 1);
	std::vector<TCanvas*> Can_PhiBinned_MeanIC (iPhiBinDivision.size() - 1);
	std::vector<TCanvas*> Can_SMBinned_MeanIC  (int(20 / NPhiSMBinDivision));

	std::vector<TCanvas*> Can_EtaBinned_GausMeanIC (iEtaBinDivision.size() - 1);
	std::vector<TCanvas*> Can_PhiBinned_GausMeanIC (iPhiBinDivision.size() - 1);
	std::vector<TCanvas*> Can_SMBinned_GausMeanIC  (int(20 / NPhiSMBinDivision));

	std::vector<TCanvas*> Can_EtaBinned_RMSIC (iEtaBinDivision.size() - 1);
	std::vector<TCanvas*> Can_PhiBinned_RMSIC (iPhiBinDivision.size() - 1);
	std::vector<TCanvas*> Can_SMBinned_RMSIC  (int(20 / NPhiSMBinDivision));

	std::vector<TCanvas*> Can_EtaBinned_GausRMSIC (iEtaBinDivision.size() - 1);
	std::vector<TCanvas*> Can_PhiBinned_GausRMSIC (iPhiBinDivision.size() - 1);
	std::vector<TCanvas*> Can_SMBinned_GausRMSIC  (int(20 / NPhiSMBinDivision));

	gStyle->SetOptStat(00000);


	Can_inclusiveMeanIC->cd();
	Can_inclusiveMeanIC->SetGridx();
	Can_inclusiveMeanIC->SetGridy();
	inclusiveMeanIC->GetXaxis()->SetTitle("i#phi");
	inclusiveMeanIC->GetYaxis()->SetTitle("i#eta");
	inclusiveMeanIC->Draw("colz");
	if(!isDeadTriggerTower) inclusiveMeanIC->SetMinimum(inclusiveMeanIC->GetMinimum() + 0.9);
	else  inclusiveMeanIC->SetMinimum(inclusiveMeanIC->GetMinimum() + 0.75);
	inclusiveMeanIC->Draw("textSAME");
	Can_inclusiveMeanIC->Print((outputCanvasPlot + std::string(Can_inclusiveMeanIC->GetName()) + ".pdf").c_str(), "pdf");
	Can_inclusiveMeanIC->Print((outputCanvasPlot + std::string(Can_inclusiveMeanIC->GetName()) + ".png").c_str(), "png");

	Can_inclusiveRMSIC->cd();
	Can_inclusiveRMSIC->SetGridx();
	Can_inclusiveRMSIC->SetGridy();
	inclusiveRMSIC->GetXaxis()->SetTitle("i#phi");
	inclusiveRMSIC->GetYaxis()->SetTitle("i#eta");
	inclusiveRMSIC->Draw("colz");
	inclusiveRMSIC->Draw("textSAME");
	Can_inclusiveRMSIC->Print((outputCanvasPlot + std::string(Can_inclusiveRMSIC->GetName()) + ".pdf").c_str(), "pdf");
	Can_inclusiveRMSIC->Print((outputCanvasPlot + std::string(Can_inclusiveRMSIC->GetName()) + ".png").c_str(), "png");

	if(!isDeadTriggerTower) {
		Can_inclusiveGausMeanIC->cd();
		Can_inclusiveGausMeanIC->SetGridx();
		Can_inclusiveGausMeanIC->SetGridy();
		inclusiveGausMeanIC->SetMinimum(inclusiveGausMeanIC->GetMinimum() + 0.9);
		inclusiveGausMeanIC->GetXaxis()->SetTitle("i#phi");
		inclusiveGausMeanIC->GetYaxis()->SetTitle("i#eta");
		inclusiveGausMeanIC->Draw("colz");
		inclusiveGausMeanIC->Draw("textSAME");
		Can_inclusiveGausMeanIC->Print((outputCanvasPlot + std::string(Can_inclusiveGausMeanIC->GetName()) + ".pdf").c_str(), "pdf");
		Can_inclusiveGausMeanIC->Print((outputCanvasPlot + std::string(Can_inclusiveGausMeanIC->GetName()) + ".png").c_str(), "png");

		Can_inclusiveGausRMSIC->cd();
		Can_inclusiveGausRMSIC->SetGridx();
		Can_inclusiveGausRMSIC->SetGridy();
		inclusiveGausRMSIC->GetXaxis()->SetTitle("i#phi");
		inclusiveGausRMSIC->GetYaxis()->SetTitle("i#eta");
		inclusiveGausRMSIC->Draw("colz");
		inclusiveGausRMSIC->Draw("textSAME");
		Can_inclusiveGausRMSIC->Print((outputCanvasPlot + std::string(Can_inclusiveGausRMSIC->GetName()) + ".pdf").c_str(), "pdf");
		Can_inclusiveGausRMSIC->Print((outputCanvasPlot + std::string(Can_inclusiveGausRMSIC->GetName()) + ".png").c_str(), "png");
	}

	Can_inclusiveMeanIC_EBP->cd();
	Can_inclusiveMeanIC_EBP->SetGridx();
	Can_inclusiveMeanIC_EBP->SetGridy();
	inclusiveMeanIC_EBP->GetXaxis()->SetTitle("i#phi");
	inclusiveMeanIC_EBP->GetYaxis()->SetTitle("i#eta");
	if(!isDeadTriggerTower) inclusiveMeanIC_EBP->SetMinimum(inclusiveMeanIC_EBP->GetMinimum() + 0.9);
	else inclusiveMeanIC_EBP->SetMinimum(inclusiveMeanIC_EBP->GetMinimum() + 0.75);
	inclusiveMeanIC_EBP->Draw("colz");
	inclusiveMeanIC_EBP->Draw("textSAME");
	Can_inclusiveMeanIC_EBP->Print((outputCanvasPlot + std::string(Can_inclusiveMeanIC_EBP->GetName()) + ".pdf").c_str(), "pdf");
	Can_inclusiveMeanIC_EBP->Print((outputCanvasPlot + std::string(Can_inclusiveMeanIC_EBP->GetName()) + ".png").c_str(), "png");

	Can_inclusiveRMSIC_EBP->cd();
	Can_inclusiveRMSIC_EBP->SetGridx();
	Can_inclusiveRMSIC_EBP->SetGridy();
	inclusiveRMSIC_EBP->GetXaxis()->SetTitle("i#phi");
	inclusiveRMSIC_EBP->GetYaxis()->SetTitle("i#eta");
	inclusiveRMSIC_EBP->Draw("colz");
	inclusiveRMSIC_EBP->Draw("textSAME");
	Can_inclusiveRMSIC_EBP->Print((outputCanvasPlot + std::string(Can_inclusiveRMSIC_EBP->GetName()) + ".pdf").c_str(), "pdf");
	Can_inclusiveRMSIC_EBP->Print((outputCanvasPlot + std::string(Can_inclusiveRMSIC_EBP->GetName()) + ".png").c_str(), "png");

	if(!isDeadTriggerTower) {
		Can_inclusiveGausMeanIC_EBP->cd();
		Can_inclusiveGausMeanIC_EBP->SetGridx();
		Can_inclusiveGausMeanIC_EBP->SetGridy();
		inclusiveGausMeanIC_EBP->GetXaxis()->SetTitle("i#phi");
		inclusiveGausMeanIC_EBP->GetYaxis()->SetTitle("i#eta");
		inclusiveGausMeanIC_EBP->SetMinimum(inclusiveGausMeanIC_EBP->GetMinimum() + 0.9);
		inclusiveGausMeanIC_EBP->Draw("colz");
		inclusiveGausMeanIC_EBP->Draw("textSAME");
		Can_inclusiveGausMeanIC_EBP->Print((outputCanvasPlot + std::string(Can_inclusiveGausMeanIC_EBP->GetName()) + ".pdf").c_str(), "pdf");
		Can_inclusiveGausMeanIC_EBP->Print((outputCanvasPlot + std::string(Can_inclusiveGausMeanIC_EBP->GetName()) + ".png").c_str(), "png");

		Can_inclusiveGausRMSIC_EBP->cd();
		Can_inclusiveGausRMSIC_EBP->SetGridx();
		Can_inclusiveGausRMSIC_EBP->SetGridy();
		inclusiveGausRMSIC_EBP->GetXaxis()->SetTitle("i#phi");
		inclusiveGausRMSIC_EBP->GetYaxis()->SetTitle("i#eta");
		inclusiveGausRMSIC_EBP->Draw("colz");
		inclusiveGausRMSIC_EBP->Draw("textSAME");
		Can_inclusiveGausRMSIC_EBP->Print((outputCanvasPlot + std::string(Can_inclusiveGausRMSIC_EBP->GetName()) + ".pdf").c_str(), "pdf");
		Can_inclusiveGausRMSIC_EBP->Print((outputCanvasPlot + std::string(Can_inclusiveGausRMSIC_EBP->GetName()) + ".png").c_str(), "png");
	}

	Can_inclusiveMeanIC_EBM->cd();
	Can_inclusiveMeanIC_EBM->SetGridx();
	Can_inclusiveMeanIC_EBM->SetGridy();
	inclusiveMeanIC_EBM->GetXaxis()->SetTitle("i#phi");
	inclusiveMeanIC_EBM->GetYaxis()->SetTitle("i#eta");
	if(!isDeadTriggerTower) inclusiveMeanIC_EBM->SetMinimum(inclusiveMeanIC_EBM->GetMinimum() + 0.9);
	else inclusiveMeanIC_EBM->SetMinimum(inclusiveMeanIC_EBM->GetMinimum() + 0.75);
	inclusiveMeanIC_EBM->Draw("colz");
	inclusiveMeanIC_EBM->Draw("textSAME");
	Can_inclusiveMeanIC_EBM->Print((outputCanvasPlot + std::string(Can_inclusiveMeanIC_EBM->GetName()) + ".pdf").c_str(), "pdf");
	Can_inclusiveMeanIC_EBM->Print((outputCanvasPlot + std::string(Can_inclusiveMeanIC_EBM->GetName()) + ".png").c_str(), "png");

	Can_inclusiveRMSIC_EBM->cd();
	Can_inclusiveRMSIC_EBM->SetGridx();
	Can_inclusiveRMSIC_EBM->SetGridy();
	inclusiveRMSIC_EBM->GetXaxis()->SetTitle("i#phi");
	inclusiveRMSIC_EBM->GetYaxis()->SetTitle("i#eta");
	inclusiveRMSIC_EBM->Draw("colz");
	inclusiveRMSIC_EBM->Draw("textSAME");
	Can_inclusiveRMSIC_EBM->Print((outputCanvasPlot + std::string(Can_inclusiveRMSIC_EBM->GetName()) + ".pdf").c_str(), "pdf");
	Can_inclusiveRMSIC_EBM->Print((outputCanvasPlot + std::string(Can_inclusiveRMSIC_EBM->GetName()) + ".png").c_str(), "png");

	if(!isDeadTriggerTower) {
		Can_inclusiveGausMeanIC_EBM->cd();
		Can_inclusiveGausMeanIC_EBM->SetGridx();
		Can_inclusiveGausMeanIC_EBM->SetGridy();
		inclusiveGausMeanIC_EBM->GetXaxis()->SetTitle("i#phi");
		inclusiveGausMeanIC_EBM->GetYaxis()->SetTitle("i#eta");
		inclusiveGausMeanIC_EBM->SetMinimum(inclusiveGausMeanIC_EBM->GetMinimum() + 0.9);
		inclusiveGausMeanIC_EBM->Draw("colz");
		inclusiveGausMeanIC_EBM->Draw("textSAME");
		Can_inclusiveGausMeanIC_EBM->Print((outputCanvasPlot + std::string(Can_inclusiveGausMeanIC_EBM->GetName()) + ".pdf").c_str(), "pdf");
		Can_inclusiveGausMeanIC_EBM->Print((outputCanvasPlot + std::string(Can_inclusiveGausMeanIC_EBM->GetName()) + ".png").c_str(), "png");

		Can_inclusiveGausRMSIC_EBM->cd();
		Can_inclusiveGausRMSIC_EBM->SetGridx();
		Can_inclusiveGausRMSIC_EBM->SetGridy();
		inclusiveGausRMSIC_EBM->GetXaxis()->SetTitle("i#phi");
		inclusiveGausRMSIC_EBM->GetYaxis()->SetTitle("i#eta");
		inclusiveGausRMSIC_EBM->Draw("colz");
		inclusiveGausRMSIC_EBM->Draw("textSAME");
		Can_inclusiveGausRMSIC_EBM->Print((outputCanvasPlot + std::string(Can_inclusiveGausRMSIC_EBM->GetName()) + ".pdf").c_str(), "pdf");
		Can_inclusiveGausRMSIC_EBM->Print((outputCanvasPlot + std::string(Can_inclusiveGausRMSIC_EBM->GetName()) + ".png").c_str(), "png");
	}

	for(int iEtaBin =  0; iEtaBin < int(EtaBinned_MeanIC.size()) ; iEtaBin++) {

		Can_EtaBinned_MeanIC[iEtaBin] = new TCanvas(std::string(Form("Can_EtaBinned_MeanIC_%d_%d", int(iEtaBinDivision.at(iEtaBin)), int(iEtaBinDivision.at(iEtaBin + 1)))).c_str(), "", 600, 500);
		Can_EtaBinned_RMSIC[iEtaBin] = new TCanvas(std::string(Form("Can_EtaBinned_RMSIC_%d_%d", int(iEtaBinDivision.at(iEtaBin)), int(iEtaBinDivision.at(iEtaBin + 1)))).c_str(), "", 600, 500);
		Can_EtaBinned_GausMeanIC[iEtaBin] = new TCanvas(std::string(Form("Can_EtaBinned_GausMeanIC_%d_%d", int(iEtaBinDivision.at(iEtaBin)),
		        int(iEtaBinDivision.at(iEtaBin + 1)))).c_str(), "", 600, 500);
		Can_EtaBinned_GausRMSIC[iEtaBin] = new TCanvas(std::string(Form("Can_EtaBinned_GausRMSIC_%d_%d", int(iEtaBinDivision.at(iEtaBin)),
		        int(iEtaBinDivision.at(iEtaBin + 1)))).c_str(), "", 600, 500);

		Can_EtaBinned_MeanIC[iEtaBin]->cd();
		Can_EtaBinned_MeanIC[iEtaBin]->SetGridx();
		Can_EtaBinned_MeanIC[iEtaBin]->SetGridy();
		EtaBinned_MeanIC[iEtaBin]->GetXaxis()->SetTitle("i#phi");
		EtaBinned_MeanIC[iEtaBin]->GetYaxis()->SetTitle("i#eta");
		if(!isDeadTriggerTower) EtaBinned_MeanIC[iEtaBin]->SetMinimum(EtaBinned_MeanIC[iEtaBin]->GetMinimum() + 0.9);
		else EtaBinned_MeanIC[iEtaBin]->SetMinimum(EtaBinned_MeanIC[iEtaBin]->GetMinimum() + 0.75);
		EtaBinned_MeanIC[iEtaBin]->Draw("colz");
		EtaBinned_MeanIC[iEtaBin]->Draw("textSAME");
		Can_EtaBinned_MeanIC[iEtaBin]->Print((outputCanvasPlot + std::string(Can_EtaBinned_MeanIC[iEtaBin]->GetName()) + ".pdf").c_str(), "pdf");
		Can_EtaBinned_MeanIC[iEtaBin]->Print((outputCanvasPlot + std::string(Can_EtaBinned_MeanIC[iEtaBin]->GetName()) + ".png").c_str(), "png");

		Can_EtaBinned_RMSIC[iEtaBin]->cd();
		Can_EtaBinned_RMSIC[iEtaBin]->SetGridx();
		Can_EtaBinned_RMSIC[iEtaBin]->SetGridy();
		EtaBinned_RMSIC[iEtaBin]->GetXaxis()->SetTitle("i#phi");
		EtaBinned_RMSIC[iEtaBin]->GetYaxis()->SetTitle("i#eta");
		EtaBinned_RMSIC[iEtaBin]->Draw("colz");
		EtaBinned_RMSIC[iEtaBin]->Draw("textSAME");
		Can_EtaBinned_RMSIC[iEtaBin]->Print((outputCanvasPlot + std::string(Can_EtaBinned_RMSIC[iEtaBin]->GetName()) + ".pdf").c_str(), "pdf");
		Can_EtaBinned_RMSIC[iEtaBin]->Print((outputCanvasPlot + std::string(Can_EtaBinned_RMSIC[iEtaBin]->GetName()) + ".png").c_str(), "png");

		if(!isDeadTriggerTower) {
			Can_EtaBinned_GausMeanIC[iEtaBin]->cd();
			Can_EtaBinned_GausMeanIC[iEtaBin]->SetGridx();
			Can_EtaBinned_GausMeanIC[iEtaBin]->SetGridy();
			EtaBinned_GausMeanIC[iEtaBin]->GetXaxis()->SetTitle("i#phi");
			EtaBinned_GausMeanIC[iEtaBin]->GetYaxis()->SetTitle("i#eta");
			EtaBinned_GausMeanIC[iEtaBin]->SetMinimum(EtaBinned_GausMeanIC[iEtaBin]->GetMinimum() + 0.9);
			EtaBinned_GausMeanIC[iEtaBin]->Draw("colz");
			EtaBinned_GausMeanIC[iEtaBin]->Draw("textSAME");
			Can_EtaBinned_GausMeanIC[iEtaBin]->Print((outputCanvasPlot + std::string(Can_EtaBinned_GausMeanIC[iEtaBin]->GetName()) + ".pdf").c_str(), "pdf");
			Can_EtaBinned_GausMeanIC[iEtaBin]->Print((outputCanvasPlot + std::string(Can_EtaBinned_GausMeanIC[iEtaBin]->GetName()) + ".png").c_str(), "png");
			Can_EtaBinned_GausRMSIC[iEtaBin]->cd();
			Can_EtaBinned_GausRMSIC[iEtaBin]->SetGridx();
			Can_EtaBinned_GausRMSIC[iEtaBin]->SetGridy();
			EtaBinned_GausRMSIC[iEtaBin]->GetXaxis()->SetTitle("i#phi");
			EtaBinned_GausRMSIC[iEtaBin]->GetYaxis()->SetTitle("i#eta");
			EtaBinned_GausRMSIC[iEtaBin]->Draw("colz");
			EtaBinned_GausRMSIC[iEtaBin]->Draw("textSAME");
			Can_EtaBinned_GausRMSIC[iEtaBin]->Print((outputCanvasPlot + std::string(Can_EtaBinned_RMSIC[iEtaBin]->GetName()) + ".pdf").c_str(), "pdf");
			Can_EtaBinned_GausRMSIC[iEtaBin]->Print((outputCanvasPlot + std::string(Can_EtaBinned_RMSIC[iEtaBin]->GetName()) + ".png").c_str(), "png");
		}
	}

	for(int iPhiBin =  0; iPhiBin < int(PhiBinned_MeanIC.size()) ; iPhiBin++) {

		Can_PhiBinned_MeanIC[iPhiBin] = new TCanvas(std::string(Form("Can_PhiBinned_MeanIC_%d_%d", int(iPhiBinDivision.at(iPhiBin)), int(iPhiBinDivision.at(iPhiBin + 1)))).c_str(), "", 600, 500);
		Can_PhiBinned_RMSIC[iPhiBin]  = new TCanvas(std::string(Form("Can_PhiBinned_RMSIC_%d_%d", int(iPhiBinDivision.at(iPhiBin)), int(iPhiBinDivision.at(iPhiBin + 1)))).c_str(), "", 600, 500);
		Can_PhiBinned_GausMeanIC[iPhiBin] = new TCanvas(std::string(Form("Can_PhiBinned_GausMeanIC_%d_%d", int(iPhiBinDivision.at(iPhiBin)),
		        int(iPhiBinDivision.at(iPhiBin + 1)))).c_str(), "", 600, 500);
		Can_PhiBinned_GausRMSIC[iPhiBin] = new TCanvas(std::string(Form("Can_PhiBinned_GausRMSIC_%d_%d", int(iPhiBinDivision.at(iPhiBin)),
		        int(iPhiBinDivision.at(iPhiBin + 1)))).c_str(), "", 600, 500);

		Can_PhiBinned_MeanIC[iPhiBin]->cd();
		Can_PhiBinned_MeanIC[iPhiBin]->SetGridx();
		Can_PhiBinned_MeanIC[iPhiBin]->SetGridy();
		PhiBinned_MeanIC[iPhiBin]->GetXaxis()->SetTitle("i#phi");
		PhiBinned_MeanIC[iPhiBin]->GetYaxis()->SetTitle("i#eta");
		if(!isDeadTriggerTower) PhiBinned_MeanIC[iPhiBin]->SetMinimum(inclusiveMeanIC_EBM->GetMinimum() + 0.9);
		else PhiBinned_MeanIC[iPhiBin]->SetMinimum(PhiBinned_MeanIC[iPhiBin]->GetMinimum() + 0.75);
		PhiBinned_MeanIC[iPhiBin]->Draw("colz");
		PhiBinned_MeanIC[iPhiBin]->Draw("textSAME");
		Can_PhiBinned_MeanIC[iPhiBin]->Print((outputCanvasPlot + std::string(Can_PhiBinned_MeanIC[iPhiBin]->GetName()) + ".pdf").c_str(), "pdf");
		Can_PhiBinned_MeanIC[iPhiBin]->Print((outputCanvasPlot + std::string(Can_PhiBinned_MeanIC[iPhiBin]->GetName()) + ".png").c_str(), "png");

		Can_PhiBinned_RMSIC[iPhiBin]->cd();
		Can_PhiBinned_RMSIC[iPhiBin]->SetGridx();
		Can_PhiBinned_RMSIC[iPhiBin]->SetGridy();
		PhiBinned_RMSIC[iPhiBin]->GetXaxis()->SetTitle("i#phi");
		PhiBinned_RMSIC[iPhiBin]->GetYaxis()->SetTitle("i#eta");
		PhiBinned_RMSIC[iPhiBin]->Draw("colz");
		PhiBinned_RMSIC[iPhiBin]->Draw("textSAME");
		Can_PhiBinned_RMSIC[iPhiBin]->Print((outputCanvasPlot + std::string(Can_PhiBinned_RMSIC[iPhiBin]->GetName()) + ".pdf").c_str(), "pdf");
		Can_PhiBinned_RMSIC[iPhiBin]->Print((outputCanvasPlot + std::string(Can_PhiBinned_RMSIC[iPhiBin]->GetName()) + ".png").c_str(), "png");


		if(!isDeadTriggerTower) {
			Can_PhiBinned_GausMeanIC[iPhiBin]->cd();
			Can_PhiBinned_GausMeanIC[iPhiBin]->SetGridx();
			Can_PhiBinned_GausMeanIC[iPhiBin]->SetGridy();
			PhiBinned_GausMeanIC[iPhiBin]->GetXaxis()->SetTitle("i#phi");
			PhiBinned_GausMeanIC[iPhiBin]->GetYaxis()->SetTitle("i#eta");
			PhiBinned_GausMeanIC[iPhiBin]->SetMinimum(PhiBinned_GausMeanIC[iPhiBin]->GetMinimum() + 0.9);
			PhiBinned_GausMeanIC[iPhiBin]->Draw("colz");
			PhiBinned_GausMeanIC[iPhiBin]->Draw("textSAME");
			Can_PhiBinned_GausMeanIC[iPhiBin]->Print((outputCanvasPlot + std::string(Can_PhiBinned_GausMeanIC[iPhiBin]->GetName()) + ".pdf").c_str(), "pdf");
			Can_PhiBinned_GausMeanIC[iPhiBin]->Print((outputCanvasPlot + std::string(Can_PhiBinned_GausMeanIC[iPhiBin]->GetName()) + ".png").c_str(), "png");

			Can_PhiBinned_GausRMSIC[iPhiBin]->cd();
			Can_PhiBinned_GausRMSIC[iPhiBin]->SetGridx();
			Can_PhiBinned_GausRMSIC[iPhiBin]->SetGridy();
			PhiBinned_GausRMSIC[iPhiBin]->GetXaxis()->SetTitle("i#phi");
			PhiBinned_GausRMSIC[iPhiBin]->GetYaxis()->SetTitle("i#eta");
			PhiBinned_GausRMSIC[iPhiBin]->Draw("colz");
			PhiBinned_GausRMSIC[iPhiBin]->Draw("textSAME");
			Can_PhiBinned_GausRMSIC[iPhiBin]->Print((outputCanvasPlot + std::string(Can_PhiBinned_RMSIC[iPhiBin]->GetName()) + ".pdf").c_str(), "pdf");
			Can_PhiBinned_GausRMSIC[iPhiBin]->Print((outputCanvasPlot + std::string(Can_PhiBinned_RMSIC[iPhiBin]->GetName()) + ".png").c_str(), "png");
		}
	}

	for(int iSMBin =  0; iSMBin < int(20 / NPhiSMBinDivision) ; iSMBin++) {

		Can_SMBinned_MeanIC[iSMBin] = new TCanvas(std::string(Form("Can_SMBinned_MeanIC_%d_%d", NPhiSMBinDivision * iSMBin, NPhiSMBinDivision * (iSMBin + 1))).c_str(), "", 600, 500);
		Can_SMBinned_RMSIC[iSMBin] = new TCanvas(std::string(Form("Can_SMBinned_RMSIC_%d_%d", NPhiSMBinDivision * iSMBin, NPhiSMBinDivision * (iSMBin + 1))).c_str(), "", 600, 500);
		Can_SMBinned_GausMeanIC[iSMBin] = new TCanvas(std::string(Form("Can_SMBinned_GausMeanIC_%d_%d", NPhiSMBinDivision * iSMBin, NPhiSMBinDivision * (iSMBin + 1))).c_str(), "", 600, 500);
		Can_SMBinned_GausRMSIC[iSMBin] = new TCanvas(std::string(Form("Can_SMBinned_GausRMSIC_%d_%d", NPhiSMBinDivision * iSMBin, NPhiSMBinDivision * (iSMBin + 1))).c_str(), "", 600, 500);

		Can_SMBinned_MeanIC[iSMBin]->cd();
		Can_SMBinned_MeanIC[iSMBin]->SetGridx();
		Can_SMBinned_MeanIC[iSMBin]->SetGridy();
		SMBinned_MeanIC[iSMBin]->GetXaxis()->SetTitle("i#phi");
		SMBinned_MeanIC[iSMBin]->GetYaxis()->SetTitle("i#eta");
		if(!isDeadTriggerTower) SMBinned_MeanIC[iSMBin]->SetMinimum(SMBinned_MeanIC[iSMBin]->GetMinimum() + 0.9);
		else SMBinned_MeanIC[iSMBin]->SetMinimum(SMBinned_MeanIC[iSMBin]->GetMinimum() + 0.75);
		SMBinned_MeanIC[iSMBin]->Draw("colz");
		SMBinned_MeanIC[iSMBin]->Draw("textSAME");
		Can_SMBinned_MeanIC[iSMBin]->Print((outputCanvasPlot + std::string(Can_SMBinned_MeanIC[iSMBin]->GetName()) + ".pdf").c_str(), "pdf");
		Can_SMBinned_MeanIC[iSMBin]->Print((outputCanvasPlot + std::string(Can_SMBinned_MeanIC[iSMBin]->GetName()) + ".png").c_str(), "png");

		Can_SMBinned_RMSIC[iSMBin]->cd();
		Can_SMBinned_RMSIC[iSMBin]->SetGridx();
		Can_SMBinned_RMSIC[iSMBin]->SetGridy();
		SMBinned_RMSIC[iSMBin]->GetXaxis()->SetTitle("i#phi");
		SMBinned_RMSIC[iSMBin]->GetYaxis()->SetTitle("i#eta");
		SMBinned_RMSIC[iSMBin]->Draw("colz");
		SMBinned_RMSIC[iSMBin]->Draw("textSAME");
		Can_SMBinned_RMSIC[iSMBin]->Print((outputCanvasPlot + std::string(Can_SMBinned_RMSIC[iSMBin]->GetName()) + ".pdf").c_str(), "pdf");
		Can_SMBinned_RMSIC[iSMBin]->Print((outputCanvasPlot + std::string(Can_SMBinned_RMSIC[iSMBin]->GetName()) + ".png").c_str(), "png");

		if(!isDeadTriggerTower) {
			Can_SMBinned_GausMeanIC[iSMBin]->cd();
			Can_SMBinned_GausMeanIC[iSMBin]->SetGridx();
			Can_SMBinned_GausMeanIC[iSMBin]->SetGridy();
			SMBinned_GausMeanIC[iSMBin]->GetXaxis()->SetTitle("i#phi");
			SMBinned_GausMeanIC[iSMBin]->GetYaxis()->SetTitle("i#eta");
			SMBinned_GausMeanIC[iSMBin]->SetMinimum(SMBinned_GausMeanIC[iSMBin]->GetMinimum() + 0.9);
			SMBinned_GausMeanIC[iSMBin]->Draw("colz");
			SMBinned_GausMeanIC[iSMBin]->Draw("textSAME");
			Can_SMBinned_GausMeanIC[iSMBin]->Print((outputCanvasPlot + std::string(Can_SMBinned_GausMeanIC[iSMBin]->GetName()) + ".pdf").c_str(), "pdf");
			Can_SMBinned_GausMeanIC[iSMBin]->Print((outputCanvasPlot + std::string(Can_SMBinned_GausMeanIC[iSMBin]->GetName()) + ".png").c_str(), "png");

			Can_SMBinned_GausRMSIC[iSMBin]->cd();
			Can_SMBinned_GausRMSIC[iSMBin]->SetGridx();
			Can_SMBinned_GausRMSIC[iSMBin]->SetGridy();
			SMBinned_GausRMSIC[iSMBin]->GetXaxis()->SetTitle("i#phi");
			SMBinned_GausRMSIC[iSMBin]->GetYaxis()->SetTitle("i#eta");
			SMBinned_GausRMSIC[iSMBin]->Draw("colz");
			SMBinned_GausRMSIC[iSMBin]->Draw("textSAME");
			Can_SMBinned_GausRMSIC[iSMBin]->Print((outputCanvasPlot + std::string(Can_SMBinned_RMSIC[iSMBin]->GetName()) + ".pdf").c_str(), "pdf");
			Can_SMBinned_GausRMSIC[iSMBin]->Print((outputCanvasPlot + std::string(Can_SMBinned_RMSIC[iSMBin]->GetName()) + ".png").c_str(), "png");
		}
	}

	return 0;

}


//  LocalWords:  ieta
