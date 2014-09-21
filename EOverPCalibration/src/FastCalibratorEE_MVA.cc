#include "../interface/FastCalibratorEE_MVA.h"
#include "../interface/GetHashedIndexEE.h"
#include <TH2.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <TRandom.h>


/// Default constructor 
FastCalibratorEE_MVA::FastCalibratorEE_MVA(TTree *tree,TString outEPDistribution):
outEPDistribution_p(outEPDistribution){
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
  if (tree == 0) {
    TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/data2/rgerosa/Fall11/WZAnalysis_DATA.root");
    if (!f) {
      f = new TFile("/data2/rgerosa/Fall11/WZAnalysis_DATA.root");
    }

    tree = (TTree*)gDirectory->Get("ntu");
  }
  
  // endcap geometry
  eRings = new TEndcapRings(); 
  
  /// Vector for ring normalization IC
  SumIC_Ring_EEP.assign(40,0);
  SumIC_Ring_EEM.assign(40,0);
  Sumxtal_Ring_EEP.assign(40,0);
  Sumxtal_Ring_EEM.assign(40,0);
  
  Init(tree);
}

/// Deconstructor

FastCalibratorEE_MVA::~FastCalibratorEE_MVA(){

  if (!fChain) return;
  delete fChain->GetCurrentFile();
}

/// Get Entry Method
Int_t FastCalibratorEE_MVA::GetEntry(Long64_t entry){

// Read contents of entry.
  if (!fChain) return 0;
  return fChain->GetEntry(entry);

}

/// Load Tree infos in a chain
Long64_t FastCalibratorEE_MVA::LoadTree(Long64_t entry){

// Set the environment to read one entry
  if (!fChain) return -5;
  Long64_t centry = fChain->LoadTree(entry);
  if (centry < 0) return centry;
  if (!fChain->InheritsFrom(TChain::Class()))  return centry;
  TChain *chain = (TChain*)fChain;
  if (chain->GetTreeNumber() != fCurrent) {
    fCurrent = chain->GetTreeNumber();
    Notify();
  }
  return centry;
}

/// Intialize pointers
void FastCalibratorEE_MVA::Init(TTree *tree){

  /// Set object pointer
  ele1_recHit_E = 0;
  ele1_recHit_hashedIndex = 0;
  ele1_recHit_iphiORiy = 0;
  ele1_recHit_ietaORix =0 ;
  ele1_recHit_flag =0 ;

  ele2_recHit_E = 0;
  ele2_recHit_hashedIndex = 0;
  ele2_recHit_iphiORiy = 0;
  ele2_recHit_ietaORix = 0;
  ele2_recHit_flag =0 ;
  
  /// Set branch addresses and branch pointers
  
  if (!tree) return;
  fChain = tree;
  fCurrent = -1;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("runId", &runId, &b_runId);
  fChain->SetBranchAddress("lumiId", &lumiId, &b_lumiId);
  fChain->SetBranchAddress("isW", &isW, &b_isW);
  fChain->SetBranchAddress("isZ", &isZ, &b_isZ);

  fChain->SetBranchAddress("BDTG_weightEE_W_1", &BDTG_weightEE_W_1, &b_BDTG_weightEE_W_1);
  fChain->SetBranchAddress("BDTG_weightEE_W_2", &BDTG_weightEE_W_2, &b_BDTG_weightEE_W_2);
//   fChain->SetBranchAddress("BDTG_weightEE_Z_1", &BDTG_weightEE_Z_1, &b_BDTG_weightEE_Z_1);
//   fChain->SetBranchAddress("BDTG_weightEE_Z_2", &BDTG_weightEE_Z_2, &b_BDTG_weightEE_Z_2);
  

  fChain->SetBranchAddress("ele1_recHit_E", &ele1_recHit_E, &b_ele1_recHit_E);
  fChain->SetBranchAddress("ele1_recHit_hashedIndex", &ele1_recHit_hashedIndex, &b_ele1_recHit_hashedIndex);
  fChain->SetBranchAddress("ele1_recHit_iphiORiy", &ele1_recHit_iphiORiy, &b_ele1_recHit_iphiORiy);
  fChain->SetBranchAddress("ele1_recHit_ietaORix", &ele1_recHit_ietaORix, &b_ele1_recHit_ietaORix);
//   fChain->SetBranchAddress("ele1_recHit_flag", &ele1_recHit_flag, &b_ele1_recHit_flag);
 
  fChain->SetBranchAddress("ele1_scERaw", &ele1_scERaw, &b_ele1_scERaw);
  fChain->SetBranchAddress("ele1_scE", &ele1_scE, &b_ele1_scE);
  fChain->SetBranchAddress("ele1_scEta", &ele1_scEta, &b_ele1_scEta);
  fChain->SetBranchAddress("ele1_es", &ele1_es, &b_ele1_es);
  fChain->SetBranchAddress("ele1_e3x3", &ele1_e3x3, &b_ele1_e3x3);
  fChain->SetBranchAddress("ele1_tkP", &ele1_tkP, &b_ele1_tkP);
  fChain->SetBranchAddress("ele1_fbrem", &ele1_fbrem, &b_ele1_fbrem);
  fChain->SetBranchAddress("ele1_EOverP", &ele1_EOverP, &b_ele1_EOverP);
  fChain->SetBranchAddress("ele1_isEB", &ele1_isEB, &b_ele1_isEB);
  fChain->SetBranchAddress("ele1_isEBEEGap", &ele1_isEBEEGap, &b_ele1_isEBEEGap);
//  fChain->SetBranchAddress("ele1_E_true", &ele1_E_true, &b_ele1_E_true);
//   fChain->SetBranchAddress("ele1_DR ", &ele1_DR , &b_ele1_DR);
  fChain->SetBranchAddress("ele1_scERaw_PUcleaned", &ele1_scERaw_PUcleaned, &b_ele1_scERaw_PUcleaned);
  fChain->SetBranchAddress("ele1_scE_regression", &ele1_scE_regression, &b_ele1_scE_regression);

  fChain->SetBranchAddress("ele1_isEBEtaGap", &ele1_isEBEtaGap, &b_ele1_isEBEtaGap);
  fChain->SetBranchAddress("ele1_isEBPhiGap", &ele1_isEBPhiGap, &b_ele1_isEBPhiGap);
  fChain->SetBranchAddress("ele1_isEEDeeGap", &ele1_isEEDeeGap, &b_ele1_isEEDeeGap);
  fChain->SetBranchAddress("ele1_isEERingGap", &ele1_isEERingGap, &b_ele1_isEERingGap);
  
  fChain->SetBranchAddress("ele2_recHit_E", &ele2_recHit_E, &b_ele2_recHit_E);
  fChain->SetBranchAddress("ele2_recHit_hashedIndex", &ele2_recHit_hashedIndex, &b_ele2_recHit_hashedIndex);
  fChain->SetBranchAddress("ele2_recHit_iphiORiy", &ele2_recHit_iphiORiy, &b_ele2_recHit_iphiORiy);
  fChain->SetBranchAddress("ele2_recHit_ietaORix", &ele2_recHit_ietaORix, &b_ele2_recHit_ietaORix);
//   fChain->SetBranchAddress("ele2_recHit_flag", &ele2_recHit_flag, &b_ele2_recHit_flag);
   
  fChain->SetBranchAddress("ele2_scERaw", &ele2_scERaw, &b_ele2_scERaw);
  fChain->SetBranchAddress("ele2_scE", &ele2_scE, &b_ele2_scE);
  fChain->SetBranchAddress("ele1_scEta", &ele1_scEta, &b_ele1_scEta);
  fChain->SetBranchAddress("ele2_es", &ele2_es, &b_ele2_es);
  fChain->SetBranchAddress("ele2_e3x3", &ele2_e3x3, &b_ele2_e3x3);
  fChain->SetBranchAddress("ele2_tkP", &ele2_tkP, &b_ele2_tkP);
  fChain->SetBranchAddress("ele2_fbrem", &ele2_fbrem, &b_ele2_fbrem);
  fChain->SetBranchAddress("ele2_EOverP", &ele2_EOverP, &b_ele2_EOverP);
  fChain->SetBranchAddress("ele2_isEB", &ele2_isEB, &b_ele2_isEB);
//   fChain->SetBranchAddress("ele2_E_true", &ele2_E_true, &b_ele2_E_true);
//   fChain->SetBranchAddress("ele2_DR ", &ele2_DR , &b_ele2_DR);
  fChain->SetBranchAddress("ele2_scERaw_PUcleaned", &ele2_scERaw_PUcleaned, &b_ele2_scERaw_PUcleaned);
  fChain->SetBranchAddress("ele2_scE_regression", &ele2_scE_regression, &b_ele2_scE_regression);

  fChain->SetBranchAddress("ele2_isEBEEGap", &ele2_isEBEEGap, &b_ele2_isEBEEGap);
  fChain->SetBranchAddress("ele2_isEBEtaGap", &ele2_isEBEtaGap, &b_ele2_isEBEtaGap);
  fChain->SetBranchAddress("ele2_isEBPhiGap", &ele2_isEBPhiGap, &b_ele2_isEBPhiGap);
  fChain->SetBranchAddress("ele2_isEEDeeGap", &ele2_isEEDeeGap, &b_ele2_isEEDeeGap);
  fChain->SetBranchAddress("ele2_isEERingGap", &ele2_isEERingGap, &b_ele2_isEERingGap);
  Notify();
}

