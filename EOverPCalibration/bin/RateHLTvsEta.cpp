#include <iostream>
#include <iomanip>
#include <cmath>
#include "TFile.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TROOT.h"
#include "TLatex.h"
#include "TTree.h"
#include "TChain.h"
#include "TGraphAsymmErrors.h"

#include <vector>
#include <map>
#include "../interface/treeReader.h"


int main (int argc, char ** argv)
{

	//  double xtalWidth=0.01745329;
	//  double luminosity=11.53;
	//  double etaringEE=0.0325;


	// Acquisition of input file
	TChain* treeDATA_HLT = new TChain("simpleNtupleEoverP/SimpleNtupleEoverP");
	treeDATA_HLT->Add("/gwteray/users/gerosa/ECALNTUPLES/22Jan2013_HLT_Rate_Study/SingleElectron_Run2012A-22Jan2013-v1_HLT.root");
	treeDATA_HLT->Add("/gwteray/users/gerosa/ECALNTUPLES/22Jan2013_HLT_Rate_Study/SingleElectron_Run2012B-22Jan2013-v1_HLT.root");
	//  treeDATA_HLT->Add("/gwteray/users/gerosa/ECALNTUPLES/22Jan2013_HLT_Rate_Study/SingleElectron_Run2012C-22Jan2013-v1_HLT.root");
	treeDATA_HLT->Add("/gwteray/users/gerosa/ECALNTUPLES/22Jan2013_HLT_Rate_Study/SingleElectron_Run2012D-22Jan2013-v1_HLT.root");

	treeReader *fReader_HLT = new treeReader((TTree*)(treeDATA_HLT), false);

	TH1F* Denominator                      = new TH1F ("Denominator", "", 100, -2.5, 2.5);
	TH1F* HLTEle27_RateVsEta               = new TH1F ("HLTEle27_RateVsEta", "", 100, -2.5, 2.5);
	TH1F* HLTEle27_PFMET_RateVsEta         = new TH1F ("HLTEle27_PFMET_RateVsEta", "", 100, -2.5, 2.5);
	TH1F* HLTEle27_Offline_RateVsEta       = new TH1F ("HLTEle27_Offline_RateVsEta", "", 100, -2.5, 2.5);
	TH1F* HLTEle27_Offline_R9_RateVsEta    = new TH1F ("HLTEle27_R9_RateVsEta", "", 100, -2.5, 2.5);
	TH1F* HLTEle27_Offline_fbrem_RateVsEta = new TH1F ("HLTEle27_fbrem_RateVsEta", "", 100, -2.5, 2.5);
	TH1F* HLTEle27_Offline_Pt_RateVsEta    = new TH1F ("HLTEle27_Pt_RateVsEta", "", 100, -2.5, 2.5);
	TH1F* HLTEle27_Offline_PFMET_RateVsEta = new TH1F ("HLTEle27_Offline_PFMET_RateVsEta", "", 100, -2.5, 2.5);
	TH1F* HLTEle27_PFMET_VsEle27Offline    = new TH1F ("HLTEle27_PFMET_VsEle27Offline", "", 100, -2.5, 2.5);

	std::cout << " DATA Entries = " << treeDATA_HLT->GetEntries() << std::endl;

	for(int entry = 0; entry < treeDATA_HLT->GetEntries(); ++entry) {

		if( entry % 100000 == 0 ) std::cout << "reading saved entry " << entry << "\r" << std::flush;
		treeDATA_HLT->GetEntry(entry);

		Denominator->Fill(fReader_HLT->getFloat("ele1_scEta")[0]);

		if(fReader_HLT->getInt("isHLTEle27")[0] == 1 )      HLTEle27_RateVsEta->Fill(fReader_HLT->getFloat("ele1_scEta")[0]);

		if(fReader_HLT->getInt("isHLTELe27PFMET")[0] == 1 ) HLTEle27_PFMET_RateVsEta->Fill(fReader_HLT->getFloat("ele1_scEta")[0]);

		if(fReader_HLT->getInt("isGood")[0] == 1 && fReader_HLT->getInt("isHLTEle27")[0] == 1 && fReader_HLT->getInt("isZ")[0] == 0)
			HLTEle27_Offline_RateVsEta->Fill(fReader_HLT->getFloat("ele1_scEta")[0]);

		if(fReader_HLT->getInt("isGood")[0] == 1 && fReader_HLT->getInt("isHLTEle27")[0] == 1 && fReader_HLT->getInt("isZ")[0] == 0 && fReader_HLT->getFloat("ele1_r9")[0] > 0.94)
			HLTEle27_Offline_R9_RateVsEta->Fill(fReader_HLT->getFloat("ele1_scEta")[0]);

		if(fReader_HLT->getInt("isGood")[0] == 1 && fReader_HLT->getInt("isHLTEle27")[0] == 1 && fReader_HLT->getInt("isZ")[0] == 0 && fabs(fReader_HLT->getFloat("ele1_fbrem")[0]) < 0.5)
			HLTEle27_Offline_fbrem_RateVsEta->Fill(fReader_HLT->getFloat("ele1_scEta")[0]);

		if(fReader_HLT->getInt("isGood")[0] == 1 && fReader_HLT->getInt("isHLTEle27")[0] == 1 && fReader_HLT->getInt("isZ")[0] == 0 && fabs(fReader_HLT->getFloat("ele1_pt")[0]) > 50)
			HLTEle27_Offline_Pt_RateVsEta->Fill(fReader_HLT->getFloat("ele1_scEta")[0]);

		if(fReader_HLT->getInt("isGood")[0] == 1 && fReader_HLT->getInt("isHLTELe27PFMET")[0] == 1 && fReader_HLT->getInt("isZ")[0] == 0 )
			HLTEle27_Offline_PFMET_RateVsEta->Fill(fReader_HLT->getFloat("ele1_scEta")[0]);

		if( fReader_HLT->getInt("isHLTELe27PFMET")[0] == 1 && fReader_HLT->getInt("isZ")[0] == 0 && fReader_HLT->getInt("isGood")[0] == 1)
			HLTEle27_PFMET_VsEle27Offline->Fill(fReader_HLT->getFloat("ele1_scEta")[0]);



	}


	TGraphAsymmErrors* HLTEle27_Efficiency               = new TGraphAsymmErrors(HLTEle27_RateVsEta, Denominator);
	TGraphAsymmErrors* HLTEle27_PFMET_Efficiency         = new TGraphAsymmErrors(HLTEle27_PFMET_RateVsEta, Denominator);
	TGraphAsymmErrors* HLTEle27_Offline_Efficiency       = new TGraphAsymmErrors(HLTEle27_Offline_RateVsEta, HLTEle27_RateVsEta);
	TGraphAsymmErrors* HLTEle27_Offline_R9_Efficiency    = new TGraphAsymmErrors(HLTEle27_Offline_R9_RateVsEta, HLTEle27_RateVsEta);
	TGraphAsymmErrors* HLTEle27_Offline_fbrem_Efficiency = new TGraphAsymmErrors(HLTEle27_Offline_fbrem_RateVsEta, HLTEle27_RateVsEta);
	TGraphAsymmErrors* HLTEle27_Offline_Pt_Efficiency    = new TGraphAsymmErrors(HLTEle27_Offline_Pt_RateVsEta, HLTEle27_RateVsEta);
	TGraphAsymmErrors* HLTEle27_Offline_PFMET_Efficiency = new TGraphAsymmErrors(HLTEle27_Offline_PFMET_RateVsEta, HLTEle27_PFMET_RateVsEta);
	TGraphAsymmErrors* HLTEle27_PFMET_VsEle27Offline_Efficiency = new TGraphAsymmErrors(HLTEle27_PFMET_VsEle27Offline, HLTEle27_Offline_RateVsEta);

	HLTEle27_Efficiency->GetXaxis()->SetRangeUser(-2.4, 2.4);
	HLTEle27_PFMET_Efficiency->GetXaxis()->SetRangeUser(-2.4, 2.4);
	HLTEle27_Offline_Efficiency->GetXaxis()->SetRangeUser(-2.4, 2.4);
	HLTEle27_Offline_R9_Efficiency->GetXaxis()->SetRangeUser(-2.4, 2.4);
	HLTEle27_Offline_fbrem_Efficiency->GetXaxis()->SetRangeUser(-2.4, 2.4);
	HLTEle27_Offline_Pt_Efficiency->GetXaxis()->SetRangeUser(-2.4, 2.4);
	HLTEle27_Offline_PFMET_Efficiency->GetXaxis()->SetRangeUser(-2.4, 2.4);
	HLTEle27_PFMET_VsEle27Offline_Efficiency->GetXaxis()->SetRangeUser(-2.4, 2.4);


	HLTEle27_Efficiency->GetXaxis()->SetTitle("#eta_{SC}");
	HLTEle27_PFMET_Efficiency->GetXaxis()->SetTitle("#eta_{SC}");
	HLTEle27_Offline_Efficiency->GetXaxis()->SetTitle("#eta_{SC}");
	HLTEle27_Offline_R9_Efficiency->GetXaxis()->SetTitle("#eta_{SC}");
	HLTEle27_Offline_fbrem_Efficiency->GetXaxis()->SetTitle("#eta_{SC}");
	HLTEle27_Offline_Pt_Efficiency->GetXaxis()->SetTitle("#eta_{SC}");
	HLTEle27_Offline_PFMET_Efficiency->GetXaxis()->SetTitle("#eta_{SC}");
	HLTEle27_PFMET_VsEle27Offline_Efficiency->GetXaxis()->SetTitle("#eta_{SC}");

	HLTEle27_Efficiency->GetYaxis()->SetTitle("#varepsilon = #frac{N^{Ele27}}{N^{all}}");
	HLTEle27_PFMET_Efficiency->GetYaxis()->SetTitle("#varepsilon = #frac{N^{Ele27PFMET}}{N^{all}}");
	HLTEle27_Offline_Efficiency->GetYaxis()->SetTitle("#varepsilon = #frac{N^{Offline}}{N^{Ele27}}");
	HLTEle27_Offline_R9_Efficiency->GetYaxis()->SetTitle("#varepsilon = #frac{N^{R9}}{N^{Ele27}}");
	HLTEle27_Offline_fbrem_Efficiency->GetYaxis()->SetTitle("#varepsilon = #frac{N^{fbrem}}{N^{Ele27}}");
	HLTEle27_Offline_Pt_Efficiency->GetYaxis()->SetTitle("#varepsilon = #frac{N^{pT}}{N^{Ele27}}");
	HLTEle27_Offline_PFMET_Efficiency->GetYaxis()->SetTitle("#varepsilon = #frac{N^{Offline}}{N^{Ele27PFMET}}");
	HLTEle27_PFMET_VsEle27Offline_Efficiency->GetYaxis()->SetTitle("#varepsilon = #frac{N^{Off+Ele27PFMET}}{N^{Off+Ele27}}");

	HLTEle27_Efficiency->SetMarkerStyle(20);
	HLTEle27_PFMET_Efficiency->SetMarkerStyle(20);
	HLTEle27_Offline_Efficiency->SetMarkerStyle(20);
	HLTEle27_Offline_R9_Efficiency->SetMarkerStyle(20);
	HLTEle27_Offline_fbrem_Efficiency->SetMarkerStyle(20);
	HLTEle27_Offline_Pt_Efficiency->SetMarkerStyle(20);
	HLTEle27_Offline_PFMET_Efficiency->SetMarkerStyle(20);
	HLTEle27_PFMET_VsEle27Offline_Efficiency->SetMarkerStyle(20);

	HLTEle27_Efficiency->SetMarkerSize(1.2);
	HLTEle27_PFMET_Efficiency->SetMarkerSize(1.2);
	HLTEle27_Offline_Efficiency->SetMarkerSize(1.2);
	HLTEle27_Offline_R9_Efficiency->SetMarkerSize(1.2);
	HLTEle27_Offline_fbrem_Efficiency->SetMarkerSize(1.2);
	HLTEle27_Offline_Pt_Efficiency->SetMarkerSize(1.2);
	HLTEle27_Offline_PFMET_Efficiency->SetMarkerSize(1.2);
	HLTEle27_PFMET_VsEle27Offline_Efficiency->SetMarkerSize(1.2);

	HLTEle27_Efficiency->SetMarkerColor(kBlue);
	HLTEle27_PFMET_Efficiency->SetMarkerColor(kBlue);
	HLTEle27_Offline_Efficiency->SetMarkerColor(kBlue);
	HLTEle27_Offline_R9_Efficiency->SetMarkerColor(kBlue);
	HLTEle27_Offline_fbrem_Efficiency->SetMarkerColor(kBlue);
	HLTEle27_Offline_Pt_Efficiency->SetMarkerColor(kBlue);
	HLTEle27_Offline_PFMET_Efficiency->SetMarkerColor(kBlue);
	HLTEle27_PFMET_VsEle27Offline_Efficiency->SetMarkerColor(kBlue);

	HLTEle27_Efficiency->SetLineColor(kBlack);
	HLTEle27_PFMET_Efficiency->SetLineColor(kBlack);
	HLTEle27_Offline_Efficiency->SetLineColor(kBlack);
	HLTEle27_Offline_R9_Efficiency->SetLineColor(kBlack);
	HLTEle27_Offline_fbrem_Efficiency->SetLineColor(kBlack);
	HLTEle27_Offline_Pt_Efficiency->SetLineColor(kBlack);
	HLTEle27_Offline_PFMET_Efficiency->SetLineColor(kBlack);
	HLTEle27_PFMET_VsEle27Offline_Efficiency->SetLineColor(kBlack);

	TCanvas* cHLTEle27_Efficiency = new TCanvas("cHLTEle27_Efficiency", "", 1);
	cHLTEle27_Efficiency->cd();
	cHLTEle27_Efficiency->SetGridx();
	cHLTEle27_Efficiency->SetGridy();
	HLTEle27_Efficiency->Draw("ap");

	TCanvas* cHLTEle27_PFMET_Efficiency = new TCanvas("cHLTEle27_PFMET_Efficiency", "", 1);
	cHLTEle27_PFMET_Efficiency ->cd();
	cHLTEle27_PFMET_Efficiency ->SetGridx();
	cHLTEle27_PFMET_Efficiency ->SetGridy();
	HLTEle27_PFMET_Efficiency->Draw("ap");

	TCanvas* cHLTEle27_Offline_Efficiency = new TCanvas("cHLTEle27_Offline_Efficiency", "", 1);
	cHLTEle27_Offline_Efficiency ->cd();
	cHLTEle27_Offline_Efficiency ->SetGridx();
	cHLTEle27_Offline_Efficiency ->SetGridy();
	HLTEle27_Offline_Efficiency  ->Draw("ap");

	TCanvas* cHLTEle27_Offline_R9_Efficiency = new TCanvas("cHLTEle27_Offline_R9_Efficiency", "", 1);
	cHLTEle27_Offline_R9_Efficiency ->cd();
	cHLTEle27_Offline_R9_Efficiency ->SetGridx();
	cHLTEle27_Offline_R9_Efficiency ->SetGridy();
	HLTEle27_Offline_R9_Efficiency  ->Draw("ap");

	TCanvas* cHLTEle27_Offline_fbrem_Efficiency = new TCanvas("cHLTEle27_Offline_fbrem_Efficiency", "", 1);
	cHLTEle27_Offline_fbrem_Efficiency ->cd();
	cHLTEle27_Offline_fbrem_Efficiency ->SetGridx();
	cHLTEle27_Offline_fbrem_Efficiency ->SetGridy();
	HLTEle27_Offline_fbrem_Efficiency  ->Draw("ap");

	TCanvas* cHLTEle27_Offline_Pt_Efficiency = new TCanvas("cHLTEle27_Offline_Pt_Efficiency", "", 1);
	cHLTEle27_Offline_Pt_Efficiency ->cd();
	cHLTEle27_Offline_Pt_Efficiency ->SetGridx();
	cHLTEle27_Offline_Pt_Efficiency ->SetGridy();
	HLTEle27_Offline_Pt_Efficiency  ->Draw("ap");

	TCanvas* cHLTEle27_Offline_PFMET_Efficiency = new TCanvas("cHLTEle27_Offline_PFMET_Efficiency", "", 1);
	cHLTEle27_Offline_PFMET_Efficiency ->cd();
	cHLTEle27_Offline_PFMET_Efficiency ->SetGridx();
	cHLTEle27_Offline_PFMET_Efficiency ->SetGridy();
	HLTEle27_Offline_PFMET_Efficiency ->Draw("ap");


	TCanvas* cHLTEle27_PFMET_VsEle27Offline_Efficiency = new TCanvas("HLTEle27_PFMET_VsEle27Offline_Efficiency", "", 1);
	cHLTEle27_PFMET_VsEle27Offline_Efficiency ->cd();
	cHLTEle27_PFMET_VsEle27Offline_Efficiency ->SetGridx();
	cHLTEle27_PFMET_VsEle27Offline_Efficiency ->SetGridy();
	HLTEle27_PFMET_VsEle27Offline_Efficiency  ->Draw("ap");


	TFile *output = new TFile("output/outputTriggerRate.root", "RECREATE");
	output->cd();
	cHLTEle27_Efficiency->Write();
	cHLTEle27_PFMET_Efficiency->Write();
	cHLTEle27_Offline_Efficiency->Write();
	cHLTEle27_Offline_R9_Efficiency->Write();
	cHLTEle27_Offline_fbrem_Efficiency->Write();
	cHLTEle27_Offline_Pt_Efficiency->Write();
	cHLTEle27_Offline_PFMET_Efficiency->Write();
	cHLTEle27_PFMET_VsEle27Offline_Efficiency->Write();

	Denominator->Write();
	HLTEle27_RateVsEta->Write();
	HLTEle27_PFMET_RateVsEta->Write();
	HLTEle27_Offline_RateVsEta->Write();
	HLTEle27_Offline_R9_RateVsEta->Write();
	HLTEle27_Offline_fbrem_RateVsEta->Write();
	HLTEle27_Offline_Pt_RateVsEta->Write();
	HLTEle27_Offline_PFMET_RateVsEta->Write();
	HLTEle27_PFMET_VsEle27Offline->Write();
	output->Close();

	return 0 ;

}
