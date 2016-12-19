#include <vector>
#include <utility>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include "TStyle.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TString.h"
#include "TGraphErrors.h"
#include "../interface/TEndcapRings.h"
#include "TLegend.h"
#include "TApplication.h"

using namespace std;

int main (int argc, char**argv)
{

	if(argc != 3) {
		std::cerr << " Wrongs input " << std::endl;
		return -1;
	}

	std::string inputFile1 = argv[1];
	std::string inputFile2 = argv[2];

	std::cout << " InputFile1 = " << inputFile1 << std::endl;
	std::cout << " InputFile2 = " << inputFile2 << std::endl;

	std::ifstream File1 (inputFile1.c_str());
	std::ifstream File2 (inputFile2.c_str());
	std::ifstream File3 (inputFile1.c_str());
	std::ifstream File4 (inputFile2.c_str());

	if(!File1.is_open()) {
		std::cerr << "** ERROR: Can't open '" << inputFile1 << "' for input" << std::endl;
		return -1;
	}

	if(!File2.is_open()) {
		std::cerr << "** ERROR: Can't open '" << inputFile2 << "' for input" << std::endl;
		return -1;
	}

	std::ofstream FileRatio("ratio.txt");
	std::ofstream FileAbsolute("absolute.txt");
	std::ofstream FileMiscalib("miscalib.txt");

// Set style options
	/*gROOT->Reset();
	gROOT->SetStyle("Plain");
	*/

	gStyle->SetPadTickX(1);
	gStyle->SetPadTickY(1);
	gStyle->SetOptTitle(1);
	gStyle->SetOptStat(00000000);
	gStyle->SetOptFit(1111);
//gStyle->SetOptStat(0000000);
//gStyle->SetFitFormat("6.3g");
	gStyle->SetPalette(1);
	gStyle->SetOptTitle(0);


	gStyle->SetTextFont(42);
	gStyle->SetTextSize(0.05);
	gStyle->SetTitleFont(42, "xyz");
	gStyle->SetTitleSize(0.05);
	gStyle->SetLabelFont(42, "xyz");
	gStyle->SetLabelSize(0.05);
	gStyle->SetTitleXOffset(0.8);
	gStyle->SetTitleYOffset(1.1);
	gROOT->ForceStyle();




	int iPhi, iEta, iz;
	double ic, eic;

	int iPhi1, iEta1, iz1;
	double ic1, eic1;
	int iPhi2, iEta2, iz2;
	double ic2, eic2;

/// Histo for first ic set
	TString Name = Form("mapEB_%s", inputFile1.c_str());
	TH2F * map1_EB = new TH2F(Name, Name, 360, 1, 361, 171, -85, 86);
	Name = Form("mapEEp_%s", inputFile1.c_str());
	TH2F * map1_EEp = new TH2F(Name, Name, 100, 1, 101, 100, 1, 101);
	Name = Form("mapEEm_%s", inputFile1.c_str());
	TH2F * map1_EEm = new TH2F(Name, Name, 100, 1, 101, 100, 1, 101);

	std::cout << " Opening first file ..... reading " << std::endl;
	while (!File1.eof()) {
		File1 >> iEta >> iPhi >> iz >> ic >> eic ;
		if(iz == 0) map1_EB->Fill(iPhi, iEta, ic);
		if(iz == 1) map1_EEp->Fill(iEta, iPhi, ic);
		if(iz == -1)map1_EEm->Fill(iEta, iPhi, ic);
	}
	std::cout << " End first file " << std::endl;
	std::cout << " Opening second file ..... reading " << std::endl;

/// Histo for first ic set
	Name = Form("mapEB_%s", inputFile2.c_str());
	TH2F * map2_EB = new TH2F(Name, Name, 360, 1, 361, 171, -85, 86);
	Name = Form("mapEEp_%s", inputFile2.c_str());
	TH2F * map2_EEp = new TH2F(Name, Name, 100, 1, 101, 100, 1, 101);
	Name = Form("mapEEm_%s", inputFile2.c_str());
	TH2F * map2_EEm = new TH2F(Name, Name, 100, 1, 101, 100, 1, 101);

	while (!File2.eof()) {
		File2 >> iEta >> iPhi >> iz >> ic >> eic ;
		if(iz == 0) map2_EB->Fill(iPhi, iEta, ic);
		if(iz == 1) map2_EEp->Fill(iEta, iPhi, ic);
		if(iz == -1) map2_EEm->Fill(iEta, iPhi, ic);
	}
	std::cout << " End second file " << std::endl;

	int cont = 0;
	while (!File3.eof() || !File4.eof()) {
		File3 >> iEta1 >> iPhi1 >> iz1 >> ic1 >> eic1 ;
		File4 >> iEta2 >> iPhi2 >> iz2 >> ic2 >> eic2;
// if (ic1!=ic2)
		//   std::cout<<"iEta: "<<iEta1<<" iPhi: "<<iPhi1<<" diff IC: "<<ic1-ic2<<std::endl;
		if (iz1 == -1) cont++;
	}
	std::cout << "ciao " << cont << std::endl;
// getchar();

// TApplication* theApp = new TApplication("Application",&argc, argv);



/// Set of two ic sets

	Name = Form("diffmapEB");
	TH2F * diffmap_EB = (TH2F*) map1_EB->Clone("diffmapEB");
	diffmap_EB->Reset();

	Name = Form("diffmapEEp");
	TH2F * diffmap_EEp = (TH2F*) map1_EEp->Clone("diffmapEEp");
	diffmap_EEp->Reset();

	Name = Form("diffmapEEm");
	TH2F * diffmap_EEm = (TH2F*) map1_EEm->Clone("diffmapEEm");
	diffmap_EEm->Reset();

	Name = Form("ratiomapEB");
	TH2F * ratiomap_EB = (TH2F*) map1_EB->Clone("ratiomapEB");
	ratiomap_EB->Reset();

	Name = Form("ratiomapEEp");
	TH2F * ratiomap_EEp = (TH2F*) map1_EEp->Clone("ratiomapEEp");
	ratiomap_EEp->Reset();

	Name = Form("ratiomapEEm");
	TH2F * ratiomap_EEm = (TH2F*) map1_EEm->Clone("ratiomapEEm");
	ratiomap_EEm->Reset();

	Name = Form("absolutemapEB");
	TH2F * absolutemap_EB = (TH2F*) map1_EB->Clone("absolutemapEB");
	absolutemap_EB->Reset();

	Name = Form("absolutemapEEp");
	TH2F * absolutemap_EEp = (TH2F*) map1_EEp->Clone("absolutemapEEp");
	absolutemap_EEp->Reset();

	Name = Form("absolutemapEEm");
	TH2F * absolutemap_EEm = (TH2F*) map1_EEm->Clone("absolutemapEEm");
	absolutemap_EEm->Reset();

	Name = Form("diffHistEB");
	TH1F * diffHistEB = new TH1F(Name, Name, 500, -0.3, 0.3);
	diffHistEB->SetLineWidth(2);

	Name = Form("diffHistEB_0_20");
	TH1F * diffHistEB_0_20 = new TH1F(Name, Name, 200, -1, 1);
	diffHistEB_0_20->SetLineWidth(2);

	Name = Form("diffHistEB_20_40");
	TH1F * diffHistEB_20_40 = new TH1F(Name, Name, 200, -1, 1);
	diffHistEB_20_40->SetLineWidth(2);

	Name = Form("diffHistEB_40_60");
	TH1F * diffHistEB_40_60 = new TH1F(Name, Name, 200, -1, 1);
	diffHistEB_40_60->SetLineWidth(2);

	Name = Form("diffHistEB_60_85");
	TH1F * diffHistEB_60_85 = new TH1F(Name, Name, 200, -1, 1);
	diffHistEB_60_85->SetLineWidth(2);

	Name = Form("diffHistEEp");
	TH1F * diffHistEEp = new TH1F(Name, Name, 100, -0.4, 0.4);
	diffHistEEp->SetLineWidth(2);

	Name = Form("diffHistEEp_0_5");
	TH1F * diffHistEEp_0_5 = new TH1F(Name, Name, 50, -1, 1);
	diffHistEEp_0_5->SetLineWidth(2);

	Name = Form("diffHistEEp_5_15");
	TH1F * diffHistEEp_5_15 = new TH1F(Name, Name, 50, -1, 1);
	diffHistEEp_5_15->SetLineWidth(2);

	Name = Form("diffHistEEp_15_25");
	TH1F * diffHistEEp_15_25 = new TH1F(Name, Name, 50, -1, 1);
	diffHistEEp_15_25->SetLineWidth(2);

	Name = Form("diffHistEEp_25_35");
	TH1F * diffHistEEp_25_35 = new TH1F(Name, Name, 50, -1, 1);
	diffHistEEp_25_35->SetLineWidth(2);

	Name = Form("diffHistEEm");
	TH1F * diffHistEEm = new TH1F(Name, Name, 100, -0.4, 0.4);
	diffHistEEm->SetLineWidth(2);

	Name = Form("diffHistEEm_0_5");
	TH1F * diffHistEEm_0_5 = new TH1F(Name, Name, 50, -1, 1);
	diffHistEEm_0_5->SetLineWidth(2);

	Name = Form("diffHistEEm_5_15");
	TH1F * diffHistEEm_5_15 = new TH1F(Name, Name, 50, -1, 1);
	diffHistEEm_5_15->SetLineWidth(2);

	Name = Form("diffHistEEm_15_25");
	TH1F * diffHistEEm_15_25 = new TH1F(Name, Name, 50, -1, 1);
	diffHistEEm_15_25->SetLineWidth(2);

	Name = Form("diffHistEEm_25_35");
	TH1F * diffHistEEm_25_35 = new TH1F(Name, Name, 50, -1, 1);
	diffHistEEm_25_35->SetLineWidth(2);


	Name = Form("correlationEB");
	TH2F * correlationEB = new TH2F(Name, Name, 100, 0.9, 1.1, 100, 0.9, 1.1);

	Name = Form("correlationEEp");
	TH2F * correlationEEp = new TH2F(Name, Name, 100, 0.8, 1.2, 100, 0.8, 1.2);

	Name = Form("correlationEEm");
	TH2F * correlationEEm = new TH2F(Name, Name, 100, 0.8, 1.2, 100, 0.8, 1.2);

	TEndcapRings *eRings = new TEndcapRings();


	float sumEB = 0, sum2EB = 0;
	float sumEEp = 0, sum2EEp = 0;
	float sumEEm = 0, sum2EEm = 0;

	float sumEB_0_20 = 0, sum2EB_0_20 = 0;
	float sumEB_20_40 = 0, sum2EB_20_40 = 0;
	float sumEB_40_60 = 0, sum2EB_40_60 = 0;
	float sumEB_60_85 = 0, sum2EB_60_85 = 0;

	float sumEEp_0_5 = 0, sum2EEp_0_5 = 0;
	float sumEEp_5_15 = 0, sum2EEp_5_15 = 0;
	float sumEEp_15_25 = 0, sum2EEp_15_25 = 0;
	float sumEEp_25_35 = 0, sum2EEp_25_35 = 0;

	float sumEEm_0_5 = 0, sum2EEm_0_5 = 0;
	float sumEEm_5_15 = 0, sum2EEm_5_15 = 0;
	float sumEEm_15_25 = 0, sum2EEm_15_25 = 0;
	float sumEEm_25_35 = 0, sum2EEm_25_35 = 0;

	for(int iEta = 1; iEta < map1_EB->GetNbinsY() + 1; iEta++) {
		if (iEta == 86) continue;
		for(int iPhi = 1; iPhi < map1_EB->GetNbinsX() + 1; iPhi++) {

			int eta = iEta - 86;
			if(map1_EB->GetBinContent(iPhi, iEta) == -1. || map2_EB->GetBinContent(iPhi, iEta) == -1.) {
				diffmap_EB->SetBinContent(iPhi, iEta, -1.);
				ratiomap_EB->SetBinContent(iPhi, iEta, -1.);
				absolutemap_EB->SetBinContent(iPhi, iEta, map1_EB->GetBinContent(iPhi, iEta));
				FileRatio << eta << "\t" << iPhi << "\t" << 0 << "\t" << -1. << "\t" << -999. << std::endl;
				FileAbsolute << eta << "\t" << iPhi << "\t" << 0 << "\t" << map1_EB->GetBinContent(iPhi, iEta) << "\t" << -999. << std::endl;
				FileMiscalib << eta << "\t" << iPhi << "\t" << 0 << "\t" << map1_EB->GetBinContent(iPhi, iEta) << "\t" << -999. << std::endl;

				continue;
			}

			sumEB += map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta);
			sum2EB += (map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta)) * (map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta));
			if (abs(eta) < 20) {
				sumEB_0_20 += map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta);
				sum2EB_0_20 += (map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta)) * (map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta));
				diffHistEB_0_20->Fill(map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta));
			} else if (abs(eta) < 40) {
				sumEB_20_40 += map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta);
				sum2EB_20_40 += (map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta)) * (map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta));
				diffHistEB_20_40->Fill(map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta));
			} else if (abs(eta) < 60) {
				sumEB_40_60 += map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta);
				sum2EB_40_60 += (map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta)) * (map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta));
				diffHistEB_40_60->Fill(map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta));
			} else {
				sumEB_60_85 += map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta);
				sum2EB_60_85 += (map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta)) * (map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta));
				diffHistEB_60_85->Fill(map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta));
			}

			diffmap_EB->SetBinContent(iPhi, iEta, map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta));
			diffHistEB->Fill(map1_EB->GetBinContent(iPhi, iEta) - map2_EB->GetBinContent(iPhi, iEta));
			ratiomap_EB->SetBinContent(iPhi, iEta, map1_EB->GetBinContent(iPhi, iEta) / map2_EB->GetBinContent(iPhi, iEta));
			absolutemap_EB->SetBinContent(iPhi, iEta, map1_EB->GetBinContent(iPhi, iEta)*map2_EB->GetBinContent(iPhi, iEta));
			FileRatio << eta << "\t" << iPhi << "\t" << 0 << "\t" << map1_EB->GetBinContent(iPhi, iEta) / map2_EB->GetBinContent(iPhi, iEta) << "\t" << -999. << std::endl;
			FileAbsolute << eta << "\t" << iPhi << "\t" << 0 << "\t" << map1_EB->GetBinContent(iPhi, iEta)*map2_EB->GetBinContent(iPhi, iEta) << "\t" << -999. << std::endl;
			correlationEB->Fill(map1_EB->GetBinContent(iPhi, iEta), map2_EB->GetBinContent(iPhi, iEta));

			if ( (iPhi > 20 && iPhi < 40) || (iPhi > 60 && iPhi < 80) || (iPhi > 100 && iPhi < 120) || (iPhi > 140 && iPhi < 160))
				FileMiscalib << eta << "\t" << iPhi << "\t" << 0 << "\t" << map1_EB->GetBinContent(iPhi, iEta) * 1.3 << "\t" << -999. << std::endl;
			else if ((iPhi > 200 && iPhi < 240) || (iPhi > 280 && iPhi < 300) || (iPhi > 340 && iPhi < 360))
				FileMiscalib << eta << "\t" << iPhi << "\t" << 0 << "\t" << map1_EB->GetBinContent(iPhi, iEta) * 0.7 << "\t" << -999. << std::endl;
			else
				FileMiscalib << eta << "\t" << iPhi << "\t" << 0 << "\t" << map1_EB->GetBinContent(iPhi, iEta) << "\t" << -999. << std::endl;

		}
	}


	for(int ix = 1; ix < map1_EEp->GetNbinsX() + 1; ix++) {
		for(int iy = 1; iy < map1_EEp->GetNbinsY() + 1; iy++) {

			if(map1_EEp->GetBinContent(ix, iy) == 0 || map2_EEp->GetBinContent(ix, iy) == 0) continue;

			if(map1_EEp->GetBinContent(ix, iy) == -1. || map2_EEp->GetBinContent(ix, iy) == -1.) {
				diffmap_EEp->SetBinContent(ix, iy, -1.);
				ratiomap_EEp->SetBinContent(ix, iy, -1.);
				absolutemap_EEp->SetBinContent(ix, iy, map1_EEp->GetBinContent(ix, iy));
				FileRatio << ix << "\t" << iy << "\t" << 1 << "\t" << -1. << "\t" << -999. << std::endl;
				FileAbsolute << ix << "\t" << iy << "\t" << 1 << "\t" << map1_EEp->GetBinContent(ix, iy) << "\t" << -999. << std::endl;
				FileMiscalib << ix << "\t" << iy << "\t" << 1 << "\t" << map1_EEp->GetBinContent(ix, iy) << "\t" << -999. << std::endl;

				continue;
			}

			sumEEp += map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy);
			sum2EEp += (map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy)) * (map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy));

			int iEta = int(eRings->GetEndcapIeta(ix, iy, 1)) - 86;

			if (abs(iEta) < 5) {
				sumEEp_0_5 += map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy);
				sum2EEp_0_5 += (map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy)) * (map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy));
				diffHistEEp_0_5->Fill(map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy));
			} else if (abs(iEta) < 15) {
				sumEEp_5_15 += map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy);
				sum2EEp_5_15 += (map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy)) * (map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy));
				diffHistEEp_5_15->Fill(map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy));
			} else if (abs(iEta) < 25) {
				sumEEp_15_25 += map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy);
				sum2EEp_15_25 += (map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy)) * (map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy));
				diffHistEEp_15_25->Fill(map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy));
			} else {
				sumEEp_25_35 += map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy);
				sum2EEp_25_35 += (map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy)) * (map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy));
				diffHistEEp_25_35->Fill(map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy));
			}

			diffmap_EEp->SetBinContent(ix, iy, map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy));
			diffHistEEp->Fill(map1_EEp->GetBinContent(ix, iy) - map2_EEp->GetBinContent(ix, iy));
			ratiomap_EEp->SetBinContent(ix, iy, map1_EEp->GetBinContent(ix, iy) / map2_EEp->GetBinContent(ix, iy));
			absolutemap_EEp->SetBinContent(ix, iy, map1_EEp->GetBinContent(ix, iy)*map2_EEp->GetBinContent(ix, iy));
			FileRatio << ix << "\t" << iy << "\t" << 1 << "\t" << map1_EEp->GetBinContent(ix, iy) / map2_EEp->GetBinContent(ix, iy) << "\t" << -999. << std::endl;
			FileAbsolute << ix << "\t" << iy << "\t" << 1 << "\t" << map1_EEp->GetBinContent(ix, iy)*map2_EEp->GetBinContent(ix, iy) << "\t" << -999. << std::endl;
			correlationEEp->Fill(map1_EEp->GetBinContent(ix, iy), map2_EEp->GetBinContent(ix, iy));

			if ( (ix > 20 && ix < 30) || (ix > 50 && ix < 55) || (ix > 80 && ix < 90))
				FileMiscalib << ix << "\t" << iy << "\t" << 1 << "\t" << map1_EEp->GetBinContent(ix, iy) * 1.3 << "\t" << -999. << std::endl;
			else if ((ix > 5 && ix < 15) || (ix > 40 && ix < 45) || (ix > 60 && ix < 70))
				FileMiscalib << ix << "\t" << iy << "\t" << 1 << "\t" << map1_EEp->GetBinContent(ix, iy) * 0.7 << "\t" << -999. << std::endl;
			else
				FileMiscalib << ix << "\t" << iy << "\t" << 1 << "\t" << map1_EEp->GetBinContent(ix, iy) << "\t" << -999. << std::endl;

		}
	}



	for(int ix = 1; ix < map1_EEm->GetNbinsX() + 1; ix++) {
		for(int iy = 1; iy < map1_EEm->GetNbinsY() + 1; iy++) {

			if(map1_EEm->GetBinContent(ix, iy) == 0 || map2_EEm->GetBinContent(ix, iy) == 0) continue;

			if(map1_EEm->GetBinContent(ix, iy) == -1. || map2_EEm->GetBinContent(ix, iy) == -1.) {
				diffmap_EEm->SetBinContent(ix, iy, -1.);
				ratiomap_EEm->SetBinContent(ix, iy, -1.);
				absolutemap_EEm->SetBinContent(ix, iy, map1_EEm->GetBinContent(ix, iy));
				FileRatio << ix << "\t" << iy << "\t" << -1 << "\t" << map1_EEm->GetBinContent(ix, iy) << "\t" << -999. << std::endl;
				FileAbsolute << ix << "\t" << iy << "\t" << -1 << "\t" << -1. << "\t" << -999. << std::endl;
				continue;
			}

			sumEEm += map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy);
			sum2EEm += (map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy)) * (map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy));

			int iEta = int(eRings->GetEndcapIeta(ix, iy, 1)) - 86;

			if (abs(iEta) < 5) {
				sumEEm_0_5 += map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy);
				sum2EEm_0_5 += (map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy)) * (map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy));
				diffHistEEm_0_5->Fill(map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy));
			} else if (abs(iEta) < 15) {
				sumEEm_5_15 += map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy);
				sum2EEm_5_15 += (map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy)) * (map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy));
				diffHistEEm_5_15->Fill(map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy));
			} else if (abs(iEta) < 25) {
				sumEEm_15_25 += map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy);
				sum2EEm_15_25 += (map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy)) * (map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy));
				diffHistEEm_15_25->Fill(map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy));
			} else {
				sumEEm_25_35 += map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy);
				sum2EEm_25_35 += (map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy)) * (map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy));
				diffHistEEm_25_35->Fill(map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy));
			}

			diffmap_EEm->SetBinContent(ix, iy, map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy));
			diffHistEEm->Fill(map1_EEm->GetBinContent(ix, iy) - map2_EEm->GetBinContent(ix, iy));
			ratiomap_EEm->SetBinContent(ix, iy, map1_EEm->GetBinContent(ix, iy) / map2_EEm->GetBinContent(ix, iy));
			absolutemap_EEm->SetBinContent(ix, iy, map1_EEm->GetBinContent(ix, iy)*map2_EEm->GetBinContent(ix, iy));
			FileRatio << ix << "\t" << iy << "\t" << -1 << "\t" << map1_EEm->GetBinContent(ix, iy) / map2_EEm->GetBinContent(ix, iy) << "\t" << -999. << std::endl;
			FileAbsolute << ix << "\t" << iy << "\t" << -1 << "\t" << map1_EEm->GetBinContent(ix, iy)*map2_EEm->GetBinContent(ix, iy) << "\t" << -999. << std::endl;
			correlationEEm->Fill(map1_EEm->GetBinContent(ix, iy), map2_EEm->GetBinContent(ix, iy));

			if ( (ix > 20 && ix < 30) || (ix > 50 && ix < 55) || (ix > 80 && ix < 90))
				FileMiscalib << ix << "\t" << iy << "\t" << -1 << "\t" << map1_EEm->GetBinContent(ix, iy) * 1.3 << "\t" << -999. << std::endl;
			else if ((ix > 5 && ix < 15) || (ix > 40 && ix < 45) || (ix > 60 && ix < 70))
				FileMiscalib << ix << "\t" << iy << "\t" << -1 << "\t" << map1_EEm->GetBinContent(ix, iy) * 0.7 << "\t" << -999. << std::endl;
			else
				FileMiscalib << ix << "\t" << iy << "\t" << -1 << "\t" << map1_EEm->GetBinContent(ix, iy) << "\t" << -999. << std::endl;

		}
	}



	TF1 *fgaus2 = new TF1("fgaus2", "gaus", -0.02, 0.02);

	std::cout << "FIT: " << std::endl;

	fgaus2->SetParameter(1, 0);
	fgaus2->SetParameter(2, diffHistEB->GetRMS());
