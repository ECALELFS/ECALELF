#include <iostream>
#include "TFile.h"
#include "TGraphErrors.h"
#include "TROOT.h"
#include "TString.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TH2F.h"

void DrawMomentumCalibration()
{
  gROOT->SetStyle("Plain");
  gStyle->SetTextFont(42);
  gStyle->SetTextSize(0.05);
  gStyle->SetTitleFont(42,"xyz");
  gStyle->SetTitleSize(0.05);
  gStyle->SetLabelFont(42,"xyz");
  gStyle->SetLabelSize(0.05);
  gStyle->SetStatFont(42);
  gStyle->SetStatFontSize(0.05);
  gStyle->SetOptTitle(0); 
  gStyle->SetOptStat(10111); 
  gStyle->SetOptFit(1); 
  gStyle->SetPalette(1); 
  gROOT->ForceStyle();

  TFile *f = TFile::Open("output/MomentumCalibration.root");
  int i=0;
  
  while(f!=0) 
  {
 
  TString Name = Form("g_EoP_EB_%d",i);
  TGraphErrors* g_EoP_EB = (TGraphErrors*)f->Get(Name);
  Name= Form("g_EoC_EB_%d",i);
  TGraphErrors* g_EoC_EB = (TGraphErrors*)f->Get(Name);
  Name= Form("g_EoC_EB_%d",i+1);
  TGraphErrors* g_EoC_EB2 = (TGraphErrors*)f->Get(Name);

  Name= Form("g_Rat_EB_%d",i);
  TGraphErrors* g_Rat_EB = (TGraphErrors*)f->Get(Name);

  if(g_EoP_EB==0 && g_EoC_EB==0 && g_Rat_EB==0) break;
 
  g_EoP_EB -> SetMarkerStyle(20);
  g_EoP_EB -> SetMarkerSize(1);
  g_EoP_EB -> SetMarkerColor(kRed+1); 
  g_EoP_EB -> SetLineColor(kRed+1);
 

  g_EoC_EB -> SetMarkerStyle(20);
  g_EoC_EB -> SetMarkerSize(1);
  g_EoC_EB -> SetMarkerColor(kGreen+1);
  g_EoC_EB -> SetLineColor(kGreen+1);
  
  if(g_EoC_EB2!=0)
  {

  g_EoC_EB2 -> SetMarkerStyle(20);
  g_EoC_EB2 -> SetMarkerSize(1);
  g_EoC_EB2 -> SetMarkerColor(kGreen+1);
  g_EoC_EB2 -> SetLineColor(kGreen+1);
  
  TH2F* correlation = new TH2F("correlation","correlation",50,0.98,1.02,50,0.97,1.03);
  for(int i=0; i<g_EoC_EB->GetN() && i<g_EoC_EB2->GetN()  ; i++)
  { double x,y,z,k;
    g_EoC_EB->GetPoint(i,x,y);
    g_EoC_EB2->GetPoint(i,z,k);

    correlation->Fill(y,k);
   }
  correlation->GetXaxis()->SetTitle("mod1-2");
  correlation->GetYaxis()->SetTitle("mod3-4");
  cout<<" correlation "<<correlation->GetCorrelationFactor()<<endl;;
  correlation->Draw("colz");
  }
//  correlation->Draw("colz");
  g_Rat_EB -> SetMarkerStyle(20);
  g_Rat_EB -> SetMarkerSize(1);
  g_Rat_EB -> SetMarkerColor(kBlue+2); 
  g_Rat_EB -> SetLineColor(kBlue+2); 

   float phiLim = 360;
   Name = Form("g_fit_EB_%d",i);
   TCanvas* c_g_fit_EB = new TCanvas(Name, Name,100,100,800,600);
   Name = Form("pad_0_EB_%d",i);
   TPad *cLower_EB = new TPad(Name,Name,0.00,0.00,1.00,0.30);
   Name = Form("pad_1_EB_%d",i);
   TPad *cUpper_EB = new TPad(Name,Name,0.00,0.30,1.00,1.00);
   
   cLower_EB->SetBottomMargin(0.25); cUpper_EB->SetTopMargin(0.01); 
   cUpper_EB->SetBottomMargin(0.01);

   cLower_EB->Draw();
   cUpper_EB->Draw();

   float FontSCF = cUpper_EB->GetHNDC()/cLower_EB->GetHNDC(); 
   float tYoffset = 0.8; 
   float labSize = 0.06;

   cUpper_EB -> cd();
   gPad->SetGrid();

   TH1F *hPad_EB = (TH1F*)gPad->DrawFrame(0.,0.95,phiLim,1.05);
   hPad_EB->GetXaxis()->SetLabelSize(labSize);
   hPad_EB->GetXaxis()->SetTitleSize(labSize);
   hPad_EB->GetYaxis()->SetLabelSize(labSize);
   hPad_EB->GetYaxis()->SetTitleSize(labSize);
   hPad_EB->GetXaxis()->SetTitleOffset(tYoffset);
   hPad_EB->GetYaxis()->SetTitleOffset(tYoffset);
   hPad_EB->GetXaxis()->SetTitle("#phi_{SC}");
   hPad_EB->GetYaxis()->SetTitle("M_{Z}^{2}/M_{ee}^{2} #propto p"); 
  
   g_EoP_EB -> Draw("PL");
   g_EoC_EB -> Draw("PL");

   TLegend *tl_EB = new TLegend(0.80,0.80,0.90,0.95);
   tl_EB -> SetTextFont(40);
   tl_EB -> SetFillColor(0);
   //  tl -> SetBorderSize(0); 
   tl_EB -> AddEntry(g_EoP_EB,"MC EB","PL");
   tl_EB -> AddEntry(g_EoC_EB,"Data EB","PL");
   tl_EB -> Draw();

   cLower_EB -> cd();
   gPad->SetGrid();

   TH1F *hRat_EB = (TH1F*)gPad->DrawFrame(0.,0.95,phiLim,1.05);
   hRat_EB->GetYaxis()->SetNdivisions(505);
   hRat_EB->GetXaxis()->SetLabelSize(labSize*FontSCF);
   hRat_EB->GetXaxis()->SetTitleSize(labSize*FontSCF);
   hRat_EB->GetYaxis()->SetLabelSize(labSize*FontSCF);
   hRat_EB->GetYaxis()->SetTitleSize(labSize*FontSCF);
   hRat_EB->GetYaxis()->SetTitleOffset(tYoffset/FontSCF);
   hRat_EB->GetXaxis()->SetTitleOffset(0.6);
   hRat_EB->GetXaxis()->SetTitle("#phi_{SC}"); 
   hRat_EB->GetYaxis()->SetTitle("Data / MC"); 
   g_Rat_EB->Draw("PL"); 
   i++;
 }

 i=0;

 while (f!=0) {
   
   TString Name = Form("g_EoP_EE_%d",i);
   TGraphErrors* g_EoP_EE = (TGraphErrors*)f->Get(Name);
   Name= Form("g_EoC_EE_%d",i);
   TGraphErrors* g_EoC_EE = (TGraphErrors*)f->Get(Name);
   Name= Form("g_Rat_EE_%d",i);
   TGraphErrors* g_Rat_EE = (TGraphErrors*)f->Get(Name);
   if(g_EoP_EE==0 && g_EoC_EE==0 && g_Rat_EE==0) break;
  
   g_EoP_EE -> SetMarkerStyle(20);
   g_EoP_EE -> SetMarkerSize(1);
   g_EoP_EE -> SetMarkerColor(kRed+1); 
   g_EoP_EE -> SetLineColor(kRed+1); 

   g_EoC_EE -> SetMarkerStyle(20);
   g_EoC_EE -> SetMarkerSize(1);
   g_EoC_EE -> SetMarkerColor(kGreen+1);
   g_EoC_EE -> SetLineColor(kGreen+1);
  
   g_Rat_EE -> SetMarkerStyle(20);
   g_Rat_EE -> SetMarkerSize(1);
   g_Rat_EE -> SetMarkerColor(kBlue+2); 
   g_Rat_EE -> SetLineColor(kBlue+2); 
 
   Name = Form("g_fit_EE_%d",i);
   TCanvas* c_g_fit_EE = new TCanvas(Name, Name,100,100,800,600);
   TPad *cLower_EE = new TPad("pad_0_EE","pad_0_EE",0.00,0.00,1.00,0.30);
   TPad *cUpper_EE = new TPad("pad_1_EE","pad_1_EE",0.00,0.30,1.00,1.00);

   cLower_EE->SetBottomMargin(0.25); cUpper_EE->SetTopMargin(0.01); 
   cUpper_EE->SetBottomMargin(0.01); 

   cLower_EE->Draw();
   cUpper_EE->Draw();

   float FontSCF = cUpper_EE->GetHNDC()/cLower_EE->GetHNDC(); 
   float tYoffset = 0.8; 
   float labSize = 0.06;
   float phiLim = 360.;
   cUpper_EE -> cd();
   gPad->SetGrid();
   TH1F *hPad_EE = (TH1F*)gPad->DrawFrame(0.,0.95,phiLim,1.05);
   hPad_EE->GetXaxis()->SetLabelSize(labSize);
   hPad_EE->GetXaxis()->SetTitleSize(labSize);
   hPad_EE->GetYaxis()->SetLabelSize(labSize);
   hPad_EE->GetYaxis()->SetTitleSize(labSize);
   hPad_EE->GetXaxis()->SetTitleOffset(tYoffset);
   hPad_EE->GetYaxis()->SetTitleOffset(tYoffset);
   hPad_EE->GetXaxis()->SetTitle("#phi_{SC}");
   hPad_EE->GetYaxis()->SetTitle("M_{ee}^{2}/M_{Z}^{2} #propto 1/p"); 
  
   g_EoP_EE -> Draw("PL");
   g_EoC_EE -> Draw("PL");

   TLegend *tl_EE = new TLegend(0.80,0.80,0.90,0.95);
   tl_EE -> SetTextFont(40);
   tl_EE -> SetFillColor(0);
   //  tl -> SetBorderSize(0); 
   tl_EE -> AddEntry(g_EoP_EE,"MC EE","PL");
   tl_EE -> AddEntry(g_EoC_EE,"Data EE","PL");
   tl_EE -> Draw();

   cLower_EE -> cd();
   gPad->SetGrid();

   TH1F *hRat_EE = (TH1F*)gPad->DrawFrame(0.,0.95,phiLim,1.05);
   hRat_EE->GetYaxis()->SetNdivisions(505);
   hRat_EE->GetXaxis()->SetLabelSize(labSize*FontSCF);
   hRat_EE->GetXaxis()->SetTitleSize(labSize*FontSCF);
   hRat_EE->GetYaxis()->SetLabelSize(labSize*FontSCF);
   hRat_EE->GetYaxis()->SetTitleSize(labSize*FontSCF);
   hRat_EE->GetYaxis()->SetTitleOffset(tYoffset/FontSCF);
   hRat_EE->GetXaxis()->SetTitleOffset(0.6);
   hRat_EE->GetXaxis()->SetTitle("#phi_{SC}"); 
   hRat_EE->GetYaxis()->SetTitle("Data / MC"); 
   g_Rat_EE->Draw("PL"); 
   i++;

 }

}