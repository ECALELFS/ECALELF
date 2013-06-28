// -*- C++ -*-
//
// Package:    ZNtupleDumper
// Class:      ZNtupleDumper
// 
/// Zee and E/p ntuple dumper from patElectrons
/**\class ZNtupleDumper ZNtupleDumper.cc calibration/ZNtupleDumper/src/ZNtupleDumper.cc
 * 
 * Description: Zee and E/p ntuple dumper from patElectrons
 * 
 * \todo 
 - recHitCollection is included in the PAT electron collection, take it from there
 - take the R9 from the PAT electron: electron->r9()
 - flag for Zee or Wenu dump
 - check   maxNumberOfExpectedMissingHits[index] = electron1.gsfTrack()->trackerExpectedHitsInner().numberOfLostHits();
 * 
 */

//
// Original Author:  Shervin Nourbakhsh,40 1-B24,+41227671643,
//         Created:  Mon Jan 16 18:20:34 CET 2012
// $Id$
//
//
//
// system include files
#include <memory>
#include <iostream>
#include <string.h>

// root include files
#include <TTree.h>
#include <TLorentzVector.h>
#include <TString.h>
#include <TFile.h>

// user include files
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"

//#include "calibration/ZNtupleDumper/interface/puWeights_class.hh"

#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"

#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"

#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "RecoEgamma/EgammaTools/interface/ConversionFinder.h"
#include "RecoEgamma/EgammaTools/interface/ConversionInfo.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"


#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"

#include "CondFormats/EcalObjects/interface/EcalADCToGeVConstant.h"
#include "CondFormats/EcalObjects/interface/EcalChannelStatus.h"
#include "CondFormats/EcalObjects/interface/EcalGainRatios.h"
#include "CondFormats/EcalObjects/interface/EcalIntercalibConstants.h"
#include "CondFormats/EcalObjects/interface/EcalIntercalibConstantsMC.h"
#include "CondFormats/EcalObjects/interface/EcalLaserAPDPNRatios.h"
#include "CondFormats/EcalObjects/interface/EcalLaserAPDPNRatiosRef.h"
#include "CondFormats/EcalObjects/interface/EcalLaserAlphas.h"
#include "CondFormats/EcalObjects/interface/EcalPedestals.h"
#include "CondFormats/EcalObjects/interface/EcalTimeCalibConstants.h"

#include "CondFormats/DataRecord/interface/EcalADCToGeVConstantRcd.h"
#include "CondFormats/DataRecord/interface/EcalChannelStatusRcd.h"
#include "CondFormats/DataRecord/interface/EcalGainRatiosRcd.h"
#include "CondFormats/DataRecord/interface/EcalIntercalibConstantsMCRcd.h"
#include "CondFormats/DataRecord/interface/EcalIntercalibConstantsRcd.h"
#include "CondFormats/DataRecord/interface/EcalLaserAPDPNRatiosRcd.h"
#include "CondFormats/DataRecord/interface/EcalLaserAPDPNRatiosRefRcd.h"
#include "CondFormats/DataRecord/interface/EcalLaserAlphasRcd.h"
#include "CondFormats/DataRecord/interface/EcalPedestalsRcd.h"
#include "CondFormats/DataRecord/interface/EcalTimeCalibConstantsRcd.h"

#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbService.h"
#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbRecord.h"

#include "DataFormats/METReco/interface/PFMET.h"

// HLT trigger
#include "FWCore/Framework/interface/TriggerNamesService.h"
#include <FWCore/Common/interface/TriggerNames.h>
#include <DataFormats/Common/interface/TriggerResults.h>

//#define DEBUG

////////////////////////////////////////////////
// class declaration
//

class ZNtupleDumper : public edm::EDAnalyzer {
public:
  explicit ZNtupleDumper(const edm::ParameterSet&);
  ~ZNtupleDumper();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
  
private:
  virtual void beginJob() ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
  
  virtual void beginRun(edm::Run const&, edm::EventSetup const&);
  virtual void endRun(edm::Run const&, edm::EventSetup const&);
  virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
  virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
  
  // ----------member data ---------------------------
private:
  const edm::Event *pEvent;
  const edm::EventSetup *pSetup;
  
  bool isMC;
  bool isWenu;

  //Handles and inputTags
private:
  //------------------------------ Handles
  edm::Handle<std::vector<pat::Electron> > electronsHandle;
  edm::Handle<reco::BeamSpot> bsHandle;
  edm::Handle<reco::VertexCollection> primaryVertexHandle;
  edm::ESHandle<CaloTopology> topologyHandle;
  edm::Handle<double> rhoHandle;
  edm::Handle<std::vector< PileupSummaryInfo > >  PupInfo;
  edm::Handle<reco::ConversionCollection> conversionsHandle;
  edm::Handle<reco::PFMET> metHandle;
  edm::Handle<edm::TriggerResults> triggerResultsHandle;

  //------------------------------ Input Tags
  // input tag for primary vertex
  edm::InputTag vtxCollectionTAG;
  edm::InputTag BeamSpotTAG;
  // input tag for electrons
  edm::InputTag electronsTAG;
  edm::InputTag ZCandidateTAG;
  edm::InputTag recHitCollectionEBTAG;
  edm::InputTag recHitCollectionEETAG;
  // input rho
  edm::InputTag rhoTAG;
  edm::InputTag conversionsProducerTAG;
  edm::InputTag metTAG;
  edm::InputTag triggerResultsTAG;
private:
  std::string foutName;
  std::string regrPhoFile;
  std::string regrEleFile;
  
  bool doExtraCalibTree;
  bool doEleIDTree;
  //TString r9weightsFilename;
  //std::string puWeightFile;
  //  puWeights_class puWeights;

  edm::Service<TFileService> fs; //< output file for extra ntuples
  TTree *tree;                   //< output file for standard ntuple

// ntuple members, private to make them visible in doxygen
private: 
  
  Int_t     	runNumber;   ///< 
  Long64_t      eventNumber; ///<
  Int_t         lumiBlock;   ///< lumi section
  UInt_t 	runTime;     ///< unix time

  std::vector< std::string > HLTNames[1]; ///< List of HLT names
  std::vector<Bool_t> HLTResults[1];      ///< 0=fail, 1=fire

  //pileup
  Float_t rho;            ///< rho fast jet
  Int_t   nPV;            ///< nVtx
  Int_t   nPU[5];   //[nBX]   ///< number of PU (filled only for MC)

  // selection
  Int_t eleID[2];        ///< bit mask for eleID: 1=fiducial, 2=WP90, 3=WP90+fiducial, 6=WP80, 7=WP80+fiducial, 14=WP70, 15=WP70+fiducial