//fgaus2->SetRange(0-0.5*diffHistEB->GetRMS(),0+0.5*diffHistEB->GetRMS());
	fgaus2->SetRange(-0.03, 0.03);
	fgaus2->SetLineColor(kRed);
//diffHistEB->Fit("fgaus2","QRME");
	cout << " EB-Mean: " << fgaus2->GetParameter(1) << " RMSFit: " << fgaus2->GetParameter(2) << endl; //<<" chi2/ndf: "<<fgaus2->GetChisquare()/fgaus2->GetNDF()<<endl;
	diffHistEB_0_20->Fit("fgaus2", "QRME");
	cout << " EB_0_20-Mean: " << fgaus2->GetParameter(1) << " RMSFit: " << fgaus2->GetParameter(2) << endl; //<<" chi2/ndf: "<<fgaus2->GetChisquare()/fgaus2->GetNDF()<<endl;
	diffHistEB_20_40->Fit("fgaus2", "QRME");
	cout << " EB_20_40-Mean: " << fgaus2->GetParameter(1) << " RMSFit: " << fgaus2->GetParameter(2) << endl; //<<" chi2/ndf: "<<fgaus2->GetChisquare()/fgaus2->GetNDF()<<endl;
	diffHistEB_40_60->Fit("fgaus2", "QRME");
	cout << " EB_40_60-Mean: " << fgaus2->GetParameter(1) << " RMSFit: " << fgaus2->GetParameter(2) << endl; //<<" chi2/ndf: "<<fgaus2->GetChisquare()/fgaus2->GetNDF()<<endl;
	diffHistEB_60_85->Fit("fgaus2", "QRME");
	cout << " EB_60_85-Mean: " << fgaus2->GetParameter(1) << " RMSFit: " << fgaus2->GetParameter(2) << endl; //<<" chi2/ndf: "<<fgaus2->GetChisquare()/fgaus2->GetNDF()<<endl;


	fgaus2->SetParameter(1, 0);
	fgaus2->SetParameter(2, diffHistEEp->GetRMS());
	fgaus2->SetRange(-0.1, 0.08);
