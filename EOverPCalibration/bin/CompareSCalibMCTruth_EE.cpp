/// Stand-alone program for compare calibration result obtained on MC with
/// the original configuration for EE Input: IC 2D map after L3 calib

#include <iostream>
#include <vector>
#include <string>
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
#include "../interface/TEndcapRings.h"

#include "FWCore/ParameterSet/interface/ProcessDesc.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

using namespace std;

int main (int argc, char **argv)
{

/// Input File MCTruth IC Map , RECO IC map, MC IC set after calibration (Usually set with miscalibration 5%)
/// and StatPrecision IC coefficient obtained from DrawCalibrationPlotsEE.C

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

	std::string inputMomentumScale = "NULL";
	if(Options.existsAs<std::string>("inputMomentumScale"))
		inputMomentumScale = Options.getParameter<std::string>("inputMomentumScale");

	std::string fileMCTruth = "NULL";
	if(Options.existsAs<std::string>("fileMCTruth"))
		fileMCTruth = Options.getParameter<std::string>("fileMCTruth");

	std::string fileMCRecoIC = "NULL";
	if(Options.existsAs<std::string>("fileMCRecoIC"))
		fileMCRecoIC = Options.getParameter<std::string>("fileMCRecoIC");

	std::string fileStatPrecision = "NULL";
	if(Options.existsAs<std::string>("fileStatPrecision"))
		fileStatPrecision = Options.getParameter<std::string>("fileStatPrecision");

	std::string outputFile = "NULL";
	if(Options.existsAs<std::string>("outputFile"))
		outputFile = Options.getParameter<std::string>("outputFile");

	TApplication* theApp = new TApplication("Application", &argc, argv);

	TFile *f1 = TFile::Open(fileMCTruth.c_str());
	TFile *f2 = TFile::Open(fileMCRecoIC.c_str());
/// L3 result MC endcap
	TFile *f3 = TFile::Open(inputFile.c_str());
/// File form momentum scale correction
	TFile* input = new TFile(inputMomentumScale.c_str());
/// Statistical Precision map
	TFile *f4 =  TFile::Open(fileStatPrecision.c_str());

	if(f1 == 0 || f2 == 0 || f3 == 0 || f4 == 0) return -1;


// Set style options
	gROOT->Reset();
	gROOT->SetStyle("Plain");

	gStyle->SetPadTickX(1);
	gStyle->SetPadTickY(1);
	gStyle->SetOptTitle(0);
	gStyle->SetOptStat(1110);
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


/// input coeff map for EEP and EEM

	TH2F *hcmapMcT_EEP = (TH2F*)f1->Get("h_scale_EEP");
	TH2F *hcmapMcT_EEM = (TH2F*)f1->Get("h_scale_EEM");
	TH2F *hcmapMcR_EEP = (TH2F*)f2->Get("h_scale_EEP");
	TH2F *hcmapMcR_EEM = (TH2F*)f2->Get("h_scale_EEM");

	TH2F * hcmap_EEP = (TH2F*)hcmapMcT_EEP->Clone("hcmap_EEP");
	TH2F * hcmap_EEM = (TH2F*)hcmapMcT_EEM->Clone("hcmap_EEM");

	hcmap_EEP->Reset();
	hcmap_EEM->Reset();

	for (int jbin = 1; jbin < hcmap_EEP-> GetNbinsY(); jbin++) {
		for (int ibin = 1; ibin < hcmap_EEM-> GetNbinsX() + 1; ibin++) {

			if(hcmapMcT_EEP->GetBinContent(ibin, jbin) != 0 && hcmapMcR_EEP->GetBinContent(ibin, jbin) != 0)
				hcmap_EEP->SetBinContent(ibin, jbin, hcmapMcT_EEP->GetBinContent(ibin, jbin) / hcmapMcR_EEP->GetBinContent(ibin, jbin));
			if(hcmapMcT_EEM->GetBinContent(ibin, jbin) != 0 && hcmapMcR_EEM->GetBinContent(ibin, jbin) != 0)
				hcmap_EEM->SetBinContent(ibin, jbin, hcmapMcT_EEM->GetBinContent(ibin, jbin) / hcmapMcR_EEM->GetBinContent(ibin, jbin));
		}
	}

/// Scalib and L3 map Normalized for EE+ and EE-

	TH2F * miscalib_map_EEP = (TH2F*) f3 -> Get("h_scalib_EEP");
	TH2F * miscalib_map_EEM = (TH2F*) f3 -> Get("h_scalib_EEM");

	TH2F *hcL3_EEP = (TH2F*)f3->Get("h_scale_map_EEP");
	TH2F *hcL3_EEM = (TH2F*)f3->Get("h_scale_map_EEM");

	TH2F *hcmap2_EEP = (TH2F*)hcL3_EEP ->Clone("hcmap2_EEP");
	TH2F *hcmap2_EEM = (TH2F*)hcL3_EEM ->Clone("hcmap2_EEM");

	hcmap2_EEP->Reset();
	hcmap2_EEM->Reset();


/// Momentum scale correction

	TGraphErrors* g_EoP_EE = (TGraphErrors*) input->Get("g_EoP_EE_0");
	TGraphErrors* PhiProjectionEEp = new TGraphErrors();
	TGraphErrors* PhiProjectionEEm = new TGraphErrors();

	PhiProjectionEEp->SetMarkerStyle(20);
	PhiProjectionEEp->SetMarkerSize(1);
	PhiProjectionEEp->SetMarkerColor(kRed);

	PhiProjectionEEm->SetMarkerStyle(20);
	PhiProjectionEEm->SetMarkerSize(1);
	PhiProjectionEEm->SetMarkerColor(kBlue);


	TEndcapRings *eRings = new TEndcapRings();
	std::vector<double> vectSum;
	std::vector<double> vectCounter;

	vectCounter.assign(g_EoP_EE->GetN(), 0.);
	vectSum.assign(g_EoP_EE->GetN(), 0.);

/// EE+ and EE- projection

	for(int ix = 1; ix < hcL3_EEM->GetNbinsX() + 1; ix++) {
		for(int iy = 1; iy < hcL3_EEM->GetNbinsY() + 1; iy++) {
			if(hcL3_EEM->GetBinContent(ix, iy) == 0) continue;
			int iPhi = int(eRings->GetEndcapIphi(ix, iy, -1) / (360. / g_EoP_EE->GetN()));
			vectSum.at(iPhi) = vectSum.at(iPhi) + hcL3_EEM->GetBinContent(ix, iy);
			vectCounter.at(iPhi) = vectCounter.at(iPhi) + 1;
		}
	}


	for(unsigned int i = 0; i < vectCounter.size(); i++)
		PhiProjectionEEm->SetPoint(i, int(i * (360. / g_EoP_EE->GetN())), vectSum.at(i) / vectCounter.at(i));

	for(unsigned int i = 0; i < vectSum.size(); i++) {
		vectSum.at(i) = 0;
		vectCounter.at(i) = 0;
	}

	for(int ix = 1; ix < hcL3_EEP->GetNbinsX() + 1; ix++) {
		for(int iy = 1; iy < hcL3_EEP->GetNbinsY() + 1; iy++) {
			if(hcL3_EEP->GetBinContent(ix, iy) == 0) continue;
			int iPhi = int(eRings->GetEndcapIphi(ix, iy, 1) / (360. / g_EoP_EE->GetN()));
			vectSum.at(iPhi) = vectSum.at(iPhi) + hcL3_EEP->GetBinContent(ix, iy);
			vectCounter.at(iPhi) = vectCounter.at(iPhi) + 1;
		}
	}


	for(unsigned int i = 0; i < vectCounter.size(); i++)
		PhiProjectionEEp->SetPoint(i, int(i * (360. / g_EoP_EE->GetN())), vectSum.at(i) / vectCounter.at(i));

	for(unsigned int i = 0; i < vectSum.size(); i++) {
		vectSum.at(i) = 0;
		vectCounter.at(i) = 0;
	}

/// Correction EE+ and EE-

	TH2F* mapMomentumCorrected[2];
	mapMomentumCorrected[0] = (TH2F*) hcL3_EEM->Clone("mapMomentumCorrected_EEM");
	mapMomentumCorrected[1] = (TH2F*) hcL3_EEP->Clone("mapMomentumCorrected_EEP");
	mapMomentumCorrected[0]->Reset();
	mapMomentumCorrected[1]->Reset();

	for(int ix = 1; ix < hcL3_EEM->GetNbinsX() + 1; ix++) {
		for(int iy = 1; iy < hcL3_EEM->GetNbinsY() + 1; iy++) {
			if(hcL3_EEM->GetBinContent(ix, iy) == 0) continue;
			int iPhi = int(eRings->GetEndcapIphi(ix, iy, -1));
			double xphi, yphi;
			g_EoP_EE->GetPoint(int(iPhi / (360. / PhiProjectionEEm->GetN())), xphi, yphi);
			mapMomentumCorrected[0]->SetBinContent(ix, iy, hcL3_EEM->GetBinContent(ix, iy)*yphi);
		}
	}

	for(int ix = 1; ix < hcL3_EEP->GetNbinsX() + 1; ix++) {
		for(int iy = 1; iy < hcL3_EEP->GetNbinsY() + 1; iy++) {
			if(hcL3_EEP->GetBinContent(ix, iy) == 0) continue;
			int iPhi = int(eRings->GetEndcapIphi(ix, iy, 1));
			double xphi, yphi;
			g_EoP_EE->GetPoint(int(iPhi / (360. / PhiProjectionEEp->GetN())), xphi, yphi);
			mapMomentumCorrected[1]->SetBinContent(ix, iy, hcL3_EEP->GetBinContent(ix, iy)*yphi);
		}
	}

/// Use momentum map corrected

	for (int jbin = 1; jbin < mapMomentumCorrected[1]-> GetNbinsY() + 1; jbin++) {
		for (int ibin = 1; ibin < mapMomentumCorrected[1]-> GetNbinsX() + 1; ibin++) {
			if(miscalib_map_EEP->GetBinContent(ibin, jbin) != 0 && mapMomentumCorrected[1]->GetBinContent(ibin, jbin) != 0)
				hcmap2_EEP->SetBinContent(ibin, jbin, miscalib_map_EEP->GetBinContent(ibin, jbin)*mapMomentumCorrected[1]->GetBinContent(ibin, jbin));
			if(miscalib_map_EEM->GetBinContent(ibin, jbin) != 0 && mapMomentumCorrected[0]->GetBinContent(ibin, jbin) != 0)
				hcmap2_EEM->SetBinContent(ibin, jbin, miscalib_map_EEM->GetBinContent(ibin, jbin)*mapMomentumCorrected[0]->GetBinContent(ibin, jbin));
		}
	}


/// output histos

	TH2F * h2_EEP = new TH2F("h2_EEP", "h2_EEP", 400, 0.5, 1.5, 400, 0.5, 1.5);
	TH2F * h2_EEM = new TH2F("h2_EEM", "h2_EEM", 400, 0.5, 1.5, 400, 0.5, 1.5);

	TH2F * h2diff_EEP = (TH2F*)hcmap_EEP->Clone("h2diff_EEP");
	TH2F * h2diff_EEM = (TH2F*)hcmap_EEM->Clone("h2diff_EEM");

	h2diff_EEP->Reset();
	h2diff_EEM->Reset();

	char hname[100];

	TH1F *hspread_EEP[40];
	TH1F *hspread_EEM[40];
	TH1F *hspread_All[40];

/// ring geometry for the endcap

	TH2F *hrings_EEP;
	TH2F *hrings_EEM;

	hrings_EEP = (TH2F*)hcmap2_EEP->Clone("hringsEEP");
	hrings_EEM = (TH2F*)hcmap2_EEM->Clone("hringsEEM");
	hrings_EEP ->Reset();
	hrings_EEM ->Reset();

	FILE *fRing;
	fRing = fopen("macros/eerings.dat", "r");
	int x, y, z, ir;
	while(fscanf(fRing, "(%d,%d,%d) %d \n", &x, &y, &z, &ir) != EOF ) {
		if(z > 0) hrings_EEP->Fill(x, y, ir);
		if(z < 0) hrings_EEM->Fill(x, y, ir);
	}

/// spread IC histos
	for (int jbin = 0; jbin < 40; jbin++) {
		sprintf(hname, "hspread_ring_EEP_%02d", jbin);
		hspread_EEP[jbin] = new TH1F(hname, hname, 75, 0.5, 1.5);
		sprintf(hname, "hspread_ring_EEM_%02d", jbin);
		hspread_EEM[jbin] = new TH1F(hname, hname, 75, 0.5, 1.5);
		sprintf(hname, "hspread_ring_All_%02d", jbin);
		hspread_All[jbin] = new TH1F(hname, hname, 75, 0.5, 1.5);

	}

	for (int jbin = 1; jbin < hcmap_EEP-> GetNbinsY() + 1; jbin++) {
		for (int ibin = 1; ibin < hcmap_EEP-> GetNbinsX() + 1; ibin++) {

			int mybin_EEP = hcmap_EEP -> FindBin(ibin, jbin);
			int ring_EEP  = int(hrings_EEP-> GetBinContent(mybin_EEP));
			float c1_EEP = hcmap_EEP->GetBinContent(mybin_EEP);
			float c2_EEP = hcmap2_EEP->GetBinContent(mybin_EEP);

			int mybin_EEM = hcmap_EEM -> FindBin(ibin, jbin);
			int ring_EEM  = int(hrings_EEM-> GetBinContent(mybin_EEM));
			float c1_EEM = hcmap_EEM->GetBinContent(mybin_EEM);
			float c2_EEM = hcmap2_EEM->GetBinContent(mybin_EEM);

			if (c1_EEP != 0 && c2_EEP != 0 ) {
				///Ratio betweem IC and not the difference
				hspread_EEP[ring_EEP]->Fill(c1_EEP / c2_EEP);
				hspread_All[ring_EEP]->Fill(c1_EEP / c2_EEP);

				h2_EEP->Fill(c1_EEP, c2_EEP);
				h2diff_EEP->SetBinContent(ibin, jbin, c1_EEP / c2_EEP);
			} else h2diff_EEP->SetBinContent(ibin, jbin, 0);

			if (c1_EEM != 0 && c2_EEM != 0 ) {
				hspread_EEM[ring_EEM]->Fill(c1_EEM / c2_EEM);
				hspread_All[ring_EEM]->Fill(c1_EEM / c2_EEM);

				h2_EEM->Fill(c1_EEM, c2_EEM);
				h2diff_EEM->SetBinContent(ibin, jbin, c1_EEM / c2_EEM);
			} else h2diff_EEM->SetBinContent(ibin, jbin, 0);
		}
	}

/// TGraphErrors for final plot
	TGraphErrors *sigma_vs_iring_EEP = new TGraphErrors();
	sigma_vs_iring_EEP->SetMarkerStyle(20);
	sigma_vs_iring_EEP->SetMarkerSize(1);
	sigma_vs_iring_EEP->SetMarkerColor(kBlue + 2);

	TGraphErrors *sigma_vs_iring_EEM = new TGraphErrors();
	sigma_vs_iring_EEM->SetMarkerStyle(20);
	sigma_vs_iring_EEM->SetMarkerSize(1);
	sigma_vs_iring_EEM->SetMarkerColor(kBlue + 2);

	TGraphErrors *sigma_vs_iring_All = new TGraphErrors();
	sigma_vs_iring_All->SetMarkerStyle(20);
	sigma_vs_iring_All->SetMarkerSize(1);
	sigma_vs_iring_All->SetMarkerColor(kBlue + 2);

	TGraphErrors *scale_vs_iring_EEP = new TGraphErrors();
	scale_vs_iring_EEP->SetMarkerStyle(20);
	scale_vs_iring_EEP->SetMarkerSize(1);
	scale_vs_iring_EEP->SetMarkerColor(kBlue + 2);

	TGraphErrors *scale_vs_iring_EEM = new TGraphErrors();
	scale_vs_iring_EEM->SetMarkerStyle(20);
	scale_vs_iring_EEM->SetMarkerSize(1);
	scale_vs_iring_EEM->SetMarkerColor(kBlue + 2);

	TGraphErrors *scale_vs_iring_All = new TGraphErrors();
	scale_vs_iring_All->SetMarkerStyle(20);
	scale_vs_iring_All->SetMarkerSize(1);
	scale_vs_iring_All->SetMarkerColor(kBlue + 2);

/// Gauss function for fit
	TF1 *fgaus_EEP = new TF1("fgaus_EEP", "gaus", 0., 2.);
	TF1 *fgaus_EEM = new TF1("fgaus_EEM", "gaus", 0., 2.);
	TF1 *fgaus_All = new TF1("fgaus_All", "gaus", 0., 2.);

	int np_EEP = 0;
	int np_EEM = 0;
	int np_All = 0;
	for (int i = 0; i < 40; i++) {
		if ( hspread_EEP[i]->GetEntries() == 0) {
			sigma_vs_iring_EEP-> SetPoint(np_EEP, i, -100);
			np_EEP++;
			continue;
		}
		if ( hspread_EEM[i]->GetEntries() == 0) {
			sigma_vs_iring_EEM-> SetPoint(np_EEM, i, -100);
			np_EEM++;
			continue;
		}
		if ( hspread_All[i]->GetEntries() == 0) {
			sigma_vs_iring_All-> SetPoint(np_All, i, -100);
			np_All++;
			continue;
		}

		hspread_EEP[i]->Fit("fgaus_EEP", "QRME");
		hspread_EEM[i]->Fit("fgaus_EEM", "QRME");
		hspread_All[i]->Fit("fgaus_All", "QRME");
		TString Name = Form("c_%d", i);

		sigma_vs_iring_EEP-> SetPoint(np_EEP, i, fgaus_EEP->GetParameter(2));
		sigma_vs_iring_EEP-> SetPointError(np_EEP, 0, fgaus_EEP->GetParError(2));

		scale_vs_iring_EEP-> SetPoint(np_EEP, i, fgaus_EEP->GetParameter(1));
		scale_vs_iring_EEP-> SetPointError(np_EEP, 0, fgaus_EEP->GetParError(1));

		np_EEP++;

		sigma_vs_iring_EEM-> SetPoint(np_EEM, i, fgaus_EEM->GetParameter(2));
		sigma_vs_iring_EEM-> SetPointError(np_EEM, 0, fgaus_EEM->GetParError(2));

		scale_vs_iring_EEM-> SetPoint(np_EEM, i, fgaus_EEM->GetParameter(1));
		scale_vs_iring_EEM-> SetPointError(np_EEM, 0, fgaus_EEM->GetParError(1));

		np_EEM++;

		sigma_vs_iring_All-> SetPoint(np_All, i, fgaus_All->GetParameter(2));
		sigma_vs_iring_All-> SetPointError(np_All, 0, fgaus_All->GetParError(2));

		scale_vs_iring_All-> SetPoint(np_All, i, fgaus_All->GetParameter(1));
		scale_vs_iring_All-> SetPointError(np_All, 0, fgaus_All->GetParError(1));

		np_All++;
	}

/// For from statistical precision analysis

	TGraphErrors* gr_stat_prec_EEP = (TGraphErrors*) f4->Get("gr_stat_prec_EEP");
	TGraphErrors* gr_stat_prec_EEM = (TGraphErrors*) f4->Get("gr_stat_prec_EEM");
	TGraphErrors* gr_stat_prec_All = (TGraphErrors*) f4->Get("gr_stat_prec");

	/// Residual Plot
	TGraphErrors* residual_EEP = new TGraphErrors();
	TGraphErrors* residual_EEM = new TGraphErrors();
	TGraphErrors* residual_All = new TGraphErrors();

	for(int pp = 0; pp < gr_stat_prec_EEP->GetN(); pp++) {
		double ring1, ring2, tot, stat, espread, estat, ex, res, eres;

		sigma_vs_iring_EEP->GetPoint(pp, ring2, tot);
		espread = sigma_vs_iring_EEP-> GetErrorY(pp);

		gr_stat_prec_EEP->GetPoint(pp, ring1, stat);
		estat = gr_stat_prec_EEP-> GetErrorY(pp);

		ex = gr_stat_prec_EEP-> GetErrorX(pp);

		if (tot > stat ) {
			res = sqrt( tot * tot - stat * stat );
			eres = sqrt( pow(tot * espread, 2) + pow(stat * estat, 2)) / res;
		} else {
			res  = -sqrt( fabs(tot * tot - stat * stat) );
			eres = sqrt( pow(tot * espread, 2) + pow(stat * estat, 2)) / fabs(res);
		}

		residual_EEP->SetPoint(pp, ring1, res);
		if(eres < 0.01) residual_EEP->SetPointError(pp, ex, eres);
		else residual_EEP->SetPointError(pp, ex, 0.01);
	}

	for(int pp = 0; pp < gr_stat_prec_EEM->GetN(); pp++) {
		double ring1, ring2, tot, stat, espread, estat, ex, res, eres;

		sigma_vs_iring_EEM->GetPoint(pp, ring2, tot);
		espread = sigma_vs_iring_EEM-> GetErrorY(pp);

		gr_stat_prec_EEM->GetPoint(pp, ring1, stat);
		estat = gr_stat_prec_EEM-> GetErrorY(pp);

		ex = gr_stat_prec_EEM-> GetErrorX(pp);
		if (tot > stat ) {
			res  = sqrt( tot * tot - stat * stat );
			eres = sqrt( pow(tot * espread, 2) + pow(stat * estat, 2)) / res;
		} else {
			res  = -sqrt( fabs(tot * tot - stat * stat) );
			eres = sqrt( pow(tot * espread, 2) + pow(stat * estat, 2)) / fabs(res);
		}
		residual_EEM->SetPoint(pp, ring1, res);
		if(eres < 0.01) residual_EEM->SetPointError(pp, ex, eres);
		else residual_EEM->SetPointError(pp, ex, 0.01);
	}

	for(int pp = 0; pp < gr_stat_prec_All->GetN(); pp++) {
		double ring1, ring2, tot, stat, espread, estat, ex, res, eres;

		sigma_vs_iring_All->GetPoint(pp, ring2, tot);
		espread = sigma_vs_iring_All-> GetErrorY(pp);

		gr_stat_prec_All->GetPoint(pp, ring1, stat);
		estat = gr_stat_prec_All-> GetErrorY(pp);

		ex = gr_stat_prec_All-> GetErrorX(pp);
		if (tot > stat ) {
			res  = sqrt( tot * tot - stat * stat );
			eres = sqrt( pow(tot * espread, 2) + pow(stat * estat, 2)) / res;
		} else {
			res  = -sqrt( fabs(tot * tot - stat * stat) );
			eres = sqrt( pow(tot * espread, 2) + pow(stat * estat, 2)) / fabs(res);
		}

		residual_All->SetPoint(pp, ring1, res);
		if(eres < 0.01) residual_All->SetPointError(pp, ex, eres);
		else residual_All->SetPointError(pp, ex, 0.01);

	}

/// Final Plot
	TCanvas *cscale_EEP = new TCanvas("cscale_EEP", "cscale_EEP");
	cscale_EEP->SetGridx();
	cscale_EEP->SetGridy();
	scale_vs_iring_EEP->GetHistogram()->GetYaxis()-> SetRangeUser(0.9, 1.1);
	scale_vs_iring_EEP->GetHistogram()->GetXaxis()-> SetRangeUser(0, 40);
	scale_vs_iring_EEP->GetHistogram()->GetYaxis()-> SetTitle("c_{1}-c_{2}");
	scale_vs_iring_EEP->GetHistogram()->GetXaxis()-> SetTitle("iring");
	scale_vs_iring_EEP->Draw("ap");

	TCanvas *cscale_EEM = new TCanvas("cscale_EEM", "cscale_EEM");
	cscale_EEM->SetGridx();
	cscale_EEM->SetGridy();
	scale_vs_iring_EEM->GetHistogram()->GetYaxis()-> SetRangeUser(0.9, 1.1);
	scale_vs_iring_EEM->GetHistogram()->GetXaxis()-> SetRangeUser(0, 40);
	scale_vs_iring_EEM->GetHistogram()->GetYaxis()-> SetTitle("c_{1}-c_{2}");
	scale_vs_iring_EEM->GetHistogram()->GetXaxis()-> SetTitle("iring");
	scale_vs_iring_EEM->Draw("ap");

	TCanvas *cmap2_EEP = new TCanvas("cmap2_EEP", "cmap2_EEP", 500, 500);
	cmap2_EEP->SetGridx();
	cmap2_EEP->SetGridy();
	cmap2_EEP -> cd();
	cmap2_EEP->SetLeftMargin(0.1);
	cmap2_EEP->SetRightMargin(0.15);
	h2_EEP->GetXaxis()->SetRangeUser(0.85, 1.15);
	h2_EEP->GetYaxis()->SetRangeUser(0.85, 1.15);
	h2_EEP->GetXaxis()->SetTitle("C_{1}");
	h2_EEP->GetYaxis()->SetTitle("C_{2}");
	h2_EEP->Draw("colz");

	TCanvas *cmap2_EEM = new TCanvas("cmap2_EEM", "cmap2_EEM", 500, 500);
	cmap2_EEM->SetGridx();
	cmap2_EEM->SetGridy();
	cmap2_EEM -> cd();
	cmap2_EEM->SetLeftMargin(0.1);
	cmap2_EEM->SetRightMargin(0.15);
	h2_EEM->GetXaxis()->SetRangeUser(0.85, 1.15);
	h2_EEM->GetYaxis()->SetRangeUser(0.85, 1.15);
	h2_EEM->GetXaxis()->SetTitle("C_{1}");
	h2_EEM->GetYaxis()->SetTitle("C_{2}");
	h2_EEM->Draw("colz");

	TCanvas *cdiff_EEP = new TCanvas("cdiff_EEP", "cdiff_EEP", 700, 500);
	cdiff_EEP->SetGridx();
	cdiff_EEP->SetGridy();
	cdiff_EEP -> cd();
	cdiff_EEP->SetLeftMargin(0.1);
	cdiff_EEP->SetRightMargin(0.15);
	h2diff_EEP->GetZaxis()->SetRangeUser(0.5, 1.6);
	h2diff_EEP->GetXaxis()->SetTitle("ix");
	h2diff_EEP->GetYaxis()->SetTitle("iy");
	h2diff_EEP->Draw("colz");

	TCanvas *cdiff_EEM = new TCanvas("cdiff_EEM", "cdiff_EEM", 700, 500);
	cdiff_EEM->SetGridx();
	cdiff_EEM->SetGridy();
	cdiff_EEM -> cd();
	cdiff_EEM->SetLeftMargin(0.1);
	cdiff_EEM->SetRightMargin(0.15);
	h2diff_EEM->GetZaxis()->SetRangeUser(0.5, 1.6);
	h2diff_EEM->GetXaxis()->SetTitle("ix");
	h2diff_EEM->GetYaxis()->SetTitle("iy");
	h2diff_EEM->Draw("colz");

	TCanvas *csigma_EEP = new TCanvas("csigma_EEP", "csigma_EEP");
	csigma_EEP->SetGridx();
	csigma_EEP->SetGridy();
	sigma_vs_iring_EEP->GetHistogram()->GetYaxis()-> SetRangeUser(0.00, 0.2);
	sigma_vs_iring_EEP->GetHistogram()->GetXaxis()-> SetRangeUser(0, 40);
	sigma_vs_iring_EEP->GetHistogram()->GetYaxis()-> SetTitle("#sigma");
	sigma_vs_iring_EEP->GetHistogram()->GetXaxis()-> SetTitle("iring");
	sigma_vs_iring_EEP->Draw("ap");
	gr_stat_prec_EEP->Draw("psame");

	TCanvas *csigma_EEM = new TCanvas("csigma_EEM", "csigma_EEM");
	csigma_EEM->SetGridx();
	csigma_EEM->SetGridy();
	sigma_vs_iring_EEM->GetHistogram()->GetYaxis()-> SetRangeUser(0.00, 0.2);
	sigma_vs_iring_EEM->GetHistogram()->GetXaxis()-> SetRangeUser(0, 40);
	sigma_vs_iring_EEM->GetHistogram()->GetYaxis()-> SetTitle("#sigma");
	sigma_vs_iring_EEM->GetHistogram()->GetXaxis()-> SetTitle("iring");
	sigma_vs_iring_EEM->Draw("ap");
	gr_stat_prec_EEM->Draw("psame");

	TCanvas *csigma_All = new TCanvas("csigma_All", "csigma_Folded");
	csigma_All->SetGridx();
	csigma_All->SetGridy();
	sigma_vs_iring_All->GetHistogram()->GetYaxis()-> SetRangeUser(0.00, 0.2);
	sigma_vs_iring_All->GetHistogram()->GetXaxis()-> SetRangeUser(0, 40);
	sigma_vs_iring_All->GetHistogram()->GetYaxis()-> SetTitle("#sigma");
	sigma_vs_iring_All->GetHistogram()->GetXaxis()-> SetTitle("iring");
	sigma_vs_iring_All->Draw("ap");
	gr_stat_prec_All->Draw("psame");

	TCanvas *cres_EEP = new TCanvas("cres_EEP", "cresidual_EEP");
	cres_EEP->SetGridx();
	cres_EEP->SetGridy();
	residual_EEP->GetHistogram()->GetYaxis()-> SetRangeUser(-0.1, 0.1);
	residual_EEP->GetHistogram()->GetXaxis()-> SetRangeUser(0, 40);
	residual_EEP-> SetTitle("residual EE+");
	residual_EEP-> SetTitle("iring");
	residual_EEP->SetMarkerStyle(20);
	residual_EEP->SetMarkerSize(1);
	residual_EEP->SetMarkerColor(kGreen + 2);
	residual_EEP->Draw("ap");

	TCanvas *cres_EEM = new TCanvas("cres_EEM", "cresidual_EEM");
	cres_EEM->SetGridx();
	cres_EEM->SetGridy();
	residual_EEM->GetHistogram()->GetYaxis()-> SetRangeUser(-0.1, 0.1);
	residual_EEM->GetHistogram()->GetXaxis()-> SetRangeUser(0, 40);
	residual_EEM->GetHistogram()->GetYaxis()-> SetTitle("residual EE-");
	residual_EEM->GetHistogram()->GetXaxis()-> SetTitle("iring");
	residual_EEM ->SetMarkerStyle(20);
	residual_EEM->SetMarkerSize(1);
	residual_EEM->SetMarkerColor(kGreen + 2);
	residual_EEM->Draw("ap");

	TCanvas *cres_All = new TCanvas("cres_All", "cresidual_Folded");
	cres_All->SetGridx();
	cres_All->SetGridy();
	residual_All->GetHistogram()->GetYaxis()-> SetRangeUser(-0.1, 0.1);
	residual_All->GetHistogram()->GetXaxis()-> SetRangeUser(0, 40);
	residual_All->GetYaxis()->SetTitle("residual EE Folded");
	residual_All->GetXaxis()->SetTitle("iring");
	residual_All->SetMarkerStyle(20);
	residual_All->SetMarkerSize(1);
	residual_All->SetMarkerColor(kGreen + 2);
	residual_All->Draw("ap");

	/// save output result:

	TFile* output = new TFile(outputFile.c_str(), "RECREATE");
	output->cd();
	residual_EEM->Write("residual_EEM");
	residual_EEP->Write("residual_EEP");
	residual_All->Write("residual_All");

	output->Close();

	theApp->Run();

	return 0;

}