  Int_t  chargeEle[2];
  Float_t etaSCEle[2], phiSCEle[2]; ///< phi of the SC
  Float_t   etaEle[2],   phiEle[2]; ///< phi of the electron (electron object)

  Int_t recoFlagsEle[2];           ///< 1=trackerDriven, 2=ecalDriven only, 3=tracker and ecal driven

  Float_t PtEle[2];
  Int_t   classificationEle[2];   ///< electron classification in GOLD, SHOWERING, etc.

  Float_t fbremEle[2];

  Float_t seedXSCEle[2];
  Float_t seedYSCEle[2];
  Float_t seedEnergySCEle[2];
  Float_t seedLCSCEle[2];

  Float_t avgLCSCEle[2];

  Float_t energyMCEle[2];    ///< Electron MC true energy
  Float_t energySCEle[2];    ///< corrected SuperCluster energy
  Float_t rawEnergySCEle[2]; ///< SC energy without cluster corrections
  Float_t esEnergySCEle[2];  ///< pre-shower energy associated to the electron

  Float_t energySCEle_regrCorr_ele[2]; ///< SC energy based on MVA (target=SuperCluster raw energy/true energy): tuned on electrons (W MC sample)
  Float_t energySCEle_regrCorr_pho[2]; ///< SC energy based on MVA (target=SuperCluster raw energy/true energy): tuned on photons (di-photon MC sample)

  Float_t energyEle_regrCorr_fra[2];
  Float_t energySigmaEle_regrCorr_fra[2];

  Float_t energySigmaSCEle_regrCorr_ele[2];
  Float_t energySigmaSCEle_regrCorr_pho[2];

  Float_t energySCEle_corr[2];  ///< ecal energy with corrections base on type of electron (see #classificationEle)

  Float_t energyEle_regrCorr_egamma[2];      ///< Egamma POG electron regression energy
  Float_t energySigmaEle_regrCorr_egamma[2]; ///< Egamma POG electron regression energy uncertainty

  //  Float_t pool_regrCorr_ele;

  Float_t e3x3SCEle[2];   //< sum of the recHit energy in 3x3 matrix centered at the seed of the SC
  Float_t e5x5SCEle[2];   ///< sum of the recHit energy in 5x5 matrix centered at the seed of the SC
  Float_t eSeedSCEle[2]; 
  Float_t pModeGsfEle[2];
  Float_t pAtVtxGsfEle[2];
  Float_t R9Ele[2];      ///< e3x3/rawEnergySCEle

  Float_t invMass;
  Float_t invMass_SC;
  //   Float_t invMass_e3x3;
  Float_t invMass_e5x5;
  Float_t invMass_rawSC;
  Float_t invMass_rawSC_esSC;
  Float_t invMass_SC_corr;
  Float_t invMass_SC_regrCorr_ele;
  Float_t invMass_SC_regrCorr_pho;
  Float_t invMass_regrCorr_fra;
  Float_t invMass_regrCorr_egamma;

  Float_t invMass_MC;
  Float_t   etaMCEle[2], phiMCEle[2];


#ifdef shervin
  Float_t invMass_inGsf;
  Float_t invMass_outGsf;
  Float_t invMass_SC_etaphiSC;
  Int_t           nBX;
  Int_t           bxPU[5];   //[nBX]
  Int_t tagProbe_check; 
  Int_t nBCSCEle[2];
#endif

  //============================== ExtraCalibTree
  TTree *extraCalibTree;
  edm::Timestamp runTime_;
  Int_t nRecHitsEle[2];
  Int_t nHitsSCEle[2];
  std::vector<int> recoFlagRecHitSCEle[2];
  std::vector<float>  rawIdRecHitSCEle[2];
  std::vector<float>      XRecHitSCEle[2];
  std::vector<float>      YRecHitSCEle[2];
  std::vector<float> energyRecHitSCEle[2];
  std::vector<float>     LCRecHitSCEle[2];
  std::vector<float>     ICRecHitSCEle[2];
  std::vector<float>  AlphaRecHitSCEle[2];
  //==============================

  //============================== check ele-id and iso
  TTree *eleIDTree;
  Float_t sigmaIEtaIEtaSCEle[2];
  Float_t sigmaIPhiIPhiSCEle[2];
  Float_t hOverE[2];
  Float_t dr03TkSumPt[2];
  Float_t dr03EcalRecHitSumEt[2];
  Float_t dr03HcalTowerSumEt[2];
  Float_t deltaPhiSuperClusterTrackAtVtx[2];
  Float_t deltaEtaSuperClusterTrackAtVtx[2];
  Bool_t hasMatchedConversion[2];
  Int_t maxNumberOfExpectedMissingHits[2];
  Float_t pfMVA[2];
  //==============================

private:
  TFile *tree_file;
  void InitNewTree(void);

  void TreeSetDiElectronVar(pat::CompositeCandidate ZEE); 
  void TreeSetSingleElectronVar(const pat::Electron& electron1, int index);
  void TreeSetDiElectronVar(const pat::Electron& electron1, const pat::Electron& electron2);

  void InitExtraCalibTree(void);
  void TreeSetExtraCalibVar(const pat::Electron& electron1, int index);
  void TreeSetExtraCalibVar(const pat::Electron& electron1, const pat::Electron& electron2);

  void InitEleIDTree(void);

  void TreeSetEleIDVar(const pat::Electron& electron1, int index);
  void TreeSetEleIDVar(const pat::Electron& electron1, const pat::Electron& electron2);

  //  void Tree_output(TString filename);
  void TreeSetEventSummaryVar(const edm::Event& iEvent);
  void TreeSetPileupVar(void);

  //puWeights_class puWeights;

private:

  // --------------- selection cuts
private:

  Long64_t nentries;

  Long64_t epsilonected;


  //double r9Weight(double etaEle_, double R9Ele_);

  //------------------------------
  // cluster tools
  EcalClusterLazyTools *clustertools;
  //  EcalClusterLocal _ecalLocal;



  //
  // static data member definitions
  //
};


