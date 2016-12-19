#include "../../../myTools/functions/src/functions.cc"
#include <TChain.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TH1F.h>
#include <TROOT.h>
#include <iostream>
#include <TPaveText.h>

void Plot_nPV_dataMC(TChain *dataChain, TChain *mcChain, TString outputFilename, TString puWeightBranch = "puWeight")
{
	TCanvas c("c", "");
	c.cd();

	dataChain->Draw("nPV>>data_nPV_hist(60,0,60)");
	mcChain->Draw("nPV>>mc_nPV_hist(60,0,60)", puWeightBranch);
	//  mcChain->Draw("nPV>>mc_nPV_hist");

	TH1F *data_hist =   (TH1F *)  gROOT->FindObject("data_nPV_hist");
	TH1F *mc_hist   =   (TH1F *)  gROOT->FindObject("mc_nPV_hist");

	data_hist->Sumw2(); // to plot the errors

	mc_hist->GetXaxis()->SetTitle("Number of reconstructed vertexes");
	mc_hist->GetYaxis()->SetTitle("[a.u.]");
	data_hist->GetXaxis()->SetTitle("Number of reconstructed vertexes");
	data_hist->GetYaxis()->SetTitle("[a.u.]");

	mc_hist->GetXaxis()->SetRangeUser(0, 30);
	data_hist->GetXaxis()->SetRangeUser(0, 30);

	mc_hist->SetLineWidth(2);
	data_hist->SetMarkerStyle(20);
	data_hist->SetMarkerSize(1.2);

	mc_hist->DrawNormalized();
	data_hist->DrawNormalized("same");

	TLegend legend(0.7, 0.85, 1, 1);

	std::cout << "[INFO] Setting entries in legend using chain titles" << std::endl;

	legend.AddEntry(data_hist, dataChain->GetTitle(), "p");
	legend.AddEntry(mc_hist,   mcChain->GetTitle(), "l");
	SetLegendStyle(&legend);
	legend.SetBorderSize(1);

	legend.Draw();


	TPaveText pv(0.3, 0.93, 0.6, 1, "ndc");
	pv.AddText("CMS Preliminary, 8TeV");
	pv.SetBorderSize(0);
	pv.SetFillStyle(0);
	pv.SetTextSize(0.045);
	pv.Draw();

	c.SaveAs(outputFilename);

	c.SaveAs(outputFilename.ReplaceAll(".eps", ".C"));
	return;
}

