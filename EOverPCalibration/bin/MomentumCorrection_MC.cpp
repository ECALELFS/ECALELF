#include "../interface/GetHashedIndexEB.h"
#include <vector>
#include <utility>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include "TFile.h"
#include "TStyle.h"
#include "TProfile2D.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TROOT.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "TTree.h"
#include "TApplication.h"
using namespace std;
/// Check if the crystal is near to a dead one

bool CheckxtalIC (TH2F* h_scale_EB, int iPhi, int iEta )
{
	if(h_scale_EB->GetBinContent(iPhi, iEta) == 0) return false;

	int bx = h_scale_EB->GetNbinsX();
	int by = h_scale_EB->GetNbinsY();

	if((iPhi < bx && h_scale_EB->GetBinContent(iPhi + 1, iEta) == 0) || (h_scale_EB->GetBinContent(iPhi - 1, iEta) == 0 && iPhi > 1)) return false;

	if((iEta < by && h_scale_EB->GetBinContent(iPhi, iEta + 1) == 0 && iEta != 85 ) || (h_scale_EB->GetBinContent(iPhi, iEta - 1) == 0 && iEta > 1 && iEta != 87)) return false;

	if((iPhi < bx && h_scale_EB->GetBinContent(iPhi + 1, iEta + 1) == 0 && iEta != 85 && iEta < by) || ( h_scale_EB->GetBinContent(iPhi - 1, iEta - 1) == 0 && iEta > 1 && iEta != 87 && iPhi > 1)) return false;

	if((h_scale_EB->GetBinContent(iPhi + 1, iEta - 1) == 0 && iEta > 1 && iEta != 87 && iPhi < bx) || ( h_scale_EB->GetBinContent(iPhi - 1, iEta + 1) == 0 && iPhi > 1 && iEta != 85 && iEta < by )) return false;

	return true;

}

/// Check if the crystal is near to a dead TT

bool CheckxtalTT (int iPhi, int iEta, std::vector<std::pair<int, int> >& TT_centre )
{
	for(unsigned int k = 0; k < TT_centre.size(); k++) {
		if(fabs(iPhi - TT_centre.at(k).second) < 5 && fabs(iEta - 86 - TT_centre.at(k).first) < 5) return false;

	}
	return true;
}