//
// constructors and destructor
//
ZNtupleDumper::ZNtupleDumper(const edm::ParameterSet& iConfig):
  //  isMC(iConfig.getParameter<bool>("isMC")), 
  isWenu(iConfig.getParameter<bool>("isWenu")),
  vtxCollectionTAG(iConfig.getParameter<edm::InputTag>("vertexCollection")),
  BeamSpotTAG(iConfig.getParameter<edm::InputTag>("BeamSpotCollection")),
  electronsTAG(iConfig.getParameter<edm::InputTag>("electronCollection")),
  //  ZCandidateTAG(iConfig.getParameter<edm::InputTag>("ZCandidateCollection")),
  recHitCollectionEBTAG(iConfig.getParameter<edm::InputTag>("recHitCollectionEB")),
  recHitCollectionEETAG(iConfig.getParameter<edm::InputTag>("recHitCollectionEE")),
  rhoTAG(iConfig.getParameter<edm::InputTag>("rhoFastJet")),
  conversionsProducerTAG(iConfig.getParameter<edm::InputTag>("conversionCollection")),
  metTAG(iConfig.getParameter<edm::InputTag>("metCollection")),
  triggerResultsTAG(iConfig.getParameter<edm::InputTag>("triggerResultsCollection")),
  foutName(iConfig.getParameter<std::string>("foutName")),
  doExtraCalibTree(iConfig.getParameter<bool>("doExtraCalibTree")),
  doEleIDTree(iConfig.getParameter<bool>("doEleIDTree"))
			    //  r9weightsFilename(iConfig.getParameter<std::string>("r9weightsFile")),
			    //puWeightFile(iConfig.getParameter<std::string>("puWeightFile")),
			    //puWeights()
{
  
  //if(isMC && puWeightFile.size()>1) puWeights.ReadFromFile(puWeightFile);




  //  if(isMC) puWeights.ReadFromFile(puWeightFile);



  //  current_dir.cd();


}

ZNtupleDumper::~ZNtupleDumper()
{
 
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  
  std::cout << "[STATUS] Calling the destructor" << std::endl;
}


//
// member functions
//

// ------------ method called for each event  ------------
void ZNtupleDumper::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  //  using namespace edm;
 
  pEvent = &iEvent;
  pSetup = &iSetup;
  
  
  nentries++;
  // filling infos runNumber, eventNumber, lumi
  if( !iEvent.isRealData() ){
    iEvent.getByLabel(edm::InputTag("addPileupInfo"), PupInfo);
    isMC=true;
  } else isMC=false;
  
  //------------------------------ RECHIT
  //iEvent.getByLabel(recHitCollectionEBTAG, recHitCollectionEBHandle);
  //iEvent.getByLabel(recHitCollectionEETAG, recHitCollectionEEHandle);
   
  //------------------------------ CONVERSIONS
  iEvent.getByLabel(conversionsProducerTAG, conversionsHandle);

  //------------------------------ HLT
#ifdef TRIGGER
  iEvent.getByLabel(triggerResultsTAG, triggerResultsHandle);
#endif

  clustertools = new EcalClusterLazyTools (iEvent, iSetup, recHitCollectionEBTAG, 
					   recHitCollectionEETAG);  
   

  // Handle to the Zee collection
  //  edm::Handle<pat::CompositeCandidateCollection > ZCandidatesHandle;
  //  iEvent.getByLabel(ZCandidateTAG, ZCandidatesHandle);
  iEvent.getByLabel(electronsTAG, electronsHandle);

  // for conversions with full vertex fit
  //------------------------------  VERTEX
  iEvent.getByLabel(vtxCollectionTAG, primaryVertexHandle); 
  iEvent.getByLabel(BeamSpotTAG, bsHandle);
  iEvent.getByLabel(rhoTAG,rhoHandle);
  
  iEvent.getByLabel(metTAG, metHandle); 
  
  TreeSetEventSummaryVar(iEvent);
  TreeSetPileupVar(); // this can be filled once per event


  //  pat::CompositeCandidateCollection ZCandidatesCollection;
  
  // count electrons
  int nWP70 = 0; //only WP70
  int nWP90 = 0; //only WP90
  for( pat::ElectronCollection::const_iterator eleIter1 = electronsHandle->begin();
       eleIter1 != electronsHandle->end();
       eleIter1++){
    if( eleIter1->electronID("WP70PU") )
      {
	++nWP70;
	continue;
      }
    if( eleIter1->electronID("WP90PU") )
      {
	++nWP90;
	continue;
      }
  }

  for( pat::ElectronCollection::const_iterator eleIter1 = electronsHandle->begin();
       eleIter1 != electronsHandle->end();
       eleIter1++){
    if(! eleIter1->electronID("WP90PU") ) continue;
    if(isWenu){
      if(! eleIter1->electronID("WP70PU") ) continue;
      if( nWP70 != 1 || nWP90 > 0 ) continue; //to be a Wenu event request only 1 ele WP70 in the event
      
      // MET/MT selection
      // manca la met?!? -> ho aggiunto metHandle
      
      TreeSetDiElectronVar(*eleIter1, *eleIter1);
      tree->Fill();
      if(doExtraCalibTree){
	TreeSetExtraCalibVar(*eleIter1, *eleIter1);
	extraCalibTree->Fill();
      }
      if(doEleIDTree){
	TreeSetEleIDVar(*eleIter1, *eleIter1);
	eleIDTree->Fill();
      }
    } else {
      for(pat::ElectronCollection::const_iterator eleIter2 = eleIter1+1;
	  eleIter2 != electronsHandle->end();
	  eleIter2++){
	// should exit when eleIter1 == end-1
	if(! eleIter2->electronID("WP90PU") ) continue;
	
	//pat::CompositeCandidate zeeCandidate;
	//if(! (eleIter1->electronID("WP90PU") && eleIter1->electronID("fiducial"))) continue;
	//if(! (eleIter2->electronID("WP90PU") && eleIter2->electronID("fiducial"))) continue;
	//zeeCandidate.addDaughter(*eleIter1, "electron1");
	//zeeCandidate.addDaughter(*eleIter2, "electron2");
	float mass=(eleIter1->p4()+eleIter2->p4()).mass();
	if(mass < 55 || mass > 125) continue;
      
	//       ZCandidatesCollection.push_back(zeeCandidate);
	TreeSetDiElectronVar(*eleIter1, *eleIter2);
	tree->Fill();
	if(doExtraCalibTree){
	  TreeSetExtraCalibVar(*eleIter1, *eleIter2);
	  extraCalibTree->Fill();
	}
	if(doEleIDTree){
	  TreeSetEleIDVar(*eleIter1, *eleIter2);
	  eleIDTree->Fill();
	}
      }
    }
  }
  //   //     // riempi il tree degli eventi selezionati
  //   std::cout << "N Zcandidates: " <<   ZCandidatesCollection->size() << std::endl;
  //   for( pat::CompositeCandidateCollection::const_iterator ZIter = ZCandidatesCollection.begin();
  //       ZIter != ZCandidatesCollection.end();
  //       ZIter++){
  //     std::cout << "Zcandidate: " << ZIter-ZCandidatesCollection.begin() << std::endl;
  //     TreeSetDiElectronVar(*ZIter); 
  //     tree->Fill();
  
  //   }

  delete clustertools;
  return;

}