Bool_t FastCalibratorEE_MVA::Notify(){
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

  return kTRUE;
}

void FastCalibratorEE_MVA::Show(Long64_t entry){
/// Print contents of entry.
/// If entry is not specified, print current entry
  if (!fChain) return;
  fChain->Show(entry);
}

Int_t FastCalibratorEE_MVA::Cut(Long64_t entry){
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
  return 1;
}

//! Declaration of the objects that are save in the output file

void FastCalibratorEE_MVA::bookHistos(int nLoops)
{

  ///service histos
  h_scale_hashedIndex_EE = new TH1F ("h_scale_hashedIndex_EE","h_scale_hashedIndex_EE", kEEhalf*2, 0, kEEhalf*2 - 1 );
  h_occupancy_hashedIndex_EE = new TH1F ("h_occupancy_hashedIndex_EE","h_occupancy_hashedIndex_EE", kEEhalf*2, 0, kEEhalf*2 - 1 );
  hC_EoP = new hChain ("EoP", "EoP", 500,0.2,3.0, nLoops);
 
  h_WeightEE_W_1 = new TH1F ("h_WeightEE_W_1","h_WeightEE_W_1",500,0.,1.5);

  h_WeightEE_W_2 = new TH1F ("h_WeightEE_W_2","h_WeightEE_W_2",500,0.,1.5);

  ///EE+

  hC_IntercalibValues_EEP = new hChain ("IntercalibValues_EEP", "IntercalibValues_EEP", 400,0.2,1.9, nLoops);
  hC_PullFromScalib_EEP = new hChain ("hC_PullFromScalib_EEP", "hC_PullFromScalib_EEP", 2000,-0.5,0.5, nLoops);
  hC_scale_EEP = new h2Chain("hC_scale_EEP", "hC_scale_EEP", 100,1, 101, 100, 1, 101, nLoops );
  
  h_scale_EEP = new TH2F("h_scale_EEP", "h_scale_EEP", 100,1, 101, 100, 1, 101 );
  h_occupancy_EEP = new TH2F("h_occupancy_EEP", "h_occupancy_EEP", 100,1, 101, 100, 1, 101 );
  h_scalib_EEP = new TH2F("h_scalib_EEP", "h_scalib_EEP", 100,1, 101, 100, 1, 101);
  h_map_Dead_Channels_EEP = new TH2F("h_map_Dead_Channels_EEP","h_map_Dead_Channels_EEP",100,1,101,100,1,101);
  h_scale_meanOnring_EEP = new TH2F ("h_scale_meanOnring_EEP", "h_scale_meanOnring_EEP",  100,1, 101, 100, 1, 101);
  
  g_ICmeanVsLoop_EEP = new TGraphErrors();
  g_ICmeanVsLoop_EEP -> SetName("g_ICmeanVsLoop_EEP");
  g_ICmeanVsLoop_EEP -> SetTitle("g_ICmeanVsLoop_EEP");
  
  g_ICrmsVsLoop_EEP = new TGraphErrors();
  g_ICrmsVsLoop_EEP -> SetName("g_ICrmsVsLoop_EEP");
  g_ICrmsVsLoop_EEP -> SetTitle("g_ICrmsVsLoop_EEP");

 
  
  ///EE-
  hC_IntercalibValues_EEM = new hChain ("IntercalibValues_EEM", "IntercalibValues_EEM", 400,0.2,1.9, nLoops);
  hC_PullFromScalib_EEM = new hChain ("hC_PullFromScalib_EEM", "hC_PullFromScalib_EEM", 2000,-0.5,0.5, nLoops);
  hC_scale_EEM = new h2Chain("hC_scale_EEM", "hC_scale_EEM", 100,1, 101, 100, 1, 101, nLoops );
  
  h_scale_EEM = new TH2F("h_scale_EEM", "h_scale_EEM", 100,1, 101, 100, 1, 101 );
  h_occupancy_EEM = new TH2F("h_occupancy_EEM", "h_occupancy_EEM", 100,1, 101, 100, 1, 101 );
  h_scalib_EEM = new TH2F("h_scalib_EEM", "h_scalib_EEM", 100,1, 101, 100, 1, 101);
  h_map_Dead_Channels_EEM = new TH2F("h_map_Dead_Channels_EEM","h_map_Dead_Channels_EEM",100,1,101,100,1,101);
  h_scale_meanOnring_EEM = new TH2F ("h_scale_meanOnring_EEM", "h_scale_meanOnring_EEM",  100,1, 101, 100, 1, 101);

  g_ICmeanVsLoop_EEM = new TGraphErrors();
  g_ICmeanVsLoop_EEM -> SetName("g_ICmeanVsLoop_EEM");
  g_ICmeanVsLoop_EEM -> SetTitle("g_ICmeanVsLoop_EEM");
  
  g_ICrmsVsLoop_EEM = new TGraphErrors();
  g_ICrmsVsLoop_EEM -> SetName("g_ICrmsVsLoop_EEM");
  g_ICrmsVsLoop_EEM -> SetTitle("g_ICrmsVsLoop_EEM");


  return;
}

