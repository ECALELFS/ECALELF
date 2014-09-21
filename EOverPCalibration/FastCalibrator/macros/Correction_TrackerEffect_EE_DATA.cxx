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
/// Macro to Correct DATA dinamic alpha with MC/MCTruth for phi lines due to the tracker
//


void Correction_TrackerEffect_EE_DATA_dinamic_alpha (Char_t* infile1 = "/data1/rgerosa/L3_Weight/PromptSkim_Single_Double_Electron_recoFlag/EE/fbrem/WZAnalysis_PromptSkim_W-DoubleElectron_FT_R_42_V21B_regression_Z_fbrem_EE.root",
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
  gStyle->SetOptStat(0); 
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

  TFile *f = new TFile(infile1,"UPDATE");
  TH2F *hcmap[2];
  hcmap[0] = (TH2F*)f->Get("h_scale_map_EEM");
  hcmap[1] = (TH2F*)f->Get("h_scale_map_EEP");
  
  
  /// ring geometry for the endcap
  TH2F *hrings[2];
  hrings[0] = (TH2F*)hcmap[0]->Clone("hringsEEM");
  hrings[1] = (TH2F*)hcmap[1]->Clone("hringsEEP");
  hrings[0] ->Reset("IMCES");
  hrings[1] ->Reset("IMCES");
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

  /// noEp Selections
/*  std::vector< std::pair <float,float> > phi_line_EEM;
  phi_line_EEM.push_back(std::pair<float,float> (16.,1.025));
  phi_line_EEM.push_back(std::pair<float,float> (59.5,1.047));
  phi_line_EEM.push_back(std::pair<float,float> (71.5,1.028));
  phi_line_EEM.push_back(std::pair<float,float> (86.,1.042));
  phi_line_EEM.push_back(std::pair<float,float> (103.,1.034));
  phi_line_EEM.push_back(std::pair<float,float> (122.5,1.024));
  phi_line_EEM.push_back(std::pair<float,float> (145.5,1.016));
  phi_line_EEM.push_back(std::pair<float,float> (164.,1.034));
  phi_line_EEM.push_back(std::pair<float,float> (183.5,1.043));
  phi_line_EEM.push_back(std::pair<float,float> (203.5,1.026));
  phi_line_EEM.push_back(std::pair<float,float> (240.,1.043));
  phi_line_EEM.push_back(std::pair<float,float> (296.5,1.031));
  phi_line_EEM.push_back(std::pair<float,float> (315.5,1.029));
  phi_line_EEM.push_back(std::pair<float,float> (356.,1.038));
 
  std::vector<std::pair <float,float> > phi_line_EEP;
  phi_line_EEP.push_back(std::pair<float,float> (16.5,1.042));
  phi_line_EEP.push_back(std::pair<float,float> (60.,1.043));
  phi_line_EEP.push_back(std::pair<float,float> (72.,1.016));
  phi_line_EEP.push_back(std::pair<float,float> (85.5,1.042));
  phi_line_EEP.push_back(std::pair<float,float> (103.,1.024));
  phi_line_EEP.push_back(std::pair<float,float> (122.5,1.025));
  phi_line_EEP.push_back(std::pair<float,float> (135.,1.035));
  phi_line_EEP.push_back(std::pair<float,float> (145.,1.016));
  phi_line_EEP.push_back(std::pair<float,float> (163.,1.034));
  phi_line_EEP.push_back(std::pair<float,float> (184.,1.033));
  phi_line_EEP.push_back(std::pair<float,float> (240.,1.054));
  phi_line_EEP.push_back(std::pair<float,float> (276.,1.026));
  phi_line_EEP.push_back(std::pair<float,float> (297.,1.043));
  phi_line_EEP.push_back(std::pair<float,float> (314.,1.024));
  phi_line_EEP.push_back(std::pair<float,float> (357.,1.055));
*/
 
/// R9 Selections
 
  std::vector< std::pair <float,float> > phi_line_EEM;
  phi_line_EEM.push_back(std::pair<float,float> (16.,1.02244));
  phi_line_EEM.push_back(std::pair<float,float> (59.5, 1.03235));
  phi_line_EEM.push_back(std::pair<float,float> (71.5,1.02545));
  phi_line_EEM.push_back(std::pair<float,float> (86.,1.04242));
  phi_line_EEM.push_back(std::pair<float,float> (103.,1.02082));
  phi_line_EEM.push_back(std::pair<float,float> (122.5,1.01224));
  phi_line_EEM.push_back(std::pair<float,float> (145.5, 1.01207));
  phi_line_EEM.push_back(std::pair<float,float> (164.,1.02766));
  phi_line_EEM.push_back(std::pair<float,float> (183.5,1.03397));
  phi_line_EEM.push_back(std::pair<float,float> (203.5,1.01597));
  phi_line_EEM.push_back(std::pair<float,float> (240.,1.03305));
  phi_line_EEM.push_back(std::pair<float,float> (296.5,1.02419));
  phi_line_EEM.push_back(std::pair<float,float> (315.5,1.02211));
  phi_line_EEM.push_back(std::pair<float,float> (356.,1.03243));

  std::vector<std::pair <float,float> > phi_line_EEP;
  phi_line_EEP.push_back(std::pair<float,float> (16.5,1.02514));
  phi_line_EEP.push_back(std::pair<float,float> (60.,1.02929));
  phi_line_EEP.push_back(std::pair<float,float> (72.,1.01563));
  phi_line_EEP.push_back(std::pair<float,float> (85.5,1.02029));
  phi_line_EEP.push_back(std::pair<float,float> (103.,1.02159));
  phi_line_EEP.push_back(std::pair<float,float> (122.5,1.02598));
  phi_line_EEP.push_back(std::pair<float,float> (135.,1.0163));
  phi_line_EEP.push_back(std::pair<float,float> (145.,1.03654));
  phi_line_EEP.push_back(std::pair<float,float> (163.,1.02249));
  phi_line_EEP.push_back(std::pair<float,float> (184.,1.04225));
  phi_line_EEP.push_back(std::pair<float,float> (240.,1.03254));
  phi_line_EEP.push_back(std::pair<float,float> (276.,1.01565));
  phi_line_EEP.push_back(std::pair<float,float> (297.,1.03486));
  phi_line_EEP.push_back(std::pair<float,float> (314.,1.01318));
  phi_line_EEP.push_back(std::pair<float,float> (357.,1.03728));

  TH2F *hcmap_corrected[2];
  hcmap_corrected[0] = (TH2F*)hcmap[0]->Clone("h_scale_map_EEM");
  hcmap_corrected[1] = (TH2F*)hcmap[1]->Clone("h_scale_map_EEP");
  hcmap_corrected[0] ->Reset("ICMES");
  hcmap_corrected[1] ->Reset("ICMES");
  hcmap_corrected[0] ->ResetStats();
  hcmap_corrected[1] ->ResetStats();

  /// Correction of IC map data

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
          { int ir = hrings[0]->GetBinContent(ix,iy);
            float deltaphi;
            if(ir<10  && ir>=1) deltaphi = (360./number_of_Rings_EEM.at(ir))*1.7;
            if(ir>=10 && ir<20) deltaphi = (360./number_of_Rings_EEM.at(ir))*1.4;
            if(ir>=20 && ir<33) deltaphi = (360./number_of_Rings_EEM.at(ir))*1.;
            if(ir>=33) hcmap_corrected[k]->SetBinContent(ix,iy,hcmap[k]->GetBinContent(ix,iy));
            float dphi;
            if(fabs(phi_xtal-phi_line_EEM.at(ivect).first)<180.) dphi=fabs(phi_xtal-phi_line_EEM.at(ivect).first);
            else dphi=360.-fabs(phi_xtal-phi_line_EEM.at(ivect).first);
            if(dphi<deltaphi) 
            {
             if(hcmap[k]->GetBinContent(ix,iy)>1.05) continue;
             hcmap_corrected[k]->SetBinContent(ix,iy,hcmap[k]->GetBinContent(ix,iy)*phi_line_EEM.at(ivect).second);
            }
            else if(flag==0) {hcmap_corrected[k]->SetBinContent(ix,iy,hcmap[k]->GetBinContent(ix,iy));
                              flag=1;}
          }
        }

       else{  int flag=0;
              for(int ivect =0 ; ivect <  phi_line_EEP.size() ; ivect++)
              {
                int ir = hrings[0]->GetBinContent(ix,iy);
                float deltaphi;
                if(ir<10 && ir>=1)  deltaphi = (360./number_of_Rings_EEP.at(ir))*1.7;
                if(ir>=10 && ir<20) deltaphi = (360./number_of_Rings_EEP.at(ir))*1.4;
                if(ir>=20 && ir<33) deltaphi = (360./number_of_Rings_EEP.at(ir))*1.;
                if(ir>=33) hcmap_corrected[k]->SetBinContent(ix,iy,hcmap[k]->GetBinContent(ix,iy));
                float dphi;
                if(fabs(phi_xtal-phi_line_EEP.at(ivect).first)<180.) dphi=fabs(phi_xtal-phi_line_EEP.at(ivect).first);
                else dphi=360.-fabs(phi_xtal-phi_line_EEP.at(ivect).first);
                if(dphi<deltaphi) 
                { if(hcmap[k]->GetBinContent(ix,iy)>1.05) continue;
                  hcmap_corrected[k]->SetBinContent(ix,iy,hcmap[k]->GetBinContent(ix,iy)*phi_line_EEP.at(ivect).second);
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

   /// --- plot 0 : map of coefficients 
  cEEP[1] = new TCanvas("cmapEEP Corrected","cmapEEP Corrected");
  cEEP[1] -> cd();
  cEEP[1]->SetLeftMargin(0.1); 
  cEEP[1]->SetRightMargin(0.13); 
  cEEP[1]->SetGridx();
  cEEP[1]->SetGridy();
  //  hcmap[1]->GetXaxis()->SetNdivisions(1020);
  hcmap_corrected[1]->GetXaxis() -> SetLabelSize(0.03);
  hcmap_corrected[1]->Draw("COLZ");
  hcmap_corrected[1]->GetXaxis() ->SetTitle("ix");
  hcmap_corrected[1]->GetYaxis() ->SetTitle("iy");
  hcmap_corrected[1]->GetZaxis() ->SetRangeUser(0.8,1.2);

  cEEM[1] = new TCanvas("cmapEEM Corrected","cmapEEM Corrected");
  cEEM[1] -> cd();
  cEEM[1]->SetLeftMargin(0.1); 
  cEEM[1]->SetRightMargin(0.13); 
  cEEM[1]->SetGridx();
  cEEM[1]->SetGridy();
  //hcmap[0]->GetXaxis()->SetNdivisions(1020);
  hcmap_corrected[0]->GetXaxis() -> SetLabelSize(0.03);
  hcmap_corrected[0]->Draw("COLZ");
  hcmap_corrected[0]->GetXaxis() ->SetTitle("ix");
  hcmap_corrected[0]->GetYaxis() ->SetTitle("iy");
  hcmap_corrected[0]->GetZaxis() ->SetRangeUser(0.8,1.2);
 
  f->cd();
  hcmap_corrected[0]->Write("h_scale_map_corrected_EEM");
  hcmap_corrected[1]->Write("h_scale_map_corrected_EEP");
 


}