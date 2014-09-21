//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Jul  2 02:30:39 2011 by ROOT version 5.27/06b
// from TTree ntu/ntu
// found on file: /data1/dimatteo/Calibration/Ntuples/Run2011A/WZAnalysisSingleXtal/WZAnalysis_SingleElectron_Run2011A-WElectron-May10ReReco-v1.root
//////////////////////////////////////////////////////////

#ifndef FastCalibratorEB_MVA_h
#define FastCalibratorEB_MVA_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>
#include <TProfile.h>
#include <vector>
#include "../../NtuplePackage/interface/hChain.h"
#include "../../NtuplePackage/interface/h2Chain.h"
#include <TGraphErrors.h>

#include <TLorentzVector.h>
#include "Math/PtEtaPhiE4D.h"
#include "Math/PtEtaPhiM4D.h"
#include "Math/LorentzVector.h"


class FastCalibratorEB_MVA {
  public :
    TTree          *fChain;   //!pointer to the analyzed TTree or TChain
    Int_t           fCurrent; //!current Tree number in a TChain

   ///! Declaration of leaf types
    
    Int_t           runId;
    Int_t           lumiId;
    Int_t           isW;
    Int_t           isZ;

   //! Declaration of regression weights

   Double_t BDTG_weightEB_W_1 ;
   Double_t BDTG_weightEB_W_2 ;
   Double_t BDTG_weightEB_Z_1 ;
   Double_t BDTG_weightEB_Z_2 ;
    
   TBranch *b_BDTG_weightEB_W_1;
   TBranch *b_BDTG_weightEB_W_2;
   TBranch *b_BDTG_weightEB_Z_1;
   TBranch *b_BDTG_weightEB_Z_2;

   ///! Ele 1 variables
    
    std::vector<float>   *ele1_recHit_E;
    std::vector<int>     *ele1_recHit_hashedIndex;
    std::vector<int>     *ele1_recHit_ietaORix;
    std::vector<int>     *ele1_recHit_iphiORiy;
    std::vector<int>     *ele1_recHit_flag;
  
    Float_t         ele1_scERaw;
    Float_t         ele1_scE;
    Float_t         ele1_scEta;
    Float_t         ele1_es;
    Float_t         ele1_e3x3;
    Float_t         ele1_tkP;
    Float_t         ele1_fbrem;
    Float_t         ele1_EOverP;
    Int_t           ele1_isEB;
    Int_t           ele1_isEBEEGap;
    Int_t           ele1_isEBEtaGap;
    Int_t           ele1_isEBPhiGap;
    Int_t           ele1_isEEDeeGap;
    Int_t           ele1_isEERingGap;
    Float_t         ele1_E_true;
    Float_t         ele1_DR;
    Float_t         ele1_scE_regression;
    Float_t         ele1_scERaw_PUcleaned;
 
//     Float_t         ele1_charge;

   ///! Ele 2 variables
    
    std::vector<float>   *ele2_recHit_E;
    std::vector<int>     *ele2_recHit_hashedIndex;
    std::vector<int>     *ele2_recHit_iphiORiy;
    std::vector<int>     *ele2_recHit_ietaORix;
    std::vector<int>     *ele2_recHit_flag;
    
    Float_t         ele2_scERaw;
    Float_t         ele2_scE;
    Float_t         ele2_scEta;
    Float_t         ele2_es;
    Float_t         ele2_e3x3;
    Float_t         ele2_tkP;
    Float_t         ele2_fbrem;
    Float_t         ele2_EOverP;
    Int_t           ele2_isEB;
    Int_t           ele2_isEBEEGap;
    Int_t           ele2_isEBEtaGap;
    Int_t           ele2_isEBPhiGap;
    Int_t           ele2_isEEDeeGap;
    Int_t           ele2_isEERingGap;
    Float_t         ele2_E_true;
    Float_t         ele2_DR;
    Float_t         ele2_scE_regression;
    Float_t         ele2_scERaw_PUcleaned;

//     Float_t         ele2_charge;

   ///! List of input branches from ntu
    
    TBranch        *b_runId;   //!
    TBranch        *b_lumiId;   //!
    TBranch        *b_isW;   //!
    TBranch        *b_isZ;   //!
   
    TBranch        *b_ele1_recHit_E;   //!
    TBranch        *b_ele1_recHit_hashedIndex;
    TBranch        *b_ele1_recHit_iphiORiy;
    TBranch        *b_ele1_recHit_ietaORix;
    TBranch        *b_ele1_recHit_flag;
    TBranch        *b_ele1_scERaw_PUcleaned;
    TBranch        *b_ele1_scE_regression;