/// ===== Build Weight MVA distribution
void FastCalibratorEE_MVA::BuildWeightDistribution_ele(int iLoop, int nentries , int useW, int useZ, std::vector<float> theScalibration, bool isR9selection,bool isfbrem){

if(iLoop ==0){
              
      Long64_t nbytes = 0, nb = 0;

      for (Long64_t jentry=0; jentry<nentries;jentry++) {
   
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
  
        nb = fChain->GetEntry(jentry);   
        nbytes += nb;
        if (!(jentry%1000000))std::cerr<<"building Weight distribution ----> "<<jentry<<" vs "<<nentries<<std::endl;
     
        float FdiEta;
             
        if(ele1_isEB==0 && (( useW == 1 && isW == 1 ) ||  ( useZ== 1 && isZ == 1 ))){

         FdiEta = ele1_scE/(ele1_scERaw+ele1_es); /// Cluster containment approximation using ps infos
         float thisE = 0;
         int   iseed = 0 ;
         float E_seed = 0;
         float thisE3x3 = 0;
         /// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
         for (unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);
          
            if(ele1_recHit_E -> at(iRecHit) > E_seed /*&& ele1_recHit_flag -> at(iRecHit) < 4 */){
              iseed=iRecHit;
              E_seed=ele1_recHit_E -> at(iRecHit); ///Seed infos

            }
    
            if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
            
//             if(ele1_recHit_flag -> at(iRecHit) < 4)
            thisE += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC; /// SC energy
          
        }

        for (unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);

	    if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
         
            if(fabs(ele1_recHit_ietaORix->at(iRecHit)-ele1_recHit_ietaORix->at(iseed))<=1 && 
               fabs(ele1_recHit_iphiORiy->at(iRecHit)-ele1_recHit_iphiORiy->at(iseed))<=1 /*&&
               ele1_recHit_flag -> at(iRecHit) < 4*/)
              thisE3x3+=theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC;
         }

         bool skipElectron = false;
   
         /// R9, fbrem selection before E/p distribution
         if(fabs(thisE3x3/thisE) < 0.80 && isR9selection == true && fabs(ele1_scEta)<=1.75) skipElectron = true;
         if(fabs(thisE3x3/thisE) < 0.88 && isR9selection == true && fabs(ele1_scEta)>1.75 && fabs(ele1_scEta)<=2.0) skipElectron = true;
         if(fabs(thisE3x3/thisE) < 0.92 && isR9selection == true && fabs(ele1_scEta)>2.0 && fabs(ele1_scEta)<=2.15 ) skipElectron = true;
         if(fabs(thisE3x3/thisE) < 0.94 && isR9selection == true && fabs(ele1_scEta)>2.15) skipElectron = true;
         if(fabs(ele1_fbrem)>0.4 && isfbrem==true) skipElectron =true;
         
         if(!skipElectron){ h_WeightEE_W_1->Fill(BDTG_weightEE_W_1);
                            h_WeightEE_W_2->Fill(BDTG_weightEE_W_2);
                           }
     }
   }
  }
 else return;

}


///===== Build E/p for electron 1 and 2

