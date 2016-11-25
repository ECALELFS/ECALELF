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

	// Foundamental parameters
	const char* infile2 = "/data1/rgerosa/L3_Weight/16Jan_LC_20120131_FT_R_42_V24/Even_WZAnalysis_WElectron_ZElectron_42XReReco_16Jan_LC_20120131_FT_R_42_V24_Z_R9_EE.root ";
	const char* infile3 = "/data1/rgerosa/L3_Weight/16Jan_LC_20120131_FT_R_42_V24/Odd_WZAnalysis_WElectron_ZElectron_42XReReco_16Jan_LC_20120131_FT_R_42_V24_Z_R9_EE.root";
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

	TApplication* theApp = new TApplication("Application", &argc, argv);


	if ( evalStat && (!infile2 || !infile3 )) {
		cout << " No input files to evaluate statistical precision specified !" << endl;
		return -1;
	}

	cout << "Making calibration plots for: " << infile2 << endl;

	char hname[100];

	// Functions for fit

	TF1 *fgaus = new TF1("fgaus", "gaus", -10, 10);

	TF1 *pol0_0EEP = new TF1("pol0_0EEP", "pol1", 85, 95);
	TF1 *pol0_1EEP = new TF1("pol0_1EEP", "pol1", 95, 105);
	TF1 *pol0_2EEP = new TF1("pol0_2EEP", "pol1", 105, 125);
	TF1 *pol0_0EEM = new TF1("pol0_0EEM", "pol1", 85, 95);
	TF1 *pol0_1EEM = new TF1("pol0_1EEM", "pol1", 95, 105);
	TF1 *pol0_2EEM = new TF1("pol0_2EEM", "pol1", 105, 125);

	TGraphErrors *statprecision_vs_ietaEEP[nLoops];
	TGraphErrors *statprecision_vs_ietaEEM[nLoops];

	TGraphErrors *statprecision_vs_loopEEP[3];
	TGraphErrors *statprecision_vs_loopEEM[3];


	int ipointEEP = 0;
	int ipointEEM = 0;

	TFile *f2 = new TFile(infile2);
	TFile *f3 = new TFile(infile3);
	TH2F *hcmap1EEP[nLoops];
	TH2F *hcmap1EEM[nLoops];
	TH2F *hcmap2EEP[nLoops];
	TH2F *hcmap2EEM[nLoops];


	for ( int ietaregion = 0; ietaregion < 3; ietaregion++) {

		statprecision_vs_loopEEP[ietaregion] = new TGraphErrors();
		statprecision_vs_loopEEP[ietaregion]->SetMarkerStyle(20);
		statprecision_vs_loopEEP[ietaregion]->SetMarkerColor(kBlue + 2);

		statprecision_vs_loopEEM[ietaregion] = new TGraphErrors();
		statprecision_vs_loopEEM[ietaregion]->SetMarkerStyle(20);
		statprecision_vs_loopEEM[ietaregion]->SetMarkerColor(kBlue + 2);

		if (ietaregion == 0) statprecision_vs_loopEEP[ietaregion]->SetTitle("85<i#eta < 100");
		if (ietaregion == 1) statprecision_vs_loopEEP[ietaregion]->SetTitle("100 < i#eta < 110");
		if (ietaregion == 2) statprecision_vs_loopEEP[ietaregion]->SetTitle("110 < i#eta < 125");

		if (ietaregion == 0) statprecision_vs_loopEEM[ietaregion]->SetTitle("85<i#eta < 100");
		if (ietaregion == 1) statprecision_vs_loopEEM[ietaregion]->SetTitle("100 < i#eta < 110");
		if (ietaregion == 2) statprecision_vs_loopEEM[ietaregion]->SetTitle("110 < i#eta < 125");

	}

	// analyze each iteration loop step

	TEndcapRings *eRings = new TEndcapRings();

	for ( int iLoop = 0; iLoop < nLoops; iLoop++ ) {

		sprintf(hname, "h2_%d_hC_scale_EEP", iLoop);
		hcmap1EEP[iLoop] = (TH2F*)f2->Get(hname);
		hcmap2EEP[iLoop] = (TH2F*)f3->Get(hname);

		sprintf(hname, "h2_%d_hC_scale_EEM", iLoop);
		hcmap1EEM[iLoop] = (TH2F*)f2->Get(hname);
		hcmap2EEM[iLoop] = (TH2F*)f3->Get(hname);

		// compute statistical precision
		TH1F *hstatprecisionEEP[40];
		TH1F *hstatprecisionEEM[40];

		for(int i = 0; i < 40; i++) {

			sprintf(hname, "hstatprecision_ring_ieta_EEP%02d_iLoop%02d", i, iLoop);
			hstatprecisionEEP[i] = new TH1F(hname, hname, 100, -0.4, 0.4);
			sprintf(hname, "hstatprecision_ring_ieta_EEM%02d_iLoop%02d", i, iLoop);
			hstatprecisionEEM[i] = new TH1F(hname, hname, 100, -0.4, 0.4);
		}

		for (int ix = 1; ix < hcmap1EEP[iLoop]-> GetNbinsX() + 1; ix++) {
			for (int iy = 1; iy < hcmap1EEP[iLoop]-> GetNbinsY() + 1; iy++) {

				if( hcmap1EEP[iLoop]->GetBinContent(ix, iy) == 0 ||  hcmap2EEP[iLoop]->GetBinContent(ix, iy) == 0 ) continue;
				int etaring = int(fabs(eRings->GetEndcapIeta(ix, iy, 1)));
				if(etaring < 0) continue;
				float ic1 = hcmap1EEP[iLoop]->GetBinContent(ix, iy);
				float ic2 = hcmap2EEP[iLoop]->GetBinContent(ix, iy);
				if (ic1 > 0 && ic1 < 2 && ic1 != 1 && ic2 > 0 && ic2 < 2 && ic2 != 1) {
					hstatprecisionEEP[etaring - 86]->Fill((ic1 - ic2) / (ic1 + ic2)); // sigma (diff/sum) gives the stat. precision on teh entire

				}
			}
		}

		for (int ix = 1; ix < hcmap1EEM[iLoop]-> GetNbinsX() + 1; ix++) {
			for (int iy = 1; iy < hcmap1EEM[iLoop]-> GetNbinsY() + 1; iy++) {

				if( hcmap1EEM[iLoop]->GetBinContent(ix, iy) == 0 ||  hcmap2EEM[iLoop]->GetBinContent(ix, iy) == 0 ) continue;
				int etaring = int(fabs(eRings->GetEndcapIeta(ix, iy, -1)));
				if(etaring < 0) continue;
				float ic1 = hcmap1EEM[iLoop]->GetBinContent(ix, iy);
				float ic2 = hcmap2EEM[iLoop]->GetBinContent(ix, iy);
				if (ic1 > 0 && ic1 < 2 && ic1 != 1 && ic2 > 0 && ic2 < 2 && ic2 != 1) {
					hstatprecisionEEM[etaring - 86]->Fill((ic1 - ic2) / (ic1 + ic2)); // sigma (diff/sum) gives the stat. precision on teh entire sample

				}
			}
		}


		statprecision_vs_ietaEEP[iLoop] = new TGraphErrors();
		statprecision_vs_ietaEEP[iLoop]->SetMarkerStyle(20);
		statprecision_vs_ietaEEP[iLoop]->SetMarkerSize(1);
		statprecision_vs_ietaEEP[iLoop]->SetMarkerColor(kRed + 2);

		int nEEP = 0;
		int nEEM = 0;
		for (int i = 0 ; i < 33 ; i++) {
			if ( hstatprecisionEEP[i]->GetEntries() <= 25) continue;
			fgaus->SetParameter(1, hstatprecisionEEP[i]->GetMean());
			fgaus->SetParameter(2, hstatprecisionEEP[i]->GetRMS());
			fgaus->SetRange(-5 * hstatprecisionEEP[i]->GetRMS(), 5 * hstatprecisionEEP[i]->GetRMS());
			hstatprecisionEEP[i]->Fit("fgaus", "QRM");
			statprecision_vs_ietaEEP[iLoop]-> SetPoint(nEEP, 86 + i, fgaus->GetParameter(2));
			statprecision_vs_ietaEEP[iLoop]-> SetPointError(nEEP, 0, fgaus->GetParError(2));
			nEEP++;
		}
		statprecision_vs_ietaEEP[iLoop]->Fit("pol0_0EEP", "QMER");
		statprecision_vs_ietaEEP[iLoop]->Fit("pol0_1EEP", "QMER");
		statprecision_vs_ietaEEP[iLoop]->Fit("pol0_2EEP", "QMER");

		statprecision_vs_loopEEP[0]->SetPoint(ipointEEP, iLoop + 1, pol0_0EEP->GetParameter(0) + pol0_0EEP->GetParameter(1) * 90);
		statprecision_vs_loopEEP[0]->SetPointError(ipointEEP, 0.5, 0.5 * sqrt(pol0_0EEP->GetParError(0)*pol0_0EEP->GetParError(0) + pol0_0EEP->GetParError(1)*pol0_0EEP->GetParError(1)));


		statprecision_vs_loopEEP[1]->SetPoint(ipointEEP, iLoop + 1, pol0_1EEP->GetParameter(0) + pol0_1EEP->GetParameter(1) * 100);
		statprecision_vs_loopEEP[1]->SetPointError(ipointEEP, 0.5, 0.5 * sqrt(pol0_1EEP->GetParError(0)*pol0_1EEP->GetParError(0) + pol0_1EEP->GetParError(1)*pol0_1EEP->GetParError(1)));


		statprecision_vs_loopEEP[2]->SetPoint(ipointEEP, iLoop + 1, pol0_2EEP->GetParameter(0) + pol0_2EEP->GetParameter(1) * 110);
		statprecision_vs_loopEEP[2]->SetPointError(ipointEEP, 0.5, 0.5 * sqrt(pol0_2EEP->GetParError(0)*pol0_2EEP->GetParError(0) + pol0_2EEP->GetParError(1)*pol0_2EEP->GetParError(1)));


		ipointEEP++;

		statprecision_vs_ietaEEM[iLoop] = new TGraphErrors();
		statprecision_vs_ietaEEM[iLoop]->SetMarkerStyle(20);
		statprecision_vs_ietaEEM[iLoop]->SetMarkerSize(1);
		statprecision_vs_ietaEEM[iLoop]->SetMarkerColor(kRed + 2);

		for (int i = 0 ; i < 33 ; i++) {

			if ( hstatprecisionEEM[i]->GetEntries() <= 25) continue;
			fgaus->SetParameter(1, hstatprecisionEEM[i]->GetMean());
			fgaus->SetParameter(2, hstatprecisionEEM[i]->GetRMS());
			fgaus->SetRange(-5 * hstatprecisionEEM[i]->GetRMS(), 5 * hstatprecisionEEM[i]->GetRMS());
			hstatprecisionEEM[i]->Fit("fgaus", "QRM");
			statprecision_vs_ietaEEM[iLoop]-> SetPoint(nEEM, 86 + i, fgaus->GetParameter(2));
			statprecision_vs_ietaEEM[iLoop]-> SetPointError(nEEM, 0, fgaus->GetParError(2));
			nEEM++;
		}

		statprecision_vs_ietaEEM[iLoop]->Fit("pol0_0EEM", "QRME");
		statprecision_vs_ietaEEM[iLoop]->Fit("pol0_1EEM", "QRME");
		statprecision_vs_ietaEEM[iLoop]->Fit("pol0_2EEM", "QRME");

		statprecision_vs_loopEEM[0]->SetPoint(ipointEEM, iLoop + 1, pol0_0EEM->GetParameter(0) + pol0_0EEM->GetParameter(1) * 90);
		statprecision_vs_loopEEM[0]->SetPointError(ipointEEM, 0.5, 0.5 * sqrt(pol0_0EEM->GetParError(0)*pol0_0EEM->GetParError(0) + pol0_0EEM->GetParError(1)*pol0_0EEM->GetParError(1)));
		statprecision_vs_loopEEM[1]->SetPoint(ipointEEM, iLoop + 1, pol0_1EEM->GetParameter(0) + pol0_1EEM->GetParameter(1) * 100);
		statprecision_vs_loopEEM[1]->SetPointError(ipointEEM, 0.5, 0.5 * sqrt(pol0_1EEM->GetParError(0)*pol0_1EEM->GetParError(0) + pol0_1EEM->GetParError(1)*pol0_1EEM->GetParError(1)));
		statprecision_vs_loopEEM[2]->SetPoint(ipointEEM, iLoop + 1, pol0_2EEM->GetParameter(0) + pol0_2EEM->GetParameter(1) * 110);
		statprecision_vs_loopEEM[2]->SetPointError(ipointEEM, 0.5, 0.5 * sqrt(pol0_2EEM->GetParError(0)*pol0_2EEM->GetParError(0) + pol0_2EEM->GetParError(1)*pol0_2EEM->GetParError(1)));


		ipointEEM++;

	}


	// DrawPlots

	TCanvas *cEEP[4];
	TCanvas *cEEM[4];


	//  TFile * out = new TFile ("StatPrecEE.root","RECREATE");
	// --- plot 5 : statistical precision vs ieta

	/* for ( int iLoop = 0; iLoop < nLoops; iLoop++ ) {
	   sprintf(hname,"cstat_EEP%d",iLoop);
	   c[iLoop] = new TCanvas(hname,hname);
	   c[iLoop]->SetGridx();
	   c[iLoop]->SetGridy();
	   statprecision_vs_ietaEEP[iLoop]->GetHistogram()->GetYaxis()-> SetRangeUser(0.001,0.15);
	   statprecision_vs_ietaEEP[iLoop]->GetHistogram()->GetXaxis()-> SetRangeUser(85,125);
	   statprecision_vs_ietaEEP[iLoop]->GetHistogram()->GetYaxis()-> SetTitle("#sigma((c_{P}-c_{D})/(c_{P}+c_{D}))");
	   statprecision_vs_ietaEEP[iLoop]->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
	   statprecision_vs_ietaEEP[iLoop]->Draw("ap");
	   if(iLoop == nLoops -1)
	   {
	    out->cd();
	    statprecision_vs_ietaEEP[iLoop]->SetName("gr_stat_precEEP");
	    statprecision_vs_ietaEEP[iLoop]->Write();
	   }

	 }*/


	/* for ( int iLoop = nLoops; iLoop < nLoops*2; iLoop++ ) {
	    sprintf(hname,"cstat_EEM%d",iLoop-nLoops);
	    c[iLoop] = new TCanvas(hname,hname);
	    c[iLoop]->SetGridx();
	    c[iLoop]->SetGridy();
	    statprecision_vs_ietaEEM[iLoop-nLoops]->GetHistogram()->GetYaxis()-> SetRangeUser(0.001,0.15);
	    statprecision_vs_ietaEEM[iLoop-nLoops]->GetHistogram()->GetXaxis()-> SetRangeUser(-125,-85);
	    statprecision_vs_ietaEEM[iLoop-nLoops]->GetHistogram()->GetYaxis()-> SetTitle("#sigma((c_{P}-c_{D})/(c_{P}+c_{D}))");
	    statprecision_vs_ietaEEM[iLoop-nLoops]->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
	    statprecision_vs_ietaEEM[iLoop-nLoops]->Draw("ap");
	    if(iLoop == 2*nLoops -1)
	    {
	     out->cd();
	     statprecision_vs_ietaEEM[iLoop-nLoops]->SetName("gr_stat_precEEM");
	     statprecision_vs_ietaEEM[iLoop-nLoops]->Write();
	    }

	  }*/

	// --- plot 6 : statistical precision vs loop

	for ( int ietaregion = 0; ietaregion < 3; ietaregion++ ) {
		sprintf(hname, "ietaregion_EEP%d", ietaregion);
		cEEP[ietaregion] = new TCanvas(hname, hname);
		cEEP[ietaregion]->SetGridx();
		cEEP[ietaregion]->SetGridy();
		statprecision_vs_loopEEP[ietaregion]->GetHistogram()->GetYaxis()-> SetRangeUser(0., 0.1);
		statprecision_vs_loopEEP[ietaregion]->GetHistogram()->GetXaxis()-> SetRangeUser(0, nLoops + 1);
		statprecision_vs_loopEEP[ietaregion]->GetHistogram()->GetYaxis()-> SetTitle("Statistical precision");
		statprecision_vs_loopEEP[ietaregion]->GetHistogram()->GetXaxis()-> SetTitle("n#circ iteration");
		statprecision_vs_loopEEP[ietaregion]->Draw("ap");
	}

	for ( int ietaregion = 0; ietaregion < 3; ietaregion++ ) {
		sprintf(hname, "ietaregion_EEM%d", ietaregion);
		cEEM[ietaregion] = new TCanvas(hname, hname);
		cEEM[ietaregion]->SetGridx();
		cEEM[ietaregion]->SetGridy();
		statprecision_vs_loopEEM[ietaregion]->GetHistogram()->GetYaxis()-> SetRangeUser(0., 0.1);
		statprecision_vs_loopEEM[ietaregion]->GetHistogram()->GetXaxis()-> SetRangeUser(0, nLoops + 1);
		statprecision_vs_loopEEM[ietaregion]->GetHistogram()->GetYaxis()-> SetTitle("Statistical precision");
		statprecision_vs_loopEEM[ietaregion]->GetHistogram()->GetXaxis()-> SetTitle("n#circ iteration");
		statprecision_vs_loopEEM[ietaregion]->Draw("ap");
	}

	theApp->Run();
	return 0;
}