// ------------ method called once each job just before starting event loop  ------------
void ZNtupleDumper::beginJob()
{
#ifdef DEBUG
  std::cout << "[DEBUG] Starting creation of ntuples" << std::endl; 
#endif

  tree_file = new TFile(foutName.c_str(), "recreate");
  if(tree_file->IsZombie()){
    throw cms::Exception("OutputError") <<  "Output tree not created (Zombie): " << foutName;
    return;
  }
  tree_file->cd();
  
  //now do what ever initialization is needed
  tree = new TTree("selected","selected");
  //tree = fs->make<TTree>("selected","selected"); //no otherwise you have the extraCalib in the same file
  
  tree->SetDirectory(tree_file);
  // controllo su tree==NULL
  
  InitNewTree();  // inizializzo il tree dell'ntupla ridotta selezionata
  
  if(doExtraCalibTree){
    extraCalibTree = fs->make<TTree>("extraCalibTree","");
    InitExtraCalibTree();
  }
  if(doEleIDTree){
    eleIDTree = fs->make<TTree>("eleIDTree","");
    InitEleIDTree();
  }
  
#ifdef DEBUG
  std::cout << "[DEBUG] End creation of ntuples" << std::endl; 
#endif
}

// ------------ method called once each job just after ending the event loop  ------------
void ZNtupleDumper::endJob() 
{
  
  if(tree->GetEntries()>0){
    tree->BuildIndex("runNumber","eventNumber");
    if(doExtraCalibTree)  extraCalibTree->BuildIndex("runNumber","eventNumber");
    if(doEleIDTree)       eleIDTree->BuildIndex("runNumber","eventNumber");
  }
  // save the tree into the file
  tree_file->cd();
  tree->Write();
  tree_file->Close();
  
  
}

// ------------ method called when starting to processes a run  ------------
void ZNtupleDumper::beginRun(const edm::Run& iRun, edm::EventSetup const& iSetup)
{
}

// ------------ method called when ending the processing of a run  ------------
void ZNtupleDumper::endRun(edm::Run const&, edm::EventSetup const&)
{

}

// ------------ method called when starting to processes a luminosity block  ------------
void ZNtupleDumper::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{

  

}

// ------------ method called when ending the processing of a luminosity block  ------------
void ZNtupleDumper::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void ZNtupleDumper::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}



