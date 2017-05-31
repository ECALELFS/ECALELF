#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TString.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1F.h>
#include <TH2F.h>
#include <map>
#include <TCanvas.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TList.h>
#include <TObject.h>
#include <TKey.h>
#include <TPaveText.h>
#include <TLine.h>
#include <TGaxis.h>
#include <iomanip>
#include <TLatex.h>
#define batch

//typedef std::vector<bin_t>  hist_t;
typedef TH1F*  hist_t;

TString plot_img_filename(TString filename, TString region, TString ext = ".eps")
{
	//  TString filename=file->GetName(); //"testing"+region+"_Histos.root";
	//  TFile file(filename,"open");

	filename.ReplaceAll("fitres/", "./img/");

	if(region.Length() != 0) region.Insert(0, "_");

	filename.ReplaceAll(".root", ext);
	filename.Insert(filename.Length() - ext.Length(), region);
	return filename;
}

typedef struct {
	double y;
	double y2;
	double n;
} bin_t;

void AddHist(hist_t& hist, TH1F *h)
{
	hist = h;
	return;
}

TH1F* GetMeanHist(hist_t hist, TH1F *h, TString name, bool data = false)
{
	return hist;
}

TH1F *GetRatioHist(TH1F *data, TH1F *mc)
{

	TString mcName = mc->GetName();
	TH1F *ratio_hist = (TH1F *) data->Clone(mcName + "_ratio");
	ratio_hist->Sumw2();
	ratio_hist->Divide(mc);


	return ratio_hist;
}

//void PlotRatioHist(TH1F *ratio

void PlotCanvas(TCanvas *c, TH1F *mc, TH1F *data, TH1F *mcSmeared)
{

	TH1F *ratio_smeared = GetRatioHist(data, mcSmeared);
	TH1F *ratio_mc = GetRatioHist(data, mc);

	c->Clear();
	c->cd();
	TPad *pad1;
	pad1 = (TPad *) gROOT->FindObject("histPlot_pad");
	if(pad1 == NULL) pad1  = new TPad("histPlot_pad", "", 0, 0.3, 1, 1);
	TPad *pad2 = (TPad *) gROOT->FindObject("ratioPlot_pad");
	if(pad2 == NULL) pad2  = new TPad("ratioPlot_pad", "", 0, 0., 1, 0.3);
	pad1->Draw();
	pad2->Draw();

	pad1->cd();
	mc->DrawNormalized("hist", data->Integral());

	mcSmeared->SetLineWidth(3);
	mcSmeared->DrawNormalized("same hist", data->Integral());

	data->Draw("E same");


	pad2->cd();
	pad2->SetTopMargin(0);
	pad2->SetTicks(0, 0);

	ratio_smeared->Draw();
	ratio_smeared->GetYaxis()->SetRangeUser(0.5, 1.5);
	ratio_mc->Draw("same");
	ratio_mc->SetMarkerStyle(22);
	ratio_mc->SetMarkerColor(kRed);

	//  TLine *line = gROOT->FindObject("orizz_line");
	//  if(line==NULL) line = new TLine(
	TLine *line = new TLine(75, 1, 105, 1);
	line->Draw();
	//  ratio_mc->GetYaxis()->Draw("X+");

	//  ratio_mc->Draw("A");
	//  ratio_mc->GetYaxis()->Draw();

	return;

}