void FastCalibratorEE_MVA::BuildEoPeta_ele(int iLoop, int nentries , int useW, int useZ, std::vector<float> theScalibration,bool       isSaveEPDistribution, bool isR9selection, bool isMCTruth,bool isfbrem){

  if(iLoop ==0)  {
   TString name = Form ("hC_EoP_eta_%d",iLoop);
   hC_EoP_ir_ele = new hChain (name,name, 250,0.1,3.0,41);
  }
  else{
          hC_EoP_ir_ele -> Reset();
          TString name = Form ("hC_EoP_eta_%d",iLoop);
          hC_EoP_ir_ele = new hChain (name,name, 250,0.1,3.0,41);
      }

  Long64_t nbytes = 0, nb = 0;

  /// Loop on ntu entries
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
   
   Long64_t ientry = LoadTree(jentry);
   if (ientry < 0) break;
   nb = fChain->GetEntry(jentry);   
   nbytes += nb;
   if (!(jentry%1000000))std::cerr<<"building E/p distribution ----> "<<jentry<<" vs "<<nentries<<std::endl;

   float pIn,FdiEta;

   ///=== electron tight W or Z only Endcap
   if ( ele1_isEB == 0 && (( useW == 1 && isW == 1 ) ||  ( useZ== 1 && isZ == 1 ))) {

    FdiEta = ele1_scE/(ele1_scERaw+ele1_es); /// Cluster containment approximation using ps infos
   
    float thisE = 0;
    int   iseed = 0 ;
    int seed_hashedIndex = 0;
    float E_seed = 0;
    float thisE3x3 = 0;
    /// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
    for (unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);
          
            if(ele1_recHit_E -> at(iRecHit) > E_seed /*&& ele1_recHit_flag -> at(iRecHit) < 4 */){
              seed_hashedIndex=ele1_recHit_hashedIndex -> at(iRecHit);
              iseed=iRecHit;
              E_seed=ele1_recHit_E -> at(iRecHit); ///Seed infos

            }
    
            if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
            
//             if(ele1_recHit_flag -> at(iRecHit) < 4)
            thisE += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC; /// SC energy
          
    }

    for (unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);

	    if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
         
            if(fabs(ele1_recHit_ietaORix->at(iRecHit)-ele1_recHit_ietaORix->at(iseed))<=1 && 
               fabs(ele1_recHit_iphiORiy->at(iRecHit)-ele1_recHit_iphiORiy->at(iseed))<=1 /*&&
               ele1_recHit_flag -> at(iRecHit) < 4*/)
              thisE3x3+=theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC;
    }

          
    int ix_seed = GetIxFromHashedIndex(seed_hashedIndex);
    int iy_seed = GetIyFromHashedIndex(seed_hashedIndex);
    int iz_seed = GetZsideFromHashedIndex(seed_hashedIndex);
    int ir_seed = eRings -> GetEndcapRing(ix_seed,iy_seed,iz_seed); /// Seed ring 
  
    bool skipElectron = false;
    
    /// Option for MCTruth analysis
    if(!isMCTruth) pIn = ele1_tkP;
    else{ pIn = ele1_E_true;
           if(fabs(ele1_DR)>0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
        }

    /// R9, fbrem selection before E/p distribution
    if(fabs(thisE3x3/thisE) < 0.80 && isR9selection == true && fabs(ele1_scEta)<=1.75) skipElectron = true;
    if(fabs(thisE3x3/thisE) < 0.88 && isR9selection == true && fabs(ele1_scEta)>1.75 && fabs(ele1_scEta)<=2.0) skipElectron = true;
    if(fabs(thisE3x3/thisE) < 0.92 && isR9selection == true && fabs(ele1_scEta)>2.0 && fabs(ele1_scEta)<=2.15 ) skipElectron = true;
    if(fabs(thisE3x3/thisE) < 0.94 && isR9selection == true && fabs(ele1_scEta)>2.15) skipElectron = true;

    if(fabs(ele1_fbrem)>0.4 && isfbrem==true) skipElectron =true;
    if(!skipElectron)    hC_EoP_ir_ele -> Fill(ir_seed,thisE/(pIn-ele1_es));
     
  
  }
  ///=== Second medium electron from Z only Endcaps
  if ( ele2_isEB == 0 && (( useW == 1 && isW == 1 ) || ( useZ == 1 && isZ == 1 )) ){

    FdiEta = ele2_scE/(ele2_scERaw+ele2_es);

    float thisE = 0;
    int   iseed = 0 ;
    int seed_hashedIndex = 0;
    float E_seed = 0;
    float thisE3x3 = 0;
  
    /// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
    for (unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);

            if(ele2_recHit_E -> at(iRecHit) > E_seed /*&& ele2_recHit_flag -> at(iRecHit) < 4*/){
              seed_hashedIndex=ele2_recHit_hashedIndex -> at(iRecHit);
              iseed=iRecHit;
              E_seed=ele2_recHit_E -> at(iRecHit);

            }
    
            if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
            
            //if(ele2_recHit_flag -> at(iRecHit) < 4) /// Only Good channels
            thisE += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC;
             
    }

    for (unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);
            // IC obtained from previous Loops
            if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
         
            if(fabs(ele2_recHit_ietaORix->at(iRecHit)-ele2_recHit_ietaORix->at(iseed))<=1 && 
               fabs(ele2_recHit_iphiORiy->at(iRecHit)-ele2_recHit_iphiORiy->at(iseed))<=1 /*&&
               ele2_recHit_flag -> at(iRecHit) < 4*/)
              thisE3x3+=theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC;
    }

  
    int ix_seed = GetIxFromHashedIndex(seed_hashedIndex);
    int iy_seed = GetIyFromHashedIndex(seed_hashedIndex);
    int iz_seed = GetZsideFromHashedIndex(seed_hashedIndex);
    int ir_seed = eRings -> GetEndcapRing(ix_seed,iy_seed,iz_seed); /// Seed ring
 
     
    bool skipElectron = false;
    /// Option for MCTruth Analysis
    if(!isMCTruth) pIn = ele2_tkP;
    else{ pIn = ele2_E_true;
           if(fabs(ele2_DR)>0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
         }

    /// R9 and fbrem selection
    if(fabs(thisE3x3/thisE) < 0.80 && isR9selection == true && fabs(ele2_scEta)<=1.75) skipElectron = true;
    if(fabs(thisE3x3/thisE) < 0.88 && isR9selection == true && fabs(ele2_scEta)>1.75 && fabs(ele2_scEta)<=2.0) skipElectron = true;
    if(fabs(thisE3x3/thisE) < 0.92 && isR9selection == true && fabs(ele2_scEta)>2.0 && fabs(ele2_scEta)<=2.15 ) skipElectron = true;
    if(fabs(thisE3x3/thisE) < 0.94 && isR9selection == true && fabs(ele2_scEta)>2.15) skipElectron = true;
 
    if ( fabs(ele2_fbrem)>0.4 && isfbrem==true) skipElectron =true;
    if(!skipElectron) hC_EoP_ir_ele -> Fill(ir_seed,thisE/(pIn-ele2_es));
  
  }
 }
 
 /// Normalization E/p distribution
 for(unsigned int ir=0 ; ir < hC_EoP_ir_ele->Size() ; ir++) hC_EoP_ir_ele->Normalize(ir);
 
 
 /// Save E/p distributions
 if(isSaveEPDistribution == true && outEPDistribution_p!="NULL" ) {
   TFile *f2 = new TFile(outEPDistribution_p.Data(),"UPDATE");
   saveEoPeta(f2);
 }

}

