// To compare two sets of IC
// Input needed: two set of IC (2D maps) 
{
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
  gStyle->SetTitleFont(42,"xyz");
  gStyle->SetTitleSize(0.05);
  gStyle->SetLabelFont(42,"xyz");
  gStyle->SetLabelSize(0.05);
  gROOT->ForceStyle();

  TFile *f1 = TFile::Open("calibrationTemplate_XTAL_allEvents_Jul05_Aug05_Oct03_Run2011B.root");
  TFile *f2 = TFile::Open("calibrationTemplate_XTAL_allEvents_Jul05_Aug05_Oct03_Run2011B_new.root");

  // input coeff
  TH2F *hcmap1 = (TH2F*)f1->Get("h_scale_map");
  TH2F *hcmap2 = (TH2F*)f2->Get("h_scale_map");
  TH2F *hemap1 = (TH2F*)f1->Get("h_error_map");
  TH2F *hemap2 = (TH2F*)f2->Get("h_error_map");

  // output histos
  TH2F * h2 = new TH2F("h2","h2",400,0.5,1.5,400,0.5,1.5);
  TH2F * h2diff = (TH2F*)hcmap1->Clone("h2diff");
  h2diff->Reset();

  TH1F *hdiff = new TH1F("hdiff", "hdiff", 400,-0.5,0.5);
  
  char hname[100];

  TH1F *hspread[172];
  for (int jbin = 1; jbin < hcmap1-> GetNbinsY(); jbin++){
    float etaring = hcmap1-> GetYaxis()->GetBinCenter(jbin);
    sprintf(hname,"hspread_ring_ieta%02d",etaring);
    hspread[jbin-1]= new TH1F(hname, hname, 400,-0.5,0.5);
  }
  
  for (int jbin = 1; jbin < hcmap1-> GetNbinsY(); jbin++){
    float etaring = hcmap1-> GetYaxis()->GetBinCenter(jbin);
    for (int ibin = 1; ibin < hcmap1-> GetNbinsX()+1; ibin++){
      float c1 = hcmap1->GetBinContent(ibin,jbin);
      float c2 = hcmap2->GetBinContent(ibin,jbin);
      float e1 = hemap1->GetBinContent(ibin,jbin);
      float e2 = hemap2->GetBinContent(ibin,jbin);
      if (c1!=0 && c2!=0 && e1!=-1 && e2!=-1){
	hspread[jbin-1]->Fill( c1-c2 );
	h2->Fill(c1,c2);
	h2diff->SetBinContent(ibin,jbin,c1-c2);
	if (fabs(etaring) < 40) hdiff->Fill(c1-c2);
       
      }
    }
  }

  
  TGraphErrors *sigma_vs_ieta = new TGraphErrors();
  sigma_vs_ieta->SetMarkerStyle(20);
  sigma_vs_ieta->SetMarkerSize(1);
  sigma_vs_ieta->SetMarkerColor(kBlue+2);
  
  TGraphErrors *rms_vs_ieta = new TGraphErrors();
  rms_vs_ieta->SetMarkerStyle(24);
  rms_vs_ieta->SetMarkerSize(1);
  rms_vs_ieta->SetMarkerColor(kBlue+2);  

  TGraphErrors *scale_vs_ieta = new TGraphErrors();
  scale_vs_ieta->SetMarkerStyle(20);
  scale_vs_ieta->SetMarkerSize(1);
  scale_vs_ieta->SetMarkerColor(kBlue+2);

  TF1 *fgaus = new TF1("fgaus","gaus",-1,1);
  int np = 0;
  for (int i = 1; i < hcmap1-> GetNbinsY(); i++){
    float etaring = hcmap1-> GetYaxis()->GetBinCenter(i);
    if (int(etaring)==0) continue;
    if ( hspread[i-1]->GetEntries() == 0) continue;
    hspread[i-1]->Fit("fgaus","Q");
    np++;
    sigma_vs_ieta-> SetPoint(np,etaring,fgaus->GetParameter(2));
    sigma_vs_ieta-> SetPointError(np,0,fgaus->GetParError(2));
    rms_vs_ieta  -> SetPoint(np,etaring, hspread[i-1]->GetRMS());
    rms_vs_ieta  -> SetPointError(np,0,hspread[i-1]->GetRMSError() );
    scale_vs_ieta-> SetPoint(np,etaring,fgaus->GetParameter(1));
    scale_vs_ieta-> SetPointError(np,0,fgaus->GetParError(1));
  }
  
  // plot

  TCanvas *csigma = new TCanvas("csigma","csigma");
  csigma->SetGridx();
  csigma->SetGridy();
  sigma_vs_ieta->GetHistogram()->GetYaxis()-> SetRangeUser(0.00,0.2);
  sigma_vs_ieta->GetHistogram()->GetXaxis()-> SetRangeUser(-85,85);
  sigma_vs_ieta->GetHistogram()->GetYaxis()-> SetTitle("#sigma");
  sigma_vs_ieta->GetHistogram()->GetXaxis()-> SetTitle("ieta");
  sigma_vs_ieta->Draw("ap");
  //rms_vs_ieta->Draw("psame");

  TCanvas *cscale = new TCanvas("cscale","cscale");
  cscale->SetGridx();
  cscale->SetGridy();
  scale_vs_ieta->GetHistogram()->GetYaxis()-> SetRangeUser(-0.1,0.1);
  scale_vs_ieta->GetHistogram()->GetXaxis()-> SetRangeUser(-85,85);
  scale_vs_ieta->GetHistogram()->GetYaxis()-> SetTitle("c_{1}-c_{2}");
  scale_vs_ieta->GetHistogram()->GetXaxis()-> SetTitle("ieta");
  scale_vs_ieta->Draw("ap");

  TCanvas *cmap2 = new TCanvas("cmap2","cmap2",500,500);
  cmap2->SetGridx();
  cmap2->SetGridy();
  cmap2 -> cd();
  cmap2->SetLeftMargin(0.1); 
  cmap2->SetRightMargin(0.15); 
  h2->GetXaxis()->SetRangeUser(0.85,1.15);
  h2->GetYaxis()->SetRangeUser(0.85,1.15);
  h2->GetXaxis()->SetTitle("C_{1}");
  h2->GetYaxis()->SetTitle("C_{2}");
  h2->Draw("colz");

  
  TCanvas *cdiff = new TCanvas("cdiff","cdiff",700,500);
  cdiff->SetGridx();
  cdiff->SetGridy();
  cdiff -> cd();
  cdiff->SetLeftMargin(0.1); 
  cdiff->SetRightMargin(0.15); 
  h2diff->GetZaxis()->SetRangeUser(-0.05,0.05);
  h2diff->GetXaxis()->SetTitle("i#phi");
  h2diff->GetYaxis()->SetTitle("i#eta");
  h2diff->Draw("colz");

  
}