//fgaus2->SetRange(0-0.5*diffHistEEp->GetRMS(),0+0.5*diffHistEEp->GetRMS());
	fgaus2->SetLineColor(kRed);
//diffHistEEp->Fit("fgaus2","QRME");
	cout << " EEp-Mean: " << fgaus2->GetParameter(1) << " RMSFit: " << fgaus2->GetParameter(2) << endl; //<<" chi2/ndf: "<<fgaus2->GetChisquare()/fgaus2->GetNDF()<<endl;
	diffHistEEp_0_5->Fit("fgaus2", "QRME");
	cout << " EEp_0_5-Mean: " << fgaus2->GetParameter(1) << " RMSFit: " << fgaus2->GetParameter(2) << endl; //<<" chi2/ndf: "<<fgaus2->GetChisquare()/fgaus2->GetNDF()<<endl;
	diffHistEEp_5_15->Fit("fgaus2", "QRME");
	cout << " EEp_5_15-Mean: " << fgaus2->GetParameter(1) << " RMSFit: " << fgaus2->GetParameter(2) << endl; //<<" chi2/ndf: "<<fgaus2->GetChisquare()/fgaus2->GetNDF()<<endl;
	diffHistEEp_15_25->Fit("fgaus2", "QRME");
	cout << " EEp_15_25-Mean: " << fgaus2->GetParameter(1) << " RMSFit: " << fgaus2->GetParameter(2) << endl; //<<" chi2/ndf: "<<fgaus2->GetChisquare()/fgaus2->GetNDF()<<endl;
	diffHistEEp_25_35->Fit("fgaus2", "QRME");
	cout << " EEp_25_35-Mean: " << fgaus2->GetParameter(1) << " RMSFit: " << fgaus2->GetParameter(2) << endl; //<<" chi2/ndf: "<<fgaus2->GetChisquare()/fgaus2->GetNDF()<<endl;


	fgaus2->SetParameter(1, 0);
	fgaus2->SetParameter(2, diffHistEEm->GetRMS());
	fgaus2->SetRange(-0.1, 0.08);
//fgaus2->SetRange(0-0.5*diffHistEEm->GetRMS(),0+0.5*diffHistEEm->GetRMS());
	fgaus2->SetLineColor(kRed);
	fgaus2->SetLineColor(kRed);
//diffHistEEm->Fit("fgaus2","QRME");
	cout << " EEm-Mean: " << fgaus2->GetParameter(1) << " RMSFit: " << fgaus2->GetParameter(2) << endl; //<<" chi2/ndf: "<<fgaus2->GetChisquare()/fgaus2->GetNDF()<<endl;
	diffHistEEm_0_5->Fit("fgaus2", "QRME");
	cout << " EEm_0_5-Mean: " << fgaus2->GetParameter(1) << " RMSFit: " << fgaus2->GetParameter(2) << endl; //<<" chi2/ndf: "<<fgaus2->GetChisquare()/fgaus2->GetNDF()<<endl;
	diffHistEEm_5_15->Fit("fgaus2", "QRME");
	cout << " EEm_5_15-Mean: " << fgaus2->GetParameter(1) << " RMSFit: " << fgaus2->GetParameter(2) << endl; //<<" chi2/ndf: "<<fgaus2->GetChisquare()/fgaus2->GetNDF()<<endl;
	diffHistEEm_15_25->Fit("fgaus2", "QRME");
	cout << " EEm_15_25-Mean: " << fgaus2->GetParameter(1) << " RMSFit: " << fgaus2->GetParameter(2) << endl; //<<" chi2/ndf: "<<fgaus2->GetChisquare()/fgaus2->GetNDF()<<endl;
	diffHistEEm_25_35->Fit("fgaus2", "QRME");
	cout << " EEm_25_35-Mean: " << fgaus2->GetParameter(1) << " RMSFit: " << fgaus2->GetParameter(2) << endl; //<<" chi2/ndf: "<<fgaus2->GetChisquare()/fgaus2->GetNDF()<<endl;


	cout << "RMS:" << endl;
	cout << " EB-Meandist: " << diffHistEB->GetMean() << " RMSdist " << diffHistEB->GetRMS() << endl;
	cout << " EB_0_20-Meandist: " << diffHistEB_0_20->GetMean() << " RMSdist " << diffHistEB_0_20->GetRMS() << endl;
	cout << " EB_20_40-Meandist: " << diffHistEB_20_40->GetMean() << " RMSdist " << diffHistEB_20_40->GetRMS() << endl;
	cout << " EB_40_60-Meandist: " << diffHistEB_40_60->GetMean() << " RMSdist " << diffHistEB_40_60->GetRMS() << endl;
	cout << " EB_60_85-Meandist: " << diffHistEB_60_85->GetMean() << " RMSdist " << diffHistEB_60_85->GetRMS() << endl;

	cout << " EEp-Meandist: " << diffHistEEp->GetMean() << " RMSdist " << diffHistEEp->GetRMS() << endl;
	cout << " EEp_0_5-Meandist: " << diffHistEEp_0_5->GetMean() << " RMSdist " << diffHistEEp_0_5->GetRMS() << endl;
	cout << " EEp_5_15-Meandist: " << diffHistEEp_5_15->GetMean() << " RMSdist " << diffHistEEp_5_15->GetRMS() << endl;
	cout << " EEp_15_25-Meandist: " << diffHistEEp_15_25->GetMean() << " RMSdist " << diffHistEEp_15_25->GetRMS() << endl;
	cout << " EEp_25_35-Meandist: " << diffHistEEp_25_35->GetMean() << " RMSdist " << diffHistEEp_25_35->GetRMS() << endl;

	cout << " EEm-Meandist: " << diffHistEEm->GetMean() << " RMSdist " << diffHistEEm->GetRMS() << endl;
	cout << " EEm_0_5-Meandist: " << diffHistEEm_0_5->GetMean() << " RMSdist " << diffHistEEm_0_5->GetRMS() << endl;
	cout << " EEm_5_15-Meandist: " << diffHistEEm_5_15->GetMean() << " RMSdist " << diffHistEEm_5_15->GetRMS() << endl;
	cout << " EEm_15_25-Meandist: " << diffHistEEm_15_25->GetMean() << " RMSdist " << diffHistEEm_15_25->GetRMS() << endl;
	cout << " EEm_25_35-Meandist: " << diffHistEEm_25_35->GetMean() << " RMSdist " << diffHistEEm_25_35->GetRMS() << endl;



	std::cout << "SOMME: " << std::endl;
	std::cout << "EB: " << sumEB << " EB2: " << sum2EB << std::endl;
	std::cout << "EE+: " << sumEEp << " EE+2: " << sum2EEp << std::endl;
	std::cout << "EE-: " << sumEEm << " EE-2: " << sum2EEm << std::endl;

	std::cout << "EB_0_20: " << sumEB_0_20 << " EB2_0_20: " << sum2EB_0_20 << std::endl;
	std::cout << "EB_20_40: " << sumEB_20_40 << " EB2_20_40: " << sum2EB_20_40 << std::endl;
	std::cout << "EB_40_60: " << sumEB_40_60 << " EB2_40_60: " << sum2EB_40_60 << std::endl;
	std::cout << "EB_60_85: " << sumEB_60_85 << " EB2_60_85: " << sum2EB_60_85 << std::endl;

	std::cout << "EE+_0_5: " << sumEEp_0_5 << " EE+2_0_5: " << sum2EEp_0_5 << std::endl;
	std::cout << "EE+_5_15: " << sumEEp_5_15 << " EE+2_5_15: " << sum2EEp_5_15 << std::endl;
	std::cout << "EE+_15_25: " << sumEEp_15_25 << " EE+2_15_25: " << sum2EEp_15_25 << std::endl;
	std::cout << "EE+_25_35: " << sumEEp_25_35 << " EE+2_25_35: " << sum2EEp_25_35 << std::endl;

	std::cout << "EE-_0_5: " << sumEEm_0_5 << " EE-2_0_5: " << sum2EEm_0_5 << std::endl;
	std::cout << "EE-_5_15: " << sumEEm_5_15 << " EE-2_5_15: " << sum2EEm_5_15 << std::endl;
	std::cout << "EE-_15_25: " << sumEEm_15_25 << " EE-2_15_25: " << sum2EEm_15_25 << std::endl;
	std::cout << "EE-_25_35: " << sumEEm_25_35 << " EE-2_25_35: " << sum2EEm_25_35 << std::endl;

