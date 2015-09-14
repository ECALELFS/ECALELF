//
// Macro to Normalize DATA IC Coefficient along one ring skipping dead channels and TT
// Input File Scale Map of a data sample (can be used also on MC) an this is saved in the same
// file updating the old scale_map normalized by rings without skip dead channels
#include <vector>
#include <utility>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include "TH2F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TStyle.h"
#include <cmath>

using namespace std;

// Check if the crystal is near to a dead one

bool CheckxtalIC (TH2F* h_scale_EE,int ix, int iy, int ir)
{
  if(h_scale_EE->GetBinContent(ix,iy) ==0) return false;
  
  int bx= h_scale_EE->GetNbinsX();
  int by= h_scale_EE->GetNbinsY();

  if((ix<bx && h_scale_EE->GetBinContent(ix+1,iy) ==0 && (ir!=0 || ir<33)) || (h_scale_EE->GetBinContent(ix-1,iy)==0 && ix>1 && (ir!=0 || ir<33))) return false;

  if((iy<by && h_scale_EE->GetBinContent(ix,iy+1) ==0 && (ir!=0 || ir<33)) || (h_scale_EE->GetBinContent(ix,iy-1)==0 && iy>1 && (ir!=0 || ir<33))) return false;

  if((ix<bx && h_scale_EE->GetBinContent(ix+1,iy+1) ==0 && iy<by && (ir!=0 || ir<33)) || ( h_scale_EE->GetBinContent(ix-1,iy-1)==0 && iy>1 && ix>1 && (ir!=0 || ir<33))) return false;
 
  if((h_scale_EE->GetBinContent(ix+1,iy-1) ==0 && iy>1 && ix<bx && (ir!=0 || ir<33)) || ( h_scale_EE->GetBinContent(ix-1,iy+1)==0 && ix>1 && iy<by && (ir!=0 || ir<33))) return false;

  return true;

}

// Check if the crystal is near to a dead TT
//fede: only 7x7 around dead TTs (not 9x9)

bool CheckxtalTT (int ix, int iy, int ir, std::vector<std::pair<int,int> >& TT_centre )
{
 for( int k =0; k<TT_centre.size(); k++)
 {
   if(fabs(ix-TT_centre.at(k).first)<4 && fabs(iy-TT_centre.at(k).second)<4) return false;

 }
 return true;
}



