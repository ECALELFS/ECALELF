#include <vector>
#include <utility>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include "TFile.h"
#include "TStyle.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TROOT.h"
#include "TLegend.h"
#include "TPaveStats.h"
//
/// Macro to produce ECAL single electron calibration plots for EE
/// Input Files : MC or Data splistat and no splitstat, MCTruth and MCReco IC maps EE
/// Output Files :  StatPrec_MC_EE.root --> stat precision MC usefull for CompareCalibMCTruth_EE.C only MC
///                 IC_MC_4Correction.root --> Map IC for radial correction only MC
//                  
using namespace std;

void DrawCalibrationPlotsEE (
 			     Char_t* infile1 = "data_LC_20120131_ALPHA_test_prompt/SingleElectron_Run2011AB-WElectron-data_LC_20120131_ALPHA_test_prompt_EoPcaibEE_11032011_Z_R9_EE.root",
 			     Char_t* infile2 = "data_LC_20120131_ALPHA_test_prompt/Even_SingleElectron_Run2011AB-WElectron-data_LC_20120131_ALPHA_test_prompt_EoPcaibEE_11032011_Z_R9_EE.root",
 			     Char_t* infile3 = "data_LC_20120131_ALPHA_test_prompt/Odd_SingleElectron_Run2011AB-WElectron-data_LC_20120131_ALPHA_test_prompt_EoPcaibEE_11032011_Z_R9_EE.root",
 			     //Char_t* infile1 = "FT_R_42_V21B/WZAnalysis_PromptSkim_W-DoubleElectron_FT_R_42_V21B_Z_R9_EE.root",
 			     //Char_t* infile2 = "FT_R_42_V21B/Even_WZAnalysis_PromptSkim_W-DoubleElectron_FT_R_42_V21B_Z_R9_EE.root",
 			     //Char_t* infile3 = "FT_R_42_V21B/Odd_WZAnalysis_PromptSkim_W-DoubleElectron_FT_R_42_V21B_Z_R9_EE.root",
			     int evalStat = 1,
                             bool isMC=false,
			     Char_t* fileType = "png", 
			     Char_t* dirName = ".")
{

  bool  printPlots = false;

  /// by xtal

  int nbins = 250;

  /// Set style options
  gROOT->Reset();
  gROOT->SetStyle("Plain");

  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetOptTitle(0); 
  gStyle->SetOptStat(11110); 
  gStyle->SetOptFit(0); 
  gStyle->SetFitFormat("6.3g"); 
  gStyle->SetPalette(1); 
 
  gStyle->SetTextFont(42);
  gStyle->SetTextSize(0.05);
  gStyle->SetTitleFont(42,"xyz");
  gStyle->SetTitleSize(0.05);
  gStyle->SetLabelFont(42,"xyz");
  gStyle->SetLabelSize(0.05);
  gStyle->SetTitleXOffset(0.8);
  gStyle->SetTitleYOffset(1.1);
  gROOT->ForceStyle();

  if ( !infile1 ) {
    cout << " No input file specified !" << endl;
    return;
  }


  if ( evalStat && (!infile2 || !infile3 )){
    cout << " No input files to evaluate statistical precision specified !" << endl;
    return;
  }

  cout << "Making calibration plots for: " << infile1 << endl;
  
  
  /// imput file with full statistic normlized to the mean in a ring

  TFile *f = new TFile(infile1);
  TH2F *hcmap[2];
  hcmap[0] = (TH2F*)f->Get("h_scale_map_EEM");
  hcmap[1] = (TH2F*)f->Get("h_scale_map_EEP");
    

  /// ring geometry for the endcap
  TH2F *hrings[2];
  hrings[0] = (TH2F*)hcmap[0]->Clone("hringsEEM");
  hrings[1] = (TH2F*)hcmap[0]->Clone("hringsEEP");
  hrings[0] ->Reset("ICMES");
  hrings[1] ->Reset("ICMES");
  hrings[0] ->ResetStats();
  hrings[1] ->ResetStats();

  FILE *fRing;
  fRing = fopen("macros/eerings.dat","r");
  int x,y,z,ir;
  while(fscanf(fRing,"(%d,%d,%d) %d \n",&x,&y,&z,&ir) !=EOF ) {
    if(z>0) hrings[1]->Fill(x,y,ir); 
    if(z<0) hrings[0]->Fill(x,y,ir);
  }

  TFile *f4 = TFile::Open("MCtruthIC_EE.root");
  TFile *f5 = TFile::Open("MCRecoIC_EE.root");

  TH2F *hcmapMcT_EEP = (TH2F*)f4->Get("h_scale_EEP");
  TH2F *hcmapMcT_EEM = (TH2F*)f4->Get("h_scale_EEM");
  TH2F *hcmapMcR_EEP = (TH2F*)f5->Get("h_scale_EEP");
  TH2F *hcmapMcR_EEM = (TH2F*)f5->Get("h_scale_EEM");
 
 
  ///--------------------------------------------------------------------------------
  ///--- Build the precision vs ring plot starting from the TH2F of IC folded and not
  ///--------------------------------------------------------------------------------

  char hname[100];
  char htitle[100];
  TH1F *hspread[2][50];
  TH1F* hspreadAll [40];

  TH2F* ICComparison [2];
  ICComparison[0] = (TH2F*) hcmapMcT_EEP->Clone("ICComparison_EEM");
  ICComparison[1] = (TH2F*) hcmapMcT_EEM->Clone("ICComparison_EEP");
  ICComparison[0]->Reset("ICMES");
  ICComparison[1]->Reset("ICMES");
  ICComparison[0]->ResetStats();
  ICComparison[1]->ResetStats();


  for (int k = 0; k < 2; k++){
         
    for (int iring = 0; iring < 40 ; iring++){
      if (k==0)
      {
       sprintf(hname,"hspreadAll_ring%02d",iring);
       hspreadAll[iring] = new TH1F(hname, hname, nbins,0.,2.);
       sprintf(hname,"hspreadEEM_MCTruth_ring%02d",iring);
       hspread[k][iring] = new TH1F(hname, hname, nbins,0.,2.);

      }
      else{ 
	
        sprintf(hname,"hspreadEEP_ring%02d",iring);
         hspread[k][iring] = new TH1F(hname, hname, nbins,0.,2.);
 
     }
    }
 }
  
 /// spread all distribution, spread for EE+ and EE- and comparison with the MC truth
  for (int k = 0; k < 2 ; k++){
    for (int ix = 1; ix < 101; ix++){
      for (int iy = 1; iy < 101; iy++){
	int iz = k;
	if (k==0) iz = -1;
	int mybin = hcmap[k] -> FindBin(ix,iy);
	int ring  = hrings[1]-> GetBinContent(mybin);
	float ic = hcmap[k]->GetBinContent(mybin);
        float ic2=0;
          if(k==0 && hcmapMcT_EEM->GetBinContent(mybin)!=0 && hcmapMcR_EEM->GetBinContent(mybin)!=0 ) ic2 = hcmapMcT_EEM->GetBinContent(mybin)/hcmapMcR_EEM->GetBinContent(mybin);
          else if(hcmapMcT_EEP->GetBinContent(mybin)!=0 && hcmapMcR_EEP->GetBinContent(mybin)!=0 ) ic2 = hcmapMcT_EEP->GetBinContent(mybin)/hcmapMcR_EEP->GetBinContent(mybin);
          
 	if ( ic>0 && ic2>0 )    {
	  hspread[k][ring]->Fill(ic);
          hspreadAll[ring]->Fill(ic);
          ICComparison[k]->Fill(ix,iy,ic/ic2);
	}
      }
    }
  }
  
  /// Graph Error for spread EE+ and EE-

  TGraphErrors *sigma_vs_ring[3];
  sigma_vs_ring[0] = new TGraphErrors();
  sigma_vs_ring[0]->SetMarkerStyle(20);
  sigma_vs_ring[0]->SetMarkerSize(1);
  sigma_vs_ring[0]->SetMarkerColor(kBlue+2);
  
  sigma_vs_ring[1] = new TGraphErrors();
  sigma_vs_ring[1]->SetMarkerStyle(20);
  sigma_vs_ring[1]->SetMarkerSize(1);
  sigma_vs_ring[1]->SetMarkerColor(kBlue+2);

  sigma_vs_ring[2] = new TGraphErrors();
  sigma_vs_ring[2]->SetMarkerStyle(20);
  sigma_vs_ring[2]->SetMarkerSize(1);
  sigma_vs_ring[2]->SetMarkerColor(kBlue+2);
 
  /// Graph for scale vs ring EE+, EE- and folded

  TGraphErrors *scale_vs_ring[3];
  scale_vs_ring[0] = new TGraphErrors();
  scale_vs_ring[0]->SetMarkerStyle(20);
  scale_vs_ring[0]->SetMarkerSize(1);
  scale_vs_ring[0]->SetMarkerColor(kBlue+2);

  scale_vs_ring[1] = new TGraphErrors();
  scale_vs_ring[1]->SetMarkerStyle(20);
  scale_vs_ring[1]->SetMarkerSize(1);
  scale_vs_ring[1]->SetMarkerColor(kBlue+2);

  scale_vs_ring[2] = new TGraphErrors();
  scale_vs_ring[2]->SetMarkerStyle(20);
  scale_vs_ring[2]->SetMarkerSize(1);
  scale_vs_ring[2]->SetMarkerColor(kBlue+2);
    
  
  TF1 *fgaus = new TF1("fgaus","gaus",-10,10);
  int np[3] = {0};

  /// Gaussian fit for EE+ and EE-

  for (int k = 0; k < 2 ; k++){
    for (int iring = 0; iring < 40; iring++){
      if (hspread[k][iring]-> GetEntries() == 0) continue;
      float e     = 0.5*hcmap[k]-> GetYaxis()->GetBinWidth(1);
      fgaus->SetParameter(1,1);
      fgaus->SetParameter(2,hspread[k][iring]->GetRMS());
      fgaus->SetRange(1-5*hspread[k][iring]->GetRMS(),1+5*hspread[k][iring]->GetRMS());
      hspread[k][iring]->Fit("fgaus","QR");
      sigma_vs_ring[k]-> SetPoint(np[k],iring,fgaus->GetParameter(2));
      sigma_vs_ring[k]-> SetPointError(np[k], e ,fgaus->GetParError(2));
      scale_vs_ring[k]-> SetPoint(np[k],iring,fgaus->GetParameter(1));
      scale_vs_ring[k]-> SetPointError(np[k],e,fgaus->GetParError(1));
      np[k]++;    
    }
  }
  
    for (int iring = 0; iring < 40; iring++){
      if (hspreadAll[iring]-> GetEntries() == 0) continue;
      float e     = 0.5*hcmap[0]-> GetYaxis()->GetBinWidth(1);
      fgaus->SetParameter(1,1);
      fgaus->SetParameter(2,hspreadAll[iring]->GetRMS());
      fgaus->SetRange(1-5*hspreadAll[iring]->GetRMS(),1+5*hspreadAll[iring]->GetRMS());
      hspreadAll[iring]->Fit("fgaus","QR");
      sigma_vs_ring[2]-> SetPoint(np[2],iring,fgaus->GetParameter(2));
      sigma_vs_ring[2]-> SetPointError(np[2], e ,fgaus->GetParError(2));
      scale_vs_ring[2]-> SetPoint(np[2],iring,fgaus->GetParameter(1));
      scale_vs_ring[2]-> SetPointError(np[2],e,fgaus->GetParError(1));
      np[2]++;    
    }

  /// Intercalibration constant vs phi
 
 /* TGraphErrors *IC_vs_phi[2];
  IC_vs_phi[0] = new TGraphErrors();
  IC_vs_phi[0]->SetMarkerStyle(20);
  IC_vs_phi[0]->SetMarkerSize(1);
  IC_vs_phi[0]->SetMarkerColor(kBlue+2);
  
  IC_vs_phi[1] = new TGraphErrors();
  IC_vs_phi[1]->SetMarkerStyle(20);
  IC_vs_phi[1]->SetMarkerSize(1);
  IC_vs_phi[1]->SetMarkerColor(kBlue+2);

  TH1F* Spread_vs_phi[2][360];
  
  for (int k = 0; k < 2; k++){
    for (int iphi = 0; iphi < 360 ; iphi++){
      if (k==0)
      {
       sprintf(hname,"hspread_vs_phi%02d_EEP",iphi);
       Spread_vs_phi[k][iphi] = new TH1F(hname, hname, nbins,0.,2.);

      }
      else{ 
        sprintf(hname,"hspread_vs_ring%02d_EEM",iphi);
        Spread_vs_phi[k][iphi] = new TH1F(hname, hname, nbins,0.,2.);}
     } 
   }
  

  for (int k = 0; k < 2 ; k++){
    for (int ix = 1; ix < 101; ix++){
      for (int iy = 1; iy < 101; iy++){
	float iphibin;
        if((atan2(iy-50.,ix-50.)-int(atan2(iy-50.,ix-50.)))*(360./(2.*3.14159)) <0.5) iphibin=floor(180.+atan2(iy-50.,ix-50.)*(360./(2.*3.14159)));
        else iphibin=ceil(180.+atan2(iy-50.,ix-50.)*(360./(2.*3.14159)));
       
        if(iphibin>359) iphibin=359;
	int mybin = hcmap[k] -> FindBin(ix,iy);
	float ic = hcmap[k]->GetBinContent(mybin);          
	if ( ic>0 )    {
	 Spread_vs_phi[k][iphibin] ->Fill(ic);
	}
      }
    }
  }
 
 int N[2]={0};
  for (int k = 0; k < 2 ; k++){
    for (int iphi = 0; iphi < 360; iphi++){
      if (Spread_vs_phi[k][iphi]-> GetEntries() == 0) continue;
      IC_vs_phi[k]-> SetPoint(N[k],iphi,Spread_vs_phi[k][iphi]->GetMean());
      IC_vs_phi[k]-> SetPointError(N[k],0,Spread_vs_phi[k][iphi]->GetRMS()/sqrt(Spread_vs_phi[k][iphi]->GetEntries()));

      N[k]++;    
    }
  }
  */

 ///----------------- Statistical Precision  and Residual --------------------

    TGraphErrors *statprecision_vs_ring[3];
    statprecision_vs_ring[0]  = new TGraphErrors();
    statprecision_vs_ring[0]->SetMarkerStyle(20);
    statprecision_vs_ring[0]->SetMarkerSize(1);
    statprecision_vs_ring[0]->SetMarkerColor(kRed+2);

    statprecision_vs_ring[1]  = new TGraphErrors();
    statprecision_vs_ring[1]->SetMarkerStyle(20);
    statprecision_vs_ring[1]->SetMarkerSize(1);
    statprecision_vs_ring[1]->SetMarkerColor(kRed+2);

    statprecision_vs_ring[2]  = new TGraphErrors();
    statprecision_vs_ring[2]->SetMarkerStyle(20);
    statprecision_vs_ring[2]->SetMarkerSize(1);
    statprecision_vs_ring[2]->SetMarkerColor(kRed+2);
    
    TGraphErrors *residual_vs_ring[3];
    residual_vs_ring[0] = new TGraphErrors();
    residual_vs_ring[0]->SetMarkerStyle(20);
    residual_vs_ring[0]->SetMarkerSize(1);
    residual_vs_ring[0]->SetMarkerColor(kGreen+2);
  
    residual_vs_ring[1] = new TGraphErrors();
    residual_vs_ring[1]->SetMarkerStyle(20);
    residual_vs_ring[1]->SetMarkerSize(1);
    residual_vs_ring[1]->SetMarkerColor(kGreen+2);
  
    residual_vs_ring[2] = new TGraphErrors();
    residual_vs_ring[2]->SetMarkerStyle(20);
    residual_vs_ring[2]->SetMarkerSize(1);
    residual_vs_ring[2]->SetMarkerColor(kGreen+2);

  
  if (evalStat){
    
    /// acquisition file for statistical precision

    TFile *f2 = new TFile(infile2);
    TFile *f3 = new TFile(infile3);
    TH2F *hcmap2[2];
    hcmap2[0] = (TH2F*)f2->Get("h_scale_map_EEM"); 
    hcmap2[1] = (TH2F*)f2->Get("h_scale_map_EEP");

    TH2F *hcmap3[2];
    hcmap3[0] = (TH2F*)f3->Get("h_scale_map_EEM"); 
    hcmap3[1] = (TH2F*)f3->Get("h_scale_map_EEP");

    TH1F *hstatprecision[2][40];
    TH1F *hstatprecisionAll[40];

    /// stat precision histos for each EE ring
    for (int k = 0; k < 2; k++){
      for (int iring = 0; iring < 40 ; iring ++){
      
	if (k==0)
	 { sprintf(hname,"hstatprecisionAll_ring%02d",iring);
           hstatprecisionAll[iring] = new TH1F(hname, hname, nbins,-2.,2.);
           sprintf(hname,"hstatprecisionEEM_ring%02d",iring);
           hstatprecision[k][iring] = new TH1F(hname, hname, nbins,-2.,2.);
          }
	else {
	      sprintf(hname,"hstatprecisionEEP_ring%02d",iring);
              hstatprecision[k][iring] = new TH1F(hname, hname, nbins,-2.,2.);
            }

      }
    }
    
    for (int k = 0; k < 2 ; k++){
      for (int ix = 1; ix < 102; ix++){
	for (int iy = 1; iy < 102; iy++){
	  int iz = k;
	  if (k==0) iz = -1;
	  int mybin = hcmap2[k] -> FindBin(ix,iy);
	  int ring  = hrings[1]-> GetBinContent(mybin);
	  float ic1 = hcmap2[k]->GetBinContent(mybin);
	  float ic2 = hcmap3[k]->GetBinContent(mybin);
          if (ic1>0 && ic2 >0){
	    hstatprecision[k][ring]->Fill((ic1-ic2)/(ic1+ic2)); /// sigma (diff/sum) gives the stat. precision on teh entire sample
            hstatprecisionAll[ring]->Fill((ic1-ic2)/(ic1+ic2));
	  }
	}
      }
    }

  
    TCanvas* c44 [120];
    /// Gaussian fit of the even/odd distribution (rms of the distribution can be also used)
    int n[3] = {0};
    for (int k = 0; k < 2; k++){
      for (int iring = 0; iring < 40 ; iring++){
	if ( hstatprecision[k][iring]->GetEntries() == 0) continue;
	float e     = 0.5*hcmap2[k]-> GetYaxis()->GetBinWidth(1);
	fgaus->SetParameter(1,1);
	fgaus->SetParameter(2,hstatprecision[k][iring]->GetRMS());
	fgaus->SetRange(-5*hstatprecision[k][iring]->GetRMS(),5*hstatprecision[k][iring]->GetRMS());
	TString name = Form("ff%d_%d",iring,k);

        hstatprecision[k][iring]->Fit("fgaus","QR");
       
	statprecision_vs_ring[k]-> SetPoint(n[k],iring,fgaus->GetParameter(2));
	statprecision_vs_ring[k]-> SetPointError(n[k],e,fgaus->GetParError(2));
	n[k]++;
      }
    }
    
    for (int iring = 0; iring < 40 ; iring++){
	if ( hstatprecisionAll[iring]->GetEntries() == 0) continue;
	float e     = 0.5*hcmap2[0]-> GetYaxis()->GetBinWidth(1);
	fgaus->SetParameter(1,1);
	fgaus->SetParameter(2,hstatprecisionAll[iring]->GetRMS());
	fgaus->SetRange(-5*hstatprecisionAll[iring]->GetRMS(),5*hstatprecisionAll[iring]->GetRMS());
	TString name = Form("ffAll%d",iring);
        hstatprecisionAll[iring]->Fit("fgaus","QR");
       
	statprecision_vs_ring[2]-> SetPoint(n[2],iring,fgaus->GetParameter(2));
	statprecision_vs_ring[2]-> SetPointError(n[2],e,fgaus->GetParError(2));
	n[2]++;
      }
     
    
    TH1F *hresidual[3];
    hresidual[0] = new TH1F("hresidualEEM","hresidualEEM",1000,0,1);
    hresidual[1] = new TH1F("hresidualEEP","hresidualEEP",1000,0,1);
    hresidual[2] = new TH1F("hresidualAll","hresidualAll",1000,0,1);

    TH1F *hstat[3];
    hstat[0] = new TH1F("hstatEEM","hstatEEM",1000,0,0.5);
    hstat[1] = new TH1F("hstatEEP","hstatEEP",1000,0,0.5);
    hstat[2] = new TH1F("hstatAll","hstatAll",1000,0,0.5);
   
    TH1F *hspre[3];
    hspre[0] = new TH1F("hspreEEM","hspreEEM",1000,0,0.5);
    hspre[1] = new TH1F("hspreEEP","hspreEEP",1000,0,0.5);
    hspre[2] = new TH1F("hspreAll","hspreAll",1000,0,0.5);

    /// Residual spread plot

    for (int k = 0; k < 3 ; k++){
      for (int i= 0; i < statprecision_vs_ring[k]-> GetN(); i++){
	double spread, espread;
	double stat, estat;
	double residual, eresidual;
	double xdummy,ex;
	sigma_vs_ring[k]-> GetPoint(i, xdummy, spread );
	espread = sigma_vs_ring[k]-> GetErrorY(i);
	statprecision_vs_ring[k]-> GetPoint(i, xdummy, stat );
	estat = statprecision_vs_ring[k]-> GetErrorY(i);
	ex = statprecision_vs_ring[k]-> GetErrorX(i);
	if (spread > stat ){
	  residual  = sqrt( spread*spread - stat*stat );
	  eresidual = sqrt( pow(spread*espread,2) + pow(stat*estat,2))/residual;
	}
	else {
	  residual = 0;
	  eresidual = 0;
	}
	residual_vs_ring[k]->SetPoint(i,xdummy, residual);
	residual_vs_ring[k]->SetPointError(i,ex,eresidual);
      }
    }
 
 }
  
  ///-----------------------------------------------------------------
  ///--- Draw plots
  ///-----------------------------------------------------------------
  TCanvas *cEEP[12];
  TCanvas *cEEM[12];

  /// --- plot 0 : map of coefficients 
  cEEP[0] = new TCanvas("cmapEEP","cmapEEP");
  cEEP[0] -> cd();
  cEEP[0]->SetLeftMargin(0.1); 
  cEEP[0]->SetRightMargin(0.13); 
  cEEP[0]->SetGridx();
  cEEP[0]->SetGridy();
  //  hcmap[1]->GetXaxis()->SetNdivisions(1020);
  hcmap[1]->GetXaxis() -> SetLabelSize(0.03);
  hcmap[1]->Draw("COLZ");
  hcmap[1]->GetXaxis() ->SetTitle("ix");
  hcmap[1]->GetYaxis() ->SetTitle("iy");
  hcmap[1]->GetZaxis() ->SetRangeUser(0.8,1.2);

  cEEM[0] = new TCanvas("cmapEEM","cmapEEM");
  cEEM[0] -> cd();
  cEEM[0]->SetLeftMargin(0.1); 
  cEEM[0]->SetRightMargin(0.13); 
  cEEM[0]->SetGridx();
  cEEM[0]->SetGridy();
  //hcmap[0]->GetXaxis()->SetNdivisions(1020);
  hcmap[0]->GetXaxis() -> SetLabelSize(0.03);
  hcmap[0]->Draw("COLZ");
  hcmap[0]->GetXaxis() ->SetTitle("ix");
  hcmap[0]->GetYaxis() ->SetTitle("iy");
  hcmap[0]->GetZaxis() ->SetRangeUser(0.8,1.2);

  /// --- plot 1 : ring precision vs ieta
  cEEP[1] = new TCanvas("csigmaEEP","csigmaEEP");
  cEEP[1]->SetGridx();
  cEEP[1]->SetGridy();
  sigma_vs_ring[1]->GetHistogram()->GetYaxis()-> SetRangeUser(0.00,0.20);
  sigma_vs_ring[1]->GetHistogram()->GetXaxis()-> SetRangeUser(-85,85);
  sigma_vs_ring[1]->GetHistogram()->GetYaxis()-> SetTitle("#sigma_{c}");
  sigma_vs_ring[1]->GetHistogram()->GetXaxis()-> SetTitle("ring");
  sigma_vs_ring[1]->Draw("ap");
  if (evalStat){
    statprecision_vs_ring[1]->Draw("psame");
    sigma_vs_ring[1]->Draw("psame");
    TLegend * leg = new TLegend(0.6,0.7,0.89, 0.89);
    leg->SetFillColor(0);
    leg->AddEntry(statprecision_vs_ring[1],"statistical precision", "LP");
    leg->AddEntry(sigma_vs_ring[1],"spread", "LP");
    leg->Draw("same");
  }

  cEEM[1] = new TCanvas("csigmaEEM","csigmaEEM");
  cEEM[1]->SetGridx();
  cEEM[1]->SetGridy();
  sigma_vs_ring[0]->GetHistogram()->GetYaxis()-> SetRangeUser(0.00,0.20);
  sigma_vs_ring[0]->GetHistogram()->GetXaxis()-> SetRangeUser(-85,85);
  sigma_vs_ring[0]->GetHistogram()->GetYaxis()-> SetTitle("#sigma_{c}");
  sigma_vs_ring[0]->GetHistogram()->GetXaxis()-> SetTitle("ring");
  sigma_vs_ring[0]->Draw("ap");
  if (evalStat){
    statprecision_vs_ring[0]->Draw("psame");
    sigma_vs_ring[0]->Draw("psame");
    TLegend * leg = new TLegend(0.6,0.7,0.89, 0.89);
    leg->SetFillColor(0);
    leg->AddEntry(statprecision_vs_ring[0],"statistical precision", "LP");
    leg->AddEntry(sigma_vs_ring[0],"spread", "LP");
    leg->Draw("same");
  }


  /// --- plot 5 : statistical precision vs ieta
  if (evalStat){
    cEEP[5] = new TCanvas("cstat","cstat");
    cEEP[5]->SetGridx();
    cEEP[5]->SetGridy();
    statprecision_vs_ring[1]->GetHistogram()->GetYaxis()-> SetRangeUser(0.0001,0.10);
    statprecision_vs_ring[1]->GetHistogram()->GetXaxis()-> SetRangeUser(-85,85);
    statprecision_vs_ring[1]->GetHistogram()->GetYaxis()-> SetTitle("#sigma((c_{P}-c_{D})/(c_{P}+c_{D}))");
    statprecision_vs_ring[1]->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
    statprecision_vs_ring[1]->Draw("ap");
    
    cEEP[6] = new TCanvas("cresidualP","cresidualP");
    cEEP[6]->SetGridx();
    cEEP[6]->SetGridy();
    residual_vs_ring[1]->GetHistogram()->GetYaxis()-> SetRangeUser(0.0001,0.10);
    residual_vs_ring[1]->GetHistogram()->GetXaxis()-> SetRangeUser(-85,85);
    residual_vs_ring[1]->GetHistogram()->GetYaxis()-> SetTitle("residual spread");
    residual_vs_ring[1]->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
    residual_vs_ring[1]->Draw("ap");
 
    cEEM[5] = new TCanvas("cstatM","cstatM");
    cEEM[5]->SetGridx();
    cEEM[5]->SetGridy();
    statprecision_vs_ring[0]->GetHistogram()->GetYaxis()-> SetRangeUser(0.0001,0.10);
    statprecision_vs_ring[0]->GetHistogram()->GetXaxis()-> SetRangeUser(-85,85);
    statprecision_vs_ring[0]->GetHistogram()->GetYaxis()-> SetTitle("#sigma((c_{P}-c_{D})/(c_{P}+c_{D}))");
    statprecision_vs_ring[0]->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
    statprecision_vs_ring[0]->Draw("ap");
    
    cEEM[6] = new TCanvas("cresidualM","cresidualM");
    cEEM[6]->SetGridx();
    cEEM[6]->SetGridy();
    residual_vs_ring[0]->GetHistogram()->GetYaxis()-> SetRangeUser(0.0001,0.10);
    residual_vs_ring[0]->GetHistogram()->GetXaxis()-> SetRangeUser(-85,85);
    residual_vs_ring[0]->GetHistogram()->GetYaxis()-> SetTitle("residual spread");
    residual_vs_ring[0]->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
    residual_vs_ring[0]->Draw("ap");
 
    cEEP[8] = new TCanvas("csigmaFolded","csigmaFolded");
    cEEP[8]->SetGridx();
    cEEP[8]->SetGridy();
    sigma_vs_ring[2]->GetHistogram()->GetYaxis()-> SetRangeUser(0.00,0.20);
    sigma_vs_ring[2]->GetHistogram()->GetXaxis()-> SetRangeUser(-85,85);
    sigma_vs_ring[2]->GetHistogram()->GetYaxis()-> SetTitle("#sigma_{c}");
    sigma_vs_ring[2]->GetHistogram()->GetXaxis()-> SetTitle("ring");
    sigma_vs_ring[2]->Draw("ap");
    if (evalStat){
    statprecision_vs_ring[2]->Draw("psame");
    sigma_vs_ring[2]->Draw("psame");
    TLegend * leg = new TLegend(0.6,0.7,0.89, 0.89);
    leg->SetFillColor(0);
    leg->AddEntry(statprecision_vs_ring[2],"statistical precision", "LP");
    leg->AddEntry(sigma_vs_ring[2],"spread", "LP");
    leg->Draw("same");
    }

    cEEP[9] = new TCanvas("cresidualFolded","cresidualFolded");
    cEEP[9]->SetGridx();
    cEEP[9]->SetGridy();
    residual_vs_ring[2]->GetHistogram()->GetYaxis()-> SetRangeUser(0.0001,0.10);
    residual_vs_ring[2]->GetHistogram()->GetXaxis()-> SetRangeUser(-85,85);
    residual_vs_ring[2]->GetHistogram()->GetYaxis()-> SetTitle("residual spread");
    residual_vs_ring[2]->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
    residual_vs_ring[2]->Draw("ap");

    /// save precision for MC comparison

    if(isMC==true)
    {
     TFile * output = new TFile ("StatPrec_MC_EE.root","RECREATE");
     output->cd();
     statprecision_vs_ring[0]->SetName("gr_stat_prec_EEP");
     statprecision_vs_ring[1]->SetName("gr_stat_prec_EEM");
     statprecision_vs_ring[2]->SetName("gr_stat_prec");

     statprecision_vs_ring[0]->Write();
     statprecision_vs_ring[1]->Write();
     statprecision_vs_ring[2]->Write();
    }
  }
 
  /// Comparison Plot MC - Data
  TCanvas* canEEP[10], *canEEM[10];
  if(isMC == true)
  {
 
   canEEM[0] = new TCanvas("ICComparison MC EEM","ICComparison MC EEM");
   canEEM[0]->SetGridx();
   canEEM[0]->SetGridy();
   ICComparison[0]->GetXaxis() -> SetLabelSize(0.03);
   ICComparison[0]->Draw("COLZ");
   ICComparison[0]->GetXaxis() ->SetTitle("ix");
   ICComparison[0]->GetYaxis() ->SetTitle("iy");
   ICComparison[0]->GetZaxis() ->SetRangeUser(0.85,1.15);
 
   canEEP[0] = new TCanvas("ICComparison MC EEP","ICComparison MC EEP");
   canEEP[0]->SetGridx();
   canEEP[0]->SetGridy();
   ICComparison[1]->GetXaxis() -> SetLabelSize(0.03);
   ICComparison[1]->Draw("COLZ");
   ICComparison[1]->GetXaxis() ->SetTitle("ix");
   ICComparison[1]->GetYaxis() ->SetTitle("iy");
   ICComparison[1]->GetZaxis() ->SetRangeUser(0.85,1.15);

   TFile* output = new TFile ("IC_MC_4Correction.root","RECREATE");
   output->cd();
   ICComparison[0]->Write();
   ICComparison[1]->Write();
   output->Close();
  }

  TFile *exisistingEE = new TFile ("existingEE.root","READ");
  TH2F* exmap = (TH2F*) exisistingEE->Get("endcap");

  TH2F* warning_Map_EEP = (TH2F*) exmap->Clone("warning_Map_EEP");
  warning_Map_EEP->Reset("ICMES");
  warning_Map_EEP->Reset();

  TH2F* warning_Map_EEM = (TH2F*) exmap->Clone("warning_Map_EEM");
  warning_Map_EEM->Reset("ICMES");
  warning_Map_EEM->Reset();


  if(isMC == false)
  {
   std::ofstream outTxt ("Calibration_Coefficient_EE_dinamic_alpha.txt",std::ios::out);

   outTxt << "---------------------------------------------------------------" << std::endl;
   outTxt << std::fixed << std::setprecision(0) << std::setw(10) << "iX"
          << std::fixed << std::setprecision(0) << std::setw(10) << "iY"
          << std::fixed << std::setprecision(0) << std::setw(10) << "iZ"
          << std::fixed << std::setprecision(6) << std::setw(15) << "IC"
          << std::fixed << std::setprecision(6) << std::setw(15) << "error"
          << std::endl;
   outTxt << "---------------------------------------------------------------" << std::endl;

   for (int ix = 1; ix < hcmap[0]->GetNbinsX()+1 ; ix ++)
     {
       for (int iy = 1; iy < hcmap[0] -> GetNbinsY()+1; iy++)
	 {
	   if( exmap->GetBinContent(ix,iy) !=1) continue;
	   
	   double x,statPrec;
	   statprecision_vs_ring[0]->GetPoint(hrings[0]->GetBinContent(ix,iy),x,statPrec);
	   
	   outTxt << std::fixed << std::setprecision(0) << std::setw(10) << hcmap[0]->GetXaxis()->GetBinLowEdge(ix)
		  << std::fixed << std::setprecision(0) << std::setw(10) << hcmap[0]->GetYaxis()->GetBinLowEdge(iy)
		  << std::fixed << std::setprecision(0) << std::setw(10) << "-1";
	   
	   if( hcmap[0]->GetBinContent(ix,iy) == 0. )
	     {  
               outTxt << std::fixed << std::setprecision(6) << std::setw(15) << "-1."
                      << std::fixed << std::setprecision(6) << std::setw(15) << "999."
                      << std::endl;
	     }
	   else
	     {
	       outTxt << std::fixed << std::setprecision(6) << std::setw(15) << hcmap[0]->GetBinContent(ix,iy)
		      << std::fixed << std::setprecision(6) << std::setw(15) << statPrec
		      << std::endl;
	       
	       //warning_Map_EEM->Fill(ix,iy);
	     }
	  
	}
    }
    
    for (int ix = 1; ix < hcmap[1]->GetNbinsX()+1 ; ix ++)
      {
	for (int iy = 1; iy < hcmap[1] -> GetNbinsY()+1; iy++)
	  {
	    if( exmap->GetBinContent(ix,iy) !=1) continue;
	    
	    double x,statPrec;
	    statprecision_vs_ring[1]->GetPoint(hrings[1]->GetBinContent(ix,iy),x,statPrec);
	    
	    outTxt << std::fixed << std::setprecision(0) << std::setw(10) << hcmap[1]->GetXaxis()->GetBinLowEdge(ix)
		   << std::fixed << std::setprecision(0) << std::setw(10) << hcmap[1]->GetYaxis()->GetBinLowEdge(iy)
		   << std::fixed << std::setprecision(0) << std::setw(10) << "1";
	    
	    if( hcmap[1]->GetBinContent(ix,iy) == 0. )
	      {
		outTxt << std::fixed << std::setprecision(6) << std::setw(15) << "-1."
		       << std::fixed << std::setprecision(6) << std::setw(15) << "999."
		       << std::endl;
	      }
	    else
	      {
		outTxt << std::fixed << std::setprecision(6) << std::setw(15) << hcmap[1]->GetBinContent(ix,iy)
		       << std::fixed << std::setprecision(6) << std::setw(15) << statPrec
		       << std::endl;
		
		//warning_Map_EEM->Fill(ix,iy);                                                                                                                               
	      }
	    
	  }
      }
  }
  
  
  canEEP[1] = new TCanvas("Warning_EEP","Warning_EEP");
  canEEP[1]->SetGridx();
  canEEP[1]->SetGridy();
  warning_Map_EEP->GetXaxis() -> SetLabelSize(0.03);
  warning_Map_EEP->Draw("COLZ");
  warning_Map_EEP->GetXaxis() ->SetTitle("ix");
  warning_Map_EEP->GetYaxis() ->SetTitle("iy");
  warning_Map_EEP->GetZaxis() ->SetRangeUser(0.85,1.15);

  canEEM[1] = new TCanvas("Warning_EEM","Warning_EEM");
  canEEM[1]->SetGridx();
  canEEM[1]->SetGridy();
  warning_Map_EEM->GetXaxis() -> SetLabelSize(0.03);
  warning_Map_EEM->Draw("COLZ");
  warning_Map_EEM->GetXaxis() ->SetTitle("ix");
  warning_Map_EEM->GetYaxis() ->SetTitle("iy");
  warning_Map_EEM->GetZaxis() ->SetRangeUser(0.85,1.15);



}