/// Profile along phi  for EB:

	TGraphErrors *phiProjectionEB1 = new TGraphErrors();
	phiProjectionEB1->SetMarkerStyle(20);
	phiProjectionEB1->SetMarkerSize(1);
	phiProjectionEB1->SetMarkerColor(kBlue);

	TGraphErrors *phiProjectionEB2 = new TGraphErrors();
	phiProjectionEB2->SetMarkerStyle(20);
	phiProjectionEB2->SetMarkerSize(1);
	phiProjectionEB2->SetMarkerColor(kRed);


	for(int iPhi = 1; iPhi < map1_EB->GetNbinsX() + 1; iPhi++) {
		double sumEta = 0, nEta = 0;

		for(int iEta = 1; iEta < map1_EB->GetNbinsY() + 1; iEta++) {
			if(map1_EB->GetBinContent(iPhi, iEta) == -1. || map1_EB->GetBinContent(iPhi, iEta) == 0.) continue;
			sumEta = sumEta + map1_EB->GetBinContent(iPhi, iEta);
			nEta++;
		}
		phiProjectionEB1->SetPoint(iPhi - 1, iPhi - 1, sumEta / nEta);
		phiProjectionEB1->SetPointError(iPhi - 1, 0., 0.002);
	}


	for(int iPhi = 1; iPhi < map2_EB->GetNbinsX() + 1; iPhi++) {
		double sumEta = 0, nEta = 0;

		for(int iEta = 1; iEta < map2_EB->GetNbinsY() + 1; iEta++) {
			if(map2_EB->GetBinContent(iPhi, iEta) == -1. || map2_EB->GetBinContent(iPhi, iEta) == 0. ) continue;
			sumEta = sumEta + map2_EB->GetBinContent(iPhi, iEta);
			nEta++;
		}
		phiProjectionEB2->SetPoint(iPhi - 1, iPhi - 1, sumEta / nEta);
		phiProjectionEB2->SetPointError(iPhi - 1, 0., 0.002);
	}


/// Profile along phi  for EE+:

	std::vector<double> vectSum;
	std::vector<double> vectCounter;

	vectCounter.assign(360, 0.);
	vectSum.assign(360, 0.);


	TGraphErrors *phiProjectionEEp1 = new TGraphErrors();
	phiProjectionEEp1->SetMarkerStyle(20);
	phiProjectionEEp1->SetMarkerSize(1);
	phiProjectionEEp1->SetMarkerColor(kBlue);

	TGraphErrors *phiProjectionEEp2 = new TGraphErrors();
	phiProjectionEEp2->SetMarkerStyle(20);
	phiProjectionEEp2->SetMarkerSize(1);
	phiProjectionEEp2->SetMarkerColor(kRed);

	for(int ix = 1; ix < map1_EEp->GetNbinsX() + 1; ix++) {
		for(int iy = 1; iy < map1_EEp->GetNbinsY() + 1; iy++) {
			if(map1_EEp->GetBinContent(ix, iy) == -1. || map1_EEp->GetBinContent(ix, iy) == 0. ) continue;
			int iPhi = int(eRings->GetEndcapIphi(ix, iy, 1));
			if (iPhi == 360) iPhi = 0;
			vectSum.at(iPhi) = vectSum.at(iPhi) + map1_EEp->GetBinContent(ix, iy);
			vectCounter.at(iPhi) = vectCounter.at(iPhi) + 1;
		}
	}


	int j = 0;
	for(unsigned int i = 0; i < vectCounter.size(); i++) {
		if(vectCounter.at(i) == 0)continue;
		phiProjectionEEp1->SetPoint(j, i, vectSum.at(i) / vectCounter.at(i));
		j++;
	}

	for(unsigned int i = 0; i < vectSum.size(); i++) {
		vectSum.at(i) = 0;
		vectCounter.at(i) = 0;
	}



	for(int ix = 1; ix < map2_EEp->GetNbinsX() + 1; ix++) {
		for(int iy = 1; iy < map2_EEp->GetNbinsY() + 1; iy++) {
			if(map2_EEp->GetBinContent(ix, iy) == -1. || map2_EEp->GetBinContent(ix, iy) == 0.) continue;
			int iPhi = int(eRings->GetEndcapIphi(ix, iy, 1));
			if (iPhi == 360) iPhi = 0;
			vectSum.at(iPhi) = vectSum.at(iPhi) + map2_EEp->GetBinContent(ix, iy);
			vectCounter.at(iPhi) = vectCounter.at(iPhi) + 1;
		}
	}


	j = 0;
	for(unsigned int i = 0; i < vectCounter.size(); i++) {
		if(vectCounter.at(i) == 0)continue;
		phiProjectionEEp2->SetPoint(j, i, vectSum.at(i) / vectCounter.at(i));
		j++;
	}

	for(unsigned int i = 0; i < vectSum.size(); i++) {
		vectSum.at(i) = 0;
		vectCounter.at(i) = 0;
	}


/// Profile along phi  for EE-:

	TGraphErrors *phiProjectionEEm1 = new TGraphErrors();
	phiProjectionEEm1->SetMarkerStyle(20);
	phiProjectionEEm1->SetMarkerSize(1);
	phiProjectionEEm1->SetMarkerColor(kBlue);

	TGraphErrors *phiProjectionEEm2 = new TGraphErrors();
	phiProjectionEEm2->SetMarkerStyle(20);
	phiProjectionEEm2->SetMarkerSize(1);
	phiProjectionEEm2->SetMarkerColor(kRed);

	for(int ix = 1; ix < map1_EEm->GetNbinsX() + 1; ix++) {
		for(int iy = 1; iy < map1_EEm->GetNbinsY() + 1; iy++) {
			if(map1_EEm->GetBinContent(ix, iy) == -1. || map1_EEm->GetBinContent(ix, iy) == 0. ) continue;
			int iPhi = int(eRings->GetEndcapIphi(ix, iy, -1));
			if (iPhi == 360) iPhi = 0;
			vectSum.at(iPhi) = vectSum.at(iPhi) + map1_EEm->GetBinContent(ix, iy);
			vectCounter.at(iPhi) = vectCounter.at(iPhi) + 1;
		}
	}


	j = 0;
	for(unsigned int i = 0; i < vectCounter.size(); i++) {
		if(vectCounter.at(i) == 0)continue;
		phiProjectionEEm1->SetPoint(j, i, vectSum.at(i) / vectCounter.at(i));
		j++;
	}
	for(unsigned int i = 0; i < vectSum.size(); i++) {
		vectSum.at(i) = 0;
		vectCounter.at(i) = 0;
	}


	for(int ix = 1; ix < map2_EEm->GetNbinsX() + 1; ix++) {
		for(int iy = 1; iy < map2_EEm->GetNbinsY() + 1; iy++) {
			if(map2_EEm->GetBinContent(ix, iy) == -1. ||  map2_EEm->GetBinContent(ix, iy) == 0.) continue;
			int iPhi = int(eRings->GetEndcapIphi(ix, iy, -1));
			if (iPhi == 360) iPhi = 0;
			vectSum.at(iPhi) = vectSum.at(iPhi) + map2_EEm->GetBinContent(ix, iy);
			vectCounter.at(iPhi) = vectCounter.at(iPhi) + 1;
		}
	}


	j = 0;
	for(unsigned int i = 0; i < vectCounter.size(); i++) {
		if(vectCounter.at(i) == 0)continue;
		phiProjectionEEm2->SetPoint(j, i, vectSum.at(i) / vectCounter.at(i));
		j++;
	}

	for(unsigned int i = 0; i < vectSum.size(); i++) {
		vectSum.at(i) = 0;
		vectCounter.at(i) = 0;
	}



/// projection along eta for EB

	TGraphErrors *etaProjectionEB1 = new TGraphErrors();
	etaProjectionEB1->SetMarkerStyle(20);
	etaProjectionEB1->SetMarkerSize(1);
	etaProjectionEB1->SetMarkerColor(kBlue);

	TGraphErrors *etaProjectionEB2 = new TGraphErrors();
	etaProjectionEB2->SetMarkerStyle(20);
	etaProjectionEB2->SetMarkerSize(1);
	etaProjectionEB2->SetMarkerColor(kRed);


	for(int iEta = 1; iEta < map1_EB->GetNbinsY() + 1; iEta++) {
		double sumPhi = 0, nPhi = 0;

		for(int iPhi = 1; iPhi < map1_EB->GetNbinsX() + 1; iPhi++) {
			if(map1_EB->GetBinContent(iPhi, iEta) == -1.) continue;
			sumPhi = sumPhi + map1_EB->GetBinContent(iPhi, iEta);
			nPhi++;
		}
		etaProjectionEB1->SetPoint(iEta - 1, iEta - 1, sumPhi / nPhi);
		etaProjectionEB1->SetPointError(iEta - 1, 0., 0.002);
	}


	for(int iEta = 1; iEta < map2_EB->GetNbinsY() + 1; iEta++) {
		double sumPhi = 0, nPhi = 0;

		for(int iPhi = 1; iPhi < map2_EB->GetNbinsX() + 1; iPhi++) {
			if(map2_EB->GetBinContent(iPhi, iEta) == -1.) continue;
			sumPhi = sumPhi + map2_EB->GetBinContent(iPhi, iEta);
			nPhi++;
		}
		etaProjectionEB2->SetPoint(iEta - 1, iEta - 1, sumPhi / nPhi);
		etaProjectionEB2->SetPointError(iEta - 1, 0., 0.002);
	}