/// L3 Loop method ----> Calibration Loop function
void FastCalibratorEE_MVA::Loop(int nentries, int useZ, int useW, int splitStat, int nLoops, bool isMiscalib,bool isSaveEPDistribution,
                            bool isEPselection,bool isR9selection,bool isMCTruth,bool isfbrem){
 
   if (fChain == 0) return;
   
   /// Define the number of crystal you want to calibrate
   int m_regions = kEEhalf;
   
   std::cout << "m_regions " << m_regions << std::endl;
  
   /// build up scalibration map
   std::vector<float> theScalibration(m_regions*2, 0.);
   TRandom genRand;
   for ( int iIndex = 0; iIndex < m_regions*2; iIndex++ ){
   
     bool isDeadXtal = false ;
     /// Check if the xtal has to be considered dead or not ---> >Fake dead list given by user
     if(DeadXtal_HashedIndex.at(0)!=-9999) isDeadXtal = CheckDeadXtal(GetIxFromHashedIndex(iIndex),GetIyFromHashedIndex(iIndex),GetZsideFromHashedIndex(iIndex));
     if(isDeadXtal == true ) {
     theScalibration[iIndex]=0;
    
     if(GetZsideFromHashedIndex(iIndex)>0)
     h_map_Dead_Channels_EEP->Fill(GetIxFromHashedIndex(iIndex),GetIyFromHashedIndex(iIndex));
     else h_map_Dead_Channels_EEM->Fill(GetIxFromHashedIndex(iIndex),GetIyFromHashedIndex(iIndex));
     }
     else{
     
         if(isMiscalib==true) theScalibration[iIndex] = genRand.Gaus(1.,0.05); /// Miscalibration fixed at 5%
         if(isMiscalib == false) theScalibration[iIndex] = 1.;
         if(GetZsideFromHashedIndex(iIndex)>0)
         h_scalib_EEP -> Fill ( GetIxFromHashedIndex(iIndex), GetIyFromHashedIndex(iIndex), theScalibration[iIndex] ); /// scalibration map for EE+ and EE-
         else  h_scalib_EEM-> Fill ( GetIxFromHashedIndex(iIndex), GetIyFromHashedIndex(iIndex), theScalibration[iIndex] );

     }
   }
  
  
   /// ----------------- Calibration Loops -----------------------------//
   for ( int iLoop = 0; iLoop < nLoops; iLoop++ ) {
 
    std::cout << "Starting iteration " << iLoop + 1 << std::endl;
    /// L3 numerator and denominator for EE+ and EE-
    std::vector<float> theNumerator_EEP(m_regions*2+1, 0.);
    std::vector<float> theDenominator_EEP(m_regions*2+1, 0.);
    std::vector<float> theNumerator_EEM(m_regions+1, 0.);
    std::vector<float> theDenominator_EEM(m_regions+1, 0.);


    std::cout << "Number of analyzed events = " << nentries << std::endl;

   /// === Build Weight Distribution 

    BuildWeightDistribution_ele(iLoop,nentries,useW,useZ,theScalibration,isR9selection,isfbrem); 
    
    ///==== build E/p distribution ele 1 and 2
    
    BuildEoPeta_ele(iLoop,nentries,useW,useZ,theScalibration,isSaveEPDistribution,isR9selection,isMCTruth,isfbrem); 
 
    
    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
      
        if (!(jentry%100000))std::cerr<<jentry;
        if (!(jentry%10000)) std::cerr<<".";
      
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   
        nbytes += nb;
              
        float pIn, pSub, FdiEta;
      
        std::map<int,double> map;
        bool skipElectron=false;
	
        /// Only tight electron from W and Z, only Endcap
        
	if ( ele1_isEB == 0 && (( useW == 1 && isW == 1 ) || ( useZ == 1 && isZ == 1 )) ) {
                  
          /// SCL energy containment correction
          FdiEta = ele1_scE/(ele1_scERaw+ele1_es);
         
	  float thisE = 0;
          float thisE3x3 =0 ;
          int iseed = 0 ;
          int seed_hashedIndex = 0 ;
          float E_seed = 0;

          /// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
          for (unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);
  
            if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
            
           // if(ele1_recHit_flag -> at(iRecHit) < 4) /// Skip bad channel 
            thisE += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC;
     
            if(ele1_recHit_E -> at(iRecHit) > E_seed /*&& ele1_recHit_flag -> at(iRecHit) < 4*/ ){
              E_seed=ele1_recHit_E -> at(iRecHit);
              iseed=iRecHit;
              seed_hashedIndex=ele1_recHit_hashedIndex -> at(iRecHit); /// Seed infos
 
             }
          
          }
          
          for (unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);
    
            if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
            
            if(fabs(ele1_recHit_ietaORix->at(iRecHit)-ele1_recHit_ietaORix->at(iseed))<=1 && 
               fabs(ele1_recHit_iphiORiy->at(iRecHit)-ele1_recHit_iphiORiy->at(iseed))<=1 /*&&
                ele1_recHit_flag -> at(iRecHit) < 4*/)
              thisE3x3+=theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC;
                 
          }
            
         pSub = 0.; //NOTALEO : test dummy
      
         /// MCTruth option 
         if(!isMCTruth) pIn = ele1_tkP;
         else{
           pIn = ele1_E_true;
           if(fabs(ele1_DR)>0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
         }
        
       /// find the zside
       int thisCaliBlock = -1;
       if (GetZsideFromHashedIndex(ele1_recHit_hashedIndex -> at(iseed)) < 0) thisCaliBlock = 0;
       else thisCaliBlock = 1;
 
       int ix_seed = GetIxFromHashedIndex(seed_hashedIndex);
       int iy_seed = GetIyFromHashedIndex(seed_hashedIndex);
       int iz_seed = GetZsideFromHashedIndex(seed_hashedIndex);
       int ir_seed = eRings -> GetEndcapRing(ix_seed,iy_seed,iz_seed);
      
       TH1F* EoPHisto = hC_EoP_ir_ele->GetHisto(ir_seed);
       
       if ( fabs(thisE/(ele1_tkP-ele1_es) - 1) > 0.7 && isEPselection==true) skipElectron = true; /// Take the correct E/p pdf to weight events in the calib procedure
       
       /// R9 and fbrem selection
       if(fabs(thisE3x3/thisE) < 0.80 && isR9selection == true && fabs(ele1_scEta)<=1.75) skipElectron = true;
       if(fabs(thisE3x3/thisE) < 0.88 && isR9selection == true && fabs(ele1_scEta)>1.75 && fabs(ele1_scEta)<=2.0) skipElectron = true;
       if(fabs(thisE3x3/thisE) < 0.92 && isR9selection == true && fabs(ele1_scEta)>2.0 && fabs(ele1_scEta)<=2.15 ) skipElectron = true;
       if(fabs(thisE3x3/thisE) < 0.94 && isR9selection == true && fabs(ele1_scEta)>2.15) skipElectron = true;
 
       if ( fabs(ele1_fbrem)>0.4 && isfbrem==true) skipElectron =true;
       if ( thisE/(pIn-ele1_es) < EoPHisto->GetXaxis()->GetXmin() || thisE/(pIn-ele1_es) > EoPHisto->GetXaxis()->GetXmax()) skipElectron=true;
 
       if(!skipElectron && BDTG_weightEE_W_1!=-99. && BDTG_weightEE_W_2!=-99.){

        double Maximum_h_WeightEE_W_1 = h_WeightEE_W_1->GetBinCenter(h_WeightEE_W_1->GetMaximumBin());
        double Maximum_h_WeightEE_W_2 = h_WeightEE_W_2->GetBinCenter(h_WeightEE_W_2->GetMaximumBin());
                  
        for ( unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {
           
//            if(ele1_recHit_flag -> at(iRecHit) >= 4) continue;
         
         int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);
         float thisIC = 1.;
         
         if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
  
           /// Fill the occupancy map JUST for the first Loop
         if ( iLoop == 0 ) { h_occupancy_hashedIndex_EE -> Fill(thisIndex);
                               if ( GetZsideFromHashedIndex(thisIndex) < 0 )
                                   h_occupancy_EEM -> Fill(GetIxFromHashedIndex(thisIndex), GetIyFromHashedIndex(thisIndex) );
                               else h_occupancy_EEP -> Fill(GetIxFromHashedIndex(thisIndex), GetIyFromHashedIndex(thisIndex) );
                           }

         ///Use full statistic
         if ( splitStat == 0) {
             
          if(thisCaliBlock == 0) {
                
             int EoPbin = EoPHisto->FindBin(thisE/(pIn-pSub-ele1_es));
             theNumerator_EEM[thisIndex] += theScalibration[thisIndex]*ele1_recHit_E ->
             at(iRecHit)*FdiEta*thisIC/thisE*(pIn-pSub-ele1_es)/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));


             theDenominator_EEM[thisIndex] += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));
                
          }
                
          if(thisCaliBlock == 1) {
             
             int EoPbin = EoPHisto->FindBin(thisE/(pIn-pSub-ele1_es));

             theNumerator_EEP[thisIndex] += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*(pIn-pSub-ele1_es)/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));

             theDenominator_EEP[thisIndex] += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));
          }
             
        }
             
        /// use evens    
        if ( splitStat == 1 && jentry%2 == 0 ) {
                  
          if(thisCaliBlock == 0) {
                
             int EoPbin = EoPHisto->FindBin(thisE/(pIn-pSub-ele1_es));
             theNumerator_EEM[thisIndex] += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*(pIn-pSub-ele1_es)/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));
                
             theDenominator_EEM[thisIndex] += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));  }
                
          if(thisCaliBlock == 1) {
                
             int EoPbin = EoPHisto->FindBin(thisE/(pIn-pSub-ele1_es));
                
             theNumerator_EEP[thisIndex] += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*(pIn-pSub-ele1_es)/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));
                
             theDenominator_EEP[thisIndex] += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));
          }
        }
             
        /// use odd    
        if ( splitStat == -1 && jentry%2 != 0 ) {
                  
          if(thisCaliBlock == 0) {
            
             int EoPbin = EoPHisto->FindBin(thisE/(pIn-pSub-ele1_es));
             
             theNumerator_EEM[thisIndex] += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*(pIn-pSub-ele1_es)/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));
             
             theDenominator_EEM[thisIndex] += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));}
                
          if(thisCaliBlock == 1) {
           
             int EoPbin = EoPHisto->FindBin(thisE/(pIn-pSub-ele1_es));
               
             theNumerator_EEP[thisIndex] += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*(pIn-pSub-ele1_es)/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));
                
             theDenominator_EEP[thisIndex] += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));   }
        }
       }
      }
      ///Fill EoP
      if (thisCaliBlock != -1) hC_EoP -> Fill(iLoop, thisE/(pIn-ele1_es));
        
     }  
     skipElectron = false;     
      
     /// Medium ele from Z only Endcap
     if ( ele2_isEB == 0 && ( useZ == 1 && isZ == 1 ) ) {
          
       /// SCL energy containment correction
       FdiEta = ele2_scE/(ele2_scERaw+ele2_es);
         
       float thisE = 0;
       float thisE3x3 =0 ;
       int iseed = 0 ;
       int seed_hashedIndex = 0;
       float E_seed = 0;

       /// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
       for (unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {
            
         float thisIC = 1.;
         int thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);
           
	 if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
            
//             if( ele2_recHit_flag -> at(iRecHit) < 4 )
         thisE += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC;

         if(ele2_recHit_E -> at(iRecHit) > E_seed /*&& ele2_recHit_flag -> at(iRecHit) < 4*/){

           E_seed=ele2_recHit_E -> at(iRecHit);
           iseed=iRecHit;
           seed_hashedIndex=ele2_recHit_hashedIndex -> at(iRecHit);
         }
       }
          
       for (unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {
            
        float thisIC = 1.;
        int thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);
            
        if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
            
        if(fabs(ele2_recHit_ietaORix->at(iRecHit)-ele2_recHit_ietaORix->at(iseed))<=1 && 
           fabs(ele2_recHit_iphiORiy->at(iRecHit)-ele2_recHit_iphiORiy->at(iseed))<=1 /*&&
            ele2_recHit_flag -> at(iRecHit) < 4*/)
           thisE3x3+=theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC;
                 
       }
            
       pSub = 0.; //NOTALEO : test dummy
       /// Option for MCTruth Analysis
       if(!isMCTruth) pIn = ele2_tkP;
       else{
            pIn = ele2_E_true;
            if(fabs(ele2_DR)>0.1) skipElectron = true ; /// No macthing beetween gen ele and reco ele
           }

       /// find the zside
       int thisCaliBlock = -1;
       if (GetZsideFromHashedIndex(ele2_recHit_hashedIndex -> at(iseed)) < 0) thisCaliBlock = 0;
       else thisCaliBlock = 1;
 
       int ix_seed = GetIxFromHashedIndex(seed_hashedIndex);
       int iy_seed = GetIyFromHashedIndex(seed_hashedIndex);
       int iz_seed = GetZsideFromHashedIndex(seed_hashedIndex);
       int ir_seed = eRings -> GetEndcapRing(ix_seed,iy_seed,iz_seed);
 
       TH1F* EoPHisto = hC_EoP_ir_ele->GetHisto(ir_seed); /// Use correct pdf for reweight events in the L3 procedure
       /// E/p and R9 selections
       if ( fabs(thisE/(pIn-ele2_es) - 1) > 0.7 && isEPselection==true) skipElectron = true;
       /// R9 and fbrem selection
       if(fabs(thisE3x3/thisE) < 0.80 && isR9selection == true && fabs(ele2_scEta)<=1.75) skipElectron = true;
       if(fabs(thisE3x3/thisE) < 0.88 && isR9selection == true && fabs(ele2_scEta)>1.75 && fabs(ele2_scEta)<=2.0) skipElectron = true;
       if(fabs(thisE3x3/thisE) < 0.92 && isR9selection == true && fabs(ele2_scEta)>2.0 && fabs(ele2_scEta)<=2.15 ) skipElectron = true;
       if(fabs(thisE3x3/thisE) < 0.94 && isR9selection == true && fabs(ele2_scEta)>2.15) skipElectron = true;
         
       if ( fabs(ele2_fbrem)>0.4 && isfbrem==true) skipElectron =true;

       if ( thisE/(pIn-ele2_es) < EoPHisto->GetXaxis()->GetXmin() || thisE/(pIn-ele2_es) > EoPHisto->GetXaxis()->GetXmax()) skipElectron=true;
 
       if(!skipElectron && BDTG_weightEE_W_1!=-99. && BDTG_weightEE_W_2!=-99.) {
          
        double Maximum_h_WeightEE_W_1 = h_WeightEE_W_1->GetBinCenter(h_WeightEE_W_1->GetMaximumBin());
        double Maximum_h_WeightEE_W_2 = h_WeightEE_W_2->GetBinCenter(h_WeightEE_W_2->GetMaximumBin());
              
           
        for ( unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {
           
//            if(ele2_recHit_flag -> at(iRecHit) >= 4) continue;
         
         int thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);
         float thisIC = 1.;
              
         if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
  
         /// Fill the occupancy map JUST for the first Loop
         if ( iLoop == 0 ) {
                             h_occupancy_hashedIndex_EE -> Fill(thisIndex);
                             if ( GetZsideFromHashedIndex(thisIndex) < 0 ) 
                             h_occupancy_EEM -> Fill(GetIxFromHashedIndex(thisIndex), GetIyFromHashedIndex(thisIndex) );
                             else h_occupancy_EEP -> Fill(GetIxFromHashedIndex(thisIndex), GetIyFromHashedIndex(thisIndex) );
         }

         /// Use full statistic
         if ( splitStat == 0) {
             
            if(thisCaliBlock == 0) {
                
              int EoPbin = EoPHisto->FindBin(thisE/(pIn-pSub-ele2_es));
                
              theNumerator_EEM[thisIndex] += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*(pIn-pSub-ele2_es)/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));
              
              theDenominator_EEM[thisIndex] += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));
                
            }
                
            if(thisCaliBlock == 1) {
               
             int EoPbin = EoPHisto->FindBin(thisE/(pIn-pSub-ele2_es));
               
             theNumerator_EEP[thisIndex] += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*(pIn-pSub-ele2_es)/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));
            
             theDenominator_EEP[thisIndex] += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));
            }    
         }
             
        /// use evens    
        if ( splitStat == 1 && jentry%2 == 0 ) {
                  
            if(thisCaliBlock == 0) {
        
              int EoPbin = EoPHisto->FindBin(thisE/(pIn-pSub-ele2_es));
              
              theNumerator_EEM[thisIndex] += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*(pIn-pSub-ele2_es)/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));
            
              theDenominator_EEM[thisIndex] += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));
            }
                
            if(thisCaliBlock == 1) {
              
             int EoPbin = EoPHisto->FindBin(thisE/(pIn-pSub-ele2_es));
           
             theNumerator_EEP[thisIndex] += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*(pIn-pSub-ele2_es)/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));
        
             theDenominator_EEP[thisIndex] += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));
            }
        }
             
        /// use odd    
        if ( splitStat == -1 && jentry%2 != 0 ) {
                  
             if(thisCaliBlock == 0) {
        
                  int EoPbin = EoPHisto->FindBin(thisE/(pIn-pSub-ele2_es));
          
                  theNumerator_EEM[thisIndex] += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*(pIn-pSub-ele2_es)/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));
            
                  theDenominator_EEM[thisIndex] += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*EoPHisto->GetBinContent(EoPbin)*(thisE3x3/thisE)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));
             }
                
             if(thisCaliBlock == 1) {
                int EoPbin = EoPHisto->FindBin(thisE/(pIn-pSub-ele2_es));

                theNumerator_EEP[thisIndex] += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*(pIn-pSub-ele1_es)/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));

                theDenominator_EEP[thisIndex] += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*EoPHisto->GetBinContent(EoPbin)*exp(-(fabs(BDTG_weightEE_W_1-Maximum_h_WeightEE_W_1)+fabs(BDTG_weightEE_W_2-Maximum_h_WeightEE_W_2)));}
          }
        }
      }
      //Fill EoP
      if (thisCaliBlock != -1) hC_EoP -> Fill(iLoop, thisE/(pIn-ele2_es));
          
     }
   }  
    ///  End Cycle on the events
       
    std::cout << ">>>>> [L3][endOfLoop] entering..." << std::endl;
  
    TH1F auxiliary_IC_EEM("auxiliary_IC_EEM","auxiliary_IC_EEM",50,0.2,1.9);
    TH1F auxiliary_IC_EEP("auxiliary_IC_EEP","auxiliary_IC_EEP",50,0.2,1.9);

    ///Fill the histo of IntercalibValues before the solve
    for ( int iIndex = 0; iIndex < kEEhalf*2; iIndex++ ){

      if ( h_occupancy_hashedIndex_EE -> GetBinContent(iIndex+1) > 0 ){
   
        int thisCaliBlock = -1;
        if (GetZsideFromHashedIndex(iIndex) < 0) thisCaliBlock = 0;
        else thisCaliBlock = 1;
        
        float thisIntercalibConstant =1. ;

        if(thisCaliBlock == 0 && theDenominator_EEM[iIndex] != 0.){ 
            thisIntercalibConstant = theNumerator_EEM[iIndex]/theDenominator_EEM[iIndex];}
        else{
             if(thisCaliBlock == 1 && theDenominator_EEP[iIndex] != 0.)
             thisIntercalibConstant = theNumerator_EEP[iIndex]/theDenominator_EEP[iIndex];}

        float oldIntercalibConstant = 1.;
        if ( iLoop > 0 ) oldIntercalibConstant = h_scale_hashedIndex_EE -> GetBinContent (iIndex+1);
        h_scale_hashedIndex_EE -> SetBinContent(iIndex+1, thisIntercalibConstant*oldIntercalibConstant);
              
        if ( thisCaliBlock == 0 ) {
          hC_IntercalibValues_EEM -> Fill (iLoop, thisIntercalibConstant);
          hC_PullFromScalib_EEM -> Fill(iLoop,(thisIntercalibConstant*oldIntercalibConstant-1./theScalibration[iIndex]));
          hC_scale_EEM -> Fill(iLoop, GetIxFromHashedIndex(iIndex), GetIyFromHashedIndex(iIndex),thisIntercalibConstant*oldIntercalibConstant);
     
          auxiliary_IC_EEM.Fill(thisIntercalibConstant);
          
        }
        else {
                 if( thisCaliBlock == 1)
                 {
                  hC_IntercalibValues_EEP -> Fill (iLoop, thisIntercalibConstant);
                  hC_PullFromScalib_EEP -> Fill(iLoop,(thisIntercalibConstant*oldIntercalibConstant-1./theScalibration[iIndex]));
                  hC_scale_EEP -> Fill(iLoop, GetIxFromHashedIndex(iIndex), GetIyFromHashedIndex(iIndex),thisIntercalibConstant*oldIntercalibConstant);
  
                  auxiliary_IC_EEP.Fill(thisIntercalibConstant);}
         }


        }
      
      }
   
    g_ICmeanVsLoop_EEM -> SetPoint(iLoop, iLoop, auxiliary_IC_EEM.GetMean());
    g_ICmeanVsLoop_EEM -> SetPointError(iLoop, 0.,auxiliary_IC_EEM.GetMeanError());
    
    g_ICrmsVsLoop_EEM -> SetPoint(iLoop, iLoop, auxiliary_IC_EEM . GetRMS());
    g_ICrmsVsLoop_EEM -> SetPointError(iLoop, 0., auxiliary_IC_EEM . GetRMSError());

    g_ICmeanVsLoop_EEP -> SetPoint(iLoop, iLoop, auxiliary_IC_EEP . GetMean());
    g_ICmeanVsLoop_EEP -> SetPointError(iLoop, 0., auxiliary_IC_EEP . GetMeanError());
    
    g_ICrmsVsLoop_EEP -> SetPoint(iLoop, iLoop, auxiliary_IC_EEP . GetRMS());
    g_ICrmsVsLoop_EEP -> SetPointError(iLoop, 0., auxiliary_IC_EEP . GetRMSError());
    
   }
   /// End of Calibration Loops
   
   ///Fill the histo of IntercalibValues after the loops at last step
   for ( int iIndex = 0; iIndex < kEEhalf*2; iIndex++ ){
           
     if ( h_occupancy_hashedIndex_EE -> GetBinContent(iIndex+1) > 0 ){
        
       int thisCaliBlock = -1;
       if (GetZsideFromHashedIndex(iIndex) < 0) thisCaliBlock = 0;
       else thisCaliBlock = 1;
       
       int thisIx = GetIxFromHashedIndex(iIndex);
       int thisIy = GetIyFromHashedIndex(iIndex);
       int thisIz = GetZsideFromHashedIndex(iIndex); /// Ix, Iy and Iz info for each xtal

       float thisIntercalibConstant = h_scale_hashedIndex_EE -> GetBinContent (iIndex+1); /// Final IC value
       if ( thisCaliBlock == 0 ) 
         h_scale_EEM -> Fill (thisIx, thisIy, thisIntercalibConstant);
       else
         h_scale_EEP -> Fill (thisIx, thisIy, thisIntercalibConstant);

       if ( thisCaliBlock == 0 )
       {
         ///Vectors for IC Normalization
         IxValues_EEM.push_back(thisIx);
         IyValues_EEM.push_back(thisIy);
         ICValues_EEM.push_back(thisIntercalibConstant);
       }
       else{
             IxValues_EEP.push_back(thisIx);
             IyValues_EEP.push_back(thisIy);
             ICValues_EEP.push_back(thisIntercalibConstant);
           }

       int thisIr = eRings -> GetEndcapRing(thisIx,thisIy,thisIz); /// Endcap ring  xtal belongs to
       if(thisIz >0)
       {
        SumIC_Ring_EEP.at(thisIr) = SumIC_Ring_EEP.at(thisIr) + thisIntercalibConstant;
        Sumxtal_Ring_EEP.at(thisIr) = Sumxtal_Ring_EEP.at(thisIr) + 1;
       }
       else{
              SumIC_Ring_EEM.at(thisIr) = SumIC_Ring_EEM.at(thisIr) + thisIntercalibConstant;
              Sumxtal_Ring_EEM.at(thisIr) = Sumxtal_Ring_EEM.at(thisIr) + 1;
           }
               
       }

      
   }
   /// IC Normaliztion trough the mean value of each ring
   for ( int iIndex = 0; iIndex < kEEhalf*2; iIndex++ ){
   
    if ( h_occupancy_hashedIndex_EE -> GetBinContent(iIndex+1) > 0 ){
        
      //       int thisCaliBlock = -1;
      //       if (GetZsideFromHashedIndex(iIndex) < 0) thisCaliBlock = 0;
      //       else thisCaliBlock = 1;
       
       int thisIx = GetIxFromHashedIndex(iIndex);
       int thisIy = GetIyFromHashedIndex(iIndex);
       int thisIz = GetZsideFromHashedIndex(iIndex);

       int thisIr = eRings -> GetEndcapRing(thisIx,thisIy,thisIz);

       float thisIntercalibConstant = h_scale_hashedIndex_EE -> GetBinContent (iIndex+1);
     
       
       if(thisIz > 0)
       {
          if(Sumxtal_Ring_EEP.at(thisIr) != 0 && SumIC_Ring_EEP.at(thisIr)!= 0)
          h_scale_meanOnring_EEP->Fill(thisIx,thisIy,thisIntercalibConstant/(SumIC_Ring_EEP.at(thisIr)/Sumxtal_Ring_EEP.at(thisIr)));
       }
       else{
            if(Sumxtal_Ring_EEM.at(thisIr) != 0 && SumIC_Ring_EEM.at(thisIr) != 0)
            h_scale_meanOnring_EEM->Fill(thisIx,thisIy,thisIntercalibConstant/(SumIC_Ring_EEM.at(thisIr)/Sumxtal_Ring_EEM.at(thisIr)));
           }
       }
   }
    
}