// output tree initialization
void ZNtupleDumper::InitNewTree(){
  
  //  tree = new TTree("selected",fChain->GetTitle());
  std::cout << "[STATUS] InitNewTree" << std::endl;
  if(tree==NULL) return;
  tree->Branch("runNumber",     &runNumber,     "runNumber/I");
  tree->Branch("eventNumber",   &eventNumber, "eventNumber/l");
  tree->Branch("lumiBlock",     &lumiBlock,     "lumiBlock/I");
  tree->Branch("runTime",       &runTime,         "runTime/i");

  tree->Branch("HLTNames",&(HLTNames[0]));
  tree->Branch("HLTResults",&(HLTResults[0]));
  //extraCalibTree->Branch("XRecHitSCEle1", &(XRecHitSCEle[0]));

  //   tree->Branch("nBX", &nBX, "nBX/I");
  //  tree->Branch("nPU", nPU, "nPU[nBX]/I");
  tree->Branch("nPU", nPU, "nPU[1]/I");
  //   tree->Branch("bxPU", bxPU, "bxPU[nBX]/I");
  tree->Branch("rho", &rho, "rho/F");

  tree->Branch("nPV", &nPV, "nPV/I");

  tree->Branch("eleID",eleID, "eleID[2]/I");
  //  tree->Branch("nBCSCEle", nBCSCEle, "nBCSCEle[2]/I");

  tree->Branch("chargeEle",   chargeEle,    "chargeEle[2]/I");	//[nEle]
  tree->Branch("etaSCEle",      etaSCEle,       "etaSCEle[2]/F");	//[nSCEle]
  tree->Branch("phiSCEle",      phiSCEle,       "phiSCEle[2]/F");	//[nSCEle]

  tree->Branch("etaEle",      etaEle,       "etaEle[2]/F");	//[nEle]
  tree->Branch("phiEle",      phiEle,       "phiEle[2]/F");	//[nEle]

  tree->Branch("classificationEle",      classificationEle,       "classificationEle[2]/I");	//[nEle]
  tree->Branch("recoFlagsEle",recoFlagsEle, "recoFlagsEle[2]/I");	//[nEle]
  tree->Branch("PtEle",       PtEle,        "PtEle[2]/F");
  tree->Branch("fbremEle",    fbremEle,     "fbremEle[2]/F");

  tree->Branch("seedXSCEle",           seedXSCEle,      "seedXSCEle[2]/F");
  tree->Branch("seedYSCEle",           seedYSCEle,      "seedYSCEle[2]/F");
  tree->Branch("seedEnergySCEle", seedEnergySCEle, "seedEnergySCEle[2]/F");
  tree->Branch("seedLCSCEle",         seedLCSCEle,     "seedLCSCEle[2]/F");

  tree->Branch("avgLCSCEle", avgLCSCEle, "avgLCSCEle[2]/F");

  tree->Branch("energyMCEle", energyMCEle, "energyMCEle[2]/F");
  tree->Branch("energySCEle", energySCEle, "energySCEle[2]/F");
  tree->Branch("rawEnergySCEle", rawEnergySCEle, "rawEnergySCEle[2]/F");
  tree->Branch("esEnergySCEle", esEnergySCEle, "esEnergySCEle[2]/F");

  tree->Branch("energySCEle_corr", energySCEle_corr, "energySCEle_corr[2]/F");

  tree->Branch("energySCEle_regrCorr_ele", energySCEle_regrCorr_ele, "energySCEle_regrCorr_ele[2]/F");
  tree->Branch("energySCEle_regrCorr_pho", energySCEle_regrCorr_pho, "energySCEle_regrCorr_pho[2]/F");
  tree->Branch("energyEle_regrCorr_fra", energyEle_regrCorr_fra, "energyEle_regrCorr_fra[2]/F");
  tree->Branch("energySigmaEle_regrCorr_fra", energySigmaEle_regrCorr_fra, "energySigmaEle_regrCorr_fra[2]/F");

  tree->Branch("energyEle_regrCorr_egamma", energyEle_regrCorr_egamma, "energyEle_regrCorr_egamma[2]/F");
  tree->Branch("energySigmaEle_regrCorr_egamma", energySigmaEle_regrCorr_egamma, "energySigmaEle_regrCorr_egamma[2]/F");

  tree->Branch("energySigmaSCEle_regrCorr_ele", energySigmaSCEle_regrCorr_ele, "energySigmaSCEle_regrCorr_ele[2]/F");
  tree->Branch("energySigmaSCEle_regrCorr_pho", energySigmaSCEle_regrCorr_pho, "energySigmaSCEle_regrCorr_pho[2]/F");

  tree->Branch("R9Ele", R9Ele, "R9Ele[2]/F");

  tree->Branch("e5x5SCEle", e5x5SCEle, "e5x5SCEle[2]/F");
  tree->Branch("eSeedSCEle", eSeedSCEle, "eSeedSCEle[2]/F");
  tree->Branch("pModeGsfEle", pModeGsfEle, "pModeGsfEle[2]/F");
  tree->Branch("pAtVtxGsfEle", pAtVtxGsfEle, "pAtVtxGsfEle[2]/F");

  tree->Branch("invMass",    &invMass,      "invMass/F");  
  tree->Branch("invMass_SC", &invMass_SC,   "invMass_SC/F");
  //   tree->Branch("invMass_e3x3",    &invMass_e3x3,      "invMass_e3x3/F");
  tree->Branch("invMass_e5x5",    &invMass_e5x5,      "invMass_e5x5/F");
  tree->Branch("invMass_rawSC", &invMass_rawSC,   "invMass_rawSC/F");
  tree->Branch("invMass_rawSC_esSC", &invMass_rawSC_esSC,   "invMass_rawSC_esSC/F");

  tree->Branch("invMass_SC_corr", &invMass_SC_corr, "invMass_SC_corr/F");
  tree->Branch("invMass_SC_regrCorr_ele", &invMass_SC_regrCorr_ele, "invMass_SC_regrCorr_ele/F");
  tree->Branch("invMass_SC_regrCorr_pho", &invMass_SC_regrCorr_pho, "invMass_SC_regrCorr_pho/F");
  tree->Branch("invMass_regrCorr_fra", &invMass_regrCorr_fra, "invMass_regrCorr_fra/F");
  tree->Branch("invMass_regrCorr_egamma", &invMass_regrCorr_egamma, "invMass_regrCorr_egamma/F");
  tree->Branch("invMass_MC", &invMass_MC, "invMass_MC/F");

  tree->Branch("etaMCEle",      etaMCEle,       "etaMCEle[2]/F");	//[nEle]
  tree->Branch("phiMCEle",      phiMCEle,       "phiMCEle[2]/F");	//[nEle]

  //tree->Branch("weight", &weight,  "weight/F");     
  //tree->Branch("r9weight", r9weight,  "r9weight[2]/F");     


  //tree->Branch("iSMEle", iSMEle, "iSMEle[2]/I");
 
  tree->Branch("nHitsSCEle", nHitsSCEle, "nHitsSCEle[2]/I");

#ifdef shervin
  tree->Branch("sigmaIEtaIEtaSCEle", sigmaIEtaIEtaSCEle, "sigmaIEtaIEtaSCEle[2]/F");
  tree->Branch("sigmaIEtaIEtaSCEle", sigmaIEtaIEtaSCEle, "sigmaIEtaIEtaSCEle[2]/F");
  tree->Branch("sigmaIPhiIPhiSCEle", sigmaIPhiIPhiSCEle, "sigmaIPhiIPhiSCEle[2]/F");

  tree->Branch("invMass_SC_etaphiSC", &invMass_SC_etaphiSC, "invMass_SC_etaphiSC/F");
  //  tree->Branch("invMass_inGsf", &invMass_inGsf,   "invMass_inGsf/F");
  //    tree->Branch("invMass_outGsf", &invMass_outGsf,   "invMass_outGsf/F");
  //  }
  //if(tagProbe) tree->Branch("tagProbe_check", &tagProbe_check, "tagProbe_check/I");
#endif



  return;
}



void ZNtupleDumper::TreeSetEventSummaryVar(const edm::Event& iEvent){
  
  runTime_   = iEvent.eventAuxiliary().time();
  runTime = runTime_.unixTime();
  runNumber = iEvent.id().run();
  eventNumber = iEvent.id().event();
  if( iEvent.isRealData() ) {
    lumiBlock = iEvent.luminosityBlock();
  } else {
    lumiBlock = -1;
  }

#ifdef TRIGGER
  edm::TriggerNames HLTNames_ = iEvent.triggerNames(*triggerResultsHandle);
  int hltCount = triggerResultsHandle->size();
  HLTNames[0].clear();
  for (int i = 0 ; i != hltCount; ++i) {
    std::string hltName_str(HLTNames_.triggerName(i));
    (HLTNames[0]).push_back(hltName_str);
    HLTResults->push_back(triggerResultsHandle->accept(i));
  } // for i
#endif

  return;
}    


void ZNtupleDumper::TreeSetPileupVar(void){
  rho=*rhoHandle;
  nPV=0;
  if(primaryVertexHandle->size() > 0) {
    for(reco::VertexCollection::const_iterator v = primaryVertexHandle->begin();
	v != primaryVertexHandle->end(); ++v){
      if((*v).tracksSize() > 0) nPV++; // non mi ricordo perche' ho fatto cosi'....
    }
  }

  if(isMC){
    std::vector<PileupSummaryInfo>::const_iterator PVI;
    for(PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI) {
      int BX = PVI->getBunchCrossing();
      if(BX == 0) { // in-time pu
	//weight = puWeights.GetWeight( PVI->getPU_NumInteractions());
	//weight = puWeights.GetWeight( PVI->getTrueNumInteractions());
	//weight = 1.;
	nPU[0]=PVI->getPU_NumInteractions();
      }
    }
  } else {
    //weight= 1.;
    nPU[0]=-1;
  }
  return;
}


void ZNtupleDumper::TreeSetDiElectronVar(pat::CompositeCandidate ZEE){ 


  const  pat::Electron *electron1_p=dynamic_cast<const pat::Electron*>(&(*(ZEE.begin())));
  const  pat::Electron *electron2_p=dynamic_cast<const pat::Electron*>(&(*(ZEE.begin()+1)));

  const  pat::Electron& electron1=*electron1_p;
  const  pat::Electron& electron2=*electron2_p;

  TreeSetDiElectronVar(electron1, electron2);
  return;
}


