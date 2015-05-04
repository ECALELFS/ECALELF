#include "../interface/TEndcapRings.h"
#include "../interface/ntpleUtils.h"
#include "../interface/treeReader.h"
#include "../interface/CalibrationUtils.h"
#include "../CommonTools/histoFunc.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <ctime>
#include <map>
#include <algorithm>
#include <math.h>
#include <vector>

#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TProfile.h"
#include "TGraphErrors.h"
#include "TPaveStats.h"
#include "TLegend.h"
#include "TChain.h"
#include "TVirtualFitter.h"
#include "TMath.h"

#include "FWCore/ParameterSet/interface/ProcessDesc.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

using namespace std;

 
bool IsEtaGap(float eta)
{
  float feta = fabs(eta);
  if( fabs(feta - 0 ) < 3 ) return true;
  if( fabs(feta - 25) < 3 ) return true;
  if( fabs(feta - 45) < 3 ) return true;
  if( fabs(feta - 65) < 3 ) return true;
  if( fabs(feta - 85) < 3 ) return true;
  return false;
}






//**************  MAIN PROGRAM **************************************************************
int main(int argc, char** argv)
{
  // Acquisition from cfg file
  if(argc != 2)
  {
    std::cerr << ">>>>> CalibrationMomentum.cpp::usage:   " << argv[0] << " configFileName" << std::endl;
    return 1;
  }
    

  std::string configFileName = argv[1];
#ifdef CMSSW_7_2_X
  std::shared_ptr<edm::ParameterSet> parameterSet = edm::readConfig(configFileName);
#else
  boost::shared_ptr<edm::ParameterSet> parameterSet = edm::readConfig(configFileName);
#endif
  edm::ParameterSet Options = parameterSet -> getParameter<edm::ParameterSet>("Options");
  //  parameterSet.reset();

  std::string TreeName = "NULL";
  if(Options.existsAs<std::string>("TreeName"))
    TreeName = Options.getParameter<std::string>("TreeName");

  std::string infileDATA = "NULL";
  if(Options.existsAs<std::string>("infileDATA"))
    infileDATA = Options.getParameter<std::string>("infileDATA");

  std::string infileMC = "NULL";
  if(Options.existsAs<std::string>("infileMC"))
    infileMC = Options.getParameter<std::string>("infileMC");

  std::string WeightforMC = "NULL";
  if(Options.existsAs<std::string>("WeightforMC"))
    WeightforMC = Options.getParameter<std::string>("WeightforMC");

  std::string typeEB = "NULL";
  if(Options.existsAs<std::string>("typeEB"))
    typeEB = Options.getParameter<std::string>("typeEB");

  std::string typeEE = "NULL";
  if(Options.existsAs<std::string>("typeEE"))
    typeEE = Options.getParameter<std::string>("typeEE");

  int nPhiBinsEB = 1;
  if(Options.existsAs<int>("nPhiBinsEB"))
    nPhiBinsEB = Options.getParameter<int>("nPhiBinsEB");

  int nPhiBinsEE = 1;
  if(Options.existsAs<int>("nPhiBinsEE"))
    nPhiBinsEE = Options.getParameter<int>("nPhiBinsEE");

  int nPhiBinsTempEB = 1;
  if(Options.existsAs<int>("nPhiBinsTempEB"))
    nPhiBinsTempEB = Options.getParameter<int>("nPhiBinsTempEB");

  int nPhiBinsTempEE = 1;
  if(Options.existsAs<int>("nPhiBinsTempEE"))
    nPhiBinsTempEE = Options.getParameter<int>("nPhiBinsTempEE");

  int rebinEB = 1;
  if(Options.existsAs<int>("rebinEB"))
    rebinEB = Options.getParameter<int>("rebinEB");

  int rebinEE = 1;
  if(Options.existsAs<int>("rebinEE"))
    rebinEE = Options.getParameter<int>("rebinEE");

  int nRegionsEB = 1;
  if(Options.existsAs<int>("nRegionsEB"))
    nRegionsEB = Options.getParameter<int>("nRegionsEB");

  int nRegionsEE = 1;
  if(Options.existsAs<int>("nRegionsEE"))
    nRegionsEE = Options.getParameter<int>("nRegionsEE");

  std::string outputFile = "NULL";
  if(Options.existsAs<std::string>("outputFile"))
    outputFile = Options.getParameter<std::string>("outputFile");

  bool usePUweights = false;
  if(Options.existsAs<bool>("usePUweights"))
    usePUweights = Options.getParameter<bool>("usePUweights");
    
  cout <<" Basic Configuration " <<endl;
  cout <<" Tree Name = "<<TreeName<<endl;
  cout <<" infileDATA = "<<infileDATA<<endl;
  cout <<" infileMC = "<<infileMC<<endl;
  cout <<" WeightforMC = "<<WeightforMC<<endl;
  cout <<" nRegionsEB = "<<nRegionsEB<<endl;
  cout <<" nRegionsEE = "<<nRegionsEE<<endl;
  cout <<" nPhiBinsEB = "<<nPhiBinsEB<<endl;
  cout <<" nPhiBinsEE = "<<nPhiBinsEE<<endl;
  cout <<" nPhiBinsTempEB = "<<nPhiBinsTempEB<<endl;
  cout <<" nPhiBinsTempEE = "<<nPhiBinsTempEE<<endl;
  cout <<" rebinEB = "<<rebinEB<<endl;
  cout <<" rebinEE = "<<rebinEE<<endl;
  cout <<" usePUweights = "<<usePUweights<<endl;
  
  cout << "Making calibration plots for Momentum scale studies "<< endl;
  
  
  
  //---- variables for selection
  float etaMax  = 2.5;
  float eta2Max = 2.5;
  
  //--- weights for MC
  TFile weightsFile (WeightforMC.c_str(),"READ"); 
  TH1F* hweights = (TH1F*)weightsFile.Get("hweights");
  
  float w[100];
  for (int ibin = 1; ibin < hweights->GetNbinsX()+1; ibin++){
    w[ibin-1] = hweights->GetBinContent(ibin);  // bin 1 --> nvtx = 0 
  }
  weightsFile.Close();
  
  //histos to get the bin in phi given the electron phi
  TH1F* hPhiBinEB = new TH1F("hphiEB","hphiEB",nPhiBinsEB, -1.*TMath::Pi(),1.*TMath::Pi());
  TH1F* hPhiBinEE = new TH1F("hphiEE","hphiEE",nPhiBinsEE, -1.*TMath::Pi(),1.*TMath::Pi());
  
  //----- NTUPLES--------------------
  TChain *ntu_DA = new TChain(TreeName.c_str());
  TChain *ntu_MC = new TChain(TreeName.c_str());
  
  if(!FillChain(*ntu_DA, infileDATA.c_str())) return 1;
  if(!FillChain(*ntu_MC, infileMC.c_str()))   return 1;
  
  std::cout << "     DATA: " << ntu_DA->GetEntries() << " entries in Data sample" << std::endl;
  std::cout << "     MC  : " << ntu_MC->GetEntries() << " entries in  MC  sample" << std::endl;
  
  // observables  
  int isW;
  float mZ;
  float scEta,  scPhi;
  float scEta2, scPhi2;
  float eleEta,  elePhi;
  float eleEta2, elePhi2;
  float scEne,  scEneReg,  scEt,  scERaw,  e3x3,  R9;
  float scEne2, scEneReg2, scEt2, scERaw2, e3x32, R92;
  float charge, charge2;
  float pTK,pTK2; 
  int iphiSeed,  ele1_ix, ele1_iy, ele1_iz; 
  int iphiSeed2, ele2_ix, ele2_iy, ele2_iz;
  int npu;
  
  // Set branch addresses for Data  
  ntu_DA->SetBranchStatus("*",0);
  ntu_DA->SetBranchStatus("isW", 1);                 ntu_DA->SetBranchAddress("isW", &isW);
  ntu_DA->SetBranchStatus("ele1_eta", 1);            ntu_DA->SetBranchAddress("ele1_eta", &eleEta);
  ntu_DA->SetBranchStatus("ele2_eta", 1);            ntu_DA->SetBranchAddress("ele2_eta", &eleEta2);
  ntu_DA->SetBranchStatus("ele1_phi", 1);            ntu_DA->SetBranchAddress("ele1_phi", &elePhi);
  ntu_DA->SetBranchStatus("ele2_phi", 1);            ntu_DA->SetBranchAddress("ele2_phi", &elePhi2);
  ntu_DA->SetBranchStatus("ele1_scEta", 1);          ntu_DA->SetBranchAddress("ele1_scEta", &scEta);
  ntu_DA->SetBranchStatus("ele2_scEta", 1);          ntu_DA->SetBranchAddress("ele2_scEta", &scEta2);
  ntu_DA->SetBranchStatus("ele1_scPhi", 1);          ntu_DA->SetBranchAddress("ele1_scPhi", &scPhi);
  ntu_DA->SetBranchStatus("ele2_scPhi", 1);          ntu_DA->SetBranchAddress("ele2_scPhi", &scPhi2);
  ntu_DA->SetBranchStatus("ele1_scE", 1);            ntu_DA->SetBranchAddress("ele1_scE", &scEne);
  ntu_DA->SetBranchStatus("ele2_scE", 1);            ntu_DA->SetBranchAddress("ele2_scE", &scEne2);
  ntu_DA->SetBranchStatus("ele1_scE_regression", 1); ntu_DA->SetBranchAddress("ele1_scE_regression", &scEneReg);
  ntu_DA->SetBranchStatus("ele2_scE_regression", 1); ntu_DA->SetBranchAddress("ele2_scE_regression", &scEneReg2);
  ntu_DA->SetBranchStatus("ele1_scEt", 1);           ntu_DA->SetBranchAddress("ele1_scEt",&scEt);
  ntu_DA->SetBranchStatus("ele2_scEt", 1);           ntu_DA->SetBranchAddress("ele2_scEt",&scEt2);
  ntu_DA->SetBranchStatus("ele1_scERaw", 1);         ntu_DA->SetBranchAddress("ele1_scERaw", &scERaw);
  ntu_DA->SetBranchStatus("ele2_scERaw", 1);         ntu_DA->SetBranchAddress("ele2_scERaw", &scERaw2);
  ntu_DA->SetBranchStatus("ele1_e3x3", 1);           ntu_DA->SetBranchAddress("ele1_e3x3", &e3x3);
  ntu_DA->SetBranchStatus("ele2_e3x3", 1);           ntu_DA->SetBranchAddress("ele2_e3x3", &e3x32);
  ntu_DA->SetBranchStatus("ele1ele2_scM", 1);        ntu_DA->SetBranchAddress("ele1ele2_scM", &mZ);
  ntu_DA->SetBranchStatus("ele1_charge", 1);         ntu_DA->SetBranchAddress("ele1_charge", &charge);
  ntu_DA->SetBranchStatus("ele2_charge", 1);         ntu_DA->SetBranchAddress("ele2_charge", &charge2);
  ntu_DA->SetBranchStatus("ele1_tkP", 1);            ntu_DA->SetBranchAddress("ele1_tkP", &pTK);
  ntu_DA->SetBranchStatus("ele2_tkP", 1);            ntu_DA->SetBranchAddress("ele2_tkP", &pTK2);
  ntu_DA->SetBranchStatus("ele1_seedIphi", 1);       ntu_DA->SetBranchAddress("ele1_seedIphi", &iphiSeed);
  ntu_DA->SetBranchStatus("ele2_seedIphi", 1);       ntu_DA->SetBranchAddress("ele2_seedIphi", &iphiSeed2);
  ntu_DA->SetBranchStatus("ele1_seedIx", 1);         ntu_DA->SetBranchAddress("ele1_seedIx", &ele1_ix);
  ntu_DA->SetBranchStatus("ele2_seedIx", 1);         ntu_DA->SetBranchAddress("ele2_seedIx", &ele2_ix);
  ntu_DA->SetBranchStatus("ele1_seedIy", 1);         ntu_DA->SetBranchAddress("ele1_seedIy", &ele1_iy);
  ntu_DA->SetBranchStatus("ele2_seedIy", 1);         ntu_DA->SetBranchAddress("ele2_seedIy", &ele2_iy);
  ntu_DA->SetBranchStatus("ele1_seedZside", 1);      ntu_DA->SetBranchAddress("ele1_seedZside", &ele1_iz);
  ntu_DA->SetBranchStatus("ele2_seedZside", 1);      ntu_DA->SetBranchAddress("ele2_seedZside", &ele2_iz);
  
  
  // Set branch addresses for MC
  ntu_MC->SetBranchStatus("*",0);
  ntu_MC->SetBranchStatus("isW", 1);                  ntu_MC->SetBranchAddress("isW", &isW);
  ntu_MC->SetBranchStatus("ele1_eta", 1);             ntu_MC->SetBranchAddress("ele1_eta", &eleEta);
  ntu_MC->SetBranchStatus("ele2_eta", 1);             ntu_MC->SetBranchAddress("ele2_eta", &eleEta2);
  ntu_MC->SetBranchStatus("ele1_phi", 1);             ntu_MC->SetBranchAddress("ele1_phi", &elePhi);
  ntu_MC->SetBranchStatus("ele2_phi", 1);             ntu_MC->SetBranchAddress("ele2_phi", &elePhi2);
  ntu_MC->SetBranchStatus("ele1_scEta", 1);           ntu_MC->SetBranchAddress("ele1_scEta", &scEta);
  ntu_MC->SetBranchStatus("ele2_scEta", 1);           ntu_MC->SetBranchAddress("ele2_scEta", &scEta2);
  ntu_MC->SetBranchStatus("ele1_scPhi", 1);           ntu_MC->SetBranchAddress("ele1_scPhi", &scPhi);
  ntu_MC->SetBranchStatus("ele2_scPhi", 1);           ntu_MC->SetBranchAddress("ele2_scPhi", &scPhi2);
  ntu_MC->SetBranchStatus("ele1_scE", 1);             ntu_MC->SetBranchAddress("ele1_scE", &scEne);
  ntu_MC->SetBranchStatus("ele2_scE", 1);             ntu_MC->SetBranchAddress("ele2_scE", &scEne2);
  ntu_MC->SetBranchStatus("ele1_scE_regression", 1);  ntu_MC->SetBranchAddress("ele1_scE_regression", &scEneReg);
  ntu_MC->SetBranchStatus("ele2_scE_regression", 1);  ntu_MC->SetBranchAddress("ele2_scE_regression", &scEneReg2);
  ntu_MC->SetBranchStatus("ele1_scEt", 1);            ntu_MC->SetBranchAddress("ele1_scEt",&scEt);
  ntu_MC->SetBranchStatus("ele2_scEt", 1);            ntu_MC->SetBranchAddress("ele2_scEt",&scEt2);
  ntu_MC->SetBranchStatus("ele1_scERaw", 1);          ntu_MC->SetBranchAddress("ele1_scERaw", &scERaw);
  ntu_MC->SetBranchStatus("ele2_scERaw", 1);          ntu_MC->SetBranchAddress("ele2_scERaw", &scERaw2);
  ntu_MC->SetBranchStatus("ele1_e3x3", 1);            ntu_MC->SetBranchAddress("ele1_e3x3", &e3x3);
  ntu_MC->SetBranchStatus("ele2_e3x3", 1);            ntu_MC->SetBranchAddress("ele2_e3x3", &e3x32);
  ntu_MC->SetBranchStatus("ele1ele2_scM", 1);         ntu_MC->SetBranchAddress("ele1ele2_scM", &mZ);
  ntu_MC->SetBranchStatus("ele1_charge", 1);          ntu_MC->SetBranchAddress("ele1_charge", &charge);
  ntu_MC->SetBranchStatus("ele2_charge", 1);          ntu_MC->SetBranchAddress("ele2_charge", &charge2);
  ntu_MC->SetBranchStatus("ele1_tkP", 1);             ntu_MC->SetBranchAddress("ele1_tkP", &pTK);
  ntu_MC->SetBranchStatus("ele2_tkP", 1);             ntu_MC->SetBranchAddress("ele2_tkP", &pTK2);
  ntu_MC->SetBranchStatus("ele1_seedIphi", 1);        ntu_MC->SetBranchAddress("ele1_seedIphi", &iphiSeed);
  ntu_MC->SetBranchStatus("ele2_seedIphi", 1);        ntu_MC->SetBranchAddress("ele2_seedIphi", &iphiSeed2);
  ntu_MC->SetBranchStatus("ele1_seedIx", 1);          ntu_MC->SetBranchAddress("ele1_seedIx", &ele1_ix);
  ntu_MC->SetBranchStatus("ele2_seedIx", 1);          ntu_MC->SetBranchAddress("ele2_seedIx", &ele2_ix);
  ntu_MC->SetBranchStatus("ele1_seedIy", 1);          ntu_MC->SetBranchAddress("ele1_seedIy", &ele1_iy);
  ntu_MC->SetBranchStatus("ele2_seedIy", 1);          ntu_MC->SetBranchAddress("ele2_seedIy", &ele2_iy);
  ntu_MC->SetBranchStatus("ele1_seedZside", 1);       ntu_MC->SetBranchAddress("ele1_seedZside", &ele1_iz);
  ntu_MC->SetBranchStatus("ele2_seedZside", 1);       ntu_MC->SetBranchAddress("ele2_seedZside", &ele2_iz);
  if(usePUweights)
  {
    ntu_MC->SetBranchStatus("PUit_NumInteractions", 1); ntu_MC->SetBranchAddress("PUit_NumInteractions", &npu);
  }
  
  
  
  // histogram definition in EB and fit functions
  std::vector<std::vector<TH1F*> > h_Phi_EB(nPhiBinsEB); // used to map iEta (as defined for Barrel and Endcap geom) into eta 
  std::vector<std::vector<TH1F*> > h_EoP_EB(nPhiBinsEB);   
  std::vector<std::vector<TH1F*> > h_EoC_EB(nPhiBinsEB); 
  std::vector<std::vector<TF1*> > f_EoP_EB(nPhiBinsEB);
  std::vector<std::vector<TF1*> > f_EoC_EB(nPhiBinsEB);
  
  // histogram definition in EE and fit functions
  std::vector<std::vector<TH1F*> > h_Phi_EE(nPhiBinsEE); // used to map iEta (as defined for Barrel and Endcap geom) into eta 
  std::vector<std::vector<TH1F*> > h_EoP_EE(nPhiBinsEE);   
  std::vector<std::vector<TH1F*> > h_EoC_EE(nPhiBinsEE);  
  std::vector<std::vector<TF1*> > f_EoP_EE(nPhiBinsEE);
  std::vector<std::vector<TF1*> > f_EoC_EE(nPhiBinsEE);
  
  
  
  // Initializate histos in EB
  std::cout << ">>> Initialize EB histos" << std::endl;
  for(int i = 0; i < nPhiBinsEB; ++i)
  {
    for(int j = 0; j < nRegionsEB; ++j)
    {
      TString histoName;
      histoName= Form("EB_EoP_%d_%d", i,j);
      TH1F* temp = new TH1F (histoName, histoName, 2200, 0.2, 1.6);
      temp->Sumw2();
      temp->SetFillColor(kRed+2);
      temp->SetLineColor(kRed+2);
      temp->SetFillStyle(3004);
      (h_EoP_EB.at(i)).push_back(temp);
      
      histoName=Form("EB_EoC_%d_%d", i,j);
      temp = new TH1F(histoName, histoName, 2200, 0.2, 1.6);
      temp->Sumw2();
      temp->SetFillColor(kGreen+2);
      temp->SetLineColor(kGreen+2);
      temp->SetFillStyle(3004);
      (h_EoC_EB.at(i)).push_back(temp);
      
      histoName=Form("EB_Phi_%d_%d", i,j);
      temp = new TH1F(histoName, histoName, 360, 0., 360.); 
      (h_Phi_EB.at(i)).push_back(temp); 
    }
  }
  
  // Initializate histos in EE
  std::cout << ">>> Initialize EE histos" << std::endl;
  for(int i = 0; i < nPhiBinsEE; ++i)
  {
    for(int j = 0; j < nRegionsEE; ++j)
    {
      TString histoName;
      histoName= Form("EE_EoP_%d_%d", i,j);
      TH1F* temp = new TH1F (histoName, histoName, 2200, 0., 2.);
      temp->Sumw2();
      temp->SetFillColor(kRed+2);
      temp->SetLineColor(kRed+2);
      temp->SetFillStyle(3004);
      (h_EoP_EE.at(i)).push_back(temp);
      
      histoName=Form("EE_EoC_%d_%d", i,j);
      temp = new TH1F(histoName, histoName, 2200, 0., 2.);
      temp->Sumw2();
      temp->SetFillColor(kGreen+2);
      temp->SetLineColor(kGreen+2);
      temp->SetFillStyle(3004);
      (h_EoC_EE.at(i)).push_back(temp);
      
      histoName=Form("EE_Phi_%d_%d", i,j);
      temp = new TH1F(histoName, histoName, 360, 0., 360.); 
      (h_Phi_EE.at(i)).push_back(temp); 
    }
  }
  
  
  
  // Template in EE and EB
  std::vector<std::vector<TH1F*> > h_template_EB(nPhiBinsTempEB);
  std::vector<std::vector<TH1F*> > h_template_EE(nPhiBinsTempEE);
  
  std::cout << ">>> Initialize EB template" << std::endl;
  for(int mod = 0; mod < nPhiBinsTempEB; ++mod)
  {
    for(int j = 0; j < nRegionsEB; ++j)
    {
      TString histoName;
      histoName=Form("EB_template_%d_%d",mod,j);
      TH1F* temp = new TH1F(histoName,"",2200,0.2,1.6);
      (h_template_EB.at(mod)).push_back(temp);
    }
  }
  
  std::cout << ">>> Initialize EE template" << std::endl;
  for(int mod = 0; mod < nPhiBinsTempEE; ++mod)
  {
    for(int j = 0; j < nRegionsEE; ++j)
    {
      TString histoName;
      histoName=Form("EE_template_%d_%d",mod,j);
      TH1F* temp = new TH1F(histoName,"",2200,0.,2.);
      (h_template_EE.at(mod)).push_back(temp);
    }
  }
  
  
  TH1F** h_phi_data_EB = new TH1F*[nRegionsEB];
  TH1F** h_phi_mc_EB   = new TH1F*[nRegionsEB];
  TH1F** h_phi_data_EE = new TH1F*[nRegionsEE];
  TH1F** h_phi_mc_EE   = new TH1F*[nRegionsEE];

  for(int index = 0; index < nRegionsEB; ++index)
  {
    TString name;
    name=Form("EB_h_phi_data_%d",index);
    h_phi_data_EB[index]= new TH1F(name,"h_phi_data",100,-TMath::Pi(),TMath::Pi());
    name=Form("EB_h_phi_mc_%d",index);
    h_phi_mc_EB[index]= new TH1F(name,"h_phi_mc",100,-TMath::Pi(),TMath::Pi());
  }
  
  for(int index = 0; index < nRegionsEE; ++index)
  {
    TString name;
    name=Form("EE_h_phi_data_%d",index);
    h_phi_data_EE[index] = new TH1F(name,"h_phi_data",100,-TMath::Pi(),TMath::Pi());
    name=Form("EE_h_phi_mc_%d",index);
    h_phi_mc_EE[index] =  new TH1F(name,"h_phi_mc",100,-TMath::Pi(),TMath::Pi());
  }
  
  TH1F* h_et_data = new TH1F("h_et_data","h_et_data",100,0.,100.);
  TH1F* h_et_mc   = new TH1F("h_et_mc",  "h_et_mc",  100,0.,100.);
  
  
  
  // Initialize endcap geometry
  TEndcapRings *eRings = new TEndcapRings(); 
  
  // Map for conversion (ix,iy) into Eta for EE
  TH2F * mapConversionEEp = new TH2F ("mapConversionEEp","mapConversionEEp",101,1,101,101,1,101);
  TH2F * mapConversionEEm = new TH2F ("mapConversionEEm","mapConversionEEm",101,1,101,101,1,101);
  
  for(int ix =0; ix<mapConversionEEp->GetNbinsX(); ix++)
    for(int iy =0; iy<mapConversionEEp->GetNbinsY(); iy++)
    {
      mapConversionEEp->SetBinContent(ix+1,iy+1,0);
      mapConversionEEm->SetBinContent(ix+1,iy+1,0);
    }
  
  
  
  // fill MC templates
  std::vector<int> refIdEB;
  refIdEB.assign(nPhiBinsEB,0);
  
  std::vector<int> refIdEE;
  refIdEE.assign(nPhiBinsEE,0);
  
  for(int iphi = 0; iphi < nPhiBinsEB; ++iphi)
  {
    float phi = hPhiBinEB->GetBinCenter(iphi+1);
    
    phi = 2.*TMath::Pi() + phi + TMath::Pi()*10./180.;
    phi -= int(phi/2./TMath::Pi()) * 2.*TMath::Pi();
    
    int modPhi = int(phi/(2.*TMath::Pi()/nPhiBinsTempEB));
    if( modPhi == nPhiBinsTempEB ) modPhi = 0;
    refIdEB.at(iphi) = modPhi;
  }
  
  for(int iphi = 0; iphi < nPhiBinsEE; ++iphi)
  {
    float phi = hPhiBinEE->GetBinCenter(iphi+1);
    
    phi = 2.*TMath::Pi() + phi + TMath::Pi()*10./180.;
    phi -= int(phi/2./TMath::Pi()) * 2.*TMath::Pi();
    
    int modPhi = int(phi/(2.*TMath::Pi()/nPhiBinsTempEE));
    if( modPhi == nPhiBinsTempEE ) modPhi = 0;
    refIdEE.at(iphi) = modPhi;
  }
  
  
    
  
  
  
  //**************************** loop on MC, make refernce and fit dist
  
  float var = 0.;
  std::cout << "Loop in MC events " << endl; 
  for(int entry = 0; entry < ntu_MC->GetEntries(); ++entry)
  {
    if( entry%10000 == 0 ) std::cout << "reading saved entry " << entry << "\r" << std::flush;
    //if( entry > 1000 ) break;
    
    ntu_MC->GetEntry(entry);
    
    if( isW == 1 )               continue;
    if( fabs(scEta)  > etaMax )  continue;
    if( fabs(scEta2) > eta2Max ) continue;
    if( scEt  < 20. ) continue;
    if( scEt2 < 20. ) continue;
    
    R9  = e3x3  / scERaw;
    R92 = e3x32 / scERaw2;
    
    
    //--- PU weights
    float ww = 1.;
    if( usePUweights ) ww *= w[npu];
    
    
    
    //--- set the mass for ele1
    var = ( mZ * sqrt(pTK/scEne) * sqrt(scEneReg2/scEne2) ) / 91.19;
    // simulate e+/e- asymmetry
    //if( charge > 0 ) ww *= 1.*(6/5);
    //else             ww *= 1.*(4/5);
    
    // MC - BARREL - ele1
    if( ele1_iz == 0 )
    {
      // fill MC templates
      int modPhi = int(iphiSeed/(360./nPhiBinsTempEB));
      if( modPhi == nPhiBinsTempEB ) modPhi = 0;
      
      int regionId = templIndexEB(typeEB,eleEta,charge,R9);
      if( regionId == -1 ) continue;
      
      (h_template_EB.at(modPhi)).at(regionId) -> Fill(var*var,ww);
      
      
      // fill MC histos in eta bins
      int PhibinEB = hPhiBinEB->FindBin(elePhi) - 1;
      if( PhibinEB == nPhiBinsEB ) PhibinEB = 0;
      
      (h_EoP_EB.at(PhibinEB)).at(regionId) -> Fill(var*var,ww);  // This is MC
      h_phi_mc_EB[regionId] -> Fill(scPhi,ww);
    }
    
    // MC - ENDCAP - ele1
    else
    { 
      int iphi = eRings->GetEndcapIphi(ele1_ix,ele1_iy,ele1_iz);
      
      if( ele1_iz ==  1 )mapConversionEEp -> SetBinContent(ele1_ix,ele1_iy,scEta);
      if( ele1_iz == -1 )mapConversionEEm -> SetBinContent(ele1_ix,ele1_iy,scEta);
      
      
      // fill MC templates
      int modPhi = int (iphi/(360./nPhiBinsTempEE));
      if( modPhi == nPhiBinsTempEE ) modPhi = 0;
      
      int regionId =  templIndexEE(typeEE,eleEta,charge,R9);
      if( regionId == -1 ) continue;
      
      (h_template_EE.at(modPhi)).at(regionId) -> Fill(var*var,ww);
      
      
      // fill MC histos in eta bins
      int PhibinEE = hPhiBinEE->FindBin(elePhi) - 1;
      if( PhibinEE == nPhiBinsEE ) PhibinEE = 0;
      
      (h_EoP_EE.at(PhibinEE)).at(regionId) -> Fill(var*var,ww);  // This is MC
      h_phi_mc_EE[regionId] -> Fill(scPhi,ww);
    }
    
    
    
    //--- set the mass for ele2
    var = ( mZ * sqrt(pTK2/scEne2) * sqrt(scEneReg/scEne) ) / 91.19;
    // simulate e+/e- asymmetry
    //if( charge > 0 ) ww *= 1.*(6/5);
    //else             ww *= 1.*(4/5);
        
    // MC - BARREL - ele2 
    if( ele2_iz == 0)
    {
       // fill MC templates
       int modPhi = int (iphiSeed2/(360./nPhiBinsTempEB));
       if( modPhi == nPhiBinsTempEB ) modPhi = 0;
       
       int regionId  = templIndexEB(typeEB,eleEta2,charge2,R92);
       if(regionId == -1) continue;
       
       (h_template_EB.at(modPhi)).at(regionId)->Fill(var*var,ww);
       
       
       // fill MC histos in eta bins
       int PhibinEB = hPhiBinEB->FindBin(elePhi2) - 1;
       if( PhibinEB==nPhiBinsEB ) PhibinEB = 0;
       
       (h_EoP_EB.at(PhibinEB)).at(regionId) -> Fill(var*var,ww); // This is MC
       h_phi_mc_EB[regionId]->Fill(scPhi2,ww);
    }
    
    // MC - ENDCAP - ele2
    else
    { 
      if( ele2_iz ==  1 ) mapConversionEEp -> SetBinContent(ele2_ix,ele2_iy,scEta2);
      if( ele2_iz == -1 ) mapConversionEEm -> SetBinContent(ele2_ix,ele2_iy,scEta2);
      
      int iphi = eRings->GetEndcapIphi(ele2_ix,ele2_iy,ele2_iz);
      
      
      // fill MC templates
      int modPhi = int (iphi/(360./nPhiBinsTempEE));
      if( modPhi == nPhiBinsTempEE ) modPhi = 0;
      
      int regionId =  templIndexEE(typeEE,eleEta2,charge2,R92);
      if(regionId == -1) continue;
      
      (h_template_EE.at(modPhi)).at(regionId) ->  Fill(var*var,ww);
      
      
      // fill MC histos in eta bins
      int PhibinEE = hPhiBinEE->FindBin(elePhi2) - 1;
      if(PhibinEE==nPhiBinsEE) PhibinEE = 0;
      
      (h_EoP_EE.at(PhibinEE)).at(regionId) -> Fill(var*var,ww);  // This is MC
      h_phi_mc_EE[regionId]->Fill(scPhi2,ww);
    }
    
    h_et_mc ->Fill(scEt, ww);
    h_et_mc ->Fill(scEt2,ww);
  }
  
  
  
  
  
  
  //**************************** loop on DATA
  
  std::cout << "Loop in Data events " << endl; 
  
  for(int entry = 0; entry < ntu_DA->GetEntries(); ++entry)
  {
    if( entry%10000 == 0 ) std::cout << "reading saved entry " << entry << "\r" << std::flush;
    
    ntu_DA->GetEntry(entry);
    
    if( isW == 1 )               continue;
    if( fabs(scEta)  > etaMax )  continue;
    if( fabs(scEta2) > eta2Max ) continue;
    if( scEt  < 20. ) continue;
    if( scEt2 < 20. ) continue;    
    
    R9  = e3x3  / scERaw;
    R92 = e3x32 / scERaw2;
        
    float ww = 1.;
    
    
    
    //--- set the mass for ele1
    if( ele1_iz == 0 ) var = ( mZ * sqrt(pTK/scEne) * sqrt(scEneReg2/scEne2) ) / 91.19;
    else               var = ( mZ * sqrt(pTK/scEne) * sqrt(scEneReg2/scEne2) ) / 91.19;
    // simulate e+/e- asymmetry
    //if( charge > 0 ) ww *= 1.*(6/5);
    //else             ww *= 1.*(4/5);
    
    // DATA - BARREL - ele1
    if( ele1_iz == 0 )
    {
      int PhibinEB = hPhiBinEB->FindBin(elePhi) - 1;
      if( PhibinEB == nPhiBinsEB ) PhibinEB = 0;
      
      int regionId = templIndexEB(typeEB,eleEta,charge,R9);
      if(regionId == -1) continue;
      
      (h_EoC_EB.at(PhibinEB)).at(regionId) -> Fill(var*var,ww);  // This is DATA
      (h_Phi_EB.at(PhibinEB)).at(regionId) -> Fill(elePhi);
      h_phi_data_EB[regionId]->Fill(elePhi);
    }
    
    // DATA - ENDCAP - ele1
    else
    {
      if( ele1_iz ==  1 ) mapConversionEEp -> SetBinContent(ele1_ix,ele1_iy,scEta);
      if( ele1_iz == -1 ) mapConversionEEm -> SetBinContent(ele1_ix,ele1_iy,scEta);
      
      int PhibinEE = hPhiBinEE->FindBin(elePhi) - 1;
      if( PhibinEE == nPhiBinsEE ) PhibinEE = 0;
      
      int regionId = templIndexEE(typeEE,eleEta,charge,R9);
      if( regionId == -1 ) continue;
      
      (h_EoC_EE.at(PhibinEE)).at(regionId) -> Fill(var*var,ww);  // This is DATA
      (h_Phi_EE.at(PhibinEE)).at(regionId) -> Fill(elePhi); 
      h_phi_data_EE[regionId] -> Fill(elePhi);
    }
    
    
    
    //--- set the mass for ele2
    if( ele2_iz == 0 ) var = ( mZ * sqrt(pTK2/scEne2) * sqrt(scEneReg/scEne) ) / 91.19;
    else               var = ( mZ * sqrt(pTK2/scEne2) * sqrt(scEneReg/scEne) ) / 91.19;
    // simulate e+/e- asymmetry
    //if( charge2 > 0 ) ww *= 1.*(6/5);
    //else              ww *= 1.*(4/5);
        
    // DATA - BARREL - ele2
    if( ele2_iz == 0 )
    {
      int PhibinEB = hPhiBinEB->FindBin(elePhi2) - 1;
      if( PhibinEB == nPhiBinsEB ) PhibinEB = 0;
      
      int regionId = templIndexEB(typeEB,eleEta2,charge2,R92);
      if( regionId == -1 ) continue;
      
      (h_EoC_EB.at(PhibinEB)).at(regionId) -> Fill(var*var,ww);  // This is DATA
      (h_Phi_EB.at(PhibinEB)).at(regionId) -> Fill(elePhi2);
      h_phi_data_EB[regionId] -> Fill(elePhi2);
    }
    else
    {     
      if( ele2_iz ==  1 ) mapConversionEEp -> SetBinContent(ele2_ix,ele2_iy,scEta2);
      if( ele2_iz == -1 ) mapConversionEEm -> SetBinContent(ele2_ix,ele2_iy,scEta2);
      
      int PhibinEE = hPhiBinEE->FindBin(elePhi2) - 1;
      if( PhibinEE == nPhiBinsEE ) PhibinEE = 0;
      
      int regionId = templIndexEE(typeEE,eleEta2,charge2,R92);
      if( regionId == -1 ) continue;
      
      (h_EoC_EE.at(PhibinEE)).at(regionId) -> Fill(var*var,ww);  // This is DATA
      (h_Phi_EE.at(PhibinEE)).at(regionId) -> Fill(elePhi2); 
      h_phi_data_EE[regionId] ->Fill(elePhi2);
    }
    
    h_et_data ->Fill(scEt);
    h_et_data ->Fill(scEt2);
  }
  
  std::cout << "End loop: Analyze events " << endl; 
  
  
  
  
  
  
  //----------------
  // Initializations
  
  // initialize TGraphs
  TFile* o = new TFile((outputFile+"_"+typeEB+"_"+typeEE+".root").c_str(),"RECREATE");
  
  TGraphErrors** g_EoP_EB = new TGraphErrors*[nRegionsEB];
  TGraphErrors** g_EoC_EB = new TGraphErrors*[nRegionsEB];
  TGraphErrors** g_Rat_EB = new TGraphErrors*[nRegionsEB];
  
  for(int j = 0; j < nRegionsEB; ++j)
  {
    g_EoP_EB[j] = new TGraphErrors();
    g_EoC_EB[j] = new TGraphErrors();
    g_Rat_EB[j] = new TGraphErrors();
  }
  
  TGraphErrors** g_EoP_EE = new TGraphErrors*[nRegionsEB];
  TGraphErrors** g_EoC_EE = new TGraphErrors*[nRegionsEB];
  TGraphErrors** g_Rat_EE = new TGraphErrors*[nRegionsEB];
 
  for(int j = 0; j < nRegionsEE; ++j)
  {
    g_EoP_EE[j]= new TGraphErrors();
    g_EoC_EE[j]= new TGraphErrors();
    g_Rat_EE[j]= new TGraphErrors();
  }
  
  // initialize template functions  
  std::vector<std::vector<histoFunc*> > templateHistoFuncEB(nPhiBinsTempEB);
  std::vector<std::vector<histoFunc*> > templateHistoFuncEE(nPhiBinsTempEE);
  
  for(int mod = 0; mod < nPhiBinsTempEB; ++mod)
  {
    for(int j = 0; j < nRegionsEB; ++j)
    {
      (h_template_EB.at(mod)).at(j) -> Rebin(rebinEB);
      (templateHistoFuncEB.at(mod)).push_back( new histoFunc((h_template_EB.at(mod)).at(j)) );
    }
  }
  
  for(int mod = 0; mod < nPhiBinsTempEE; ++mod)
  {
    for(int j = 0; j < nRegionsEE; ++j)
    {
      (h_template_EE.at(mod)).at(j) -> Rebin(rebinEE);
      (templateHistoFuncEE.at(mod)).push_back( new histoFunc((h_template_EE.at(mod)).at(j)) );
    }
  }
  
  
  
  
  
  
  //-------------------
  // Template Fit in EB
  
  if( typeEB != "none" )
  {
    for(int i = 0; i < nPhiBinsEB; ++i)
    {
      for(int j = 0; j < nRegionsEB; ++j)
      {
        float flPhi = hPhiBinEB->GetXaxis()->GetBinCenter(i+1);
        
        (h_EoP_EB.at(i)).at(j) -> Rebin(rebinEB);
        (h_EoC_EB.at(i)).at(j) -> Rebin(rebinEB);    
        
        
        // define the fitting function
        // N.B. [0] * ( [1] * f( [1]*(x-[2]) ) )
        char funcName[50];
        sprintf(funcName,"f_EoP_%d_%d_Ref_%d_%d_EB",i,j,refIdEB.at(i),j);
        (f_EoP_EB.at(i)).push_back( new TF1(funcName, (templateHistoFuncEB.at(refIdEB.at(i))).at(j), 0.85, 1.1, 3, "histoFunc") );
        
        (f_EoP_EB.at(i)).at(j) -> SetParName(0,"Norm"); 
        (f_EoP_EB.at(i)).at(j) -> SetParName(1,"Scale factor"); 
        
        (f_EoP_EB.at(i)).at(j) -> SetLineWidth(1); 
        (f_EoP_EB.at(i)).at(j) -> SetLineColor(kRed+2); 
        (f_EoP_EB.at(i)).at(j) -> SetNpx(10000);
        
        (h_EoP_EB.at(i)).at(j) -> Scale(1*(h_EoC_EB.at(i)).at(j)->GetEntries()/(h_EoP_EB.at(i)).at(j)->GetEntries());
        
        // uncorrected    
        double xNorm = (h_EoP_EB.at(i)).at(j)->Integral()/(h_template_EB.at(refIdEB.at(i))).at(j)->Integral() *
                       (h_EoP_EB.at(i)).at(j)->GetBinWidth(1)/(h_template_EB.at(refIdEB.at(i))).at(j)->GetBinWidth(1); 
        
        
        (f_EoP_EB.at(i)).at(j) -> FixParameter(0, xNorm);
        (f_EoP_EB.at(i)).at(j) -> FixParameter(2, 0.);
        
        
        std::cout << "***** Fitting MC EB " << flPhi << " (" << i << "," << j << "):   ";
        TFitResultPtr rp;
        int fStatus; 
        for(int trial = 0; trial < 10; ++trial)
        {
          (f_EoP_EB.at(i)).at(j) -> SetParameter(1, 0.99);
          rp = (h_EoP_EB.at(i)).at(j) -> Fit(funcName, "QRWL+");
          fStatus = rp;
          if(fStatus !=4 && (f_EoP_EB.at(i)).at(j)->GetParError(1) != 0. )
	  { 
	    std::cout << "fit OK    ";
            
            double k = (f_EoP_EB.at(i)).at(j)->GetParameter(1);
            double eee = (f_EoP_EB.at(i)).at(j)->GetParError(1);
            g_EoP_EB[j] -> SetPoint(i, flPhi, 1./k);
            g_EoP_EB[j] -> SetPointError(i, 0., eee/k/k);
            
            break;
          }
          else if( trial == 9 )
          {
            std::cout << "fit BAD   ";
            
            g_EoP_EB[j] -> SetPoint(i, flPhi, 1.);
            g_EoP_EB[j] -> SetPointError(i, 0., 0.01);
          }
        }
        
        
        //ratio preparation
        float rat = (f_EoP_EB.at(i)).at(j)->GetParameter(1);
        float era = (f_EoP_EB.at(i)).at(j)->GetParError(1); 
        
        xNorm = (h_EoC_EB.at(i)).at(j)->Integral()/(h_template_EB.at(refIdEB.at(i))).at(j)->Integral() *
                (h_EoC_EB.at(i)).at(j)->GetBinWidth(1)/(h_template_EB.at(refIdEB.at(i))).at(j)->GetBinWidth(1); 
        
        sprintf(funcName,"f_EoC_%d_%d_Ref_%d_%d_EB",i,j,refIdEB.at(i),i);
        
        (f_EoC_EB.at(i)).push_back( new TF1(funcName, (templateHistoFuncEB.at(refIdEB.at(i))).at(j), 0.85, 1.1, 3, "histoFunc") );
        
        (f_EoC_EB.at(i)).at(j) -> SetParName(0,"Norm"); 
        (f_EoC_EB.at(i)).at(j) -> SetParName(1,"Scale factor"); 
        
        (f_EoC_EB.at(i)).at(j) -> SetLineWidth(1); 
        (f_EoC_EB.at(i)).at(j) -> SetLineColor(kGreen+2); 
        (f_EoC_EB.at(i)).at(j) -> SetNpx(10000);
        
        (f_EoC_EB.at(i)).at(j) -> FixParameter(0, xNorm);
        (f_EoC_EB.at(i)).at(j) -> FixParameter(2, 0.);
        
        
        std::cout << "***** Fitting DATA EB (" << i << "," << j << "):   ";
        for(int trial = 0; trial < 10; ++trial)
        {
          (f_EoC_EB.at(i)).at(j) -> SetParameter(1, 0.99);
          rp = (h_EoC_EB.at(i)).at(j) -> Fit(funcName, "QR+");
          fStatus = rp;
          if( fStatus !=4 && (f_EoC_EB.at(i)).at(j)->GetParError(1) != 0 )
          {
	    std::cout << "fit OK    ";
            
	    double k = (f_EoC_EB.at(i)).at(j)->GetParameter(1);
	    double eee = (f_EoC_EB.at(i)).at(j)->GetParError(1);
            g_EoC_EB[j] -> SetPoint(i, flPhi, 1./k);
            g_EoC_EB[j] -> SetPointError(i, 0., eee/k/k);
            
            break;
          }
          else if( trial == 9 )
          {
            std::cout << "fit BAD   ";
            
            g_EoC_EB[j] -> SetPoint(i, flPhi, 1.);
            g_EoC_EB[j] -> SetPointError(i, 0., 0.01);
          }
        }
        
        
        //ratio finalization
        rat /= (f_EoC_EB.at(i)).at(j)->GetParameter(1);
        era = rat*sqrt(era*era+(f_EoC_EB.at(i)).at(j)->GetParError(1)*(f_EoC_EB.at(i)).at(j)->GetParError(1)); 
        
        if(i==0) g_Rat_EB[j] -> SetPoint(i, 0.,    rat);
        else     g_Rat_EB[j] -> SetPoint(i, flPhi, rat);
        
        g_Rat_EB[j] -> SetPointError(i,  0. , era); 
        g_Rat_EB[j]->SetLineColor(kBlue+2); 
        
        std::cout << std::endl;
      }
    }
  }
  else
  {
    for(int i = 0; i < nPhiBinsEB; ++i)
    {  
      for(int j = 0; j < nRegionsEB; ++j)
      {
        float flPhi = hPhiBinEB->GetXaxis()->GetBinCenter(i+1);
        g_EoP_EB[j] -> SetPoint(i, flPhi, 1.);
        g_EoC_EB[j] -> SetPoint(i, flPhi, 1.);
        g_Rat_EB[j] -> SetPoint(i, flPhi, 1.);
      }
    }
  }
  
  
  
  
  
  
  //-------------------
  // Template Fit in EE
  
  if( typeEE != "none" )
  {
    for(int i = 0; i < nPhiBinsEE; ++i)
    {
      for(int j = 0; j < nRegionsEE; ++j)
      {
        float flPhi = hPhiBinEE->GetXaxis()->GetBinCenter(i);
        
        (h_EoP_EE.at(i)).at(j) -> Rebin(rebinEE);
        (h_EoC_EE.at(i)).at(j) -> Rebin(rebinEE);    
        
        
        // define the fitting function
        // N.B. [0] * ( [1] * f( [1]*(x-[2]) ) )
        
        char funcName[50];
        sprintf(funcName,"f_EoP_%d_%d_Ref_%d_%d_EE",i,j,refIdEE.at(i),j);
        (f_EoP_EE.at(i)).push_back( new TF1(funcName, (templateHistoFuncEE.at(refIdEE.at(i))).at(j), 0.7, 1.1, 3, "histoFunc") );
        
        (f_EoP_EE.at(i)).at(j) -> SetParName(0,"Norm"); 
        (f_EoP_EE.at(i)).at(j) -> SetParName(1,"Scale factor"); 
        
        (f_EoP_EE.at(i)).at(j) -> SetLineWidth(1); 
        (f_EoP_EE.at(i)).at(j) -> SetLineColor(kRed+2); 
        (f_EoP_EE.at(i)).at(j) -> SetNpx(10000);
        (f_EoP_EE.at(i)).at(j) -> SetNpx(10000);
       
        (h_EoP_EE.at(i)).at(j) -> Scale(1*(h_EoC_EE.at(i)).at(j)->GetEntries()/(h_EoP_EE.at(i)).at(j)->GetEntries());
        
        // uncorrected    
        double xNorm = (h_EoP_EE.at(i)).at(j)->Integral()/(h_template_EE.at(refIdEE.at(i))).at(j)->Integral() *
                       (h_EoP_EE.at(i)).at(j)->GetBinWidth(1)/(h_template_EE.at(refIdEE.at(i))).at(j)->GetBinWidth(1); 
        
        (f_EoP_EE.at(i)).at(j) -> FixParameter(0, xNorm);
        (f_EoP_EE.at(i)).at(j) -> FixParameter(2, 0.);
        
        
        std::cout << "***** Fitting MC EE " << flPhi << " (" << i << "," << j << "):   ";
        TFitResultPtr rp;
        int fStatus; 
        for(int trial = 0; trial < 10; ++trial)
        {
          (f_EoP_EE.at(i)).at(j) -> SetParameter(1, 0.99);
          rp = (h_EoP_EE.at(i)).at(j) -> Fit(funcName, "QRWL+");
          fStatus = rp;
          
          if( fStatus !=4 && (f_EoP_EE.at(i)).at(j)->GetParError(1) != 0. )
          {
            std::cout << "fit OK    ";
            
	    double k = (f_EoP_EE.at(i)).at(j)->GetParameter(1);
	    double eee = (f_EoP_EE.at(i)).at(j)->GetParError(1);
            g_EoP_EE[j] -> SetPoint(i, flPhi, 1./k);
            g_EoP_EE[j] -> SetPointError(i, 0., eee/k/k);
            
            break;
          }
          else if( trial == 9 )
          {
            std::cout << "fit BAD   ";
            
            g_EoP_EE[j] -> SetPoint(i, flPhi, 1.);
            g_EoP_EE[j] -> SetPointError(i, 0., 0.03);
          }
        }
        
        //ratio preparation
        float rat = (f_EoP_EE.at(i)).at(j)->GetParameter(1);
        float era = (f_EoP_EE.at(i)).at(j)->GetParError(1); 
        
        // corrected    
        xNorm = (h_EoC_EE.at(i)).at(j)->Integral()/(h_template_EE.at(refIdEE.at(i))).at(j)->Integral() *
                (h_EoC_EE.at(i)).at(j)->GetBinWidth(1)/(h_template_EE.at(refIdEE.at(i))).at(j)->GetBinWidth(1); 
        
        sprintf(funcName,"f_EoC_%d_%d_Ref_%d_%d_EE",i,j,refIdEE.at(i),j);
        (f_EoC_EE.at(i)).push_back( new TF1(funcName, (templateHistoFuncEE.at(refIdEE.at(i))).at(j), 0.7, 1.1, 3, "histoFunc") );
        
        (f_EoC_EE.at(i)).at(j) -> SetParName(0,"Norm"); 
        (f_EoC_EE.at(i)).at(j) -> SetParName(1,"Scale factor"); 
        
        (f_EoC_EE.at(i)).at(j) -> SetLineWidth(1); 
        (f_EoC_EE.at(i)).at(j) -> SetLineColor(kGreen+2); 
        (f_EoC_EE.at(i)).at(j) -> SetNpx(10000);
        
        (f_EoC_EE.at(i)).at(j) -> FixParameter(0, xNorm);
        (f_EoC_EE.at(i)).at(j) -> FixParameter(2, 0.);
        
        
        std::cout << "***** Fitting DATA EE " << flPhi << " (" << i << "," << j << "):   ";
        for(int trial = 0; trial < 10; ++trial)
        {
          (f_EoC_EE.at(i)).at(j) -> SetParameter(1, 0.99);
          rp = (h_EoC_EE.at(i)).at(j) -> Fit(funcName, "QR+");
          if( fStatus !=4 && (f_EoC_EE.at(i)).at(j)->GetParError(1) != 0. )
          {
            std::cout << "fit OK    ";
            
            double k = (f_EoC_EE.at(i)).at(j)->GetParameter(1);
            double eee = (f_EoC_EE.at(i)).at(j)->GetParError(1);
            g_EoC_EE[j] -> SetPoint(i, flPhi, 1./k);
            g_EoC_EE[j] -> SetPointError(i, 0., eee/k/k);
            
            break;
          }
          else if( trial == 9 )
          {
            g_EoC_EE[j] -> SetPoint(i, flPhi, 1.);
            g_EoC_EE[j] -> SetPointError(i, 0., 0.03);        
          }
        }
        
        
        //ratio finalization
        rat /= (f_EoC_EE.at(i)).at(j)->GetParameter(1);
        era = rat*sqrt(era*era+(f_EoC_EE.at(i)).at(j)->GetParError(1)*(f_EoC_EE.at(i)).at(j)->GetParError(1)); 
        
        g_Rat_EE[j] -> SetPoint(i, flPhi, rat);
        g_Rat_EE[j] -> SetPointError(i,  0. , era);
        
        g_Rat_EE[j]->SetLineColor(kBlue+2); 
      }
      
      std::cout << std::endl;
    }
  }
  else
  {
    for(int i = 0; i < nPhiBinsEE; ++i)
    {  
      for(int j = 0; j < nRegionsEE; ++j)
      {
        float flPhi = hPhiBinEE->GetXaxis()->GetBinCenter(i+1);
        g_EoP_EE[j] -> SetPoint(i, flPhi, 1.);
        g_EoC_EE[j] -> SetPoint(i, flPhi, 1.);
        g_Rat_EE[j] -> SetPoint(i, flPhi, 1.);
      }
    }
  }
  
  
  
  
  
  
  //-------
  // Output
   
  o -> cd();
  
  for(int j = 0; j < nRegionsEB; ++j)
  {
    TString Name;
    //Name = Form("g_EoP_EB_%d",j);
    //if(g_EoP_EB[j]->GetN()!=0) g_EoP_EB[j] -> Write(Name);
    Name = Form("g_EoC_EB_%d",j);
    if(g_EoC_EB[j]->GetN()!=0) g_EoC_EB[j] -> Write(Name);
    //Name = Form("g_Rat_EB_%d",j);
    //if(g_Rat_EB[j]->GetN()!=0) g_Rat_EB[j] -> Write(Name);
  }
  
  for(int j = 0; j < nRegionsEE; ++j)
  {
    TString Name;
    //Name = Form("g_EoP_EE_%d",j);
    //if(g_EoP_EE[j]->GetN()!=0) g_EoP_EE[j] -> Write(Name);
    Name = Form("g_EoC_EE_%d",j);
    if(g_EoC_EE[j]->GetN()!=0) g_EoC_EE[j] -> Write(Name);
    //Name = Form("g_Rat_EE_%d",j);
    //if(g_Rat_EE[j]->GetN()!=0) g_Rat_EE[j] -> Write(Name);
  }
  
  for(int mod = 0; mod<nPhiBinsTempEB; ++mod)
    for(int j = 0; j<nRegionsEB; ++j)
    {
      if( h_template_EB[mod][j] -> GetEntries() != 0 ) h_template_EB[mod][j] -> Write();
    }
  
  for(int mod = 0; mod<nPhiBinsTempEE; ++mod)
    for(int j = 0; j<nRegionsEE; ++j)
    {
      if( h_template_EE[mod][j] -> GetEntries() != 0 ) h_template_EE[mod][j] -> Write();
    }
  
  for(int i = 0; i<nPhiBinsEB; ++i)
    for(int j = 0; j<nRegionsEB; ++j)
    {
      //if( (h_EoP_EB.at(i)).at(j) -> GetEntries() != 0 ) (h_EoP_EB.at(i)).at(j) -> Write();
      //if( (h_EoC_EB.at(i)).at(j) -> GetEntries() != 0 ) (h_EoC_EB.at(i)).at(j) -> Write();
    }
  
  for(int i = 0; i<nPhiBinsEE; ++i)
    for(int j = 0; j<nRegionsEE; ++j)
    {
      //if( (h_EoP_EE.at(i)).at(j) -> GetEntries() != 0 ) (h_EoP_EE.at(i)).at(j) -> Write();
      //if( (h_EoC_EE.at(i)).at(j) -> GetEntries() != 0 ) (h_EoC_EE.at(i)).at(j) -> Write();
    }
  
  for(int j =0; j< nRegionsEB; ++j)
  {
    if( h_phi_mc_EB[j]   -> GetEntries() !=0 ) h_phi_mc_EB[j]   -> Write();
    if( h_phi_data_EB[j] -> GetEntries() !=0 ) h_phi_data_EB[j] -> Write();
  }
  
  for(int j =0; j< nRegionsEE; ++j)
  {
    if( h_phi_mc_EE[j]   -> GetEntries() !=0 ) h_phi_mc_EE[j]   -> Write(); 
    if( h_phi_data_EE[j] -> GetEntries() !=0 ) h_phi_data_EE[j] -> Write();
  }
  
  h_et_mc->Write();
  h_et_data->Write();
  
  mapConversionEEp -> Write();
  mapConversionEEm -> Write();
  
  o -> Close();
  
  
  
  return 0;
}