/// Save in the output  
void FastCalibratorEE_MVA::saveHistos(TFile * f1){

  f1->cd();
  
  /// EE+
  hC_IntercalibValues_EEP-> Write(*f1);
  hC_PullFromScalib_EEP->Write(*f1);
  hC_EoP->Write(*f1);
  hC_scale_EEP->Write("",*f1);
  
  h_occupancy_EEP->Write();
  h_scale_EEP->Write();
  h_scalib_EEP->Write();
  
  h_scale_hashedIndex_EE->Write();
  h_occupancy_hashedIndex_EE->Write();
  h_map_Dead_Channels_EEP->Write();
    
  g_ICmeanVsLoop_EEP->Write();
  g_ICrmsVsLoop_EEP->Write();
  h_scale_meanOnring_EEP->Write("h_scale_map_EEP");
  h_map_Dead_Channels_EEP->Write();
    
  
  /// EE-
  hC_IntercalibValues_EEM-> Write(*f1);
  hC_scale_EEM->Write("",*f1);
  hC_PullFromScalib_EEM->Write(*f1);
  h_occupancy_EEM->Write();
  h_scale_EEM->Write();
  h_scalib_EEM->Write();
 
  g_ICmeanVsLoop_EEM->Write();
  g_ICrmsVsLoop_EEM->Write();
  h_scale_meanOnring_EEM->Write("h_scale_map_EEM");
  h_map_Dead_Channels_EEM->Write();
  

  f1->Close();

  return;
}