// a negative index means that the corresponding electron does not exist, fill with 0
void ZNtupleDumper::TreeSetSingleElectronVar(const pat::Electron& electron1, int index){

  if(index<0){
    PtEle[-index] 	  = 0;  
    chargeEle[-index] = 0;
    etaEle[-index]    = 0; 
    phiEle[-index]    = 0;
    return;
  }   

  PtEle[index]     = electron1.et();  
  chargeEle[index] = electron1.charge();
  etaEle[index]    = electron1.eta(); // degli elettroni
  phiEle[index]    = electron1.phi();

  if(electron1.ecalDrivenSeed()){
    if(electron1.trackerDrivenSeed()) recoFlagsEle[index]=3;
    else recoFlagsEle[index] = 2;
  } else recoFlagsEle[index] = 1;


  fbremEle[index] = electron1.fbrem(); // fatta con pIn-pOut

  etaSCEle[index] = electron1.superCluster()->eta();
  phiSCEle[index] = electron1.superCluster()->phi();

  const EcalRecHitCollection *recHits = (electron1.isEB()) ?  clustertools->getEcalEBRecHitCollection() : clustertools->getEcalEERecHitCollection();
  const edm::ESHandle<EcalLaserDbService>& laserHandle_ = clustertools->getLaserHandle();
  
  if(electron1.isEB()){
    EBDetId seedDetId = electron1.superCluster()->seed()->seed();
    EcalRecHitCollection::const_iterator seedRecHit = recHits->find(seedDetId) ;
    seedXSCEle[index]=seedDetId.ieta();
    seedYSCEle[index]=seedDetId.iphi();
    seedEnergySCEle[index]=seedRecHit->energy();
    if(true || isMC) seedLCSCEle[index]=-10;
    else seedLCSCEle[index]=laserHandle_->getLaserCorrection(seedDetId,runTime_);
  } else {
    EEDetId seedDetId = electron1.superCluster()->seed()->seed();
    EcalRecHitCollection::const_iterator seedRecHit = recHits->find(seedDetId) ;
    seedXSCEle[index]=(EEDetId)(seedDetId).ix();
    seedYSCEle[index]=(EEDetId)(seedDetId).iy();
    seedEnergySCEle[index]=seedRecHit->energy();
    if(true ||isMC) seedLCSCEle[index]=-10;
    else seedLCSCEle[index]=laserHandle_->getLaserCorrection(seedDetId,runTime_);
  }
  
  float sumLC_E = 0.;
  float sumE = 0.;
  if(true || !isMC){
    std::vector< std::pair<DetId, float> > hitsAndFractions_ele1 = electron1.superCluster()->hitsAndFractions();
    for (std::vector<std::pair<DetId, float> >::const_iterator detitr = hitsAndFractions_ele1.begin();
	 detitr != hitsAndFractions_ele1.end(); detitr++ )
      {
	EcalRecHitCollection::const_iterator oneHit = recHits->find( (detitr -> first) ) ;
	sumLC_E += laserHandle_->getLaserCorrection(detitr->first, runTime_) * oneHit->energy();
	sumE    += oneHit->energy();
      }
    avgLCSCEle[index] = sumLC_E / sumE;
  } else     avgLCSCEle[index] = -10;
  
  nHitsSCEle[index] = electron1.superCluster()->size();

  //  sigmaIEtaIEtaSCEle[index] = sqrt(clustertools->localCovariances(*(electron1.superCluster()->seed()))[index]);
  //  sigmaIEtaIEtaSCEle[1] = sqrt(clustertools->localCovariances(*(electron2.superCluster()->seed()))[index]);
  //  sigmaIPhiIPhiBC = sqrt(clustertools->localCovariances(*b)[2]);
  //  sigmaIEtaIPhiBC = clustertools->localCovariances(*b)[1]; 

  if(electron1.genLepton()!=0){
    energyMCEle[index]  = electron1.genLepton()->energy();
    etaMCEle[index]  = electron1.genLepton()->eta();
    phiMCEle[index]  = electron1.genLepton()->phi();
  } else {
    energyMCEle[index]=0;
    etaMCEle[index]=0;
    phiMCEle[index]=0;
  }

  energySCEle[index]  = electron1.superCluster()->energy();
  rawEnergySCEle[index]  = electron1.superCluster()->rawEnergy();
  esEnergySCEle[index] = electron1.superCluster()->preshowerEnergy();
#ifndef CMSSW42X
  energySCEle_corr[index] = electron1.correctedEcalEnergy();
#else
  energySCEle_corr[index] = electron1.ecalEnergy();
#endif

  energySCEle_regrCorr_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPho");
  energySCEle_regrCorr_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEle");
  energyEle_regrCorr_fra[index] = electron1.userFloat("eleNewEnergiesProducer:energyEleFra");
  energyEle_regrCorr_egamma[index] = electron1.userFloat("eleRegressionEnergy:eneRegForGsfEle");

  energySigmaSCEle_regrCorr_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoVar");
  energySigmaSCEle_regrCorr_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEleVar");
  energySigmaEle_regrCorr_fra[index] = electron1.userFloat("eleNewEnergiesProducer:energyEleFraVar");
  energySigmaEle_regrCorr_egamma[index] = electron1.userFloat("eleRegressionEnergy:eneErrorRegForGsfEle");

  // change in an electron properties please, EleNewEnergyProducer
  e3x3SCEle[index] = clustertools->e3x3(*electron1.superCluster()->seed());
  e5x5SCEle[index] = clustertools->e5x5(*electron1.superCluster()->seed());
  eSeedSCEle[index]= electron1.superCluster()->seed()->energy();
  pModeGsfEle[index] = electron1.gsfTrack()->pMode();
  pAtVtxGsfEle[index] = electron1.trackMomentumAtVtx().R();

  R9Ele[index] = e3x3SCEle[index]/electron1.superCluster()->rawEnergy();

  if(isMC){
    if(electron1.isEB()) 
      R9Ele[index] = R9Ele[index]*1.0045+0.0010;
    else 
      R9Ele[index] = R9Ele[index]*1.0086-0.0007;
  } 

  // make it a function
  eleID[index] = ((bool) electron1.electronID("fiducial")) << 0;
  eleID[index] += ((bool) electron1.electronID("WP90PU")) << 1;
  eleID[index] += ((bool) electron1.electronID("WP80PU")) << 2;
  eleID[index] += ((bool) electron1.electronID("WP70PU")) << 3;

  classificationEle[index] = electron1.classification();

  return;
}