/// projection along eta for EE+:

	TGraphErrors *etaProjectionEEp1 = new TGraphErrors();
	etaProjectionEEp1->SetMarkerStyle(20);
	etaProjectionEEp1->SetMarkerSize(1);
	etaProjectionEEp1->SetMarkerColor(kBlue);

	TGraphErrors *etaProjectionEEp2 = new TGraphErrors();
	etaProjectionEEp2->SetMarkerStyle(20);
	etaProjectionEEp2->SetMarkerSize(1);
	etaProjectionEEp2->SetMarkerColor(kRed);

	vectCounter.clear();
	vectSum.clear();
	vectCounter.assign(360, 0.);
	vectSum.assign(360, 0.);

	for(int ix = 1; ix < map1_EEp->GetNbinsX() + 1; ix++) {
		for(int iy = 1; iy < map1_EEp->GetNbinsY() + 1; iy++) {
			if(map1_EEp->GetBinContent(ix, iy) == -1. || map1_EEp->GetBinContent(ix, iy) == 0.) continue;
			int iEta = int(eRings->GetEndcapIeta(ix, iy, 1));
			if(iEta < 0 || iEta > 360)continue;
			vectSum.at(iEta) = vectSum.at(iEta) + map1_EEp->GetBinContent(ix, iy);
			vectCounter.at(iEta) = vectCounter.at(iEta) + 1;
		}
	}

	j = 0;
	for(unsigned int i = 0; i < vectCounter.size(); i++) {
		if(vectCounter.at(i) == 0) continue;
		etaProjectionEEp1->SetPoint(j, i, vectSum.at(i) / vectCounter.at(i));
		j++;
	}


	for(unsigned int i = 0; i < vectSum.size(); i++) {
		vectSum.at(i) = 0;
		vectCounter.at(i) = 0;
	}


	for(int ix = 1; ix < map2_EEp->GetNbinsX() + 1; ix++) {
		for(int iy = 1; iy < map2_EEp->GetNbinsY() + 1; iy++) {
			if(map2_EEp->GetBinContent(ix, iy) == -1. || map2_EEp->GetBinContent(ix, iy) == 0. ) continue;
			int iEta = int(eRings->GetEndcapIeta(ix, iy, 1));
			if(iEta < 0 || iEta > 360)continue;
			vectSum.at(iEta) = vectSum.at(iEta) + map2_EEp->GetBinContent(ix, iy);
			vectCounter.at(iEta) = vectCounter.at(iEta) + 1;
		}
	}


	j = 0;
	for(unsigned int i = 0; i < vectCounter.size(); i++) {
		if(vectCounter.at(i) == 0) continue;
		etaProjectionEEp2->SetPoint(j, i, vectSum.at(i) / vectCounter.at(i));
		j++;
	}


	for(unsigned int i = 0; i < vectSum.size(); i++) {
		vectSum.at(i) = 0;
		vectCounter.at(i) = 0;
	}


/// projection along eta for EE-:

	TGraphErrors *etaProjectionEEm1 = new TGraphErrors();
	etaProjectionEEm1->SetMarkerStyle(20);
	etaProjectionEEm1->SetMarkerSize(1);
	etaProjectionEEm1->SetMarkerColor(kBlue);

	TGraphErrors *etaProjectionEEm2 = new TGraphErrors();
	etaProjectionEEm2->SetMarkerStyle(20);
	etaProjectionEEm2->SetMarkerSize(1);
	etaProjectionEEm2->SetMarkerColor(kRed);

	for(int ix = 1; ix < map1_EEm->GetNbinsX() + 1; ix++) {
		for(int iy = 1; iy < map1_EEm->GetNbinsY() + 1; iy++) {
			if(map1_EEm->GetBinContent(ix, iy) == -1. || map1_EEm->GetBinContent(ix, iy) == 0.) continue;
			int iEta = int(eRings->GetEndcapIeta(ix, iy, 1));
			if(iEta < 0 || iEta > 360)continue;
			vectSum.at(iEta) = vectSum.at(iEta) + map1_EEm->GetBinContent(ix, iy);
			vectCounter.at(iEta) = vectCounter.at(iEta) + 1;
		}
	}


	j = 0;
	for(unsigned int i = 0; i < vectCounter.size(); i++) {
		if(vectCounter.at(i) == 0)continue;
		etaProjectionEEm1->SetPoint(j, i, vectSum.at(i) / vectCounter.at(i));
		j++;
	}

	for(unsigned int i = 0; i < vectSum.size(); i++) {
		vectSum.at(i) = 0;
		vectCounter.at(i) = 0;
	}


	for(int ix = 1; ix < map2_EEm->GetNbinsX() + 1; ix++) {
		for(int iy = 1; iy < map2_EEm->GetNbinsY() + 1; iy++) {
			if(map2_EEm->GetBinContent(ix, iy) == -1. || map2_EEm->GetBinContent(ix, iy) == 0.) continue;
			int iEta = int(eRings->GetEndcapIeta(ix, iy, 1));
			if(iEta < 0 || iEta > 171)continue;
			vectSum.at(iEta) = vectSum.at(iEta) + map2_EEm->GetBinContent(ix, iy);
			vectCounter.at(iEta) = vectCounter.at(iEta) + 1;
		}
	}


	j = 0;
	for(unsigned int i = 0; i < vectCounter.size(); i++) {
		if(vectCounter.at(i) == 0)continue;
		etaProjectionEEm2->SetPoint(j, i, vectSum.at(i) / vectCounter.at(i));
		j++;
	}

	for(unsigned int i = 0; i < vectSum.size(); i++) {
		vectSum.at(i) = 0;
		vectCounter.at(i) = 0;
	}



///  phi Profile Histos EB

	cout << " Phi Profile Histos " << endl;

	TH1F* phiProfileEB1 = new TH1F ("phiProfileEB1", "phiProfileEB1", 60, 0.7, 1.3);
	TH1F* phiProfileEB2 = new TH1F ("phiProfileEB2", "phiProfileEB2", 60, 0.7, 1.3);

	for(int i = 0; i < phiProjectionEB1->GetN() ; i++) {
		double x = 0, y = 0;
		phiProjectionEB1->GetPoint(i, x, y);
		phiProfileEB1->Fill(y);
	}

	for(int i = 0; i < phiProjectionEB2->GetN() ; i++) {
		double x = 0, y = 0;
		phiProjectionEB2->GetPoint(i, x, y);
		phiProfileEB2->Fill(y);
	}

	TF1 *fgaus = new TF1("fgaus", "gaus", -10, 10);

	fgaus->SetParameter(1, 1);
	fgaus->SetParameter(2, phiProfileEB1->GetRMS());
	fgaus->SetRange(1 - 5 * phiProfileEB1->GetRMS(), 1 + 5 * phiProfileEB1->GetRMS());
	fgaus->SetLineColor(kBlue);
	phiProfileEB1->Fit("fgaus", "QRME");
	cout << " First Set :  Mean Fit = " << fgaus->GetParameter(1) << " RMS Fit = " << fgaus->GetParameter(2) << " chi2/ndf = " << fgaus->GetChisquare() / fgaus->GetNDF() << endl;
	cout << " First Set : Mean dist = " << phiProfileEB1->GetMean() << " RMS dist " << phiProfileEB1->GetRMS() << endl;

	fgaus->SetParameter(1, 1);
	fgaus->SetParameter(2, phiProfileEB2->GetRMS());
	fgaus->SetRange(1 - 5 * phiProfileEB2->GetRMS(), 1 + 5 * phiProfileEB2->GetRMS());
	fgaus->SetLineColor(kRed);
	phiProfileEB2->Fit("fgaus", "QRME");
	cout << " Second Set : Mean Fit = " << fgaus->GetParameter(1) << " RMS Fit = " << fgaus->GetParameter(2) << " chi2/ndf = " << fgaus->GetChisquare() / fgaus->GetNDF() << endl;
	cout << " Second Set : Mean dist = " << phiProfileEB2->GetMean() << " RMS dist " << phiProfileEB2->GetRMS() << endl;

/// phi Profile Histos EE+

	TH1F* phiProfileEEp1 = new TH1F ("phiProfileEEp1", "phiProfileEEp1", 60, 0.6, 1.6);
	TH1F* phiProfileEEp2 = new TH1F ("phiProfileEEp2", "phiProfileEEp2", 60, 0.6, 1.6);

	for(int i = 0; i < phiProjectionEEp1->GetN() ; i++) {
		double x = 0, y = 0;
		phiProjectionEEp1->GetPoint(i, x, y);
		phiProfileEEp1->Fill(y);
	}

	for(int i = 0; i < phiProjectionEEp2->GetN() ; i++) {
		double x = 0, y = 0;
		phiProjectionEEp2->GetPoint(i, x, y);
		phiProfileEEp2->Fill(y);
	}

	fgaus->SetParameter(1, 1);
	fgaus->SetParameter(2, phiProfileEEp1->GetRMS());
	fgaus->SetRange(1 - 5 * phiProfileEEp1->GetRMS(), 1 + 5 * phiProfileEEp1->GetRMS());
	fgaus->SetLineColor(kBlue);
	phiProfileEEp1->Fit("fgaus", "QRME");
	cout << " First Set :  Mean Fit = " << fgaus->GetParameter(1) << " RMS Fit = " << fgaus->GetParameter(2) << " chi2/ndf = " << fgaus->GetChisquare() / fgaus->GetNDF() << endl;
	cout << " First Set : Mean dist = " << phiProfileEEp1->GetMean() << " RMS dist " << phiProfileEEp1->GetRMS() << endl;

	fgaus->SetParameter(1, 1);
	fgaus->SetParameter(2, phiProfileEEp2->GetRMS());
	fgaus->SetRange(1 - 5 * phiProfileEEp2->GetRMS(), 1 + 5 * phiProfileEEp2->GetRMS());
	fgaus->SetLineColor(kRed);
	phiProfileEEp2->Fit("fgaus", "QRME");
	cout << " Second Set : Mean Fit = " << fgaus->GetParameter(1) << " RMS Fit = " << fgaus->GetParameter(2) << " chi2/ndf = " << fgaus->GetChisquare() / fgaus->GetNDF() << endl;
	cout << " Second Set : Mean dist = " << phiProfileEEp2->GetMean() << " RMS dist " << phiProfileEEp2->GetRMS() << endl;

