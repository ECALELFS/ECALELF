// -*- C++ -*-
//
// Package:    ZNtupleDumper
// Class:      ZNtupleDumper
// 
/// Zee and E/p ntuple dumper from patElectrons
/**\class ZNtupleDumper ZNtupleDumper.cc Calibration/ZNtupleDumper/src/ZNtupleDumper.cc
 * 
 * Description: Zee and E/p ntuple dumper from patElectrons
 * 
 * \todo 
 - recHitCollection is included in the PAT electron collection, take it from there
 - take the R9 from the PAT electron: electron->r9()
 - flag for Zee or Wenu dump
 - Use MET corrections
   - https://twiki.cern.ch/twiki/bin/view/CMS/MissingET
   - https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMetAnalysis
   - https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMETRecipe53X
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
#include <regex>

// root include files
#include <TTree.h>
#include <TLorentzVector.h>
#include <Math/Vector4D.h>
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

#ifdef CMSSW_7_2_X
   #include "FWCore/Utilities/interface/EDGetToken.h"
#endif

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"

//#include "Calibration/ZNtupleDumper/interface/puWeights_class.hh"

#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"

#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/CaloRecHit/interface/CaloID.h"

#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "RecoEgamma/EgammaTools/interface/ConversionFinder.h"
#include "RecoEgamma/EgammaTools/interface/ConversionInfo.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"


#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
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
#include "DataFormats/METReco/interface/PFMETFwd.h"

// HLT trigger
#include "FWCore/Framework/interface/TriggerNamesService.h"
#include <FWCore/Common/interface/TriggerNames.h>
#include <DataFormats/Common/interface/TriggerResults.h>

// alcaSkimPaths
#include "DataFormats/Provenance/interface/ParameterSetID.h"

//#define DEBUG

////////////////////////////////////////////////
// class declaration
//

// number of electrons in each branch (max nEle)
#define NELE 2

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
  bool isPartGun;
  double doHighEta_LowerEtaCut;

  //Handles and inputTags
private:
  //------------------------------ Handles
  edm::Handle<std::vector<pat::Electron> > electronsHandle;
  edm::Handle<std::vector<pat::Muon> > muonsHandle;
  edm::Handle<std::vector<pat::Photon> > photonsHandle;
  edm::Handle<std::vector<reco::SuperCluster>> EESuperClustersHandle; //used only for high-eta 
  edm::Handle<reco::BeamSpot> bsHandle;
  edm::Handle<reco::VertexCollection> primaryVertexHandle;
  edm::ESHandle<CaloTopology> topologyHandle;
  edm::Handle<double> rhoHandle;
  edm::Handle<std::vector< PileupSummaryInfo > >  PupInfo;
  edm::Handle< GenEventInfoProduct >  GenEventInfoHandle;
  edm::Handle<reco::ConversionCollection> conversionsHandle;
  edm::Handle< reco::PFMETCollection > metHandle;
  edm::Handle<edm::TriggerResults> triggerResultsHandle;
  edm::Handle<edm::TriggerResults> WZSkimResultsHandle;

  //------------------------------ Input Tags
  // input tag for primary vertex
  edm::InputTag vtxCollectionTAG;
  edm::InputTag BeamSpotTAG;
  // input tag for electrons
  edm::InputTag electronsTAG;
  edm::InputTag muonsTAG;
  edm::InputTag photonsTAG;

#ifdef CMSSW_7_2_X
  edm::EDGetTokenT<EcalRecHitCollection> recHitCollectionEBTAG;
  edm::EDGetTokenT<EcalRecHitCollection> recHitCollectionEETAG;
#else
  edm::InputTag recHitCollectionEBTAG;
  edm::InputTag recHitCollectionEETAG;
#endif

  edm::InputTag EESuperClustersTAG;
  // input rho
  edm::InputTag rhoTAG;
  edm::InputTag conversionsProducerTAG;
  edm::InputTag metTAG;
  edm::InputTag triggerResultsTAG;
  edm::InputTag WZSkimResultsTAG;
  std::vector< std::string> hltPaths, SelectEvents;
private:
  std::string foutName;
  
  bool doExtraCalibTree;
  bool doEleIDTree;


  edm::Service<TFileService> fs; //< output file for extra ntuples
  TTree *tree;                   //< output file for standard ntuple

  // ntuple members, private to make them visible in doxygen
private: 
  
  Int_t     	runNumber;   ///< 
  Long64_t      eventNumber; ///<
  Int_t         lumiBlock;   ///< lumi section
  UInt_t 	runTime;     ///< unix time

  Float_t       mcGenWeight; ///< weight in generator for MC

  std::vector< std::string > HLTNames[1]; ///< List of HLT names
  std::vector<Bool_t> HLTResults[1];      ///< 0=fail, 1=fire
  std::map<std::string, bool> HLTBits;
  Bool_t HLTfire; ///< true if pass the triggers indicated by hltPaths in cfg

  //pileup
  Float_t rho;            ///< rho fast jet
  Int_t   nPV;            ///< nVtx
  Int_t   nPU[5];   //[nBX]   ///< number of PU (filled only for MC)

  // selection
  Int_t eleID[3];        ///< bit mask for eleID: 1=fiducial, 2=loose, 6=medium, 14=tight, 16=WP90PU, 48=WP80PU, 112=WP70PU, 128=loose25nsRun2, 384=medium25nsRun2, 896=tight25nsRun2, 1024=loose50nsRun2, 3072=medium50nsRun2, 7168=tight50nsRun2. Selection from https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaCutBasedIdentification#Electron_ID_Working_Points

  Int_t  chargeEle[3]; ///< -100: no electron, 0: SC or photon, -1 or +1:electron or muon
  Float_t etaSCEle[3], phiSCEle[3]; ///< phi of the SC
  Float_t   etaEle[3],   phiEle[3]; ///< phi of the electron (electron object)

  Int_t recoFlagsEle[3];           ///< 1=trackerDriven, 2=ecalDriven only, 3=tracker and ecal driven

  Float_t PtEle[3];
  Int_t   classificationEle[3];   ///< electron classification in GOLD, SHOWERING, etc.

  Float_t fbremEle[3];

  Float_t seedXSCEle[3];        ///< ieta(ix) of the SC seed in EB(EE)
  Float_t seedYSCEle[3];        ///< iphi(iy) of the SC seed in EB(EE)
  Float_t seedEnergySCEle[3];   ///< energy of the SC seed
  UChar_t gainEle[3];           ///< Gain switch 0==gain12, 1==gain6, 2==gain1; gain status of the seed of the SC
  Float_t seedLCSCEle[3];       ///< laser correction of the SC seed crystal

  Float_t avgLCSCEle[3];





  Float_t energyMCEle[3];    ///< Electron MC true energy
  Float_t energySCEle[3];    ///< corrected SuperCluster energy
  Float_t rawEnergySCEle[3]; ///< SC energy without cluster corrections
  Float_t esEnergySCEle[3];  ///< pre-shower energy associated to the electron

  Float_t energySCEle_regrCorr_ele[3]; ///< SC energy based on MVA (target=SuperCluster raw energy/true energy): tuned on electrons (W MC sample)
  Float_t energySCEle_regrCorr_pho[3]; ///< SC energy based on MVA (target=SuperCluster raw energy/true energy): tuned on photons (di-photon MC sample)

  Float_t energyEle_regrCorr_fra[3];
  Float_t energySigmaEle_regrCorr_fra[3];

  Float_t energySigmaSCEle_regrCorr_ele[3];
  Float_t energySigmaSCEle_regrCorr_pho[3];

  Float_t energySigmaSCEle_regrCorrSemiParV4_pho[3], energySCEle_regrCorrSemiParV4_pho[3]; ///< see \ref regressions
  Float_t energySigmaSCEle_regrCorrSemiParV4_ele[3], energySCEle_regrCorrSemiParV4_ele[3]; ///< see \ref regressions
  Float_t energySigmaSCEle_regrCorrSemiParV5_pho[3], energySCEle_regrCorrSemiParV5_pho[3]; ///< see \ref regressions
  Float_t energySigmaSCEle_regrCorrSemiParV5_ele[3], energySCEle_regrCorrSemiParV5_ele[3]; ///< see \ref regressions
  Float_t energySigmaSCEle_regrCorrSemiParV6_pho[3], energySCEle_regrCorrSemiParV6_pho[3]; ///< see \ref regressions
  Float_t energySigmaSCEle_regrCorrSemiParV6_ele[3], energySCEle_regrCorrSemiParV6_ele[3]; ///< see \ref regressions
  Float_t energySigmaSCEle_regrCorrSemiParV7_pho[3], energySCEle_regrCorrSemiParV7_pho[3]; ///< see \ref regressions
  Float_t energySigmaSCEle_regrCorrSemiParV7_ele[3], energySCEle_regrCorrSemiParV7_ele[3]; ///< see \ref regressions
  Float_t energySigmaSCEle_regrCorrSemiParV8_pho[3], energySCEle_regrCorrSemiParV8_pho[3]; ///< see \ref regressions
  Float_t energySigmaSCEle_regrCorrSemiParV8_ele[3], energySCEle_regrCorrSemiParV8_ele[3]; ///< see \ref regressions
  Float_t energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_pho[3], energySCEle_regrCorrSemiPar7TeVtrainV6_pho[3]; ///< see \ref regressions
  Float_t energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_ele[3], energySCEle_regrCorrSemiPar7TeVtrainV6_ele[3]; ///< see \ref regressions
  Float_t energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_pho[3], energySCEle_regrCorrSemiPar7TeVtrainV7_pho[3]; ///< see \ref regressions
  Float_t energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_ele[3], energySCEle_regrCorrSemiPar7TeVtrainV7_ele[3]; ///< see \ref regressions
  Float_t energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_pho[3], energySCEle_regrCorrSemiPar7TeVtrainV8_pho[3]; ///< see \ref regressions
  Float_t energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_ele[3], energySCEle_regrCorrSemiPar7TeVtrainV8_ele[3]; ///< see \ref regressions

  Float_t energySCEle_corr[3];  ///< ecal energy with corrections base on type of electron (see #classificationEle)

  Float_t energyEle_regrCorr_egamma[3];      ///< Egamma POG electron regression energy
  Float_t energySigmaEle_regrCorr_egamma[3]; ///< Egamma POG electron regression energy uncertainty

  //  Float_t pool_regrCorr_ele;

  Float_t e3x3SCEle[3];   //< sum of the recHit energy in 3x3 matrix centered at the seed of the SC
  Float_t e5x5SCEle[3];   ///< sum of the recHit energy in 5x5 matrix centered at the seed of the SC
  Float_t eSeedSCEle[3];
  Float_t pModeGsfEle[3];  ///< track momentum from Gsf Track (mode)
  Float_t pAtVtxGsfEle[3]; ///< momentum estimated at the vertex
  Float_t trackMomentumErrorEle[3]; ///< track momentum error from standard electron method
  Float_t pNormalizedChi2Ele[3];  ///< track normalized chi2 of the fit (GSF)

  Float_t R9Ele[3];      ///< e3x3/rawEnergySCEle

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
  Float_t invMass_SC_regrCorrSemiParV4_ele,invMass_SC_regrCorrSemiParV4_pho;
  Float_t invMass_SC_regrCorrSemiParV5_ele,invMass_SC_regrCorrSemiParV5_pho;
  Float_t invMass_SC_regrCorrSemiParV6_ele,invMass_SC_regrCorrSemiParV6_pho;
  Float_t invMass_SC_regrCorrSemiParV7_ele,invMass_SC_regrCorrSemiParV7_pho;
  Float_t invMass_SC_regrCorrSemiParV8_ele,invMass_SC_regrCorrSemiParV8_pho;
  Float_t invMass_SC_regrCorrSemiPar7TeVtrainV6_ele,invMass_SC_regrCorrSemiPar7TeVtrainV6_pho;
  Float_t invMass_SC_regrCorrSemiPar7TeVtrainV7_ele,invMass_SC_regrCorrSemiPar7TeVtrainV7_pho;
  Float_t invMass_SC_regrCorrSemiPar7TeVtrainV8_ele,invMass_SC_regrCorrSemiPar7TeVtrainV8_pho;

  Float_t invMass_MC;
  Float_t invMass_mumu;
  Float_t   etaMCEle[3], phiMCEle[3];


#ifdef shervin
  Float_t invMass_inGsf;
  Float_t invMass_outGsf;
  Float_t invMass_SC_etaphiSC;
  Int_t   nBX;
  Int_t   bxPU[5];   //[nBX]
  Int_t tagProbe_check; 
  Int_t nBCSCEle[3];
#endif

  //============================== ExtraCalibTree
  TFile *extraCalibTreeFile;
  TTree *extraCalibTree;
  edm::Timestamp runTime_;
  Int_t nRecHitsEle[3];
  Int_t nHitsSCEle[3];
  std::vector<int> recoFlagRecHitSCEle[3];
  std::vector<float>  rawIdRecHitSCEle[3];
  std::vector<int>        XRecHitSCEle[3];
  std::vector<int>        YRecHitSCEle[3];
  std::vector<int>        ZRecHitSCEle[3];
  std::vector<float> energyRecHitSCEle[3];
  std::vector<float>     LCRecHitSCEle[3];
  std::vector<float>     ICRecHitSCEle[3];
  std::vector<float>  AlphaRecHitSCEle[3];
  //==============================

  //============================== check ele-id and iso
  TFile *eleIDTreeFile;
  TTree *eleIDTree;
  Float_t sigmaIEtaIEtaSCEle[3];
  Float_t sigmaIPhiIPhiSCEle[3];
  Float_t hOverE[3];
  Float_t dr03TkSumPt[3];
  Float_t dr03EcalRecHitSumEt[3];
  Float_t dr03HcalTowerSumEt[3];
  Float_t deltaPhiSuperClusterTrackAtVtx[3];
  Float_t deltaEtaSuperClusterTrackAtVtx[3];
  Bool_t hasMatchedConversion[3];
  Int_t maxNumberOfExpectedMissingHits[3];
  Float_t pfMVA[3];
  Float_t eleIDloose[3], eleIDmedium[3], eleIDtight[3];
  //==============================

  //==============================  
  TTree *pdfSystTree;
  //std::vector<Int_t>   pdfSystWeightNum;
  std::vector<Double_t> pdfSystWeight[5];
  // pdfWeightTree
  bool doPdfSystTree;
  std::vector< edm::InputTag > pdfWeightTAGS;
  edm::Handle< std::vector<double> > pdfWeightHandle;

  edm::InputTag fsrWeightTAG, weakWeightTAG;
  edm::Handle<Double_t> fsrWeightHandle, weakWeightHandle;
  Float_t fsrWeight;
  Float_t weakWeight;

  void InitPdfSystTree(void);
  void TreeSetPdfSystVar(const edm::Event& iEvent);

  //==============================
private:
  TFile *tree_file;
  void InitNewTree(void);

  void TreeSetSingleElectronVar(const pat::Electron& electron1, int index);
  void TreeSetSingleElectronVar(const reco::SuperCluster& electron1, int index);
  void TreeSetSinglePhotonVar(const pat::Photon& photon, int index);
  void TreeSetSingleMuonVar(const pat::Muon& muon1, int index);
  void TreeSetDiElectronVar(const pat::Electron& electron1, const pat::Electron& electron2);
  void TreeSetDiElectronVar(const pat::Electron& electron1, const reco::SuperCluster& electron2);
  void TreeSetMuMuGammaVar(const pat::Photon& photon, const pat::Muon& muon1, const pat::Muon& muon2);
  DetId findSCseed(const reco::SuperCluster& cluster);

  void InitExtraCalibTree(void);
  void TreeSetExtraCalibVar(const pat::Electron& electron1, int index);
  void TreeSetExtraCalibVar(const reco::SuperCluster& electron1, int index);
  void TreeSetExtraCalibVar(const pat::Photon& photon, int index);
  void TreeSetExtraCalibVar(const pat::Muon& muon1, int index);
  void TreeSetExtraCalibVar(const pat::Electron& electron1, const pat::Electron& electron2);
  void TreeSetExtraCalibVar(const pat::Electron& electron1, const reco::SuperCluster& electron2);
  void TreeSetExtraCalibVar(const pat::Photon& photon, const pat::Muon& muon1, const pat::Muon& muon2);

  void InitEleIDTree(void);

  void TreeSetEleIDVar(const pat::Electron& electron1, int index);
  void TreeSetEleIDVar(const pat::Electron& electron1, const pat::Electron& electron2);
  void TreeSetEleIDVar(const pat::Photon& photon, const pat::Muon& muon1, const pat::Muon& muon2);
  void TreeSetEleIDVar(const pat::Photon& photon, int index);
  void TreeSetEleIDVar(const pat::Muon& muon1, int index);

  //  void Tree_output(TString filename);
  void TreeSetEventSummaryVar(const edm::Event& iEvent);
  void TreeSetPileupVar(void);

  //puWeights_class puWeights;

private:

  // --------------- selection cuts
private:

  Long64_t epsilonected;

  //------------------------------
  // cluster tools
  EcalClusterLazyTools *clustertools;
  //  EcalClusterLocal _ecalLocal;


  std::set<unsigned int> alcaSkimPathIndexes;
  edm::ParameterSetID alcaSkimPathID;
  //
  // static data member definitions
  //

  typedef enum {
    ZEE=0,
    WENU,
    ZSC,
    ZMMG,
    PARTGUN,
    UNKNOWN,
    SINGLEELE, //no skim, no preselection and no selection are applied
    DEBUG
  }eventType_t;

  eventType_t eventType;
  unsigned int eventTypeCounter[6];
};


//
// constructors and destructor
//
ZNtupleDumper::ZNtupleDumper(const edm::ParameterSet& iConfig):
  //  isMC(iConfig.getParameter<bool>("isMC")), 
  isPartGun(iConfig.getParameter<bool>("isPartGun")),
  doHighEta_LowerEtaCut(iConfig.getParameter<double>("doHighEta_LowerEtaCut")),
  vtxCollectionTAG(iConfig.getParameter<edm::InputTag>("vertexCollection")),
  BeamSpotTAG(iConfig.getParameter<edm::InputTag>("BeamSpotCollection")),
  electronsTAG(iConfig.getParameter<edm::InputTag>("electronCollection")),
  muonsTAG(iConfig.getParameter<edm::InputTag>("muonCollection")),
  photonsTAG(iConfig.getParameter<edm::InputTag>("photonCollection")),
#ifdef CMSSW_7_2_X
  recHitCollectionEBTAG(consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>( "recHitCollectionEB" ))),
  recHitCollectionEETAG(consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>( "recHitCollectionEE" ))),
#else
  recHitCollectionEBTAG(iConfig.getParameter<edm::InputTag>("recHitCollectionEB")),
  recHitCollectionEETAG(iConfig.getParameter<edm::InputTag>("recHitCollectionEE")),
#endif

  EESuperClustersTAG(iConfig.getParameter<edm::InputTag>("EESuperClusterCollection")),
  rhoTAG(iConfig.getParameter<edm::InputTag>("rhoFastJet")),
  conversionsProducerTAG(iConfig.getParameter<edm::InputTag>("conversionCollection")),
  metTAG(iConfig.getParameter<edm::InputTag>("metCollection")),
  triggerResultsTAG(iConfig.getParameter<edm::InputTag>("triggerResultsCollection")),
  WZSkimResultsTAG(iConfig.getParameter<edm::InputTag>("WZSkimResultsCollection")),
  hltPaths(iConfig.getParameter< std::vector<std::string> >("hltPaths")),
  SelectEvents(iConfig.getParameter<std::vector<std::string> >("SelectEvents")),
  foutName(iConfig.getParameter<std::string>("foutName")),
  doExtraCalibTree(iConfig.getParameter<bool>("doExtraCalibTree")),
  doEleIDTree(iConfig.getParameter<bool>("doEleIDTree")),
  doPdfSystTree(false),
  pdfWeightTAGS(iConfig.getParameter< std::vector<edm::InputTag> >("pdfWeightCollections")),
  fsrWeightTAG(iConfig.getParameter< edm::InputTag>("fsrWeightCollection")),
  weakWeightTAG(iConfig.getParameter< edm::InputTag>("weakWeightCollection")),
  fsrWeight(1.), weakWeight(1.),
  eventType(ZEE)
{
  
  if(pdfWeightTAGS.size()>0) doPdfSystTree=true;




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
  eventType= isPartGun ? PARTGUN : UNKNOWN;

  chargeEle[0]=-100;
  chargeEle[1]=-100;
  chargeEle[2]=-100;
  invMass_mumu=0;

  pEvent = &iEvent;
  pSetup = &iSetup;
  
  // filling infos runNumber, eventNumber, lumi
  if( !iEvent.isRealData() ){
    iEvent.getByLabel(edm::InputTag("addPileupInfo"), PupInfo);
    iEvent.getByLabel(edm::InputTag("generator"), GenEventInfoHandle);
    isMC=true;
  } else isMC=false;
  
  //------------------------------ HLT
  /// \todo check why
  if(triggerResultsTAG.label()!="") iEvent.getByLabel(triggerResultsTAG, triggerResultsHandle);
  iEvent.getByLabel(WZSkimResultsTAG,  WZSkimResultsHandle);

  //Check if it is Wenu, Z or ZSC event according to triggerResults
  edm::TriggerNames alcaSkimPathNames = iEvent.triggerNames(*WZSkimResultsHandle);

  if(!SelectEvents.empty()){
    // If the alca skim paths are not changing, this is done only once
    if(alcaSkimPathID != alcaSkimPathNames.parameterSetID()){ //order of trigger results is changed
      alcaSkimPathID = alcaSkimPathNames.parameterSetID();    //update the map of trigger index
      alcaSkimPathIndexes.clear();
      unsigned int alcaSkimPathNameSize=alcaSkimPathNames.size(); // should have the same size of WZSkimResultsHandle

      for(unsigned int i=0; i < alcaSkimPathNameSize; i++){ // look over alcaSkimPaths
	std::string trgName = alcaSkimPathNames.triggerName(i);
	
	for(std::vector<std::string>::const_iterator selectEvents_itr = SelectEvents.begin();
	    selectEvents_itr!=SelectEvents.end();
	    selectEvents_itr++){
	  if(std::regex_match(trgName, std::regex(*selectEvents_itr))){
	    alcaSkimPathIndexes.insert(i);
	    //std::cout << " - Trigger path saved in ntuples: " << trgName << "\t" << i << std::endl;
	    break;
	  }
	}
	//if(alcaSkimPathIndexes.count(i)==0){
	//std::cout << " -! Trigger path not saved in ntuples: " << trgName <<  "\t" << i << std::endl;
	//}
      }
    }
    eventType=DEBUG;
    bool skipEvent=true;
    for(std::set<unsigned int>::const_iterator alcaSkimPath_itr = alcaSkimPathIndexes.begin(); 
	alcaSkimPath_itr != alcaSkimPathIndexes.end() && skipEvent==true; 
	alcaSkimPath_itr++){
      //std::cout << *alcaSkimPath_itr << std::endl;
      if(WZSkimResultsHandle->accept(*alcaSkimPath_itr)){
	skipEvent=false;
	std::string hltName_str(alcaSkimPathNames.triggerName(*alcaSkimPath_itr));
	if(hltName_str.find("WElectron")!=std::string::npos)
	  eventType=WENU;
	else if(hltName_str.find("ZSCElectron")!=std::string::npos)
	  eventType=ZSC;
	else if(hltName_str.find("ZElectron")!=std::string::npos)
	  eventType=ZEE;
	else if(hltName_str.find("SingleElectron")!=std::string::npos)
	  eventType=SINGLEELE;
	else if(hltName_str.find("Zmmg")!=std::string::npos) 
	  eventType=ZMMG;
	else
	  eventType=UNKNOWN;
	// this paths are exclusive, then we can skip the check of the others
	//
	//	std::cout << alcaSkimPathNames.triggerName(*alcaSkimPath_itr) << "\t" << eventType << std::endl;
	break;
      }


    }
    //std::cout << "skip event: " << skipEvent << "\t" << eventType << std::endl;
    //assert(!skipEvent);
    eventTypeCounter[eventType]++;
    if(skipEvent) return; // event not coming from any skim or paths
  }
  //------------------------------ CONVERSIONS
  iEvent.getByLabel(conversionsProducerTAG, conversionsHandle);

  //------------------------------
  clustertools = new EcalClusterLazyTools (iEvent, iSetup, recHitCollectionEBTAG, 
					   recHitCollectionEETAG);  

  //------------------------------ electrons
  if (eventType==ZMMG) {
    //------------------------------ muons
    iEvent.getByLabel(muonsTAG, muonsHandle);
    //------------------------------ photons
    iEvent.getByLabel(photonsTAG, photonsHandle);
  }

  else
    iEvent.getByLabel(electronsTAG, electronsHandle);

  //------------------------------ SuperClusters (for high Eta studies)
  iEvent.getByLabel(EESuperClustersTAG, EESuperClustersHandle);

  // for conversions with full vertex fit
  //------------------------------  VERTEX
  iEvent.getByLabel(vtxCollectionTAG, primaryVertexHandle); 
  iEvent.getByLabel(BeamSpotTAG, bsHandle);
  iEvent.getByLabel(rhoTAG,rhoHandle);
  
  iEvent.getByLabel(metTAG, metHandle); 
  //if(metHandle.isValid()==false) iEvent.getByType(metHandle);
  reco::PFMET met = ((*metHandle))[0]; /// \todo use corrected phi distribution


  //Here the HLTBits are filled. TriggerResults
  TreeSetEventSummaryVar(iEvent);
  TreeSetPileupVar(); // this can be filled once per event

  if(doPdfSystTree && isMC){ 
    TreeSetPdfSystVar(iEvent);
  }

  
  // at least one of the triggers
  HLTfire=false;
  if(!hltPaths.empty()){
    for(std::vector<std::string>::const_iterator hltPath_itr = hltPaths.begin();
	hltPath_itr != hltPaths.end();
	hltPath_itr++){
      if(hltPath_itr->empty()) continue;
      std::map<std::string, bool>::const_iterator it = HLTBits.find(*hltPath_itr);
      if(it!=HLTBits.end()){
	HLTfire+=it->second;
	//	std::cout << "Not empty:" << hltPaths[0] << "\t" << it->first << "\t" << it->second << "\t" << triggerFire << std::endl;
	//}else{
	// 	for(std::map<std::string, bool>::const_iterator it_ = HLTBits.begin();
	// 	    it_!=HLTBits.end();
	// 	    it_++){
	// 	  std::cout  << "\t" << it_->first << "\t" << it_->second << std::endl;
	// 	}
	//	edm::LogError("ZNtupleDumper") << "HLT path required but not find in TriggerResults" << " " << HLTBits.size();
	//edm::LogWarning("ZNtupleDumper") << "HLT path " << *hltPath_itr << " required but not found in TriggerResults" << " " << HLTBits.size();
	//exit(1);
      }
    }
  } 

  // count electrons: needed to avoid double counting events in Wenu and Zee samples
  // in Wenu is required ONLY ONE tight electron 
  // in Zee at least two loose electrons
  // in particle gun case, the matching with the gen particle is required

  int nWP70   = 0; //only WP70
  int nMedium = 0; //passing medium eleID
  int nWP90   = 0; //only WP90

  if (eventType!=ZMMG) {
    for( pat::ElectronCollection::const_iterator eleIter1 = electronsHandle->begin();
	 eleIter1 != electronsHandle->end();
	 eleIter1++){
      if( eleIter1->electronID("tight") )       ++nWP70;
      else if( eleIter1->electronID("medium") ) ++nMedium;
      else if( eleIter1->electronID("loose") )  ++nWP90;
    }
  }  

  bool doFill=false;
  if(eventType==PARTGUN){
    pat::ElectronCollection::const_iterator eleIter1 = electronsHandle->begin();
    pat::ElectronCollection::const_iterator eleIter2 = eleIter1;
    for(eleIter1 = electronsHandle->begin();
	eleIter1 != electronsHandle->end() && eleIter1->genLepton()==0;
	eleIter1++){
    }
    
    //if no electron matching the gen particles then the event is skipped
    //if(eleIter1 == electronsHandle->end()) return;
    if(eleIter1 == electronsHandle->end()) eleIter1=electronsHandle->begin();
    
    //in order to not put duplicate electrons, remove the ones not matching the genparticle
    for(eleIter2=eleIter1, eleIter2++;
	eleIter2 != electronsHandle->end() && eleIter2->genLepton()==0;
	eleIter2++){
    }
    if(eleIter2 == electronsHandle->end()){
      if(eleIter1->genLepton()!=0 || electronsHandle->size() < NELE) eleIter2=eleIter1;
      else eleIter2=eleIter1+1;
    }

    //if one electron matching the gen particles then eleIter2 = eleIter1
    //else we have two electrons
    TreeSetDiElectronVar(*eleIter1, *eleIter2);
    doFill=true;
    if(doExtraCalibTree){
      TreeSetExtraCalibVar(*eleIter1, *eleIter2);
    }
    if(doEleIDTree){
      TreeSetEleIDVar(*eleIter1, *eleIter2);
    }
  } 
  else if(eventType==ZEE || eventType==WENU || eventType==UNKNOWN){
    for( pat::ElectronCollection::const_iterator eleIter1 = electronsHandle->begin();
	 eleIter1 != electronsHandle->end();
	 eleIter1++){
      if(eventType==WENU){
	if(! eleIter1->electronID("tight") ) continue;
	if( nWP70 != 1 || nWP90 > 0 ) continue; //to be a Wenu event request only 1 ele WP70 in the event
	
	// MET/MT selection
	if(  met.et() < 25. ) continue;
	if( sqrt( 2.*eleIter1->et()*met.et()*(1 -cos(eleIter1->phi()-met.phi()))) < 50. ) continue;
	if( eleIter1->et()<30) continue;

	doFill=true;	
	if(eventType==UNKNOWN) eventType=WENU;
	TreeSetSingleElectronVar(*eleIter1, 0);  //fill first electron 
	TreeSetSingleElectronVar(*eleIter1, -1); // fill fake second electron

	if(doExtraCalibTree){
	  TreeSetExtraCalibVar(*eleIter1, 0);
	  TreeSetExtraCalibVar(*eleIter1, -1);
	}
	if(doEleIDTree){
	  TreeSetEleIDVar(*eleIter1, 0);
	  TreeSetEleIDVar(*eleIter1, -1);
	}
      }else { //ZEE or UNKNOWN
	// take only the fist di-electron pair (highest pt)
	for(pat::ElectronCollection::const_iterator eleIter2 = eleIter1+1;
	    eleIter2 != electronsHandle->end() && doFill==false;
	    eleIter2++){
	  // should exit when eleIter1 == end-1
	  //if(! eleIter2->electronID("loose") ) continue;
	  
	  float mass=(eleIter1->p4()+eleIter2->p4()).mass();

	  if((mass < 55 || mass > 125)) continue;	  
	  doFill=true;

	  if(eventType==UNKNOWN) eventType=ZEE;
	  TreeSetDiElectronVar(*eleIter1, *eleIter2);
	  
	  if(doExtraCalibTree){
	    TreeSetExtraCalibVar(*eleIter1, *eleIter2);
	  }
	  if(doEleIDTree){
	    TreeSetEleIDVar(*eleIter1, *eleIter2);
	  }
	  if(doPdfSystTree && isMC){
	    TreeSetPdfSystVar(iEvent);
	    //pdfSystTree->Fill();
	  }
	}

	if(electronsHandle->size() < NELE &&  eventType == SINGLEELE);
	
	doFill=true;	
	TreeSetSingleElectronVar(*eleIter1, 0);  //fill first electron 
	TreeSetSingleElectronVar(*eleIter1, -1); // fill fake second electron

	if(doExtraCalibTree){
	  TreeSetExtraCalibVar(*eleIter1, 0);
	  TreeSetExtraCalibVar(*eleIter1, -1);
	}
	if(doEleIDTree){
	  TreeSetEleIDVar(*eleIter1, 0);
	  TreeSetEleIDVar(*eleIter1, -1);
	}

      }
    }
  }

  else if (eventType==ZMMG){
    for( pat::MuonCollection::const_iterator muIter1 = muonsHandle->begin();
	 muIter1 != muonsHandle->end() && doFill==false;
	 muIter1++){

	for(pat::MuonCollection::const_iterator muIter2 = muIter1+1;
	    muIter2 != muonsHandle->end() && doFill==false;
	    muIter2++){

	  // should exit when muIter1 == end-1
	  //if(! muIter2->electronID("loose") ) continue;
	  for( pat::PhotonCollection::const_iterator phoIter1 = photonsHandle->begin();
	       phoIter1 != photonsHandle->end() && doFill==false;
	       phoIter1++){

	    float mass=(muIter1->p4()+muIter2->p4()+phoIter1->p4()).mass();

	    if (phoIter1->pt()<10) continue;
	    if((mass < 55 || mass > 125)) continue;
	    doFill=true;	    

	    TreeSetMuMuGammaVar(*phoIter1, *muIter1, *muIter2);

	    if(doExtraCalibTree){
	      TreeSetExtraCalibVar(*phoIter1, *muIter1, *muIter2);
	    }
	    if(doEleIDTree){
	      TreeSetEleIDVar(*phoIter1, *muIter1, *muIter2);
	    }
	    if(doPdfSystTree && isMC){
	      TreeSetPdfSystVar(iEvent);
	      //pdfSystTree->Fill();
	    }

	  }
	  
	}
    }
  }

  if (eventType==ZSC || eventType==UNKNOWN){
    
    //leading pt electron in EB (patElectrons should be pt-ordered)
    // iterators storing pat Electons and HighEta SCs
    pat::ElectronCollection::const_iterator PatEle1 = electronsHandle->begin();
    // iterators storing HighEta SCs
    // select the highest pt SC in the highEta region
    reco::SuperClusterCollection::const_iterator HighEtaSC1 = EESuperClustersHandle->end();

    for( PatEle1 = electronsHandle->begin();
	 //stop when HighEtaSC1 is a valid SC (this means that there is a pair matching the Z mass
         PatEle1 != electronsHandle->end();
         PatEle1++){

      // consider electrons passing at least the loose identification
      if(!PatEle1->electronID("loose") ) continue; 

      // take the highest pt tight electrons if it exists (the collection is ordered in pt)
      // consider only the electrons passing the tightest electron identification
      if(nWP70>0){ // if there are tight electrons, consider only those
	if(!PatEle1->electronID("tight") ) continue; 
      }else if(nMedium>0){ // if there are only medium electrons, consider only those
	if(!PatEle1->electronID("medium") ) continue; 
      }

      // you have the first electrons candidate satifying the electrons criteria
      // now look for a SC matching the Z invariant mass. If not SC is found, let's look to another electrons candidate

      double HighEtaSCPt=0;
      double t1=TMath::Exp(-PatEle1->eta());
      double t1q = t1*t1;
      for( reco::SuperClusterCollection::const_iterator iter = EESuperClustersHandle->begin();
	   iter!= EESuperClustersHandle->end();
	   iter++){
	// only SCs in the high eta region
        if (fabs(iter->eta()) < doHighEta_LowerEtaCut) continue;

	//calculate the invariant mass
	double t2=TMath::Exp(-iter->eta());
	double t2q = t2*t2;
	double angle=1-
	  ( (1-t1q)*(1-t2q)+4*t1*t2*cos(PatEle1->phi()-iter->phi()))/(
								      (1+t1q)*(1+t2q)
								      );
	float mass = sqrt(2*PatEle1->energy()*iter->energy() *angle);
	if((mass < 55 || mass > 125)) continue;

	//take the highest pt SC matching the Z mass
	double pt=iter->energy()/cosh(iter->eta());
        if(HighEtaSCPt<pt){
	  HighEtaSCPt=pt;
	  HighEtaSC1=iter;
	}
      }

      if(HighEtaSC1!= EESuperClustersHandle->end()) break;
    }

    // if you have found an ele-SC pair matching the high eta criteria, 
    // save the event in the tree
    if(HighEtaSC1!= EESuperClustersHandle->end()){
      doFill=true;
      TreeSetDiElectronVar(*PatEle1, *HighEtaSC1);
      if(doExtraCalibTree){
	TreeSetExtraCalibVar(*PatEle1, *HighEtaSC1);
      }
    }
  }
  

  if(doFill){
    tree->Fill();
    if(doExtraCalibTree)  extraCalibTree->Fill();
    if(doEleIDTree)      eleIDTree->Fill();
  }
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
    //extraCalibTree = fs->make<TTree>("extraCalibTree","");
    // put the extraCalibTree into the default outfile
    extraCalibTreeFile = new TFile("extraCalibTree.root", "recreate");
    if(extraCalibTreeFile->IsZombie()){
      throw cms::Exception("OutputError") <<  "Output tree for extra calib not created (Zombie): " << foutName;
      return;
    }
    extraCalibTreeFile->cd();
  
    extraCalibTree = new TTree("extraCalibTree", "extraCalibTree");
    extraCalibTree->SetDirectory(extraCalibTreeFile);
    InitExtraCalibTree();
  }

  if(doEleIDTree){
    eleIDTreeFile = new TFile("eleIDTree.root","recreate");
    if(eleIDTreeFile->IsZombie()){
      throw cms::Exception("OutputError") <<  "Output tree for extra calib not created (Zombie): " << foutName;
      return;
    }
    eleIDTreeFile->cd();
    eleIDTree = new TTree("eleIDTree","eleIDTree");
    eleIDTree->SetDirectory(eleIDTreeFile);
    //eleIDTree = fs->make<TTree>("eleIDTree","");
    InitEleIDTree();
  }
  
  if(doPdfSystTree){
    std::cout << "[INFO] Doing pdfSystTree" << std::endl;
    //pdfSystTree = fs->make<TTree>("pdfSystTree","pdfSystTree");
    pdfSystTree=tree;
    InitPdfSystTree();
  }

  for(int i=ZEE; i <= DEBUG; i++){
    eventTypeCounter[i]=0;
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
    if(doEleIDTree)       eleIDTree->BuildIndex("runNumber","eventNumber");
  }
  // save the tree into the file
  tree_file->cd();
  tree->Write();
  tree_file->Close();
  
  if(doExtraCalibTree){
    extraCalibTree->BuildIndex("runNumber","eventNumber");
    extraCalibTreeFile->cd();
    extraCalibTree->Write();  
    extraCalibTreeFile->Close();
  }
  if(doEleIDTree){
    eleIDTree->BuildIndex("runNumber","eventNumber");
    eleIDTreeFile->cd();
    eleIDTree->Write();
    eleIDTreeFile->Close();
  }


  for(int i=ZEE; i <= DEBUG; i++){
    std::cout << "[NTUPLEDUMPER] EventTypeCounter[" << i << "]\t"<< eventTypeCounter[i] << std::endl;
  }  
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
  
  tree->Branch("mcGenWeight",   &mcGenWeight, "mcGenWeight/F");

  tree->Branch("HLTfire", &HLTfire, "HLTfire/B");
  //tree->Branch("HLTNames",&(HLTNames[0]));
  //tree->Branch("HLTResults",&(HLTResults[0]));

  //extraCalibTree->Branch("XRecHitSCEle1", &(XRecHitSCEle[0]));

  //   tree->Branch("nBX", &nBX, "nBX/I");
  //  tree->Branch("nPU", nPU, "nPU[nBX]/I");
  tree->Branch("nPU", nPU, "nPU[1]/I");
  //   tree->Branch("bxPU", bxPU, "bxPU[nBX]/I");
  tree->Branch("rho", &rho, "rho/F");

  tree->Branch("nPV", &nPV, "nPV/I");

  tree->Branch("eleID",eleID, "eleID[3]/I");
  //  tree->Branch("nBCSCEle", nBCSCEle, "nBCSCEle[3]/I");

  tree->Branch("chargeEle",   chargeEle,    "chargeEle[3]/I");	//[nEle]
  tree->Branch("etaSCEle",      etaSCEle,       "etaSCEle[3]/F");	//[nSCEle]
  tree->Branch("phiSCEle",      phiSCEle,       "phiSCEle[3]/F");	//[nSCEle]

  tree->Branch("etaEle",      etaEle,       "etaEle[3]/F");	//[nEle]
  tree->Branch("phiEle",      phiEle,       "phiEle[3]/F");	//[nEle]

  tree->Branch("classificationEle",      classificationEle,       "classificationEle[3]/I");	//[nEle]
  tree->Branch("recoFlagsEle",recoFlagsEle, "recoFlagsEle[3]/I");	//[nEle]
  tree->Branch("PtEle",       PtEle,        "PtEle[3]/F");
  tree->Branch("fbremEle",    fbremEle,     "fbremEle[3]/F");

  tree->Branch("seedXSCEle",           seedXSCEle,      "seedXSCEle[3]/F");
  tree->Branch("seedYSCEle",           seedYSCEle,      "seedYSCEle[3]/F");
  tree->Branch("seedEnergySCEle", seedEnergySCEle, "seedEnergySCEle[3]/F");
  tree->Branch("seedLCSCEle",         seedLCSCEle,     "seedLCSCEle[3]/F");

  tree->Branch("avgLCSCEle", avgLCSCEle, "avgLCSCEle[3]/F");

  tree->Branch("gainEle", gainEle, "gainEle[3]/b");

  tree->Branch("energyMCEle", energyMCEle, "energyMCEle[3]/F");
  tree->Branch("energySCEle", energySCEle, "energySCEle[3]/F");
  tree->Branch("rawEnergySCEle", rawEnergySCEle, "rawEnergySCEle[3]/F");
  tree->Branch("esEnergySCEle", esEnergySCEle, "esEnergySCEle[3]/F");

  tree->Branch("energySCEle_corr", energySCEle_corr, "energySCEle_corr[3]/F");

  tree->Branch("energySCEle_regrCorr_ele", energySCEle_regrCorr_ele, "energySCEle_regrCorr_ele[3]/F");
  tree->Branch("energySCEle_regrCorr_pho", energySCEle_regrCorr_pho, "energySCEle_regrCorr_pho[3]/F");
  tree->Branch("energyEle_regrCorr_fra", energyEle_regrCorr_fra, "energyEle_regrCorr_fra[3]/F");
  tree->Branch("energySigmaEle_regrCorr_fra", energySigmaEle_regrCorr_fra, "energySigmaEle_regrCorr_fra[3]/F");

  tree->Branch("energyEle_regrCorr_egamma", energyEle_regrCorr_egamma, "energyEle_regrCorr_egamma[3]/F");
  tree->Branch("energySigmaEle_regrCorr_egamma", energySigmaEle_regrCorr_egamma, "energySigmaEle_regrCorr_egamma[3]/F");

  tree->Branch("energySigmaSCEle_regrCorr_ele", energySigmaSCEle_regrCorr_ele, "energySigmaSCEle_regrCorr_ele[3]/F");
  tree->Branch("energySigmaSCEle_regrCorr_pho", energySigmaSCEle_regrCorr_pho, "energySigmaSCEle_regrCorr_pho[3]/F");

  // semi parametric regression V4
  tree->Branch("energySCEle_regrCorrSemiParV4_ele", energySCEle_regrCorrSemiParV4_ele, "energySCEle_regrCorrSemiParV4_ele[3]/F");
  tree->Branch("energySCEle_regrCorrSemiParV4_pho", energySCEle_regrCorrSemiParV4_pho, "energySCEle_regrCorrSemiParV4_pho[3]/F");
  tree->Branch("energySCEle_regrCorrSemiParV5_ele", energySCEle_regrCorrSemiParV5_ele, "energySCEle_regrCorrSemiParV5_ele[3]/F");
  tree->Branch("energySCEle_regrCorrSemiParV5_pho", energySCEle_regrCorrSemiParV5_pho, "energySCEle_regrCorrSemiParV5_pho[3]/F");

  tree->Branch("energySigmaSCEle_regrCorrSemiParV4_ele", energySigmaSCEle_regrCorrSemiParV4_ele, "energySigmaSCEle_regrCorrSemiParV4_ele[3]/F");
  tree->Branch("energySigmaSCEle_regrCorrSemiParV4_pho", energySigmaSCEle_regrCorrSemiParV4_pho, "energySigmaSCEle_regrCorrSemiParV4_pho[3]/F");
  tree->Branch("energySigmaSCEle_regrCorrSemiParV5_ele", energySigmaSCEle_regrCorrSemiParV5_ele, "energySigmaSCEle_regrCorrSemiParV5_ele[3]/F");
  tree->Branch("energySigmaSCEle_regrCorrSemiParV5_pho", energySigmaSCEle_regrCorrSemiParV5_pho, "energySigmaSCEle_regrCorrSemiParV5_pho[3]/F");

  tree->Branch("energySCEle_regrCorrSemiParV6_ele", energySCEle_regrCorrSemiParV6_ele, "energySCEle_regrCorrSemiParV6_ele[3]/F");
  tree->Branch("energySCEle_regrCorrSemiParV6_pho", energySCEle_regrCorrSemiParV6_pho, "energySCEle_regrCorrSemiParV6_pho[3]/F");
  tree->Branch("energySigmaSCEle_regrCorrSemiParV6_ele", energySigmaSCEle_regrCorrSemiParV6_ele, "energySigmaSCEle_regrCorrSemiParV6_ele[3]/F");
  tree->Branch("energySigmaSCEle_regrCorrSemiParV6_pho", energySigmaSCEle_regrCorrSemiParV6_pho, "energySigmaSCEle_regrCorrSemiParV6_pho[3]/F");

  tree->Branch("energySCEle_regrCorrSemiParV7_ele", energySCEle_regrCorrSemiParV7_ele, "energySCEle_regrCorrSemiParV7_ele[3]/F");
  tree->Branch("energySCEle_regrCorrSemiParV7_pho", energySCEle_regrCorrSemiParV7_pho, "energySCEle_regrCorrSemiParV7_pho[3]/F");
  tree->Branch("energySigmaSCEle_regrCorrSemiParV7_ele", energySigmaSCEle_regrCorrSemiParV7_ele, "energySigmaSCEle_regrCorrSemiParV7_ele[3]/F");
  tree->Branch("energySigmaSCEle_regrCorrSemiParV7_pho", energySigmaSCEle_regrCorrSemiParV7_pho, "energySigmaSCEle_regrCorrSemiParV7_pho[3]/F");

  tree->Branch("energySCEle_regrCorrSemiParV8_ele", energySCEle_regrCorrSemiParV8_ele, "energySCEle_regrCorrSemiParV8_ele[3]/F");
  tree->Branch("energySCEle_regrCorrSemiParV8_pho", energySCEle_regrCorrSemiParV8_pho, "energySCEle_regrCorrSemiParV8_pho[3]/F");
  tree->Branch("energySigmaSCEle_regrCorrSemiParV8_ele", energySigmaSCEle_regrCorrSemiParV8_ele, "energySigmaSCEle_regrCorrSemiParV8_ele[3]/F");
  tree->Branch("energySigmaSCEle_regrCorrSemiParV8_pho", energySigmaSCEle_regrCorrSemiParV8_pho, "energySigmaSCEle_regrCorrSemiParV8_pho[3]/F");

  tree->Branch("energySCEle_regrCorrSemiPar7TeVtrainV6_ele", energySCEle_regrCorrSemiPar7TeVtrainV6_ele, "energySCEle_regrCorrSemiPar7TeVtrainV6_ele[3]/F");
  tree->Branch("energySCEle_regrCorrSemiPar7TeVtrainV6_pho", energySCEle_regrCorrSemiPar7TeVtrainV6_pho, "energySCEle_regrCorrSemiPar7TeVtrainV6_pho[3]/F");
  tree->Branch("energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_ele", energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_ele, "energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_ele[3]/F");
  tree->Branch("energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_pho", energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_pho, "energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_pho[3]/F");

  tree->Branch("energySCEle_regrCorrSemiPar7TeVtrainV7_ele", energySCEle_regrCorrSemiPar7TeVtrainV7_ele, "energySCEle_regrCorrSemiPar7TeVtrainV7_ele[3]/F");
  tree->Branch("energySCEle_regrCorrSemiPar7TeVtrainV7_pho", energySCEle_regrCorrSemiPar7TeVtrainV7_pho, "energySCEle_regrCorrSemiPar7TeVtrainV7_pho[3]/F");
  tree->Branch("energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_ele", energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_ele, "energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_ele[3]/F");
  tree->Branch("energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_pho", energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_pho, "energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_pho[3]/F");

  tree->Branch("energySCEle_regrCorrSemiPar7TeVtrainV8_ele", energySCEle_regrCorrSemiPar7TeVtrainV8_ele, "energySCEle_regrCorrSemiPar7TeVtrainV8_ele[3]/F");
  tree->Branch("energySCEle_regrCorrSemiPar7TeVtrainV8_pho", energySCEle_regrCorrSemiPar7TeVtrainV8_pho, "energySCEle_regrCorrSemiPar7TeVtrainV8_pho[3]/F");
  tree->Branch("energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_ele", energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_ele, "energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_ele[3]/F");
  tree->Branch("energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_pho", energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_pho, "energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_pho[3]/F");

  tree->Branch("R9Ele", R9Ele, "R9Ele[3]/F");

  tree->Branch("e5x5SCEle", e5x5SCEle, "e5x5SCEle[3]/F");
  //tree->Branch("eSeedSCEle", eSeedSCEle, "eSeedSCEle[3]/F");
  tree->Branch("pModeGsfEle", pModeGsfEle, "pModeGsfEle[3]/F");
  tree->Branch("pAtVtxGsfEle", pAtVtxGsfEle, "pAtVtxGsfEle[3]/F");
  tree->Branch("pNormalizedChi2Ele", pNormalizedChi2Ele, "pNormalizedChi2Ele[3]/F");
  tree->Branch("trackMomentumErrorEle", trackMomentumErrorEle, "trackMomentumErrorEle[3]/F");

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
  tree->Branch("invMass_SC_regrCorrSemiParV4_pho", &invMass_SC_regrCorrSemiParV4_pho, "invMass_SC_regrCorrSemiParV4_pho/F");
  tree->Branch("invMass_SC_regrCorrSemiParV4_ele", &invMass_SC_regrCorrSemiParV4_ele, "invMass_SC_regrCorrSemiParV4_ele/F");
  tree->Branch("invMass_SC_regrCorrSemiParV5_pho", &invMass_SC_regrCorrSemiParV5_pho, "invMass_SC_regrCorrSemiParV5_pho/F");
  tree->Branch("invMass_SC_regrCorrSemiParV5_ele", &invMass_SC_regrCorrSemiParV5_ele, "invMass_SC_regrCorrSemiParV5_ele/F");
  tree->Branch("invMass_SC_regrCorrSemiParV6_pho", &invMass_SC_regrCorrSemiParV6_pho, "invMass_SC_regrCorrSemiParV6_pho/F");
  tree->Branch("invMass_SC_regrCorrSemiParV6_ele", &invMass_SC_regrCorrSemiParV6_ele, "invMass_SC_regrCorrSemiParV6_ele/F");
  tree->Branch("invMass_SC_regrCorrSemiParV7_pho", &invMass_SC_regrCorrSemiParV7_pho, "invMass_SC_regrCorrSemiParV7_pho/F");
  tree->Branch("invMass_SC_regrCorrSemiParV7_ele", &invMass_SC_regrCorrSemiParV7_ele, "invMass_SC_regrCorrSemiParV7_ele/F");
  tree->Branch("invMass_SC_regrCorrSemiParV8_pho", &invMass_SC_regrCorrSemiParV8_pho, "invMass_SC_regrCorrSemiParV8_pho/F");
  tree->Branch("invMass_SC_regrCorrSemiParV8_ele", &invMass_SC_regrCorrSemiParV8_ele, "invMass_SC_regrCorrSemiParV8_ele/F");
  tree->Branch("invMass_SC_regrCorrSemiPar7TeVtrainV6_pho", &invMass_SC_regrCorrSemiPar7TeVtrainV6_pho, "invMass_SC_regrCorrSemiPar7TeVtrainV6_pho/F");
  tree->Branch("invMass_SC_regrCorrSemiPar7TeVtrainV6_ele", &invMass_SC_regrCorrSemiPar7TeVtrainV6_ele, "invMass_SC_regrCorrSemiPar7TeVtrainV6_ele/F");
  tree->Branch("invMass_SC_regrCorrSemiPar7TeVtrainV7_pho", &invMass_SC_regrCorrSemiPar7TeVtrainV7_pho, "invMass_SC_regrCorrSemiPar7TeVtrainV7_pho/F");
  tree->Branch("invMass_SC_regrCorrSemiPar7TeVtrainV7_ele", &invMass_SC_regrCorrSemiPar7TeVtrainV7_ele, "invMass_SC_regrCorrSemiPar7TeVtrainV7_ele/F");
  tree->Branch("invMass_SC_regrCorrSemiPar7TeVtrainV8_pho", &invMass_SC_regrCorrSemiPar7TeVtrainV8_pho, "invMass_SC_regrCorrSemiPar7TeVtrainV8_pho/F");
  tree->Branch("invMass_SC_regrCorrSemiPar7TeVtrainV8_ele", &invMass_SC_regrCorrSemiPar7TeVtrainV8_ele, "invMass_SC_regrCorrSemiPar7TeVtrainV8_ele/F");

  tree->Branch("invMass_MC", &invMass_MC, "invMass_MC/F");
  tree->Branch("invMass_mumu", &invMass_mumu, "invMass_mumu/F");

  tree->Branch("etaMCEle",      etaMCEle,       "etaMCEle[3]/F");	//[nEle]
  tree->Branch("phiMCEle",      phiMCEle,       "phiMCEle[3]/F");	//[nEle]

  //tree->Branch("weight", &weight,  "weight/F");     
  //tree->Branch("r9weight", r9weight,  "r9weight[3]/F");     


  //tree->Branch("iSMEle", iSMEle, "iSMEle[3]/I");
 
  tree->Branch("nHitsSCEle", nHitsSCEle, "nHitsSCEle[3]/I");

#ifdef shervin
  tree->Branch("sigmaIEtaIEtaSCEle", sigmaIEtaIEtaSCEle, "sigmaIEtaIEtaSCEle[3]/F");
  tree->Branch("sigmaIEtaIEtaSCEle", sigmaIEtaIEtaSCEle, "sigmaIEtaIEtaSCEle[3]/F");
  tree->Branch("sigmaIPhiIPhiSCEle", sigmaIPhiIPhiSCEle, "sigmaIPhiIPhiSCEle[3]/F");

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

  if(!hltPaths.empty()){
    edm::TriggerNames HLTNames_ = iEvent.triggerNames(*triggerResultsHandle);
    int hltCount = triggerResultsHandle->size();
    HLTNames[0].clear();
    HLTBits.clear();
    for (int i = 0 ; i < hltCount; ++i) {
      std::string hltName_str(HLTNames_.triggerName(i));
      (HLTNames[0]).push_back(hltName_str);
      (HLTResults[0]).push_back(triggerResultsHandle->accept(i));
      HLTBits.insert(std::pair<std::string, bool>( hltName_str, triggerResultsHandle->accept(i)));
    } // for i
  }

  return;
}    


void ZNtupleDumper::TreeSetPileupVar(void){
  rho=*rhoHandle;
  nPV=0;
  nPU[0]=-1;
  mcGenWeight=-1;

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
	//nPU[0]=PVI->getPU_NumInteractions();
	nPU[0]=PVI->getTrueNumInteractions();
      }
    }
  
    if(!GenEventInfoHandle->weights().empty())
      mcGenWeight=(GenEventInfoHandle->weights())[0];

  } // else {
  //     //weight= 1.;
  //     nPU[0]=-1;
  //     mcGenWeight=-1;
  //   }
  return;
}



DetId ZNtupleDumper::findSCseed(const reco::SuperCluster& cluster){
  DetId seedDetId;
  float seedEnergy=0;
  std::vector< std::pair<DetId, float> > hitsFractions = cluster.hitsAndFractions();
  for(std::vector< std::pair<DetId, float> >::const_iterator hitsAndFractions_itr = hitsFractions.begin();
      hitsAndFractions_itr != hitsFractions.end();
      hitsAndFractions_itr++){
    if(hitsAndFractions_itr->second > seedEnergy)
      seedDetId=hitsAndFractions_itr->first;
  }
#ifdef DEBUG
  std::cout << "[DEBUG findSCseed] seedDetIt: " << seedDetId.rawId() << std::endl
            << cluster << std::endl
            << *(cluster.seed()) << std::endl;
#endif
  if(seedDetId.null()){
    std::cerr << "[ERROR] Invalid detID: " << cluster << std::endl;
  }
  return seedDetId;
}


// a negative index means that the corresponding electron does not exist, fill with 0
void ZNtupleDumper::TreeSetSingleElectronVar(const pat::Electron& electron1, int index){

  if(index<0){
    PtEle[-index] 	  = 0;  
    chargeEle[-index] = -100;
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
  //assert(recHits!=NULL);
  const edm::ESHandle<EcalLaserDbService>& laserHandle_ = clustertools->getLaserHandle();
  
  DetId seedDetId = electron1.superCluster()->seed()->seed();
  if(seedDetId.null()){
    //assert(electron1.trackerDrivenSeed()); // DEBUG
    seedDetId = findSCseed(*(electron1.superCluster()));
  }

  if(electron1.isEB() && seedDetId.subdetId() == EcalBarrel){
    EBDetId seedDetIdEcal = seedDetId;
    seedXSCEle[index]=seedDetIdEcal.ieta();
    seedYSCEle[index]=seedDetIdEcal.iphi();
  }else if(electron1.isEE() && seedDetId.subdetId() == EcalEndcap){
    EEDetId seedDetIdEcal = seedDetId;
    seedXSCEle[index]=seedDetIdEcal.ix();
    seedYSCEle[index]=seedDetIdEcal.iy();
  }else{ ///< this case is strange but happens for trackerDriven electrons
    seedXSCEle[index]=0;
    seedYSCEle[index]=0;
  }

  EcalRecHitCollection::const_iterator seedRecHit = recHits->find(seedDetId) ;
  //assert(seedRecHit!=recHits->end()); // DEBUG
  seedEnergySCEle[index]=seedRecHit->energy();
  if(isMC) seedLCSCEle[index]=-10;
  else seedLCSCEle[index]=laserHandle_->getLaserCorrection(seedDetId,runTime_);

  if(seedRecHit->checkFlag(EcalRecHit::kHasSwitchToGain6)) gainEle[index]=1;
  else if(seedRecHit->checkFlag(EcalRecHit::kHasSwitchToGain1)) gainEle[index]=2;
  else gainEle[index]=0;
  
  float sumLC_E = 0.;
  float sumE = 0.;
  if( !isMC){
    std::vector< std::pair<DetId, float> > hitsAndFractions_ele1 = electron1.superCluster()->hitsAndFractions();
    for (std::vector<std::pair<DetId, float> >::const_iterator detitr = hitsAndFractions_ele1.begin();
	 detitr != hitsAndFractions_ele1.end(); detitr++ )
      {
	EcalRecHitCollection::const_iterator oneHit = recHits->find( (detitr -> first) ) ;
	//assert(oneHit!=recHits->end()); // DEBUG
	sumLC_E += laserHandle_->getLaserCorrection(detitr->first, runTime_) * oneHit->energy();
	sumE    += oneHit->energy();
      }
    avgLCSCEle[index] = sumLC_E / sumE;

  } else     avgLCSCEle[index] = -10;
  
  nHitsSCEle[index] = electron1.superCluster()->size();

  //  sigmaIEtaIEtaSCEle[index] = sqrt(clustertools->localCovariances(*(electron1.superCluster()->seed()))[index]);
  //  sigmaIEtaIEtaSCEle[1] = sqrt(clustertools->localCovariances(*(electron2.superCluster()->seed()))[index]);
  //  sigmaIPhiIPhiBC = sqrt(clustertools->localCovariances(*b)[3]);
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

#ifdef CMSSW_7_2_X
  energySCEle_regrCorr_pho[index] = -1;
  energySCEle_regrCorr_ele[index] = -1;
  energyEle_regrCorr_fra[index] = -1;
  energyEle_regrCorr_egamma[index] = -1;
  energySigmaSCEle_regrCorr_pho[index] = -1;
  energySigmaSCEle_regrCorr_ele[index] = -1;
  energySigmaEle_regrCorr_fra[index] = -1;
  energySigmaEle_regrCorr_egamma[index] =  -1;
  energySCEle_regrCorrSemiParV4_pho[index] =  -1;
  energySCEle_regrCorrSemiParV4_ele[index] = -1;
  energySigmaSCEle_regrCorrSemiParV4_pho[index] = -1;
  energySigmaSCEle_regrCorrSemiParV4_ele[index] = -1;
  energySCEle_regrCorrSemiParV5_pho[index] =  -1;
  energySCEle_regrCorrSemiParV5_ele[index] = -1;
  energySigmaSCEle_regrCorrSemiParV5_pho[index] = -1;
  energySigmaSCEle_regrCorrSemiParV5_ele[index] = -1;
  energySCEle_regrCorrSemiParV6_pho[index] = -1;
  energySCEle_regrCorrSemiParV6_ele[index] = -1;
  energySigmaSCEle_regrCorrSemiParV6_pho[index] = -1;
  energySigmaSCEle_regrCorrSemiParV6_ele[index] = -1;
  energySCEle_regrCorrSemiParV7_pho[index] = -1;
  energySCEle_regrCorrSemiParV7_ele[index] = -1;
  energySigmaSCEle_regrCorrSemiParV7_pho[index] = -1;
  energySigmaSCEle_regrCorrSemiParV7_ele[index] = -1;
  energySCEle_regrCorrSemiParV8_pho[index] = -1;
  energySCEle_regrCorrSemiParV8_ele[index] = -1;
  energySigmaSCEle_regrCorrSemiParV8_pho[index] = -1;
  energySigmaSCEle_regrCorrSemiParV8_ele[index] = -1;
  energySCEle_regrCorrSemiPar7TeVtrainV6_pho[index] = -1;
  energySCEle_regrCorrSemiPar7TeVtrainV6_ele[index] = -1;
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_pho[index] = -1;
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_ele[index] = -1;
  energySCEle_regrCorrSemiPar7TeVtrainV7_pho[index] = -1;
  energySCEle_regrCorrSemiPar7TeVtrainV7_ele[index] = -1;
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_pho[index] = -1;
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_ele[index] = -1;
  energySCEle_regrCorrSemiPar7TeVtrainV8_pho[index] = -1;
  energySCEle_regrCorrSemiPar7TeVtrainV8_ele[index] = -1;
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_pho[index] = -1;
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_ele[index] = -1;
#else
  energySCEle_regrCorr_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPho");
  energySCEle_regrCorr_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEle");
  energyEle_regrCorr_fra[index] = electron1.userFloat("eleNewEnergiesProducer:energyEleFra");
  energyEle_regrCorr_egamma[index] = electron1.userFloat("eleRegressionEnergy:eneRegForGsfEle");

  energySigmaSCEle_regrCorr_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoVar");
  energySigmaSCEle_regrCorr_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEleVar");
  energySigmaEle_regrCorr_fra[index] = electron1.userFloat("eleNewEnergiesProducer:energyEleFraVar");
  energySigmaEle_regrCorr_egamma[index] = electron1.userFloat("eleRegressionEnergy:eneErrorRegForGsfEle");

  energySCEle_regrCorrSemiParV4_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV4ecorr");
  energySCEle_regrCorrSemiParV4_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV4ecorr");
  energySigmaSCEle_regrCorrSemiParV4_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV4sigma");
  energySigmaSCEle_regrCorrSemiParV4_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV4sigma");

  energySCEle_regrCorrSemiParV5_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV5ecorr");
  energySCEle_regrCorrSemiParV5_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV5ecorr");
  energySigmaSCEle_regrCorrSemiParV5_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV5sigma");
  energySigmaSCEle_regrCorrSemiParV5_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV5sigma");

  energySCEle_regrCorrSemiParV6_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV6ecorr");
  energySCEle_regrCorrSemiParV6_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV6ecorr");
  energySigmaSCEle_regrCorrSemiParV6_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV6sigmaEoverE");
  energySigmaSCEle_regrCorrSemiParV6_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV6sigmaEoverE");

  energySCEle_regrCorrSemiParV7_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV7ecorr");
  energySCEle_regrCorrSemiParV7_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV7ecorr");
  energySigmaSCEle_regrCorrSemiParV7_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV7sigmaEoverE");
  energySigmaSCEle_regrCorrSemiParV7_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV7sigmaEoverE");

  energySCEle_regrCorrSemiParV8_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV8ecorr");
  energySCEle_regrCorrSemiParV8_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV8ecorr");
  energySigmaSCEle_regrCorrSemiParV8_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV8sigmaEoverE");
  energySigmaSCEle_regrCorrSemiParV8_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV8sigmaEoverE");


  energySCEle_regrCorrSemiPar7TeVtrainV6_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParam7TeVtrainV6ecorr");
  energySCEle_regrCorrSemiPar7TeVtrainV6_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParam7TeVtrainV6ecorr");
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParam7TeVtrainV6sigmaEoverE");
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParam7TeVtrainV6sigmaEoverE");

  energySCEle_regrCorrSemiPar7TeVtrainV7_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParam7TeVtrainV7ecorr");
  energySCEle_regrCorrSemiPar7TeVtrainV7_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParam7TeVtrainV7ecorr");
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParam7TeVtrainV7sigmaEoverE");
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParam7TeVtrainV7sigmaEoverE");

  energySCEle_regrCorrSemiPar7TeVtrainV8_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParam7TeVtrainV8ecorr");
  energySCEle_regrCorrSemiPar7TeVtrainV8_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParam7TeVtrainV8ecorr");
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_pho[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParam7TeVtrainV8sigmaEoverE");
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_ele[index] = electron1.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParam7TeVtrainV8sigmaEoverE");
#endif

  // change in an electron properties please, EleNewEnergyProducer
  e3x3SCEle[index] = clustertools->e3x3(*electron1.superCluster()->seed());
  e5x5SCEle[index] = clustertools->e5x5(*electron1.superCluster()->seed());
  eSeedSCEle[index]= electron1.superCluster()->seed()->energy();

  pModeGsfEle[index] = electron1.gsfTrack()->pMode();
  trackMomentumErrorEle[index] = electron1.trackMomentumError();
  pNormalizedChi2Ele[index] = electron1.gsfTrack()->normalizedChi2();
  pAtVtxGsfEle[index] = electron1.trackMomentumAtVtx().R();

  R9Ele[index] = e3x3SCEle[index]/electron1.superCluster()->rawEnergy();

  //   if(isMC){
  //     if(electron1.isEB()) 
  //       R9Ele[index] = R9Ele[index]*1.0045+0.0010;
  //     else 
  //       R9Ele[index] = R9Ele[index]*1.0086-0.0007;
  //   } 

  // make it a function
  eleID[index] = ((bool) electron1.electronID("fiducial")) << 0;
  eleID[index] += ((bool) electron1.electronID("loose")) << 1;
  eleID[index] += ((bool) electron1.electronID("medium")) << 2;
  eleID[index] += ((bool) electron1.electronID("tight")) << 3;
  eleID[index] += ((bool) electron1.electronID("WP90PU")) << 4;
  eleID[index] += ((bool) electron1.electronID("WP80PU")) << 5;
  eleID[index] += ((bool) electron1.electronID("WP70PU")) << 6;
  //LUCA: need to decide if to put the run2 selection here (bits 7-12) or not. Also, need to modify the ElectronCategory_class according to this..
  eleID[index] += ((bool) electron1.electronID("loose25nsRun2")) << 7;  
  eleID[index] += ((bool) electron1.electronID("medium25nsRun2")) << 8;
  eleID[index] += ((bool) electron1.electronID("tight25nsRun2")) << 9;
  eleID[index] += ((bool) electron1.electronID("loose50nsRun2")) << 10;
  eleID[index] += ((bool) electron1.electronID("medium50nsRun2")) << 11;
  eleID[index] += ((bool) electron1.electronID("tight50nsRun2")) << 12;

  classificationEle[index] = electron1.classification();

  return;
}


// a negative index means that the corresponding electron does not exist, fill with 0
void ZNtupleDumper::TreeSetSingleElectronVar(const reco::SuperCluster& electron1, int index){

  if(index<0){
    PtEle[-index] 	  = 0;
    chargeEle[-index] = -100;
    etaEle[-index]    = 0;
    phiEle[-index]    = 0;
    return;
  }

  //checks

  PtEle[index]     = electron1.energy()/cosh(electron1.eta());
  chargeEle[index] = 0; // dont know the charge for SC
  etaEle[index]    = electron1.eta(); // eta = etaSC
  phiEle[index]    = electron1.phi();

  recoFlagsEle[index] = -1; // define -1 as a SC

  fbremEle[index] = -1; // no bremstrahlung for SC 

  etaSCEle[index] = electron1.eta(); // itself is a SC
  phiSCEle[index] = electron1.phi(); 

  const EcalRecHitCollection *recHitsEE = clustertools->getEcalEERecHitCollection();

  const edm::ESHandle<EcalLaserDbService>& laserHandle_ = clustertools->getLaserHandle();

  DetId seedDetId = electron1.seed()->seed();
  if(seedDetId.null()){
    seedDetId = findSCseed(electron1);
  }
  assert(seedDetId.subdetId() == EcalEndcap);
  assert(!seedDetId.null());
  EEDetId seedDetIdEcal = seedDetId;
  seedXSCEle[index]=seedDetIdEcal.ix();
  seedYSCEle[index]=seedDetIdEcal.iy();
  
  EcalRecHitCollection::const_iterator seedRecHit = recHitsEE->find(seedDetId) ;
  assert(seedRecHit!=recHitsEE->end());
  seedEnergySCEle[index]=seedRecHit->energy();
  if(seedRecHit->checkFlag(EcalRecHit::kHasSwitchToGain6)) gainEle[index]=1;
  else if(seedRecHit->checkFlag(EcalRecHit::kHasSwitchToGain1)) gainEle[index]=2;
  else gainEle[index]=0;

  if(isMC) seedLCSCEle[index]=-10;
  else seedLCSCEle[index]=laserHandle_->getLaserCorrection(seedDetId,runTime_);
 
  float sumLC_E = 0.;
  float sumE = 0.;
  if( !isMC){
    std::vector< std::pair<DetId, float> > hitsAndFractions_ele1 = electron1.hitsAndFractions();
    for (std::vector<std::pair<DetId, float> >::const_iterator detitr = hitsAndFractions_ele1.begin();
	 detitr != hitsAndFractions_ele1.end(); detitr++ ){
      
      double hitenergy = 0;
      EcalRecHitCollection::const_iterator oneHit = recHitsEE->find( (detitr -> first) ) ;
      hitenergy = oneHit->energy();

      sumLC_E += laserHandle_->getLaserCorrection(detitr->first, runTime_) * hitenergy;
      sumE    += hitenergy;
    }
    avgLCSCEle[index] = sumLC_E / sumE;
    
  } else     avgLCSCEle[index] = -10;

  nHitsSCEle[index] = electron1.size();

  // no MC matching has been considered yet for SCV
  energyMCEle[index]=-100;
  etaMCEle[index]=-100;
  phiMCEle[index]=-100;

  energySCEle[index]  = electron1.energy();
  rawEnergySCEle[index]  = electron1.rawEnergy();
  esEnergySCEle[index] = electron1.preshowerEnergy();
  energySCEle_corr[index] = electron1.energy();

  // for the regression energies above, temporarily only use SC energy because the training hasn't been done yet
  energySCEle_regrCorr_pho[index] = electron1.energy();
  energySCEle_regrCorr_ele[index] = electron1.energy();
  energyEle_regrCorr_fra[index] = 1;
  energyEle_regrCorr_egamma[index] = electron1.energy();
  energySigmaSCEle_regrCorr_pho[index] = electron1.energy();
  energySigmaSCEle_regrCorr_ele[index] = electron1.energy();
  energySigmaEle_regrCorr_fra[index] = 1;
  energySigmaEle_regrCorr_egamma[index] =  electron1.energy();
  energySCEle_regrCorrSemiParV4_pho[index] =  electron1.energy();
  energySCEle_regrCorrSemiParV4_ele[index] = electron1.energy();
  energySigmaSCEle_regrCorrSemiParV4_pho[index] = -1;
  energySigmaSCEle_regrCorrSemiParV4_ele[index] = -1;
  energySCEle_regrCorrSemiParV5_pho[index] =  electron1.energy();
  energySCEle_regrCorrSemiParV5_ele[index] = electron1.energy();
  energySigmaSCEle_regrCorrSemiParV5_pho[index] = -1;
  energySigmaSCEle_regrCorrSemiParV5_ele[index] = -1;
  energySCEle_regrCorrSemiParV6_pho[index] = electron1.energy();
  energySCEle_regrCorrSemiParV6_ele[index] = electron1.energy();
  energySigmaSCEle_regrCorrSemiParV6_pho[index] = -1;
  energySigmaSCEle_regrCorrSemiParV6_ele[index] = -1;
  energySCEle_regrCorrSemiParV7_pho[index] = electron1.energy();
  energySCEle_regrCorrSemiParV7_ele[index] = electron1.energy();
  energySigmaSCEle_regrCorrSemiParV7_pho[index] = -1;
  energySigmaSCEle_regrCorrSemiParV7_ele[index] = -1;
  energySCEle_regrCorrSemiParV8_pho[index] = electron1.energy();
  energySCEle_regrCorrSemiParV8_ele[index] = electron1.energy();
  energySigmaSCEle_regrCorrSemiParV8_pho[index] = -1;
  energySigmaSCEle_regrCorrSemiParV8_ele[index] = -1;
  energySCEle_regrCorrSemiPar7TeVtrainV6_pho[index] = electron1.energy();
  energySCEle_regrCorrSemiPar7TeVtrainV6_ele[index] = electron1.energy();
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_pho[index] = -1;
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_ele[index] = -1;
  energySCEle_regrCorrSemiPar7TeVtrainV7_pho[index] = electron1.energy();
  energySCEle_regrCorrSemiPar7TeVtrainV7_ele[index] = electron1.energy();
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_pho[index] = -1;
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_ele[index] = -1;
  energySCEle_regrCorrSemiPar7TeVtrainV8_pho[index] = electron1.energy();
  energySCEle_regrCorrSemiPar7TeVtrainV8_ele[index] = electron1.energy();
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_pho[index] = -1;
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_ele[index] = -1;

  // change in an electron properties please, EleNewEnergyProducer
  e3x3SCEle[index] = clustertools->e3x3(*electron1.seed());
  e5x5SCEle[index] = clustertools->e5x5(*electron1.seed());
  eSeedSCEle[index]= electron1.seed()->energy();

  pModeGsfEle[index] = -1; // no track, though ..
  trackMomentumErrorEle[index] = -1;
  pNormalizedChi2Ele[index] = -1; 
  pAtVtxGsfEle[index] = -1;

  R9Ele[index] = e3x3SCEle[index]/electron1.rawEnergy();


  // make it a function
  //eleID[index] = ((bool) electron1.electronID("fiducial")) << 0;
  //eleID[index] += ((bool) electron1.electronID("loose")) << 1;
  //eleID[index] += ((bool) electron1.electronID("medium")) << 2;
  //eleID[index] += ((bool) electron1.electronID("tight")) << 3;
  //eleID[index] += ((bool) electron1.electronID("WP90PU")) << 4;
  //eleID[index] += ((bool) electron1.electronID("WP80PU")) << 5;
  //eleID[index] += ((bool) electron1.electronID("WP70PU")) << 6;
  //classificationEle[index] = electron1.classification();

  // temporary ignor the id and classification
  eleID[index] = -100;
  classificationEle[index] = -100;

  return;
}

// a negative index means that the corresponding muon does not exist, fill with 0
void ZNtupleDumper::TreeSetSingleMuonVar(const pat::Muon& muon1, int index){

  if(index<0){
    PtEle[-index] 	  = 0;  
    chargeEle[-index] = -100;
    etaEle[-index]    = 0; 
    phiEle[-index]    = 0;
    return;
  }   

  PtEle[index]     = muon1.pt();  
  chargeEle[index] = muon1.charge();
  etaEle[index]    = muon1.eta(); // degli elettroni
  phiEle[index]    = muon1.phi();


  if(muon1.genLepton()!=0){
    energyMCEle[index]  = muon1.genLepton()->energy();
    etaMCEle[index]  = muon1.genLepton()->eta();
    phiMCEle[index]  = muon1.genLepton()->phi();
  } else {
    energyMCEle[index]=0;
    etaMCEle[index]=0;
    phiMCEle[index]=0;
  }


  // why the hell this does not work????
  //  eleID[index] = ((bool) muon1.muonID("soft")) << 0;
  //  eleID[index] += ((bool) muon1.muonID("loose")) << 1;
  //  eleID[index] += ((bool) muon1.muonID("highPT")) << 2;
  //  eleID[index] += ((bool) muon1.muonID("tight")) << 3;

  //  classificationEle[index] = muon1.classification();

  return;
}


// a negative index means that the corresponding electron does not exist, fill with 0
void ZNtupleDumper::TreeSetSinglePhotonVar(const pat::Photon& photon, int index){

  if(index<0){
    PtEle[-index] 	  = 0;  
    chargeEle[-index] = -100;
    etaEle[-index]    = 0; 
    phiEle[-index]    = 0;
    return;
  }   

  PtEle[index]     = photon.energy()/cosh(photon.eta());
  chargeEle[index] = photon.charge();
  etaEle[index]    = photon.eta(); 
  phiEle[index]    = photon.phi();

  etaSCEle[index] = photon.superCluster()->eta();
  phiSCEle[index] = photon.superCluster()->phi();

  const EcalRecHitCollection *recHits = (photon.isEB()) ?  clustertools->getEcalEBRecHitCollection() : clustertools->getEcalEERecHitCollection();
  //assert(recHits!=NULL);
  const edm::ESHandle<EcalLaserDbService>& laserHandle_ = clustertools->getLaserHandle();
  
  DetId seedDetId = photon.superCluster()->seed()->seed();
  if(seedDetId.null()){
    //assert(photon.trackerDrivenSeed()); // DEBUG
    seedDetId = findSCseed(*(photon.superCluster()));
  }

  if(photon.isEB() && seedDetId.subdetId() == EcalBarrel){
    EBDetId seedDetIdEcal = seedDetId;
    seedXSCEle[index]=seedDetIdEcal.ieta();
    seedYSCEle[index]=seedDetIdEcal.iphi();
  }else if(photon.isEE() && seedDetId.subdetId() == EcalEndcap){
    EEDetId seedDetIdEcal = seedDetId;
    seedXSCEle[index]=seedDetIdEcal.ix();
    seedYSCEle[index]=seedDetIdEcal.iy();
  }else{ ///< this case is strange but happens for trackerDriven electrons
    seedXSCEle[index]=0;
    seedYSCEle[index]=0;
  }

  EcalRecHitCollection::const_iterator seedRecHit = recHits->find(seedDetId) ;
  //assert(seedRecHit!=recHits->end()); // DEBUG
  seedEnergySCEle[index]=seedRecHit->energy();
  if(isMC) seedLCSCEle[index]=-10;
  else seedLCSCEle[index]=laserHandle_->getLaserCorrection(seedDetId,runTime_);

  if(seedRecHit->checkFlag(EcalRecHit::kHasSwitchToGain6)) gainEle[index]=1;
  else if(seedRecHit->checkFlag(EcalRecHit::kHasSwitchToGain1)) gainEle[index]=2;
  else gainEle[index]=0;
  
  float sumLC_E = 0.;
  float sumE = 0.;
  if( !isMC){
    std::vector< std::pair<DetId, float> > hitsAndFractions_ele1 = photon.superCluster()->hitsAndFractions();
    for (std::vector<std::pair<DetId, float> >::const_iterator detitr = hitsAndFractions_ele1.begin();
	 detitr != hitsAndFractions_ele1.end(); detitr++ )
      {
	EcalRecHitCollection::const_iterator oneHit = recHits->find( (detitr -> first) ) ;
	//assert(oneHit!=recHits->end()); // DEBUG
	sumLC_E += laserHandle_->getLaserCorrection(detitr->first, runTime_) * oneHit->energy();
	sumE    += oneHit->energy();
      }
    avgLCSCEle[index] = sumLC_E / sumE;

  } else     avgLCSCEle[index] = -10;
  
  nHitsSCEle[index] = photon.superCluster()->size();

  //  sigmaIEtaIEtaSCEle[index] = sqrt(clustertools->localCovariances(*(photon.superCluster()->seed()))[index]);
  //  sigmaIEtaIEtaSCEle[1] = sqrt(clustertools->localCovariances(*(electron2.superCluster()->seed()))[index]);
  //  sigmaIPhiIPhiBC = sqrt(clustertools->localCovariances(*b)[3]);
  //  sigmaIEtaIPhiBC = clustertools->localCovariances(*b)[1]; 

  if(photon.genParticle()!=0){
    energyMCEle[index]  = photon.genParticle()->energy();
    etaMCEle[index]  = photon.genParticle()->eta();
    phiMCEle[index]  = photon.genParticle()->phi();
  } else {
    energyMCEle[index]=0;
    etaMCEle[index]=0;
    phiMCEle[index]=0;
  }

  energySCEle[index]  = photon.superCluster()->energy();
  rawEnergySCEle[index]  = photon.superCluster()->rawEnergy();
  esEnergySCEle[index] = photon.superCluster()->preshowerEnergy();
  //  energySCEle_corr[index] = photon.scEcalEnergy(); //but, I don't think this is the correct energy..

  energySCEle_regrCorr_pho[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshPho");
  energySCEle_regrCorr_ele[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshEle");
  energyEle_regrCorr_fra[index] = photon.userFloat("eleNewEnergiesProducer:energyEleFra");
  energyEle_regrCorr_egamma[index] = photon.userFloat("eleRegressionEnergy:eneRegForGsfEle");

  energySigmaSCEle_regrCorr_pho[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoVar");
  energySigmaSCEle_regrCorr_ele[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshEleVar");
  energySigmaEle_regrCorr_fra[index] = photon.userFloat("eleNewEnergiesProducer:energyEleFraVar");
  energySigmaEle_regrCorr_egamma[index] = photon.userFloat("eleRegressionEnergy:eneErrorRegForGsfEle");

  energySCEle_regrCorrSemiParV4_pho[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV4ecorr");
  energySCEle_regrCorrSemiParV4_ele[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV4ecorr");
  energySigmaSCEle_regrCorrSemiParV4_pho[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV4sigma");
  energySigmaSCEle_regrCorrSemiParV4_ele[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV4sigma");

  energySCEle_regrCorrSemiParV5_pho[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV5ecorr");
  energySCEle_regrCorrSemiParV5_ele[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV5ecorr");
  energySigmaSCEle_regrCorrSemiParV5_pho[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV5sigma");
  energySigmaSCEle_regrCorrSemiParV5_ele[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV5sigma");

  energySCEle_regrCorrSemiParV6_pho[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV6ecorr");
  energySCEle_regrCorrSemiParV6_ele[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV6ecorr");
  energySigmaSCEle_regrCorrSemiParV6_pho[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV6sigmaEoverE");
  energySigmaSCEle_regrCorrSemiParV6_ele[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV6sigmaEoverE");

  energySCEle_regrCorrSemiParV7_pho[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV7ecorr");
  energySCEle_regrCorrSemiParV7_ele[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV7ecorr");
  energySigmaSCEle_regrCorrSemiParV7_pho[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV7sigmaEoverE");
  energySigmaSCEle_regrCorrSemiParV7_ele[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV7sigmaEoverE");

  energySCEle_regrCorrSemiParV8_pho[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV8ecorr");
  energySCEle_regrCorrSemiParV8_ele[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV8ecorr");
  energySigmaSCEle_regrCorrSemiParV8_pho[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParamV8sigmaEoverE");
  energySigmaSCEle_regrCorrSemiParV8_ele[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParamV8sigmaEoverE");


  energySCEle_regrCorrSemiPar7TeVtrainV6_pho[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParam7TeVtrainV6ecorr");
  energySCEle_regrCorrSemiPar7TeVtrainV6_ele[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParam7TeVtrainV6ecorr");
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_pho[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParam7TeVtrainV6sigmaEoverE");
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV6_ele[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParam7TeVtrainV6sigmaEoverE");

  energySCEle_regrCorrSemiPar7TeVtrainV7_pho[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParam7TeVtrainV7ecorr");
  energySCEle_regrCorrSemiPar7TeVtrainV7_ele[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParam7TeVtrainV7ecorr");
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_pho[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParam7TeVtrainV7sigmaEoverE");
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV7_ele[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParam7TeVtrainV7sigmaEoverE");

  energySCEle_regrCorrSemiPar7TeVtrainV8_pho[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParam7TeVtrainV8ecorr");
  energySCEle_regrCorrSemiPar7TeVtrainV8_ele[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParam7TeVtrainV8ecorr");
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_pho[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshPhoSemiParam7TeVtrainV8sigmaEoverE");
  energySigmaSCEle_regrCorrSemiPar7TeVtrainV8_ele[index] = photon.userFloat("eleNewEnergiesProducer:energySCEleJoshEleSemiParam7TeVtrainV8sigmaEoverE");

  // change in an electron properties please, EleNewEnergyProducer
  e3x3SCEle[index] = clustertools->e3x3(*photon.superCluster()->seed());
  e5x5SCEle[index] = clustertools->e5x5(*photon.superCluster()->seed());
  eSeedSCEle[index]= photon.superCluster()->seed()->energy();

  R9Ele[index] = e3x3SCEle[index]/photon.superCluster()->rawEnergy();

  //   if(isMC){
  //     if(photon.isEB()) 
  //       R9Ele[index] = R9Ele[index]*1.0045+0.0010;
  //     else 
  //       R9Ele[index] = R9Ele[index]*1.0086-0.0007;
  //   } 

  eleID[index] = ((bool) photon.photonID("fiducial")) << 0;
  eleID[index] += ((bool) photon.photonID("loose")) << 1;
  eleID[index] += ((bool) photon.photonID("medium")) << 2;
  eleID[index] += ((bool) photon.photonID("tight")) << 3;

  return;
}


void ZNtupleDumper:: TreeSetDiElectronVar(const pat::Electron& electron1, const pat::Electron& electron2){
  
  TreeSetSingleElectronVar(electron1, 0);
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

  invMass_SC_regrCorrSemiParV4_ele = sqrt(2* energySCEle_regrCorrSemiParV4_ele[0] * energySCEle_regrCorrSemiParV4_ele[1] *
					  angle);
  invMass_SC_regrCorrSemiParV4_pho = sqrt(2* energySCEle_regrCorrSemiParV4_pho[0] * energySCEle_regrCorrSemiParV4_pho[1] *
					  angle);
  invMass_SC_regrCorrSemiParV5_ele = sqrt(2* energySCEle_regrCorrSemiParV5_ele[0] * energySCEle_regrCorrSemiParV5_ele[1] *
					  angle);
  invMass_SC_regrCorrSemiParV5_pho = sqrt(2* energySCEle_regrCorrSemiParV5_pho[0] * energySCEle_regrCorrSemiParV5_pho[1] *
					  angle);
  invMass_SC_regrCorrSemiParV6_ele = sqrt(2* energySCEle_regrCorrSemiParV6_ele[0] * energySCEle_regrCorrSemiParV6_ele[1] *
					  angle);
  invMass_SC_regrCorrSemiParV6_pho = sqrt(2* energySCEle_regrCorrSemiParV6_pho[0] * energySCEle_regrCorrSemiParV6_pho[1] *
					  angle);
  invMass_SC_regrCorrSemiParV7_ele = sqrt(2* energySCEle_regrCorrSemiParV7_ele[0] * energySCEle_regrCorrSemiParV7_ele[1] *
					  angle);
  invMass_SC_regrCorrSemiParV7_pho = sqrt(2* energySCEle_regrCorrSemiParV7_pho[0] * energySCEle_regrCorrSemiParV7_pho[1] *
					  angle);
  invMass_SC_regrCorrSemiParV8_ele = sqrt(2* energySCEle_regrCorrSemiParV8_ele[0] * energySCEle_regrCorrSemiParV8_ele[1] *
					  angle);
  invMass_SC_regrCorrSemiParV8_pho = sqrt(2* energySCEle_regrCorrSemiParV8_pho[0] * energySCEle_regrCorrSemiParV8_pho[1] *
					  angle);

  invMass_SC_regrCorrSemiPar7TeVtrainV6_ele = sqrt(2* energySCEle_regrCorrSemiPar7TeVtrainV6_ele[0] * energySCEle_regrCorrSemiPar7TeVtrainV6_ele[1] *
						   angle);
  invMass_SC_regrCorrSemiPar7TeVtrainV6_pho = sqrt(2* energySCEle_regrCorrSemiPar7TeVtrainV6_pho[0] * energySCEle_regrCorrSemiPar7TeVtrainV6_pho[1] *
						   angle);
  invMass_SC_regrCorrSemiPar7TeVtrainV7_ele = sqrt(2* energySCEle_regrCorrSemiPar7TeVtrainV7_ele[0] * energySCEle_regrCorrSemiPar7TeVtrainV7_ele[1] *
						   angle);
  invMass_SC_regrCorrSemiPar7TeVtrainV7_pho = sqrt(2* energySCEle_regrCorrSemiPar7TeVtrainV7_pho[0] * energySCEle_regrCorrSemiPar7TeVtrainV7_pho[1] *
						   angle);
  invMass_SC_regrCorrSemiPar7TeVtrainV8_ele = sqrt(2* energySCEle_regrCorrSemiPar7TeVtrainV8_ele[0] * energySCEle_regrCorrSemiPar7TeVtrainV8_ele[1] *
						   angle);
  invMass_SC_regrCorrSemiPar7TeVtrainV8_pho = sqrt(2* energySCEle_regrCorrSemiPar7TeVtrainV8_pho[0] * energySCEle_regrCorrSemiPar7TeVtrainV8_pho[1] *
						   angle);

  invMass_regrCorr_fra = sqrt(2* energyEle_regrCorr_fra[0] * energyEle_regrCorr_fra[1] *angle);
  invMass_regrCorr_egamma = sqrt(2* energyEle_regrCorr_egamma[0] * energyEle_regrCorr_egamma[1] *angle);

  if(electron1.genLepton()!=0 && electron2.genLepton()!=0){
    invMass_MC     = sqrt(2*electron1.genLepton()->energy() *electron2.genLepton()->energy() *
			  angle);
  } else invMass_MC = 0;
  //  invMass_genMC     = (electron1.genLepton()->p4 + electron2.genLepton()->p4()).M();
			    

  // se non hanno fatto match con il MC?
  // qual e' la frazione di Z selezionate che non matchano il MC?

  //#ifdef shervin
  //  r9weight[0]=r9Weight(etaEle[0], R9Ele[0]);
  //  r9weight[1]=r9Weight(etaEle[1], R9Ele[1]);
  //#endif
 
  return;
}

void ZNtupleDumper::TreeSetDiElectronVar(const pat::Electron& electron1, const reco::SuperCluster& electron2){
  
  TreeSetSingleElectronVar(electron1, 0);
  TreeSetSingleElectronVar(electron2, 1);

  double t1=TMath::Exp(-etaEle[0]);
  double t2=TMath::Exp(-etaEle[1]);
  double t1q = t1*t1;
  double t2q = t2*t2;

  double angle=1- ( (1-t1q)*(1-t2q)+4*t1*t2*cos(phiEle[0]-phiEle[1]))/( (1+t1q)*(1+t2q) );


  invMass = sqrt(2*electron1.energy()*electron2.energy() *angle);
  invMass_e5x5   = sqrt(2*electron1.e5x5()*(clustertools->e5x5(*electron2.seed())) * angle);

  invMass_SC = sqrt(2*energySCEle[0]*energySCEle[1] *  angle);


  invMass_rawSC = sqrt(2 * rawEnergySCEle[0] * rawEnergySCEle[1] * angle);


  invMass_rawSC_esSC = sqrt(2 * (rawEnergySCEle[0] + esEnergySCEle[0]) *
			    (rawEnergySCEle[1] + esEnergySCEle[1]) *
			    angle);

  invMass_SC_corr = sqrt(2*energySCEle_corr[0]*energySCEle_corr[1] *
			 angle);

  invMass_SC_regrCorr_ele = sqrt(2* energySCEle_regrCorr_ele[0] * energySCEle_regrCorr_ele[1] *
				 angle);
  invMass_SC_regrCorr_pho = sqrt(2* energySCEle_regrCorr_pho[0] * energySCEle_regrCorr_pho[1] *
				 angle);

  invMass_SC_regrCorrSemiParV4_ele = sqrt(2* energySCEle_regrCorrSemiParV4_ele[0] * energySCEle_regrCorrSemiParV4_ele[1] *
					  angle);
  invMass_SC_regrCorrSemiParV4_pho = sqrt(2* energySCEle_regrCorrSemiParV4_pho[0] * energySCEle_regrCorrSemiParV4_pho[1] *
					  angle);
  invMass_SC_regrCorrSemiParV4_pho = sqrt(2* energySCEle_regrCorrSemiParV4_pho[0] * energySCEle_regrCorrSemiParV4_pho[1] *
					  angle);
  invMass_SC_regrCorrSemiParV5_ele = sqrt(2* energySCEle_regrCorrSemiParV5_ele[0] * energySCEle_regrCorrSemiParV5_ele[1] *
					  angle);
  invMass_SC_regrCorrSemiParV5_pho = sqrt(2* energySCEle_regrCorrSemiParV5_pho[0] * energySCEle_regrCorrSemiParV5_pho[1] *
					  angle);
  invMass_SC_regrCorrSemiParV6_ele = sqrt(2* energySCEle_regrCorrSemiParV6_ele[0] * energySCEle_regrCorrSemiParV6_ele[1] *
					  angle);
  invMass_SC_regrCorrSemiParV6_pho = sqrt(2* energySCEle_regrCorrSemiParV6_pho[0] * energySCEle_regrCorrSemiParV6_pho[1] *
					  angle);
  invMass_SC_regrCorrSemiParV7_ele = sqrt(2* energySCEle_regrCorrSemiParV7_ele[0] * energySCEle_regrCorrSemiParV7_ele[1] *
					  angle);
  invMass_SC_regrCorrSemiParV7_pho = sqrt(2* energySCEle_regrCorrSemiParV7_pho[0] * energySCEle_regrCorrSemiParV7_pho[1] *
					  angle);
  invMass_SC_regrCorrSemiParV8_ele = sqrt(2* energySCEle_regrCorrSemiParV8_ele[0] * energySCEle_regrCorrSemiParV8_ele[1] *
					  angle);
  invMass_SC_regrCorrSemiParV8_pho = sqrt(2* energySCEle_regrCorrSemiParV8_pho[0] * energySCEle_regrCorrSemiParV8_pho[1] *
					  angle);

  invMass_SC_regrCorrSemiPar7TeVtrainV6_ele = sqrt(2* energySCEle_regrCorrSemiPar7TeVtrainV6_ele[0] * energySCEle_regrCorrSemiPar7TeVtrainV6_ele[1] *
						   angle);
  invMass_SC_regrCorrSemiPar7TeVtrainV6_pho = sqrt(2* energySCEle_regrCorrSemiPar7TeVtrainV6_pho[0] * energySCEle_regrCorrSemiPar7TeVtrainV6_pho[1] *
						   angle);
  invMass_SC_regrCorrSemiPar7TeVtrainV7_ele = sqrt(2* energySCEle_regrCorrSemiPar7TeVtrainV7_ele[0] * energySCEle_regrCorrSemiPar7TeVtrainV7_ele[1] *
						   angle);
  invMass_SC_regrCorrSemiPar7TeVtrainV7_pho = sqrt(2* energySCEle_regrCorrSemiPar7TeVtrainV7_pho[0] * energySCEle_regrCorrSemiPar7TeVtrainV7_pho[1] *
						   angle);
  invMass_SC_regrCorrSemiPar7TeVtrainV8_ele = sqrt(2* energySCEle_regrCorrSemiPar7TeVtrainV8_ele[0] * energySCEle_regrCorrSemiPar7TeVtrainV8_ele[1] *
						   angle);
  invMass_SC_regrCorrSemiPar7TeVtrainV8_pho = sqrt(2* energySCEle_regrCorrSemiPar7TeVtrainV8_pho[0] * energySCEle_regrCorrSemiPar7TeVtrainV8_pho[1] *
						   angle);

  invMass_regrCorr_fra = sqrt(2* energyEle_regrCorr_fra[0] * energyEle_regrCorr_fra[1] *angle);
  invMass_regrCorr_egamma = sqrt(2* energyEle_regrCorr_egamma[0] * energyEle_regrCorr_egamma[1] *angle);

  invMass_MC = -100; // temporary set it to be -100 for SC

  return;
}


void ZNtupleDumper:: TreeSetMuMuGammaVar(const pat::Photon& photon, const pat::Muon& muon1, const pat::Muon& muon2){

  TreeSetSinglePhotonVar(photon, 0);
  TreeSetSingleMuonVar(muon1, 1);
  TreeSetSingleMuonVar(muon2, 2); 

  TLorentzVector *Z = new TLorentzVector();
  TLorentzVector *ph = new TLorentzVector();
  TLorentzVector *m1 = new TLorentzVector();
  TLorentzVector *m2 = new TLorentzVector();

  ph->SetPtEtaPhiE(PtEle[0], etaEle[0], phiEle[0], photon.energy());
  m1->SetPtEtaPhiE(PtEle[1], etaEle[1], phiEle[1], muon1.energy());
  m2->SetPtEtaPhiE(PtEle[2], etaEle[2], phiEle[2], muon2.energy());

  *Z = *ph + *m1 + *m2;
  invMass = Z->M();

  Z->SetE(photon.e5x5()+muon1.energy()+muon2.energy());
  invMass_e5x5 = Z->M();

  Z->SetE(energySCEle[0]+muon1.energy()+muon2.energy());
  invMass_SC = Z->M();

  Z->SetE(rawEnergySCEle[0]+muon1.energy()+muon2.energy());
  invMass_rawSC = Z->M();

  Z->SetE(rawEnergySCEle[0]+esEnergySCEle[0]+muon1.energy()+muon2.energy());
  invMass_rawSC_esSC = Z->M();

  Z->SetE(energySCEle_corr[0]+muon1.energy()+muon2.energy());
  invMass_SC_corr = Z->M();

  if(photon.genPhoton()!=0 && muon1.genLepton()!=0 && muon2.genLepton()!=0){
    Z->SetE(photon.genPhoton()->energy()+muon1.genLepton()->energy()+muon2.genLepton()->energy());
    invMass_MC = Z->M();
  } else invMass_MC = 0;

  double t1=TMath::Exp(-etaEle[1]);
  double t2=TMath::Exp(-etaEle[2]);
  double t1q = t1*t1;
  double t2q = t2*t2;
  
  double angle=1-
    ( (1-t1q)*(1-t2q)+4*t1*t2*cos(phiEle[1]-phiEle[2]))/(
							 (1+t1q)*(1+t2q)
							 );
  
  invMass_mumu     = sqrt(2*muon1.energy() *muon2.energy() *
			  angle);

  delete Z;
  delete ph;
  delete m1;
  delete m2;
 
  return;
}

//////////////


//#============================== extra calib tree
void ZNtupleDumper::InitExtraCalibTree(){
  
  //  tree = new TTree("selected",fChain->GetTitle());
  std::cout << "[STATUS] InitExtraCalibTree" << std::endl;
  if(extraCalibTree==NULL) return;
  
  extraCalibTree->Branch("runNumber",     &runNumber,     "runNumber/I");
  extraCalibTree->Branch("eventNumber",   &eventNumber, "eventNumber/l");
  extraCalibTree->Branch("lumiBlock",     &lumiBlock,     "lumiBlock/I");
  extraCalibTree->Branch("runTime",       &runTime, "        runTime/i");
 
  extraCalibTree->Branch("nHitsSCEle", nHitsSCEle, "nHitsSCEle[3]/I");

  extraCalibTree->Branch("recoFlagRecHitSCEle1", &(recoFlagRecHitSCEle[0]));
  extraCalibTree->Branch("recoFlagRecHitSCEle2", &(recoFlagRecHitSCEle[1]));
  extraCalibTree->Branch("rawIdRecHitSCEle1", &(rawIdRecHitSCEle[0]));
  extraCalibTree->Branch("rawIdRecHitSCEle2", &(rawIdRecHitSCEle[1]));
  extraCalibTree->Branch("XRecHitSCEle1", &(XRecHitSCEle[0]));
  extraCalibTree->Branch("XRecHitSCEle2", &(XRecHitSCEle[1]));
  extraCalibTree->Branch("YRecHitSCEle1", &(YRecHitSCEle[0]));
  extraCalibTree->Branch("YRecHitSCEle2", &(YRecHitSCEle[1]));
  extraCalibTree->Branch("ZRecHitSCEle1", &(ZRecHitSCEle[0]));
  extraCalibTree->Branch("ZRecHitSCEle2", &(ZRecHitSCEle[1]));
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

  TreeSetExtraCalibVar(electron1, 0);
  TreeSetExtraCalibVar(electron1, 1);
  return;
}

void ZNtupleDumper::TreeSetExtraCalibVar(const pat::Electron& electron1, const reco::SuperCluster& electron2){

  TreeSetExtraCalibVar(electron1, 0);
  TreeSetExtraCalibVar(electron2, 1);
  return;
}

void ZNtupleDumper::TreeSetExtraCalibVar(const pat::Electron& electron1, int index){

  if(index<0){
    recoFlagRecHitSCEle[-index].clear();
    rawIdRecHitSCEle[-index].clear();
    XRecHitSCEle[-index].clear();
    YRecHitSCEle[-index].clear();
    ZRecHitSCEle[-index].clear();
    energyRecHitSCEle[-index].clear();
    LCRecHitSCEle[-index].clear();
    ICRecHitSCEle[-index].clear();
    AlphaRecHitSCEle[-index].clear();
    return;
  }

  recoFlagRecHitSCEle[index].clear();
  rawIdRecHitSCEle[index].clear();
  XRecHitSCEle[index].clear();
  YRecHitSCEle[index].clear();
  ZRecHitSCEle[index].clear();
  energyRecHitSCEle[index].clear();
  LCRecHitSCEle[index].clear();
  ICRecHitSCEle[index].clear();
  AlphaRecHitSCEle[index].clear();

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
	edm::LogError("ZNtupleDumper") << "No intercalib const found for xtal "  << (detitr->first).rawId() 
				       << " in subdetector " << (detitr->first).subdetId() << " bailing out";
	//assert(0);
        continue;
      }
      recoFlagRecHitSCEle[index].push_back(oneHit->recoFlag());
      rawIdRecHitSCEle[index].push_back(detitr->first.rawId());
      if(electron1.isEB()){
        EBDetId recHitId(detitr->first);
	XRecHitSCEle[index].push_back(recHitId.ieta());
	YRecHitSCEle[index].push_back(recHitId.iphi());
	ZRecHitSCEle[index].push_back(recHitId.zside());
      }
      else{
        EEDetId recHitId(detitr->first);
	XRecHitSCEle[index].push_back(recHitId.ix());
	YRecHitSCEle[index].push_back(recHitId.iy());
	ZRecHitSCEle[index].push_back(recHitId.zside());
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
	//assert(0);
        continue;
      }
      // 3) get adc2GeV
      //float adcToGeV = ( (detitr -> first).subdetId() == EcalBarrel ) ? 
      // float(adcToGeVHandle->getEBValue()) : float(adcToGeVHandle->getEEValue());
      ICRecHitSCEle[index].push_back(icalconst);
    }

  return;
}

void ZNtupleDumper::TreeSetExtraCalibVar(const reco::SuperCluster& electron1, int index){

  if(index<0){
    recoFlagRecHitSCEle[-index].clear();
    rawIdRecHitSCEle[-index].clear();
    XRecHitSCEle[-index].clear();
    YRecHitSCEle[-index].clear();
    ZRecHitSCEle[-index].clear();
    energyRecHitSCEle[-index].clear();
    LCRecHitSCEle[-index].clear();
    ICRecHitSCEle[-index].clear();
    AlphaRecHitSCEle[-index].clear();
    return;
  }

  recoFlagRecHitSCEle[index].clear();
  rawIdRecHitSCEle[index].clear();
  XRecHitSCEle[index].clear();
  YRecHitSCEle[index].clear();
  ZRecHitSCEle[index].clear();
  energyRecHitSCEle[index].clear();
  LCRecHitSCEle[index].clear();
  ICRecHitSCEle[index].clear();
  AlphaRecHitSCEle[index].clear();

  std::vector< std::pair<DetId, float> > hitsAndFractions_ele1 = electron1.hitsAndFractions();
  nHitsSCEle[index] = hitsAndFractions_ele1.size();

  const EcalRecHitCollection *recHitsEB = clustertools->getEcalEBRecHitCollection();
  const EcalRecHitCollection *recHitsEE = clustertools->getEcalEERecHitCollection();

  const EcalIntercalibConstantMap& icalMap = clustertools->getEcalIntercalibConstants();
  const edm::ESHandle<EcalLaserDbService>& laserHandle_ = clustertools->getLaserHandle();

  for (std::vector<std::pair<DetId, float> >::const_iterator detitr = hitsAndFractions_ele1.begin();
     detitr != hitsAndFractions_ele1.end(); detitr++ )
  {
    // get out the DetId of the hit
    DetId hitId = (detitr -> first);
    // define a iterator of the EcalRecoHit
    EcalRecHitCollection::const_iterator oneHit(NULL);

    // treat it seperately for EB and EE
    if ( hitId.subdetId() == EcalBarrel)
    {
      oneHit = recHitsEB->find( hitId );
      // protection of the missing hit
      if(oneHit==recHitsEB->end()){
        edm::LogError("ZNtupleDumper") << "No intercalib const found for xtal "  << hitId.rawId() << "bailing out";
        assert(0);
      }
      // redifine EBDetId and get EB hit position
      EBDetId recHitId(hitId);
      XRecHitSCEle[index].push_back(recHitId.ieta());
      YRecHitSCEle[index].push_back(recHitId.iphi());
      ZRecHitSCEle[index].push_back(recHitId.zside());
    }
    else if ( hitId.subdetId() == EcalEndcap )
    {
      oneHit = recHitsEE->find( hitId );
      // protection of the missing hit
      if(oneHit==recHitsEE->end()){
        edm::LogError("ZNtupleDumper") << "No intercalib const found for xtal "  << hitId.rawId() << "bailing out";
        assert(0);
      }
      // redifine EEDetId and get EE hit position
      EEDetId recHitId(hitId);
      XRecHitSCEle[index].push_back(recHitId.ix());
      YRecHitSCEle[index].push_back(recHitId.iy());
      ZRecHitSCEle[index].push_back(recHitId.zside());
    }
    else
    {
        // error if not able to find the hit in EE and EB
	edm::LogError("ZNtupleDumper") << "SC hit cannot be found in EB and EE. " ;
	assert(0);
    }
   
    // other information
    recoFlagRecHitSCEle[index].push_back(oneHit->recoFlag());
    rawIdRecHitSCEle[index].push_back(hitId.rawId());

    energyRecHitSCEle[index].push_back(oneHit->energy());

    // in order to get back the ADC counts from the recHit energy, three ingredients are necessary:
    // 1) get laser correction coefficient
    LCRecHitSCEle[index].push_back(laserHandle_->getLaserCorrection(hitId, runTime_));
    // 2) get intercalibration
    EcalIntercalibConstantMap::const_iterator icalit = icalMap.find(hitId);
    EcalIntercalibConstant icalconst = 1.;
    if( icalit!=icalMap.end() ) {
      icalconst = (*icalit);
      // std::cout << "icalconst set to: " << icalconst << std::endl;
    } else {
      edm::LogError("ZNtupleDumper") << "No intercalib const found for xtal "  << (hitId).rawId() << "bailing out";
      assert(0);
    }
    // 3) get adc2GeV
    //float adcToGeV = ( (detitr -> first).subdetId() == EcalBarrel ); 
    // float(adcToGeVHandle->getEBValue()) : float(adcToGeVHandle->getEEValue());
    ICRecHitSCEle[index].push_back(icalconst);
 
  }
  
  return;
}


void ZNtupleDumper::TreeSetExtraCalibVar(const pat::Photon& photon, const pat::Muon& muon1, const pat::Muon& muon2){

  TreeSetExtraCalibVar(photon, 0);
  TreeSetExtraCalibVar(muon1, -1);
  TreeSetExtraCalibVar(muon2, -2);
  return;
}


void ZNtupleDumper::TreeSetExtraCalibVar(const pat::Photon& photon, int index){

  if(index<0){
    recoFlagRecHitSCEle[-index].clear();
    rawIdRecHitSCEle[-index].clear();
    XRecHitSCEle[-index].clear();
    YRecHitSCEle[-index].clear();
    ZRecHitSCEle[-index].clear();
    energyRecHitSCEle[-index].clear();
    LCRecHitSCEle[-index].clear();
    ICRecHitSCEle[-index].clear();
    AlphaRecHitSCEle[-index].clear();
    return;
  }

  recoFlagRecHitSCEle[index].clear();
  rawIdRecHitSCEle[index].clear();
  XRecHitSCEle[index].clear();
  YRecHitSCEle[index].clear();
  ZRecHitSCEle[index].clear();
  energyRecHitSCEle[index].clear();
  LCRecHitSCEle[index].clear();
  ICRecHitSCEle[index].clear();
  AlphaRecHitSCEle[index].clear();

  //  EcalIntercalibConstantMap icMap = icHandle->get()
  std::vector< std::pair<DetId, float> > hitsAndFractions_ele1 = photon.superCluster()->hitsAndFractions();
  nHitsSCEle[index] = hitsAndFractions_ele1.size();
  
  const EcalRecHitCollection *recHits = (photon.isEB()) ?  clustertools->getEcalEBRecHitCollection() : clustertools->getEcalEERecHitCollection();
  const EcalIntercalibConstantMap& icalMap = clustertools->getEcalIntercalibConstants();
  const edm::ESHandle<EcalLaserDbService>& laserHandle_ = clustertools->getLaserHandle();
  for (std::vector<std::pair<DetId, float> >::const_iterator detitr = hitsAndFractions_ele1.begin(); 
       detitr != hitsAndFractions_ele1.end(); detitr++ )
    {
      //      EcalRecHitCollection::const_iterator theSeedHit = recHits->find (id); // trash this
      EcalRecHitCollection::const_iterator oneHit = recHits->find( (detitr -> first) ) ;
      if(oneHit==recHits->end()){
	edm::LogError("ZNtupleDumper") << "No intercalib const found for xtal "  << (detitr->first).rawId() 
				       << " in subdetector " << (detitr->first).subdetId() << " bailing out";
	//assert(0);
        continue;
      }
      recoFlagRecHitSCEle[index].push_back(oneHit->recoFlag());
      rawIdRecHitSCEle[index].push_back(detitr->first.rawId());
      if(photon.isEB()){
        EBDetId recHitId(detitr->first);
	XRecHitSCEle[index].push_back(recHitId.ieta());
	YRecHitSCEle[index].push_back(recHitId.iphi());
	ZRecHitSCEle[index].push_back(recHitId.zside());
      }
      else{
        EEDetId recHitId(detitr->first);
	XRecHitSCEle[index].push_back(recHitId.ix());
	YRecHitSCEle[index].push_back(recHitId.iy());
	ZRecHitSCEle[index].push_back(recHitId.zside());
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
	//assert(0);
        continue;
      }
      // 3) get adc2GeV
      //float adcToGeV = ( (detitr -> first).subdetId() == EcalBarrel ) ? 
      // float(adcToGeVHandle->getEBValue()) : float(adcToGeVHandle->getEEValue());
      ICRecHitSCEle[index].push_back(icalconst);
    }

  return;
}

void ZNtupleDumper::TreeSetExtraCalibVar(const pat::Muon& muon1, int index){

  if(index<0){
    recoFlagRecHitSCEle[-index].clear();
    rawIdRecHitSCEle[-index].clear();
    XRecHitSCEle[-index].clear();
    YRecHitSCEle[-index].clear();
    ZRecHitSCEle[-index].clear();
    energyRecHitSCEle[-index].clear();
    LCRecHitSCEle[-index].clear();
    ICRecHitSCEle[-index].clear();
    AlphaRecHitSCEle[-index].clear();
    return;
  }

  recoFlagRecHitSCEle[index].clear();
  rawIdRecHitSCEle[index].clear();
  XRecHitSCEle[index].clear();
  YRecHitSCEle[index].clear();
  ZRecHitSCEle[index].clear();
  energyRecHitSCEle[index].clear();
  LCRecHitSCEle[index].clear();
  ICRecHitSCEle[index].clear();
  AlphaRecHitSCEle[index].clear();

  return;
}

//#============================== Ele ID tree
void ZNtupleDumper::InitEleIDTree(){
  
  //  tree = new TTree("selected",fChain->GetTitle());
  std::cout << "[STATUS] InitEleIDTree" << std::endl;
  if(eleIDTree==NULL) {return;}
  
  eleIDTree->Branch("runNumber",     &runNumber,     "runNumber/I");
  eleIDTree->Branch("eventNumber",   &eventNumber, "eventNumber/l");
  eleIDTree->Branch("lumiBlock",     &lumiBlock,     "lumiBlock/I");
  eleIDTree->Branch("runTime",       &runTime,         "runTime/i");
 
  eleIDTree->Branch("dr03TkSumPt", dr03TkSumPt, "dr03TkSumPt[3]/F");
  eleIDTree->Branch("dr03EcalRecHitSumEt", dr03EcalRecHitSumEt, "dr03EcalRecHitSumEt[3]/F");
  eleIDTree->Branch("dr03HcalTowerSumEt", dr03HcalTowerSumEt, "dr03HcalTowerSumEt[3]/F");
  eleIDTree->Branch("sigmaIEtaIEtaSCEle", sigmaIEtaIEtaSCEle, "sigmaIEtaIEtaSCEle[3]/F");
  //  eleIDTree->Branch("sigmaIPhiIPhiSCEle", sigmaIPhiIPhiSCEle, "sigmaIPhiIPhiSCEle[3]/F");
  eleIDTree->Branch("deltaEtaSuperClusterTrackAtVtx", deltaEtaSuperClusterTrackAtVtx, "deltaEtaSuperClusterTrackAtVtx[3]/F");
  eleIDTree->Branch("deltaPhiSuperClusterTrackAtVtx", deltaPhiSuperClusterTrackAtVtx, "deltaPhiSuperClusterTrackAtVtx[3]/F");
  eleIDTree->Branch("hOverE", hOverE, "hOverE[3]/F");
  eleIDTree->Branch("pfMVA", pfMVA, "pfMVA[3]/F");
  eleIDTree->Branch("hasMatchedConversion", hasMatchedConversion, "hasMatchedConversion[3]/b");
  eleIDTree->Branch("maxNumberOfExpectedMissingHits", maxNumberOfExpectedMissingHits, "maxNumberOfExpectedMissingHits[3]/I");
  return;
}


void ZNtupleDumper::TreeSetEleIDVar(const pat::Electron& electron1, const pat::Electron& electron2){

  TreeSetEleIDVar(electron1, 0);
  TreeSetEleIDVar(electron2, 1);
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

#ifdef CMSSW_7_2_X
  pfMVA[index]   = electron1.mva_e_pi();
  hasMatchedConversion[index] = ConversionTools::hasMatchedConversion(electron1, conversionsHandle, bsHandle->position());
  maxNumberOfExpectedMissingHits[index] = electron1.gsfTrack()->hitPattern().numberOfLostHits(reco::HitPattern::MISSING_INNER_HITS);
#else
  pfMVA[index]   = electron1.mva();
  hasMatchedConversion[index] = ConversionTools::hasMatchedConversion(electron1, conversionsHandle, bsHandle->position());
  maxNumberOfExpectedMissingHits[index] = electron1.gsfTrack()->trackerExpectedHitsInner().numberOfLostHits();
#endif


  //   if (primaryVertexHandle->size() > 0) {
  //     reco::VertexRef vtx(primaryVertexHandle, 0);    
  //     d0vtx[index] = electron1.gsfTrack()->dxy(vtx->position());
  //     dzvtx[index] = electron1.gsfTrack()->dz(vtx->position());
  //   } else {
  //     d0vtx[index] = electron1.gsfTrack()->dxy();
  //     dzvtx[index] = electron1.gsfTrack()->dz();
  //   }
  
  eleIDloose[index]  = electron1.electronID("loose");
  eleIDmedium[index] = electron1.electronID("medium");
  eleIDtight[index]  = electron1.electronID("tight");
  return;
}

void ZNtupleDumper::TreeSetEleIDVar(const pat::Photon& photon, const pat::Muon& muon1, const pat::Muon& muon2){

  TreeSetEleIDVar(photon, 0);
  TreeSetEleIDVar(muon1, -1);
  TreeSetEleIDVar(muon2, -2);
  return;
}

void ZNtupleDumper::TreeSetEleIDVar(const pat::Photon& photon, int index){
  if(index<0){
    return;
  }
  
  sigmaIEtaIEtaSCEle[index]  = photon.sigmaIetaIeta(); // alcarereco
  hOverE[index] = photon.hadronicOverEm();

  const reco::SuperCluster photonSC = *(photon.superCluster());
#ifdef CMSSW_7_2_X
  hasMatchedConversion[index] = ConversionTools::hasMatchedConversion(photonSC, conversionsHandle, bsHandle->position());
#else
  hasMatchedConversion[index] = ConversionTools::hasMatchedConversion(photonSC, conversionsHandle, bsHandle->position());
#endif
  
  eleIDloose[index]  = photon.photonID("loose");
  eleIDmedium[index] = photon.photonID("medium");
  eleIDtight[index]  = photon.photonID("tight");
  return;
}


void ZNtupleDumper::TreeSetEleIDVar(const pat::Muon& muon1, int index){
  if(index<0){
    return;
  }
  
  //  eleIDloose[index]  = muon1.muonID("loose");
  //  eleIDmedium[index] = muon1.muonID("medium");
  //  eleIDtight[index]  = muon1.muonID("tight");
  return;
}


//#============================== Ele ID tree
void ZNtupleDumper::InitPdfSystTree(void){
  
  //  tree = new TTree("selected",fChain->GetTitle());
  std::cout << "[STATUS] InitPdfSystTree" << std::endl;
  if(pdfSystTree==NULL){
    std::cerr << "[ERROR] pdfSystTree==NULL" << std::endl;
    exit(1);
    return;
  }
  //   pdfSystTree->Branch("runNumber",     &runNumber,     "runNumber/I");
  //   pdfSystTree->Branch("eventNumber",   &eventNumber, "eventNumber/l");
  //   pdfSystTree->Branch("lumiBlock",     &lumiBlock,     "lumiBlock/I");
  //   pdfSystTree->Branch("runTime",       &runTime,         "runTime/i");
  

  for(std::vector< edm::InputTag >::const_iterator pdfWeightTAGS_itr = pdfWeightTAGS.begin();
      pdfWeightTAGS_itr != pdfWeightTAGS.end();
      pdfWeightTAGS_itr++){
    int i = pdfWeightTAGS_itr - pdfWeightTAGS.begin();
    std::string tagName = pdfWeightTAGS_itr->instance();
    //tagName.replace(0,pdfWeightTAGS_itr->label().size());
    //std::cout << i << "\t" << tagName << "\t" << pdfWeightTAGS_itr->label() << "\t" << pdfWeightTAGS_itr->encode() << std::endl;
    //pdfSystTree->Branch(pdfWeightTAGS_itr->encode().c_str(), &(pdfSystWeightNum[i]), "pdfSystWeightNum/I");
    pdfSystTree->Branch((pdfWeightTAGS_itr->label()+"_"+pdfWeightTAGS_itr->instance()).c_str(), &(pdfSystWeight[i]));
  }

  pdfSystTree->Branch("fsrWeight", &fsrWeight, "fsrWeight/F");
  pdfSystTree->Branch("weakWeight", &weakWeight, "weakWeight/F");
  return;
}

void ZNtupleDumper::TreeSetPdfSystVar(const edm::Event& iEvent){

  for(std::vector< edm::InputTag >::const_iterator pdfWeightTAGS_itr = pdfWeightTAGS.begin();
      pdfWeightTAGS_itr != pdfWeightTAGS.end();
      pdfWeightTAGS_itr++){
    int i = pdfWeightTAGS_itr-pdfWeightTAGS.begin();
    iEvent.getByLabel(*pdfWeightTAGS_itr, pdfWeightHandle);
    
    //pdfSystWeight[i] = 
    std::vector<Double_t> weights = std::vector<Double_t>(*pdfWeightHandle);
    pdfSystWeight[i]=weights;
    //std::cout << "Event weight for central PDF " << pdfWeightTAGS_itr->encode() << ": " << pdfSystWeight[i][0] << std::endl;
    //unsigned int nmembers = weights.size();
    //pdfSystWeightNum[i]=pdfSystWeight.size();
    //    for (unsigned int j=1; j<nmembers; j+=2) {
    //      std::cout << "Event weight for PDF variation +" << (j+1)/2 << ": " << pdfSystWeight[i][j] << std::endl;
    //      std::cout << "Event weight for PDF variation -" << (j+1)/2 << ": " << pdfSystWeight[i][j+1] << std::endl;
    //    }
  }  
  
  iEvent.getByLabel(fsrWeightTAG, fsrWeightHandle);
  iEvent.getByLabel(weakWeightTAG, weakWeightHandle);

  fsrWeight = (Float_t) *fsrWeightHandle;
  weakWeight = (Float_t) *weakWeightHandle;

  return ;
}

//define this as a plug-in
DEFINE_FWK_MODULE(ZNtupleDumper);
