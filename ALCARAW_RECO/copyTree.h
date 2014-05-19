#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <utility>
#include "TROOT.h"
#include "TChain.h"
#include "TTree.h"
#include "TH1D.h"
#include "TMath.h"


// variables for ROOT Tree
//Declaration of leaves types
   Int_t           runNumber;
   ULong64_t       eventNumber;
   Int_t           lumiBlock;
   ULong64_t       runTime;
   Float_t         mcGenWeight;
   Char_t          HLTfire;
   Int_t           nPU[1];
   Float_t         rho;
   Int_t           nPV;
   Int_t           eleID[2];
   Int_t           ZStat;
   Int_t           chargeEle[2];
   Float_t         etaSCEle[2];
   Float_t         phiSCEle[2];
   Float_t         etaEle[2];
   Float_t         phiEle[2];
   Int_t           classificationEle[2];
   Int_t           recoFlagsEle[2];
   Float_t         PtEle[2];
   Float_t         fbremEle[2];
   Float_t         seedXSCEle[2];
   Float_t         seedYSCEle[2];
   Float_t         seedEnergySCEle[2];
   Float_t         seedLCSCEle[2];
   Float_t         avgLCSCEle[2];
   UChar_t         gainEle[2];
   Float_t         energyMCEle[2];
   Float_t         energySCEle[2];
   Float_t         rawEnergySCEle[2];
   Float_t         esEnergySCEle[2];
   Float_t         energySCEle_corr[2];
   Float_t         energySCEle_regrCorr_ele[2];
   Float_t         energySCEle_regrCorr_pho[2];
   Float_t         energyEle_regrCorr_fra[2];
   Float_t         energySigmaEle_regrCorr_fra[2];
   Float_t         energyEle_regrCorr_egamma[2];
   Float_t         energySigmaEle_regrCorr_egamma[2];
   Float_t         energySigmaSCEle_regrCorr_ele[2];
   Float_t         energySigmaSCEle_regrCorr_pho[2];
   Float_t         energySCEle_regrCorrSemiParV4_ele[2];
   Float_t         energySCEle_regrCorrSemiParV4_pho[2];
   Float_t         energySCEle_regrCorrSemiParV5_ele[2];
   Float_t         energySCEle_regrCorrSemiParV5_pho[2];
   Float_t         energySigmaSCEle_regrCorrSemiParV4_ele[2];
   Float_t         energySigmaSCEle_regrCorrSemiParV4_pho[2];
   Float_t         energySigmaSCEle_regrCorrSemiParV5_ele[2];
   Float_t         energySigmaSCEle_regrCorrSemiParV5_pho[2];
   Float_t         energySCEle_regrCorrSemiParV6_ele[2];
   Float_t         energySCEle_regrCorrSemiParV6_pho[2];
   Float_t         energySigmaSCEle_regrCorrSemiParV6_ele[2];
   Float_t         energySigmaSCEle_regrCorrSemiParV6_pho[2];
   Float_t         energySCEle_regrCorrSemiParV7_ele[2];
   Float_t         energySCEle_regrCorrSemiParV7_pho[2];
   Float_t         energySigmaSCEle_regrCorrSemiParV7_ele[2];
   Float_t         energySigmaSCEle_regrCorrSemiParV7_pho[2];
   Float_t         energySCEle_regrCorrSemiParV8_ele[2];
   Float_t         energySCEle_regrCorrSemiParV8_pho[2];
   Float_t         energySigmaSCEle_regrCorrSemiParV8_ele[2];
   Float_t         energySigmaSCEle_regrCorrSemiParV8_pho[2];
   Float_t         energySCEle_regrCorrSemiPar7TeVtrainV6_ele[2];
   Float_t         energySCEle_regrCorrSemiPar7TeVtrainV6_pho[2];
   Float_t         energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_ele[2];
   Float_t         energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_pho[2];
   Float_t         energySCEle_regrCorrSemiPar7TeVtrainV7_ele[2];
   Float_t         energySCEle_regrCorrSemiPar7TeVtrainV7_pho[2];
   Float_t         energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_ele[2];
   Float_t         energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_pho[2];
   Float_t         energySCEle_regrCorrSemiPar7TeVtrainV8_ele[2];
   Float_t         energySCEle_regrCorrSemiPar7TeVtrainV8_pho[2];
   Float_t         energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_ele[2];
   Float_t         energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_pho[2];
   Float_t         R9Ele[2];
   Float_t         e5x5SCEle[2];
   Float_t         pModeGsfEle[2];
   Float_t         pAtVtxGsfEle[2];
   Float_t         invMass;
   Float_t         invMass_SC;
   Float_t         invMass_e5x5;
   Float_t         invMass_rawSC;
   Float_t         invMass_rawSC_esSC;
   Float_t         invMass_SC_corr;
   Float_t         invMass_SC_regrCorr_ele;
   Float_t         invMass_SC_regrCorr_pho;
   Float_t         invMass_regrCorr_fra;
   Float_t         invMass_regrCorr_egamma;
   Float_t         invMass_SC_regrCorrSemiParV4_pho;
   Float_t         invMass_SC_regrCorrSemiParV4_ele;
   Float_t         invMass_SC_regrCorrSemiParV5_pho;
   Float_t         invMass_SC_regrCorrSemiParV5_ele;
   Float_t         invMass_SC_regrCorrSemiParV6_pho;
   Float_t         invMass_SC_regrCorrSemiParV6_ele;
   Float_t         invMass_SC_regrCorrSemiParV7_pho;
   Float_t         invMass_SC_regrCorrSemiParV7_ele;
   Float_t         invMass_SC_regrCorrSemiParV8_pho;
   Float_t         invMass_SC_regrCorrSemiParV8_ele;
   Float_t         invMass_SC_regrCorrSemiPar7TeVtrainV6_pho;
   Float_t         invMass_SC_regrCorrSemiPar7TeVtrainV6_ele;
   Float_t         invMass_SC_regrCorrSemiPar7TeVtrainV7_pho;
   Float_t         invMass_SC_regrCorrSemiPar7TeVtrainV7_ele;
   Float_t         invMass_SC_regrCorrSemiPar7TeVtrainV8_pho;
   Float_t         invMass_SC_regrCorrSemiPar7TeVtrainV8_ele;
   Float_t         invMass_MC;
   Float_t         etaMCEle[2];
   Float_t         phiMCEle[2];
   Int_t           nHitsSCEle[2];

