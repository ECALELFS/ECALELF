///
/// Stand-alone to Normalize DATA/MC IC Coefficient along one ring skipping dead channels and TT EE
/// Input File Scale Map of a data sample (can be used also on MC) an this is saved in the same
/// file updating the old scale_map normalized by rings without skip dead channels

#include <vector>
#include <utility>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include "TH2F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TStyle.h"
#include <cmath>
#include "../interface/ntpleUtils.h"
#include "TApplication.h"

#include "FWCore/ParameterSet/interface/ProcessDesc.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

using namespace std;

/// Check if the crystal is near to a dead one
bool CheckxtalIC (TH2F* h_scale_EE, int ix, int iy, int ir);
/// Check if the crystal is near to a dead TT
bool CheckxtalTT (int ix, int iy, int ir, std::vector<std::pair<int, int> >& TT_centre );

int main( int argc, char **argv)
{

/// map for dead TT centre

	std::vector< std::pair<int, int> > TT_centre_EEP;
	std::vector< std::pair<int, int> > TT_centre_EEM;

	TT_centre_EEP.push_back(std::pair<int, int> (78, 78));
	TT_centre_EEP.push_back(std::pair<int, int> (83, 28));
	TT_centre_EEP.push_back(std::pair<int, int> (83, 23));
	TT_centre_EEM.push_back(std::pair<int, int> (53, 28));

// Set style options
	gROOT->Reset();
	gROOT->SetStyle("Plain");

	gStyle->SetPadTickX(1);
	gStyle->SetPadTickY(1);
	gStyle->SetOptTitle(0);
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

	/// Acquisition from cfg file

	if(argc != 2) {
		std::cerr << ">>>>> analysis.cpp::usage: " << argv[0] << " configFileName" << std::endl ;
		return 1;
	}


	std::string configFileName = argv[1];
	std::shared_ptr<edm::ParameterSet> parameterSet = edm::readConfig(configFileName);
	edm::ParameterSet Options = parameterSet -> getParameter<edm::ParameterSet>("Options");
//  parameterSet.reset();

	std::string infile1 = "NULL";
	if(Options.existsAs<std::string>("infile1"))
		infile1 = Options.getParameter<std::string>("infile1");

	std::string infile2 = "NULL";
	if(Options.existsAs<std::string>("infile2"))
		infile2 = Options.getParameter<std::string>("infile2");

	std::string infile3 = "NULL";
	if(Options.existsAs<std::string>("infile3"))
		infile3 = Options.getParameter<std::string>("infile3");

	int evalStat = 1;
	if(Options.existsAs<int>("evalStat"))
		evalStat = Options.getParameter<int>("evalStat");

	if ( infile1.empty()) {
		cout << " No input file specified !" << endl;
		return 1;
	}

	if ( evalStat && (infile2.empty() || infile3.empty() )) {
		cout << " No input files to evaluate statistical precision specified !" << endl;
		return 1;
	}

	std::string fileType = "NULL";
	if(Options.existsAs<std::string>("fileType"))
		fileType = Options.getParameter<std::string>("fileType");

	std::string dirName = "NULL";
	if(Options.existsAs<std::string>("dirName"))
		dirName = Options.getParameter<std::string>("dirName");

	cout << "Making Eta ring Normalization for: " << infile1 << endl;


	TApplication* theApp = new TApplication("Application", &argc, argv);

///  Input file with full statistic

	TFile *f = new TFile(infile1.c_str(), "UPDATE");
	TH2F* h_scale_EE[2], *hcmap_EE[2];

	h_scale_EE[1] = (TH2F*)f->Get("h_scale_EEP");
	h_scale_EE[0] = (TH2F*)f->Get("h_scale_EEM");

	hcmap_EE[1] = (TH2F*) h_scale_EE[1]->Clone("hcmap_EEP");
	hcmap_EE[0] = (TH2F*) h_scale_EE[0]->Clone("hcmap_EEM");

	hcmap_EE[0] -> Reset("ICEMS");
	hcmap_EE[1] -> Reset("ICEMS");
	hcmap_EE[0] -> ResetStats();
	hcmap_EE[1] -> ResetStats();

/// ring structure from eerings.dat
	TH2F *hrings[2];
	hrings[0] = (TH2F*)h_scale_EE[0]->Clone("hringsEEM");
	hrings[1] = (TH2F*)h_scale_EE[1]->Clone("hringsEEP");
	hrings[0] ->Reset("ICMES");
	hrings[1] ->Reset("ICMES");
	hrings[0] ->ResetStats();
	hrings[1] ->ResetStats();


	FILE *fRing;
	fRing = fopen("macros/eerings.dat", "r");
	int x, y, z, ir;
	while(fscanf(fRing, "(%d,%d,%d) %d \n", &x, &y, &z, &ir) != EOF ) {
		if(z > 0) hrings[1]->Fill(x, y, ir);
		if(z < 0) hrings[0]->Fill(x, y, ir);
	}

	std::vector<float> SumIC_Ring_EEP, SumIC_Ring_EEM, Sumxtal_Ring_EEP, Sumxtal_Ring_EEM;

	SumIC_Ring_EEP.assign(40, 0);
	SumIC_Ring_EEM.assign(40, 0);
	Sumxtal_Ring_EEP.assign(40, 0);
	Sumxtal_Ring_EEM.assign(40, 0);

/// Mean over phi corrected skipping dead channel

	for(int k = 0; k < 2 ; k++) {
		for( int ix = 0; ix < h_scale_EE[k]->GetNbinsX() + 1 ; ix++ ) {
			for(int iy = 0; iy < h_scale_EE[k]->GetNbinsY() + 1 ; iy++ ) {

				ir = int(hrings[k]->GetBinContent(ix, iy));

				bool isGood = CheckxtalIC(h_scale_EE[k], ix, iy, ir);
				bool isGoodTT;

				if(k == 0) isGoodTT = CheckxtalTT(ix, iy, ir, TT_centre_EEM);
				else isGoodTT = CheckxtalTT(ix, iy, ir, TT_centre_EEP);

				if(k != 0 && isGoodTT && isGood ) {
					SumIC_Ring_EEP.at(ir) = SumIC_Ring_EEP.at(ir) + h_scale_EE[k]->GetBinContent(ix, iy);
					Sumxtal_Ring_EEP.at(ir) = Sumxtal_Ring_EEP.at(ir) + 1.;
				}
				if(k == 0 && isGoodTT && isGood) {
					SumIC_Ring_EEM.at(ir) = SumIC_Ring_EEM.at(ir) + h_scale_EE[k]->GetBinContent(ix, iy);
					Sumxtal_Ring_EEM.at(ir) = Sumxtal_Ring_EEM.at(ir) + 1.;
				}
			}
		}
	}

	for(int k = 0; k < 2 ; k++) {
		for( int ix = 0; ix < h_scale_EE[k]->GetNbinsX() + 1 ; ix++ ) {
			for(int iy = 0; iy < h_scale_EE[k]->GetNbinsY() + 1 ; iy++ ) {

				ir = int(hrings[k]->GetBinContent(ix, iy));

				if(k != 0) {
					if(ir > 33) {
						hcmap_EE[k]->Fill(ix, iy, 0.);
						continue;
					}
					if(Sumxtal_Ring_EEP.at(ir) != 0 && SumIC_Ring_EEP.at(ir) != 0)
						hcmap_EE[k]->Fill(ix, iy, h_scale_EE[k]->GetBinContent(ix, iy) / (SumIC_Ring_EEP.at(ir) / Sumxtal_Ring_EEP.at(ir)));
				} else {
					if(ir > 33) {
						hcmap_EE[k]->Fill(ix, iy, 0.);
						continue;
					}
					if(Sumxtal_Ring_EEM.at(ir) != 0 && SumIC_Ring_EEM.at(ir) != 0)
						hcmap_EE[k]->Fill(ix, iy, h_scale_EE[k]->GetBinContent(ix, iy) / (SumIC_Ring_EEM.at(ir) / Sumxtal_Ring_EEM.at(ir)));
				}
			}
		}
	}

	TH2F* h_scale_EE_Even[2], *hcmap_EE_Even[2];
	TH2F* h_scale_EE_Odd[2], *hcmap_EE_Odd[2];

	h_scale_EE_Even[1] = new TH2F();
	h_scale_EE_Even[0] = new TH2F();

	h_scale_EE_Odd[1] = new TH2F();
	h_scale_EE_Odd[0] = new TH2F();

	hcmap_EE_Even[1] = new TH2F();
	hcmap_EE_Even[0] = new TH2F();

	hcmap_EE_Odd[1] = new TH2F();
	hcmap_EE_Odd[0] = new TH2F();

	TFile *f2 = new TFile();
	TFile *f3 = new TFile();

	/// if evalstat --> apply also to the other file Odd and Even
	if(evalStat) {

		f2 = new TFile(infile2.c_str(), "UPDATE");
		f3 = new TFile(infile3.c_str(), "UPDATE");

		h_scale_EE_Even[1] = (TH2F*)f2->Get("h_scale_EEP");
		h_scale_EE_Even[0] = (TH2F*)f2->Get("h_scale_EEM");

		h_scale_EE_Odd[1] = (TH2F*)f3->Get("h_scale_EEP");
		h_scale_EE_Odd[0] = (TH2F*)f3->Get("h_scale_EEM");

		hcmap_EE_Even[1] = (TH2F*) h_scale_EE_Even[1]->Clone("hcmap_EEP");
		hcmap_EE_Even[0] = (TH2F*) h_scale_EE_Even[0]->Clone("hcmap_EEM");

		hcmap_EE_Odd[1] = (TH2F*) h_scale_EE_Odd[1]->Clone("hcmap_EEP");
		hcmap_EE_Odd[0] = (TH2F*) h_scale_EE_Odd[0]->Clone("hcmap_EEM");

		hcmap_EE_Even[0] -> Reset("ICEMS");
		hcmap_EE_Even[1] -> Reset("ICEMS");
		hcmap_EE_Even[0] -> ResetStats();
		hcmap_EE_Even[1] -> ResetStats();

		hcmap_EE_Odd[0] -> Reset("ICEMS");
		hcmap_EE_Odd[1] -> Reset("ICEMS");
		hcmap_EE_Odd[0] -> ResetStats();
		hcmap_EE_Odd[1] -> ResetStats();

		std::vector<float> SumIC_Ring_EEP_Even, SumIC_Ring_EEM_Even, Sumxtal_Ring_EEP_Even, Sumxtal_Ring_EEM_Even;
		std::vector<float> SumIC_Ring_EEP_Odd, SumIC_Ring_EEM_Odd, Sumxtal_Ring_EEP_Odd, Sumxtal_Ring_EEM_Odd;

		SumIC_Ring_EEP_Even.assign(40, 0);
		SumIC_Ring_EEM_Even.assign(40, 0);
		Sumxtal_Ring_EEP_Even.assign(40, 0);
		Sumxtal_Ring_EEM_Even.assign(40, 0);
		SumIC_Ring_EEP_Odd.assign(40, 0);
		SumIC_Ring_EEM_Odd.assign(40, 0);
		Sumxtal_Ring_EEP_Odd.assign(40, 0);
		Sumxtal_Ring_EEM_Odd.assign(40, 0);

		/// Mean over phi corrected skipping dead channel: Even sample

		for(int k = 0; k < 2 ; k++) {
			for( int ix = 0; ix < h_scale_EE_Even[k]->GetNbinsX() + 1 ; ix++ ) {
				for(int iy = 0; iy < h_scale_EE_Even[k]->GetNbinsY() + 1 ; iy++ ) {

					ir = int(hrings[k]->GetBinContent(ix, iy));

					bool isGood = CheckxtalIC(h_scale_EE_Even[k], ix, iy, ir);
					bool isGoodTT;

					if(k == 0) isGoodTT = CheckxtalTT(ix, iy, ir, TT_centre_EEM);
					else isGoodTT = CheckxtalTT(ix, iy, ir, TT_centre_EEP);

					if(k != 0 && isGoodTT && isGood ) {
						SumIC_Ring_EEP_Even.at(ir) = SumIC_Ring_EEP_Even.at(ir) + h_scale_EE_Even[k]->GetBinContent(ix, iy);
						Sumxtal_Ring_EEP_Even.at(ir) = Sumxtal_Ring_EEP_Even.at(ir) + 1.;
					}
					if(k == 0 && isGoodTT && isGood) {
						SumIC_Ring_EEM_Even.at(ir) = SumIC_Ring_EEM_Even.at(ir) + h_scale_EE_Even[k]->GetBinContent(ix, iy);
						Sumxtal_Ring_EEM_Even.at(ir) = Sumxtal_Ring_EEM_Even.at(ir) + 1.;
					}
				}
			}
		}

		for(int k = 0; k < 2 ; k++) {
			for( int ix = 0; ix < h_scale_EE_Even[k]->GetNbinsX() + 1 ; ix++ ) {
				for(int iy = 0; iy < h_scale_EE_Even[k]->GetNbinsY() + 1 ; iy++ ) {

					ir = int(hrings[k]->GetBinContent(ix, iy));

					if(k != 0) {
						if(ir > 33) {
							hcmap_EE_Even[k]->Fill(ix, iy, 0.);
							continue;
						}
						if(Sumxtal_Ring_EEP_Even.at(ir) != 0 && SumIC_Ring_EEP_Even.at(ir) != 0)
							hcmap_EE_Even[k]->Fill(ix, iy, h_scale_EE_Even[k]->GetBinContent(ix, iy) / (SumIC_Ring_EEP_Even.at(ir) / Sumxtal_Ring_EEP_Even.at(ir)));
					} else {
						if(ir > 33) {
							hcmap_EE_Even[k]->Fill(ix, iy, 0.);
							continue;
						}
						if(Sumxtal_Ring_EEM_Even.at(ir) != 0 && SumIC_Ring_EEM_Even.at(ir) != 0)
							hcmap_EE_Even[k]->Fill(ix, iy, h_scale_EE_Even[k]->GetBinContent(ix, iy) / (SumIC_Ring_EEM_Even.at(ir) / Sumxtal_Ring_EEM_Even.at(ir)));
					}
				}
			}
		}


		/// Mean over phi corrected skipping dead channel: Odd sample

		for(int k = 0; k < 2 ; k++) {
			for( int ix = 0; ix < h_scale_EE_Odd[k]->GetNbinsX() + 1 ; ix++ ) {
				for(int iy = 0; iy < h_scale_EE_Odd[k]->GetNbinsY() + 1 ; iy++ ) {

					ir = int(hrings[k]->GetBinContent(ix, iy));

					bool isGood = CheckxtalIC(h_scale_EE_Odd[k], ix, iy, ir);
					bool isGoodTT;

					if(k == 0) isGoodTT = CheckxtalTT(ix, iy, ir, TT_centre_EEM);
					else isGoodTT = CheckxtalTT(ix, iy, ir, TT_centre_EEP);

					if(k != 0 && isGoodTT && isGood ) {
						SumIC_Ring_EEP_Odd.at(ir) = SumIC_Ring_EEP_Odd.at(ir) + h_scale_EE_Odd[k]->GetBinContent(ix, iy);
						Sumxtal_Ring_EEP_Odd.at(ir) = Sumxtal_Ring_EEP_Odd.at(ir) + 1.;
					}
					if(k == 0 && isGoodTT && isGood) {
						SumIC_Ring_EEM_Odd.at(ir) = SumIC_Ring_EEM_Odd.at(ir) + h_scale_EE_Odd[k]->GetBinContent(ix, iy);
						Sumxtal_Ring_EEM_Odd.at(ir) = Sumxtal_Ring_EEM_Odd.at(ir) + 1.;
					}
				}
			}
		}

		for(int k = 0; k < 2 ; k++) {
			for( int ix = 0; ix < h_scale_EE_Odd[k]->GetNbinsX() + 1 ; ix++ ) {
				for(int iy = 0; iy < h_scale_EE_Odd[k]->GetNbinsY() + 1 ; iy++ ) {

					ir = int(hrings[k]->GetBinContent(ix, iy));

					if(k != 0) {
						if(ir > 33) {
							hcmap_EE_Odd[k]->Fill(ix, iy, 0.);
							continue;
						}
						if(Sumxtal_Ring_EEP_Odd.at(ir) != 0 && SumIC_Ring_EEP_Odd.at(ir) != 0)
							hcmap_EE_Odd[k]->Fill(ix, iy, h_scale_EE_Odd[k]->GetBinContent(ix, iy) / (SumIC_Ring_EEP_Odd.at(ir) / Sumxtal_Ring_EEP_Odd.at(ir)));
					} else {
						if(ir > 33) {
							hcmap_EE_Odd[k]->Fill(ix, iy, 0.);
							continue;
						}
						if(Sumxtal_Ring_EEM_Odd.at(ir) != 0 && SumIC_Ring_EEM_Odd.at(ir) != 0)
							hcmap_EE_Odd[k]->Fill(ix, iy, h_scale_EE_Odd[k]->GetBinContent(ix, iy) / (SumIC_Ring_EEM_Odd.at(ir) / Sumxtal_Ring_EEM_Odd.at(ir)));
					}
				}
			}
		}
	}

	TCanvas *cEEP[12];
	TCanvas *cEEM[12];

/// --- plot 0 : map of coefficients
	cEEP[0] = new TCanvas("cEEP", "cmapEEP not normalized");
	cEEP[0] -> cd();
	cEEP[0]->SetLeftMargin(0.1);
	cEEP[0]->SetRightMargin(0.13);
	cEEP[0]->SetGridx();
	cEEP[0]->SetGridy();
	h_scale_EE[1]->GetXaxis() -> SetLabelSize(0.03);
	h_scale_EE[1]->Draw("COLZ");
	h_scale_EE[1]->GetXaxis() ->SetTitle("ix");
	h_scale_EE[1]->GetYaxis() ->SetTitle("iy");
	h_scale_EE[1]->GetZaxis() ->SetRangeUser(0.5, 2.);

	cEEM[0] = new TCanvas("cEEM", "cmapEEM not normalized");
	cEEM[0] -> cd();
	cEEM[0]->SetLeftMargin(0.1);
	cEEM[0]->SetRightMargin(0.13);
	cEEM[0]->SetGridx();
	cEEM[0]->SetGridy();

	h_scale_EE[0]->GetXaxis() -> SetLabelSize(0.03);
	h_scale_EE[0]->Draw("COLZ");
	h_scale_EE[0]->GetXaxis() ->SetTitle("ix");
	h_scale_EE[0]->GetYaxis() ->SetTitle("iy");
	h_scale_EE[0]->GetZaxis() ->SetRangeUser(0.5, 2.);

	cEEP[1] = new TCanvas("cmapEEP Normalized", "cmapEEP Normalized");
	cEEP[1] -> cd();
	cEEP[1]->SetLeftMargin(0.1);
	cEEP[1]->SetRightMargin(0.13);
	cEEP[1]->SetGridx();
	cEEP[1]->SetGridy();

	hcmap_EE[1]->GetXaxis() -> SetLabelSize(0.03);
	hcmap_EE[1]->Draw("COLZ");
	hcmap_EE[1]->GetXaxis() ->SetTitle("ix");
	hcmap_EE[1]->GetYaxis() ->SetTitle("iy");
	hcmap_EE[1]->GetZaxis() ->SetRangeUser(0.8, 1.2);

	cEEM[1] = new TCanvas("cmapEEM Normalized", "cmapEEM Normalized");
	cEEM[1] -> cd();
	cEEM[1]->SetLeftMargin(0.1);
	cEEM[1]->SetRightMargin(0.13);
	cEEM[1]->SetGridx();
	cEEM[1]->SetGridy();
	hcmap_EE[0]->GetXaxis() -> SetLabelSize(0.03);
	hcmap_EE[0]->Draw("COLZ");
	hcmap_EE[0]->GetXaxis() ->SetTitle("ix");
	hcmap_EE[0]->GetYaxis() ->SetTitle("iy");
	hcmap_EE[0]->GetZaxis() ->SetRangeUser(0.8, 1.2);


	f->cd();
	hcmap_EE[0]->Write("h_scale_map_EEP");
	hcmap_EE[1]->Write("h_scale_map_EEP");

	if(evalStat) {
/// --- plot 0 : map of coefficients
		cEEP[2] = new TCanvas("cEEP_Even", "cmapEEP not normalized Even");
		cEEP[2] -> cd();
		cEEP[2]->SetLeftMargin(0.1);
		cEEP[2]->SetRightMargin(0.13);
		cEEP[2]->SetGridx();
		cEEP[2]->SetGridy();
		h_scale_EE_Even[1]->GetXaxis() -> SetLabelSize(0.03);
		h_scale_EE_Even[1]->Draw("COLZ");
		h_scale_EE_Even[1]->GetXaxis() ->SetTitle("ix");
		h_scale_EE_Even[1]->GetYaxis() ->SetTitle("iy");
		h_scale_EE_Even[1]->GetZaxis() ->SetRangeUser(0.5, 2.);

		cEEM[2] = new TCanvas("cEEM_Even", "cmapEEM not normalized Even");
		cEEM[2] -> cd();
		cEEM[2]->SetLeftMargin(0.1);
		cEEM[2]->SetRightMargin(0.13);
		cEEM[2]->SetGridx();
		cEEM[2]->SetGridy();

		h_scale_EE_Even[0]->GetXaxis() -> SetLabelSize(0.03);
		h_scale_EE_Even[0]->Draw("COLZ");
		h_scale_EE_Even[0]->GetXaxis() ->SetTitle("ix");
		h_scale_EE_Even[0]->GetYaxis() ->SetTitle("iy");
		h_scale_EE_Even[0]->GetZaxis() ->SetRangeUser(0.5, 2.);

		cEEP[3] = new TCanvas("cmapEEP Normalized Even", "cmapEEP Normalized Even");
		cEEP[3] -> cd();
		cEEP[3]->SetLeftMargin(0.1);
		cEEP[3]->SetRightMargin(0.13);
		cEEP[3]->SetGridx();
		cEEP[3]->SetGridy();

		hcmap_EE_Even[1]->GetXaxis() -> SetLabelSize(0.03);
		hcmap_EE_Even[1]->Draw("COLZ");
		hcmap_EE_Even[1]->GetXaxis() ->SetTitle("ix");
		hcmap_EE_Even[1]->GetYaxis() ->SetTitle("iy");
		hcmap_EE_Even[1]->GetZaxis() ->SetRangeUser(0.8, 1.2);

		cEEM[3] = new TCanvas("cmapEEM Normalized Even", "cmapEEM Normalized Even");
		cEEM[3] -> cd();
		cEEM[3]->SetLeftMargin(0.1);
		cEEM[3]->SetRightMargin(0.13);
		cEEM[3]->SetGridx();
		cEEM[3]->SetGridy();
		hcmap_EE_Even[0]->GetXaxis() -> SetLabelSize(0.03);
		hcmap_EE_Even[0]->Draw("COLZ");
		hcmap_EE_Even[0]->GetXaxis() ->SetTitle("ix");
		hcmap_EE_Even[0]->GetYaxis() ->SetTitle("iy");
		hcmap_EE_Even[0]->GetZaxis() ->SetRangeUser(0.8, 1.2);


		f2->cd();
		hcmap_EE_Even[0]->Write("h_scale_map_EEP");
		hcmap_EE_Even[1]->Write("h_scale_map_EEP");

		/// --- plot 0 : map of coefficients
		cEEP[4] = new TCanvas("cEEP_Odd", "cmapEEP not normalized Odd");
		cEEP[4] -> cd();
		cEEP[4]->SetLeftMargin(0.1);
		cEEP[4]->SetRightMargin(0.13);
		cEEP[4]->SetGridx();
		cEEP[4]->SetGridy();
		h_scale_EE_Odd[1]->GetXaxis() -> SetLabelSize(0.03);
		h_scale_EE_Odd[1]->Draw("COLZ");
		h_scale_EE_Odd[1]->GetXaxis() ->SetTitle("ix");
		h_scale_EE_Odd[1]->GetYaxis() ->SetTitle("iy");
		h_scale_EE_Odd[1]->GetZaxis() ->SetRangeUser(0.5, 2.);

		cEEM[4] = new TCanvas("cEEM_Odd", "cmapEEM not normalized Odd");
		cEEM[4] -> cd();
		cEEM[4]->SetLeftMargin(0.1);
		cEEM[4]->SetRightMargin(0.13);
		cEEM[4]->SetGridx();
		cEEM[4]->SetGridy();

		h_scale_EE_Odd[0]->GetXaxis() -> SetLabelSize(0.03);
		h_scale_EE_Odd[0]->Draw("COLZ");
		h_scale_EE_Odd[0]->GetXaxis() ->SetTitle("ix");
		h_scale_EE_Odd[0]->GetYaxis() ->SetTitle("iy");
		h_scale_EE_Odd[0]->GetZaxis() ->SetRangeUser(0.5, 2.);

		cEEP[5] = new TCanvas("cmapEEP Normalized Odd", "cmapEEP Normalized Odd");
		cEEP[5] -> cd();
		cEEP[5]->SetLeftMargin(0.1);
		cEEP[5]->SetRightMargin(0.13);
		cEEP[5]->SetGridx();
		cEEP[5]->SetGridy();

		hcmap_EE_Odd[1]->GetXaxis() -> SetLabelSize(0.03);
		hcmap_EE_Odd[1]->Draw("COLZ");
		hcmap_EE_Odd[1]->GetXaxis() ->SetTitle("ix");
		hcmap_EE_Odd[1]->GetYaxis() ->SetTitle("iy");
		hcmap_EE_Odd[1]->GetZaxis() ->SetRangeUser(0.8, 1.2);

		cEEM[5] = new TCanvas("cmapEEM Normalized Odd", "cmapEEM Normalized Odd");
		cEEM[5] -> cd();
		cEEM[5]->SetLeftMargin(0.1);
		cEEM[5]->SetRightMargin(0.13);
		cEEM[5]->SetGridx();
		cEEM[5]->SetGridy();
		hcmap_EE_Odd[0]->GetXaxis() -> SetLabelSize(0.03);
		hcmap_EE_Odd[0]->Draw("COLZ");
		hcmap_EE_Odd[0]->GetXaxis() ->SetTitle("ix");
		hcmap_EE_Odd[0]->GetYaxis() ->SetTitle("iy");
		hcmap_EE_Odd[0]->GetZaxis() ->SetRangeUser(0.8, 1.2);

		f3->cd();
		hcmap_EE_Odd[0]->Write("h_scale_map_EEP");
		hcmap_EE_Odd[1]->Write("h_scale_map_EEP");
	}

	theApp->Run();
	return 0;

}