void Plot(TCanvas *c, TH1F *data, TH1F *mc, TH1F *mcSmeared = NULL, TLegend *legend = NULL, TString region = "", TString filename = "",  TString energy = "13", TString lumi = "", bool ratio = true)
{
	//lumi is specified in PlotMeanHist
	c->Clear();
	TPad * pad1 = new TPad("pad1", "pad1", 0.00, 0.25, 1, 1.);
	TPad * pad2 = new TPad("pad2", "pad2", 0.00, 0.00, 1, 0.25);
	TPad * pad3 = new TPad("pad3", "pad3", 0.75, 0.00, 1., 0.2);
	//pad1 e' data-MC mass spectra
	//pad2 e pad3 sono entrambe per il ratio
	float yscale = 0.75 / (pad1->GetYlowNDC() - pad2->GetYlowNDC());
	float xscale = 1;
	std::cout << "Starting plotting. Method Plot of plot_data_mc.C" << endl;
	if(ratio) {
		pad1->SetTopMargin(0.08);
		pad1->SetBottomMargin(0.02);
		//  pad1->SetLogy();
		pad1->Draw();
		pad1->cd();
		c->cd();
		pad2->SetGrid();
		pad2->SetBottomMargin(0.5);
		pad2->SetTopMargin(0.02);
		pad2->Draw();
		pad2->cd();
		c->cd();
		pad3->SetGrid(); //->pad3???
		pad3->SetBottomMargin(0.4);
		pad3->SetRightMargin(0.1);

		pad1->cd();
	}

	TGaxis::SetMaxDigits(3);

	//mc->SetLineColor(kRed);
	//mc->SetFillColor(kRed);
	mc->SetLineColor(kAzure + 6);
	mc->SetFillColor(kAzure + 6);
	mc->SetLineWidth(2);
	mc->SetFillStyle(1001);//colore pieno
	//mc->SetFillStyle(3001);//colore zigrinato
	mc->GetXaxis()->SetLabelSize(0);//the numbers on x axis are shown on the ratio
	mc->Draw();
	std::cout << "MC name is" << mc->GetTitle() << std::endl;
	double x_min = 0.;
	double x_max = 2.;
	if(mc->GetXaxis()->GetXmax() < 3) {
		mc->GetXaxis()->SetTitle("ptRatio");
		x_min = 0.5;
		x_max = 1.5;
		mc->GetXaxis()->SetRangeUser(x_min, x_max);
	} else if(mc->GetXaxis()->GetXmax() < 150) {
		mc->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
		x_min = 80;
		x_max = 100;
		mc->GetXaxis()->SetRangeUser(x_min, x_max);
	} else if(mc->GetXaxis()->GetXmax() > 150) {
		mc->GetXaxis()->SetTitle("pt2Sum (GeV/c^{2})");
		x_min = 20;
		x_max = 120;
		mc->GetXaxis()->SetRangeUser(x_min, x_max);
	}

	char ylabel[100];
	sprintf(ylabel, "Events/%.1f GeV", mc->GetBinWidth(1));
	//sprintf(ylabel,"Events");
	mc->GetYaxis()->SetTitle(ylabel);
	mc->GetYaxis()->SetTitleOffset(1.05);

	TH1* mcNorm = mc->DrawNormalized("hist", data->Integral());
	float ymax = std::max(mcNorm->GetMaximum(), data->GetMaximum()) * 1.2;
	mcNorm->GetYaxis()->SetRangeUser(0, ymax);

	mcSmeared->SetLineWidth(3);
	if(filename.Contains("scaleStep4smearing_0") || filename.Contains("scaleStep0")) {
		mcSmeared->SetLineColor(kAzure + 6);
		mcSmeared->SetFillColor(kAzure + 6);
	}
	TH1* mcSmeared_norm = mcSmeared->DrawNormalized("same hist", data->Integral());
	data->SetMarkerStyle(20);
	data->Draw("E same");
	pad1->RedrawAxis();

	//legend->Clear();
	//if(legend==NULL) legend = new TLegend(0.7,0.8,2.2,0.99); //This does NOT work properly

	TLegend* better_legend = new TLegend(0.6, 0.7, 0.82, 0.85); //legend is already defined
	better_legend->SetBorderSize(0);
	better_legend->SetFillColor(0);
	better_legend->SetTextSize(0.05);
	if(!filename.Contains("scaleStep4smearing_0") && !filename.Contains("scaleStep0")) {
		better_legend->AddEntry(mc, "MC", "f");
	}
	if(!filename.Contains("scaleStep4smearing_0") && !filename.Contains("scaleStep0")) {
		better_legend->AddEntry(mcSmeared, "#font[42]{MC smear}", "l");
	} else {
		better_legend->AddEntry(mcSmeared, "#font[42]{MC smear}", "f");
	}
	better_legend->AddEntry(data, "#font[42]{data}", "p");
	better_legend->Draw();

	double KS = data->KolmogorovTest(mcSmeared);
	TString ks = "Kolmogorov test: ";
	char line[50];
	sprintf(line, "%.2f", KS);
	ks += line;
	std::cout << "" << ks << std::endl;

	//TLatex pave(0.200,0.94,"#bf{CMS} #scale[0.75]{#it{Preliminary}}");
	//TLatex pave(0.200,0.94,"CMS #font[50]{Preliminary}");
	TLatex pave(0.200, 0.94, "#font[42]{#bf{CMS} #it{Preliminary}}");
	pave.SetNDC();
	pave.Draw();

	TLatex lumi_label(0.6, 0.94, "#font[42]{" + lumi + " fb^{-1} (" + energy + ")}");
	lumi_label.SetNDC();
	lumi_label.Draw();

	//legend->Draw();

	TLatex* region_label = new TLatex(0.2, 0.8, region);
	region_label->SetNDC();
	//region_label->Draw();

	if(ratio) {
		TH1F *sRatio = (TH1F*) data->Clone("sRatio");
		sRatio->Divide(mcSmeared_norm);
		pad2->cd();
		sRatio->Draw();

		TGraphErrors *ratioGraph = new TGraphErrors(sRatio);
		//ratioGraph->SetMarkerColor(kBlue);
		ratioGraph->SetMarkerColor(kBlack);
		ratioGraph->SetMarkerSize(0.8);

		ratioGraph->Draw("AP");
		//ratioGraph->GetXaxis()->SetRangeUser(data->GetXaxis()->GetXmin(),data->GetXaxis()->GetXmax());//To set limits correctly
		//ratioGraph->GetXaxis()->SetRangeUser(mc->GetXaxis()->GetXmin(),mc->GetXaxis()->GetXmax());//To set limits correctly
		ratioGraph->GetXaxis()->SetRangeUser(x_min, x_max); //To set limits correctly
		ratioGraph->GetXaxis()->SetTitle(mc->GetXaxis()->GetTitle());
		ratioGraph->GetYaxis()->SetTitle("Data/MC");

		ratioGraph->GetYaxis()->SetTitleSize(sRatio->GetYaxis()->GetTitleSize()*yscale);
		//ratioGraph->GetYaxis()->SetTitleOffset(sRatio->GetYaxis()->GetTitleOffset()/yscale);
		ratioGraph->GetYaxis()->SetTitleOffset(0.3);
		ratioGraph->GetYaxis()->SetLabelSize(sRatio->GetYaxis()->GetLabelSize()*yscale);
		ratioGraph->GetYaxis()->SetLabelOffset(sRatio->GetYaxis()->GetLabelOffset()*yscale);
		ratioGraph->GetXaxis()->SetTitleSize(sRatio->GetYaxis()->GetTitleSize() *yscale   );
		ratioGraph->GetXaxis()->SetTitleOffset(0.92);
		ratioGraph->GetXaxis()->SetLabelSize(sRatio->GetYaxis()->GetLabelSize() *yscale   );
		ratioGraph->GetXaxis()->SetLabelOffset(sRatio->GetYaxis()->GetLabelOffset());
		//double Y_min=std::min(0.9,1-2*ratioGraph->GetRMS(2));
		//double Y_max=std::max(1.1,1+2*ratioGraph->GetRMS(2));
		double Y_min = 0.8;
		double Y_max = 1.2;
		ratioGraph->GetYaxis()->SetRangeUser(Y_min, Y_max);
		ratioGraph->GetYaxis()->SetNdivisions(5);

		c->cd();
	}

	std::cout << "*****************Saving the plots" << std::endl;
	TString eps_name = plot_img_filename(filename, region, ".eps");
	TString png_name = plot_img_filename(filename, region, ".png");
	TString C_name = plot_img_filename(filename, region, ".C");
	c->SaveAs(eps_name);
	c->SaveAs(png_name);
	c->SaveAs(C_name);
	delete mc;
	mc = NULL;
	delete data;
	data = NULL;
	delete mcSmeared;
	mcSmeared = NULL;
}


