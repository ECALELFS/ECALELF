#include "../interface/DrawingUtils.h"



void DrawICMap(TH2F* h2, const std::string& fileName, const std::string& fileType, const bool& isEB)
{
	TCanvas* c;

	SetLabelAndTitle(h2);

	if( isEB == true ) {
		c = new TCanvas("c", fileName.c_str(), 0, 0, 1000, 600);
		c -> SetGridx();
		c -> SetGridy();

		h2 -> GetXaxis() -> SetNdivisions(418);
		h2 -> GetXaxis() -> SetTitle("i#phi");
		h2 -> GetYaxis() -> SetTitle("i#eta");
		h2 -> GetZaxis() -> SetRangeUser(0.90, 1.10);
	} else {
		c = new TCanvas("c", fileName.c_str(), 0, 0, 700, 600);
		c -> SetGridx();
		c -> SetGridy();

		h2 -> GetXaxis() -> SetNdivisions(510);
		h2 -> GetXaxis() -> SetTitle("ix");
		h2 -> GetYaxis() -> SetTitle("iy");
		h2 -> GetZaxis() -> SetRangeUser(0.70, 1.30);
	}

	h2 -> Draw("COLZ");

	c -> Print((fileName + "." + fileType).c_str(), fileType.c_str());
	delete c;
}



void DrawSpreadHisto(TH1F* h, const std::string& fileName, const std::string& funcName, const std::string& fileType, const bool& isEB)
{
	TCanvas* c = new TCanvas("c", fileName.c_str(), 0, 0, 700, 600);

	SetLabelAndTitle(h);

	if( isEB == true ) h -> GetXaxis() -> SetRangeUser(0.9, 1.0999);
	else               h -> GetXaxis() -> SetRangeUser(0.7, 1.2999);
	if( isEB == true ) h -> GetYaxis() -> SetRangeUser(0., 2500.*(h->GetNbinsX() / 2000.));
	else               h -> GetYaxis() -> SetRangeUser(0., 150.*(h->GetNbinsX() / 2000.));
	h -> GetYaxis() -> SetTitle("entries");
	h -> GetXaxis() -> SetTitle("IC");
	h -> SetFillColor(kBlue + 2);
	h -> SetFillStyle(3002);
	h -> Draw("");

	TF1* f_spread = h -> GetFunction(funcName.c_str());
	f_spread -> SetLineWidth(2);

	char latexBuffer[50];
	sprintf(latexBuffer, "#mu = %1.2e", f_spread->GetParameter(1));
	TLatex* latex_mean = new TLatex(0.17, 0.80, latexBuffer);
	latex_mean -> SetNDC();
	latex_mean -> SetTextFont(42);
	latex_mean -> SetTextSize(0.05);
	latex_mean -> Draw("same");
	sprintf(latexBuffer, "#sigma = %1.2e", f_spread->GetParameter(2));
	TLatex* latex_sigma = new TLatex(0.17, 0.75, latexBuffer);
	latex_sigma -> SetNDC();
	latex_sigma -> SetTextFont(42);
	latex_sigma -> SetTextSize(0.05);
	latex_sigma -> Draw("same");
	sprintf(latexBuffer, "RMS = %1.2e", h->GetRMS());
	TLatex* latex_RMS = new TLatex(0.17, 0.70, latexBuffer);
	latex_RMS -> SetNDC();
	latex_RMS -> SetTextFont(42);
	latex_RMS -> SetTextSize(0.05);
	latex_RMS -> Draw("same");

	c -> Print((fileName + "." + fileType).c_str(), fileType.c_str());
	delete c;
}


