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
#include "../interface/TEndcapRings.h"

using namespace std ;

int main(int argc, char**argv)
{


	const char* infile2 = "/data1/rgerosa/L3_Weight/PromptSkim_Single_Double_Electron_recoFlag/EB/Even_WZAnalysis_PromptSkim_W-DoubleElectron_FT_R_42_V21B_Z_noEP.root";
	const char* infile3 = "/data1/rgerosa/L3_Weight/PromptSkim_Single_Double_Electron_recoFlag/EB/Odd_WZAnalysis_PromptSkim_W-DoubleElectron_FT_R_42_V21B_Z_noEP.root";
	int evalStat = 1;
	int inputLoops = 25;

	const int nLoops = inputLoops;

// Set style options
	gROOT->Reset();
	gROOT->SetStyle("Plain");

	gStyle->SetPadTickX(1);
	gStyle->SetPadTickY(1);
	gStyle->SetOptTitle(1);
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
	gStyle->SetTitleXOffset(0.8);
	gStyle->SetTitleYOffset(1.1);
	gROOT->ForceStyle();


	if ( evalStat && (!infile2 || !infile3 )) {
		cout << " No input files to evaluate statistical precision specified !" << endl;
		return -1;
	}

	cout << "Making calibration plots for: " << infile2 << endl;

	TApplication* theApp = new TApplication("Application", &argc, argv);

	char hname[100];

	TF1 *fgaus = new TF1("fgaus", "gaus", -10, 10);

	TF1 *pol0_0 = new TF1("pol0_0", "pol1", 0, 20);
	TF1 *pol0_1 = new TF1("pol0_1", "pol1", 20, 40);
	TF1 *pol0_2 = new TF1("pol0_2", "pol1", 40, 60);
	TF1 *pol0_3 = new TF1("pol0_3", "pol1", 60, 85);

	TFile *f2 = new TFile(infile2);
	TFile *f3 = new TFile(infile3);
	TH2F *hcmap2[nLoops];
	TH2F *hcmap3[nLoops];

	TGraphErrors *statprecision_vs_ieta[nLoops];
	TGraphErrors *statprecision_vs_loop[4];

	int ipoint = 0;

	for ( int ietaregion = 0; ietaregion < 4; ietaregion++) {

		statprecision_vs_loop[ietaregion] = new TGraphErrors();
		statprecision_vs_loop[ietaregion]->SetMarkerStyle(20);
		statprecision_vs_loop[ietaregion]->SetMarkerSize(1);
		statprecision_vs_loop[ietaregion]->SetMarkerColor(kBlue + 2);
		if (ietaregion == 0) statprecision_vs_loop[ietaregion]->SetTitle("i#eta < 20");
		if (ietaregion == 1) statprecision_vs_loop[ietaregion]->SetTitle("20 < i#eta < 40");
		if (ietaregion == 2) statprecision_vs_loop[ietaregion]->SetTitle("40 < i#eta < 60");
		if (ietaregion == 3) statprecision_vs_loop[ietaregion]->SetTitle("60 < i#eta < 85");
	}



	for ( int iLoop = 0; iLoop < nLoops; iLoop++ ) {

		sprintf(hname, "h2_%d_hC_scale_EB", iLoop);
		hcmap2[iLoop] = (TH2F*)f2->Get(hname);
		hcmap3[iLoop] = (TH2F*)f3->Get(hname);

		TH1F *hstatprecision[171];

		for (int jbin = 1; jbin < hcmap2[iLoop]-> GetNbinsY() + 1; jbin++) {

			float etaring = hcmap2[iLoop]-> GetYaxis()->GetBinCenter(jbin);
			sprintf(hname, "hstatprecision_ring_ieta%02f_%d", etaring, iLoop);
			hstatprecision[jbin - 1] = new TH1F(hname, hname, 150, -0.5, 0.5);
			for (int ibin = 1; ibin < hcmap2[iLoop]-> GetNbinsX() + 1; ibin++) {

				float ic1 = hcmap2[iLoop]->GetBinContent(ibin, jbin);
				float ic2 = hcmap3[iLoop]->GetBinContent(ibin, jbin);
				if (ic1 > 0 && ic1 < 2 && ic1 != 1 && ic2 > 0 && ic2 < 2 && ic2 != 1) {
					hstatprecision[jbin - 1]->Fill((ic1 - ic2) / (ic1 + ic2)); // sigma (diff/sum) gives the stat. precision on teh entire sample

				}
			}
		}


		statprecision_vs_ieta[iLoop] = new TGraphErrors();
		statprecision_vs_ieta[iLoop]->SetMarkerStyle(20);
		statprecision_vs_ieta[iLoop]->SetMarkerSize(1);
		statprecision_vs_ieta[iLoop]->SetMarkerColor(kRed + 2);

		int n = 0;

		for (int i = 1; i < hcmap2[iLoop]-> GetNbinsY() + 1; i++) {
			float etaring = hcmap2[iLoop]-> GetYaxis()->GetBinCenter(i);
			if (int(etaring) == 0) continue;
			if ( hstatprecision[i - 1]->GetEntries() == 0) continue;
			float e     = 0.5 * hcmap2[iLoop]-> GetYaxis()->GetBinWidth(i);
			fgaus->SetParameter(1, 1);
			fgaus->SetParameter(2, hstatprecision[i - 1]->GetRMS());
			fgaus->SetRange(-5 * hstatprecision[i - 1]->GetRMS(), 5 * hstatprecision[i - 1]->GetRMS());
			hstatprecision[i - 1]->Fit("fgaus", "QR");
			statprecision_vs_ieta[iLoop]-> SetPoint(n, etaring, fgaus->GetParameter(2));
			statprecision_vs_ieta[iLoop]-> SetPointError(n, e, fgaus->GetParError(2));
			n++;
		}

		statprecision_vs_ieta[iLoop]->Fit("pol0_0", "QR");
		statprecision_vs_ieta[iLoop]->Fit("pol0_1", "QR");
		statprecision_vs_ieta[iLoop]->Fit("pol0_2", "QR");
		statprecision_vs_ieta[iLoop]->Fit("pol0_3", "QR");

		statprecision_vs_loop[0]->SetPoint(ipoint, iLoop + 1, pol0_0->GetParameter(0) + pol0_0->GetParameter(1) * 10);
		statprecision_vs_loop[0]->SetPointError(ipoint, 0.5, sqrt(pol0_0->GetParError(0)*pol0_0->GetParError(0) + pol0_0->GetParError(1)*pol0_0->GetParError(1)));
		statprecision_vs_loop[1]->SetPoint(ipoint, iLoop + 1, pol0_1->GetParameter(0) + pol0_1->GetParameter(1) * 30);
		statprecision_vs_loop[1]->SetPointError(ipoint, 0.5, sqrt(pol0_1->GetParError(0)*pol0_1->GetParError(0) + pol0_1->GetParError(1)*pol0_1->GetParError(1)));
		statprecision_vs_loop[2]->SetPoint(ipoint, iLoop + 1, pol0_2->GetParameter(0) + pol0_2->GetParameter(1) * 50);
		statprecision_vs_loop[2]->SetPointError(ipoint, 0.5, sqrt(pol0_0->GetParError(0)*pol0_2->GetParError(0) + pol0_2->GetParError(1)*pol0_2->GetParError(1)));
		statprecision_vs_loop[3]->SetPoint(ipoint, iLoop + 1, pol0_3->GetParameter(0) + pol0_3->GetParameter(1) * 72.5);
		statprecision_vs_loop[3]->SetPointError(ipoint, 0.5, sqrt(pol0_3->GetParError(0)*pol0_3->GetParError(0) + pol0_3->GetParError(1)*pol0_3->GetParError(1)));
		ipoint++;

	}

	//-----------------------------------------------------------------
	//--- Draw plots
	//-----------------------------------------------------------------
	TCanvas *c[nLoops];
	TCanvas *c2[4];

	TFile * out = new TFile ("StatPrecEB.root", "RECREATE");
	// --- plot 5 : statistical precision vs ieta

	for ( int iLoop = 0; iLoop < nLoops; iLoop++ ) {

		sprintf(hname, "cstat_%d", iLoop);
		c[iLoop] = new TCanvas(hname, hname);
		c[iLoop]->SetGridx();
		c[iLoop]->SetGridy();
		statprecision_vs_ieta[iLoop]->GetHistogram()->GetYaxis()-> SetRangeUser(0.0001, 0.07);
		statprecision_vs_ieta[iLoop]->GetHistogram()->GetXaxis()-> SetRangeUser(-85, 85);
		statprecision_vs_ieta[iLoop]->GetHistogram()->GetYaxis()-> SetTitle("#sigma((c_{P}-c_{D})/(c_{P}+c_{D}))");
		statprecision_vs_ieta[iLoop]->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
		statprecision_vs_ieta[iLoop]->Draw("ap");

		if(iLoop == nLoops - 1) {
			out->cd();
			statprecision_vs_ieta[iLoop]->SetName("gr_stat_prec");
			statprecision_vs_ieta[iLoop]->Write();
		}

	}


	for ( int ietaregion = 0; ietaregion < 4; ietaregion++ ) {

		sprintf(hname, "ietaregion_%d", ietaregion);
		c2[ietaregion] = new TCanvas(hname, hname);
		c2[ietaregion]->SetGridx();
		c2[ietaregion]->SetGridy();
		statprecision_vs_loop[ietaregion]->GetHistogram()->GetYaxis()-> SetRangeUser(0., 0.04);
		statprecision_vs_loop[ietaregion]->GetHistogram()->GetXaxis()-> SetRangeUser(0, nLoops + 1);
		statprecision_vs_loop[ietaregion]->GetHistogram()->GetYaxis()-> SetTitle("Statistical precision");
		statprecision_vs_loop[ietaregion]->GetHistogram()->GetXaxis()-> SetTitle("n#circ iteration");
		statprecision_vs_loop[ietaregion]->Draw("ap");
	}

	theApp->Run();
	return 0;
}