void Normalize_IC_EE(     
		     //Char_t* infile1 = "data_LC_20120131_ALPHA_test_prompt/SingleElectron_Run2011AB-WElectron-data_LC_20120131_ALPHA_test_prompt_EoPcaibEE_11032011_Z_R9_EE.root",
		     //Char_t* infile1 = "data_LC_20120131_ALPHA_test_prompt/Even_SingleElectron_Run2011AB-WElectron-data_LC_20120131_ALPHA_test_prompt_EoPcaibEE_11032011_Z_R9_EE.root",
		     Char_t* infile1 = "data_LC_20120131_ALPHA_test_prompt/Odd_SingleElectron_Run2011AB-WElectron-data_LC_20120131_ALPHA_test_prompt_EoPcaibEE_11032011_Z_R9_EE.root",

		     //Char_t* infile1 = "FT_R_42_V21B/WZAnalysis_PromptSkim_W-DoubleElectron_FT_R_42_V21B_Z_R9_EE.root",
		     //Char_t* infile1 = "FT_R_42_V21B/Even_WZAnalysis_PromptSkim_W-DoubleElectron_FT_R_42_V21B_Z_R9_EE.root",
		     //Char_t* infile1 = "FT_R_42_V21B/Odd_WZAnalysis_PromptSkim_W-DoubleElectron_FT_R_42_V21B_Z_R9_EE.root",

			  Char_t* fileType = "png", 
			  Char_t* dirName = ".")
{

  bool  printPlots = false;

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
  gStyle->SetTitleXOffset(0.8);
  gStyle->SetTitleYOffset(1.1);
  gROOT->ForceStyle();

  if ( !infile1 ) {
    cout << " No input file specified !" << endl;
    return;
  }

 // map for dead TT centre

 std::vector< std::pair<int,int> > TT_centre_EEP ;
 std::vector< std::pair<int,int> > TT_centre_EEM ;

 TT_centre_EEP.push_back(std::pair<int,int> (78,78));
 TT_centre_EEP.push_back(std::pair<int,int> (83,28));
 TT_centre_EEP.push_back(std::pair<int,int> (83,23));
 TT_centre_EEM.push_back(std::pair<int,int> (53,28)); 

  //  Input file with full statistic
 
 TFile *f = new TFile(infile1,"UPDATE");
 TH2F* h_scale_EE[2],*hcmap_EE[2];

 h_scale_EE[1] = (TH2F*)f->Get("h_scale_EEP");
 h_scale_EE[0] = (TH2F*)f->Get("h_scale_EEM");

 hcmap_EE[1] = (TH2F*) h_scale_EE[1]->Clone("hcmap_EEP");
 hcmap_EE[0] = (TH2F*) h_scale_EE[0]->Clone("hcmap_EEM");
  
 hcmap_EE[0] -> Reset("ICEMS");
 hcmap_EE[1] -> Reset("ICEMS");
 hcmap_EE[0] -> ResetStats();
 hcmap_EE[1] -> ResetStats();
 
 
 TH2F *hrings[2];
 hrings[0] = (TH2F*)h_scale_EE[0]->Clone("hringsEEM");
 hrings[1] = (TH2F*)h_scale_EE[1]->Clone("hringsEEP");
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

  std::vector<float> SumIC_Ring_EEP,SumIC_Ring_EEM,Sumxtal_Ring_EEP,Sumxtal_Ring_EEM;
  
  SumIC_Ring_EEP.assign(40,0);
  SumIC_Ring_EEM.assign(40,0);
  Sumxtal_Ring_EEP.assign(40,0);
  Sumxtal_Ring_EEM.assign(40,0);
 
  // Mean over phi corrected skipping dead channel 
  for(int k=0; k<2 ; k++)
  {
    for( int ix = 0; ix < h_scale_EE[k]->GetNbinsX()+1 ; ix++ ){
 
     for(int iy = 0; iy < h_scale_EE[k]->GetNbinsY()+1 ; iy++ ){
       
       ir = hrings[k]->GetBinContent(ix,iy);
       
       bool isGood = CheckxtalIC(h_scale_EE[k],ix,iy,ir);
       bool isGoodTT;
       if(k==0) isGoodTT = CheckxtalTT(ix,iy,ir,TT_centre_EEM);
       else isGoodTT = CheckxtalTT(ix,iy,ir,TT_centre_EEP);
 
       if(k!=0 && isGoodTT && isGood )
       {
        SumIC_Ring_EEP.at(ir) = SumIC_Ring_EEP.at(ir) + h_scale_EE[k]->GetBinContent(ix,iy);
        Sumxtal_Ring_EEP.at(ir) = Sumxtal_Ring_EEP.at(ir) + 1.;
       }
       if(k==0 && isGoodTT && isGood)
       {
              SumIC_Ring_EEM.at(ir) = SumIC_Ring_EEM.at(ir) + h_scale_EE[k]->GetBinContent(ix,iy);
              Sumxtal_Ring_EEM.at(ir) = Sumxtal_Ring_EEM.at(ir) + 1.;
       }
               
       }
    }
  }
  
  for(int k=0; k<2 ; k++)
    {
      for( int ix = 0; ix < h_scale_EE[k]->GetNbinsX()+1 ; ix++ )
	{	
	  for(int iy = 0; iy < h_scale_EE[k]->GetNbinsY()+1 ; iy++ )
	    {
	      ir = hrings[k]->GetBinContent(ix,iy);
	      
	      //fede: skip bad channels and bad TTs    
	      bool isGood = CheckxtalIC(h_scale_EE[k],ix,iy,ir);
	      bool isGoodTT;
	      if(k==0) isGoodTT = CheckxtalTT(ix,iy,ir,TT_centre_EEM);
	      else isGoodTT = CheckxtalTT(ix,iy,ir,TT_centre_EEP);
	      if (!isGood || !isGoodTT) continue;

	      if(k!=0)
		{
		  if(ir>33){  
		    hcmap_EE[k]->Fill(ix,iy,0.);
		    continue;
		  }
		  if(Sumxtal_Ring_EEP.at(ir) != 0 && SumIC_Ring_EEP.at(ir)!= 0)
		    hcmap_EE[k]->Fill(ix,iy,h_scale_EE[k]->GetBinContent(ix,iy)/(SumIC_Ring_EEP.at(ir)/Sumxtal_Ring_EEP.at(ir)));
		}
	      else
		{
		  if(ir>33){
		    hcmap_EE[k]->Fill(ix,iy,0.);
		    continue;
		  }
		  if(Sumxtal_Ring_EEM.at(ir) != 0 && SumIC_Ring_EEM.at(ir) != 0)
		    hcmap_EE[k]->Fill(ix,iy,h_scale_EE[k]->GetBinContent(ix,iy)/(SumIC_Ring_EEM.at(ir)/Sumxtal_Ring_EEM.at(ir)));
		}
	    }
	}
    }
  
  TCanvas *cEEP[12];
  TCanvas *cEEM[12];
 
   // --- plot 0 : map of coefficients 
  cEEP[0] = new TCanvas("cEEP","cmapEEP not normalized");
  cEEP[0] -> cd();
  cEEP[0]->SetLeftMargin(0.1); 
  cEEP[0]->SetRightMargin(0.13); 
  cEEP[0]->SetGridx();
  cEEP[0]->SetGridy();
  //  hcmap[1]->GetXaxis()->SetNdivisions(1020);
  h_scale_EE[1]->GetXaxis() -> SetLabelSize(0.03);
  h_scale_EE[1]->Draw("COLZ");
  h_scale_EE[1]->GetXaxis() ->SetTitle("ix");
  h_scale_EE[1]->GetYaxis() ->SetTitle("iy");
  h_scale_EE[1]->GetZaxis() ->SetRangeUser(0.5,2.);

  cEEM[0] = new TCanvas("cEEM","cmapEEM not normalized");
  cEEM[0] -> cd();
  cEEM[0]->SetLeftMargin(0.1); 
  cEEM[0]->SetRightMargin(0.13); 
  cEEM[0]->SetGridx();
  cEEM[0]->SetGridy();
  //hcmap[0]->GetXaxis()->SetNdivisions(1020);
  h_scale_EE[0]->GetXaxis() -> SetLabelSize(0.03);
  h_scale_EE[0]->Draw("COLZ");
  h_scale_EE[0]->GetXaxis() ->SetTitle("ix");
  h_scale_EE[0]->GetYaxis() ->SetTitle("iy");
  h_scale_EE[0]->GetZaxis() ->SetRangeUser(0.5,2.);
  
  cEEP[1] = new TCanvas("cmapEEP Normalized","cmapEEP Normalized");
  cEEP[1] -> cd();
  cEEP[1]->SetLeftMargin(0.1); 
  cEEP[1]->SetRightMargin(0.13); 
  cEEP[1]->SetGridx();
  cEEP[1]->SetGridy();
  //  hcmap[1]->GetXaxis()->SetNdivisions(1020);
  hcmap_EE[1]->GetXaxis() -> SetLabelSize(0.03);
  hcmap_EE[1]->Draw("COLZ");
  hcmap_EE[1]->GetXaxis() ->SetTitle("ix");
  hcmap_EE[1]->GetYaxis() ->SetTitle("iy");
  hcmap_EE[1]->GetZaxis() ->SetRangeUser(0.8,1.2);

  cEEM[1] = new TCanvas("cmapEEM Normalized","cmapEEM Normalized");
  cEEM[1] -> cd();
  cEEM[1]->SetLeftMargin(0.1); 
  cEEM[1]->SetRightMargin(0.13); 
  cEEM[1]->SetGridx();
  cEEM[1]->SetGridy();
  //hcmap[0]->GetXaxis()->SetNdivisions(1020);
  hcmap_EE[0]->GetXaxis() -> SetLabelSize(0.03);
  hcmap_EE[0]->Draw("COLZ");
  hcmap_EE[0]->GetXaxis() ->SetTitle("ix");
  hcmap_EE[0]->GetYaxis() ->SetTitle("iy");
  hcmap_EE[0]->GetZaxis() ->SetRangeUser(0.8,1.2);
 

   f->cd();
   hcmap_EE[0]->Write("h_scale_map_EEM");
   hcmap_EE[1]->Write("h_scale_map_EEP");
  

 }
