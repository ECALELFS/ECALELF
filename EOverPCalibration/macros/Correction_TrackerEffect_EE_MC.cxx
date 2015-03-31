#include <vector>
#include <utility>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include "TStyle.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include <cmath>
#include "TFile.h"
#include "TCanvas.h"
//
/// Macro to produce correction taking MC and MCTruth to apply on data for phi structure due to the tracker
//


void Correction_TrackerEffect_EE_MC (Char_t* infile1 = "/afs/cern.ch/user/r/rgerosa/scratch0/FastCalibrator/output/IC_MC_4Correction.root",
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
  gStyle->SetTitleXOffset(0.8);
  gStyle->SetTitleYOffset(1.1);
  gROOT->ForceStyle();

  if ( !infile1 ) {
    cout << " No input file specified !" << endl;
    return;
  }


  cout << "Making calibration plots for: " << infile1 << endl;
  
  
  /// imput file with full statistic normlized to the mean in a ring

  TFile *f = new TFile(infile1);
  TH2F *hcmap[2];
  hcmap[0] = (TH2F*)f->Get("ICComparison_EEM");
  hcmap[1] = (TH2F*)f->Get("ICComparison_EEP");
  
  
  /// ring geometry for the endcap
  TH2F *hrings[2];
  hrings[0] = (TH2F*)hcmap[0]->Clone("hringsEEM");
  hrings[1] = (TH2F*)hcmap[0]->Clone("hringsEEP");
  hrings[0] ->Reset("ICMES");
  hrings[1] ->Reset("ICEMS");
  hrings[0] ->ResetStats();
  hrings[1] ->ResetStats();


  FILE *fRing;
  fRing = fopen("macros/eerings.dat","r");
  int x,y,z,ir;
  std::vector<int> number_of_Rings_EEP(40,0);
  std::vector<int> number_of_Rings_EEM(40,0);
  
  while(fscanf(fRing,"(%d,%d,%d) %d \n",&x,&y,&z,&ir) !=EOF ) {
    if(z>0) { hrings[1]->Fill(x,y,ir); 
              number_of_Rings_EEP.at(ir)++;
           }
    if(z<0) {hrings[0]->Fill(x,y,ir);
             number_of_Rings_EEM.at(ir)++;
            }
     }   
  /// Declaration of phi structure coordinate
  std::vector<float> phi_line_EEM;
  phi_line_EEM.push_back(16.);
  phi_line_EEM.push_back(59.5);
  phi_line_EEM.push_back(71.5);
  phi_line_EEM.push_back(83.5);
  phi_line_EEM.push_back(105.);
  phi_line_EEM.push_back(123.5);
  phi_line_EEM.push_back(145.);
  phi_line_EEM.push_back(164.);
  phi_line_EEM.push_back(183.5);
  phi_line_EEM.push_back(203.5);
  phi_line_EEM.push_back(240.);
  phi_line_EEM.push_back(297.);
  phi_line_EEM.push_back(314.5);
  phi_line_EEM.push_back(356.5);

  std::vector<float> phi_line_EEP;
  phi_line_EEP.push_back(16.5);
  phi_line_EEP.push_back(60.);
  phi_line_EEP.push_back(72.);
  phi_line_EEP.push_back(85.);
  phi_line_EEP.push_back(105.);
  phi_line_EEP.push_back(123.);
  phi_line_EEP.push_back(135.);
  phi_line_EEP.push_back(145.);
  phi_line_EEP.push_back(163.);
  phi_line_EEP.push_back(184.);
  phi_line_EEP.push_back(240.);
  phi_line_EEP.push_back(287.);
  phi_line_EEP.push_back(296.);
  phi_line_EEP.push_back(314.);
  phi_line_EEP.push_back(356.);

  /// Calcolation of correction factors
  std::vector<float> Bad_IC_EEP(phi_line_EEP.size(),0);
  std::vector<float> Good_IC_EEP(phi_line_EEP.size(),0);
  std::vector<float> Bad_IC_EEM(phi_line_EEM.size(),0);
  std::vector<float> Good_IC_EEM(phi_line_EEM.size(),0);
  std::vector<float> sum_Bad_IC_EEP(phi_line_EEP.size(),0),sum_Good_IC_EEP(phi_line_EEP.size(),0),sum_Good_IC_EEM(phi_line_EEM.size(),0),sum_Bad_IC_EEM(phi_line_EEM.size(),0);
  std::vector<float> num_Bad_IC_EEP(phi_line_EEP.size(),0),num_Good_IC_EEP(phi_line_EEP.size(),0),num_Good_IC_EEM(phi_line_EEM.size(),0),num_Bad_IC_EEM(phi_line_EEM.size(),0);

  TH2F *Bad_EEP = (TH2F*)  hcmap[0]->Clone("Bad_EEP");
  TH2F *Bad_EEM = (TH2F*)  hcmap[1]->Clone("Bad_EEM");
  Bad_EEP->Reset();
  Bad_EEM->Reset();
  Bad_EEP->ResetStats();
  Bad_EEM->ResetStats();


  for(int k=0 ; k<2 ; k++)
  { 
   for(int ix =0; ix < hcmap[k]->GetNbinsX(); ix++)
   {
    for(int iy =0; iy< hcmap[k]->GetNbinsY() ; iy++)
    {
      if(hcmap[k]->GetBinContent(ix,iy)==0 ) continue;
      float phi_xtal = atan2(iy-50.5,ix-50.5)*360./(2.*3.14159);
      if(phi_xtal<0.) phi_xtal=360.+phi_xtal;
      if(k==0)
        {
          for(int ivect =0 ; ivect <  phi_line_EEM.size() ; ivect++)
          {
            float deltaphi=0;
            int ir = hrings[0]->GetBinContent(ix,iy);
            if(ir<10 && ir>=1) deltaphi = (360./number_of_Rings_EEM.at(ir))*1.7;
            if(ir>=10 && ir<20) deltaphi = (360./number_of_Rings_EEM.at(ir))*1.4;
            if(ir>=20 && ir<33) deltaphi = (360./number_of_Rings_EEM.at(ir))*1.;
            if(ir>=32) continue;
            float dphi=0;
            if(fabs(phi_xtal-phi_line_EEM.at(ivect))<180.) dphi=fabs(phi_xtal-phi_line_EEM.at(ivect));
            else dphi=360.-fabs(phi_xtal-phi_line_EEM.at(ivect));
             if(dphi<deltaphi) {
             if(hcmap[k]->GetBinContent(ix,iy)>1.05) continue;
             sum_Bad_IC_EEM[ivect]=sum_Bad_IC_EEM[ivect]+hcmap[k]->GetBinContent(ix,iy);
             num_Bad_IC_EEM[ivect]++;
             Bad_EEM->Fill(ix,iy,hcmap[k]->GetBinContent(ix,iy));
            }
            else{
                  sum_Good_IC_EEM[ivect] = sum_Good_IC_EEM[ivect] +hcmap[k]->GetBinContent(ix,iy);
                  num_Good_IC_EEM[ivect]++;
                }
           }
        }

       else{
             for(int ivect =0 ; ivect <  phi_line_EEP.size() ; ivect++)
             {
              int ir = hrings[1]->GetBinContent(ix,iy);
              float deltaphi;
              if(ir<10 && ir>=1)  deltaphi = (360./number_of_Rings_EEP.at(ir))*1.7;
              if(ir>=10 && ir<20) deltaphi = (360./number_of_Rings_EEP.at(ir))*1.4;
              if(ir>=20 && ir<33) deltaphi = (360./number_of_Rings_EEP.at(ir))*1.;
              if(ir>=33) continue;
              float dphi;
              if(fabs(phi_xtal-phi_line_EEP.at(ivect))<180.) dphi=fabs(phi_xtal-phi_line_EEP.at(ivect));
              else dphi=360.-fabs(phi_xtal-phi_line_EEP.at(ivect));
         
              if(dphi<deltaphi){   
              if(hcmap[k]->GetBinContent(ix,iy)>1.05) continue;
              sum_Bad_IC_EEP[ivect]=sum_Bad_IC_EEP[ivect]+hcmap[k]->GetBinContent(ix,iy);
              num_Bad_IC_EEP[ivect]++;
              Bad_EEP->Fill(ix,iy,hcmap[k]->GetBinContent(ix,iy));}
              else{
                    sum_Good_IC_EEP[ivect] = sum_Good_IC_EEP[ivect] +hcmap[k]->GetBinContent(ix,iy);
                    num_Good_IC_EEP[ivect]++;}
                }
               
             }
           }
       }
     }
  /// Vectors of empiric correction factors
  for(int ivect =0 ; ivect <  phi_line_EEM.size() ; ivect++){
        if(num_Bad_IC_EEM[ivect]!=0)
        Bad_IC_EEM.at(ivect)=sum_Bad_IC_EEM[ivect]/num_Bad_IC_EEM[ivect];
        if(num_Good_IC_EEM[ivect]!=0)
        Good_IC_EEM.at(ivect)= sum_Good_IC_EEM[ivect]/num_Good_IC_EEM[ivect];
        cout<<" Good/Bad - "<< Good_IC_EEM.at(ivect)/Bad_IC_EEM.at(ivect)<<endl;
   }

   for(int ivect =0 ; ivect <  phi_line_EEP.size() ; ivect++){
        if(num_Bad_IC_EEP[ivect]!=0)
        Bad_IC_EEP.at(ivect)=sum_Bad_IC_EEP[ivect]/num_Bad_IC_EEP[ivect];
        if(num_Good_IC_EEP[ivect]!=0)
        Good_IC_EEP.at(ivect)= sum_Good_IC_EEP[ivect]/num_Good_IC_EEP[ivect];
        cout<<" Good/Bad + "<<Good_IC_EEP.at(ivect)/Bad_IC_EEP.at(ivect)<<endl;
  
   }

  /// MC map corrected to control the result of the method
  TH2F *hcmap_corrected[2];
  hcmap_corrected[0] = (TH2F*)hcmap[0]->Clone("h_scale_map_EEM");
  hcmap_corrected[1] = (TH2F*)hcmap[1]->Clone("h_scale_map_EEP");
  hcmap_corrected[0] ->Reset("ICMES");
  hcmap_corrected[1] ->Reset("ICMES");
  hcmap_corrected[0] ->ResetStats();
  hcmap_corrected[1] ->ResetStats();

   for(int k=0 ; k<2 ; k++)
   { 
    for(int ix =0; ix < hcmap[k]->GetNbinsX(); ix++)
    {
     for(int iy =0; iy< hcmap[k]->GetNbinsY() ; iy++)
     {
      if(hcmap[k]->GetBinContent(ix,iy)==0 ) continue;
      float phi_xtal = atan2(iy-50.5,ix-50.5)*360./(2.*3.14159);
      if(phi_xtal<0.) phi_xtal=360.+phi_xtal;
      if(k==0)
        { int flag=0;
          for(int ivect =0 ; ivect <  phi_line_EEM.size() ; ivect++)
          {
            int ir = hrings[0]->GetBinContent(ix,iy);
            float deltaphi;
            if(ir<10  && ir>=1) deltaphi = (360./number_of_Rings_EEM.at(ir))*1.7;
            if(ir>=10 && ir<20) deltaphi = (360./number_of_Rings_EEM.at(ir))*1.4;
            if(ir>=20 && ir<33) deltaphi = (360./number_of_Rings_EEM.at(ir))*1.;
            if(ir>=33) hcmap_corrected[k]->SetBinContent(ix,iy,hcmap[k]->GetBinContent(ix,iy));
            float dphi;
            if(fabs(phi_xtal-phi_line_EEM.at(ivect))<180.) dphi=fabs(phi_xtal-phi_line_EEM.at(ivect));
            else dphi=360.-fabs(phi_xtal-phi_line_EEM.at(ivect));
            if(dphi<deltaphi) 
            { if(hcmap[k]->GetBinContent(ix,iy)>1.05) continue;
              hcmap_corrected[k]->SetBinContent(ix,iy,hcmap[k]->GetBinContent(ix,iy)*Good_IC_EEM.at(ivect)/Bad_IC_EEM.at(ivect));
//              cout<<"original "<<hcmap[k]->GetBinContent(ix,iy)<<" Corrected "<<hcmap_corrected[k]->GetBinContent(ix,iy)<<endl;
            }
            else if(flag==0) {
                              hcmap_corrected[k]->SetBinContent(ix,iy,hcmap[k]->GetBinContent(ix,iy));
                              flag=1;}
          }
        }

       else{  int flag=0;
              for(int ivect =0 ; ivect <  phi_line_EEP.size() ; ivect++)
              {
                int ir = hrings[0]->GetBinContent(ix,iy);
                float deltaphi;
                if(ir<10 && ir>=1) deltaphi = (360./number_of_Rings_EEP.at(ir))*1.7;
                if(ir>=10 && ir<20) deltaphi = (360./number_of_Rings_EEP.at(ir))*1.4;
                if(ir>=20 && ir<33) deltaphi = (360./number_of_Rings_EEP.at(ir))*1.;
                if(ir>=33) hcmap_corrected[k]->SetBinContent(ix,iy,hcmap[k]->GetBinContent(ix,iy));
                float dphi;
                if(fabs(phi_xtal-phi_line_EEP.at(ivect))<180.) dphi=fabs(phi_xtal-phi_line_EEP.at(ivect));
                else dphi=360.-fabs(phi_xtal-phi_line_EEP.at(ivect));
                if(dphi<deltaphi) 
                { 
                  if(hcmap[k]->GetBinContent(ix,iy)>1.05) continue;
                  hcmap_corrected[k]->SetBinContent(ix,iy,hcmap[k]->GetBinContent(ix,iy)*Good_IC_EEP.at(ivect)/Bad_IC_EEP.at(ivect));
                 }
                else if(flag==0)
                     {
                        hcmap_corrected[k]->SetBinContent(ix,iy,hcmap[k]->GetBinContent(ix,iy));
                        flag=1;
                     }
              }
            }
   
       }
     }
  }
 
  TCanvas *cEEP[12];
  TCanvas *cEEM[12];
 
   /// --- plot 0 : map of coefficients 
  cEEP[0] = new TCanvas("cEEP","cmapEEP Uncorrected");
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

  cEEM[0] = new TCanvas("cEEM","cmapEEM Uncorrected");
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


  // --- plot 0 : map of coefficients 
  cEEP[1] = new TCanvas("cmapEEP","cmapEEP");
  cEEP[1] -> cd();
  cEEP[1]->SetLeftMargin(0.1); 
  cEEP[1]->SetRightMargin(0.13); 
  cEEP[1]->SetGridx();
  cEEP[1]->SetGridy();
  //  hcmap[1]->GetXaxis()->SetNdivisions(1020);
  Bad_EEP->GetXaxis() -> SetLabelSize(0.03);
  Bad_EEP->Draw("COLZ");
  Bad_EEP->GetXaxis() ->SetTitle("ix");
  Bad_EEP->GetYaxis() ->SetTitle("iy");
  Bad_EEP->GetZaxis() ->SetRangeUser(0.85,1.15);

  cEEM[1] = new TCanvas("cmapEEM","cmapEEM");
  cEEM[1] -> cd();
  cEEM[1]->SetLeftMargin(0.1); 
  cEEM[1]->SetRightMargin(0.13); 
  cEEM[1]->SetGridx();
  cEEM[1]->SetGridy();
  //hcmap[0]->GetXaxis()->SetNdivisions(1020);
  Bad_EEM->GetXaxis() -> SetLabelSize(0.03);
  Bad_EEM->Draw("COLZ");
  Bad_EEM->GetXaxis() ->SetTitle("ix");
  Bad_EEM->GetYaxis() ->SetTitle("iy");
  Bad_EEM->GetZaxis() ->SetRangeUser(0.85,1.15);
  
   // --- plot 0 : map of coefficients 
  cEEP[2] = new TCanvas("cmapEEP Corrected","cmapEEP Corrected");
  cEEP[2] -> cd();
  cEEP[2]->SetLeftMargin(0.1); 
  cEEP[2]->SetRightMargin(0.13); 
  cEEP[2]->SetGridx();
  cEEP[2]->SetGridy();
  //  hcmap[1]->GetXaxis()->SetNdivisions(1020);
  hcmap_corrected[1]->GetXaxis() -> SetLabelSize(0.03);
  hcmap_corrected[1]->Draw("COLZ");
  hcmap_corrected[1]->GetXaxis() ->SetTitle("ix");
  hcmap_corrected[1]->GetYaxis() ->SetTitle("iy");
  hcmap_corrected[1]->GetZaxis() ->SetRangeUser(0.8,1.2);

  cEEM[2] = new TCanvas("cmapEEM Corrected","cmapEEM Corrected");
  cEEM[2] -> cd();
  cEEM[2]->SetLeftMargin(0.1); 
  cEEM[2]->SetRightMargin(0.13); 
  cEEM[2]->SetGridx();
  cEEM[2]->SetGridy();
  //hcmap[0]->GetXaxis()->SetNdivisions(1020);
  hcmap_corrected[0]->GetXaxis() -> SetLabelSize(0.03);
  hcmap_corrected[0]->Draw("COLZ");
  hcmap_corrected[0]->GetXaxis() ->SetTitle("ix");
  hcmap_corrected[0]->GetYaxis() ->SetTitle("iy");
  hcmap_corrected[0]->GetZaxis() ->SetRangeUser(0.8,1.2);
 
  




}