int main (int argc, char **argv)
{
	/// Mc Ntuplas
	TString input = Form("/data1/rgerosa/NTUPLES_FINAL_CALIB/MC/WJetsToLNu_DYJetsToLL_7TeV-madgraph-tauola_Fall11_All.root");
	/// MC Calibration result E/p
	TString input2 = Form("/data1/rgerosa/L3_Weight/MC_WJets/EB_Z_recoFlag/WJetsToLNu_DYJetsToLL_7TeV-madgraph-tauola_Fall11_Z_noEP.root");

	TApplication* theApp = new TApplication("Application", &argc, argv);

	TFile *f = new TFile(input, "");
	TTree *inputTree = (TTree*)f->Get("ntu");

	TFile *f2 = new TFile(input2, "");
	TH2F *h_scale_EB = (TH2F*)f2->Get("h_scale_EB");
	TH2F *hcmap = (TH2F*) h_scale_EB->Clone("hcmap");

	hcmap -> Reset("ICEMS");
	hcmap -> ResetStats();

	/// Taking infos

	std::vector<float>* ele1_recHit_E = 0;
	std::vector<float>* ele2_recHit_E = 0;
	std::vector<int>* ele1_recHit_hashedIndex = 0;
	std::vector<int>* ele2_recHit_hashedIndex = 0;
	std::vector<int>* ele1_recHit_flag = 0;
	std::vector<int>* ele2_recHit_flag = 0;
	float ele1_E_true, ele2_E_true;
	float ele1_tkP, ele2_tkP;
	int ele1_isEB, ele2_isEB;
	float   ele1_fbrem, ele2_fbrem;
	int isW, isZ;

	inputTree->SetBranchAddress("ele1_recHit_E", &ele1_recHit_E);
	inputTree->SetBranchAddress("ele2_recHit_E", &ele2_recHit_E);
	inputTree->SetBranchAddress("ele1_recHit_hashedIndex", &ele1_recHit_hashedIndex);
	inputTree->SetBranchAddress("ele2_recHit_hashedIndex", &ele2_recHit_hashedIndex);
	inputTree->SetBranchAddress("ele1_recHit_flag", &ele1_recHit_flag);
	inputTree->SetBranchAddress("ele2_recHit_flag", &ele2_recHit_flag);
	inputTree->SetBranchAddress("ele1_E_true", &ele1_E_true);
	inputTree->SetBranchAddress("ele2_E_true", &ele2_E_true);
	inputTree->SetBranchAddress("ele1_tkP", &ele1_tkP);
	inputTree->SetBranchAddress("ele2_tkP", &ele2_tkP);
	inputTree->SetBranchAddress("ele1_isEB", &ele1_isEB);
	inputTree->SetBranchAddress("ele2_isEB", &ele2_isEB);
	inputTree->SetBranchAddress("ele1_fbrem", &ele1_fbrem);
	inputTree->SetBranchAddress("ele2_fbrem", &ele2_fbrem);
	inputTree->SetBranchAddress("isW", &isW);
	inputTree->SetBranchAddress("isZ", &isZ);

	TProfile2D* mapMomentum = new TProfile2D("mapMomentum", "mapMomentum", 360, 0, 360, 170, -85, 85);
	TProfile2D* mapfbrem = new TProfile2D("mapfbrem", "mapfbrem", 360, 0, 360, 170, -85, 85);

	/// Make fbrem and p/ptrue map cycling on MC --> all the events

	for(Long64_t i = 0; i < inputTree->GetEntries(); i++) {
		inputTree->GetEntry(i);
		if (!(i % 100000))std::cerr << i;
		if (!(i % 10000)) std::cerr << ".";

		if (ele1_isEB == 1 && (isW == 1 || isZ == 1)) {

			double E_seed = 0;
			int seed_hashedIndex = -1; // iseed;

			for (unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {

				if(ele1_recHit_E -> at(iRecHit) > E_seed &&  ele1_recHit_flag->at(iRecHit) < 4 ) { /// control if this recHit is good
					seed_hashedIndex = ele1_recHit_hashedIndex -> at(iRecHit);
					//  iseed=iRecHit;
					E_seed = ele1_recHit_E -> at(iRecHit); ///! Seed search

				}
			}

			int eta_seed = GetIetaFromHashedIndex(seed_hashedIndex);
			int phi_seed = GetIphiFromHashedIndex(seed_hashedIndex);
			if(ele1_tkP > 0 && ele1_E_true > 0 && abs(ele1_tkP / ele1_E_true) < 2. && abs(ele1_tkP / ele1_E_true) > 0.5) mapMomentum->Fill(phi_seed, eta_seed, abs(ele1_tkP / ele1_E_true));
			mapfbrem->Fill(phi_seed, eta_seed, abs(ele1_fbrem));
		}

		if (ele2_isEB == 1 && isZ == 1) {

			double E_seed = 0;
			int seed_hashedIndex = -1; // iseed;

			for (unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {

				if(ele2_recHit_E -> at(iRecHit) > E_seed &&  ele2_recHit_flag->at(iRecHit) < 4 ) { /// control if this recHit is good
					seed_hashedIndex = ele2_recHit_hashedIndex -> at(iRecHit);
					// iseed=iRecHit;
					E_seed = ele2_recHit_E -> at(iRecHit); ///! Seed search

				}
			}

			int eta_seed = GetIetaFromHashedIndex(seed_hashedIndex);
			int phi_seed = GetIphiFromHashedIndex(seed_hashedIndex);
			if(ele2_tkP > 0 && ele2_E_true > 0 && abs(ele2_tkP / ele2_E_true) < 2. && abs(ele2_tkP / ele2_E_true) > 0.5) mapMomentum->Fill(phi_seed, eta_seed, abs(ele2_tkP / ele2_E_true));
			mapfbrem->Fill(phi_seed, eta_seed, abs(ele2_fbrem));

		}
	}

/// Map of IC normalized in eta rings

	std::vector< std::pair<int, int> > TT_centre ;

	TT_centre.push_back(std::pair<int, int> (58, 49));
	TT_centre.push_back(std::pair<int, int> (53, 109));
	TT_centre.push_back(std::pair<int, int> (8, 114));
	TT_centre.push_back(std::pair<int, int> (83, 169));
	TT_centre.push_back(std::pair<int, int> (53, 174));
	TT_centre.push_back(std::pair<int, int> (63, 194));
	TT_centre.push_back(std::pair<int, int> (83, 224));
	TT_centre.push_back(std::pair<int, int> (73, 344));
	TT_centre.push_back(std::pair<int, int> (83, 358));
	TT_centre.push_back(std::pair<int, int> (-13, 18));
	TT_centre.push_back(std::pair<int, int> (-18, 23));
	TT_centre.push_back(std::pair<int, int> (-8, 53));
	TT_centre.push_back(std::pair<int, int> (-3, 63));
	TT_centre.push_back(std::pair<int, int> (-53, 128));
	TT_centre.push_back(std::pair<int, int> (-53, 183));
	TT_centre.push_back(std::pair<int, int> (-83, 193));
	TT_centre.push_back(std::pair<int, int> (-74, 218));
	TT_centre.push_back(std::pair<int, int> (-8, 223));
	TT_centre.push_back(std::pair<int, int> (-68, 303));
	TT_centre.push_back(std::pair<int, int> (-43, 328));

/// Mean over phi corrected skipping dead channel

	for (int iEta = 1 ; iEta < h_scale_EB->GetNbinsY() + 1; iEta ++) {
		float SumIC = 0;
		int numIC = 0;

		for(int iPhi = 1 ; iPhi < h_scale_EB->GetNbinsX() + 1 ; iPhi++) {
			bool isGood = CheckxtalIC(h_scale_EB, iPhi, iEta);
			bool isGoodTT = CheckxtalTT(iPhi, iEta, TT_centre);

			if(isGood && isGoodTT) {
				SumIC = SumIC + h_scale_EB->GetBinContent(iPhi, iEta);
				numIC ++ ;
			}
		}
		//fede: skip bad channels and bad TTs
		for (int iPhi = 1; iPhi < h_scale_EB->GetNbinsX() + 1  ; iPhi++) {
			if(numIC == 0 || SumIC == 0) continue;

			bool isGood = CheckxtalIC(h_scale_EB, iPhi, iEta);
			bool isGoodTT = CheckxtalTT(iPhi, iEta, TT_centre);
			if (!isGood || !isGoodTT) continue;

			hcmap->SetBinContent(iPhi, iEta, h_scale_EB->GetBinContent(iPhi, iEta) / (SumIC / numIC));
		}
	}

/// ratio map

	TH2F* ratioMap = (TH2F*) hcmap -> Clone("ratioMap");
	ratioMap->Reset();

	for( int i = 0 ; i < hcmap->GetNbinsX() ; i++) {
		for( int j = 0; j < hcmap->GetNbinsY() ; j++) {
			if(hcmap->GetBinContent(i, j) != 0 && mapMomentum->GetBinContent(i, j) != 0)
				ratioMap->SetBinContent(i + 1, j + 1, mapMomentum->GetBinContent(i, j) / hcmap->GetBinContent(i, j));
		}
	}

/// Profile along phi taking into account dead channels
	TGraphErrors *coeffEBp = new TGraphErrors();
	TGraphErrors *coeffEBm = new TGraphErrors();

	for (int iPhi = 1; iPhi < hcmap->GetNbinsX() + 1 ; iPhi++) {
		double SumEBp = 0, SumEBm = 0;
		double iEBp = 0, iEBm = 0;
		for(int iEta = 1; iEta < hcmap->GetNbinsY() + 1 ; iEta++) {
			if(hcmap->GetBinContent(iPhi, iEta) == 0)continue;
			if(iEta > 85) {
				SumEBp = SumEBp + mapMomentum->GetBinContent(iPhi, iEta) / hcmap->GetBinContent(iPhi, iEta);
				iEBp++;
			} else {
				SumEBm = SumEBm + mapMomentum->GetBinContent(iPhi, iEta) / hcmap->GetBinContent(iPhi, iEta);
				iEBm++;
			}
		}
		coeffEBp->SetPoint(iPhi - 1, iPhi - 1, SumEBp / iEBp);
		coeffEBm->SetPoint(iPhi - 1, iPhi - 1, SumEBm / iEBm);

	}

	TFile* outputGraph = new TFile("output/GraphFor_P_Correction.root", "RECREATE");
	outputGraph->cd();

	coeffEBp->Write("coeffEBp");
	coeffEBm->Write("coeffEBm");
	outputGraph->Close();

	gROOT->Reset();
	gROOT->SetStyle("Plain");

	gStyle->SetPadTickX(1);
	gStyle->SetPadTickY(1);
	gStyle->SetOptTitle(1);
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(0);
	gStyle->SetFitFormat("6.3g");
	gStyle->SetPalette(1);

	gStyle->SetTextFont(42);
	gStyle->SetTextSize(0.05);
	gStyle->SetTitleFont(42, "xyz");
	gStyle->SetTitleSize(0.05);
	gStyle->SetLabelFont(42, "xyz");
	gStyle->SetLabelSize(0.05);
	gStyle->SetTitleXOffset(0.8);
	gStyle->SetTitleYOffset(1.1);
	gROOT->ForceStyle();

	TCanvas* c1 = new TCanvas("mapMomentum", "mapMomentum", 1);
	c1->cd();
	mapMomentum->GetXaxis()->SetTitle("#phi");
	mapMomentum->GetXaxis()->SetNdivisions(20);
	c1->SetGridx();
	mapMomentum->GetYaxis()->SetTitle("#eta");
	mapMomentum->GetZaxis()->SetRangeUser(0.7, 1.3);
	mapMomentum->Draw("colz");

	TCanvas* c2 = new TCanvas("mapfbrem", "mapfbrem", 1);
	c2->cd();
	mapfbrem->GetXaxis()->SetTitle("#phi");
	mapfbrem->GetYaxis()->SetTitle("#eta");
	mapfbrem->GetXaxis()->SetNdivisions(20);
	c2->SetGridx();
	mapfbrem->GetZaxis()->SetRangeUser(0., 0.7);
	mapfbrem->Draw("colz");

	TCanvas* c3 = new TCanvas("ratioMap", "ratioMap", 1);
	c3->cd();
	ratioMap->GetXaxis()->SetTitle("#phi");
	ratioMap->GetYaxis()->SetTitle("#eta");
	ratioMap->GetXaxis()->SetNdivisions(20);
	c3->SetGridx();
	ratioMap->GetZaxis()->SetRangeUser(0.7, 1.3);
	ratioMap->Draw("colz");

	TCanvas* c4 = new TCanvas("coeffEB", "coeffEB", 1);
	c4->cd();
	coeffEBp->GetXaxis()->SetTitle("#phi");
	coeffEBp->GetYaxis()->SetTitle("p/p_{true}");
	coeffEBp -> SetMarkerStyle(20);
	coeffEBp -> SetMarkerSize(1);
	coeffEBp -> SetMarkerColor(kRed + 1);
	coeffEBp -> SetLineColor(kRed + 1);
	c4->SetGridx();
	c4->SetGridy();
	ratioMap->Draw("ap");

	coeffEBm->GetXaxis()->SetTitle("#phi");
	coeffEBm->GetYaxis()->SetTitle("p/p_{true}");
	coeffEBm -> SetMarkerStyle(20);
	coeffEBm -> SetMarkerSize(1);
	coeffEBm -> SetMarkerColor(kBlue + 1);
	coeffEBm -> SetLineColor(kBlue + 1);
	coeffEBm->Draw("ap same");


	theApp->Run();
	return 0;
}
