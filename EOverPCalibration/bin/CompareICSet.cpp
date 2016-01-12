#include <vector>
#include <utility>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include "TStyle.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TString.h"
#include "TGraphErrors.h"
#include "../interface/TEndcapRings.h"
#include "TLegend.h"
#include "TApplication.h"

using namespace std;

int main (int argc, char**argv){

if(argc!=3){
 std::cerr<<" Wrongs input "<<std::endl;
 return -1;
}

std::string inputFile1 = argv[1];
std::string inputFile2 = argv[2];

std::cout<<" InputFile1 = "<<inputFile1<<std::endl;
std::cout<<" InputFile2 = "<<inputFile2<<std::endl;

std::ifstream File1 (inputFile1.c_str());
std::ifstream File2 (inputFile2.c_str());
std::ifstream File3 (inputFile1.c_str());
std::ifstream File4 (inputFile2.c_str());

if(!File1.is_open()){
    std::cerr << "** ERROR: Can't open '" << inputFile1 << "' for input" << std::endl;
    return -1;
}

if(!File2.is_open()){
    std::cerr << "** ERROR: Can't open '" << inputFile2 << "' for input" << std::endl;
    return -1;
}

 std::ofstream FileRatio("ratio.txt");
 std::ofstream FileAbsolute("absolute.txt");

// Set style options
/*gROOT->Reset();
gROOT->SetStyle("Plain");
*/

gStyle->SetPadTickX(1);
gStyle->SetPadTickY(1);
gStyle->SetOptTitle(1); 
//gStyle->SetOptStat(1111111); 
gStyle->SetOptStat(0000000); 
gStyle->SetFitFormat("6.3g"); 
gStyle->SetPalette(1); 
gStyle->SetOptTitle(0);

  
gStyle->SetTextFont(42);
gStyle->SetTextSize(0.05);
gStyle->SetTitleFont(42,"xyz");
gStyle->SetTitleSize(0.05);
gStyle->SetLabelFont(42,"xyz");
gStyle->SetLabelSize(0.05);
gStyle->SetTitleXOffset(0.8);
gStyle->SetTitleYOffset(1.1);
gROOT->ForceStyle();




int iPhi, iEta, iz;
double ic, eic;

int iPhi1, iEta1, iz1;
double ic1, eic1;
int iPhi2, iEta2, iz2;
double ic2, eic2;

/// Histo for first ic set
TString Name = Form("mapEB_%s",inputFile1.c_str());
TH2F * map1_EB = new TH2F(Name,Name,360,1, 361, 171, -85, 86);
Name = Form("mapEEp_%s",inputFile1.c_str());
TH2F * map1_EEp = new TH2F(Name,Name,100,1, 101, 100, 1, 101);
Name = Form("mapEEm_%s",inputFile1.c_str());
TH2F * map1_EEm = new TH2F(Name,Name,100,1, 101, 100, 1, 101);

std::cout<<" Opening first file ..... reading "<<std::endl;
while (!File1.eof()){
File1 >> iEta >> iPhi >> iz >> ic >> eic ;
if(iz==0) map1_EB->Fill(iPhi,iEta,ic);
if(iz==1) map1_EEp->Fill(iEta,iPhi,ic);
if(iz==-1)map1_EEm->Fill(iEta,iPhi,ic);
}
std::cout<<" End first file "<<std::endl;
std::cout<<" Opening second file ..... reading "<<std::endl;

/// Histo for first ic set
Name = Form("mapEB_%s",inputFile2.c_str());
TH2F * map2_EB = new TH2F(Name,Name,360,1, 361, 171, -85, 86);
Name = Form("mapEEp_%s",inputFile2.c_str());
TH2F * map2_EEp = new TH2F(Name,Name,100,1, 101, 100, 1, 101);
Name = Form("mapEEm_%s",inputFile2.c_str());
TH2F * map2_EEm = new TH2F(Name,Name,100,1, 101, 100, 1, 101);

while (!File2.eof()){
File2 >> iEta >> iPhi >> iz >> ic >> eic ;
if(iz==0) map2_EB->Fill(iPhi,iEta,ic);
if(iz==1) map2_EEp->Fill(iEta,iPhi,ic);
if(iz==-1) map2_EEm->Fill(iEta,iPhi,ic);
}
std::cout<<" End second file "<<std::endl;

 int cont=0;
 while (!File3.eof() || !File4.eof()){
File3 >> iEta1 >> iPhi1 >> iz1 >> ic1 >> eic1 ;
 File4 >> iEta2 >> iPhi2 >> iz2 >> ic2 >> eic2;
 if (ic1!=ic2)
   std::cout<<"iEta: "<<iEta1<<" iPhi: "<<iPhi1<<" diff IC: "<<ic1-ic2<<std::endl;
 if (iz1==-1) cont++;
}
 std::cout<<"ciao "<<cont<<std::endl;
 // getchar();

 // TApplication* theApp = new TApplication("Application",&argc, argv);



/// Set of two ic sets

Name = Form("diffmapEB");
TH2F * diffmap_EB = (TH2F*) map1_EB->Clone("diffmapEB");
diffmap_EB->Reset();

Name = Form("diffmapEEp");
TH2F * diffmap_EEp = (TH2F*) map1_EEp->Clone("diffmapEEp");
diffmap_EEp->Reset();

Name = Form("diffmapEEm");
TH2F * diffmap_EEm = (TH2F*) map1_EEm->Clone("diffmapEEm");
diffmap_EEm->Reset();

Name = Form("ratiomapEB");
TH2F * ratiomap_EB = (TH2F*) map1_EB->Clone("ratiomapEB");
ratiomap_EB->Reset();

Name = Form("ratiomapEEp");
TH2F * ratiomap_EEp = (TH2F*) map1_EEp->Clone("ratiomapEEp");
ratiomap_EEp->Reset();

Name = Form("ratiomapEEm");
TH2F * ratiomap_EEm = (TH2F*) map1_EEm->Clone("ratiomapEEm");
ratiomap_EEm->Reset();

Name = Form("absolutemapEB");
TH2F * absolutemap_EB = (TH2F*) map1_EB->Clone("absolutemapEB");
absolutemap_EB->Reset();

Name = Form("absolutemapEEp");
TH2F * absolutemap_EEp = (TH2F*) map1_EEp->Clone("absolutemapEEp");
absolutemap_EEp->Reset();

Name = Form("absolutemapEEm");
TH2F * absolutemap_EEm = (TH2F*) map1_EEm->Clone("absolutemapEEm");
absolutemap_EEm->Reset();

Name = Form("diffHistEB");
TH1F * diffHistEB = new TH1F(Name,Name,100,-0.6,0.6);
diffHistEB->SetLineWidth(2);

Name = Form("diffHistEEp");
TH1F * diffHistEEp = new TH1F(Name,Name,100,-0.6,0.6);
diffHistEEp->SetLineWidth(2);

Name = Form("diffHistEEm");
TH1F * diffHistEEm = new TH1F(Name,Name,100,-0.6,0.6);
diffHistEEm->SetLineWidth(2);

Name = Form("correlationEB");
TH2F * correlationEB = new TH2F(Name,Name,100,0.2,2.,100,0.2,2.);

Name = Form("correlationEEp");
TH2F * correlationEEp= new TH2F(Name,Name,100,0.2,2.,100,0.2,2.);

Name = Form("correlationEEm");
TH2F * correlationEEm= new TH2F(Name,Name,100,0.2,2.,100,0.2,2.);






for(int iPhi =1; iPhi<map1_EB->GetNbinsX()+1; iPhi++){
 for(int iEta=1; iEta<map1_EB->GetNbinsY()+1; iEta++){

 if(map1_EB->GetBinContent(iPhi,iEta)==-1. || map2_EB->GetBinContent(iPhi,iEta)==-1.){
 diffmap_EB->SetBinContent(iPhi,iEta,-1.);
 ratiomap_EB->SetBinContent(iPhi,iEta,-1.);
 absolutemap_EB->SetBinContent(iPhi,iEta,map1_EB->GetBinContent(iPhi,iEta));
 FileRatio << iEta << "\t" << iPhi << "\t" << 0 << "\t" << -1. << "\t" << -999. << std::endl;
 FileAbsolute << iEta << "\t" << iPhi << "\t" << 0 << "\t" << map1_EB->GetBinContent(iPhi,iEta) << "\t" << -999. << std::endl;
 continue;}

 diffmap_EB->SetBinContent(iPhi,iEta,map1_EB->GetBinContent(iPhi,iEta)-map2_EB->GetBinContent(iPhi,iEta));
 diffHistEB->Fill(map1_EB->GetBinContent(iPhi,iEta)-map2_EB->GetBinContent(iPhi,iEta));
 ratiomap_EB->SetBinContent(iPhi,iEta,map1_EB->GetBinContent(iPhi,iEta)/map2_EB->GetBinContent(iPhi,iEta));
 absolutemap_EB->SetBinContent(iPhi,iEta,map1_EB->GetBinContent(iPhi,iEta)*map2_EB->GetBinContent(iPhi,iEta));
 FileRatio << iEta << "\t" << iPhi << "\t" << 0 << "\t" << map1_EB->GetBinContent(iPhi,iEta)/map2_EB->GetBinContent(iPhi,iEta) << "\t" << -999. << std::endl;
 FileAbsolute << iEta << "\t" << iPhi << "\t" << 0 << "\t" << map1_EB->GetBinContent(iPhi,iEta)*map2_EB->GetBinContent(iPhi,iEta) << "\t" << -999. << std::endl;
 correlationEB->Fill(map1_EB->GetBinContent(iPhi,iEta),map2_EB->GetBinContent(iPhi,iEta));

 }
}


for(int ix =1; ix<map1_EEp->GetNbinsX()+1; ix++){
 for(int iy=1; iy<map1_EEp->GetNbinsY()+1; iy++){

  if(map1_EEp->GetBinContent(ix,iy)==-1. || map2_EEp->GetBinContent(ix,iy)==-1.){
  diffmap_EEp->SetBinContent(ix,iy,-1.);
  ratiomap_EEp->SetBinContent(ix,iy,-1.);
  absolutemap_EEp->SetBinContent(ix,iy,map1_EEp->GetBinContent(ix,iy));
  FileRatio << ix << "\t" << iy << "\t" << 1 << "\t" << -1. << "\t" << -999. << std::endl;
  FileAbsolute << ix << "\t" << iy << "\t" << 1 << "\t" << map1_EEp->GetBinContent(ix,iy) << "\t" << -999. << std::endl;
  continue;}

  diffmap_EEp->SetBinContent(ix,iy,map1_EEp->GetBinContent(ix,iy)-map2_EEp->GetBinContent(ix,iy));
  diffHistEEp->Fill(map1_EEp->GetBinContent(ix,iy)-map2_EEp->GetBinContent(ix,iy));
  ratiomap_EEp->SetBinContent(ix,iy,map1_EEp->GetBinContent(ix,iy)/map2_EEp->GetBinContent(ix,iy));
  absolutemap_EEp->SetBinContent(ix,iy,map1_EEp->GetBinContent(ix,iy)*map2_EEp->GetBinContent(ix,iy));
  FileRatio << ix << "\t" << iy << "\t" << 1 << "\t" << map1_EEp->GetBinContent(ix,iy)/map2_EEp->GetBinContent(ix,iy) << "\t" << -999. << std::endl;
  FileAbsolute << ix << "\t" << iy << "\t" << 1 << "\t" << map1_EEp->GetBinContent(ix,iy)*map2_EEp->GetBinContent(ix,iy) << "\t" << -999. << std::endl;
  correlationEEp->Fill(map1_EEp->GetBinContent(ix,iy),map2_EEp->GetBinContent(ix,iy));

 }
}


for(int ix =1; ix<map1_EEm->GetNbinsX()+1; ix++){
 for(int iy=1; iy<map1_EEm->GetNbinsY()+1; iy++){

 if(map1_EEm->GetBinContent(ix,iy)==-1. || map2_EEm->GetBinContent(ix,iy)==-1.){
 diffmap_EEm->SetBinContent(ix,iy,-1.);
 ratiomap_EEm->SetBinContent(ix,iy,-1.);
 absolutemap_EEm->SetBinContent(ix,iy,map1_EEm->GetBinContent(ix,iy));
 FileRatio << ix << "\t" << iy << "\t" << -1 << "\t" << map1_EEm->GetBinContent(ix,iy) << "\t" << -999. << std::endl;
  FileAbsolute << ix << "\t" << iy << "\t" << -1 << "\t" << -1. << "\t" << -999. << std::endl;
 continue;}

 diffmap_EEm->SetBinContent(ix,iy,map1_EEm->GetBinContent(ix,iy)-map2_EEm->GetBinContent(ix,iy));
 diffHistEEm->Fill(map1_EEm->GetBinContent(ix,iy)-map2_EEm->GetBinContent(ix,iy));
 ratiomap_EEm->SetBinContent(ix,iy,map1_EEm->GetBinContent(ix,iy)/map2_EEm->GetBinContent(ix,iy));
 absolutemap_EEm->SetBinContent(ix,iy,map1_EEm->GetBinContent(ix,iy)*map2_EEm->GetBinContent(ix,iy));
  FileRatio << ix << "\t" << iy << "\t" << -1 << "\t" << map1_EEp->GetBinContent(ix,iy)/map2_EEp->GetBinContent(ix,iy) << "\t" << -999. << std::endl;
  FileAbsolute << ix << "\t" << iy << "\t" << -1 << "\t" << map1_EEp->GetBinContent(ix,iy)*map2_EEp->GetBinContent(ix,iy) << "\t" << -999. << std::endl;
 correlationEEm->Fill(map1_EEm->GetBinContent(ix,iy),map2_EEm->GetBinContent(ix,iy));

 }
}




/// Profile along phi  for EB:

TGraphErrors *phiProjectionEB1 = new TGraphErrors();
phiProjectionEB1->SetMarkerStyle(20);
phiProjectionEB1->SetMarkerSize(1);
phiProjectionEB1->SetMarkerColor(kBlue);

TGraphErrors *phiProjectionEB2 = new TGraphErrors();
phiProjectionEB2->SetMarkerStyle(20);
phiProjectionEB2->SetMarkerSize(1);
phiProjectionEB2->SetMarkerColor(kRed);


for(int iPhi =1; iPhi<map1_EB->GetNbinsX()+1; iPhi++){
   double sumEta=0, nEta=0;
  
   for(int iEta =1; iEta<map1_EB->GetNbinsY()+1; iEta++){
    if(map1_EB->GetBinContent(iPhi,iEta)==-1. || map1_EB->GetBinContent(iPhi,iEta)==0.) continue;
    sumEta=sumEta+map1_EB->GetBinContent(iPhi,iEta);
    nEta++;
   }
   phiProjectionEB1->SetPoint(iPhi-1,iPhi-1,sumEta/nEta);
   phiProjectionEB1->SetPointError(iPhi-1,0.,0.002);
  }


for(int iPhi =1; iPhi<map2_EB->GetNbinsX()+1; iPhi++){
   double sumEta=0, nEta=0;
  
   for(int iEta =1; iEta<map2_EB->GetNbinsY()+1; iEta++){
    if(map2_EB->GetBinContent(iPhi,iEta)==-1.||map2_EB->GetBinContent(iPhi,iEta)==0. ) continue;
    sumEta=sumEta+map2_EB->GetBinContent(iPhi,iEta);
    nEta++;
   }
   phiProjectionEB2->SetPoint(iPhi-1,iPhi-1,sumEta/nEta);
   phiProjectionEB2->SetPointError(iPhi-1,0.,0.002);
  }


/// Profile along phi  for EE+:

TEndcapRings *eRings = new TEndcapRings();
std::vector<double> vectSum;
std::vector<double> vectCounter;
 
vectCounter.assign(360,0.);
vectSum.assign(360,0.);


TGraphErrors *phiProjectionEEp1 = new TGraphErrors();
phiProjectionEEp1->SetMarkerStyle(20);
phiProjectionEEp1->SetMarkerSize(1);
phiProjectionEEp1->SetMarkerColor(kBlue);

TGraphErrors *phiProjectionEEp2 = new TGraphErrors();
phiProjectionEEp2->SetMarkerStyle(20);
phiProjectionEEp2->SetMarkerSize(1);
phiProjectionEEp2->SetMarkerColor(kRed);

 for(int ix=1; ix<map1_EEp->GetNbinsX()+1;ix++){
   for(int iy=1; iy<map1_EEp->GetNbinsY()+1;iy++){
    if(map1_EEp->GetBinContent(ix,iy)==-1. || map1_EEp->GetBinContent(ix,iy)==0. ) continue;    
      int iPhi = int(eRings->GetEndcapIphi(ix,iy,1));
      if (iPhi==360) iPhi=0;
      vectSum.at(iPhi)=vectSum.at(iPhi)+map1_EEp->GetBinContent(ix,iy);
      vectCounter.at(iPhi)=vectCounter.at(iPhi)+1;
  }
 }


 int j=0;
 for(unsigned int i=0; i<vectCounter.size();i++){
  if(vectCounter.at(i)==0)continue;
  phiProjectionEEp1->SetPoint(j,i,vectSum.at(i)/vectCounter.at(i));
  j++;
 }

 for(unsigned int i=0; i<vectSum.size(); i++){
  vectSum.at(i)=0; vectCounter.at(i)=0;
 }



 for(int ix=1; ix<map2_EEp->GetNbinsX()+1;ix++){
   for(int iy=1; iy<map2_EEp->GetNbinsY()+1;iy++){
    if(map2_EEp->GetBinContent(ix,iy)==-1. ||map2_EEp->GetBinContent(ix,iy)==0.) continue;
      int iPhi = int(eRings->GetEndcapIphi(ix,iy,1));
      if (iPhi==360) iPhi=0;
       vectSum.at(iPhi)=vectSum.at(iPhi)+map2_EEp->GetBinContent(ix,iy);
      vectCounter.at(iPhi)=vectCounter.at(iPhi)+1;
  }
 }


 j=0;
 for(unsigned int i=0; i<vectCounter.size();i++){
  if(vectCounter.at(i)==0)continue;
  phiProjectionEEp2->SetPoint(j,i,vectSum.at(i)/vectCounter.at(i));
  j++;
 }

 for(unsigned int i=0; i<vectSum.size(); i++){
  vectSum.at(i)=0; vectCounter.at(i)=0;
 }


/// Profile along phi  for EE-:

TGraphErrors *phiProjectionEEm1 = new TGraphErrors();
phiProjectionEEm1->SetMarkerStyle(20);
phiProjectionEEm1->SetMarkerSize(1);
phiProjectionEEm1->SetMarkerColor(kBlue);

TGraphErrors *phiProjectionEEm2 = new TGraphErrors();
phiProjectionEEm2->SetMarkerStyle(20);
phiProjectionEEm2->SetMarkerSize(1);
phiProjectionEEm2->SetMarkerColor(kRed);

 for(int ix=1; ix<map1_EEm->GetNbinsX()+1;ix++){
   for(int iy=1; iy<map1_EEm->GetNbinsY()+1;iy++){
    if(map1_EEm->GetBinContent(ix,iy)==-1. || map1_EEm->GetBinContent(ix,iy)==0. ) continue;
      int iPhi = int(eRings->GetEndcapIphi(ix,iy,-1));
      if (iPhi==360) iPhi=0;
      vectSum.at(iPhi)=vectSum.at(iPhi)+map1_EEm->GetBinContent(ix,iy);
      vectCounter.at(iPhi)=vectCounter.at(iPhi)+1;
  }
 }


 j=0;
 for(unsigned int i=0; i<vectCounter.size();i++){
  if(vectCounter.at(i)==0)continue;
  phiProjectionEEm1->SetPoint(j,i,vectSum.at(i)/vectCounter.at(i));
  j++;
 }
 for(unsigned int i=0; i<vectSum.size(); i++){
  vectSum.at(i)=0; vectCounter.at(i)=0;
 }


 for(int ix=1; ix<map2_EEm->GetNbinsX()+1;ix++){
   for(int iy=1; iy<map2_EEm->GetNbinsY()+1;iy++){
    if(map2_EEm->GetBinContent(ix,iy)==-1. ||  map2_EEm->GetBinContent(ix,iy)==0.) continue;
      int iPhi = int(eRings->GetEndcapIphi(ix,iy,-1));
      if (iPhi==360) iPhi=0;
      vectSum.at(iPhi)=vectSum.at(iPhi)+map2_EEm->GetBinContent(ix,iy);
      vectCounter.at(iPhi)=vectCounter.at(iPhi)+1;
  }
 }


 j=0;
 for(unsigned int i=0; i<vectCounter.size();i++){
  if(vectCounter.at(i)==0)continue;
  phiProjectionEEm2->SetPoint(j,i,vectSum.at(i)/vectCounter.at(i));
  j++;
}

 for(unsigned int i=0; i<vectSum.size(); i++){
  vectSum.at(i)=0; vectCounter.at(i)=0;
 }



/// projection along eta for EB

TGraphErrors *etaProjectionEB1 = new TGraphErrors();
etaProjectionEB1->SetMarkerStyle(20);
etaProjectionEB1->SetMarkerSize(1);
etaProjectionEB1->SetMarkerColor(kBlue);

TGraphErrors *etaProjectionEB2 = new TGraphErrors();
etaProjectionEB2->SetMarkerStyle(20);
etaProjectionEB2->SetMarkerSize(1);
etaProjectionEB2->SetMarkerColor(kRed);


for(int iEta =1; iEta<map1_EB->GetNbinsY()+1; iEta++){
   double sumPhi=0, nPhi=0;
  
   for(int iPhi =1; iPhi<map1_EB->GetNbinsX()+1; iPhi++){
    if(map1_EB->GetBinContent(iPhi,iEta)==-1.) continue;
    sumPhi=sumPhi+map1_EB->GetBinContent(iPhi,iEta);
    nPhi++;
   }
   etaProjectionEB1->SetPoint(iEta-1,iEta-1,sumPhi/nPhi);
   etaProjectionEB1->SetPointError(iEta-1,0.,0.002);
  }


for(int iEta =1; iEta<map2_EB->GetNbinsY()+1; iEta++){
   double sumPhi=0, nPhi=0;
  
   for(int iPhi =1; iPhi<map2_EB->GetNbinsX()+1; iPhi++){
    if(map2_EB->GetBinContent(iPhi,iEta)==-1.) continue;
    sumPhi=sumPhi+map2_EB->GetBinContent(iPhi,iEta);
    nPhi++;
   }
   etaProjectionEB2->SetPoint(iEta-1,iEta-1,sumPhi/nPhi);
   etaProjectionEB2->SetPointError(iEta-1,0.,0.002);
  }


 
/// projection along eta for EE+:

TGraphErrors *etaProjectionEEp1 = new TGraphErrors();
etaProjectionEEp1->SetMarkerStyle(20);
etaProjectionEEp1->SetMarkerSize(1);
etaProjectionEEp1->SetMarkerColor(kBlue);

TGraphErrors *etaProjectionEEp2 = new TGraphErrors();
etaProjectionEEp2->SetMarkerStyle(20);
etaProjectionEEp2->SetMarkerSize(1);
etaProjectionEEp2->SetMarkerColor(kRed);

vectCounter.clear();
vectSum.clear();
vectCounter.assign(360,0.);
vectSum.assign(360,0.);

 for(int ix=1; ix<map1_EEp->GetNbinsX()+1;ix++){
   for(int iy=1; iy<map1_EEp->GetNbinsY()+1;iy++){
    if(map1_EEp->GetBinContent(ix,iy)==-1. || map1_EEp->GetBinContent(ix,iy)==0.) continue;
      int iEta = int(eRings->GetEndcapIeta(ix,iy,1));
      if(iEta<0 || iEta>360)continue;
      vectSum.at(iEta)=vectSum.at(iEta)+map1_EEp->GetBinContent(ix,iy);
      vectCounter.at(iEta)=vectCounter.at(iEta)+1;
  }
 }

 j=0;
 for(unsigned int i=0; i<vectCounter.size();i++){
  if(vectCounter.at(i)==0) continue;
  etaProjectionEEp1->SetPoint(j,i,vectSum.at(i)/vectCounter.at(i));
  j++;
 }


 for(unsigned int i=0; i<vectSum.size(); i++){
  vectSum.at(i)=0; vectCounter.at(i)=0;
 }


 for(int ix=1; ix<map2_EEp->GetNbinsX()+1;ix++){
   for(int iy=1; iy<map2_EEp->GetNbinsY()+1;iy++){
    if(map2_EEp->GetBinContent(ix,iy)==-1. || map2_EEp->GetBinContent(ix,iy)==0. ) continue;
      int iEta = int(eRings->GetEndcapIeta(ix,iy,1));
      if(iEta<0 || iEta>360)continue;
      vectSum.at(iEta)=vectSum.at(iEta)+map2_EEp->GetBinContent(ix,iy);
      vectCounter.at(iEta)=vectCounter.at(iEta)+1;
  }
 }


 j=0;
 for(unsigned int i=0; i<vectCounter.size();i++){
  if(vectCounter.at(i)==0) continue;
  etaProjectionEEp2->SetPoint(j,i,vectSum.at(i)/vectCounter.at(i));
  j++;
 }


 for(unsigned int i=0; i<vectSum.size(); i++){
  vectSum.at(i)=0; vectCounter.at(i)=0;
 }


/// projection along eta for EE-:

TGraphErrors *etaProjectionEEm1 = new TGraphErrors();
etaProjectionEEm1->SetMarkerStyle(20);
etaProjectionEEm1->SetMarkerSize(1);
etaProjectionEEm1->SetMarkerColor(kBlue);

TGraphErrors *etaProjectionEEm2 = new TGraphErrors();
etaProjectionEEm2->SetMarkerStyle(20);
etaProjectionEEm2->SetMarkerSize(1);
etaProjectionEEm2->SetMarkerColor(kRed);

 for(int ix=1; ix<map1_EEm->GetNbinsX()+1;ix++){
   for(int iy=1; iy<map1_EEm->GetNbinsY()+1;iy++){
    if(map1_EEm->GetBinContent(ix,iy)==-1.||map1_EEm->GetBinContent(ix,iy)==0.) continue;
      int iEta = int(eRings->GetEndcapIeta(ix,iy,1));
      if(iEta<0 || iEta>360)continue;
      vectSum.at(iEta)=vectSum.at(iEta)+map1_EEm->GetBinContent(ix,iy);
      vectCounter.at(iEta)=vectCounter.at(iEta)+1;
   }
 }


 j=0;
 for(unsigned int i=0; i<vectCounter.size();i++){
  if(vectCounter.at(i)==0)continue;
  etaProjectionEEm1->SetPoint(j,i,vectSum.at(i)/vectCounter.at(i));
  j++;
 }

 for(unsigned int i=0; i<vectSum.size(); i++){
  vectSum.at(i)=0; vectCounter.at(i)=0;
 }


 for(int ix=1; ix<map2_EEm->GetNbinsX()+1;ix++){
   for(int iy=1; iy<map2_EEm->GetNbinsY()+1;iy++){
    if(map2_EEm->GetBinContent(ix,iy)==-1. || map2_EEm->GetBinContent(ix,iy)==0.) continue;
      int iEta = int(eRings->GetEndcapIeta(ix,iy,1));
      if(iEta<0 || iEta>171)continue;
      vectSum.at(iEta)=vectSum.at(iEta)+map2_EEm->GetBinContent(ix,iy);
      vectCounter.at(iEta)=vectCounter.at(iEta)+1;
  }
 }


 j=0;
 for(unsigned int i=0; i<vectCounter.size();i++){
  if(vectCounter.at(i)==0)continue; 
  etaProjectionEEm2->SetPoint(j,i,vectSum.at(i)/vectCounter.at(i));
  j++;
 }

 for(unsigned int i=0; i<vectSum.size(); i++){
  vectSum.at(i)=0; vectCounter.at(i)=0;
 }



 ///  phi Profile Histos EB

 cout<<" Phi Profile Histos "<<endl;

 TH1F* phiProfileEB1 = new TH1F ("phiProfileEB1","phiProfileEB1",60,0.7,1.3);
 TH1F* phiProfileEB2 = new TH1F ("phiProfileEB2","phiProfileEB2",60,0.7,1.3);
 
 for(int i=0; i<phiProjectionEB1->GetN() ; i++){
      double x=0,y=0;
      phiProjectionEB1->GetPoint(i,x,y);
      phiProfileEB1->Fill(y);
  }
 
for(int i=0; i<phiProjectionEB2->GetN() ; i++){
      double x=0,y=0;
      phiProjectionEB2->GetPoint(i,x,y);
      phiProfileEB2->Fill(y);
  }
 
TF1 *fgaus = new TF1("fgaus","gaus",-10,10);
 
fgaus->SetParameter(1,1);
fgaus->SetParameter(2,phiProfileEB1->GetRMS());
fgaus->SetRange(1-5*phiProfileEB1->GetRMS(),1+5*phiProfileEB1->GetRMS());
fgaus->SetLineColor(kBlue);
phiProfileEB1->Fit("fgaus","QRME");
cout<<" First Set :  Mean Fit = "<<fgaus->GetParameter(1)<<" RMS Fit = "<<fgaus->GetParameter(2)<<" chi2/ndf = "<<fgaus->GetChisquare()/fgaus->GetNDF()<<endl;
cout<<" First Set : Mean dist = "<<phiProfileEB1->GetMean()<<" RMS dist "<<phiProfileEB1->GetRMS()<<endl;

fgaus->SetParameter(1,1);
fgaus->SetParameter(2,phiProfileEB2->GetRMS());
fgaus->SetRange(1-5*phiProfileEB2->GetRMS(),1+5*phiProfileEB2->GetRMS());
fgaus->SetLineColor(kRed);
phiProfileEB2->Fit("fgaus","QRME");
cout<<" Second Set : Mean Fit = "<<fgaus->GetParameter(1)<<" RMS Fit = "<<fgaus->GetParameter(2)<<" chi2/ndf = "<<fgaus->GetChisquare()/fgaus->GetNDF()<<endl;
cout<<" Second Set : Mean dist = "<<phiProfileEB2->GetMean()<<" RMS dist "<<phiProfileEB2->GetRMS()<<endl;

 /// phi Profile Histos EE+

 TH1F* phiProfileEEp1 = new TH1F ("phiProfileEEp1","phiProfileEEp1",60,0.6,1.6);
 TH1F* phiProfileEEp2 = new TH1F ("phiProfileEEp2","phiProfileEEp2",60,0.6,1.6);
 
 for(int i=0; i<phiProjectionEEp1->GetN() ; i++){
      double x=0,y=0;
      phiProjectionEEp1->GetPoint(i,x,y);
      phiProfileEEp1->Fill(y);
  }
 
for(int i=0; i<phiProjectionEEp2->GetN() ; i++){
      double x=0,y=0;
      phiProjectionEEp2->GetPoint(i,x,y);
      phiProfileEEp2->Fill(y);
  }
  
fgaus->SetParameter(1,1);
fgaus->SetParameter(2,phiProfileEEp1->GetRMS());
fgaus->SetRange(1-5*phiProfileEEp1->GetRMS(),1+5*phiProfileEEp1->GetRMS());
fgaus->SetLineColor(kBlue);
phiProfileEEp1->Fit("fgaus","QRME");
cout<<" First Set :  Mean Fit = "<<fgaus->GetParameter(1)<<" RMS Fit = "<<fgaus->GetParameter(2)<<" chi2/ndf = "<<fgaus->GetChisquare()/fgaus->GetNDF()<<endl;
cout<<" First Set : Mean dist = "<<phiProfileEEp1->GetMean()<<" RMS dist "<<phiProfileEEp1->GetRMS()<<endl;

fgaus->SetParameter(1,1);
fgaus->SetParameter(2,phiProfileEEp2->GetRMS());
fgaus->SetRange(1-5*phiProfileEEp2->GetRMS(),1+5*phiProfileEEp2->GetRMS());
fgaus->SetLineColor(kRed);
phiProfileEEp2->Fit("fgaus","QRME");
cout<<" Second Set : Mean Fit = "<<fgaus->GetParameter(1)<<" RMS Fit = "<<fgaus->GetParameter(2)<<" chi2/ndf = "<<fgaus->GetChisquare()/fgaus->GetNDF()<<endl;
cout<<" Second Set : Mean dist = "<<phiProfileEEp2->GetMean()<<" RMS dist "<<phiProfileEEp2->GetRMS()<<endl;

 /// phi Profile Histos EE-

 TH1F* phiProfileEEm1 = new TH1F ("phiProfileEEm1","phiProfileEEm1",60,0.6,1.6);
 TH1F* phiProfileEEm2 = new TH1F ("phiProfileEEm2","phiProfileEEm2",60,0.6,1.6);
 
 for(int i=0; i<phiProjectionEEm1->GetN() ; i++){
      double x=0,y=0;
      phiProjectionEEm1->GetPoint(i,x,y);
      phiProfileEEm1->Fill(y);
  }
 
for(int i=0; i<phiProjectionEEm2->GetN() ; i++){
      double x=0,y=0;
      phiProjectionEEm2->GetPoint(i,x,y);
      phiProfileEEm2->Fill(y);
  }
  
fgaus->SetParameter(1,1);
fgaus->SetParameter(2,phiProfileEEm1->GetRMS());
fgaus->SetRange(1-5*phiProfileEEm1->GetRMS(),1+5*phiProfileEEm1->GetRMS());
fgaus->SetLineColor(kBlue);
phiProfileEEm1->Fit("fgaus","QRME");
cout<<" First Set :  Mean Fit = "<<fgaus->GetParameter(1)<<" RMS Fit = "<<fgaus->GetParameter(2)<<" chi2/ndf = "<<fgaus->GetChisquare()/fgaus->GetNDF()<<endl;
cout<<" First Set : Mean dist = "<<phiProfileEEm1->GetMean()<<" RMS dist "<<phiProfileEEm1->GetRMS()<<endl;

fgaus->SetParameter(1,1);
fgaus->SetParameter(2,phiProfileEEm2->GetRMS());
fgaus->SetRange(1-5*phiProfileEEm2->GetRMS(),1+5*phiProfileEEm2->GetRMS());
fgaus->SetLineColor(kRed);
phiProfileEEm2->Fit("fgaus","QRME");
cout<<" Second Set : Mean Fit = "<<fgaus->GetParameter(1)<<" RMS Fit = "<<fgaus->GetParameter(2)<<" chi2/ndf = "<<fgaus->GetChisquare()/fgaus->GetNDF()<<endl;
cout<<" Second Set : Mean dist = "<<phiProfileEEm2->GetMean()<<" RMS dist "<<phiProfileEEm2->GetRMS()<<endl;

 /// eta Profile Histos EB

 cout<<" Eta Profile Histos "<<endl;

 TH1F* etaProfileEB1 = new TH1F ("etaProfileEB1","etaProfileEB1",60,0.85,1.15);
 TH1F* etaProfileEB2 = new TH1F ("etaProfileEB2","etaProfileEB2",60,0.85,1.15);
 
 for(int i=0; i<etaProjectionEB1->GetN() ; i++){
      double x=0,y=0;
      etaProjectionEB1->GetPoint(i,x,y);
      etaProfileEB1->Fill(y);
  }
 
for(int i=0; i<etaProjectionEB2->GetN() ; i++){
      double x=0,y=0;
      etaProjectionEB2->GetPoint(i,x,y);
      etaProfileEB2->Fill(y);
  }
  
fgaus->SetParameter(1,1);
fgaus->SetParameter(2,etaProfileEB1->GetRMS());
fgaus->SetRange(1-5*etaProfileEB1->GetRMS(),1+5*etaProfileEB1->GetRMS());
fgaus->SetLineColor(kBlue);
etaProfileEB1->Fit("fgaus","QRME");
cout<<" First Set : Mean Fit = "<<fgaus->GetParameter(1)<<" RMS Fit = "<<fgaus->GetParameter(2)<<" chi2/ndf = "<<fgaus->GetChisquare()/fgaus->GetNDF()<<endl;
cout<<" First Set : Mean dist = "<<etaProfileEB1->GetMean()<<" RMS dist "<<etaProfileEB1->GetRMS()<<endl;

fgaus->SetParameter(1,1);
fgaus->SetParameter(2,etaProfileEB2->GetRMS());
fgaus->SetRange(1-5*etaProfileEB2->GetRMS(),1+5*etaProfileEB2->GetRMS());
fgaus->SetLineColor(kRed);
etaProfileEB2->Fit("fgaus","QRME");
cout<<" Second Set : Mean Fit = "<<fgaus->GetParameter(1)<<" RMS Fit = "<<fgaus->GetParameter(2)<<" chi2/ndf = "<<fgaus->GetChisquare()/fgaus->GetNDF()<<endl;
cout<<" Second Set : Mean dist = "<<etaProfileEB2->GetMean()<<" RMS dist "<<etaProfileEB2->GetRMS()<<endl;

 /// eta Profile Histos EE+

 TH1F* etaProfileEEp1 = new TH1F ("etaProfileEEp1","etaProfileEEp1",35,0.,2.5);
 TH1F* etaProfileEEp2 = new TH1F ("etaProfileEEp2","etaProfileEEp2",35,0.,2.5);
 
 for(int i=0; i<etaProjectionEEp1->GetN() ; i++){
      double x=0,y=0;
      etaProjectionEEp1->GetPoint(i,x,y);
      etaProfileEEp1->Fill(y);
  }
 
for(int i=0; i<etaProjectionEEp2->GetN() ; i++){
      double x=0,y=0;
      etaProjectionEEp2->GetPoint(i,x,y);
      etaProfileEEp2->Fill(y);
  }
  
fgaus->SetParameter(1,1);
fgaus->SetParameter(2,etaProfileEEp1->GetRMS());
fgaus->SetRange(1-5*etaProfileEEp1->GetRMS(),1+5*etaProfileEEp1->GetRMS());
fgaus->SetLineColor(kBlue);
etaProfileEEp1->Fit("fgaus","QRME");
cout<<" First Set : Mean Fit = "<<fgaus->GetParameter(1)<<" RMS Fit = "<<fgaus->GetParameter(2)<<" chi2/ndf = "<<fgaus->GetChisquare()/fgaus->GetNDF()<<endl;
cout<<" First Set : Mean dist = "<<etaProfileEEp1->GetMean()<<" RMS dist "<<etaProfileEEp1->GetRMS()<<endl;

fgaus->SetParameter(1,1);
fgaus->SetParameter(2,etaProfileEEp2->GetRMS());
fgaus->SetRange(1-5*etaProfileEEp2->GetRMS(),1+5*etaProfileEEp2->GetRMS());
fgaus->SetLineColor(kRed);
etaProfileEEp2->Fit("fgaus","QRME");
cout<<" Second Set : Mean Fit = "<<fgaus->GetParameter(1)<<" RMS Fit = "<<fgaus->GetParameter(2)<<" chi2/ndf = "<<fgaus->GetChisquare()/fgaus->GetNDF()<<endl;
cout<<" Second Set : Mean dist = "<<etaProfileEEp2->GetMean()<<" RMS dist "<<etaProfileEEp2->GetRMS()<<endl;


 /// eta Profile Histos EB

 TH1F* etaProfileEEm1 = new TH1F ("etaProfileEEm1","etaProfileEEm1",35,0.,2.5);
 TH1F* etaProfileEEm2 = new TH1F ("etaProfileEEm2","etaProfileEEm2",35,0.,2.5);
 
 for(int i=0; i<etaProjectionEEm1->GetN() ; i++){
      double x=0,y=0;
      etaProjectionEEm1->GetPoint(i,x,y);
      etaProfileEEm1->Fill(y);
  }
 
for(int i=0; i<etaProjectionEEm2->GetN() ; i++){
      double x=0,y=0;
      etaProjectionEEm2->GetPoint(i,x,y);
      etaProfileEEm2->Fill(y);
  }
  
fgaus->SetParameter(1,1);
fgaus->SetParameter(2,etaProfileEEm1->GetRMS());
fgaus->SetRange(1-5*etaProfileEEm1->GetRMS(),1+5*etaProfileEEm1->GetRMS());
fgaus->SetLineColor(kBlue);
etaProfileEEm1->Fit("fgaus","QRME");
cout<<" First Set : Mean Fit = "<<fgaus->GetParameter(1)<<" RMS Fit = "<<fgaus->GetParameter(2)<<" chi2/ndf = "<<fgaus->GetChisquare()/fgaus->GetNDF()<<endl;
cout<<" First Set : Mean dist = "<<etaProfileEEm1->GetMean()<<" RMS dist "<<etaProfileEEm1->GetRMS()<<endl;

fgaus->SetParameter(1,1);
fgaus->SetParameter(2,etaProfileEEm2->GetRMS());
fgaus->SetRange(1-5*etaProfileEEm2->GetRMS(),1+5*etaProfileEEm2->GetRMS());
fgaus->SetLineColor(kRed);
etaProfileEEm2->Fit("fgaus","QRME");
cout<<" Second Set : Mean Fit = "<<fgaus->GetParameter(1)<<" RMS Fit = "<<fgaus->GetParameter(2)<<" chi2/ndf = "<<fgaus->GetChisquare()/fgaus->GetNDF()<<endl;
cout<<" Second Set : Mean dist = "<<etaProfileEEm2->GetMean()<<" RMS dist "<<etaProfileEEm2->GetRMS()<<endl;


  
 


 ///------------------------------------------------------------------------
 ///-----------------------------------------------------------------
 ///--- Draw plots
 ///-----------------------------------------------------------------
 
 TCanvas *c[30];

 c[0] = new TCanvas("hdiffEB","hdiffEB");
 c[0]->SetLeftMargin(0.1); 
 c[0]->SetRightMargin(0.13); 
 c[0]->SetGridx();
  
 diffmap_EB->GetXaxis()->SetNdivisions(1020);
 diffmap_EB->GetXaxis() -> SetLabelSize(0.03);
 diffmap_EB->GetXaxis() ->SetTitle("i#phi");
 diffmap_EB->GetYaxis() ->SetTitle("i#eta");
 diffmap_EB->GetZaxis() ->SetRangeUser(-0.1,0.1);
 diffmap_EB->Draw("COLZ");

 c[1] = new TCanvas("histdiffEB","histdiffEB");
 c[1]->SetLeftMargin(0.1); 
 c[1]->SetRightMargin(0.13); 
 c[1]->SetLogy();

 diffHistEB->GetXaxis()->SetTitle("c_{#pi}-c_{ele}"); 
 diffHistEB->Draw();

 
 c[2] = new TCanvas("hratioEB","hratioEB");
 c[2]->SetLeftMargin(0.1); 
 c[2]->SetRightMargin(0.13); 
 c[2]->SetGridx();
  
 ratiomap_EB->GetXaxis()->SetNdivisions(1020);
 ratiomap_EB->GetXaxis() -> SetLabelSize(0.03);
 ratiomap_EB->GetXaxis() ->SetTitle("i#phi");
 ratiomap_EB->GetYaxis() ->SetTitle("i#eta");
 ratiomap_EB->GetZaxis() ->SetRangeUser(0.95,1.05);
 ratiomap_EB->Draw("COLZ");

 c[3] = new TCanvas("correlationEB","correlationEB");
 c[3]->SetLeftMargin(0.1); 
 c[3]->SetRightMargin(0.13); 
 c[3]->SetGridx();
 c[3]->SetGridy();

 correlationEB->GetXaxis()->SetNdivisions(1020);
 correlationEB->GetXaxis() -> SetLabelSize(0.03);
 correlationEB->GetXaxis() ->SetTitle("c_{#pi}");
 correlationEB->GetYaxis() ->SetTitle("c_{ele}");
 correlationEB->Draw("COLZ");

 
 c[4] = new TCanvas("hdiffEEp","hdiffEEp");
 c[4]->SetLeftMargin(0.1); 
 c[4]->SetRightMargin(0.13); 
 c[4]->SetGridx();
  
 diffmap_EEp->GetXaxis()->SetNdivisions(1020);
 diffmap_EEp->GetXaxis() -> SetLabelSize(0.03);
 diffmap_EEp->GetXaxis() ->SetTitle("ix");
 diffmap_EEp->GetYaxis() ->SetTitle("iy");
 diffmap_EEp->GetZaxis() ->SetRangeUser(-0.15,0.15);
 diffmap_EEp->Draw("COLZ");

 c[5] = new TCanvas("histdiffEEp","histdiffEEp");
 c[5]->SetLeftMargin(0.1); 
 c[5]->SetRightMargin(0.13); 
 c[5]->SetLogy();

 diffHistEEp->GetXaxis()->SetTitle("c_{#pi}-c_{ele}"); 
 diffHistEEp->Draw();
 
 c[6] = new TCanvas("hratioEEp","hratioEEp");
 c[6]->SetLeftMargin(0.1); 
 c[6]->SetRightMargin(0.13); 
 c[6]->SetGridx();
  
 ratiomap_EEp->GetXaxis()->SetNdivisions(1020);
 ratiomap_EEp->GetXaxis() -> SetLabelSize(0.03);
 ratiomap_EEp->GetXaxis() ->SetTitle("ix");
 ratiomap_EEp->GetYaxis() ->SetTitle("iy");
 ratiomap_EEp->GetZaxis() ->SetRangeUser(0.9,1.1);
 ratiomap_EEp->Draw("COLZ");

 c[7] = new TCanvas("correlationEEp","correlationEEp");
 c[7]->SetLeftMargin(0.1); 
 c[7]->SetRightMargin(0.13); 
 c[7]->SetGridx();
 c[7]->SetGridy();
  
 correlationEEp->GetXaxis()->SetNdivisions(1020);
 correlationEEp->GetXaxis() -> SetLabelSize(0.03);
 correlationEEp->GetXaxis() ->SetTitle("c_{#pi}");
 correlationEEp->GetYaxis() ->SetTitle("c_{ele}");
 correlationEEp->Draw("COLZ");

 c[8] = new TCanvas("hdiffEEm","hdiffEEm");
 c[8]->SetLeftMargin(0.1); 
 c[8]->SetRightMargin(0.13); 
 c[8]->SetGridx();
  
 diffmap_EEm->GetXaxis()->SetNdivisions(1020);
 diffmap_EEm->GetXaxis() -> SetLabelSize(0.03);
 diffmap_EEm->GetXaxis() ->SetTitle("ix");
 diffmap_EEm->GetYaxis() ->SetTitle("iy");
 diffmap_EEm->GetZaxis() ->SetRangeUser(-0.15,0.15);
 diffmap_EEm->Draw("COLZ");

 c[9] = new TCanvas("histdiffEEm","histdiffEEm");
 c[9]->SetLeftMargin(0.1); 
 c[9]->SetRightMargin(0.13); 
 c[9]->SetLogy();

 diffHistEEm->GetXaxis()->SetTitle("c_{#pi}-c_{ele}"); 
 diffHistEEm->Draw();
 


 c[10] = new TCanvas("hratioEEm","hratioEEm");
 c[10]->SetLeftMargin(0.1); 
 c[10]->SetRightMargin(0.13); 
 c[10]->SetGridx();
  
 ratiomap_EEm->GetXaxis()->SetNdivisions(1020);
 ratiomap_EEm->GetXaxis() -> SetLabelSize(0.03);
 ratiomap_EEm->GetXaxis() ->SetTitle("ix");
 ratiomap_EEm->GetYaxis() ->SetTitle("iy");
 ratiomap_EEm->GetZaxis() ->SetRangeUser(0.9,1.1);
 ratiomap_EEm->Draw("COLZ");
 
 c[11] = new TCanvas("correlationEEm","correlationEEm");
 c[11]->SetLeftMargin(0.1); 
 c[11]->SetRightMargin(0.13); 
 c[11]->SetGridx();
 c[11]->SetGridy();
  
 correlationEEm->GetXaxis()->SetNdivisions(1020);
 correlationEEm->GetXaxis() -> SetLabelSize(0.03);
 correlationEEm->GetXaxis() ->SetTitle("c_{#pi}");
 correlationEEm->GetYaxis() ->SetTitle("c_{ele}");
 correlationEEm->Draw("COLZ");


 c[12] = new TCanvas("phiProjectionEB","phiProjectionEB");
 c[12]->SetGridx();
 c[12]->SetGridy();
 phiProjectionEB1->GetHistogram()->GetYaxis()-> SetRangeUser(0.85,1.1);
 phiProjectionEB1->GetHistogram()->GetXaxis()-> SetRangeUser(1,361);
 phiProjectionEB1->GetHistogram()->GetYaxis()-> SetTitle("Mean IC");
 phiProjectionEB1->GetHistogram()->GetXaxis()-> SetTitle("i#phi");
 phiProjectionEB1->Draw("apl");
 phiProjectionEB2->Draw("plsame");
 
 TLegend * leg1 = new TLegend(0.75,0.75,0.89, 0.89);
 leg1->AddEntry(phiProjectionEB1,"IC set 1","LP");
 leg1->AddEntry(phiProjectionEB2,"IC set 2","LP");
 leg1->SetFillColor(0);
 leg1->Draw("same");

 c[13] = new TCanvas("phiProjectionEEp","phiProjectionEEp");
 c[13]->SetGridx();
 c[13]->SetGridy();
 phiProjectionEEp1->GetHistogram()->GetYaxis()-> SetRangeUser(0.7,1.4);
 phiProjectionEEp1->GetHistogram()->GetXaxis()-> SetRangeUser(1,361);
 phiProjectionEEp1->GetHistogram()->GetYaxis()-> SetTitle("Mean IC");
 phiProjectionEEp1->GetHistogram()->GetXaxis()-> SetTitle("i#phi");
 phiProjectionEEp1->Draw("apl");
 phiProjectionEEp2->Draw("plsame");
 
 TLegend * leg2 = new TLegend(0.75,0.75,0.89, 0.89);
 leg2->AddEntry(phiProjectionEEp1,"IC set 1","LP");
 leg2->AddEntry(phiProjectionEEp2,"IC set 2","LP");
 leg2->SetFillColor(0);
 leg2->Draw("same");


 c[14] = new TCanvas("phiProjectionEEm","phiProjectionEEm");
 c[14]->SetGridx();
 c[14]->SetGridy();
 phiProjectionEEm1->GetHistogram()->GetYaxis()-> SetRangeUser(0.7,1.4);
 phiProjectionEEm1->GetHistogram()->GetXaxis()-> SetRangeUser(1,361);
 phiProjectionEEm1->GetHistogram()->GetYaxis()-> SetTitle("Mean IC");
 phiProjectionEEm1->GetHistogram()->GetXaxis()-> SetTitle("i#phi");
 phiProjectionEEm1->Draw("apl");
 phiProjectionEEm2->Draw("plsame");
 
 TLegend * leg3 = new TLegend(0.75,0.75,0.89, 0.89);
 leg3->AddEntry(phiProjectionEEm1,"IC set 1","LP");
 leg3->AddEntry(phiProjectionEEm2,"IC set 2","LP");
 leg3->SetFillColor(0);
 leg3->Draw("same");

 c[15] = new TCanvas("etaProjectionEB","etaProjectionEB");
 c[15]->SetGridx();
 c[15]->SetGridy();
 etaProjectionEB1->GetHistogram()->GetYaxis()-> SetRangeUser(0.9,1.1);
 etaProjectionEB1->GetHistogram()->GetXaxis()-> SetRangeUser(0,171);
 etaProjectionEB1->GetHistogram()->GetYaxis()-> SetTitle("Mean IC");
 etaProjectionEB1->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
 etaProjectionEB1->Draw("apl");
 etaProjectionEB2->Draw("plsame");
 
 TLegend * leg4 = new TLegend(0.75,0.75,0.89, 0.89);
 leg4->AddEntry(etaProjectionEB1,"IC set 1","LP");
 leg4->AddEntry(etaProjectionEB2,"IC set 2","LP");
 leg4->SetFillColor(0);
 leg1->Draw("same");

 c[16] = new TCanvas("etaProjectionEEp","etaProjectionEEp");
 c[16]->SetGridx();
 c[16]->SetGridy();
 etaProjectionEEp1->GetHistogram()->GetYaxis()-> SetRangeUser(0.55,1.5);
 etaProjectionEEp1->GetHistogram()->GetXaxis()-> SetRangeUser(85,125);
 etaProjectionEEp1->GetHistogram()->GetYaxis()-> SetTitle("Mean IC");
 etaProjectionEEp1->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
 etaProjectionEEp1->Draw("apl");
 etaProjectionEEp2->Draw("plsame");
 
 TLegend * leg5 = new TLegend(0.75,0.75,0.89, 0.89);
 leg5->AddEntry(etaProjectionEEp1,"IC set 1","LP");
 leg5->AddEntry(etaProjectionEEp2,"IC set 2","LP");
 leg5->SetFillColor(0);
 leg5->Draw("same");


 c[17] = new TCanvas("etaProjectionEEm","etaProjectionEEm");
 c[17]->SetGridx();
 c[17]->SetGridy();
 etaProjectionEEm1->GetHistogram()->GetYaxis()-> SetRangeUser(0.55,1.5);
 etaProjectionEEm1->GetHistogram()->GetXaxis()-> SetRangeUser(85,125);
 etaProjectionEEm1->GetHistogram()->GetYaxis()-> SetTitle("Mean IC");
 etaProjectionEEm1->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
 etaProjectionEEm1->Draw("apl");
 etaProjectionEEm2->Draw("plsame");
 
 TLegend * leg6 = new TLegend(0.75,0.75,0.89, 0.89);
 leg6->AddEntry(etaProjectionEEm1,"IC set 1","LP");
 leg6->AddEntry(etaProjectionEEm2,"IC set 2","LP");
 leg6->SetFillColor(0);
 leg6->Draw("same");

 c[18] = new TCanvas("phiProfileEB","phiProfileEB");
 c[18]->SetGridx();
 c[18]->SetGridy();
 phiProfileEB1->GetXaxis()->SetTitle("#bar{IC}");
 phiProfileEB1->SetLineColor(kBlue);
 phiProfileEB1->SetMarkerSize(0.8);
 phiProfileEB1->SetLineWidth(2);
 phiProfileEB2->SetLineColor(kRed);
 phiProfileEB2->SetMarkerSize(0.8);
 phiProfileEB2->SetLineWidth(2);
 phiProfileEB1->Draw();
 phiProfileEB2->Draw("same");
  
 TLegend * leg7 = new TLegend(0.6,0.7,0.89, 0.89);
 leg7->SetFillColor(0);
 leg7->AddEntry(phiProfileEB1,"EB Projection I set ", "LP");
 leg7->AddEntry(phiProfileEB2,"EB Projection II set ", "LP");
 leg7->Draw("same");

 c[19] = new TCanvas("phiProfileEEp","phiProfileEEp");
 c[19]->SetGridx();
 c[19]->SetGridy();
 phiProfileEEp1->GetXaxis()->SetTitle("#bar{IC}");
 phiProfileEEp1->SetLineColor(kBlue);
 phiProfileEEp1->SetMarkerSize(0.8);
 phiProfileEEp1->SetLineWidth(2);
 phiProfileEEp2->SetLineColor(kRed);
 phiProfileEEp2->SetMarkerSize(0.8);
 phiProfileEEp2->SetLineWidth(2);
 phiProfileEEp1->Draw();
 phiProfileEEp2->Draw("same");
  
 TLegend * leg8 = new TLegend(0.6,0.7,0.89, 0.89);
 leg8->SetFillColor(0);
 leg8->AddEntry(phiProfileEEp1,"EE+ Projection I set ", "LP");
 leg8->AddEntry(phiProfileEEp2,"EE+ Projection II set ", "LP");
 leg8->Draw("same");

 c[20] = new TCanvas("phiProfileEEm","phiProfileEEm");
 c[20]->SetGridx();
 c[20]->SetGridy();
 phiProfileEEm1->GetXaxis()->SetTitle("#bar{IC}");
 phiProfileEEm1->SetLineColor(kBlue);
 phiProfileEEm1->SetMarkerSize(0.8);
 phiProfileEEm1->SetLineWidth(2);
 phiProfileEEm2->SetLineColor(kRed);
 phiProfileEEm2->SetMarkerSize(0.8);
 phiProfileEEm2->SetLineWidth(2);
 phiProfileEEm1->Draw();
 phiProfileEEm2->Draw("same");
  
 TLegend * leg9 = new TLegend(0.6,0.7,0.89, 0.89);
 leg9->SetFillColor(0);
 leg9->AddEntry(phiProfileEEm1,"EE- Projection I set ", "LP");
 leg9->AddEntry(phiProfileEEm2,"EE- Projection II set ", "LP");
 leg9->Draw("same");

 c[21] = new TCanvas("etaProfileEB","etaProfileEB");
 c[21]->SetGridx();
 c[21]->SetGridy();
 etaProfileEB1->GetXaxis()->SetTitle("#bar{IC}");
 etaProfileEB1->SetLineColor(kBlue);
 etaProfileEB1->SetMarkerSize(0.8);
 etaProfileEB1->SetLineWidth(2);
 etaProfileEB2->SetLineColor(kRed);
 etaProfileEB2->SetMarkerSize(0.8);
 etaProfileEB2->SetLineWidth(2);
 etaProfileEB1->Draw();
 etaProfileEB2->Draw("same");
  
 TLegend * leg10 = new TLegend(0.6,0.7,0.89, 0.89);
 leg10->SetFillColor(0);
 leg10->AddEntry(etaProfileEB1,"EB Projection I set ", "LP");
 leg10->AddEntry(etaProfileEB2,"EB Projection II set ", "LP");
 leg10->Draw("same");

 c[22] = new TCanvas("etaProfileEEp","etaProfileEEp");
 c[22]->SetGridx();
 c[22]->SetGridy();
 etaProfileEEp1->GetXaxis()->SetTitle("#bar{IC}");
 etaProfileEEp1->SetLineColor(kBlue);
 etaProfileEEp1->SetMarkerSize(0.8);
 etaProfileEEp1->SetLineWidth(2);
 etaProfileEEp2->SetLineColor(kRed);
 etaProfileEEp2->SetMarkerSize(0.8);
 etaProfileEEp2->SetLineWidth(2);
 etaProfileEEp1->Draw();
 etaProfileEEp2->Draw("same");
  
 TLegend * leg11 = new TLegend(0.6,0.7,0.89, 0.89);
 leg11->SetFillColor(0);
 leg11->AddEntry(phiProfileEEp1,"EE+ Projection I set ", "LP");
 leg11->AddEntry(phiProfileEEp2,"EE+ Projection II set ", "LP");
 leg11->Draw("same");

 c[23] = new TCanvas("etaProfileEEm","etaProfileEEm");
 c[23]->SetGridx();
 c[23]->SetGridy();
 etaProfileEEm1->GetXaxis()->SetTitle("#bar{IC}");
 etaProfileEEm1->SetLineColor(kBlue);
 etaProfileEEm1->SetMarkerSize(0.8);
 etaProfileEEm1->SetLineWidth(2);
 etaProfileEEm2->SetLineColor(kRed);
 etaProfileEEm2->SetMarkerSize(0.8);
 etaProfileEEm2->SetLineWidth(2);
 etaProfileEEm1->Draw();
 etaProfileEEm2->Draw("same");
  
 TLegend * leg12 = new TLegend(0.6,0.7,0.89, 0.89);
 leg12->SetFillColor(0);
 leg12->AddEntry(phiProfileEEm1,"EE- Projection I set ", "LP");
 leg12->AddEntry(phiProfileEEm2,"EE- Projection II set ", "LP");
 leg12->Draw("same");

 c[24] = new TCanvas("habsoluteEB","habsoluteEB");
 c[24]->SetLeftMargin(0.1); 
 c[24]->SetRightMargin(0.13); 
 c[24]->SetGridx();
  
 absolutemap_EB->GetXaxis()->SetNdivisions(1020);
 absolutemap_EB->GetXaxis() -> SetLabelSize(0.03);
 absolutemap_EB->GetXaxis() ->SetTitle("i#phi");
 absolutemap_EB->GetYaxis() ->SetTitle("i#eta");
 absolutemap_EB->GetZaxis() ->SetRangeUser(0.85,1.15);
 absolutemap_EB->Draw("COLZ");

 c[25] = new TCanvas("habsoluteEEp","habsoluteEEp");
 c[25]->SetLeftMargin(0.1); 
 c[25]->SetRightMargin(0.13); 
 c[25]->SetGridx();
  
 absolutemap_EEp->GetXaxis()->SetNdivisions(1020);
 absolutemap_EEp->GetXaxis() -> SetLabelSize(0.03);
 absolutemap_EEp->GetXaxis() ->SetTitle("ix");
 absolutemap_EEp->GetYaxis() ->SetTitle("iy");
 absolutemap_EEp->GetZaxis() ->SetRangeUser(0.7,1.3);
 absolutemap_EEp->Draw("COLZ");


 c[26] = new TCanvas("habsoluteEEm","habsoluteEEm");
 c[26]->SetLeftMargin(0.1); 
 c[26]->SetRightMargin(0.13); 
 c[26]->SetGridx();
  
 absolutemap_EEm->GetXaxis()->SetNdivisions(1020);
 absolutemap_EEm->GetXaxis() -> SetLabelSize(0.03);
 absolutemap_EEm->GetXaxis() ->SetTitle("ix");
 absolutemap_EEm->GetYaxis() ->SetTitle("iy");
 absolutemap_EEm->GetZaxis() ->SetRangeUser(0.7,1.3);
 absolutemap_EEm->Draw("COLZ");


 TFile f("compareIC.root","recreate");
 f.cd();

 for (int i=0; i<27; i++) {
   c[i]->Write();
 }

 f.Write();
 // f.close();

 // theApp->Run();

return 0;

} 


