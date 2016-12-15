/// Stand-alone program for compare calibration result obtained on MC with
/// the original configuration for EB Input: IC 2D map after L3 calib

#include <iostream>
#include "TH2F.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TApplication.h"

#include "../interface/ntpleUtils.h"

#include "FWCore/ParameterSet/interface/ProcessDesc.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

using namespace std;


int main(int argc, char**argv)
{

// Set style options
	gROOT->Reset();
	gROOT->SetStyle("Plain");

	gStyle->SetPadTickX(1);
	gStyle->SetPadTickY(1);
	gStyle->SetOptTitle(0);
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
	gROOT->ForceStyle();

/// Input File MCTruth IC Map , RECO IC map, MC IC set after calibration (Usually set with miscalibration 5%)
/// and StatPrecision IC coefficient obtained from CalibrationBarrel.cpp

	if(argc != 2) {
		std::cerr << ">>>>> analysis.cpp::usage: " << argv[0] << " configFileName" << std::endl ;
		return 1;
	}

	std::string configFileName = argv[1];
	std::shared_ptr<edm::ParameterSet> parameterSet = edm::readConfig(configFileName);
	edm::ParameterSet Options = parameterSet -> getParameter<edm::ParameterSet>("Options");

	std::string inputFile = "NULL";
	if(Options.existsAs<std::string>("inputFile"))
		inputFile = Options.getParameter<std::string>("inputFile");

	std::string fileMCTruth = "NULL";
	if(Options.existsAs<std::string>("fileMCTruth"))
		fileMCTruth = Options.getParameter<std::string>("fileMCTruth");

	std::string fileMCRecoIC = "NULL";
	if(Options.existsAs<std::string>("fileMCRecoIC"))
		fileMCRecoIC = Options.getParameter<std::string>("fileMCRecoIC");

	std::string fileStatPrecision = "NULL";
	if(Options.existsAs<std::string>("fileStatPrecision"))
		fileStatPrecision = Options.getParameter<std::string>("fileStatPrecision");

	TApplication* theApp = new TApplication("Application", &argc, argv);


	TFile *f1 = TFile::Open(fileMCTruth.c_str());
	TFile *f2 = TFile::Open(fileMCRecoIC.c_str());
	TFile *f3 = TFile::Open(inputFile.c_str());
	TFile *f4 = TFile::Open(fileStatPrecision.c_str());

	if( f1 == 0 || f2 == 0 || f3 == 0 || f4 == 0) return -1;

	TH2F *hcmapMcT = (TH2F*)f1->Get("h_scale_map");
	TH2F *hcmapMcR = (TH2F*)f2->Get("h_scale_map");

	TH2F * hcmap1 = (TH2F*)hcmapMcT->Clone("hcmap1");
	hcmap1->Reset();
	TH1F * hringdiff = new TH1F("hringdiff", "difference of ring average", 100, -0.1, 0.1);

	for (int jbin = 1; jbin < hcmap1-> GetNbinsY(); jbin++) {
		for (int ibin = 1; ibin < hcmap1-> GetNbinsX() + 1; ibin++) {
			hcmap1->SetBinContent(ibin, jbin, hcmapMcT->GetBinContent(ibin, jbin) / hcmapMcR->GetBinContent(ibin, jbin));
		}
	}


	TH2F * miscalib_map = (TH2F*) f3 -> Get("h_scalib_EB");
	TH2F *hcL3 = (TH2F*)f3->Get("h_scale_map");

	TH2F *hcmap2 = (TH2F*)hcL3 ->Clone("hcmap2");
	hcmap2->Reset();

	for (int jbin = 1; jbin < hcmap2-> GetNbinsY() + 1; jbin++) {
		for (int ibin = 1; ibin < hcmap2-> GetNbinsX() + 1; ibin++) {
			hcmap2->SetBinContent(ibin, jbin, miscalib_map->GetBinContent(ibin, jbin)*hcL3->GetBinContent(ibin, jbin));
		}
	}

/// IC Histogramm in Eta ring and 2D map difference
	TH2F * h2 = new TH2F("h2", "h2", 400, 0.5, 1.5, 400, 0.5, 1.5);
	TH2F * h2diff = (TH2F*)hcmap1->Clone("h2diff");
	h2diff->Reset();
	TH1F *hdiff = new TH1F("hdiff", "hdiff", 400, -0.5, 0.5);

	char hname[100];

	TH1F *hspread[172];
	for (int jbin = 1; jbin < hcmap1-> GetNbinsY() + 1; jbin++) {
		int etaring = hcmap1-> GetYaxis()->GetBinCenter(jbin);
		sprintf(hname, "hspread_ring_ieta%02d", etaring);
		hspread[jbin - 1] = new TH1F(hname, hname, 400, -0.5, 0.5);
	}

	for (int jbin = 1; jbin < hcmap1-> GetNbinsY() + 1; jbin++) {
		float etaring = hcmap1-> GetYaxis()->GetBinCenter(jbin);
		for (int ibin = 1; ibin < hcmap1-> GetNbinsX() + 1; ibin++) {
			float c1 = hcmap1->GetBinContent(ibin, jbin);
			float c2 = hcmap2->GetBinContent(ibin, jbin);
			if (c1 != 0 && c2 != 0 ) {
				hspread[jbin - 1]->Fill( c1 - c2 );
				h2->Fill(c1, c2);
				h2diff->SetBinContent(ibin, jbin, c1 - c2);
				if (fabs(etaring) < 40) hdiff->Fill(c1 - c2);

			}
		}
	}

/// Final Plot in eta ring (stat prescision and scale)
	TGraphErrors *sigma_vs_ieta = new TGraphErrors();
	sigma_vs_ieta->SetMarkerStyle(20);
	sigma_vs_ieta->SetMarkerSize(1);
	sigma_vs_ieta->SetMarkerColor(kBlue + 2);

	TGraphErrors *rms_vs_ieta = new TGraphErrors();
	rms_vs_ieta->SetMarkerStyle(24);
	rms_vs_ieta->SetMarkerSize(1);
	rms_vs_ieta->SetMarkerColor(kBlue + 2);

	TGraphErrors *scale_vs_ieta = new TGraphErrors();
	scale_vs_ieta->SetMarkerStyle(20);
	scale_vs_ieta->SetMarkerSize(1);
	scale_vs_ieta->SetMarkerColor(kBlue + 2);

/// Gaussian Fit of spread coefficient dstribution
	TF1 *fgaus = new TF1("fgaus", "gaus", -1, 1);
	int np = 0;
	for (int i = 1; i < hcmap1-> GetNbinsY() + 1; i++) {
		float etaring = hcmap1-> GetYaxis()->GetBinCenter(i);
		if (etaring == 0.5) continue;
		if ( hspread[i - 1]->GetEntries() == 0) {
			sigma_vs_ieta-> SetPoint(np, etaring, -100);
			np++;
			continue;
		}
		hspread[i - 1]->Fit("fgaus", "Q");
		sigma_vs_ieta-> SetPoint(np, etaring, fgaus->GetParameter(2));
		sigma_vs_ieta-> SetPointError(np, 0, fgaus->GetParError(2));
		rms_vs_ieta  -> SetPoint(np, etaring, hspread[i - 1]->GetRMS());
		rms_vs_ieta  -> SetPointError(np, 0, hspread[i - 1]->GetRMSError() );
		scale_vs_ieta-> SetPoint(np, etaring, fgaus->GetParameter(1));
		scale_vs_ieta-> SetPointError(np, 0, fgaus->GetParError(1));
		if( fabs(etaring) < 20 ) {
			hringdiff->Fill( fgaus->GetParameter(1) );
		}
		np++;
	}

	/// Final Plot
	TGraphErrors* gr_stat_prec = (TGraphErrors*) f4->Get("gr_stat_prec");
	TCanvas *csigma = new TCanvas("csigma", "csigma");
	csigma->SetGridx();
	csigma->SetGridy();
	sigma_vs_ieta->GetHistogram()->GetYaxis()-> SetRangeUser(0.00, 0.2);
	sigma_vs_ieta->GetHistogram()->GetXaxis()-> SetRangeUser(-85, 85);
	sigma_vs_ieta->GetHistogram()->GetYaxis()-> SetTitle("#sigma");
	sigma_vs_ieta->GetHistogram()->GetXaxis()-> SetTitle("ieta");
	sigma_vs_ieta->Draw("ap");
	rms_vs_ieta->Draw("psame");
	gr_stat_prec->Draw("psame");


/// Residual Plot (spread - statistical precision)
	TGraphErrors* residual = new TGraphErrors();

	for(int pp = 0; pp < gr_stat_prec->GetN(); pp++) {
		double eta1, eta2, tot, stat;
		gr_stat_prec->GetPoint(pp, eta1, stat);
		sigma_vs_ieta->GetPoint(pp, eta2, tot);
		if(eta1 != eta2) {
			cout << "error different ring " << eta1 << "  " << eta2 << endl;
		}
		double res = tot * tot - stat * stat;
		if (res > 0) res = sqrt(res);
		else res = -sqrt(fabs(res));
		double errres = sqrt( pow(tot * sigma_vs_ieta->GetErrorY(pp), 2) + pow(stat * gr_stat_prec->GetErrorY(pp), 2)) / fabs(res);
		residual->SetPoint(pp, eta1, res);
		residual->SetPointError(pp, 0, errres);
	}

/// Residual spread plot
	TCanvas *cres = new TCanvas("cres", "cresidual");
	cres->SetGridx();
	cres->SetGridy();
	residual->GetHistogram()->GetYaxis()-> SetRangeUser(-0.1, 0.1);
	residual->GetHistogram()->GetXaxis()-> SetRangeUser(-85, 85);
	residual->GetHistogram()->GetYaxis()-> SetTitle("residual");
	residual->GetHistogram()->GetXaxis()-> SetTitle("ieta");
	residual ->SetMarkerStyle(20);
	residual->SetMarkerSize(1);
	residual->SetMarkerColor(kGreen + 2);
	residual->GetYaxis()->SetTitle("residual");
	residual->GetXaxis()->SetTitle("i#eta");
	residual->Draw("ap");

/// scale vs eta plot
	TCanvas *cscale = new TCanvas("cscale", "cscale");
	cscale->SetGridx();
	cscale->SetGridy();
	scale_vs_ieta->GetHistogram()->GetYaxis()-> SetRangeUser(-0.1, 0.1);
	scale_vs_ieta->GetHistogram()->GetXaxis()-> SetRangeUser(-85, 85);
	scale_vs_ieta->GetHistogram()->GetYaxis()-> SetTitle("c_{1}-c_{2}");
	scale_vs_ieta->GetHistogram()->GetXaxis()-> SetTitle("ieta");
	scale_vs_ieta->Draw("ap");

/// IC Diff map
	TCanvas *cmap2 = new TCanvas("cmap2", "cmap2", 500, 500);
	cmap2->SetGridx();
	cmap2->SetGridy();
	cmap2 -> cd();
	cmap2->SetLeftMargin(0.1);
	cmap2->SetRightMargin(0.15);
	h2->GetXaxis()->SetRangeUser(0.85, 1.15);
	h2->GetYaxis()->SetRangeUser(0.85, 1.15);
	h2->GetXaxis()->SetTitle("C_{1}");
	h2->GetYaxis()->SetTitle("C_{2}");
	h2->Draw("colz");

	TCanvas *cdiff = new TCanvas("cdiff", "cdiff", 700, 500);
	cdiff->SetGridx();
	cdiff->SetGridy();
	cdiff -> cd();
	cdiff->SetLeftMargin(0.1);
	cdiff->SetRightMargin(0.15);
	h2diff->GetZaxis()->SetRangeUser(-0.05, 0.05);
	h2diff->GetXaxis()->SetTitle("i#phi");
	h2diff->GetYaxis()->SetTitle("i#eta");
	h2diff->Draw("colz");

	theApp->Run();

	return 0;
}