// extra tree

//Declaration of leaves types
   std::vector<float>*   rawIdRecHitSCEle1;
   std::vector<float>*   rawIdRecHitSCEle2;
   std::vector<float>*   XRecHitSCEle1;
   std::vector<float>*   XRecHitSCEle2;
   std::vector<float>*   YRecHitSCEle1;
   std::vector<float>*   YRecHitSCEle2;
   std::vector<float>*   energyRecHitSCEle1;
   std::vector<float>*   energyRecHitSCEle2;
   std::vector<float>*   LCRecHitSCEle1;
   std::vector<float>*   LCRecHitSCEle2;
   std::vector<float>*   ICRecHitSCEle1;
   std::vector<float>*   ICRecHitSCEle2;
   std::vector<float>*   AlphaRecHitSCEle1;
   std::vector<float>*   AlphaRecHitSCEle2;



// set tree branches
void SetTreeBranch(TTree* tree)
{
   // Set branch addresses.
   tree->SetBranchAddress("runNumber",&runNumber);
   tree->SetBranchAddress("eventNumber",&eventNumber);
   tree->SetBranchAddress("lumiBlock",&lumiBlock);
   tree->SetBranchAddress("runTime",&runTime);
   tree->SetBranchAddress("mcGenWeight",&mcGenWeight);
   tree->SetBranchAddress("HLTfire",&HLTfire);
   tree->SetBranchAddress("nPU",nPU);
   tree->SetBranchAddress("rho",&rho);
   tree->SetBranchAddress("nPV",&nPV);
   tree->SetBranchAddress("eleID",eleID);
   tree->SetBranchAddress("ZStat",&ZStat);
   tree->SetBranchAddress("chargeEle",chargeEle);
   tree->SetBranchAddress("etaSCEle",etaSCEle);
   tree->SetBranchAddress("phiSCEle",phiSCEle);
   tree->SetBranchAddress("etaEle",etaEle);
   tree->SetBranchAddress("phiEle",phiEle);
   tree->SetBranchAddress("classificationEle",classificationEle);
   tree->SetBranchAddress("recoFlagsEle",recoFlagsEle);
   tree->SetBranchAddress("PtEle",PtEle);
   tree->SetBranchAddress("fbremEle",fbremEle);
   tree->SetBranchAddress("seedXSCEle",seedXSCEle);
   tree->SetBranchAddress("seedYSCEle",seedYSCEle);
   tree->SetBranchAddress("seedEnergySCEle",seedEnergySCEle);
   tree->SetBranchAddress("seedLCSCEle",seedLCSCEle);
   tree->SetBranchAddress("avgLCSCEle",avgLCSCEle);
   tree->SetBranchAddress("gainEle",gainEle);
   tree->SetBranchAddress("energyMCEle",energyMCEle);
   tree->SetBranchAddress("energySCEle",energySCEle);
   tree->SetBranchAddress("rawEnergySCEle",rawEnergySCEle);
   tree->SetBranchAddress("esEnergySCEle",esEnergySCEle);
   tree->SetBranchAddress("energySCEle_corr",energySCEle_corr);
   tree->SetBranchAddress("energySCEle_regrCorr_ele",energySCEle_regrCorr_ele);
   tree->SetBranchAddress("energySCEle_regrCorr_pho",energySCEle_regrCorr_pho);
   tree->SetBranchAddress("energyEle_regrCorr_fra",energyEle_regrCorr_fra);
   tree->SetBranchAddress("energySigmaEle_regrCorr_fra",energySigmaEle_regrCorr_fra);
   tree->SetBranchAddress("energyEle_regrCorr_egamma",energyEle_regrCorr_egamma);
   tree->SetBranchAddress("energySigmaEle_regrCorr_egamma",energySigmaEle_regrCorr_egamma);
   tree->SetBranchAddress("energySigmaSCEle_regrCorr_ele",energySigmaSCEle_regrCorr_ele);
   tree->SetBranchAddress("energySigmaSCEle_regrCorr_pho",energySigmaSCEle_regrCorr_pho);
   tree->SetBranchAddress("energySCEle_regrCorrSemiParV4_ele",energySCEle_regrCorrSemiParV4_ele);
   tree->SetBranchAddress("energySCEle_regrCorrSemiParV4_pho",energySCEle_regrCorrSemiParV4_pho);
   tree->SetBranchAddress("energySCEle_regrCorrSemiParV5_ele",energySCEle_regrCorrSemiParV5_ele);
   tree->SetBranchAddress("energySCEle_regrCorrSemiParV5_pho",energySCEle_regrCorrSemiParV5_pho);
   tree->SetBranchAddress("energySigmaSCEle_regrCorrSemiParV4_ele",energySigmaSCEle_regrCorrSemiParV4_ele);
   tree->SetBranchAddress("energySigmaSCEle_regrCorrSemiParV4_pho",energySigmaSCEle_regrCorrSemiParV4_pho);
   tree->SetBranchAddress("energySigmaSCEle_regrCorrSemiParV5_ele",energySigmaSCEle_regrCorrSemiParV5_ele);
   tree->SetBranchAddress("energySigmaSCEle_regrCorrSemiParV5_pho",energySigmaSCEle_regrCorrSemiParV5_pho);
   tree->SetBranchAddress("energySCEle_regrCorrSemiParV6_ele",energySCEle_regrCorrSemiParV6_ele);
   tree->SetBranchAddress("energySCEle_regrCorrSemiParV6_pho",energySCEle_regrCorrSemiParV6_pho);
   tree->SetBranchAddress("energySigmaSCEle_regrCorrSemiParV6_ele",energySigmaSCEle_regrCorrSemiParV6_ele);
   tree->SetBranchAddress("energySigmaSCEle_regrCorrSemiParV6_pho",energySigmaSCEle_regrCorrSemiParV6_pho);
   tree->SetBranchAddress("energySCEle_regrCorrSemiParV7_ele",energySCEle_regrCorrSemiParV7_ele);
   tree->SetBranchAddress("energySCEle_regrCorrSemiParV7_pho",energySCEle_regrCorrSemiParV7_pho);
   tree->SetBranchAddress("energySigmaSCEle_regrCorrSemiParV7_ele",energySigmaSCEle_regrCorrSemiParV7_ele);
   tree->SetBranchAddress("energySigmaSCEle_regrCorrSemiParV7_pho",energySigmaSCEle_regrCorrSemiParV7_pho);
   tree->SetBranchAddress("energySCEle_regrCorrSemiParV8_ele",energySCEle_regrCorrSemiParV8_ele);
   tree->SetBranchAddress("energySCEle_regrCorrSemiParV8_pho",energySCEle_regrCorrSemiParV8_pho);
   tree->SetBranchAddress("energySigmaSCEle_regrCorrSemiParV8_ele",energySigmaSCEle_regrCorrSemiParV8_ele);
   tree->SetBranchAddress("energySigmaSCEle_regrCorrSemiParV8_pho",energySigmaSCEle_regrCorrSemiParV8_pho);
   tree->SetBranchAddress("energySCEle_regrCorrSemiPar7TeVtrainV6_ele",energySCEle_regrCorrSemiPar7TeVtrainV6_ele);
   tree->SetBranchAddress("energySCEle_regrCorrSemiPar7TeVtrainV6_pho",energySCEle_regrCorrSemiPar7TeVtrainV6_pho);
   tree->SetBranchAddress("energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_ele",energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_ele);
   tree->SetBranchAddress("energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_pho",energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_pho);
   tree->SetBranchAddress("energySCEle_regrCorrSemiPar7TeVtrainV7_ele",energySCEle_regrCorrSemiPar7TeVtrainV7_ele);
   tree->SetBranchAddress("energySCEle_regrCorrSemiPar7TeVtrainV7_pho",energySCEle_regrCorrSemiPar7TeVtrainV7_pho);
   tree->SetBranchAddress("energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_ele",energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_ele);
   tree->SetBranchAddress("energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_pho",energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_pho);
   tree->SetBranchAddress("energySCEle_regrCorrSemiPar7TeVtrainV8_ele",energySCEle_regrCorrSemiPar7TeVtrainV8_ele);
   tree->SetBranchAddress("energySCEle_regrCorrSemiPar7TeVtrainV8_pho",energySCEle_regrCorrSemiPar7TeVtrainV8_pho);
   tree->SetBranchAddress("energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_ele",energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_ele);
   tree->SetBranchAddress("energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_pho",energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_pho);
   tree->SetBranchAddress("R9Ele",R9Ele);
   tree->SetBranchAddress("e5x5SCEle",e5x5SCEle);
   tree->SetBranchAddress("pModeGsfEle",pModeGsfEle);
   tree->SetBranchAddress("pAtVtxGsfEle",pAtVtxGsfEle);
   tree->SetBranchAddress("invMass",&invMass);
   tree->SetBranchAddress("invMass_SC",&invMass_SC);
   tree->SetBranchAddress("invMass_e5x5",&invMass_e5x5);
   tree->SetBranchAddress("invMass_rawSC",&invMass_rawSC);
   tree->SetBranchAddress("invMass_rawSC_esSC",&invMass_rawSC_esSC);
   tree->SetBranchAddress("invMass_SC_corr",&invMass_SC_corr);
   tree->SetBranchAddress("invMass_SC_regrCorr_ele",&invMass_SC_regrCorr_ele);
   tree->SetBranchAddress("invMass_SC_regrCorr_pho",&invMass_SC_regrCorr_pho);
   tree->SetBranchAddress("invMass_regrCorr_fra",&invMass_regrCorr_fra);
   tree->SetBranchAddress("invMass_regrCorr_egamma",&invMass_regrCorr_egamma);
   tree->SetBranchAddress("invMass_SC_regrCorrSemiParV4_pho",&invMass_SC_regrCorrSemiParV4_pho);
   tree->SetBranchAddress("invMass_SC_regrCorrSemiParV4_ele",&invMass_SC_regrCorrSemiParV4_ele);
   tree->SetBranchAddress("invMass_SC_regrCorrSemiParV5_pho",&invMass_SC_regrCorrSemiParV5_pho);
   tree->SetBranchAddress("invMass_SC_regrCorrSemiParV5_ele",&invMass_SC_regrCorrSemiParV5_ele);
   tree->SetBranchAddress("invMass_SC_regrCorrSemiParV6_pho",&invMass_SC_regrCorrSemiParV6_pho);
   tree->SetBranchAddress("invMass_SC_regrCorrSemiParV6_ele",&invMass_SC_regrCorrSemiParV6_ele);
   tree->SetBranchAddress("invMass_SC_regrCorrSemiParV7_pho",&invMass_SC_regrCorrSemiParV7_pho);
   tree->SetBranchAddress("invMass_SC_regrCorrSemiParV7_ele",&invMass_SC_regrCorrSemiParV7_ele);
   tree->SetBranchAddress("invMass_SC_regrCorrSemiParV8_pho",&invMass_SC_regrCorrSemiParV8_pho);
   tree->SetBranchAddress("invMass_SC_regrCorrSemiParV8_ele",&invMass_SC_regrCorrSemiParV8_ele);
   tree->SetBranchAddress("invMass_SC_regrCorrSemiPar7TeVtrainV6_pho",&invMass_SC_regrCorrSemiPar7TeVtrainV6_pho);
   tree->SetBranchAddress("invMass_SC_regrCorrSemiPar7TeVtrainV6_ele",&invMass_SC_regrCorrSemiPar7TeVtrainV6_ele);
   tree->SetBranchAddress("invMass_SC_regrCorrSemiPar7TeVtrainV7_pho",&invMass_SC_regrCorrSemiPar7TeVtrainV7_pho);
   tree->SetBranchAddress("invMass_SC_regrCorrSemiPar7TeVtrainV7_ele",&invMass_SC_regrCorrSemiPar7TeVtrainV7_ele);
   tree->SetBranchAddress("invMass_SC_regrCorrSemiPar7TeVtrainV8_pho",&invMass_SC_regrCorrSemiPar7TeVtrainV8_pho);
   tree->SetBranchAddress("invMass_SC_regrCorrSemiPar7TeVtrainV8_ele",&invMass_SC_regrCorrSemiPar7TeVtrainV8_ele);
   tree->SetBranchAddress("invMass_MC",&invMass_MC);
   tree->SetBranchAddress("etaMCEle",etaMCEle);
   tree->SetBranchAddress("phiMCEle",phiMCEle);
   tree->SetBranchAddress("nHitsSCEle",nHitsSCEle);
}

