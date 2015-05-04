// To Plot E/p distributions for endcap or barrel at different iteration
#include "TH2F.h"
#include "TCanvas.h"
#include <iostream>
#include "TH1F.h"
#include "TFile.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TLegend.h"
// Input needed: E/p distributions file 
void DrawEPDistributions (){
  // Set style options
  gROOT->Reset();
  gROOT->SetStyle("Plain");

  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
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
  gStyle->SetOptStat(0);
 

  TFile *f1 = TFile::Open("/data1/rgerosa/L3_Weight/PromptSkim_recoFlag/EE/Weight_WZAnalysis_SingleElectron_Run2011AB_WElectron-PromptSkim_Z_R9_EE.root");
  TH1F *EoP_eta_2_iter_0 = (TH1F*)f1->Get("h_24_hC_EoP_eta_0");
  TH1F *EoP_eta_2_iter_4 = (TH1F*)f1->Get("h_24_hC_EoP_eta_10");
  TH1F *EoP_eta_2_iter_14 = (TH1F*)f1->Get("h_24_hC_EoP_eta_24");
 
  TH1F *EoP_eta_70_iter_0 = (TH1F*)f1->Get("h_31_hC_EoP_eta_0");
  TH1F *EoP_eta_70_iter_4 = (TH1F*)f1->Get("h_31_hC_EoP_eta_10");
  TH1F *EoP_eta_70_iter_14 = (TH1F*)f1->Get("h_31_hC_EoP_eta_24");
 

 
  TCanvas *cEoP2 = new TCanvas("cEoP2","cEoP2");
  cEoP2->SetGridx();
  cEoP2->SetGridy();
  EoP_eta_2_iter_0->GetXaxis()-> SetTitle("E/p");
  EoP_eta_2_iter_0->SetTitle("E/p ring n#circ24");
  EoP_eta_2_iter_0->SetLineColor(kGreen);
  EoP_eta_2_iter_0->SetLineWidth(2);
  EoP_eta_2_iter_0->SetLineStyle(1);
  EoP_eta_2_iter_0->Draw();
  EoP_eta_2_iter_4->SetLineColor(kBlue);
  EoP_eta_2_iter_4->SetLineWidth(3);
  EoP_eta_2_iter_4->SetLineStyle(7);
  EoP_eta_2_iter_4->Draw("same");
  EoP_eta_2_iter_14->SetLineColor(kRed);
  EoP_eta_2_iter_14->SetLineWidth(2);
  EoP_eta_2_iter_14->SetLineStyle(2);
  EoP_eta_2_iter_14->Draw("same");
  TLegend * leg = new TLegend(0.6,0.6,0.89, 0.89);
  leg->SetFillColor(0);
  leg->AddEntry(EoP_eta_2_iter_0,"EoP at iteration 1", "LP");
  leg->AddEntry(EoP_eta_2_iter_4,"EoP at iteration 10", "LP");
  leg->AddEntry(EoP_eta_2_iter_14,"EoP at iteration 25", "LP");

  leg->Draw();


  TCanvas *cEoP70 = new TCanvas("cEoP70","cEoP70");
  cEoP70->SetGridx();
  cEoP70->SetGridy();
  EoP_eta_70_iter_0->GetXaxis()-> SetTitle("E/p");
  EoP_eta_70_iter_0->SetTitle("E/p ring n#circ31");
  EoP_eta_70_iter_0->SetLineColor(kGreen);
  EoP_eta_70_iter_0->SetLineWidth(2);
  EoP_eta_70_iter_0->SetLineStyle(1);
  EoP_eta_70_iter_0->Draw();
  EoP_eta_70_iter_4->SetLineColor(kBlue);
  EoP_eta_70_iter_4->SetLineWidth(3);
  EoP_eta_70_iter_4->SetLineStyle(7);
  EoP_eta_70_iter_4->Draw("same");
  EoP_eta_70_iter_14->SetLineColor(kRed);
  EoP_eta_70_iter_14->SetLineWidth(2);
  EoP_eta_70_iter_14->SetLineStyle(2);
  EoP_eta_70_iter_14->Draw("same");
  leg->Draw();

}