/// Save E/p distributions
void FastCalibratorEE_MVA::saveEoPeta(TFile * f2){
 f2->cd();
 hC_EoP_ir_ele ->Write(*f2);
 f2->Close(); 
}

/// Acquire fake Dead Xtal in order to study the effect of IC near them
void FastCalibratorEE_MVA::AcquireDeadXtal(TString inputDeadXtal){
  if(inputDeadXtal!="NULL")
  {
   std::ifstream DeadXtal (inputDeadXtal.Data(),std::ios::binary);
   
   std::string buffer;
   int iX, iY ,iZ;
  
   while(!DeadXtal.eof())
   {
    getline(DeadXtal,buffer);
    std::stringstream line( buffer );
    line >> iX >> iY >>iZ ;
    DeadXtal_HashedIndex.push_back(GetHashedIndexEE(iX,iY,iZ)) ;
   
   }

  sort(DeadXtal_HashedIndex.begin(), DeadXtal_HashedIndex.end());
  }
  else{
       DeadXtal_HashedIndex.push_back(-9999);
      }

}
/// Check if the channel considered is in the list of dead or not
bool FastCalibratorEE_MVA::CheckDeadXtal(const int & iX, const int & iY, const int & iZ){

  int hashed_Index;
  hashed_Index = GetHashedIndexEE(iX,iY,iZ);
  
  std::vector<int>::iterator iter = find(DeadXtal_HashedIndex.begin(),DeadXtal_HashedIndex.end(),hashed_Index);

  if(iter !=DeadXtal_HashedIndex.end())
     return true;
  else return false;
}
   