/// phi Profile Histos EE-

	TH1F* phiProfileEEm1 = new TH1F ("phiProfileEEm1", "phiProfileEEm1", 60, 0.6, 1.6);
	TH1F* phiProfileEEm2 = new TH1F ("phiProfileEEm2", "phiProfileEEm2", 60, 0.6, 1.6);

	for(int i = 0; i < phiProjectionEEm1->GetN() ; i++) {
		double x = 0, y = 0;
		phiProjectionEEm1->GetPoint(i, x, y);
		phiProfileEEm1->Fill(y);
	}

	for(int i = 0; i < phiProjectionEEm2->GetN() ; i++) {
		double x = 0, y = 0;
		phiProjectionEEm2->GetPoint(i, x, y);
		phiProfileEEm2->Fill(y);
	}

	fgaus->SetParameter(1, 1);
	fgaus->SetParameter(2, phiProfileEEm1->GetRMS());
	fgaus->SetRange(1 - 5 * phiProfileEEm1->GetRMS(), 1 + 5 * phiProfileEEm1->GetRMS());
	fgaus->SetLineColor(kBlue);
	phiProfileEEm1->Fit("fgaus", "QRME");
	cout << " First Set :  Mean Fit = " << fgaus->GetParameter(1) << " RMS Fit = " << fgaus->GetParameter(2) << " chi2/ndf = " << fgaus->GetChisquare() / fgaus->GetNDF() << endl;
	cout << " First Set : Mean dist = " << phiProfileEEm1->GetMean() << " RMS dist " << phiProfileEEm1->GetRMS() << endl;

	fgaus->SetParameter(1, 1);
	fgaus->SetParameter(2, phiProfileEEm2->GetRMS());
	fgaus->SetRange(1 - 5 * phiProfileEEm2->GetRMS(), 1 + 5 * phiProfileEEm2->GetRMS());
	fgaus->SetLineColor(kRed);
	phiProfileEEm2->Fit("fgaus", "QRME");
	cout << " Second Set : Mean Fit = " << fgaus->GetParameter(1) << " RMS Fit = " << fgaus->GetParameter(2) << " chi2/ndf = " << fgaus->GetChisquare() / fgaus->GetNDF() << endl;
	cout << " Second Set : Mean dist = " << phiProfileEEm2->GetMean() << " RMS dist " << phiProfileEEm2->GetRMS() << endl;

/// eta Profile Histos EB

	cout << " Eta Profile Histos " << endl;

	TH1F* etaProfileEB1 = new TH1F ("etaProfileEB1", "etaProfileEB1", 60, 0.85, 1.15);
	TH1F* etaProfileEB2 = new TH1F ("etaProfileEB2", "etaProfileEB2", 60, 0.85, 1.15);

	for(int i = 0; i < etaProjectionEB1->GetN() ; i++) {
		double x = 0, y = 0;
		etaProjectionEB1->GetPoint(i, x, y);
		etaProfileEB1->Fill(y);
	}

	for(int i = 0; i < etaProjectionEB2->GetN() ; i++) {
		double x = 0, y = 0;
		etaProjectionEB2->GetPoint(i, x, y);
		etaProfileEB2->Fill(y);
	}

	fgaus->SetParameter(1, 1);
	fgaus->SetParameter(2, etaProfileEB1->GetRMS());
	fgaus->SetRange(1 - 5 * etaProfileEB1->GetRMS(), 1 + 5 * etaProfileEB1->GetRMS());
	fgaus->SetLineColor(kBlue);
	etaProfileEB1->Fit("fgaus", "QRME");
	cout << " First Set : Mean Fit = " << fgaus->GetParameter(1) << " RMS Fit = " << fgaus->GetParameter(2) << " chi2/ndf = " << fgaus->GetChisquare() / fgaus->GetNDF() << endl;
	cout << " First Set : Mean dist = " << etaProfileEB1->GetMean() << " RMS dist " << etaProfileEB1->GetRMS() << endl;

	fgaus->SetParameter(1, 1);
	fgaus->SetParameter(2, etaProfileEB2->GetRMS());
	fgaus->SetRange(1 - 5 * etaProfileEB2->GetRMS(), 1 + 5 * etaProfileEB2->GetRMS());
	fgaus->SetLineColor(kRed);
	etaProfileEB2->Fit("fgaus", "QRME");
	cout << " Second Set : Mean Fit = " << fgaus->GetParameter(1) << " RMS Fit = " << fgaus->GetParameter(2) << " chi2/ndf = " << fgaus->GetChisquare() / fgaus->GetNDF() << endl;
	cout << " Second Set : Mean dist = " << etaProfileEB2->GetMean() << " RMS dist " << etaProfileEB2->GetRMS() << endl;

/// eta Profile Histos EE+

	TH1F* etaProfileEEp1 = new TH1F ("etaProfileEEp1", "etaProfileEEp1", 35, 0., 2.5);
	TH1F* etaProfileEEp2 = new TH1F ("etaProfileEEp2", "etaProfileEEp2", 35, 0., 2.5);

	for(int i = 0; i < etaProjectionEEp1->GetN() ; i++) {
		double x = 0, y = 0;
		etaProjectionEEp1->GetPoint(i, x, y);
		etaProfileEEp1->Fill(y);
	}

	for(int i = 0; i < etaProjectionEEp2->GetN() ; i++) {
		double x = 0, y = 0;
		etaProjectionEEp2->GetPoint(i, x, y);
		etaProfileEEp2->Fill(y);
	}

	fgaus->SetParameter(1, 1);
	fgaus->SetParameter(2, etaProfileEEp1->GetRMS());
	fgaus->SetRange(1 - 5 * etaProfileEEp1->GetRMS(), 1 + 5 * etaProfileEEp1->GetRMS());
	fgaus->SetLineColor(kBlue);
	etaProfileEEp1->Fit("fgaus", "QRME");
	cout << " First Set : Mean Fit = " << fgaus->GetParameter(1) << " RMS Fit = " << fgaus->GetParameter(2) << " chi2/ndf = " << fgaus->GetChisquare() / fgaus->GetNDF() << endl;
	cout << " First Set : Mean dist = " << etaProfileEEp1->GetMean() << " RMS dist " << etaProfileEEp1->GetRMS() << endl;

	fgaus->SetParameter(1, 1);
	fgaus->SetParameter(2, etaProfileEEp2->GetRMS());
	fgaus->SetRange(1 - 5 * etaProfileEEp2->GetRMS(), 1 + 5 * etaProfileEEp2->GetRMS());
	fgaus->SetLineColor(kRed);
	etaProfileEEp2->Fit("fgaus", "QRME");
	cout << " Second Set : Mean Fit = " << fgaus->GetParameter(1) << " RMS Fit = " << fgaus->GetParameter(2) << " chi2/ndf = " << fgaus->GetChisquare() / fgaus->GetNDF() << endl;
	cout << " Second Set : Mean dist = " << etaProfileEEp2->GetMean() << " RMS dist " << etaProfileEEp2->GetRMS() << endl;


/// eta Profile Histos EB

	TH1F* etaProfileEEm1 = new TH1F ("etaProfileEEm1", "etaProfileEEm1", 35, 0., 2.5);
	TH1F* etaProfileEEm2 = new TH1F ("etaProfileEEm2", "etaProfileEEm2", 35, 0., 2.5);

	for(int i = 0; i < etaProjectionEEm1->GetN() ; i++) {
		double x = 0, y = 0;
		etaProjectionEEm1->GetPoint(i, x, y);
		etaProfileEEm1->Fill(y);
	}

	for(int i = 0; i < etaProjectionEEm2->GetN() ; i++) {
		double x = 0, y = 0;
		etaProjectionEEm2->GetPoint(i, x, y);
		etaProfileEEm2->Fill(y);
	}

	fgaus->SetParameter(1, 1);
	fgaus->SetParameter(2, etaProfileEEm1->GetRMS());
	fgaus->SetRange(1 - 5 * etaProfileEEm1->GetRMS(), 1 + 5 * etaProfileEEm1->GetRMS());
	fgaus->SetLineColor(kBlue);
	etaProfileEEm1->Fit("fgaus", "QRME");
	cout << " First Set : Mean Fit = " << fgaus->GetParameter(1) << " RMS Fit = " << fgaus->GetParameter(2) << " chi2/ndf = " << fgaus->GetChisquare() / fgaus->GetNDF() << endl;
	cout << " First Set : Mean dist = " << etaProfileEEm1->GetMean() << " RMS dist " << etaProfileEEm1->GetRMS() << endl;

	fgaus->SetParameter(1, 1);
	fgaus->SetParameter(2, etaProfileEEm2->GetRMS());
	fgaus->SetRange(1 - 5 * etaProfileEEm2->GetRMS(), 1 + 5 * etaProfileEEm2->GetRMS());
	fgaus->SetLineColor(kRed);
	etaProfileEEm2->Fit("fgaus", "QRME");
	cout << " Second Set : Mean Fit = " << fgaus->GetParameter(1) << " RMS Fit = " << fgaus->GetParameter(2) << " chi2/ndf = " << fgaus->GetChisquare() / fgaus->GetNDF() << endl;
	cout << " Second Set : Mean dist = " << etaProfileEEm2->GetMean() << " RMS dist " << etaProfileEEm2->GetRMS() << endl;