void ZNtupleDumper:: TreeSetDiElectronVar(const pat::Electron& electron1, const pat::Electron& electron2){
  
  TreeSetSingleElectronVar(electron1, 0);
  if(isWenu) TreeSetSingleElectronVar(electron2, -1); // negative index: do not fill, fill with 0
  else{
    TreeSetSingleElectronVar(electron2, 1); 

    double t1=TMath::Exp(-etaEle[0]);
    double t2=TMath::Exp(-etaEle[1]);
    double t1q = t1*t1;
    double t2q = t2*t2;
  
    double angle=1-
      ( (1-t1q)*(1-t2q)+4*t1*t2*cos(phiEle[0]-phiEle[1]))/(
								   (1+t1q)*(1+t2q)
								   );


    invMass = sqrt(2*electron1.energy()*electron2.energy() *angle);
    invMass_e5x5   = sqrt(2*electron1.e5x5()*electron2.e5x5() *
			      angle);

    invMass_SC = sqrt(2*energySCEle[0]*energySCEle[1] *
			  angle);


    invMass_rawSC = sqrt(2 * rawEnergySCEle[0] * rawEnergySCEle[1] *
			     angle);


    invMass_rawSC_esSC = sqrt(2 * (rawEnergySCEle[0] + esEnergySCEle[0]) * 
				  (rawEnergySCEle[1] + esEnergySCEle[1]) * 
				  angle);

    invMass_SC_corr = sqrt(2*energySCEle_corr[0]*energySCEle_corr[1] *
			       angle);

    invMass_SC_regrCorr_ele = sqrt(2* energySCEle_regrCorr_ele[0] * energySCEle_regrCorr_ele[1] *
				       angle);
    invMass_SC_regrCorr_pho = sqrt(2* energySCEle_regrCorr_pho[0] * energySCEle_regrCorr_pho[1] *
				       angle);

    invMass_regrCorr_fra = sqrt(2* energyEle_regrCorr_fra[0] * energyEle_regrCorr_fra[1] *angle);
    invMass_regrCorr_egamma = sqrt(2* energyEle_regrCorr_egamma[0] * energyEle_regrCorr_egamma[1] *angle);

    if(electron1.genLepton()!=0 && electron2.genLepton()!=0){
      invMass_MC     = sqrt(2*electron1.genLepton()->energy() *electron1.genLepton()->energy() *
				angle);
    } else invMass_MC = 0;
    //  invMass_genMC     = (electron1.genLepton()->p4 + electron2.genLepton()->p4()).M();
			    

    // se non hanno fatto match con il MC?
    // qual e' la frazione di Z selezionate che non matchano il MC?

    //#ifdef shervin
    //  r9weight[0]=r9Weight(etaEle[0], R9Ele[0]);
    //  r9weight[1]=r9Weight(etaEle[1], R9Ele[1]);
    //#endif
  }
  
  return;
}



//#============================== extra calib tree
void ZNtupleDumper::InitExtraCalibTree(){
  
  //  tree = new TTree("selected",fChain->GetTitle());
  std::cout << "[STATUS] InitExtraCalibTree" << std::endl;
  if(extraCalibTree==NULL) return;
  
  extraCalibTree->Branch("runNumber",     &runNumber,     "runNumber/I");
  extraCalibTree->Branch("eventNumber",   &eventNumber, "eventNumber/l");
  extraCalibTree->Branch("lumiBlock",     &lumiBlock,     "lumiBlock/I");
  extraCalibTree->Branch("runTime",       &runTime, "        runTime/l");
 
  extraCalibTree->Branch("nHitsSCEle", nHitsSCEle, "nHitsSCEle[2]/I");

  extraCalibTree->Branch("rawIdRecHitSCEle1", &(rawIdRecHitSCEle[0]));
  extraCalibTree->Branch("rawIdRecHitSCEle2", &(rawIdRecHitSCEle[1]));
  extraCalibTree->Branch("XRecHitSCEle1", &(XRecHitSCEle[0]));
  extraCalibTree->Branch("XRecHitSCEle2", &(XRecHitSCEle[1]));
  extraCalibTree->Branch("YRecHitSCEle1", &(YRecHitSCEle[0]));
  extraCalibTree->Branch("YRecHitSCEle2", &(YRecHitSCEle[1]));
  extraCalibTree->Branch("energyRecHitSCEle1", &(energyRecHitSCEle[0]));
  extraCalibTree->Branch("energyRecHitSCEle2", &(energyRecHitSCEle[1]));
  extraCalibTree->Branch("LCRecHitSCEle1", &(LCRecHitSCEle[0]));
  extraCalibTree->Branch("LCRecHitSCEle2", &(LCRecHitSCEle[1]));
  extraCalibTree->Branch("ICRecHitSCEle1", &(ICRecHitSCEle[0]));
  extraCalibTree->Branch("ICRecHitSCEle2", &(ICRecHitSCEle[1]));
  extraCalibTree->Branch("AlphaRecHitSCEle1", &(AlphaRecHitSCEle[0]));
  extraCalibTree->Branch("AlphaRecHitSCEle2", &(AlphaRecHitSCEle[1]));

  return;
}

void ZNtupleDumper::TreeSetExtraCalibVar(const pat::Electron& electron1, const pat::Electron& electron2){

  recoFlagRecHitSCEle[0].clear();
  rawIdRecHitSCEle[0].clear();
  XRecHitSCEle[0].clear();
  YRecHitSCEle[0].clear();
  energyRecHitSCEle[0].clear();
  LCRecHitSCEle[0].clear();
  ICRecHitSCEle[0].clear();
  AlphaRecHitSCEle[0].clear();

  recoFlagRecHitSCEle[1].clear();
  rawIdRecHitSCEle[1].clear();
  XRecHitSCEle[1].clear();
  YRecHitSCEle[1].clear();
  energyRecHitSCEle[1].clear();
  LCRecHitSCEle[1].clear();
  ICRecHitSCEle[1].clear();
  AlphaRecHitSCEle[1].clear();

  TreeSetExtraCalibVar(electron1, 0);
  if(!isWenu)   TreeSetExtraCalibVar(electron2, 1);
  return;
}