void DrawSpreadGraph(TGraphErrors* g, const std::string& fileName, const std::string& fileType, const bool& isEB,
                     TGraphErrors* g_stat)
{
	TCanvas* c;

	SetLabelAndTitle(g_stat);

	if( isEB == true ) {
		c = new TCanvas("c", fileName.c_str(), 0, 0, 1000, 600);
		c -> SetGridx();
		c -> SetGridy();

		g_stat -> GetXaxis() -> SetRangeUser(0., 85.999);
		g_stat -> GetYaxis() -> SetRangeUser(0.00, 0.05);
		g_stat -> GetXaxis() -> SetTitle("|i#eta|");
		g_stat -> GetYaxis() -> SetTitle("#sigma");
		g_stat -> Draw("AP");
	} else {
		c = new TCanvas("c", fileName.c_str(), 0, 0, 700, 600);
		c -> SetGridx();
		c -> SetGridy();

		g_stat -> GetXaxis() -> SetRangeUser(-1., 38.999);
		g_stat -> GetYaxis() -> SetRangeUser(0.00, 0.05);
		g_stat -> GetXaxis() -> SetTitle("iRing");
		g_stat -> GetYaxis() -> SetTitle("#sigma");
		g_stat -> Draw("AP");
	}

	TLegend* leg = new TLegend(0.15, 0.74, 0.43, 0.89);
	leg -> SetFillColor(0);
	leg -> SetTextFont(42);
	leg -> SetTextSize(0.05);
	//  leg -> AddEntry(g,"IC spread","P");
	leg -> AddEntry(g_stat, "Statistical precision", "P");

	/*  if( g_stat != NULL )
	{
	  g_stat -> Draw("P,same");
	  leg -> AddEntry(g_stat,"statistical precision","P");
	}
	*/
	leg -> Draw("same");

	c -> Print((fileName + "." + fileType).c_str(), fileType.c_str());
	//  c -> Print((fileName+".png").c_str(),"png");

	delete c;
}



void DrawPhiAvgICSpread(TH1F* h, const std::string& fileName, const std::string& fileType, const bool& isEB)
{
	TCanvas* c = new TCanvas("c", fileName.c_str(), 0, 0, 700, 600);

	SetLabelAndTitle(h);

	if( isEB == true ) h -> GetXaxis() -> SetRangeUser(0.98, 1.01999);
	else               h -> GetXaxis() -> SetRangeUser(0.90, 1.0999);
	if(isEB == true ) h -> GetYaxis() -> SetRangeUser(0., 70.);
	h -> GetYaxis() -> SetTitle("entries");
	h -> GetXaxis() -> SetTitle("#LTIC#GT_{i#phi}");
	h -> SetFillColor(kBlack);
	h -> SetFillStyle(3003);
	h -> Draw("");

	TF1* f_phiAvgICSpread = h -> GetFunction("f_phiAvgICSpread");
	f_phiAvgICSpread -> SetLineWidth(2);

	char latexBuffer[50];
	sprintf(latexBuffer, "#mu = %1.2e", f_phiAvgICSpread->GetParameter(1));
	TLatex* latex_mean = new TLatex(0.17, 0.80, latexBuffer);
	latex_mean -> SetNDC();
	latex_mean -> SetTextFont(42);
	latex_mean -> SetTextSize(0.05);
	latex_mean -> Draw("same");
	sprintf(latexBuffer, "#sigma = %1.2e", f_phiAvgICSpread->GetParameter(2));
	TLatex* latex_sigma = new TLatex(0.17, 0.75, latexBuffer);
	latex_sigma -> SetNDC();
	latex_sigma -> SetTextFont(42);
	latex_sigma -> SetTextSize(0.05);
	latex_sigma -> Draw("same");
	sprintf(latexBuffer, "RMS = %1.2e", h->GetRMS());
	TLatex* latex_RMS = new TLatex(0.17, 0.70, latexBuffer);
	latex_RMS -> SetNDC();
	latex_RMS -> SetTextFont(42);
	latex_RMS -> SetTextSize(0.05);
	latex_RMS -> Draw("same");

	c -> Print((fileName + "." + fileType).c_str(), fileType.c_str());
	delete c;
}



void DrawAvgICVsPhiGraph(TGraphErrors* g, const std::string& fileName, const std::string& fileType, const Color_t& color, const bool& isEB)
{
	TCanvas* c = new TCanvas("c", fileName.c_str(), 0, 0, 1000, 600);
	c -> SetGridx();
	c -> SetGridy();

	SetLabelAndTitle(g);

	g -> GetXaxis() -> SetRangeUser(0., 360.999);
	if( isEB == true) g -> GetYaxis() -> SetRangeUser(0.975, 1.025);
	else              g -> GetYaxis() -> SetRangeUser(0.900, 1.100);
	g -> GetYaxis() -> SetTitle("#LTIC#GT_{i#phi}");
	g -> GetXaxis() -> SetTitle("i#phi");
	g -> SetMarkerStyle(20);
	g -> SetMarkerSize(0.5);
	g -> SetMarkerColor(color);
	g -> Draw("APL");

	c -> Print((fileName + "." + fileType).c_str(), fileType.c_str());
	delete c;
}



