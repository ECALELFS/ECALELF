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
#include "TMath.h"



int nEtaBinsEB = 1;
int nEtaBinsEE = 1;

void CombineMomentumCalibration()
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
  
  TFile *f1 = TFile::Open("output/MomentumCalibration2011_fede_eta1_eta1.root");
  TFile *f2 = TFile::Open("output/MomentumCalibration2012_fede_eta1_eta1.root");
  TFile *f3 = new TFile("output/MomentumCalibrationCombined_2011AB-2012ABC_fede_eta1_eta1.root","RECREATE");
  f3->cd();
  
  double x,y;
  
  
  
  //-------
  // Barrel
  
  for(int etaBin = 0; etaBin < nEtaBinsEB; ++etaBin)
  {
    TString Name = Form("g_EoC_EB_%d",etaBin);
    TGraphErrors* g_EoC_EB_1 = (TGraphErrors*)f1->Get(Name);
    double yAvg_EB_1 = 0.;
    int nGoodPoints_EB_1 = 0;
    for(int point = 0; point < g_EoC_EB_1->GetN(); ++point)
    {
      g_EoC_EB_1 -> GetPoint(point,x,y);
      if( (y > 0.9) && (y < 1.1) )
      {
        yAvg_EB_1 += y;
        ++nGoodPoints_EB_1;
      }
    }
    yAvg_EB_1 /= nGoodPoints_EB_1;
    std::cout << "*** EB_" << etaBin << "_1 yScale: " << yAvg_EB_1 << std::endl;
    
    
    TString Name = Form("g_EoC_EB_%d",etaBin);
    TGraphErrors* g_EoC_EB_2 = (TGraphErrors*)f2->Get(Name);
    double yAvg_EB_2 = 0.;
    int nGoodPoints_EB_2 = 0;
    for(int point = 0; point < g_EoC_EB_2->GetN(); ++point)
    {
      g_EoC_EB_2 -> GetPoint(point,x,y);
      if( (y > 0.9) && (y < 1.1) )
      {
        yAvg_EB_2 += y;
        ++nGoodPoints_EB_2;
      }
    }
    yAvg_EB_2 /= nGoodPoints_EB_2;
    std::cout << "*** EB_" << etaBin << "_2 yScale: " << yAvg_EB_2 << std::endl;
    
    
    
    g_EoC_EB_1 -> SetMarkerStyle(20);
    g_EoC_EB_1 -> SetMarkerSize(1);
    g_EoC_EB_1 -> SetMarkerColor(kGreen+1);
    g_EoC_EB_1 -> SetLineColor(kGreen+1);
    
    g_EoC_EB_2 -> SetMarkerStyle(20);
    g_EoC_EB_2 -> SetMarkerSize(1);
    g_EoC_EB_2 -> SetMarkerColor(kRed+1);
    g_EoC_EB_2 -> SetLineColor(kRed+1);
    
    TGraphErrors* g_Diff_EB = (TGraphErrors*)g_EoC_EB_1->Clone(0);
    TGraphErrors* g_Corr_EB = (TGraphErrors*)g_EoC_EB_1->Clone(0);
    
    g_Diff_EB -> SetMarkerStyle(20);
    g_Diff_EB -> SetMarkerSize(1);
    g_Diff_EB -> SetMarkerColor(kBlue+2); 
    g_Diff_EB -> SetLineColor(kBlue+2); 
    
    g_Corr_EB -> SetMarkerStyle(20);
    g_Corr_EB -> SetMarkerSize(1);
    g_Corr_EB -> SetMarkerColor(kBlack); 
    g_Corr_EB -> SetLineColor(kBlack); 
    
    
    for(int jPoint = 0; jPoint < g_EoC_EB_1->GetN(); jPoint++)
    {
      double x1,y1,ex1,ey1;
      double x2,y2,ex2,ey2;
      g_EoC_EB_1->GetPoint(jPoint,x1,y1);
      ex1= g_EoC_EB_1->GetErrorX(jPoint);
      ey1= g_EoC_EB_1->GetErrorY(jPoint);
      
      g_EoC_EB_2->GetPoint(jPoint,x2,y2);
      ex2= g_EoC_EB_2->GetErrorX(jPoint);
      ey2= g_EoC_EB_2->GetErrorY(jPoint);
      
      g_Corr_EB ->SetPoint(jPoint,x1,(y2+y1)/2);
      g_Corr_EB ->SetPointError(jPoint,ex1,0.5*sqrt(ey2*ey2+ey1*ey2));
      
      g_Diff_EB ->SetPoint(jPoint,x1,(y2-y1)/2);
      g_Diff_EB ->SetPointError(jPoint,ex1,0.5*sqrt(ey2*ey2+ey1*ey2));
    }
    
    
    float phiLim = 360;
    Name = Form("g_fit_EB_%d",etaBin);
    TCanvas* c_g_fit_EB = new TCanvas(Name, Name,100,100,800,600);
    Name = Form("pad_0_EB_%d",etaBin);
    TPad *cLower_EB = new TPad(Name,Name,0.01,0.00,0.99,0.30);
    Name = Form("pad_1_EB_%d",etaBin);
    TPad *cUpper_EB = new TPad(Name,Name,0.01,0.30,0.99,0.99);
    
    cLower_EB->SetBottomMargin(0.25); cUpper_EB->SetTopMargin(0.01); 
    cUpper_EB->SetBottomMargin(0.01);
    
    cLower_EB->Draw();
    cUpper_EB->Draw();
    
    float FontSCF = cUpper_EB->GetHNDC()/cLower_EB->GetHNDC(); 
    float tYoffset = 0.8; 
    float labSize = 0.06;
    cUpper_EB -> cd();
    gPad->SetGrid();
    
    TH1F *hPad_EB = (TH1F*)gPad->DrawFrame(-3.15,0.95,3.15,1.05);
    hPad_EB->GetXaxis()->SetLabelSize(labSize);
    hPad_EB->GetXaxis()->SetTitleSize(labSize);
    hPad_EB->GetYaxis()->SetLabelSize(labSize);
    hPad_EB->GetYaxis()->SetTitleSize(labSize);
    hPad_EB->GetXaxis()->SetTitleOffset(tYoffset);
    hPad_EB->GetYaxis()->SetTitleOffset(tYoffset);
    hPad_EB->GetXaxis()->SetTitle("#phi_{SC}");
    hPad_EB->GetYaxis()->SetTitle("M_{Z}^{2}/M_{ee}^{2} #propto p"); 
    
    g_EoC_EB_1 -> Draw("PL");
    g_EoC_EB_2 -> Draw("PL");
    //g_Corr_EB -> Draw("PL");
    
    
    TLegend *tl_EB = new TLegend(0.80,0.80,0.90,0.95);
    tl_EB -> SetTextFont(40);
    tl_EB -> SetFillColor(0);
    //tl -> SetBorderSize(0); 
    tl_EB -> AddEntry(g_EoC_EB_1,"Data 2011 EB","PL");
    tl_EB -> AddEntry(g_EoC_EB_2,"Data 2012 EB","PL");
    //tl_EB -> AddEntry(g_Corr_EB,"2011-2012 Combined","PL");
    
    tl_EB -> Draw();
     
    cLower_EB -> cd();
    gPad->SetGrid();
    
    TH1F *hRat_EB = (TH1F*)gPad->DrawFrame(-3.15,-0.01,3.15,0.01);
    hRat_EB->GetYaxis()->SetNdivisions(505);
    hRat_EB->GetXaxis()->SetLabelSize(labSize*FontSCF);
    hRat_EB->GetXaxis()->SetTitleSize(labSize*FontSCF);
    hRat_EB->GetYaxis()->SetLabelSize(labSize*FontSCF);
    hRat_EB->GetYaxis()->SetTitleSize(labSize*FontSCF);
    hRat_EB->GetYaxis()->SetTitleOffset(tYoffset/FontSCF);
    hRat_EB->GetXaxis()->SetTitleOffset(0.6);
    hRat_EB->GetXaxis()->SetTitle("#phi_{SC}"); 
    hRat_EB->GetYaxis()->SetTitle("Spread/2"); 
    g_Diff_EB->Draw("PL"); 
    
    Name = Form("g_EoC_EB_%d",etaBin);
    g_Corr_EB->Write(Name);
  }
  
  
  
  //--------
  // Endcaps
  
  for(int etaBin = 0; etaBin < nEtaBinsEE; ++etaBin)
  {
    TString Name = Form("g_EoC_EE_%d",etaBin);
    TGraphErrors* g_EoC_EE_1 = (TGraphErrors*)f1->Get(Name);
    double yAvg_EE_1 = 0.;
    int nGoodPoints_EE_1 = 0;
    for(int point = 0; point < g_EoC_EE_1->GetN(); ++point)
    {
      g_EoC_EE_1 -> GetPoint(point,x,y);
      if( (y > 0.9) && (y < 1.1) )
      {
        yAvg_EE_1 += y;
        ++nGoodPoints_EE_1;
      }
    }
    yAvg_EE_1 /= nGoodPoints_EE_1;
    std::cout << "*** EE_" << etaBin << "_1 yScale: " << yAvg_EE_1 << std::endl;
    
    Name= Form("g_EoC_EE_%d",etaBin);
    TGraphErrors* g_EoC_EE_2 = (TGraphErrors*)f2->Get(Name);
    double yAvg_EE_2 = 0.;
    int nGoodPoints_EE_2 = 0;
    for(int point = 0; point < g_EoC_EE_2->GetN(); ++point)
    {
      g_EoC_EE_2 -> GetPoint(point,x,y);
      if( (y > 0.9) && (y < 1.1) )
      {
        yAvg_EE_2 += y;
        ++nGoodPoints_EE_2;
      }
    }
    yAvg_EE_2 /= nGoodPoints_EE_2;
    std::cout << "*** EE_" << etaBin << "_2 yScale: " << yAvg_EE_2 << std::endl;
    
    
    g_EoC_EE_1 -> SetMarkerStyle(20);
    g_EoC_EE_1 -> SetMarkerSize(1);
    g_EoC_EE_1 -> SetMarkerColor(kGreen+1); 
    g_EoC_EE_1 -> SetLineColor(kGreen+1); 
    
    g_EoC_EE_2 -> SetMarkerStyle(20);
    g_EoC_EE_2 -> SetMarkerSize(1);
    g_EoC_EE_2 -> SetMarkerColor(kRed+1);
    g_EoC_EE_2 -> SetLineColor(kRed+1);
    
    TGraphErrors* g_Diff_EE = (TGraphErrors*)g_EoC_EE_1->Clone(0);
    TGraphErrors* g_Corr_EE = (TGraphErrors*)g_EoC_EE_1->Clone(0);
    
    g_Diff_EE -> SetMarkerStyle(20);
    g_Diff_EE -> SetMarkerSize(1);
    g_Diff_EE -> SetMarkerColor(kBlue+2); 
    g_Diff_EE -> SetLineColor(kBlue+2); 
    
    g_Corr_EE -> SetMarkerStyle(20);
    g_Corr_EE -> SetMarkerSize(1);
    g_Corr_EE -> SetMarkerColor(kBlack); 
    g_Corr_EE -> SetLineColor(kBlack); 
    
    
    for(int jPoint = 0; jPoint < g_EoC_EE_1->GetN() ; jPoint++)
    {
      double x1,y1,ex1,ey1;
      double x2,y2,ex2,ey2;
      g_EoC_EE_1->GetPoint(jPoint,x1,y1);
      ex1= g_EoC_EE_1->GetErrorX(jPoint);
      ey1= g_EoC_EE_1->GetErrorY(jPoint);
      
      g_EoC_EE_2->GetPoint(jPoint,x2,y2);
      ex2= g_EoC_EE_2->GetErrorX(jPoint);
      ey2= g_EoC_EE_2->GetErrorY(jPoint);
      
      g_Diff_EE ->SetPoint(jPoint,x1,(y2-y1)/2);
      g_Diff_EE ->SetPointError(jPoint,ex1,0.5*sqrt(ey2*ey2+ey1*ey2));
      
      g_Corr_EE ->SetPoint(jPoint,x1,(y2+y1)/2);
      g_Corr_EE ->SetPointError(jPoint,ex1,0.5*sqrt(ey2*ey2+ey1*ey2));
    }
    
    
    Name = Form("g_fit_EE_%d",etaBin);
    
    TCanvas* c_g_fit_EE = new TCanvas(Name, Name,100,100,800,600);
    TPad *cLower_EE = new TPad("pad_0_EE","pad_0_EE",0.01,0.00,0.99,0.30);
    TPad *cUpper_EE = new TPad("pad_1_EE","pad_1_EE",0.01,0.30,0.99,0.99);
    
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
    TH1F *hPad_EE = (TH1F*)gPad->DrawFrame(-3.15,0.90,3.15,1.10);
    hPad_EE->GetXaxis()->SetLabelSize(labSize);
    hPad_EE->GetXaxis()->SetTitleSize(labSize);
    hPad_EE->GetYaxis()->SetLabelSize(labSize);
    hPad_EE->GetYaxis()->SetTitleSize(labSize);
    hPad_EE->GetXaxis()->SetTitleOffset(tYoffset);
    hPad_EE->GetYaxis()->SetTitleOffset(tYoffset);
    hPad_EE->GetXaxis()->SetTitle("#phi_{SC}");
    hPad_EE->GetYaxis()->SetTitle("M_{Z}^{2}/M_{ee}^{2} #propto 1/p"); 
    
    g_EoC_EE_1 -> Draw("PL");
    g_EoC_EE_2 -> Draw("PL");
    //g_Corr_EE -> Draw("PL");
    
    TLegend *tl_EE = new TLegend(0.80,0.80,0.90,0.95);
    tl_EE -> SetTextFont(40);
    tl_EE -> SetFillColor(0);
    //tl -> SetBorderSize(0); 
    tl_EE -> AddEntry(g_EoC_EE_1,"Data 2011 EE","PL");
    tl_EE -> AddEntry(g_EoC_EE_2,"Data 2012 EE","PL");
    //tl_EE -> AddEntry(g_Corr_EE,"2011-2012 Combination EE","PL");
    
    tl_EE -> Draw();
    
    cLower_EE -> cd();
    gPad->SetGrid();
    
    TH1F *hRat_EE = (TH1F*)gPad->DrawFrame(-3.15,-0.03,3.15,0.03);
    hRat_EE->GetYaxis()->SetNdivisions(505);
    hRat_EE->GetXaxis()->SetLabelSize(labSize*FontSCF);
    hRat_EE->GetXaxis()->SetTitleSize(labSize*FontSCF);
    hRat_EE->GetYaxis()->SetLabelSize(labSize*FontSCF);
    hRat_EE->GetYaxis()->SetTitleSize(labSize*FontSCF);
    hRat_EE->GetYaxis()->SetTitleOffset(tYoffset/FontSCF);
    hRat_EE->GetXaxis()->SetTitleOffset(0.6);
    hRat_EE->GetXaxis()->SetTitle("#phi_{SC}"); 
    hRat_EE->GetYaxis()->SetTitle("Spread/2"); 
    g_Diff_EE->Draw("PL");
    
    Name = Form("g_EoC_EE_%d",etaBin);
    g_Corr_EE->Write(Name);
  }
  
  
  f3->Close();
}