void ZNtupleDumper::TreeSetExtraCalibVar(const pat::Electron& electron1, int index){

  //  EcalIntercalibConstantMap icMap = icHandle->get()
  std::vector< std::pair<DetId, float> > hitsAndFractions_ele1 = electron1.superCluster()->hitsAndFractions();
  nHitsSCEle[index] = hitsAndFractions_ele1.size();
  
  const EcalRecHitCollection *recHits = (electron1.isEB()) ?  clustertools->getEcalEBRecHitCollection() : clustertools->getEcalEERecHitCollection();
  const EcalIntercalibConstantMap& icalMap = clustertools->getEcalIntercalibConstants();
  const edm::ESHandle<EcalLaserDbService>& laserHandle_ = clustertools->getLaserHandle();
  for (std::vector<std::pair<DetId, float> >::const_iterator detitr = hitsAndFractions_ele1.begin(); 
       detitr != hitsAndFractions_ele1.end(); detitr++ )
    {
      //      EcalRecHitCollection::const_iterator theSeedHit = recHits->find (id); // trash this
      EcalRecHitCollection::const_iterator oneHit = recHits->find( (detitr -> first) ) ;
      if(oneHit==recHits->end()){
	edm::LogError("ZNtupleDumper") << "No intercalib const found for xtal "  << (detitr->first).rawId() << "bailing out";
	assert(0);
      }
      recoFlagRecHitSCEle[index].push_back(oneHit->recoFlag());
      rawIdRecHitSCEle[index].push_back(detitr->first.rawId());
      if(electron1.isEB()){
        EBDetId recHitId(detitr->first);
	XRecHitSCEle[index].push_back(recHitId.ieta());
	YRecHitSCEle[index].push_back(recHitId.iphi());
      }
      else{
        EEDetId recHitId(detitr->first);
	XRecHitSCEle[index].push_back(recHitId.ix());
	YRecHitSCEle[index].push_back(recHitId.iy());
      }
      energyRecHitSCEle[index].push_back(oneHit->energy());
      // in order to get back the ADC counts from the recHit energy, three ingredients are necessary:
      // 1) get laser correction coefficient
      LCRecHitSCEle[index].push_back(laserHandle_->getLaserCorrection(detitr->first, runTime_));
      //laserHandle->
      // 2) get intercalibration
      EcalIntercalibConstantMap::const_iterator icalit = icalMap.find(detitr->first); 
      EcalIntercalibConstant icalconst = 1.;
      if( icalit!=icalMap.end() ) {
	icalconst = (*icalit);
	// std::cout << "icalconst set to: " << icalconst << std::endl;
      } else {
	edm::LogError("ZNtupleDumper") << "No intercalib const found for xtal "  << (detitr->first).rawId() << "bailing out";
	assert(0);
      }
      // 3) get adc2GeV
      //float adcToGeV = ( (detitr -> first).subdetId() == EcalBarrel ) ? 
      // float(adcToGeVHandle->getEBValue()) : float(adcToGeVHandle->getEEValue());
      ICRecHitSCEle[index].push_back(icalconst);
    }

  return;
}


//#============================== Ele ID tree
void ZNtupleDumper::InitEleIDTree(){
  
  //  tree = new TTree("selected",fChain->GetTitle());
  std::cout << "[STATUS] InitEleIDTree" << std::endl;
  if(eleIDTree==NULL) return;
  
  eleIDTree->Branch("runNumber",     &runNumber,     "runNumber/I");
  eleIDTree->Branch("eventNumber",   &eventNumber, "eventNumber/l");
  eleIDTree->Branch("lumiBlock",     &lumiBlock,     "lumiBlock/I");
  eleIDTree->Branch("runTime",       &runTime,         "runTime/l");
 
  eleIDTree->Branch("dr03TkSumPt", dr03TkSumPt, "dr03TkSumPt[2]/F");
  eleIDTree->Branch("dr03EcalRecHitSumEt", dr03EcalRecHitSumEt, "dr03EcalRecHitSumEt[2]/F");
  eleIDTree->Branch("dr03HcalTowerSumEt", dr03HcalTowerSumEt, "dr03HcalTowerSumEt[2]/F");
  eleIDTree->Branch("sigmaIEtaIEtaSCEle", sigmaIEtaIEtaSCEle, "sigmaIEtaIEtaSCEle[2]/F");
  //  eleIDTree->Branch("sigmaIPhiIPhiSCEle", sigmaIPhiIPhiSCEle, "sigmaIPhiIPhiSCEle[2]/F");
  eleIDTree->Branch("deltaEtaSuperClusterTrackAtVtx", deltaEtaSuperClusterTrackAtVtx, "deltaEtaSuperClusterTrackAtVtx[2]/F");
  eleIDTree->Branch("deltaPhiSuperClusterTrackAtVtx", deltaPhiSuperClusterTrackAtVtx, "deltaPhiSuperClusterTrackAtVtx[2]/F");
  eleIDTree->Branch("hOverE", hOverE, "hOverE[2]/F");
  eleIDTree->Branch("pfMVA", pfMVA, "pfMVA[2]/F");
  eleIDTree->Branch("hasMatchedConversion", hasMatchedConversion, "hasMatchedConversion[2]/b");
  eleIDTree->Branch("maxNumberOfExpectedMissingHits", maxNumberOfExpectedMissingHits, "maxNumberOfExpectedMissingHits[2]/I");
  return;
}


void ZNtupleDumper::TreeSetEleIDVar(const pat::Electron& electron1, const pat::Electron& electron2){

  TreeSetEleIDVar(electron1, 0);
  if(isWenu) TreeSetEleIDVar(electron1, -1);
  return;
}

void ZNtupleDumper::TreeSetEleIDVar(const pat::Electron& electron1, int index){
  if(index<0){
    hOverE[-index]=-1;
    return;
  }
  
  dr03TkSumPt[index]         = electron1.dr03TkSumPt();
  dr03EcalRecHitSumEt[index] = electron1.dr03EcalRecHitSumEt();
  dr03HcalTowerSumEt[index]  = electron1.dr03HcalTowerSumEt();
  sigmaIEtaIEtaSCEle[index]  = electron1.sigmaIetaIeta(); // alcarereco
  sigmaIPhiIPhiSCEle[index]  = electron1.sigmaIphiIphi(); // alcarereco
  deltaPhiSuperClusterTrackAtVtx[index]  = electron1.deltaPhiSuperClusterTrackAtVtx();
  deltaEtaSuperClusterTrackAtVtx[index]  = electron1.deltaEtaSuperClusterTrackAtVtx();
  hOverE[index] = electron1.hadronicOverEm();
  pfMVA[index]   = electron1.mva();
  hasMatchedConversion[index] = ConversionTools::hasMatchedConversion(electron1, conversionsHandle, bsHandle->position());
  maxNumberOfExpectedMissingHits[index] = electron1.gsfTrack()->trackerExpectedHitsInner().numberOfLostHits();
  
  return;
}



//define this as a plug-in
DEFINE_FWK_MODULE(ZNtupleDumper);