void DrawAvgICVsPhiFoldGraph(TGraphErrors* g, TGraphErrors* g2, const std::string& fileName, const std::string& fileType, const bool& isEB)
{
	TCanvas* c = new TCanvas("c", fileName.c_str(), 0, 0, 1000, 600);
	c -> SetGridx();
	c -> SetGridy();

	SetLabelAndTitle(g);
	SetLabelAndTitle(g2);

	g -> GetYaxis() -> SetRangeUser(0.98, 1.01999);
	g -> GetXaxis() -> SetRangeUser(0., 21.);
	g -> GetYaxis() -> SetTitle("#LTIC#GT_{i#phi}");
	g -> GetXaxis() -> SetTitle("i#phi%20");
	g -> SetMarkerStyle(20);
	g -> SetMarkerSize(0.7);
	g -> SetMarkerColor(kRed + 2);
	g -> SetLineColor(kRed + 2);
	g -> Draw("APL");

	g2 -> SetMarkerStyle(20);
	g2 -> SetMarkerSize(0.7);
	g2 -> SetMarkerColor(kGreen + 2);
	g2 -> SetLineColor(kGreen + 2);
	g2 -> Draw("PL,same");

	TLegend* leg = new TLegend(0.13, 0.74, 0.43, 0.89);
	leg -> SetFillColor(0);
	leg -> AddEntry(g, "w/o crack corr.", "P");
	leg -> AddEntry(g2, "w/ crack corr.", "P");
	leg -> Draw("same");

	c -> Print((fileName + "." + fileType).c_str(), fileType.c_str());
	delete c;
}



void DrawResidualGraph(TGraphErrors* g, const std::string& fileName, const std::string& fileType, const bool& isEB)
{
	TCanvas* c;

	SetLabelAndTitle(g);

	if( isEB == true ) {
		c = new TCanvas("c", fileName.c_str(), 0, 0, 1000, 600);
		c -> SetGridx();
		c -> SetGridy();

		g -> GetXaxis() -> SetRangeUser(0., 85.999);
		g -> GetYaxis() -> SetRangeUser(0.00, 0.03);
		g -> GetXaxis() -> SetTitle("|i#eta|");
		g -> GetYaxis() -> SetTitle("#sigma");
		g -> Draw("AP");
	} else {
		c = new TCanvas("c", fileName.c_str(), 0, 0, 700, 600);
		c -> SetGridx();
		c -> SetGridy();

		g -> GetXaxis() -> SetRangeUser(-1., 38.999);
		g -> GetYaxis() -> SetRangeUser(0.00, 0.20);
		g -> GetXaxis() -> SetTitle("iRing");
		g -> GetYaxis() -> SetTitle("#sigma");
		g -> Draw("AP");
	}

	TLegend* leg = new TLegend(0.15, 0.74, 0.43, 0.89);
	leg -> SetFillColor(0);
	leg -> SetTextFont(42);
	leg -> SetTextSize(0.05);
	leg -> AddEntry(g, "IC residual spread", "P");

	leg -> Draw("same");

	c -> Print((fileName + "." + fileType).c_str(), fileType.c_str());

	delete c;
}



void SetLabelAndTitle(TGraphErrors* g)
{
	g -> GetXaxis() -> SetLabelSize(0.04);
	g -> GetYaxis() -> SetLabelSize(0.04);
	g -> GetXaxis() -> SetTitleSize(0.05);
	g -> GetYaxis() -> SetTitleSize(0.05);
	g -> GetXaxis() -> SetTitleOffset(1.20);
	g -> GetYaxis() -> SetTitleOffset(1.20);
}

void SetLabelAndTitle(TH1F* h)
{
	h -> GetXaxis() -> SetLabelSize(0.04);
	h -> GetYaxis() -> SetLabelSize(0.04);
	h -> GetXaxis() -> SetTitleSize(0.05);
	h -> GetYaxis() -> SetTitleSize(0.05);
	h -> GetXaxis() -> SetTitleOffset(1.20);
	h -> GetYaxis() -> SetTitleOffset(1.20);
}

void SetLabelAndTitle(TH2F* h2)
{
	h2 -> GetXaxis() -> SetLabelSize(0.04);
	h2 -> GetYaxis() -> SetLabelSize(0.04);
	h2 -> GetXaxis() -> SetTitleSize(0.05);
	h2 -> GetYaxis() -> SetTitleSize(0.05);
	h2 -> GetXaxis() -> SetTitleOffset(1.20);
	h2 -> GetYaxis() -> SetTitleOffset(1.20);
}
