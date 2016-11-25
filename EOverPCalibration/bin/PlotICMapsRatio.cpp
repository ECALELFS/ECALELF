//Plot map ratio between two IC sets (used in miscalib studies)

#include <vector>
#include <utility>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "TH2F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TF1.h"
#include "TLegend.h"
#include "TGraphErrors.h"
#include "TApplication.h"
#include "TLatex.h"
#include "TFile.h"
#include "TGraph.h"

int main(int argc, char **argv)
{

	std::ifstream io1, io2, rms1, rms2;

	//  std::ifstream io3, io4, io6;
	io1.open ("output_runD_10ITER_SISCALIB_ETABIN2/IC_Run2012ABC_22JanuaryRereco_WZ_Fbrem_EB_SISCALIB_relative.txt");
	io2.open ("output_runD_10ITER_NOSCALIB/IC_Run2012ABC_22JanuaryRereco_WZ_Fbrem_EB_SISCALIB_relative.txt");

	//  io3.open ("output_runD_10ITER_SISCALIB_ETABIN3/IC_Run2012ABC_22JanuaryRereco_WZ_Fbrem_EB_SISCALIB_relative.txt");
	//  io4.open ("output_runD_10ITER_SISCALIB_ETABIN4/IC_Run2012ABC_22JanuaryRereco_WZ_Fbrem_EB_SISCALIB_relative.txt");
	//  io6.open ("output_runD_10ITER_SISCALIB_ETABIN6/IC_Run2012ABC_22JanuaryRereco_WZ_Fbrem_EB_SISCALIB_relative.txt");
	//  rms1.open ("output_runD_10ITER_SISCALIB_ETALINEAR/RMSFile.txt");
	//  rms2.open ("output_runD_10ITER_NOSCALIB/RMSFile.txt");

	float status, IC, err;
	float status2, IC2, err2;

	//  float IC3,IC4,IC6;
	float etaCount = 0., etaCount2 = 0.;

	int eta, phi, eta2, phi2;

	TH2F *mapRatio = new TH2F ("map", "map", 360, 0.5, 360.5, 171, -85, +85);
	TH1F *histo = new TH1F ("histo", "histo", 100, 0.995, 1.005);
	//  TH1F *histoSpreadRatio = new TH1F ("histoSpreadRatio", "histoSpreadRatio", 100, 0.9, 1.1);
	TH1F *histoMeanEta = new TH1F ("histoMeanEta", "histoMeanEta", 100, 0.995, 1.005);
	TH1F *histoMeanPhi = new TH1F ("histoMeanPhi", "histoMeanPhi", 100, 0.995, 1.005);

	TH2F *ICmap1 = new TH2F ("map1", "map1", 360, 0.5, 360.5, 171, -85, +85);
	TH2F *ICmap2 = new TH2F ("map2", "map2", 360, 0.5, 360.5, 171, -85, +85);
	TH1F *hIC1 = new TH1F ("hIC1", "hIC1", 100, 0.7, 1.3);
	TH1F *hIC2 = new TH1F ("hIC2", "hIC2", 100, 0.7, 1.3);

	//  TH1F *hprova = new TH1F ("g_RMS", "g_RMS", 91,0,91);
	TGraph *g_RMS = new TGraph();

	//  TGraph *g_RMS3 = new TGraph();
	//  TGraph *g_RMS4 = new TGraph();
	//  TGraph *g_RMS6 = new TGraph();

	TH1F *histoEtaRing[86];
	char histoName[100];
	char funcName[100];

	/*  TH1F *histoEtaRing4[86];
	char histoName4[100];
	TH1F *histoEtaRing3[86];
	char histoName3[100];
	TH1F *histoEtaRing6[86];
	char histoName6[100];
	*/
	float map[171][360];
	float map2[171][360];

	for (int e = 0; e < 86; e++) {
		sprintf(histoName, "h_ratio_%d", e);
		histoEtaRing[e] = new TH1F(histoName, "", 50, 0.9, 1.1);

		/*    sprintf(histoName4,"h_ratio4_%d",e);
		histoEtaRing4[e] = new TH1F(histoName4,"",50,0.99,1.01);
		sprintf(histoName3,"h_ratio3_%d",e);
		histoEtaRing3[e] = new TH1F(histoName3,"",50,0.99,1.01);
		sprintf(histoName6,"h_ratio6_%d",e);
		histoEtaRing6[e] = new TH1F(histoName6,"",50,0.99,1.01);
		*/
	}

	for (int e = 0; e < 171; e++) {
		for (int p = 0; p < 360; p++) {
			map[e][p] = 0.;
			map2[e][p] = 0.;
		}
	}


	//  mapRatio->SetDrawOption ("colz");

	eta = 0;


	while (!io1.eof()) {
		io1 >> eta >> phi >> status >> IC >> err;
		io2 >> eta2 >> phi2 >> status2 >> IC2 >> err2;

		//      io3>>eta>>phi>>status>>IC3>>err;
		//      io4>>eta>>phi>>status>>IC4>>err;
		//      io6>>eta>>phi>>status>>IC6>>err;
		map[eta + 85][phi - 1] = IC;
		map2[eta2 + 85][phi2 - 1] = IC2;

		if ((status == 0) && (IC != -1) && (status2 == 0) && (IC2 != -1)) {
			histoEtaRing[int(fabs(eta))]->Fill(IC / IC2);

			//	  histoEtaRing3[int(fabs(eta))]->Fill(IC3/IC2);
			//	  histoEtaRing4[int(fabs(eta))]->Fill(IC4/IC2);
			//	  histoEtaRing6[int(fabs(eta))]->Fill(IC6/IC2);
		}

		//	  if (fabs(eta)==46)   std::cout<<eta<<" "<<IC/IC2<<std::endl;

		if (phi == 360) {
			histoMeanEta->Fill(etaCount / etaCount2);

			etaCount = 0.;
			etaCount2 = 0.;

		}


		if ((status == 0) && (IC != -1) && (status2 == 0) && (IC2 != -1) && (IC < 1.5)) {
			if ((eta == eta2) && (phi == phi2)) {
				mapRatio->SetBinContent (phi, eta + 85, IC / IC2);
				histo->Fill(IC / IC2);
				//	  std::cout<<IC/IC2<<std::endl;

				ICmap1->SetBinContent (phi, eta + 85, IC);
				ICmap2->SetBinContent (phi, eta + 85, IC2);
				//	  std::cout<<IC-IC2<<std::endl;
				hIC1->Fill(IC);
				hIC2->Fill(IC2);

				etaCount += IC;
				etaCount2 += IC2;
				//	  std::cout<<eta<<" "<<phi<<" "<<etaCount<<" "<<etaCount2<<std::endl;
				//  getchar();

				//	std::cout<<eta<<IC/IC2<<std::endl;
			} else
				std::cout << "Problem: incoherent eta or phi " << std::endl;

		}
	}


	/*  float e1,e2,r1,r2;
	while (!rms1.eof())
	  {
	    rms1 >> e1 >> r1;
	    rms2 >> e2 >> r2;
	    histoSpreadRatio->Fill(r1/r2);
	  }
	*/
	float phiCount = 0., phiCount2 = 0.;
	for (int p = 0; p < 360; p++) {
		for (int e = 0; e < 171; e++) {
			phiCount += map[e][p];
			phiCount2 += map2[e][p];
		}
		histoMeanPhi->Fill(phiCount / phiCount2);
		phiCount = 0.;
		phiCount2 = 0.;
	}

	TFile f1 ("confronti_etabin2.root", "RECREATE");
	f1.cd();

	for (int e = 1; e < 86; e++) {

		sprintf(funcName, "f_%d", e);
		TF1* fgaus = new TF1(funcName, "gaus", 0.99, 1.01);
		fgaus -> SetParameter(1, histoEtaRing[e]->GetMean());
		fgaus -> SetParameter(2, histoEtaRing[e]->GetRMS());
		//    histoEtaRing[e] -> Fit(funcName,"QS+","",1-histoEtaRing[e]->GetRMS(),1+histoEtaRing[e]->GetRMS());
		g_RMS->SetPoint (e - 1, float(e), histoEtaRing[e]->GetRMS());

		//    g_RMS3->SetPoint (e-1, float(e), histoEtaRing3[e]->GetRMS());
		//    g_RMS4->SetPoint (e-1, float(e), histoEtaRing4[e]->GetRMS());
		//    g_RMS6->SetPoint (e-1, float(e), histoEtaRing6[e]->GetRMS());
		//    g_RMS->SetPointError (e-1, 0, 0);
		if (e == 20 || e == 40 || e == 60 || e == 75 || e == 85 || e == 45 || e == 46 || e == 47) {
			histoEtaRing[e]->Draw();
			//   fgaus->Draw("same");
			histoEtaRing[e]->Write();
		}
	}

	TCanvas *c1 = new TCanvas("c1");
	c1->cd();
	g_RMS -> GetXaxis() -> SetTitle("|i#eta|");
	g_RMS -> GetYaxis() -> SetTitle("RMS");
	g_RMS -> SetMinimum(0.00000);
	//    g_RMS -> SetMaximum(0.0025);
	g_RMS -> SetMarkerStyle(20);
	g_RMS -> SetMarkerSize(1.0);
	g_RMS -> SetMarkerColor(kBlue + 1);
	c1->SetGrid();
	g_RMS -> Draw("APL");

	g_RMS -> GetXaxis() -> SetRangeUser (0, 86);

	/*  g_RMS3 -> SetMarkerStyle(20);
	g_RMS3 -> SetMarkerSize(1.0);
	g_RMS3 -> SetMarkerColor(51+1);
	g_RMS4 -> SetMarkerStyle(20);
	g_RMS4 -> SetMarkerSize(1.0);
	g_RMS4 -> SetMarkerColor(kRed+1);
	g_RMS6 -> SetMarkerStyle(20);
	g_RMS6 -> SetMarkerSize(1.0);
	g_RMS6 -> SetMarkerColor(kOrange+1);

	    g_RMS3 -> Draw("PLsame");
	   g_RMS4 -> Draw("PLsame");
	  g_RMS6 -> Draw("PLsame");

	  TLegend* leg = new TLegend(0.15,0.72,0.43,0.89);
	  leg -> SetFillColor(0);
	  leg -> SetTextFont(42);
	  leg -> SetTextSize(0.05);
	  leg -> AddEntry(g_RMS,"Miscalib. 2%","P");
	  leg -> AddEntry(g_RMS3,"Miscalib. 3%","P");
	  leg -> AddEntry(g_RMS4,"Miscalib. 4%","P");
	  leg -> AddEntry(g_RMS6,"Miscalib. 6%","P");
	  leg -> Draw("same");
	*/
	c1->Print("g_RMS.png", "png");

	mapRatio -> GetXaxis() -> SetTitle("i#phi");
	mapRatio -> GetYaxis() -> SetRangeUser(-85, 85);

	mapRatio -> GetYaxis() -> SetTitle("i#eta");

	histo -> GetXaxis() -> SetTitle("IC1/IC2");
	histo -> GetYaxis() -> SetTitle("N");

	histo->SetStats(1);

	mapRatio->GetZaxis()->SetRangeUser(0.99, 1.01);
	//  mapRatio->Draw("COLZ");
	//          mapRatio->SaveAs("map.root","root");
	mapRatio->Write();
	//      histo->Draw();
	histo->Write();
	histoMeanEta->Write();
	histoMeanPhi->Write();
	hIC1->Write();
	hIC2->Write();
	ICmap1->Write();
	ICmap2->Write();
	//  histoSpreadRatio->Write();
	g_RMS->Write("g_RMS");
	f1.Close();

	return 0;
}