    TBranch        *b_ele1_scERaw;   //!
    TBranch        *b_ele1_scE;   //!
    TBranch        *b_ele1_scEta;   //!
    TBranch        *b_ele1_es;   //!
    TBranch        *b_ele1_e3x3;   //!
    TBranch        *b_ele1_tkP;   //!
    TBranch        *b_ele1_fbrem;   //!
    TBranch        *b_ele1_EOverP;   //!
    TBranch        *b_ele1_isEB;   //!
    TBranch        *b_ele1_E_true;   //!
    TBranch        *b_ele1_DR;   //!
    TBranch        *b_ele1_charge;   //!
 
    TBranch        *b_ele1_isEBEEGap;   //!
    TBranch        *b_ele1_isEBEtaGap;   //!
    TBranch        *b_ele1_isEBPhiGap;   //!
    TBranch        *b_ele1_isEEDeeGap;   //!
    TBranch        *b_ele1_isEERingGap;   //!

    TBranch        *b_ele2_recHit_E;   //!
    TBranch        *b_ele2_recHit_hashedIndex;
    TBranch        *b_ele2_recHit_iphiORiy;
    TBranch        *b_ele2_recHit_ietaORix;   //!
    TBranch        *b_ele2_recHit_flag;
    TBranch        *b_ele2_scERaw;   //!
    TBranch        *b_ele2_scE;   //!
    TBranch        *b_ele2_scEta;   //!
    TBranch        *b_ele2_es;   //!
    TBranch        *b_ele2_e3x3;   //!
    TBranch        *b_ele2_tkP;   //!
    TBranch        *b_ele2_fbrem;   //!
    TBranch        *b_ele2_E_true;   //!
    TBranch        *b_ele2_DR;   //!
    TBranch        *b_ele2_charge;   //!
 
    TBranch        *b_ele2_scERaw_PUcleaned;
    TBranch        *b_ele2_scE_regression;

    TBranch        *b_ele2_EOverP;   //!
    TBranch        *b_ele2_isEB;   //!
    TBranch        *b_ele2_isEBEEGap;   //!
    TBranch        *b_ele2_isEBEtaGap;   //!
    TBranch        *b_ele2_isEBPhiGap;   //!
    TBranch        *b_ele2_isEEDeeGap;   //!
    TBranch        *b_ele2_isEERingGap;   //!

   ///! List of class methods
   
    FastCalibratorEB_MVA(TTree *tree=0, TString outEPDistribution="NULL");
   
    virtual ~FastCalibratorEB_MVA();
    
    virtual void     bookHistos(int);
    
    virtual void     saveHistos(TFile *f1);
    
    virtual Int_t    Cut(Long64_t entry);
    
    virtual Int_t    GetEntry(Long64_t entry);
    
    virtual Long64_t LoadTree(Long64_t entry);
    
    virtual void     Init(TTree *tree);
    
    virtual void     Loop(int, int, int, int, int,bool,bool,bool,bool,bool);
    
    virtual Bool_t   Notify();
    
    virtual void     Show(Long64_t entry = -1);
    
    virtual void     BuildEoPeta_ele(int,int,int,int,std::vector<float>,bool,bool,bool);

    virtual void     BuildWeightDistribution_ele(int,int,int,int,std::vector<float>,bool);
    
    virtual void     saveEoPeta(TFile * f2);
    
    virtual void     AcquireDeadXtal(TString imputDeadXtal);
    
    virtual bool     CheckDeadXtal(const int & iEta, const int & iPhi);

    ///! Output information 
    
    std::vector<int>   IetaValues;
    std::vector<int>   IphiValues;
    std::vector<float> ICValues;
    std::vector<float> meanICforPhiRingValues;
    std::vector<int> DeadXtal_HashedIndex;

    
    hChain     *hC_EoP_eta_ele;
    hChain     *hC_IntercalibValues;
    hChain     *hC_EoP;
    hChain     *hC_PullFromScalib;
    h2Chain    *hC_scale_EB;
    TH1F       *h_Occupancy_hashedIndex;
    TH2F       *h_occupancy;
    TProfile   *p_IntercalibValues_iEta;
    TH2F       *h_scalib_EB;
    TH2F       *h_scale_EB;
    TH2F       *h_scale_EB_meanOnPhi;
    TH1F       *h_scale_EB_hashedIndex;
    TH1F       *h_IntercalibSpread_iEta;
    TH1F       *h_IntercalibValues_test;
    TH1F       *h_Init_IntercalibValues;
    TH1F       *h_WeigthEB_W_1;
    TH1F       *h_WeigthEB_W_2;

   
    TH2F       *h_map_Dead_Channels ;

    TGraphErrors *g_ICmeanVsLoop;
    TGraphErrors *g_ICrmsVsLoop;

 private:
 
 TString outEPDistribution_p;

};

#endif
