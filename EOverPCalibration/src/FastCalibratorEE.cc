#include "../interface/FastCalibratorEE.h"
#include "../interface/GetHashedIndexEE.h"
#include <fstream>
#include <TRandom3.h>
#include <TString.h>
#include "../interface/CalibrationUtils.h"
#include "../interface/readJSONFile.h"


/// Default constructor 
FastCalibratorEE::FastCalibratorEE(TTree *tree, std::vector<TGraphErrors*> & inputMomentumScale, const std::string& typeEE, TString outEPDistribution):
outEPDistribution_p(outEPDistribution){

// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
  if (tree == 0) {
    TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/data1/dimatteo/Calibration/Ntuples/Run2011A/WZAnalysisSingleXtal/WZAnalysis_SingleElectron_Run2011A-WElectron-May10ReReco-v1.root");
    if (!f) {
      f = new TFile("/data1/dimatteo/Calibration/Ntuples/Run2011A/WZAnalysisSingleXtal/WZAnalysis_SingleElectron_Run2011A-WElectron-May10ReReco-v1.root");
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

  // Set my momentum scale using the input graphs
  myMomentumScale = inputMomentumScale;
  myTypeEE = typeEE;
}

/// Deconstructor

FastCalibratorEE::~FastCalibratorEE(){

  if (!fChain) return;
  delete fChain->GetCurrentFile();
}

/// Get Entry Method
Int_t FastCalibratorEE::GetEntry(Long64_t entry){
// Read contents of entry.
  if (!fChain) return 0;
  return fChain->GetEntry(entry);
}

/// Load Tree infos in a chain
Long64_t FastCalibratorEE::LoadTree(Long64_t entry){

// Set the environment to read one entry
  if (!fChain) return -5;
  Long64_t centry = fChain->LoadTree(entry);
  if (centry < 0) return centry;
  if (!fChain->InheritsFrom(TChain::Class()))  return centry;
  TChain *chain = (TChain*)fChain;
  if (chain->GetTreeNumber() != fCurrent) {
    fCurrent = chain->GetTreeNumber();
  }
  return centry;
}

///Fill the miscalib map
void FastCalibratorEE::FillScalibMap(TString miscalibMap){

  std::ifstream scalibFile;
  scalibFile.open(miscalibMap.Data());

  if (!scalibFile) {
    std::cout<<"miscalib map missing!!!"<<std::endl;
    return;
  }

  else {
    std::cout<<"miscalib map successfully opened!!! Map: "<<miscalibMap<<std::endl;
  }

  float etaRing;
  float scalibValue;

  while (!scalibFile.eof()) {
    scalibFile >> etaRing >> scalibValue;
    scalibMap.insert(std::pair<float,float>(etaRing,scalibValue));
  }
  scalibFile.close();
}


/// Intialize pointers
void FastCalibratorEE::Init(TTree *tree){

  /// Set object pointer
  
    energyRecHitSCEle1 = 0;
  XRecHitSCEle1 = 0;  
  YRecHitSCEle1 = 0;  
  ZRecHitSCEle1 = 0;
  recoFlagRecHitSCEle1 = 0;
  
  energyRecHitSCEle2 = 0;
  XRecHitSCEle2 = 0;  
  YRecHitSCEle2 = 0;  
  ZRecHitSCEle2 = 0;
  recoFlagRecHitSCEle2 = 0;
  
  /// Set branch addresses and branch pointers
  
  if (!tree) return;
  fChain = tree;
  fCurrent = -1;
  fChain->SetMakeClass(1);
  
  fChain->SetBranchStatus("*", 0);
  
  fChain->SetBranchStatus("runNumber", 1);   fChain->SetBranchAddress("runNumber", &runNumber, &b_runNumber);
  fChain->SetBranchStatus("lumiBlock", 1);  fChain->SetBranchAddress("lumiBlock", &lumiBlock, &b_lumiBlock);
  fChain->SetBranchStatus("eventNumber", 1); fChain->SetBranchAddress("eventNumber", &eventNumber, &b_eventNumber);
  
  fChain->SetBranchStatus("chargeEle", 1);             fChain->SetBranchAddress("chargeEle", chargeEle);
  fChain->SetBranchStatus("etaEle", 1);             fChain->SetBranchAddress("etaEle", &etaEle, &b_etaEle);
  fChain->SetBranchStatus("PtEle", 1);             fChain->SetBranchAddress("PtEle", &PtEle, &b_PtEle);
  fChain->SetBranchStatus("phiEle", 1);             fChain->SetBranchAddress("phiEle", &phiEle, &b_phiEle);
  fChain->SetBranchStatus("rawEnergySCEle", 1);             fChain->SetBranchAddress("rawEnergySCEle", &rawEnergySCEle, &b_rawEnergySCEle);
  fChain->SetBranchStatus("energySCEle", 1);             fChain->SetBranchAddress("energySCEle", &energySCEle, &b_energySCEle);
  fChain->SetBranchStatus("etaSCEle", 1);             fChain->SetBranchAddress("etaSCEle", &etaSCEle, &b_etaSCEle);
  fChain->SetBranchStatus("esEnergySCEle", 1);             fChain->SetBranchAddress("esEnergySCEle", &esEnergySCEle, &b_esEnergySCEle);
  fChain->SetBranchStatus("pAtVtxGsfEle", 1);             fChain->SetBranchAddress("pAtVtxGsfEle", &pAtVtxGsfEle, &b_pAtVtxGsfEle);
  fChain->SetBranchStatus("fbremEle", 1);             fChain->SetBranchAddress("fbremEle", &fbremEle, &b_fbremEle);
  fChain->SetBranchStatus("energyMCEle", 1);             fChain->SetBranchAddress("energyMCEle", &energyMCEle, &b_energyMCEle);
  fChain->SetBranchStatus("etaMCEle", 1);             fChain->SetBranchAddress("etaMCEle", &etaMCEle, &b_etaMCEle);
  fChain->SetBranchStatus("phiMCEle", 1);             fChain->SetBranchAddress("phiMCEle", &phiMCEle, &b_phiMCEle);

  // ele1
  fChain->SetBranchStatus("energyRecHitSCEle1", 1);           fChain->SetBranchAddress("energyRecHitSCEle1", &energyRecHitSCEle1, &b_energyRecHitSCEle1);
  fChain->SetBranchStatus("XRecHitSCEle1", 1);    fChain->SetBranchAddress("XRecHitSCEle1", &XRecHitSCEle1, &b_XRecHitSCEle1);
  fChain->SetBranchStatus("YRecHitSCEle1", 1);    fChain->SetBranchAddress("YRecHitSCEle1", &YRecHitSCEle1, &b_YRecHitSCEle1);
  fChain->SetBranchStatus("ZRecHitSCEle1", 1);    fChain->SetBranchAddress("ZRecHitSCEle1", &ZRecHitSCEle1, &b_ZRecHitSCEle1);
  fChain->SetBranchStatus("recoFlagRecHitSCEle1", 1);        fChain->SetBranchAddress("recoFlagRecHitSCEle1", &recoFlagRecHitSCEle1, &b_recoFlagRecHitSCEle1);
  
  // ele2
  fChain->SetBranchStatus("energyRecHitSCEle2", 1);           fChain->SetBranchAddress("energyRecHitSCEle2", &energyRecHitSCEle2, &b_energyRecHitSCEle2);
  fChain->SetBranchStatus("XRecHitSCEle2", 1);    fChain->SetBranchAddress("XRecHitSCEle2", &XRecHitSCEle2, &b_XRecHitSCEle2);
  fChain->SetBranchStatus("YRecHitSCEle2", 1);    fChain->SetBranchAddress("YRecHitSCEle2", &YRecHitSCEle2, &b_YRecHitSCEle2);
  fChain->SetBranchStatus("ZRecHitSCEle2", 1);    fChain->SetBranchAddress("ZRecHitSCEle2", &ZRecHitSCEle2, &b_ZRecHitSCEle2);
  fChain->SetBranchStatus("recoFlagRecHitSCEle2", 1);        fChain->SetBranchAddress("recoFlagRecHitSCEle2", &recoFlagRecHitSCEle2, &b_recoFlagRecHitSCEle2);
  
}


//! Declaration of the objects that are save in the output file

void FastCalibratorEE::bookHistos(int nLoops){

  ///service histos
  h_scale_hashedIndex_EE = new TH1F ("h_scale_hashedIndex_EE","h_scale_hashedIndex_EE", kEEhalf*2, 0, kEEhalf*2 - 1 );
  h_occupancy_hashedIndex_EE = new TH1F ("h_occupancy_hashedIndex_EE","h_occupancy_hashedIndex_EE", kEEhalf*2, 0, kEEhalf*2 - 1 );
  hC_EoP = new hChain ("EoP", "EoP", 500,0.2,3.0, nLoops);
 
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



///===== Build E/p for electron 1 and 2

void FastCalibratorEE::BuildEoPeta_ele(int iLoop, int nentries , int useW, int useZ, std::vector<float> theScalibration, bool isSaveEPDistribution, bool isR9selection, float R9Min, bool isfbrem, float fbremMax, bool isPtCut, float PtMin, bool isMCTruth){

  if(iLoop ==0){
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
   //   std::cerr<<"building E/p distribution ----> "<<jentry<<" vs "<<nentries<<std::endl;
      if (!(jentry%1000000))std::cerr<<"building E/p distribution ----> "<<jentry<<" vs "<<nentries<<std::endl;

   float pIn, FdiEta;
   ///=== electron tight W or Z only Endcap
   if ( isEBEle[0] == 0 && (( useW == 1 && chargeEle[1] == -100 ) || ( useZ== 1 && chargeEle[1]!=-100 ))) {

    FdiEta = energySCEle[0]/(rawEnergySCEle[0]+esEnergySCEle[0]); /// Cluster containment approximation using ps infos
   
    float thisE = 0;
    int   iseed = 0 ;
    int seed_hashedIndex = 0;
    float E_seed = 0;
    float thisE3x3 = 0;

    /// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
    for (unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle1->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = GetHashedIndexEE(XRecHitSCEle1->at(iRecHit), YRecHitSCEle1->at(iRecHit), ZRecHitSCEle1->at(iRecHit));
          
            if(energyRecHitSCEle1 -> at(iRecHit) > E_seed && recoFlagRecHitSCEle1 -> at(iRecHit) < 4 ){
              seed_hashedIndex=GetHashedIndexEE(XRecHitSCEle1->at(iRecHit), YRecHitSCEle1->at(iRecHit), ZRecHitSCEle1->at(iRecHit));
              iseed=iRecHit;
              E_seed=energyRecHitSCEle1 -> at(iRecHit); ///Seed infos

            }
    
            if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
            
            //if(recoFlagRecHitSCEle1 -> at(iRecHit) < 4)
            thisE += theScalibration[thisIndex]*energyRecHitSCEle1 -> at(iRecHit)*FdiEta*thisIC; /// SC energy
          
    }

    for (unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle1->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = GetHashedIndexEE(XRecHitSCEle1->at(iRecHit), YRecHitSCEle1->at(iRecHit), ZRecHitSCEle1->at(iRecHit));

	    if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
         
            if(fabs(XRecHitSCEle1->at(iRecHit)-XRecHitSCEle1->at(iseed))<=1 && 
               fabs(YRecHitSCEle1->at(iRecHit)-YRecHitSCEle1->at(iseed))<=1 &&
               recoFlagRecHitSCEle1 -> at(iRecHit) < 4)
              thisE3x3+=theScalibration[thisIndex]*energyRecHitSCEle1 -> at(iRecHit)*FdiEta*thisIC;
    }

          
    int ix_seed   = GetIxFromHashedIndex(seed_hashedIndex);
    int iy_seed   = GetIyFromHashedIndex(seed_hashedIndex);
    int iz_seed   = GetZsideFromHashedIndex(seed_hashedIndex);
    int ir_seed   = eRings -> GetEndcapRing(ix_seed,iy_seed,iz_seed); /// Seed ring 
   
    bool skipElectron = false;

    /// Option for MCTruth analysis
    if(!isMCTruth){ 
       pIn = pAtVtxGsfEle[0];
       int regionId = templIndexEE(myTypeEE,etaEle[0],chargeEle[0],thisE3x3/thisE);
       pIn /= myMomentumScale[regionId] -> Eval( phiEle[0] );
    }
    else{ 
       pIn = energyMCEle[0];
       ele1_DR     = TMath::Sqrt((etaMCEle[0]-etaEle[0])*(etaMCEle[0]-etaEle[0])+(phiMCEle[0]-phiEle[0])*(phiMCEle[0]-phiEle[0])) ;
       if(fabs(ele1_DR)>0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
    }

    /// R9, fbrem selection before E/p distribution
    if( fabs(thisE3x3/thisE) < 0.80 && isR9selection == true && fabs(etaSCEle[0]) <= 1.75 )                             skipElectron = true;
    if( fabs(thisE3x3/thisE) < 0.88 && isR9selection == true && fabs(etaSCEle[0]) >  1.75 && fabs(etaSCEle[0]) <= 2.00 ) skipElectron = true;
    if( fabs(thisE3x3/thisE) < 0.92 && isR9selection == true && fabs(etaSCEle[0]) >  2.00 && fabs(etaSCEle[0]) <= 2.15 ) skipElectron = true;
    if( fabs(thisE3x3/thisE) < 0.94 && isR9selection == true && fabs(etaSCEle[0]) >  2.15 )                             skipElectron = true;
 
    if( fabs(thisE3x3/thisE) < R9Min && isR9selection == true ) skipElectron = true;

    if( fabs(fbremEle[0]) > fbremMax && isfbrem == true ) skipElectron =true;
    if( PtEle[0] < PtMin && isPtCut == true ) skipElectron =true;

    //    std::cout<<skipElectron<<" "<<ir_seed<<" "<<thisE<<" "<<pIn<<std::endl;   //DEBUG
       if( !skipElectron )  hC_EoP_ir_ele -> Fill(ir_seed,thisE/(pIn-esEnergySCEle[0])); 
  }

  ///=== Second medium electron from Z only Endcaps
   if ( fabs(etaSCEle[1]) >= 1.479 && ( useZ== 1 && chargeEle[1]!=-100 )) {

    FdiEta = energySCEle[1]/(rawEnergySCEle[1]+esEnergySCEle[1]);

    float thisE = 0;
    int   iseed = 0 ;
    int seed_hashedIndex = 0;
    float E_seed = 0;
    float thisE3x3 = 0;
  
    /// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
    for (unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle2->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = GetHashedIndexEE(XRecHitSCEle2->at(iRecHit), YRecHitSCEle2->at(iRecHit), ZRecHitSCEle2->at(iRecHit));

            if(energyRecHitSCEle2 -> at(iRecHit) > E_seed && recoFlagRecHitSCEle2 -> at(iRecHit) < 4 ){
              seed_hashedIndex=GetHashedIndexEE(XRecHitSCEle2->at(iRecHit), YRecHitSCEle2->at(iRecHit), ZRecHitSCEle2->at(iRecHit));
              iseed=iRecHit;
              E_seed=energyRecHitSCEle2 -> at(iRecHit);

            }
    
            if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
            
            if(recoFlagRecHitSCEle2 -> at(iRecHit) < 4) /// Only Good channels
            thisE += theScalibration[thisIndex]*energyRecHitSCEle2 -> at(iRecHit)*FdiEta*thisIC;
             
    }
    for (unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle2->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = GetHashedIndexEE(XRecHitSCEle2->at(iRecHit), YRecHitSCEle2->at(iRecHit), ZRecHitSCEle2->at(iRecHit));
            // IC obtained from previous Loops
            if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
         
            if(fabs(XRecHitSCEle2->at(iRecHit)-XRecHitSCEle2->at(iseed))<=1 && 
               fabs(YRecHitSCEle2->at(iRecHit)-YRecHitSCEle2->at(iseed))<=1 &&
               recoFlagRecHitSCEle2 -> at(iRecHit) < 4)
              thisE3x3+=theScalibration[thisIndex]*energyRecHitSCEle2 -> at(iRecHit)*FdiEta*thisIC;
    }

    int ix_seed = GetIxFromHashedIndex(seed_hashedIndex);
    int iy_seed = GetIyFromHashedIndex(seed_hashedIndex);
    int iz_seed = GetZsideFromHashedIndex(seed_hashedIndex);
    int ir_seed = eRings -> GetEndcapRing(ix_seed,iy_seed,iz_seed); /// Seed ring
     
    bool skipElectron = false;
    /// Option for MCTruth Analysis
    if(!isMCTruth){
       pIn = pAtVtxGsfEle[1];
       int regionId = templIndexEE(myTypeEE,etaEle[1],chargeEle[1],thisE3x3/thisE);
       pIn /= myMomentumScale[regionId] -> Eval( phiEle[1] );
    }
    else{ 
       pIn = energyMCEle[1];
       ele2_DR     = TMath::Sqrt((etaMCEle[1]-etaEle[1])*(etaMCEle[1]-etaEle[1])+(phiMCEle[1]-phiEle[1])*(phiMCEle[1]-phiEle[1])) ;
       if(fabs(ele2_DR)>0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
    }
    /// R9 and fbrem selection
    if( fabs(thisE3x3/thisE) < 0.80 && isR9selection == true && fabs(etaSCEle[1]) <= 1.75 )                             skipElectron = true;
    if( fabs(thisE3x3/thisE) < 0.88 && isR9selection == true && fabs(etaSCEle[1]) >  1.75 && fabs(etaSCEle[1]) <= 2.00 ) skipElectron = true;
    if( fabs(thisE3x3/thisE) < 0.92 && isR9selection == true && fabs(etaSCEle[1]) >  2.00 && fabs(etaSCEle[1]) <= 2.15 ) skipElectron = true;
    if( fabs(thisE3x3/thisE) < 0.94 && isR9selection == true && fabs(etaSCEle[1]) >  2.15 )                             skipElectron = true;
     
    if( fabs(thisE3x3/thisE) < R9Min && isR9selection == true ) skipElectron = true;

    if( fabs(fbremEle[1]) > fbremMax && isfbrem == true ) skipElectron =true;
    if( PtEle[1] < PtMin && isPtCut == true ) skipElectron =true;

    if(!skipElectron) hC_EoP_ir_ele -> Fill(ir_seed,thisE/(pIn-esEnergySCEle[1]));
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
void FastCalibratorEE::Loop( int nentries, int useZ, int useW, int splitStat, int nLoops, bool isMiscalib,bool isSaveEPDistribution,
			     bool isEPselection, bool isR9selection, float R9Min, bool isfbrem, float fbremMax, bool isPtCut, float PtMin,
                             bool isMCTruth, std::map<int, std::vector<std::pair<int, int> > > jsonMap, float miscalibMethod, TString miscalibMap){

   if (fChain == 0) return;

   if(isMiscalib == true) {
      std::cout<<"method used for the scalibration (1=from map, 0=linear): "<<miscalibMethod<<std::endl;
      if (miscalibMethod == 1) {  //miscalibration with a gaussian spread (eta-dependent)                                          
	 FillScalibMap(miscalibMap);  //fill the map with the scalib values    
	 std::cout<<"Using miscalibration from map"<<std::endl;
      }
   }

   /// Define the number of crystal you want to calibrate
   int m_regions = kEEhalf;
   
   std::cout << "m_regions " << m_regions << std::endl;
  
   /// build up scalibration map
   std::vector<float> theScalibration(m_regions*2, 0.);
   TRandom3 genRand;
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
       if(isMiscalib == true) {
	 int etaRing = eRings->GetEndcapRing(GetIxFromHashedIndex(iIndex),GetIyFromHashedIndex(iIndex),GetZsideFromHashedIndex(iIndex));
         if (miscalibMethod == 1) {  //miscalibration with a gaussian spread (eta-dependent)                                          
	   theScalibration[iIndex] = scalibMap.at(etaRing);   //take the values from the map filled before                         
	 }
	 else
	   	   theScalibration[iIndex] = 1 + 0.015*etaRing/33.;   //linear eta-dependent scalibration              
	   //	   	   theScalibration[iIndex] = genRand.Gaus(1.,0.001);
       }
     
       //       if(isMiscalib==true) theScalibration[iIndex] = genRand.Gaus(1.,0.05); /// Miscalibration fixed at 5%
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
    
    ///==== build E/p distribution ele 1 and 2
    BuildEoPeta_ele(iLoop,nentries,useW,useZ,theScalibration,isSaveEPDistribution,isR9selection,R9Min,isfbrem,fbremMax,isPtCut,PtMin,isMCTruth);
    
    // define map with events
    std::map<std::pair<int,std::pair<int,int> >,int> eventsMap;
    
    /// Loop over events
    std::cout << "Number of analyzed events = " << nentries << std::endl;
    
    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries;jentry++){
        if (!(jentry%100000))std::cerr<<jentry;
        if (!(jentry%10000)) std::cerr<<".";
      
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   
        nbytes += nb;
        
        
        //*********************************
	// JSON FILE AND DUPLIACTES IN DATA
        
        bool skipEvent = false;
	if( isMCTruth == 0 ){

          if(AcceptEventByRunAndLumiSection(runNumber,lumiBlock,jsonMap) == false) skipEvent = true;
          
          std::pair<int,Long64_t> eventLSandID(lumiBlock,eventNumber);
          std::pair<int,std::pair<int,Long64_t> > eventRUNandLSandID(runNumber,eventLSandID);
          if( eventsMap[eventRUNandLSandID] == 1 ) skipEvent = true;
          else eventsMap[eventRUNandLSandID] = 1;
        }
        
        if( skipEvent == true ) continue;
                        
        float pIn, FdiEta;
      
        std::map<int,double> map;
        bool skipElectron=false;
	
        /// Only tight electron from W and Z, only Endcap
        
	if ( fabs(etaSCEle[0]) >= 1.479 && (( useW == 1 && chargeEle[1]==-100 ) || ( useZ== 1 && chargeEle[1]!=-100 ))) {

          /// SCL energy containment correction
          FdiEta = energySCEle[0]/(rawEnergySCEle[0]+esEnergySCEle[0]);
         
	  float thisE = 0;
          float thisE3x3 =0 ;
          int iseed = 0 ;
          int seed_hashedIndex = 0 ;
          float E_seed = 0;

          /// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
          for (unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle1->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = GetHashedIndexEE(XRecHitSCEle1->at(iRecHit), YRecHitSCEle1->at(iRecHit), ZRecHitSCEle1->at(iRecHit));
  
            if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
            
            if(recoFlagRecHitSCEle1 -> at(iRecHit) < 4) /// Skip bad channel 
            thisE += theScalibration[thisIndex]*energyRecHitSCEle1 -> at(iRecHit)*FdiEta*thisIC;
     
            if(energyRecHitSCEle1 -> at(iRecHit) > E_seed && recoFlagRecHitSCEle1 -> at(iRecHit) < 4 ){
              E_seed=energyRecHitSCEle1 -> at(iRecHit);
              iseed=iRecHit;
              seed_hashedIndex=GetHashedIndexEE(XRecHitSCEle1->at(iRecHit), YRecHitSCEle1->at(iRecHit), ZRecHitSCEle1->at(iRecHit)); /// Seed infos
             }
          
          }
          
          for (unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle1->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = GetHashedIndexEE(XRecHitSCEle1->at(iRecHit), YRecHitSCEle1->at(iRecHit), ZRecHitSCEle1->at(iRecHit));
    
            if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
            
            if(fabs(XRecHitSCEle1->at(iRecHit)-XRecHitSCEle1->at(iseed))<=1 && 
            fabs(YRecHitSCEle1->at(iRecHit)-YRecHitSCEle1->at(iseed))<=1 &&
            recoFlagRecHitSCEle1 -> at(iRecHit) < 4)
              thisE3x3+=theScalibration[thisIndex]*energyRecHitSCEle1 -> at(iRecHit)*FdiEta*thisIC;
                 
          }
            
        /// find the zside
        int thisCaliBlock = -1;
        if (GetZsideFromHashedIndex(GetHashedIndexEE(XRecHitSCEle1->at(iseed), YRecHitSCEle1->at(iseed), ZRecHitSCEle1->at(iseed))) < 0) thisCaliBlock = 0;
        else thisCaliBlock = 1;
 
        int ix_seed = GetIxFromHashedIndex(seed_hashedIndex);
        int iy_seed = GetIyFromHashedIndex(seed_hashedIndex);
        int iz_seed = GetZsideFromHashedIndex(seed_hashedIndex);
        int ir_seed = eRings -> GetEndcapRing(ix_seed,iy_seed,iz_seed);
      
        /// MCTruth option 
         if(!isMCTruth) {
           pIn = pAtVtxGsfEle[0];
	   int regionId = templIndexEE(myTypeEE,etaEle[0],chargeEle[0],thisE3x3/thisE);
           pIn /= myMomentumScale[regionId] -> Eval( phiEle[0] );
         }
         else{
           pIn = energyMCEle[0];
	   ele1_DR     = TMath::Sqrt((etaMCEle[0]-etaEle[0])*(etaMCEle[0]-etaEle[0])+(phiMCEle[0]-phiEle[0])*(phiMCEle[0]-phiEle[0])) ;
           if(fabs(ele1_DR)>0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
         }
        
      
       TH1F* EoPHisto = hC_EoP_ir_ele->GetHisto(ir_seed);
       
       if ( fabs(thisE/(pAtVtxGsfEle[0]-esEnergySCEle[0]) - 1) > 0.7 && isEPselection==true) skipElectron = true; /// Take the correct E/p pdf to weight events in the calib procedure
       
       /// R9 and fbrem selection
       if( fabs(thisE3x3/thisE) < 0.80 && isR9selection == true && fabs(etaSCEle[0]) <= 1.75 )                             skipElectron = true;
       if( fabs(thisE3x3/thisE) < 0.88 && isR9selection == true && fabs(etaSCEle[0]) >  1.75 && fabs(etaSCEle[0]) <= 2.00 ) skipElectron = true;
       if( fabs(thisE3x3/thisE) < 0.92 && isR9selection == true && fabs(etaSCEle[0]) >  2.00 && fabs(etaSCEle[0]) <= 2.15 ) skipElectron = true;
       if( fabs(thisE3x3/thisE) < 0.94 && isR9selection == true && fabs(etaSCEle[0]) >  2.15 )                             skipElectron = true;
       
       if( fabs(thisE3x3/thisE) < R9Min && isR9selection == true ) skipElectron = true;
       
       if( fabs(fbremEle[0]) > fbremMax && isfbrem == true ) skipElectron =true;
       if( PtEle[0] < PtMin  && isPtCut == true ) skipElectron =true;
       
       if( thisE/(pIn-esEnergySCEle[0]) < EoPHisto->GetXaxis()->GetXmin() ||
           thisE/(pIn-esEnergySCEle[0]) > EoPHisto->GetXaxis()->GetXmax() ) skipElectron=true;
 
         if( !skipElectron ){
           for( unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle1->size(); iRecHit++){
             if(recoFlagRecHitSCEle1 -> at(iRecHit) >= 4) continue;
             
             int thisIndex = GetHashedIndexEE(XRecHitSCEle1->at(iRecHit), YRecHitSCEle1->at(iRecHit), ZRecHitSCEle1->at(iRecHit));
             float thisIC = 1.;
             
             if( iLoop > 0 ) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
             
             /// Fill the occupancy map JUST for the first Loop
             if( iLoop == 0 ){
               h_occupancy_hashedIndex_EE -> Fill(thisIndex);
               if ( GetZsideFromHashedIndex(thisIndex) < 0 ) h_occupancy_EEM -> Fill(GetIxFromHashedIndex(thisIndex), GetIyFromHashedIndex(thisIndex) );
               else                                          h_occupancy_EEP -> Fill(GetIxFromHashedIndex(thisIndex), GetIyFromHashedIndex(thisIndex) );
             }
            
             ///Use full statistic
             if( splitStat == 0 ){
               if(thisCaliBlock == 0){
                 int EoPbin = EoPHisto->FindBin(thisE/(pIn-esEnergySCEle[0]));
                 theNumerator_EEM[thisIndex]   += theScalibration[thisIndex]*energyRecHitSCEle1 -> at(iRecHit)*FdiEta*thisIC / 
                                                  thisE*(pIn-esEnergySCEle[0])/thisE*EoPHisto->GetBinContent(EoPbin);
                 theDenominator_EEM[thisIndex] += theScalibration[thisIndex]*energyRecHitSCEle1 -> at(iRecHit)*FdiEta*thisIC / 
                                                  thisE*EoPHisto->GetBinContent(EoPbin);
               }
               
               if( thisCaliBlock == 1 ){
                 int EoPbin = EoPHisto->FindBin(thisE/(pIn-esEnergySCEle[0]));
                 theNumerator_EEP[thisIndex]   += theScalibration[thisIndex]*energyRecHitSCEle1 -> at(iRecHit)*FdiEta*thisIC / 
                                                  thisE*(pIn-esEnergySCEle[0])/thisE*EoPHisto->GetBinContent(EoPbin);
                 theDenominator_EEP[thisIndex] += theScalibration[thisIndex]*energyRecHitSCEle1 -> at(iRecHit)*FdiEta*thisIC / 
                                                  thisE*EoPHisto->GetBinContent(EoPbin);
               }
	     }
             
             
             /// use evens    
             if( splitStat == 1 && jentry%2 == 0 ){
               if( thisCaliBlock == 0 ){
                 int EoPbin = EoPHisto->FindBin(thisE/(pIn-esEnergySCEle[0]));
                 theNumerator_EEM[thisIndex]   += theScalibration[thisIndex]*energyRecHitSCEle1 -> at(iRecHit)*FdiEta*thisIC /
                                                  thisE*(pIn-esEnergySCEle[0])/thisE*EoPHisto->GetBinContent(EoPbin);
                 theDenominator_EEM[thisIndex] += theScalibration[thisIndex]*energyRecHitSCEle1 -> at(iRecHit)*FdiEta*thisIC /
                                                  thisE*EoPHisto->GetBinContent(EoPbin);
               }
               
               if(thisCaliBlock == 1){
                 int EoPbin = EoPHisto->FindBin(thisE/(pIn-esEnergySCEle[0]));
                 theNumerator_EEP[thisIndex]   += theScalibration[thisIndex]*energyRecHitSCEle1 -> at(iRecHit)*FdiEta*thisIC /
                                                  thisE*(pIn-esEnergySCEle[0])/thisE*EoPHisto->GetBinContent(EoPbin);
                 theDenominator_EEP[thisIndex] += theScalibration[thisIndex]*energyRecHitSCEle1 -> at(iRecHit)*FdiEta*thisIC /
                                                  thisE*EoPHisto->GetBinContent(EoPbin);
               }
             }
             
             /// use odd    
             if( splitStat == -1 && jentry%2 != 0 ){
               if(thisCaliBlock == 0){
                 int EoPbin = EoPHisto->FindBin(thisE/(pIn-esEnergySCEle[0]));
                 theNumerator_EEM[thisIndex]   += theScalibration[thisIndex]*energyRecHitSCEle1 -> at(iRecHit)*FdiEta*thisIC /
                                                  thisE*(pIn-esEnergySCEle[0])/thisE*EoPHisto->GetBinContent(EoPbin);
                 theDenominator_EEM[thisIndex] += theScalibration[thisIndex]*energyRecHitSCEle1 -> at(iRecHit)*FdiEta*thisIC /
                                                  thisE*EoPHisto->GetBinContent(EoPbin);
               }
               
               if(thisCaliBlock == 1){
                 int EoPbin = EoPHisto->FindBin(thisE/(pIn-esEnergySCEle[0]));
                 theNumerator_EEP[thisIndex]   += theScalibration[thisIndex]*energyRecHitSCEle1 -> at(iRecHit)*FdiEta*thisIC /
                                                  thisE*(pIn-esEnergySCEle[0])/thisE*EoPHisto->GetBinContent(EoPbin);
                 theDenominator_EEP[thisIndex] += theScalibration[thisIndex]*energyRecHitSCEle1 -> at(iRecHit)*FdiEta*thisIC /
                                                  thisE*EoPHisto->GetBinContent(EoPbin);
               }
             }
           }
         }
         
         ///Fill EoP
         if( thisCaliBlock != -1 ) hC_EoP -> Fill(iLoop, thisE/(pIn-esEnergySCEle[0]));
        }
        
        skipElectron = false;     
        
        
	/// Medium ele from Z only Endcap
	if ( fabs(etaSCEle[1]) >= 1.479 && ( useZ== 1 && chargeEle[1]!=-100 )) {
          /// SCL energy containment correction
          FdiEta = energySCEle[1]/(rawEnergySCEle[1]+esEnergySCEle[1]);
         
	  float thisE = 0;
          float thisE3x3 =0 ;
          int iseed = 0 ;
          int seed_hashedIndex = 0;
          float E_seed = 0;

         
          /// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
          for(unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle2->size(); iRecHit++ ){
            float thisIC = 1.;
            int thisIndex = GetHashedIndexEE(XRecHitSCEle2->at(iRecHit), YRecHitSCEle2->at(iRecHit), ZRecHitSCEle2->at(iRecHit));
           
	    if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
            
            if( recoFlagRecHitSCEle2 -> at(iRecHit) < 4 )
            thisE += theScalibration[thisIndex]*energyRecHitSCEle2 -> at(iRecHit)*FdiEta*thisIC;
            
            if(energyRecHitSCEle2 -> at(iRecHit) > E_seed && recoFlagRecHitSCEle2 -> at(iRecHit) < 4){
              E_seed=energyRecHitSCEle2 -> at(iRecHit);
              iseed=iRecHit;
              seed_hashedIndex=GetHashedIndexEE(XRecHitSCEle2->at(iRecHit), YRecHitSCEle2->at(iRecHit), ZRecHitSCEle2->at(iRecHit));
            }
          }
          
          for (unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle2->size(); iRecHit++ )
          {
            float thisIC = 1.;
            int thisIndex = GetHashedIndexEE(XRecHitSCEle2->at(iRecHit), YRecHitSCEle2->at(iRecHit), ZRecHitSCEle2->at(iRecHit));
            
            if (iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
            
            if(fabs(XRecHitSCEle2->at(iRecHit)-XRecHitSCEle2->at(iseed))<=1 && 
               fabs(YRecHitSCEle2->at(iRecHit)-YRecHitSCEle2->at(iseed))<=1 &&
               recoFlagRecHitSCEle2 -> at(iRecHit) < 4)
              thisE3x3+=theScalibration[thisIndex]*energyRecHitSCEle2 -> at(iRecHit)*FdiEta*thisIC;
	  }                
          
          /// find the zside
          int thisCaliBlock = -1;
          if (GetZsideFromHashedIndex(GetHashedIndexEE(XRecHitSCEle2->at(iseed), YRecHitSCEle2->at(iseed), ZRecHitSCEle2->at(iseed))) < 0) thisCaliBlock = 0;
          else thisCaliBlock = 1;
 
          int ix_seed = GetIxFromHashedIndex(seed_hashedIndex);
          int iy_seed = GetIyFromHashedIndex(seed_hashedIndex);
          int iz_seed = GetZsideFromHashedIndex(seed_hashedIndex);
          int ir_seed = eRings -> GetEndcapRing(ix_seed,iy_seed,iz_seed);
          
          /// Option for MCTruth Analysis
          if(!isMCTruth)  {
            pIn = pAtVtxGsfEle[1];
	    int regionId = templIndexEE(myTypeEE,etaEle[1],chargeEle[1],thisE3x3/thisE);
            pIn /= myMomentumScale[regionId] -> Eval( phiEle[1] );
          }
          else{
            pIn = energyMCEle[1];
	    ele2_DR     = TMath::Sqrt((etaMCEle[1]-etaEle[1])*(etaMCEle[1]-etaEle[1])+(phiMCEle[1]-phiEle[1])*(phiMCEle[1]-phiEle[1])) ;
            if(fabs(ele2_DR)>0.1) skipElectron = true ; /// No macthing beetween gen ele and reco ele
          }

          TH1F* EoPHisto = hC_EoP_ir_ele->GetHisto(ir_seed); /// Use correct pdf for reweight events in the L3 procedure
          
          /// E/p and R9 selections
          if ( fabs(thisE/(pIn-esEnergySCEle[1]) - 1) > 0.7 && isEPselection==true) skipElectron = true;
          
          /// R9 and fbrem selection
          if( fabs(thisE3x3/thisE) < 0.80 && isR9selection == true && fabs(etaSCEle[1]) <= 1.75 )                             skipElectron = true;
          if( fabs(thisE3x3/thisE) < 0.88 && isR9selection == true && fabs(etaSCEle[1]) >  1.75 && fabs(etaSCEle[1]) <= 2.00 ) skipElectron = true;
          if( fabs(thisE3x3/thisE) < 0.92 && isR9selection == true && fabs(etaSCEle[1]) >  2.00 && fabs(etaSCEle[1]) <= 2.15 ) skipElectron = true;
          if( fabs(thisE3x3/thisE) < 0.94 && isR9selection == true && fabs(etaSCEle[1]) >  2.15 )                             skipElectron = true;
          
          if( fabs(thisE3x3/thisE) < R9Min && isR9selection == true ) skipElectron = true;

          if( fabs(fbremEle[1]) > fbremMax && isfbrem == true ) skipElectron =true;
          if( PtEle[1] < PtMin  && isPtCut == true ) skipElectron =true;
          
          
          if( thisE/(pIn-esEnergySCEle[1]) < EoPHisto->GetXaxis()->GetXmin() ||
              thisE/(pIn-esEnergySCEle[1]) > EoPHisto->GetXaxis()->GetXmax() ) skipElectron=true;
           
          if( !skipElectron ){
            for( unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle2->size(); iRecHit++ ){
              if(recoFlagRecHitSCEle2 -> at(iRecHit) >= 4) continue;
              
              int thisIndex = GetHashedIndexEE(XRecHitSCEle2->at(iRecHit), YRecHitSCEle2->at(iRecHit), ZRecHitSCEle2->at(iRecHit));
              float thisIC = 1.;
              
              if(iLoop > 0) thisIC = h_scale_hashedIndex_EE -> GetBinContent(thisIndex+1);
                
              /// Fill the occupancy map JUST for the first Loop
              if ( iLoop == 0 ){
                h_occupancy_hashedIndex_EE -> Fill(thisIndex);
                if( GetZsideFromHashedIndex(thisIndex) < 0 ) h_occupancy_EEM -> Fill(GetIxFromHashedIndex(thisIndex), GetIyFromHashedIndex(thisIndex) );
                else                                         h_occupancy_EEP -> Fill(GetIxFromHashedIndex(thisIndex), GetIyFromHashedIndex(thisIndex) );
              }
              
              /// Use full statistic
              if( splitStat == 0){
                if(thisCaliBlock == 0){
                  int EoPbin = EoPHisto->FindBin(thisE/(pIn-esEnergySCEle[1]));
                  theNumerator_EEM[thisIndex]   += theScalibration[thisIndex]*energyRecHitSCEle2 -> at(iRecHit)*FdiEta*thisIC /
                                                   thisE*(pIn-esEnergySCEle[1])/thisE*EoPHisto->GetBinContent(EoPbin);
                  theDenominator_EEM[thisIndex] += theScalibration[thisIndex]*energyRecHitSCEle2 -> at(iRecHit)*FdiEta*thisIC /
                                                   thisE*EoPHisto->GetBinContent(EoPbin);
                }
                
                if( thisCaliBlock == 1 ){
                  int EoPbin = EoPHisto->FindBin(thisE/(pIn-esEnergySCEle[1]));
                  theNumerator_EEP[thisIndex]   += theScalibration[thisIndex]*energyRecHitSCEle2 -> at(iRecHit)*FdiEta*thisIC /
                                                   thisE*(pIn-esEnergySCEle[1])/thisE*EoPHisto->GetBinContent(EoPbin);
                  theDenominator_EEP[thisIndex] += theScalibration[thisIndex]*energyRecHitSCEle2 -> at(iRecHit)*FdiEta*thisIC / 
                                                   thisE*EoPHisto->GetBinContent(EoPbin);
                }
              }
              
              
              /// use evens    
              if( splitStat == 1 && jentry%2 == 0 ){
                if( thisCaliBlock == 0 ){
                  int EoPbin = EoPHisto->FindBin(thisE/(pIn-esEnergySCEle[1]));
                  theNumerator_EEM[thisIndex]   += theScalibration[thisIndex]*energyRecHitSCEle2 -> at(iRecHit)*FdiEta*thisIC /
                                                   thisE*(pIn-esEnergySCEle[1])/thisE*EoPHisto->GetBinContent(EoPbin);
                  theDenominator_EEM[thisIndex] += theScalibration[thisIndex]*energyRecHitSCEle2 -> at(iRecHit)*FdiEta*thisIC /
                                                   thisE*EoPHisto->GetBinContent(EoPbin);
               }
               
               if( thisCaliBlock == 1 ){
                 int EoPbin = EoPHisto->FindBin(thisE/(pIn-esEnergySCEle[1]));
                 theNumerator_EEP[thisIndex]   += theScalibration[thisIndex]*energyRecHitSCEle2 -> at(iRecHit)*FdiEta*thisIC /
                                                  thisE*(pIn-esEnergySCEle[1])/thisE*EoPHisto->GetBinContent(EoPbin);
                 theDenominator_EEP[thisIndex] += theScalibration[thisIndex]*energyRecHitSCEle2 -> at(iRecHit)*FdiEta*thisIC /
                                                  thisE*EoPHisto->GetBinContent(EoPbin);
               }
             }
             
             /// use odd    
             if( splitStat == -1 && jentry%2 != 0 ){
               if(thisCaliBlock == 0){
                 int EoPbin = EoPHisto->FindBin(thisE/(pIn-esEnergySCEle[1]));
                 theNumerator_EEM[thisIndex]   += theScalibration[thisIndex]*energyRecHitSCEle2 -> at(iRecHit)*FdiEta*thisIC /
                                                  thisE*(pIn-esEnergySCEle[1])/thisE*EoPHisto->GetBinContent(EoPbin);
                 theDenominator_EEM[thisIndex] += theScalibration[thisIndex]*energyRecHitSCEle2 -> at(iRecHit)*FdiEta*thisIC /
                                                  thisE*EoPHisto->GetBinContent(EoPbin);}
                
                if( thisCaliBlock == 1 ){
                  int EoPbin = EoPHisto->FindBin(thisE/(pIn-esEnergySCEle[1]));
                  theNumerator_EEP[thisIndex]   += theScalibration[thisIndex]*energyRecHitSCEle2 -> at(iRecHit)*FdiEta*thisIC /
                                                   thisE*(pIn-esEnergySCEle[0])/thisE*EoPHisto->GetBinContent(EoPbin);
                  theDenominator_EEP[thisIndex] += theScalibration[thisIndex]*energyRecHitSCEle2 -> at(iRecHit)*FdiEta*thisIC /
                                                   thisE*EoPHisto->GetBinContent(EoPbin);
                }
              }
            }
          }
          
          //Fill EoP
          if( thisCaliBlock != -1 ) hC_EoP -> Fill(iLoop, thisE/(pIn-esEnergySCEle[1]));
        }
    } ///  End Cycle on the events
       
    std::cout << ">>>>> [L3][endOfLoop] entering..." << std::endl;
      
    TH1F auxiliary_IC_EEM("auxiliary_IC_EEM","auxiliary_IC_EEM",50,0.2,1.9);
    TH1F auxiliary_IC_EEP("auxiliary_IC_EEP","auxiliary_IC_EEP",50,0.2,1.9);
    
    ///Fill the histo of IntercalibValues before the solve
    for( int iIndex = 0; iIndex < kEEhalf*2; iIndex++ ){
      if( h_occupancy_hashedIndex_EE -> GetBinContent(iIndex+1) > 0 ){
        int thisCaliBlock = -1;
        if( GetZsideFromHashedIndex(iIndex) < 0 ) thisCaliBlock = 0;
        else thisCaliBlock = 1;
        
        float thisIntercalibConstant = 1.;
        
        if( thisCaliBlock == 0 && theDenominator_EEM[iIndex] != 0. ) thisIntercalibConstant = theNumerator_EEM[iIndex] / theDenominator_EEM[iIndex];
        if( thisCaliBlock == 1 && theDenominator_EEP[iIndex] != 0. ) thisIntercalibConstant = theNumerator_EEP[iIndex] / theDenominator_EEP[iIndex];
        
        float oldIntercalibConstant = 1.;
        if( iLoop > 0 ) oldIntercalibConstant = h_scale_hashedIndex_EE -> GetBinContent (iIndex+1);
        h_scale_hashedIndex_EE -> SetBinContent(iIndex+1, thisIntercalibConstant*oldIntercalibConstant);
        
        if( thisCaliBlock == 0 ){
          hC_IntercalibValues_EEM -> Fill (iLoop, thisIntercalibConstant);
          hC_PullFromScalib_EEM -> Fill(iLoop,(thisIntercalibConstant*oldIntercalibConstant-1./theScalibration[iIndex]));
          hC_scale_EEM -> Fill(iLoop, GetIxFromHashedIndex(iIndex), GetIyFromHashedIndex(iIndex),thisIntercalibConstant*oldIntercalibConstant);
          
          auxiliary_IC_EEM.Fill(thisIntercalibConstant);
        }
        if( thisCaliBlock == 1){
          hC_IntercalibValues_EEP -> Fill (iLoop, thisIntercalibConstant);
          hC_PullFromScalib_EEP -> Fill(iLoop,(thisIntercalibConstant*oldIntercalibConstant-1./theScalibration[iIndex]));
          hC_scale_EEP -> Fill(iLoop, GetIxFromHashedIndex(iIndex), GetIyFromHashedIndex(iIndex),thisIntercalibConstant*oldIntercalibConstant);
          
          auxiliary_IC_EEP.Fill(thisIntercalibConstant);
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
    
   } /// End of Calibration Loops
   
   
   
   ///Fill the histo of IntercalibValues after the loops at last step
   for( int iIndex = 0; iIndex < kEEhalf*2; iIndex++ ){
     if( h_occupancy_hashedIndex_EE -> GetBinContent(iIndex+1) > 0 ){
       int thisCaliBlock = -1;
       if (GetZsideFromHashedIndex(iIndex) < 0) thisCaliBlock = 0;
       else thisCaliBlock = 1;
       
       int thisIx = GetIxFromHashedIndex(iIndex);
       int thisIy = GetIyFromHashedIndex(iIndex);
       int thisIz = GetZsideFromHashedIndex(iIndex); /// Ix, Iy and Iz info for each xtal
       
       float thisIntercalibConstant = h_scale_hashedIndex_EE -> GetBinContent (iIndex+1); /// Final IC value
       if ( thisCaliBlock == 0 ) h_scale_EEM -> Fill (thisIx, thisIy, thisIntercalibConstant);
       else                      h_scale_EEP -> Fill (thisIx, thisIy, thisIntercalibConstant);
       
       if( thisCaliBlock == 0 )
       {
         ///Vectors for IC Normalization
         IxValues_EEM.push_back(thisIx);
         IyValues_EEM.push_back(thisIy);
         ICValues_EEM.push_back(thisIntercalibConstant);
       }
       else
       {
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
       else
       {
         SumIC_Ring_EEM.at(thisIr) = SumIC_Ring_EEM.at(thisIr) + thisIntercalibConstant;
         Sumxtal_Ring_EEM.at(thisIr) = Sumxtal_Ring_EEM.at(thisIr) + 1;
       }
     }
   }
   
   /// IC Normaliztion trough the mean value of each ring
   for ( int iIndex = 0; iIndex < kEEhalf*2; iIndex++ )
   {
     if ( h_occupancy_hashedIndex_EE -> GetBinContent(iIndex+1) > 0 )
     {
       //       int thisCaliBlock = -1;
       //       if (GetZsideFromHashedIndex(iIndex) < 0) thisCaliBlock = 0;
       //       else thisCaliBlock = 1;
       
       int thisIx = GetIxFromHashedIndex(iIndex);
       int thisIy = GetIyFromHashedIndex(iIndex);
       int thisIz = GetZsideFromHashedIndex(iIndex);
       
       int thisIr = eRings -> GetEndcapRing(thisIx,thisIy,thisIz);
       
       float thisIntercalibConstant = h_scale_hashedIndex_EE -> GetBinContent (iIndex+1);
       
       if( thisIz > 0 )
       {
         if(Sumxtal_Ring_EEP.at(thisIr) != 0 && SumIC_Ring_EEP.at(thisIr)!= 0)
         h_scale_meanOnring_EEP->Fill(thisIx,thisIy,thisIntercalibConstant/(SumIC_Ring_EEP.at(thisIr)/Sumxtal_Ring_EEP.at(thisIr)));
       }
       else
       {
         if(Sumxtal_Ring_EEM.at(thisIr) != 0 && SumIC_Ring_EEM.at(thisIr) != 0)
         h_scale_meanOnring_EEM->Fill(thisIx,thisIy,thisIntercalibConstant/(SumIC_Ring_EEM.at(thisIr)/Sumxtal_Ring_EEM.at(thisIr)));
       }
     }
   }

}

/// Save in the output  
void FastCalibratorEE::saveHistos(TFile * f1)
{

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
void FastCalibratorEE::saveEoPeta(TFile * f2)
{
 f2->cd();
 hC_EoP_ir_ele ->Write(*f2);
 f2->Close(); 
}

/// Acquire fake Dead Xtal in order to study the effect of IC near them
void FastCalibratorEE::AcquireDeadXtal(TString inputDeadXtal, const bool & isDeadTriggerTower){

  if(inputDeadXtal!="NULL"){

   std::ifstream DeadXtal (inputDeadXtal.Data(),std::ios::binary);
   
   std::string buffer;
   int iX, iY ,iZ;
  
   while(!DeadXtal.eof()){
 
    getline(DeadXtal,buffer);
    std::stringstream line( buffer );
    line >> iX >> iY >>iZ ;
  
    DeadXtal_HashedIndex.push_back(GetHashedIndexEE(iX,iY,iZ)) ;
    if(isDeadTriggerTower){
 
      for (int ix = -2 ; ix <=2 ; ix++){
       for (int iy = -2 ; iy <=2 ; iy++){
         if(ix==0 && iy==0) continue ;
	 DeadXtal_HashedIndex.push_back(GetHashedIndexEE(iX+ix,iY+iy,iZ)) ; 
       }   
      }
    } 
  }

  sort(DeadXtal_HashedIndex.begin(), DeadXtal_HashedIndex.end());
  }
  else DeadXtal_HashedIndex.push_back(-9999);
      

}
/// Check if the channel considered is in the list of dead or not
bool FastCalibratorEE::CheckDeadXtal(const int & iX, const int & iY, const int & iZ){

  int hashed_Index;
  hashed_Index = GetHashedIndexEE(iX,iY,iZ);
  
  std::vector<int>::iterator iter = find(DeadXtal_HashedIndex.begin(),DeadXtal_HashedIndex.end(),hashed_Index);

  if(iter !=DeadXtal_HashedIndex.end())
     return true;
  else return false;
}
   