void PlotMeanHist(TString filename, TString energy = "13 TeV", TString lumi = "36.4", float rebin = 0.5, TString myRegion = "")
{
	//rebin=2 --> per il closure test in Et
	//rebin=4 --> binwidth=0.5 GeV

	TH2F eventFraction("eventFraction", "", 10, 0, 9, 10, 0, 9);
	int index_max = 0;
	double  region_array[20][20];

	TString pdfWeightIndex = filename;
	pdfWeightIndex.ReplaceAll("test/dato/22Jan2012-runDepMCAll_v3/loose/invMass_SC_regrCorrSemiParV5_pho/step9pdfWeight/", "");
	pdfWeightIndex.ReplaceAll("/fitres/histos-scaleStep2smearing_9-Et_20-trigger-noPF.root", "");
	std::ofstream f_out("tmp/stat-" + pdfWeightIndex + ".dat");

	TFile f_in(filename, "read");
	if(f_in.IsZombie()) {
		std::cerr << "File opening error: " << filename << std::endl;
		return;
	}

	TList *KeyList = f_in.GetListOfKeys();

	//std::map<TString, hist_t > dataHist;
	//std::map<TString, hist_t > mcHist;
	//std::map<TString, hist_t > mcSmearedHist;

	std::map<TString, hist_t > dataHist;
	std::map<TString, hist_t > mcHist;
	std::map<TString, hist_t > mcSmearedHist;


	std::cout << KeyList->GetEntries() << std::endl;
	for(int i = 0; i <  KeyList->GetEntries(); i++) {

		TH1F *h = (TH1F *)((TKey *)KeyList->At(i))->ReadObj();
		TString keyName = h->GetName();

		TString type = keyName;
		type.Remove(0, type.Last('_') + 1);

		TString region = keyName;
		region.Remove(region.Last('_'));

		std::cout << "Data Entries in " << keyName << "\t" << h->GetEntries() << std::endl;
		std::cout << "KeyName is " << keyName << std::endl;
		std::cout << "type is " << type << std::endl;
		std::cout << "region is " << region << std::endl;

		if(type.CompareTo("data") == 0 || type.CompareTo("smeardata") == 0) {
			if(dataHist.find(region) == dataHist.end()) {
				hist_t hist_vec; // vector of bins of ONE histogram
				AddHist(hist_vec, h); // add the content of the histogram h to the vector
				dataHist[region] = h;
			} //else 	AddHist(dataHist[region], h);
		}

		if(type.CompareTo("MC") == 0) {
			if(mcHist.find(region) == mcHist.end()) {
				hist_t hist_vec;
				AddHist(hist_vec, h);
				mcHist[region] = h;
			} //else 	AddHist(mcHist[region], h);
		}

		if(type.CompareTo("smearMC") == 0) {
			if(mcSmearedHist.find(region) == mcSmearedHist.end()) {
				hist_t hist_vec;
				AddHist(hist_vec, h);
				mcSmearedHist[region] = h;
			} //else 	AddHist(mcSmearedHist[region], h);
		}
	}


	/*------------------------------ Plotting */
	TCanvas *c = new TCanvas("c", "c");
	TLegend *legend = new TLegend(0.6, 0.6, 0.65, 0.88);
	legend->SetFillStyle(3001);
	legend->SetFillColor(1);
	legend->SetTextFont(22);
	legend->SetTextSize(0.025);
	//  legend->SetFillColor(0); // colore di riempimento bianco
	//legend->SetMargin(0.4);  // percentuale della larghezza del simbolo
	//    SetLegendStyle(legend);

	//TH1F *mc_all[3]={NULL}, *data_all[3]={NULL}, *mcSmeared_all[3]={NULL};

	std::cout << "looping over histos to plot them" << std::endl;
	for(std::map<TString, hist_t>::const_iterator itr = dataHist.begin();
	        itr != dataHist.end(); itr++) {
		c->cd();
		TString region = itr->first;
		if(myRegion != "" && !region.Contains(myRegion)) continue;
		TH1F *h_ref = (TH1F *)((TKey *)KeyList->At(0))->ReadObj();
		TH1F *mc = GetMeanHist(mcHist[region], h_ref, "mc_" + region);
		TH1F *data = GetMeanHist(dataHist[region], h_ref, "data_" + region, true);
		TH1F *mcSmeared = GetMeanHist(mcSmearedHist[region], h_ref, "mcSmeared_" + region);
		f_out << pdfWeightIndex
		      << "\t" << data->GetName() << "\t" << data->GetTitle()
		      << "\t" << data->GetEntries() << "\t" << data->Integral()
		      << "\t" << data->GetMean() << "\t" << data->GetMeanError()
		      << "\t" << data->GetRMS() << "\t" << data->GetRMSError()
		      << std::endl;

		if (rebin >= data->GetXaxis()->GetBinWidth(1)) {
			int ngroups = int( rebin / data->GetXaxis()->GetBinWidth(1));
			data->Rebin(ngroups);
			mc->Rebin(ngroups);
			mcSmeared->Rebin(ngroups);
		}
		if(data->Integral() != 0 && mc->Integral() != 0 && mcSmeared->Integral() != 0)
			Plot(c, data, mc, mcSmeared, legend, region, filename, energy, lumi);
	}

	delete c;
	f_in.Close();
	f_out.close();
	return;
}