//////////////////////////////////////////////////////////////

bool CheckxtalIC (TH2F* h_scale_EE, int ix, int iy, int ir)
{
	if(h_scale_EE->GetBinContent(ix, iy) == 0) return false;

	int bx = h_scale_EE->GetNbinsX();
	int by = h_scale_EE->GetNbinsY();

	if((ix < bx && h_scale_EE->GetBinContent(ix + 1, iy) == 0 && (ir != 0 || ir < 33)) || (h_scale_EE->GetBinContent(ix - 1, iy) == 0 && ix > 1 && (ir != 0 || ir < 33))) return false;

	if((iy < by && h_scale_EE->GetBinContent(ix, iy + 1) == 0 && (ir != 0 || ir < 33)) || (h_scale_EE->GetBinContent(ix, iy - 1) == 0 && iy > 1 && (ir != 0 || ir < 33))) return false;

	if((ix < bx && h_scale_EE->GetBinContent(ix + 1, iy + 1) == 0 && iy < by && (ir != 0 || ir < 33)) || ( h_scale_EE->GetBinContent(ix - 1, iy - 1) == 0 && iy > 1 && ix > 1 && (ir != 0 || ir < 33))) return false;

	if((h_scale_EE->GetBinContent(ix + 1, iy - 1) == 0 && iy > 1 && ix < bx && (ir != 0 || ir < 33)) || ( h_scale_EE->GetBinContent(ix - 1, iy + 1) == 0 && ix > 1 && iy < by && (ir != 0 || ir < 33))) return false;

	return true;

}

////////////////////////////////////////////////////////////////

bool CheckxtalTT (int ix, int iy, int ir, std::vector<std::pair<int, int> >& TT_centre )
{
	for( unsigned int k = 0; k < TT_centre.size(); k++) {
		if(fabs(ix - TT_centre.at(k).first) < 5 && fabs(iy - TT_centre.at(k).second) < 5) return false;

	}
	return true;
}