///------------------------------------------------------------------------
///-----------------------------------------------------------------
///--- Draw plots
///-----------------------------------------------------------------

	TCanvas *c[34];

	c[0] = new TCanvas("hdiffEB", "hdiffEB");
	c[0]->SetLeftMargin(0.1);
	c[0]->SetRightMargin(0.13);
	c[0]->SetGridx();

	diffmap_EB->GetXaxis()->SetNdivisions(1020);
	diffmap_EB->GetXaxis() -> SetLabelSize(0.03);
	diffmap_EB->GetXaxis() ->SetTitle("i#phi");
	diffmap_EB->GetYaxis() ->SetTitle("i#eta");
	diffmap_EB->GetZaxis() ->SetRangeUser(-0.1, 0.1);
	diffmap_EB->Draw("COLZ");

	c[1] = new TCanvas("histdiffEB", "histdiffEB");
	c[1]->SetLeftMargin(0.1);
	c[1]->SetRightMargin(0.13);
	c[1]->SetLogy();

	diffHistEB->GetXaxis()->SetTitle("c_{1}-c_{2}");
	diffHistEB->Draw();


	c[2] = new TCanvas("hratioEB", "hratioEB");
	c[2]->SetLeftMargin(0.1);
	c[2]->SetRightMargin(0.13);
	c[2]->SetGridx();

	ratiomap_EB->GetXaxis()->SetNdivisions(1020);
	ratiomap_EB->GetXaxis() -> SetLabelSize(0.03);
	ratiomap_EB->GetXaxis() ->SetTitle("i#phi");
	ratiomap_EB->GetYaxis() ->SetTitle("i#eta");
	ratiomap_EB->GetZaxis() ->SetRangeUser(0.95, 1.05);
	ratiomap_EB->Draw("COLZ");

	c[3] = new TCanvas("correlationEB", "correlationEB");
	c[3]->SetLeftMargin(0.1);
	c[3]->SetRightMargin(0.13);
	c[3]->SetGridx();
	c[3]->SetGridy();

	correlationEB->GetXaxis()->SetNdivisions(1020);
	correlationEB->GetXaxis() -> SetLabelSize(0.03);
	correlationEB->GetXaxis() ->SetTitle("c_{1}");
	correlationEB->GetYaxis() ->SetTitle("c_{2}");
	correlationEB->Draw("COLZ");

	std::cout << "correlation EB: " << correlationEB->GetCorrelationFactor() << std::endl;

	c[4] = new TCanvas("hdiffEEp", "hdiffEEp");
	c[4]->SetLeftMargin(0.1);
	c[4]->SetRightMargin(0.13);
	c[4]->SetGridx();

	diffmap_EEp->GetXaxis()->SetNdivisions(1020);
	diffmap_EEp->GetXaxis() -> SetLabelSize(0.03);
	diffmap_EEp->GetXaxis() ->SetTitle("ix");
	diffmap_EEp->GetYaxis() ->SetTitle("iy");
	diffmap_EEp->GetZaxis() ->SetRangeUser(-0.15, 0.15);
	diffmap_EEp->Draw("COLZ");

	c[5] = new TCanvas("histdiffEEp", "histdiffEEp");
	c[5]->SetLeftMargin(0.1);
	c[5]->SetRightMargin(0.13);
	c[5]->SetLogy();

	diffHistEEp->GetXaxis()->SetTitle("c_{1}-c_{2}");
	diffHistEEp->Draw();

	c[6] = new TCanvas("hratioEEp", "hratioEEp");
	c[6]->SetLeftMargin(0.1);
	c[6]->SetRightMargin(0.13);
	c[6]->SetGridx();

	ratiomap_EEp->GetXaxis()->SetNdivisions(1020);
	ratiomap_EEp->GetXaxis() -> SetLabelSize(0.03);
	ratiomap_EEp->GetXaxis() ->SetTitle("ix");
	ratiomap_EEp->GetYaxis() ->SetTitle("iy");
	ratiomap_EEp->GetZaxis() ->SetRangeUser(0.9, 1.1);
	ratiomap_EEp->Draw("COLZ");

	c[7] = new TCanvas("correlationEEp", "correlationEEp");
	c[7]->SetLeftMargin(0.1);
	c[7]->SetRightMargin(0.13);
	c[7]->SetGridx();
	c[7]->SetGridy();

	correlationEEp->GetXaxis()->SetNdivisions(1020);
	correlationEEp->GetXaxis() -> SetLabelSize(0.03);
	correlationEEp->GetXaxis() ->SetTitle("c_{1}");
	correlationEEp->GetYaxis() ->SetTitle("c_{2}");
	correlationEEp->Draw("COLZ");

	std::cout << "correlation EE+: " << correlationEEp->GetCorrelationFactor() << std::endl;

	c[8] = new TCanvas("hdiffEEm", "hdiffEEm");
	c[8]->SetLeftMargin(0.1);
	c[8]->SetRightMargin(0.13);
	c[8]->SetGridx();

	diffmap_EEm->GetXaxis()->SetNdivisions(1020);
	diffmap_EEm->GetXaxis() -> SetLabelSize(0.03);
	diffmap_EEm->GetXaxis() ->SetTitle("ix");
	diffmap_EEm->GetYaxis() ->SetTitle("iy");
	diffmap_EEm->GetZaxis() ->SetRangeUser(-0.15, 0.15);
	diffmap_EEm->Draw("COLZ");

	c[9] = new TCanvas("histdiffEEm", "histdiffEEm");
	c[9]->SetLeftMargin(0.1);
	c[9]->SetRightMargin(0.13);
	c[9]->SetLogy();

	diffHistEEm->GetXaxis()->SetTitle("c_{1}-c_{2}");
	diffHistEEm->Draw();



	c[10] = new TCanvas("hratioEEm", "hratioEEm");
	c[10]->SetLeftMargin(0.1);
	c[10]->SetRightMargin(0.13);
	c[10]->SetGridx();

	ratiomap_EEm->GetXaxis()->SetNdivisions(1020);
	ratiomap_EEm->GetXaxis() -> SetLabelSize(0.03);
	ratiomap_EEm->GetXaxis() ->SetTitle("ix");
	ratiomap_EEm->GetYaxis() ->SetTitle("iy");
	ratiomap_EEm->GetZaxis() ->SetRangeUser(0.9, 1.1);
	ratiomap_EEm->Draw("COLZ");

	c[11] = new TCanvas("correlationEEm", "correlationEEm");
	c[11]->SetLeftMargin(0.1);
	c[11]->SetRightMargin(0.13);
	c[11]->SetGridx();
	c[11]->SetGridy();

	correlationEEm->GetXaxis()->SetNdivisions(1020);
	correlationEEm->GetXaxis() -> SetLabelSize(0.03);
	correlationEEm->GetXaxis() ->SetTitle("c_{1}");
	correlationEEm->GetYaxis() ->SetTitle("c_{2}");
	correlationEEm->Draw("COLZ");

	std::cout << "correlation EE-: " << correlationEEm->GetCorrelationFactor() << std::endl;


	c[12] = new TCanvas("phiProjectionEB", "phiProjectionEB");
	c[12]->SetGridx();
	c[12]->SetGridy();
	phiProjectionEB1->GetHistogram()->GetYaxis()-> SetRangeUser(0.85, 1.1);
	phiProjectionEB1->GetHistogram()->GetXaxis()-> SetRangeUser(1, 361);
	phiProjectionEB1->GetHistogram()->GetYaxis()-> SetTitle("Mean IC");
	phiProjectionEB1->GetHistogram()->GetXaxis()-> SetTitle("i#phi");
	phiProjectionEB1->Draw("apl");
	phiProjectionEB2->Draw("plsame");

	TLegend * leg1 = new TLegend(0.75, 0.75, 0.89, 0.89);
	leg1->AddEntry(phiProjectionEB1, "IC set 1", "LP");
	leg1->AddEntry(phiProjectionEB2, "IC set 2", "LP");
	leg1->SetFillColor(0);
	leg1->Draw("same");

	c[13] = new TCanvas("phiProjectionEEp", "phiProjectionEEp");
	c[13]->SetGridx();
	c[13]->SetGridy();
	phiProjectionEEp1->GetHistogram()->GetYaxis()-> SetRangeUser(0.7, 1.4);
	phiProjectionEEp1->GetHistogram()->GetXaxis()-> SetRangeUser(1, 361);
	phiProjectionEEp1->GetHistogram()->GetYaxis()-> SetTitle("Mean IC");
	phiProjectionEEp1->GetHistogram()->GetXaxis()-> SetTitle("i#phi");
	phiProjectionEEp1->Draw("apl");
	phiProjectionEEp2->Draw("plsame");

	TLegend * leg2 = new TLegend(0.75, 0.75, 0.89, 0.89);
	leg2->AddEntry(phiProjectionEEp1, "IC set 1", "LP");
	leg2->AddEntry(phiProjectionEEp2, "IC set 2", "LP");
	leg2->SetFillColor(0);
	leg2->Draw("same");


	c[14] = new TCanvas("phiProjectionEEm", "phiProjectionEEm");
	c[14]->SetGridx();
	c[14]->SetGridy();
	phiProjectionEEm1->GetHistogram()->GetYaxis()-> SetRangeUser(0.7, 1.4);
	phiProjectionEEm1->GetHistogram()->GetXaxis()-> SetRangeUser(1, 361);
	phiProjectionEEm1->GetHistogram()->GetYaxis()-> SetTitle("Mean IC");
	phiProjectionEEm1->GetHistogram()->GetXaxis()-> SetTitle("i#phi");
	phiProjectionEEm1->Draw("apl");
	phiProjectionEEm2->Draw("plsame");

	TLegend * leg3 = new TLegend(0.75, 0.75, 0.89, 0.89);
	leg3->AddEntry(phiProjectionEEm1, "IC set 1", "LP");
	leg3->AddEntry(phiProjectionEEm2, "IC set 2", "LP");
	leg3->SetFillColor(0);
	leg3->Draw("same");

	c[15] = new TCanvas("etaProjectionEB", "etaProjectionEB");
	c[15]->SetGridx();
	c[15]->SetGridy();
	etaProjectionEB1->GetHistogram()->GetYaxis()-> SetRangeUser(0.9, 1.1);
	etaProjectionEB1->GetHistogram()->GetXaxis()-> SetRangeUser(0, 171);
	etaProjectionEB1->GetHistogram()->GetYaxis()-> SetTitle("Mean IC");
	etaProjectionEB1->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
	etaProjectionEB1->Draw("apl");
	etaProjectionEB2->Draw("plsame");

	TLegend * leg4 = new TLegend(0.75, 0.75, 0.89, 0.89);
	leg4->AddEntry(etaProjectionEB1, "IC set 1", "LP");
	leg4->AddEntry(etaProjectionEB2, "IC set 2", "LP");
	leg4->SetFillColor(0);
	leg1->Draw("same");

	c[16] = new TCanvas("etaProjectionEEp", "etaProjectionEEp");
	c[16]->SetGridx();
	c[16]->SetGridy();
	etaProjectionEEp1->GetHistogram()->GetYaxis()-> SetRangeUser(0.55, 1.5);
	etaProjectionEEp1->GetHistogram()->GetXaxis()-> SetRangeUser(85, 125);
	etaProjectionEEp1->GetHistogram()->GetYaxis()-> SetTitle("Mean IC");
	etaProjectionEEp1->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
	etaProjectionEEp1->Draw("apl");
	etaProjectionEEp2->Draw("plsame");

	TLegend * leg5 = new TLegend(0.75, 0.75, 0.89, 0.89);
	leg5->AddEntry(etaProjectionEEp1, "IC set 1", "LP");
	leg5->AddEntry(etaProjectionEEp2, "IC set 2", "LP");
	leg5->SetFillColor(0);
	leg5->Draw("same");


	c[17] = new TCanvas("etaProjectionEEm", "etaProjectionEEm");
	c[17]->SetGridx();
	c[17]->SetGridy();
	etaProjectionEEm1->GetHistogram()->GetYaxis()-> SetRangeUser(0.55, 1.5);
	etaProjectionEEm1->GetHistogram()->GetXaxis()-> SetRangeUser(85, 125);
	etaProjectionEEm1->GetHistogram()->GetYaxis()-> SetTitle("Mean IC");
	etaProjectionEEm1->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
	etaProjectionEEm1->Draw("apl");
	etaProjectionEEm2->Draw("plsame");

	TLegend * leg6 = new TLegend(0.75, 0.75, 0.89, 0.89);
	leg6->AddEntry(etaProjectionEEm1, "IC set 1", "LP");
	leg6->AddEntry(etaProjectionEEm2, "IC set 2", "LP");
	leg6->SetFillColor(0);
	leg6->Draw("same");

	c[18] = new TCanvas("phiProfileEB", "phiProfileEB");
	c[18]->SetGridx();
	c[18]->SetGridy();
	phiProfileEB1->GetXaxis()->SetTitle("#bar{IC}");
	phiProfileEB1->SetLineColor(kBlue);
	phiProfileEB1->SetMarkerSize(0.8);
	phiProfileEB1->SetLineWidth(2);
	phiProfileEB2->SetLineColor(kRed);
	phiProfileEB2->SetMarkerSize(0.8);
	phiProfileEB2->SetLineWidth(2);
	phiProfileEB1->Draw();
	phiProfileEB2->Draw("same");

	TLegend * leg7 = new TLegend(0.6, 0.7, 0.89, 0.89);
	leg7->SetFillColor(0);
	leg7->AddEntry(phiProfileEB1, "EB Projection I set ", "LP");
	leg7->AddEntry(phiProfileEB2, "EB Projection II set ", "LP");
	leg7->Draw("same");

	c[19] = new TCanvas("phiProfileEEp", "phiProfileEEp");
	c[19]->SetGridx();
	c[19]->SetGridy();
	phiProfileEEp1->GetXaxis()->SetTitle("#bar{IC}");
	phiProfileEEp1->SetLineColor(kBlue);
	phiProfileEEp1->SetMarkerSize(0.8);
	phiProfileEEp1->SetLineWidth(2);
	phiProfileEEp2->SetLineColor(kRed);
	phiProfileEEp2->SetMarkerSize(0.8);
	phiProfileEEp2->SetLineWidth(2);
	phiProfileEEp1->Draw();
	phiProfileEEp2->Draw("same");

	TLegend * leg8 = new TLegend(0.6, 0.7, 0.89, 0.89);
	leg8->SetFillColor(0);
	leg8->AddEntry(phiProfileEEp1, "EE+ Projection I set ", "LP");
	leg8->AddEntry(phiProfileEEp2, "EE+ Projection II set ", "LP");
	leg8->Draw("same");

	c[20] = new TCanvas("phiProfileEEm", "phiProfileEEm");
	c[20]->SetGridx();
	c[20]->SetGridy();
	phiProfileEEm1->GetXaxis()->SetTitle("#bar{IC}");
	phiProfileEEm1->SetLineColor(kBlue);
	phiProfileEEm1->SetMarkerSize(0.8);
	phiProfileEEm1->SetLineWidth(2);
	phiProfileEEm2->SetLineColor(kRed);
	phiProfileEEm2->SetMarkerSize(0.8);
	phiProfileEEm2->SetLineWidth(2);
	phiProfileEEm1->Draw();
	phiProfileEEm2->Draw("same");

	TLegend * leg9 = new TLegend(0.6, 0.7, 0.89, 0.89);
	leg9->SetFillColor(0);
	leg9->AddEntry(phiProfileEEm1, "EE- Projection I set ", "LP");
	leg9->AddEntry(phiProfileEEm2, "EE- Projection II set ", "LP");
	leg9->Draw("same");

	c[21] = new TCanvas("etaProfileEB", "etaProfileEB");
	c[21]->SetGridx();
	c[21]->SetGridy();
	etaProfileEB1->GetXaxis()->SetTitle("#bar{IC}");
	etaProfileEB1->SetLineColor(kBlue);
	etaProfileEB1->SetMarkerSize(0.8);
	etaProfileEB1->SetLineWidth(2);
	etaProfileEB2->SetLineColor(kRed);
	etaProfileEB2->SetMarkerSize(0.8);
	etaProfileEB2->SetLineWidth(2);
	etaProfileEB1->Draw();
	etaProfileEB2->Draw("same");

	TLegend * leg10 = new TLegend(0.6, 0.7, 0.89, 0.89);
	leg10->SetFillColor(0);
	leg10->AddEntry(etaProfileEB1, "EB Projection I set ", "LP");
	leg10->AddEntry(etaProfileEB2, "EB Projection II set ", "LP");
	leg10->Draw("same");

	c[22] = new TCanvas("etaProfileEEp", "etaProfileEEp");
	c[22]->SetGridx();
	c[22]->SetGridy();
	etaProfileEEp1->GetXaxis()->SetTitle("#bar{IC}");
	etaProfileEEp1->SetLineColor(kBlue);
	etaProfileEEp1->SetMarkerSize(0.8);
	etaProfileEEp1->SetLineWidth(2);
	etaProfileEEp2->SetLineColor(kRed);
	etaProfileEEp2->SetMarkerSize(0.8);
	etaProfileEEp2->SetLineWidth(2);
	etaProfileEEp1->Draw();
	etaProfileEEp2->Draw("same");

	TLegend * leg11 = new TLegend(0.6, 0.7, 0.89, 0.89);
	leg11->SetFillColor(0);
	leg11->AddEntry(phiProfileEEp1, "EE+ Projection I set ", "LP");
	leg11->AddEntry(phiProfileEEp2, "EE+ Projection II set ", "LP");
	leg11->Draw("same");

	c[23] = new TCanvas("etaProfileEEm", "etaProfileEEm");
	c[23]->SetGridx();
	c[23]->SetGridy();
	etaProfileEEm1->GetXaxis()->SetTitle("#bar{IC}");
	etaProfileEEm1->SetLineColor(kBlue);
	etaProfileEEm1->SetMarkerSize(0.8);
	etaProfileEEm1->SetLineWidth(2);
	etaProfileEEm2->SetLineColor(kRed);
	etaProfileEEm2->SetMarkerSize(0.8);
	etaProfileEEm2->SetLineWidth(2);
	etaProfileEEm1->Draw();
	etaProfileEEm2->Draw("same");

	TLegend * leg12 = new TLegend(0.6, 0.7, 0.89, 0.89);
	leg12->SetFillColor(0);
	leg12->AddEntry(phiProfileEEm1, "EE- Projection I set ", "LP");
	leg12->AddEntry(phiProfileEEm2, "EE- Projection II set ", "LP");
	leg12->Draw("same");

	c[24] = new TCanvas("habsoluteEB", "habsoluteEB");
	c[24]->SetLeftMargin(0.1);
	c[24]->SetRightMargin(0.13);
	c[24]->SetGridx();

	absolutemap_EB->GetXaxis()->SetNdivisions(1020);
	absolutemap_EB->GetXaxis() -> SetLabelSize(0.03);
	absolutemap_EB->GetXaxis() ->SetTitle("i#phi");
	absolutemap_EB->GetYaxis() ->SetTitle("i#eta");
	absolutemap_EB->GetZaxis() ->SetRangeUser(0.85, 1.15);
	absolutemap_EB->Draw("COLZ");

	c[25] = new TCanvas("habsoluteEEp", "habsoluteEEp");
	c[25]->SetLeftMargin(0.1);
	c[25]->SetRightMargin(0.13);
	c[25]->SetGridx();

	absolutemap_EEp->GetXaxis()->SetNdivisions(1020);
	absolutemap_EEp->GetXaxis() -> SetLabelSize(0.03);
	absolutemap_EEp->GetXaxis() ->SetTitle("ix");
	absolutemap_EEp->GetYaxis() ->SetTitle("iy");
	absolutemap_EEp->GetZaxis() ->SetRangeUser(0.7, 1.3);
	absolutemap_EEp->Draw("COLZ");


	c[26] = new TCanvas("habsoluteEEm", "habsoluteEEm");
	c[26]->SetLeftMargin(0.1);
	c[26]->SetRightMargin(0.13);
	c[26]->SetGridx();

	absolutemap_EEm->GetXaxis()->SetNdivisions(1020);
	absolutemap_EEm->GetXaxis() -> SetLabelSize(0.03);
	absolutemap_EEm->GetXaxis() ->SetTitle("ix");
	absolutemap_EEm->GetYaxis() ->SetTitle("iy");
	absolutemap_EEm->GetZaxis() ->SetRangeUser(0.7, 1.3);
	absolutemap_EEm->Draw("COLZ");


	c[27] = new TCanvas("map1_EB", "map1_EB");
	c[27]->SetLeftMargin(0.1);
	c[27]->SetRightMargin(0.13);
	c[27]->SetGridx();

	map1_EB->GetXaxis()->SetNdivisions(1020);
	map1_EB->GetXaxis() -> SetLabelSize(0.03);
	map1_EB->GetXaxis() ->SetTitle("i#phi");
	map1_EB->GetYaxis() ->SetTitle("i#eta");
	map1_EB->GetZaxis() ->SetRangeUser(0.6, 1.4);
	map1_EB->Draw("COLZ");


	c[28] = new TCanvas("map1_EEp", "map1_EEp");
	c[28]->SetLeftMargin(0.1);
	c[28]->SetRightMargin(0.13);
	c[28]->SetGridx();

	map1_EEp->GetXaxis()->SetNdivisions(1020);
	map1_EEp->GetXaxis() -> SetLabelSize(0.03);
	map1_EEp->GetXaxis() ->SetTitle("i#phi");
	map1_EEp->GetYaxis() ->SetTitle("i#eta");
	map1_EEp->GetZaxis() ->SetRangeUser(0.6, 1.4);
	map1_EEp->Draw("COLZ");


	c[29] = new TCanvas("map1_EEm", "map1_EEm");
	c[29]->SetLeftMargin(0.1);
	c[29]->SetRightMargin(0.13);
	c[29]->SetGridx();

	map1_EEm->GetXaxis()->SetNdivisions(1020);
	map1_EEm->GetXaxis() -> SetLabelSize(0.03);
	map1_EEm->GetXaxis() ->SetTitle("i#phi");
	map1_EEm->GetYaxis() ->SetTitle("i#eta");
	map1_EEm->GetZaxis() ->SetRangeUser(0.6, 1.4);
	map1_EEm->Draw("COLZ");


	c[30] = new TCanvas("map2_EB", "map2_EB");
	c[30]->SetLeftMargin(0.1);
	c[30]->SetRightMargin(0.13);
	c[30]->SetGridx();

	map2_EB->GetXaxis()->SetNdivisions(1020);
	map2_EB->GetXaxis() -> SetLabelSize(0.03);
	map2_EB->GetXaxis() ->SetTitle("i#phi");
	map2_EB->GetYaxis() ->SetTitle("i#eta");
	map2_EB->GetZaxis() ->SetRangeUser(0.6, 1.4);
	map2_EB->Draw("COLZ");


	c[31] = new TCanvas("map2_EEp", "map2_EEp");
	c[31]->SetLeftMargin(0.1);
	c[31]->SetRightMargin(0.13);
	c[31]->SetGridx();

	map2_EEp->GetXaxis()->SetNdivisions(1020);
	map2_EEp->GetXaxis() -> SetLabelSize(0.03);
	map2_EEp->GetXaxis() ->SetTitle("i#phi");
	map2_EEp->GetYaxis() ->SetTitle("i#eta");
	map2_EEp->GetZaxis() ->SetRangeUser(0.6, 1.4);
	map2_EEp->Draw("COLZ");


	c[32] = new TCanvas("map2_EEm", "map2_EEm");
	c[32]->SetLeftMargin(0.1);
	c[32]->SetRightMargin(0.13);
	c[32]->SetGridx();

	map2_EEm->GetXaxis()->SetNdivisions(1020);
	map2_EEm->GetXaxis() -> SetLabelSize(0.03);
	map2_EEm->GetXaxis() ->SetTitle("i#phi");
	map2_EEm->GetYaxis() ->SetTitle("i#eta");
	map2_EEm->GetZaxis() ->SetRangeUser(0.6, 1.4);
	map2_EEm->Draw("COLZ");


	TFile f("compareIC.root", "recreate");
	f.cd();

	for (int i = 0; i < 33; i++) {
		c[i]->Write();
	}

	f.Write();
// f.close();

// theApp->Run();

	return 0;

}