// set tree branches
void SetExtraTreeBranch(TTree* extraCalibTree){

   // Set branch addresses.
   extraCalibTree->SetBranchAddress("runNumber",&runNumber);
   extraCalibTree->SetBranchAddress("eventNumber",&eventNumber);
   extraCalibTree->SetBranchAddress("lumiBlock",&lumiBlock);
   extraCalibTree->SetBranchAddress("runTime",&runTime);
   extraCalibTree->SetBranchAddress("nHitsSCEle",nHitsSCEle);
   extraCalibTree->SetBranchAddress("rawIdRecHitSCEle1",&rawIdRecHitSCEle1);
   extraCalibTree->SetBranchAddress("rawIdRecHitSCEle2",&rawIdRecHitSCEle2);
   extraCalibTree->SetBranchAddress("XRecHitSCEle1",&XRecHitSCEle1);
   extraCalibTree->SetBranchAddress("XRecHitSCEle2",&XRecHitSCEle2);
   extraCalibTree->SetBranchAddress("YRecHitSCEle1",&YRecHitSCEle1);
   extraCalibTree->SetBranchAddress("YRecHitSCEle2",&YRecHitSCEle2);
   extraCalibTree->SetBranchAddress("energyRecHitSCEle1",&energyRecHitSCEle1);
   extraCalibTree->SetBranchAddress("energyRecHitSCEle2",&energyRecHitSCEle2);
   extraCalibTree->SetBranchAddress("LCRecHitSCEle1",&LCRecHitSCEle1);
   extraCalibTree->SetBranchAddress("LCRecHitSCEle2",&LCRecHitSCEle2);
   extraCalibTree->SetBranchAddress("ICRecHitSCEle1",&ICRecHitSCEle1);
   extraCalibTree->SetBranchAddress("ICRecHitSCEle2",&ICRecHitSCEle2);
   extraCalibTree->SetBranchAddress("AlphaRecHitSCEle1",&AlphaRecHitSCEle1);
   extraCalibTree->SetBranchAddress("AlphaRecHitSCEle2",&AlphaRecHitSCEle2);

}





