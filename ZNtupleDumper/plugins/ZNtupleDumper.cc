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
#include <map>

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

#include "FWCore/Utilities/interface/EDGetToken.h"

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
#include "DataFormats/EcalDetId/interface/ESDetId.h"
//#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
//#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
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
#include "DataFormats/METReco/interface/CaloMET.h"
#include "DataFormats/METReco/interface/CaloMETFwd.h"
#include "DataFormats/METReco/interface/CaloMETCollection.h"
// HLT trigger
#include "FWCore/Framework/interface/TriggerNamesService.h"
#include <FWCore/Common/interface/TriggerNames.h>
#include <DataFormats/Common/interface/TriggerResults.h>

// alcaSkimPaths
#include "DataFormats/Provenance/interface/ParameterSetID.h"

#include "Calibration/ZNtupleDumper/interface/eleIDMap.h"
#include "DataFormats/Common/interface/ValueMap.h"

//#define DEBUG

////////////////////////////////////////////////
// class declaration
//

// number of electrons in each branch (max nEle)
#define NELE 2

class ZNtupleDumper : public edm::EDAnalyzer
{
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
        edm::Handle< reco::CaloMETCollection > caloMetHandle;
	edm::Handle<edm::TriggerResults> triggerResultsHandle;
	edm::Handle<edm::TriggerResults> WZSkimResultsHandle;
	edm::Handle<EcalRecHitCollection> ESRechitsHandle;
	edm::Handle<EcalUncalibratedRecHitCollection> pEBUncRecHits;
	edm::Handle<EcalUncalibratedRecHitCollection> pEEUncRecHits;

	//------------------------------ Input Tags
	// input tag for primary vertex
	edm::EDGetTokenT<GenEventInfoProduct> generatorInfoToken_;
	edm::EDGetTokenT<reco::VertexCollection> vtxCollectionToken_;
	//edm::InputTag vtxCollectionTAG;
	edm::EDGetTokenT<reco::BeamSpot>         beamSpotToken_;
	//edm::InputTag BeamSpotTAG;
	edm::EDGetTokenT<pat::ElectronCollection> electronsToken_;
	edm::EDGetTokenT<pat::MuonCollection>     muonsToken_;
	edm::EDGetTokenT<pat::PhotonCollection>     photonsToken_;

	edm::EDGetTokenT<EcalRecHitCollection> recHitCollectionEBToken_;
	edm::EDGetTokenT<EcalRecHitCollection> recHitCollectionEEToken_;
	edm::EDGetTokenT<EcalRecHitCollection> recHitCollectionESToken_;
	edm::EDGetTokenT<EcalUncalibratedRecHitCollection> ebURHToken_;
	edm::EDGetTokenT<EcalUncalibratedRecHitCollection> eeURHToken_;

	edm::EDGetTokenT<std::vector<reco::SuperCluster> > EESuperClustersToken_;

	// input rho
	edm::EDGetTokenT<double> rhoToken_;
	edm::EDGetTokenT<std::vector<PileupSummaryInfo> > pileupInfoToken_;
	edm::EDGetTokenT<reco::ConversionCollection> conversionsProducerToken_;
	edm::EDGetTokenT<reco::PFMETCollection> metToken_;
        edm::EDGetTokenT<reco::CaloMETCollection> caloMetToken_;
	edm::EDGetTokenT<edm::TriggerResults> triggerResultsToken_;
	edm::EDGetTokenT<edm::TriggerResults> WZSkimResultsToken_;
	edm::InputTag triggerResultsTAG, WZSkimResultsTAG;
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
	UInt_t eleID[3];        ///< bit mask for eleID: 1=fiducial, 2=loose, 6=medium, 14=tight, 16=WP90PU, 48=WP80PU, 112=WP70PU, 128=loose25nsRun2, 384=medium25nsRun2, 896=tight25nsRun2, 1024=loose50nsRun2, 3072=medium50nsRun2, 7168=tight50nsRun2. Selection from https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaCutBasedIdentification#Electron_ID_Working_Points

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
	Float_t energyEle[3];      ///< electron.energy()
	Float_t energySCEle[3];    ///< corrected SuperCluster energy with PF. NB: in the rereco case, this is mustache too!
	Float_t energySCEle_must[3];    ///< corrected SuperCluster energy with mustache
	Float_t rawEnergySCEle[3]; ///< SC energy without cluster corrections
	Float_t rawEnergySCEle_must[3]; ///< SC mustach energy without cluster corrections

	Float_t energySCEle_must_regrCorr_ele[3]; ///< mustache SC energy derived with regression (offline tool)
	Float_t energySigmaSCEle_must_regrCorr_ele[3]; ///< mustache SC energy resolution derived with regression (offline tool)

	Float_t energySCEle_pho_regrCorr[3]; ///< mustache SC energy derived with regression (offline tool)
	Float_t energySigmaSCEle_pho_regrCorr[3]; ///< mustache SC energy resolution derived with regression (offline tool)

	Float_t esEnergySCEle[3];  ///< pre-shower energy associated to the electron
	Float_t esEnergyPlane1SCEle[3]; ///< energy associate to the electron in the first plane of ES
	Float_t esEnergyPlane2SCEle[3]; ///< energy associate to the electron in the second plane of ES
	Float_t rawESEnergyPlane1SCEle[3];  ///< pre-shower rechit energy sum of Plane 1 associated to the electron
	Float_t rawESEnergyPlane2SCEle[3];  ///< pre-shower recHit energy sum of Plane 2 associated to the electron

	Float_t energySCEle_corr[3];  ///< ecal energy with corrections base on type of electron (see #classificationEle)

	Float_t e3x3SCEle[3];   //< sum of the recHit energy in 3x3 matrix centered at the seed of the SC
	Float_t e5x5SCEle[3];   ///< sum of the recHit energy in 5x5 matrix centered at the seed of the SC
	Float_t efull5x5SCEle[3];   ///< full 5x5
	Float_t eSeedSCEle[3];
	Float_t pModeGsfEle[3];  ///< track momentum from Gsf Track (mode)
	Float_t pAtVtxGsfEle[3]; ///< momentum estimated at the vertex
	Float_t trackMomentumErrorEle[3]; ///< track momentum error from standard electron method
	Float_t pNormalizedChi2Ele[3];  ///< track normalized chi2 of the fit (GSF)

	Float_t R9Ele[3];      ///< e3x3/rawEnergySCEle
	Float_t invMass;
	Float_t invMass_SC;   ///< invariant mass using SC energy with PF. NB: in the rereco case, this is mustache too!
	Float_t invMass_SC_must;   ///< invariant mass using SC energy with mustache
	Float_t invMass_SC_must_regrCorr_ele;   ///< invariant mass using SC energy with mustache corrected with regression
	Float_t invMass_SC_pho_regrCorr;   ///< invariant mass using SC energy from associated photon

	//   Float_t invMass_e3x3;
	Float_t invMass_e5x5;
	Float_t invMass_efull5x5;
	Float_t invMass_rawSC;
	Float_t invMass_rawSC_must;
	Float_t invMass_rawSC_esSC;
	Float_t invMass_rawSC_esSC_must;
	Float_t invMass_SC_corr;

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
	std::vector<unsigned int>  rawIdRecHitSCEle[3];
	std::vector<int>        XRecHitSCEle[3];
	std::vector<int>        YRecHitSCEle[3];
	std::vector<int>        ZRecHitSCEle[3];
	std::vector<float> energyRecHitSCEle[3];
	std::vector<float>     LCRecHitSCEle[3];
	std::vector<float>     ICRecHitSCEle[3];
	std::vector<float>  AlphaRecHitSCEle[3];
	std::vector<float*> ootAmplisUncalibRecHitSCEle[3];
	std::vector<float> ampliUncalibRecHitSCEle[3];
	std::vector<float> ampliErrUncalibRecHitSCEle[3];
	std::vector<float> pedEUncalibRecHitSCEle[3];
	std::vector<float> jitterUncalibRecHitSCEle[3];
	std::vector<float> jitterErrUncalibRecHitSCEle[3];
	std::vector<float> chi2UncalibRecHitSCEle[3];
	std::vector<uint32_t> flagsUncalibRecHitSCEle[3];
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
	void resetExtraVariables(int index);
	void TreeSetExtraCalibVar(const pat::Electron& electron1, int index);
	void TreeSetExtraCalibVar(const reco::SuperCluster& electron1, int index);
	void TreeSetExtraCalibVar(const pat::Photon& photon, int index);
	void TreeSetExtraCalibVar(const pat::Muon& muon1, int index);
	void TreeSetExtraCalibVar(const pat::Electron& electron1, const pat::Electron& electron2);
	void TreeSetExtraCalibVar(const pat::Electron& electron1, const reco::SuperCluster& electron2);
	void TreeSetExtraCalibVar(const pat::Photon& photon, const pat::Muon& muon1, const pat::Muon& muon2);
	void TreeSetExtraCalibVar(const std::vector<std::pair<DetId, float> > & hitsFracs, int index, bool isEB);


	void InitEleIDTree(void);

	void TreeSetEleIDVar(const pat::Electron& electron1, int index);
	void TreeSetEleIDVar(const pat::Electron& electron1, const pat::Electron& electron2);
	void TreeSetEleIDVar(const pat::Photon& photon, const pat::Muon& muon1, const pat::Muon& muon2);
	void TreeSetEleIDVar(const pat::Photon& photon, int index);
	void TreeSetEleIDVar(const pat::Muon& muon1, int index);

	//  void Tree_output(TString filename);
	void TreeSetEventSummaryVar(const edm::Event& iEvent);
	void TreeSetPileupVar(void);
	float GetESPlaneRawEnergy(const reco::SuperClusterRef& sc, unsigned int planeIndex);

	bool elePreselection(const pat::Electron& electron);
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
	bool preselID_;
	std::string eleID_loose, eleID_medium, eleID_tight;

	typedef enum {
		ZEE = 0,
		WENU,
		ZSC,
		ZMMG,
		PARTGUN,
		WSTREAM,
		ZSTREAM,
		UNKNOWN,
		SINGLEELE, //no skim, no preselection and no selection are applied
		DEBUG_T
	} eventType_t;


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
	generatorInfoToken_(consumes<GenEventInfoProduct>(edm::InputTag("generator"))),
	vtxCollectionToken_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexCollection"))),
	beamSpotToken_(consumes<reco::BeamSpot>(iConfig.getParameter<edm::InputTag>("BeamSpotCollection"))),
	electronsToken_(consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electronCollection"))),
	muonsToken_(consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muonCollection"))),
	photonsToken_(consumes<pat::PhotonCollection>(iConfig.getParameter<edm::InputTag>("photonCollection"))),
	recHitCollectionEBToken_(consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>( "recHitCollectionEB" ))),
	recHitCollectionEEToken_(consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>( "recHitCollectionEE" ))),
	recHitCollectionESToken_(consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("recHitCollectionES"))),
	ebURHToken_(consumes<EcalUncalibratedRecHitCollection>(iConfig.getParameter<edm::InputTag>( "uncalibRecHitCollectionEB" ))),
	eeURHToken_(consumes<EcalUncalibratedRecHitCollection>(iConfig.getParameter<edm::InputTag>( "uncalibRecHitCollectionEE" ))),
	EESuperClustersToken_(consumes<reco::SuperClusterCollection>(iConfig.getParameter< edm::InputTag>("EESuperClusterCollection"))),
	rhoToken_(consumes<double>(iConfig.getParameter<edm::InputTag>("rhoFastJet"))),
	pileupInfoToken_(consumes<std::vector<PileupSummaryInfo>>(iConfig.getParameter<edm::InputTag>("pileupInfo"))),
	conversionsProducerToken_(consumes<reco::ConversionCollection>(iConfig.getParameter<edm::InputTag>("conversionCollection"))),
	metToken_(consumes<reco::PFMETCollection>(iConfig.getParameter<edm::InputTag>("metCollection"))),
        caloMetToken_(consumes<reco::CaloMETCollection>(iConfig.getParameter<edm::InputTag>("caloMetCollection"))), //for the stream
	triggerResultsToken_(consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("triggerResultsCollection"))),
	WZSkimResultsToken_(consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("WZSkimResultsCollection"))),
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
	preselID_(iConfig.getParameter<bool>("useIDforPresel")),
	// used for preselection and event type determination
	eleID_loose(iConfig.getParameter< std::string>("eleID_loose")),
	eleID_medium(iConfig.getParameter< std::string>("eleID_medium")),
	eleID_tight(iConfig.getParameter< std::string>("eleID_tight")),
	eventType(ZEE)
{

	if(pdfWeightTAGS.size() > 0) doPdfSystTree = true;




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
	eventType = isPartGun ? PARTGUN : UNKNOWN;

	chargeEle[0] = -100;
	chargeEle[1] = -100;
	chargeEle[2] = -100;
	invMass_mumu = 0;


	energySCEle[0] = -99;
	energySCEle[1] = -99;
	energyEle[0] = -99;
	energyEle[1] = -99;
	energySCEle_must[0] = -99;
	energySCEle_must[1] = -99;
	invMass_SC = -99;
	invMass_SC_must = -99;

	pEvent = &iEvent;
	pSetup = &iSetup;



	// filling infos runNumber, eventNumber, lumi
	if( !iEvent.isRealData() ) {
		iEvent.getByToken(pileupInfoToken_, PupInfo);
		iEvent.getByToken(generatorInfoToken_, GenEventInfoHandle);
		isMC = true;
	} else isMC = false;


	//------------------------------ HLT
	/// \todo check why
	if(triggerResultsTAG.label() != "") iEvent.getByToken(triggerResultsToken_, triggerResultsHandle);
	if(WZSkimResultsTAG.label() != "") {
		iEvent.getByToken(WZSkimResultsToken_,  WZSkimResultsHandle); //else it is not produced with ALCARECO selection
		//then the type of event has to be defined

		//Check if it is Wenu, Z or ZSC event according to triggerResults
		edm::TriggerNames alcaSkimPathNames = iEvent.triggerNames(*WZSkimResultsHandle);

		if(!SelectEvents.empty()) {
			// If the alca skim paths are not changing, this is done only once
			if(alcaSkimPathID != alcaSkimPathNames.parameterSetID()) { //order of trigger results is changed
				alcaSkimPathID = alcaSkimPathNames.parameterSetID();    //update the map of trigger index
				alcaSkimPathIndexes.clear();
				unsigned int alcaSkimPathNameSize = alcaSkimPathNames.size(); // should have the same size of WZSkimResultsHandle

				for(unsigned int i = 0; i < alcaSkimPathNameSize; i++) { // look over alcaSkimPaths
					std::string trgName = alcaSkimPathNames.triggerName(i);

					for(std::vector<std::string>::const_iterator selectEvents_itr = SelectEvents.begin();
					        selectEvents_itr != SelectEvents.end();
					        selectEvents_itr++) {
						if(std::regex_match(trgName, std::regex(*selectEvents_itr))) {
							alcaSkimPathIndexes.insert(i);
							std::cout << " - Trigger path saved in ntuples: " << trgName << "\t" << i << std::endl;
							break;
						}

					}
					//if(alcaSkimPathIndexes.count(i)==0){
					//std::cout << " -! Trigger path not saved in ntuples: " << trgName <<  "\t" << i << std::endl;
					//}
				}
			}

			eventType = DEBUG_T;
			bool skipEvent = true;
			for(std::set<unsigned int>::const_iterator alcaSkimPath_itr = alcaSkimPathIndexes.begin();
			        alcaSkimPath_itr != alcaSkimPathIndexes.end() && skipEvent == true;
			        alcaSkimPath_itr++) {
				//std::cout << *alcaSkimPath_itr << std::endl;
				if(WZSkimResultsHandle->accept(*alcaSkimPath_itr)) {
					skipEvent = false;
					std::string hltName_str(alcaSkimPathNames.triggerName(*alcaSkimPath_itr));
					if(hltName_str.find("WElectronStream")!=std::string::npos)
					        eventType=WSTREAM;
					else if(hltName_str.find("ZElectronStream")!=std::string::npos)
					        eventType=ZSTREAM;
					else if(hltName_str.find("WElectron") != std::string::npos)
						eventType = WENU;
					else if(hltName_str.find("ZSCElectron") != std::string::npos)
						eventType = ZSC;
					else if(hltName_str.find("ZElectron") != std::string::npos)
						eventType = ZEE;
					else if(hltName_str.find("SingleElectron") != std::string::npos)
						eventType = SINGLEELE;
					else if(hltName_str.find("Zmmg") != std::string::npos)
						eventType = ZMMG;
					else
						eventType = UNKNOWN;
					// this paths are exclusive, then we can skip the check of the others
					//
					//	std::cout << alcaSkimPathNames.triggerName(*alcaSkimPath_itr) << "\t" << eventType << std::endl;
					break;
				}
			}
			//if(alcaSkimPathIndexes.size()==0){
//			skipEvent = false;
//			eventType = UNKNOWN;
			//}
			//std::cout << "skip event: " << skipEvent << "\t" << eventType << std::endl;
			//assert(!skipEvent);
			eventTypeCounter[eventType]++;
			if(skipEvent) return; // event not coming from any skim or paths
		}
	}

	//------------------------------ CONVERSIONS
	iEvent.getByToken(conversionsProducerToken_, conversionsHandle);

	//------------------------------
	clustertools = new EcalClusterLazyTools (iEvent, iSetup, recHitCollectionEBToken_,
	        recHitCollectionEEToken_);
	iEvent.getByToken(ebURHToken_, pEBUncRecHits);
	iEvent.getByToken(eeURHToken_, pEEUncRecHits);


	//------------------------------ electrons
	if (eventType == ZMMG) {
		//------------------------------ muons
		iEvent.getByToken(muonsToken_, muonsHandle);
		//------------------------------ photons
		iEvent.getByToken(photonsToken_, photonsHandle);
	}	else {
		iEvent.getByToken(electronsToken_, electronsHandle);
	}

	//------------------------------ SuperClusters (for high Eta studies)
	iEvent.getByToken(EESuperClustersToken_, EESuperClustersHandle);

	// for conversions with full vertex fit
	//------------------------------  VERTEX
	iEvent.getByToken(vtxCollectionToken_, primaryVertexHandle);
	iEvent.getByToken(beamSpotToken_, bsHandle);
	iEvent.getByToken(rhoToken_, rhoHandle);

	iEvent.getByToken(recHitCollectionESToken_, ESRechitsHandle);

	iEvent.getByToken(metToken_, metHandle);
	iEvent.getByToken(caloMetToken_, caloMetHandle); 

	reco::MET met =  ((*metHandle))[0];
	if (eventType == WSTREAM) {
	    met = (*caloMetHandle)[0];
	  }

	//Here the HLTBits are filled. TriggerResults
	TreeSetEventSummaryVar(iEvent);
	TreeSetPileupVar(); // this can be filled once per event

	if(doPdfSystTree && isMC) {
		TreeSetPdfSystVar(iEvent);
	}

	// at least one of the triggers
	HLTfire = false;
	if(!hltPaths.empty()) {
		for(std::vector<std::string>::const_iterator hltPath_itr = hltPaths.begin();
		        hltPath_itr != hltPaths.end();
		        hltPath_itr++) {
			if(hltPath_itr->empty()) continue;
			std::map<std::string, bool>::const_iterator it = HLTBits.find(*hltPath_itr);
			if(it != HLTBits.end()) {
				HLTfire += it->second;
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
#ifdef DEBUG
	std::cout << "After trigger selection" << std::endl;
#endif

	// count electrons: needed to avoid double counting events in Wenu and Zee samples
	// in Wenu is required ONLY ONE tight electron
	// in Zee at least two loose electrons
	// in particle gun case, the matching with the gen particle is required

	int nTight   = 0; //number of electrons passing only the tight  ID for preselection
	int nMedium  = 0; //number of electrons passing only the medium ID for preselection
	int nLoose   = 0; //number of electrons passing only the loose  ID for preselection
	int nEle     = 0; //number of electrons saved in the electron stream

	//if (eventType!=ZMMG) { // count the number of electrons passing the different IDs for preselection and event type determination
	if (eventType != UNKNOWN) { // count the number of electrons passing the different IDs for preselection and event type determination
		for( pat::ElectronCollection::const_iterator eleIter1 = electronsHandle->begin();
		        eleIter1 != electronsHandle->end();
		        eleIter1++) {
			if( eleIter1->electronID(eleID_tight) )          ++nTight;
			else if( eleIter1->electronID(eleID_medium) ) ++nMedium;
			else if( eleIter1->electronID(eleID_loose) )  ++nLoose;
			nEle++;
		}
	}

	bool doFill = false;

	if(eventType == PARTGUN) {
		pat::ElectronCollection::const_iterator eleIter1 = electronsHandle->begin();
		pat::ElectronCollection::const_iterator eleIter2 = eleIter1;
		for(eleIter1 = electronsHandle->begin();
		        eleIter1 != electronsHandle->end() && eleIter1->genLepton() == 0;
		        eleIter1++) {
		}

		//if no electron matching the gen particles then the event is skipped
		//if(eleIter1 == electronsHandle->end()) return;
		if(eleIter1 == electronsHandle->end()) eleIter1 = electronsHandle->begin();

		//in order to not put duplicate electrons, remove the ones not matching the genparticle
		for(eleIter2 = eleIter1, eleIter2++;
		        eleIter2 != electronsHandle->end() && eleIter2->genLepton() == 0;
		        eleIter2++) {
		}
		if(eleIter2 == electronsHandle->end()) {
			if(eleIter1->genLepton() != 0 || electronsHandle->size() < NELE) eleIter2 = eleIter1;
			else eleIter2 = eleIter1 + 1;
		}

		//if one electron matching the gen particles then eleIter2 = eleIter1
		//else we have two electrons
		TreeSetDiElectronVar(*eleIter1, *eleIter2);
		doFill = true;
		if(doExtraCalibTree) {
			TreeSetExtraCalibVar(*eleIter1, *eleIter2);
		}
		if(doEleIDTree) {
			TreeSetEleIDVar(*eleIter1, *eleIter2);
		}
        } else if(eventType==ZEE || eventType==WENU || eventType==UNKNOWN || eventType==WSTREAM || eventType==ZSTREAM){
#ifdef DEBUG
		std::cout << "[DEBUG] Electrons in the event: " << electronsHandle->size() << std::endl;
#endif

		for( pat::ElectronCollection::const_iterator eleIter1 = electronsHandle->begin();
		        eleIter1 != electronsHandle->end();
		        eleIter1++) {

			if(! elePreselection(*eleIter1)) continue;

			if(eventType == WENU || eventType == WSTREAM) {
				if(! (eleIter1->electronID(eleID_tight)) ) continue;
				if( nTight != 1 || nLoose > 0 ) continue; //to be a Wenu event request only 1 ele WP70 in the event

				// MET/MT selection
				if(  met.et() < 25. ) continue;
				if( sqrt( 2.*eleIter1->et()*met.et() * (1 - cos(eleIter1->phi() - met.phi()))) < 50. ) continue;
				
				if( eleIter1->et() < 30) continue;

				doFill = true;
				if(eventType == UNKNOWN) eventType = WENU; //after selection it's clear that it's WENU
				TreeSetSingleElectronVar(*eleIter1, 0);  //fill first electron
				TreeSetSingleElectronVar(*eleIter1, -1); // fill fake second electron

				if(doExtraCalibTree) {
					TreeSetExtraCalibVar(*eleIter1, 0);
					TreeSetExtraCalibVar(*eleIter1, -1);
				}
				if(doEleIDTree) {
					TreeSetEleIDVar(*eleIter1, 0);
					TreeSetEleIDVar(*eleIter1, -1);
				}
			} else { //ZEE or UNKNOWN
				// take only the fist di-electron pair (highest pt)
				for(pat::ElectronCollection::const_iterator eleIter2 = eleIter1 + 1;
				        eleIter2 != electronsHandle->end() && doFill == false;
				        eleIter2++) {

					if(! elePreselection(*eleIter1)) continue;
#ifdef DEBUG
					std::cout << "[DEBUG] Electron passing preselection" << std::endl;
#endif
					//	  float mass=(eleIter1->p4()+eleIter2->p4()).mass();

					//calculate the invariant mass
					double t1 = TMath::Exp(-eleIter1->eta());
					double t1q = t1 * t1;
					double t2 = TMath::Exp(-eleIter2->eta());
					double t2q = t2 * t2;

					if(!eleIter2->parentSuperCluster().isNonnull()) continue;
					double angle = 1 -
					               ( (1 - t1q) * (1 - t2q) + 4 * t1 * t2 * cos(eleIter1->phi() - eleIter2->phi())) / (
					                   (1 + t1q) * (1 + t2q)
					               );
					double mass = sqrt(2 * eleIter1->energy() * eleIter2->energy() * angle); //use default electron energy, in order to have the same number of events between alcareco and alcarereco ntuples

					//	  std::cout<<" ele1 SC: "<<eleIter1->superCluster()->energy()<<" ele1 SC must: "<<eleIter1->parentSuperCluster()->energy()<<" eta1: "<<eleIter1->eta()<<" phi1: "<<eleIter1->phi()<<std::endl
					//		   <<" ele2 SC: "<<eleIter2->superCluster()->energy()<<" ele2 SC must: "<<eleIter2->parentSuperCluster()->energy()<<" eta2: "<<eleIter2->eta()<<" phi2: "<<eleIter2->phi()<<"mass: "<<mass<<std::endl;

					if(mass < 55 ) continue;
					doFill = true;

					if(eventType == UNKNOWN) eventType = ZEE;
					TreeSetDiElectronVar(*eleIter1, *eleIter2);

					if(doExtraCalibTree) {
						TreeSetExtraCalibVar(*eleIter1, *eleIter2);
					}
					if(doEleIDTree) {
						TreeSetEleIDVar(*eleIter1, *eleIter2);
					}
					if(doPdfSystTree && isMC) {
						TreeSetPdfSystVar(iEvent);
						//pdfSystTree->Fill();
					}

					// if(electronsHandle->size() < NELE &&  eventType == SINGLEELE){

					// 	doFill=true;
					// 	TreeSetSingleElectronVar(*eleIter1, 0);  //fill first electron
					// 	TreeSetSingleElectronVar(*eleIter1, -1); // fill fake second electron

					// 	if(doExtraCalibTree){
					// 		TreeSetExtraCalibVar(*eleIter1, 0);
					// 		TreeSetExtraCalibVar(*eleIter1, -1);
					// 	}
					// 	if(doEleIDTree){
					// 		TreeSetEleIDVar(*eleIter1, 0);
					// 		TreeSetEleIDVar(*eleIter1, -1);
					// 	}
					// }
				}
			}
		}

	}	  else if (eventType == ZMMG) {
		for( pat::MuonCollection::const_iterator muIter1 = muonsHandle->begin();
		        muIter1 != muonsHandle->end() && doFill == false;
		        muIter1++) {

			for(pat::MuonCollection::const_iterator muIter2 = muIter1 + 1;
			        muIter2 != muonsHandle->end() && doFill == false;
			        muIter2++) {

				// should exit when muIter1 == end-1
				//if(! muIter2->electronID("loose") ) continue;
				for( pat::PhotonCollection::const_iterator phoIter1 = photonsHandle->begin();
				        phoIter1 != photonsHandle->end() && doFill == false;
				        phoIter1++) {

					float mass = (muIter1->p4() + muIter2->p4() + phoIter1->p4()).mass();

					if (phoIter1->pt() < 10) continue;
					if((mass < 55 || mass > 125)) continue;
					doFill = true;

					TreeSetMuMuGammaVar(*phoIter1, *muIter1, *muIter2);

					if(doExtraCalibTree) {
						TreeSetExtraCalibVar(*phoIter1, *muIter1, *muIter2);
					}
					if(doEleIDTree) {
						TreeSetEleIDVar(*phoIter1, *muIter1, *muIter2);
					}
					if(doPdfSystTree && isMC) {
						TreeSetPdfSystVar(iEvent);
						//pdfSystTree->Fill();
					}

				}

			}
		}
	}

	if (eventType == ZSC) { // || eventType==UNKNOWN){ removed for miniAOD, to be put back in

		//leading pt electron in EB (patElectrons should be pt-ordered)
		// iterators storing pat Electons and HighEta SCs
		pat::ElectronCollection::const_iterator PatEle1 = electronsHandle->begin();
		// iterators storing HighEta SCs
		// select the highest pt SC in the highEta region
		reco::SuperClusterCollection::const_iterator HighEtaSC1 = EESuperClustersHandle->end();

		for( PatEle1 = electronsHandle->begin();
		        //stop when HighEtaSC1 is a valid SC (this means that there is a pair matching the Z mass
		        PatEle1 != electronsHandle->end();
		        PatEle1++) {

			// consider electrons passing at least the loose identification
			//if(!PatEle1->electronID("loose50nsRun2") ) continue;
			//nMedium=0;
			// take the highest pt tight electrons if it exists (the collection is ordered in pt)
			// consider only the electrons passing the tightest electron identification
			//if(nWP70>0){ // if there are tight electrons, consider only those
			//if(!PatEle1->electronID("tight50nsRun2") ) continue;
			//}else if(nMedium>0){ // if there are only medium electrons, consider only those
			//if(!PatEle1->electronID("medium50nsRun2") ) continue;
			//}

			//      if(!PatEle1->ecalDriven()){ //to make alcareco/alcarereco ntuples coeherent
			//        continue;
			//      }

			// you have the first electrons candidate satifying the electrons criteria
			// now look for a SC matching the Z invariant mass. If not SC is found, let's look to another electrons candidate

			double HighEtaSCPt = 0;
			double t1 = TMath::Exp(-PatEle1->eta());
			double t1q = t1 * t1;
			for( reco::SuperClusterCollection::const_iterator iter = EESuperClustersHandle->begin();
			        iter != EESuperClustersHandle->end();
			        iter++) {
				// only SCs in the high eta region
				if (fabs(iter->eta()) < doHighEta_LowerEtaCut) continue;

				//calculate the invariant mass
				double t2 = TMath::Exp(-iter->eta());
				double t2q = t2 * t2;
				double angle = 1 -
				               ( (1 - t1q) * (1 - t2q) + 4 * t1 * t2 * cos(PatEle1->phi() - iter->phi())) / (
				                   (1 + t1q) * (1 + t2q)
				               );
				float mass = sqrt(2 * PatEle1->energy() * iter->energy() * angle);
				if((mass < 55 || mass > 125)) continue;

				//take the highest pt SC matching the Z mass
				double pt = iter->energy() / cosh(iter->eta());
				if(HighEtaSCPt < pt) {
					HighEtaSCPt = pt;
					HighEtaSC1 = iter;
				}
			}

			if(HighEtaSC1 != EESuperClustersHandle->end()) break;
		}

		// if you have found an ele-SC pair matching the high eta criteria,
		// save the event in the tree
		if(HighEtaSC1 != EESuperClustersHandle->end()) {
			doFill = true;
			TreeSetDiElectronVar(*PatEle1, *HighEtaSC1);
			if(doExtraCalibTree) {
				TreeSetExtraCalibVar(*PatEle1, *HighEtaSC1);
			}
		}
	}

	if(doFill) {
		tree->Fill();
		if(doExtraCalibTree)  extraCalibTree->Fill();
		if(doEleIDTree)      eleIDTree->Fill();
	}
	delete clustertools;
	return;

}


bool ZNtupleDumper::elePreselection(const pat::Electron& electron)
{
	if(electron.et() < 10) return false; // minimum Et cut in preselection

	//to make alcareco/alcarereco ntuples coeherent
	if(!electron.ecalDriven()) return false;

	//if(eleIter1->parentSuperCluster().isNull()) continue;
	if(preselID_) {
		if(! (electron.electronID(eleID_loose))) return false;
	}
	return true;
}


// ------------ method called once each job just before starting event loop  ------------
void ZNtupleDumper::beginJob()
{
#ifdef DEBUG
	std::cout << "[DEBUG] Starting creation of ntuples" << std::endl;
#endif

	tree_file = new TFile(foutName.c_str(), "recreate");
	if(tree_file->IsZombie()) {
		throw cms::Exception("OutputError") <<  "Output tree not created (Zombie): " << foutName;
		return;
	}
	tree_file->cd();

	//now do what ever initialization is needed
	tree = new TTree("selected", "selected");
	//tree = fs->make<TTree>("selected","selected"); //no otherwise you have the extraCalib in the same file

	tree->SetDirectory(tree_file);
	// controllo su tree==NULL

	InitNewTree();  // inizializzo il tree dell'ntupla ridotta selezionata

	if(doExtraCalibTree) {
		//extraCalibTree = fs->make<TTree>("extraCalibTree","");
		// put the extraCalibTree into the default outfile
		extraCalibTreeFile = new TFile("extraCalibTree.root", "recreate");
		if(extraCalibTreeFile->IsZombie()) {
			throw cms::Exception("OutputError") <<  "Output tree for extra calib not created (Zombie): " << foutName;
			return;
		}
		extraCalibTreeFile->cd();

		extraCalibTree = new TTree("extraCalibTree", "extraCalibTree");
		extraCalibTree->SetDirectory(extraCalibTreeFile);
		InitExtraCalibTree();
	}

	if(doEleIDTree) {
		eleIDTreeFile = new TFile("eleIDTree.root", "recreate");
		if(eleIDTreeFile->IsZombie()) {
			throw cms::Exception("OutputError") <<  "Output tree for extra calib not created (Zombie): " << foutName;
			return;
		}
		eleIDTreeFile->cd();
		eleIDTree = new TTree("eleIDTree", "eleIDTree");
		eleIDTree->SetDirectory(eleIDTreeFile);
		//eleIDTree = fs->make<TTree>("eleIDTree","");
		InitEleIDTree();
	}

	if(doPdfSystTree) {
		std::cout << "[INFO] Doing pdfSystTree" << std::endl;
		//pdfSystTree = fs->make<TTree>("pdfSystTree","pdfSystTree");
		pdfSystTree = tree;
		InitPdfSystTree();
	}

	for(int i = ZEE; i <= DEBUG_T; i++) {
		eventTypeCounter[i] = 0;
	}

#ifdef DEBUG
	std::cout << "[DEBUG] End creation of ntuples" << std::endl;
#endif
}

// ------------ method called once each job just after ending the event loop  ------------
void ZNtupleDumper::endJob()
{

	if(tree->GetEntries() > 0) {
		tree->BuildIndex("runNumber", "eventNumber");
		if(doEleIDTree)       eleIDTree->BuildIndex("runNumber", "eventNumber");
		if(doExtraCalibTree) extraCalibTree->BuildIndex("runNumber", "eventNumber");
	}
	// save the tree into the file
	tree_file->cd();
	tree->Write();
	tree_file->Close();

	if(doExtraCalibTree) {
		extraCalibTreeFile->cd();
		extraCalibTree->Write();
		extraCalibTreeFile->Close();
	}
	if(doEleIDTree) {
		eleIDTreeFile->cd();
		eleIDTree->Write();
		eleIDTreeFile->Close();
	}


	for(int i = ZEE; i <= DEBUG_T; i++) {
		std::cout << "[NTUPLEDUMPER] EventTypeCounter[" << i << "]\t" << eventTypeCounter[i] << std::endl;
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
void ZNtupleDumper::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
	//The following says we do not know what parameters are allowed so do no validation
	// Please change this to state exactly what you do use, even if it is no parameters
	edm::ParameterSetDescription desc;
	desc.setUnknown();
	descriptions.addDefault(desc);
}



// output tree initialization
void ZNtupleDumper::InitNewTree()
{

	//  tree = new TTree("selected",fChain->GetTitle());
	std::cout << "[STATUS] InitNewTree" << std::endl;
	if(tree == NULL) return;
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

	tree->Branch("eleID", eleID, "eleID[3]/i");
	//  tree->Branch("nBCSCEle", nBCSCEle, "nBCSCEle[3]/I");

	tree->Branch("chargeEle",   chargeEle,    "chargeEle[3]/I");	//[nEle]
	tree->Branch("etaSCEle",      etaSCEle,       "etaSCEle[3]/F");	//[nSCEle]
	tree->Branch("phiSCEle",      phiSCEle,       "phiSCEle[3]/F");	//[nSCEle]

	tree->Branch("etaEle",      etaEle,       "etaEle[3]/F");	//[nEle]
	tree->Branch("phiEle",      phiEle,       "phiEle[3]/F");	//[nEle]

	tree->Branch("classificationEle",      classificationEle,       "classificationEle[3]/I");	//[nEle]
	tree->Branch("recoFlagsEle", recoFlagsEle, "recoFlagsEle[3]/I");	//[nEle]
	tree->Branch("PtEle",       PtEle,        "PtEle[3]/F");
	tree->Branch("fbremEle",    fbremEle,     "fbremEle[3]/F");

	tree->Branch("seedXSCEle",           seedXSCEle,      "seedXSCEle[3]/F");
	tree->Branch("seedYSCEle",           seedYSCEle,      "seedYSCEle[3]/F");
	tree->Branch("seedEnergySCEle", seedEnergySCEle, "seedEnergySCEle[3]/F");
	tree->Branch("seedLCSCEle",         seedLCSCEle,     "seedLCSCEle[3]/F");

	tree->Branch("avgLCSCEle", avgLCSCEle, "avgLCSCEle[3]/F");

	tree->Branch("gainEle", gainEle, "gainEle[3]/b");

	tree->Branch("energyMCEle", energyMCEle, "energyMCEle[3]/F");
	tree->Branch("energyEle", energyEle, "energyEle[3]/F");
	tree->Branch("energySCEle", energySCEle, "energySCEle[3]/F");
	tree->Branch("energySCEle_must", energySCEle_must, "energySCEle_must[3]/F");
	tree->Branch("rawEnergySCEle", rawEnergySCEle, "rawEnergySCEle[3]/F");
	tree->Branch("rawEnergySCEle_must", rawEnergySCEle_must, "rawEnergySCEle_must[3]/F");

	tree->Branch("energySCEle_must_regrCorr_ele", energySCEle_must_regrCorr_ele, "energySCEle_must_regrCorr_ele[3]/F");
	tree->Branch("energySigmaSCEle_must_regrCorr_ele", energySigmaSCEle_must_regrCorr_ele, "energySigmaSCEle_must_regrCorr_ele[3]/F");

	tree->Branch("energySCEle_pho_regrCorr", energySCEle_pho_regrCorr, "energySCEle_pho_regrCorr[3]/F");
	tree->Branch("energySigmaSCEle_pho_regrCorr", energySigmaSCEle_pho_regrCorr, "energySigmaSCEle_pho_regrCorr[3]/F");

	tree->Branch("esEnergySCEle", esEnergySCEle, "esEnergySCEle[3]/F");
	tree->Branch("esEnergyPlane2SCEle", esEnergyPlane2SCEle, "esEnergyPlane2SCEle[3]/F");
	tree->Branch("esEnergyPlane1SCEle", esEnergyPlane1SCEle, "esEnergyPlane1SCEle[3]/F");
	tree->Branch("rawESEnergyPlane2SCEle", rawESEnergyPlane2SCEle, "rawESEnergyPlane2SCEle[3]/F");
	tree->Branch("rawESEnergyPlane1SCEle", rawESEnergyPlane1SCEle, "rawESEnergyPlane1SCEle[3]/F");


	tree->Branch("energySCEle_corr", energySCEle_corr, "energySCEle_corr[3]/F");


	tree->Branch("R9Ele", R9Ele, "R9Ele[3]/F");

	tree->Branch("e5x5SCEle", e5x5SCEle, "e5x5SCEle[3]/F");
	tree->Branch("efull5x5SCEle", efull5x5SCEle, "efull5x5SCEle[3]/F");
	//tree->Branch("eSeedSCEle", eSeedSCEle, "eSeedSCEle[3]/F");
	tree->Branch("pModeGsfEle", pModeGsfEle, "pModeGsfEle[3]/F");
	tree->Branch("pAtVtxGsfEle", pAtVtxGsfEle, "pAtVtxGsfEle[3]/F");
	tree->Branch("pNormalizedChi2Ele", pNormalizedChi2Ele, "pNormalizedChi2Ele[3]/F");
	tree->Branch("trackMomentumErrorEle", trackMomentumErrorEle, "trackMomentumErrorEle[3]/F");

	tree->Branch("invMass",    &invMass,      "invMass/F");
	tree->Branch("invMass_SC", &invMass_SC,   "invMass_SC/F");
	tree->Branch("invMass_SC_must", &invMass_SC_must,   "invMass_SC_must/F");
	tree->Branch("invMass_SC_must_regrCorr_ele", &invMass_SC_must_regrCorr_ele,   "invMass_SC_must_regrCorr_ele/F");
	tree->Branch("invMass_SC_pho_regrCorr", &invMass_SC_pho_regrCorr,   "invMass_SC_pho_regrCorr/F");
	//   tree->Branch("invMass_e3x3",    &invMass_e3x3,      "invMass_e3x3/F");
	tree->Branch("invMass_e5x5",    &invMass_e5x5,      "invMass_e5x5/F");
	tree->Branch("invMass_efull5x5",    &invMass_efull5x5,      "invMass_efull5x5/F");
	tree->Branch("invMass_rawSC", &invMass_rawSC,   "invMass_rawSC/F");
	tree->Branch("invMass_rawSC_must", &invMass_rawSC_must,   "invMass_rawSC_must/F");
	tree->Branch("invMass_rawSC_esSC", &invMass_rawSC_esSC,   "invMass_rawSC_esSC/F");

	tree->Branch("invMass_SC_corr", &invMass_SC_corr, "invMass_SC_corr/F");

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



void ZNtupleDumper::TreeSetEventSummaryVar(const edm::Event& iEvent)
{

	runTime_   = iEvent.eventAuxiliary().time();
	runTime = runTime_.unixTime();
	runNumber = iEvent.id().run();
	eventNumber = iEvent.id().event();
	if( iEvent.isRealData() ) {
		lumiBlock = iEvent.luminosityBlock();
	} else {
		lumiBlock = -1;
	}

	if(!hltPaths.empty()) {
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


void ZNtupleDumper::TreeSetPileupVar(void)
{
	rho = *rhoHandle;
	nPV = 0;
	nPU[0] = -1;
	mcGenWeight = -1;

	if(primaryVertexHandle->size() > 0) {
		for(reco::VertexCollection::const_iterator v = primaryVertexHandle->begin(); v != primaryVertexHandle->end(); ++v) {
			//if((*v).tracksSize() > 0)
			nPV++; // non mi ricordo perche' ho fatto cosi'....
		}
	}

	if(isMC) {
		std::vector<PileupSummaryInfo>::const_iterator PVI;
		for(PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI) {
			int BX = PVI->getBunchCrossing();
			if(BX == 0) { // in-time pu
				//weight = puWeights.GetWeight( PVI->getPU_NumInteractions());
				//weight = puWeights.GetWeight( PVI->getTrueNumInteractions());
				//weight = 1.;
				//nPU[0]=PVI->getPU_NumInteractions();
				nPU[0] = PVI->getTrueNumInteractions();
			}
		}

		if(!GenEventInfoHandle->weights().empty())
			mcGenWeight = (GenEventInfoHandle->weights())[0];

	} // else {
	//     //weight= 1.;
	//     nPU[0]=-1;
	//     mcGenWeight=-1;
	//   }
	return;
}



DetId ZNtupleDumper::findSCseed(const reco::SuperCluster& cluster)
{
	DetId seedDetId;
	float seedEnergy = 0;
	std::vector< std::pair<DetId, float> > hitsFractions = cluster.hitsAndFractions();
	for(std::vector< std::pair<DetId, float> >::const_iterator hitsAndFractions_itr = hitsFractions.begin();
	        hitsAndFractions_itr != hitsFractions.end();
	        hitsAndFractions_itr++) {
		if(hitsAndFractions_itr->second > seedEnergy)
			seedDetId = hitsAndFractions_itr->first;
	}
#ifdef DEBUG
	std::cout << "[DEBUG findSCseed] seedDetIt: " << seedDetId.rawId() << std::endl
	          << cluster << std::endl
	          << *(cluster.seed()) << std::endl;
#endif
	if(seedDetId.null()) {
		std::cerr << "[ERROR] Invalid detID: " << cluster << std::endl;
	}
	return seedDetId;
}


// a negative index means that the corresponding electron does not exist, fill with 0
void ZNtupleDumper::TreeSetSingleElectronVar(const pat::Electron& electron1, int index)
{

	if(index < 0) {
		PtEle[-index] 	  = 0;
		chargeEle[-index] = -100;
		etaEle[-index]    = 0;
		phiEle[-index]    = 0;
		recoFlagsEle[-index] = -1;
		return;
	}

	energyEle[index] = electron1.energy();
	PtEle[index]     = electron1.et();
	chargeEle[index] = electron1.charge();
	etaEle[index]    = electron1.eta(); // degli elettroni
	phiEle[index]    = electron1.phi();


	if(electron1.ecalDrivenSeed()) {
		if(electron1.trackerDrivenSeed()) recoFlagsEle[index] = 3;
		else recoFlagsEle[index] = 2;
	} else recoFlagsEle[index] = 1;


	fbremEle[index] = electron1.fbrem(); // fatta con pIn-pOut

	const reco::SuperClusterRef& sc = electron1.superCluster().isNonnull() ? electron1.superCluster() : electron1.parentSuperCluster();
	assert(sc.isNonnull());

	if(sc.isNonnull()) {
		etaSCEle[index] = sc->eta();
		phiSCEle[index] = sc->phi();
	}

	if(electron1.superCluster().isNonnull()) {
		energySCEle[index]  = electron1.superCluster()->energy();
		rawEnergySCEle[index]  = electron1.superCluster()->rawEnergy();
		esEnergySCEle[index] = electron1.superCluster()->preshowerEnergy();
		esEnergyPlane1SCEle[index] = electron1.superCluster()-> preshowerEnergyPlane1();
		esEnergyPlane2SCEle[index] = electron1.superCluster()-> preshowerEnergyPlane2();
	} else {
		energySCEle[index]			= -99;
		rawEnergySCEle[index]			= -99;
		esEnergySCEle[index]			= -99;
		esEnergyPlane1SCEle[index]		= -99;
		esEnergyPlane2SCEle[index]		= -99;
	}

	if(electron1.parentSuperCluster().isAvailable()) {
		energySCEle_must[index] = electron1.parentSuperCluster()->energy();
		rawEnergySCEle_must[index]  = electron1.parentSuperCluster()->rawEnergy();
	} else  {
		energySCEle_must[index] = -99;
		rawEnergySCEle_must[index] = -99;
	}

	energySCEle_must_regrCorr_ele[index] = electron1.userFloat("energySCEleMust");
	energySigmaSCEle_must_regrCorr_ele[index] = electron1.userFloat("energySCEleMustVar");

	energySCEle_pho_regrCorr[index] = electron1.userFloat("energySCElePho");
	energySigmaSCEle_pho_regrCorr[index] = electron1.userFloat("energySCElePhoVar");


	rawESEnergyPlane1SCEle[index] = GetESPlaneRawEnergy(sc, 1);
	rawESEnergyPlane2SCEle[index] = GetESPlaneRawEnergy(sc, 2);

	energySCEle_corr[index] = electron1.correctedEcalEnergy();
	efull5x5SCEle[index] = electron1.full5x5_e5x5(); //check this

	if(sc.isNonnull()) {
		nHitsSCEle[index] = sc->size();

		// change in an electron properties please, EleNewEnergyProducer
		e3x3SCEle[index] = clustertools->e3x3(*sc->seed());
		e5x5SCEle[index] = clustertools->e5x5(*sc->seed());
		eSeedSCEle[index] = sc->seed()->energy();
	} else {
		e3x3SCEle[index]		= -99;
		e5x5SCEle[index]		= -99;
		eSeedSCEle[index]		= -99;
	}

	pModeGsfEle[index] = electron1.gsfTrack()->pMode();
	trackMomentumErrorEle[index] = electron1.trackMomentumError();
	pNormalizedChi2Ele[index] = electron1.gsfTrack()->normalizedChi2();
	pAtVtxGsfEle[index] = electron1.trackMomentumAtVtx().R();

	//R9Ele[index] = e3x3SCEle[index]/sc->rawEnergy();//already commented
	R9Ele[index] = electron1.full5x5_r9();

	//   if(isMC){
	//     if(electron1.isEB())
	//       R9Ele[index] = R9Ele[index]*1.0045+0.0010;
	//     else
	//       R9Ele[index] = R9Ele[index]*1.0086-0.0007;
	//   }

	eleIDMap eleID_map;

	eleID[index] = 0;
	for (std::map<std::string, UInt_t>::iterator it = eleID_map.eleIDmap.begin(); it != eleID_map.eleIDmap.end(); ++it) {
		if(electron1.isElectronIDAvailable(it->first)) { //
			if ((bool) electron1.electronID(it->first))  eleID[index] |= it->second;//
		}//
	}

	classificationEle[index] = electron1.classification();

//	return;//To be fixed


	if(electron1.ecalDrivenSeed() && sc.isNonnull()) {
		//assert(recHits!=NULL);
		const edm::ESHandle<EcalLaserDbService>& laserHandle_ = clustertools->getLaserHandle();

		seedXSCEle[index] = 0;
		seedYSCEle[index] = 0;
		nHitsSCEle[index] = -1;
		avgLCSCEle[index] = -1.;
		seedEnergySCEle[index] = -1.;
		seedLCSCEle[index] = -10;
		avgLCSCEle[index] = -10;
		gainEle[index] = -1;

		assert(sc->seed().isAvailable());
		DetId seedDetId = sc->seed()->seed();

		if(seedDetId.null()) {
			assert(electron1.trackerDrivenSeed()); // DEBUG
			//seedDetId = findSCseed(*(sc));
		}


		if(!seedDetId.null()) { //false for trackerDriverElectons
			if(electron1.isEB() && seedDetId.subdetId() == EcalBarrel) {
				EBDetId seedDetIdEcal = seedDetId;
				seedXSCEle[index] = seedDetIdEcal.ieta();
				seedYSCEle[index] = seedDetIdEcal.iphi();
			} else if(electron1.isEE() && seedDetId.subdetId() == EcalEndcap) {
				EEDetId seedDetIdEcal = seedDetId;
				seedXSCEle[index] = seedDetIdEcal.ix();
				seedYSCEle[index] = seedDetIdEcal.iy();
			}

			const EcalRecHitCollection *recHits = (seedDetId.subdetId() == EcalBarrel) ?  clustertools->getEcalEBRecHitCollection() : clustertools->getEcalEERecHitCollection();

			EcalRecHitCollection::const_iterator seedRecHit = recHits->find(seedDetId) ;
			//assert(seedRecHit!=recHits->end()); // DEBUG
			if(seedRecHit != recHits->end()) {
				seedEnergySCEle[index] = seedRecHit->energy();

				float sumLC_E = 0.;
				float sumE = 0.;

				if( !isMC) {
					seedLCSCEle[index] = laserHandle_->getLaserCorrection(seedDetId, runTime_);
					std::vector< std::pair<DetId, float> > hitsAndFractions_ele1 = sc->hitsAndFractions();
					for (std::vector<std::pair<DetId, float> >::const_iterator detitr = hitsAndFractions_ele1.begin();
					        detitr != hitsAndFractions_ele1.end(); detitr++ ) {
						EcalRecHitCollection::const_iterator oneHit = recHits->find( (detitr -> first) ) ;
						//assert(oneHit!=recHits->end()); // DEBUG
						sumLC_E += laserHandle_->getLaserCorrection(detitr->first, runTime_) * oneHit->energy();
						sumE    += oneHit->energy();
					}
					avgLCSCEle[index] = sumLC_E / sumE;
				}

				if(seedRecHit->checkFlag(EcalRecHit::kHasSwitchToGain6)) gainEle[index] = 1;
				else if(seedRecHit->checkFlag(EcalRecHit::kHasSwitchToGain1)) gainEle[index] = 2;
				else gainEle[index] = 0;
			}
		}
	}
	//  sigmaIEtaIEtaSCEle[index] = sqrt(clustertools->localCovariances(*(electron1.superCluster()->seed()))[index]);
	//  sigmaIEtaIEtaSCEle[1] = sqrt(clustertools->localCovariances(*(electron2.superCluster()->seed()))[index]);
	//  sigmaIPhiIPhiBC = sqrt(clustertools->localCovariances(*b)[3]);
	//  sigmaIEtaIPhiBC = clustertools->localCovariances(*b)[1];

	if(electron1.genLepton() != 0) {
		energyMCEle[index]  = electron1.genLepton()->energy();
		etaMCEle[index]  = electron1.genLepton()->eta();
		phiMCEle[index]  = electron1.genLepton()->phi();
	} else {
		energyMCEle[index] = 0;
		etaMCEle[index] = 0;
		phiMCEle[index] = 0;
	}

	return;
}


// a negative index means that the corresponding electron does not exist, fill with 0
void ZNtupleDumper::TreeSetSingleElectronVar(const reco::SuperCluster& electron1, int index)
{

	if(index < 0) {
		PtEle[-index] 	  = 0;
		chargeEle[-index] = -100;
		etaEle[-index]    = 0;
		phiEle[-index]    = 0;
		return;
	}

	//checks

	PtEle[index]     = electron1.energy() / cosh(electron1.eta());
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
	if(seedDetId.null()) {
		seedDetId = findSCseed(electron1);
	}
	assert(seedDetId.subdetId() == EcalEndcap);
	assert(!seedDetId.null());
	EEDetId seedDetIdEcal = seedDetId;
	seedXSCEle[index] = seedDetIdEcal.ix();
	seedYSCEle[index] = seedDetIdEcal.iy();

	EcalRecHitCollection::const_iterator seedRecHit = recHitsEE->find(seedDetId) ;
	assert(seedRecHit != recHitsEE->end());
	seedEnergySCEle[index] = seedRecHit->energy();
	if(seedRecHit->checkFlag(EcalRecHit::kHasSwitchToGain6)) gainEle[index] = 1;
	else if(seedRecHit->checkFlag(EcalRecHit::kHasSwitchToGain1)) gainEle[index] = 2;
	else gainEle[index] = 0;

	if(isMC) seedLCSCEle[index] = -10;
	else seedLCSCEle[index] = laserHandle_->getLaserCorrection(seedDetId, runTime_);

	float sumLC_E = 0.;
	float sumE = 0.;
	if( !isMC) {
		std::vector< std::pair<DetId, float> > hitsAndFractions_ele1 = electron1.hitsAndFractions();
		for (std::vector<std::pair<DetId, float> >::const_iterator detitr = hitsAndFractions_ele1.begin();
		        detitr != hitsAndFractions_ele1.end(); detitr++ ) {

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
	energyMCEle[index] = -100;
	etaMCEle[index] = -100;
	phiMCEle[index] = -100;

	energySCEle[index]  = electron1.energy();
	energySCEle_must[index]  = electron1.energy();
	rawEnergySCEle[index]  = electron1.rawEnergy();
	rawEnergySCEle_must[index]  = electron1.rawEnergy();
	esEnergySCEle[index] = electron1.preshowerEnergy();
	esEnergyPlane1SCEle[index] = electron1.preshowerEnergyPlane1();
	esEnergyPlane2SCEle[index] = electron1.preshowerEnergyPlane2();
	rawESEnergyPlane2SCEle[index] = -1;
	rawESEnergyPlane1SCEle[index] = -1;

	energySCEle_corr[index] = electron1.energy();


	// change in an electron properties please, EleNewEnergyProducer
	e3x3SCEle[index] = clustertools->e3x3(*electron1.seed());
	e5x5SCEle[index] = clustertools->e5x5(*electron1.seed());
	eSeedSCEle[index] = electron1.seed()->energy();

	pModeGsfEle[index] = -1; // no track, though ..
	trackMomentumErrorEle[index] = -1;
	pNormalizedChi2Ele[index] = -1;
	pAtVtxGsfEle[index] = -1;

	R9Ele[index] = e3x3SCEle[index] / electron1.rawEnergy(); //original version

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
void ZNtupleDumper::TreeSetSingleMuonVar(const pat::Muon& muon1, int index)
{

	if(index < 0) {
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


	if(muon1.genLepton() != 0) {
		energyMCEle[index]  = muon1.genLepton()->energy();
		etaMCEle[index]  = muon1.genLepton()->eta();
		phiMCEle[index]  = muon1.genLepton()->phi();
	} else {
		energyMCEle[index] = 0;
		etaMCEle[index] = 0;
		phiMCEle[index] = 0;
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
void ZNtupleDumper::TreeSetSinglePhotonVar(const pat::Photon& photon, int index)
{

	if(index < 0) {
		PtEle[-index] 	  = 0;
		chargeEle[-index] = -100;
		etaEle[-index]    = 0;
		phiEle[-index]    = 0;
		return;
	}

	PtEle[index]     = photon.energy() / cosh(photon.eta());
	chargeEle[index] = photon.charge();
	etaEle[index]    = photon.eta();
	phiEle[index]    = photon.phi();

	etaSCEle[index] = photon.superCluster()->eta();
	phiSCEle[index] = photon.superCluster()->phi();

	const EcalRecHitCollection *recHits = (photon.isEB()) ?  clustertools->getEcalEBRecHitCollection() : clustertools->getEcalEERecHitCollection();
	//assert(recHits!=NULL);
	const edm::ESHandle<EcalLaserDbService>& laserHandle_ = clustertools->getLaserHandle();

	DetId seedDetId = photon.superCluster()->seed()->seed();
	if(seedDetId.null()) {
		//assert(photon.trackerDrivenSeed()); // DEBUG
		seedDetId = findSCseed(*(photon.superCluster()));
	}

	if(photon.isEB() && seedDetId.subdetId() == EcalBarrel) {
		EBDetId seedDetIdEcal = seedDetId;
		seedXSCEle[index] = seedDetIdEcal.ieta();
		seedYSCEle[index] = seedDetIdEcal.iphi();
	} else if(photon.isEE() && seedDetId.subdetId() == EcalEndcap) {
		EEDetId seedDetIdEcal = seedDetId;
		seedXSCEle[index] = seedDetIdEcal.ix();
		seedYSCEle[index] = seedDetIdEcal.iy();
	} else { ///< this case is strange but happens for trackerDriven electrons
		seedXSCEle[index] = 0;
		seedYSCEle[index] = 0;
	}

	EcalRecHitCollection::const_iterator seedRecHit = recHits->find(seedDetId) ;
	//assert(seedRecHit!=recHits->end()); // DEBUG
	seedEnergySCEle[index] = seedRecHit->energy();
	if(isMC) seedLCSCEle[index] = -10;
	else seedLCSCEle[index] = laserHandle_->getLaserCorrection(seedDetId, runTime_);

	if(seedRecHit->checkFlag(EcalRecHit::kHasSwitchToGain6)) gainEle[index] = 1;
	else if(seedRecHit->checkFlag(EcalRecHit::kHasSwitchToGain1)) gainEle[index] = 2;
	else gainEle[index] = 0;

	float sumLC_E = 0.;
	float sumE = 0.;
	if( !isMC) {
		std::vector< std::pair<DetId, float> > hitsAndFractions_ele1 = photon.superCluster()->hitsAndFractions();
		for (std::vector<std::pair<DetId, float> >::const_iterator detitr = hitsAndFractions_ele1.begin();
		        detitr != hitsAndFractions_ele1.end(); detitr++ ) {
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

	if(photon.genParticle() != 0) {
		energyMCEle[index]  = photon.genParticle()->energy();
		etaMCEle[index]  = photon.genParticle()->eta();
		phiMCEle[index]  = photon.genParticle()->phi();
	} else {
		energyMCEle[index] = 0;
		etaMCEle[index] = 0;
		phiMCEle[index] = 0;
	}

	energySCEle[index]  = photon.superCluster()->energy();
	if(photon.parentSuperCluster().isNonnull()) {
		energySCEle_must[index] = photon.parentSuperCluster()->energy();
		rawEnergySCEle_must[index]  = photon.parentSuperCluster()->rawEnergy();
	} else {
		energySCEle_must[index] = -99.;
		rawEnergySCEle_must[index] = -99.;
	}

	rawEnergySCEle[index]  = photon.superCluster()->rawEnergy();
	esEnergySCEle[index] = photon.superCluster()->preshowerEnergy();
	esEnergyPlane1SCEle[index] = photon.superCluster()-> preshowerEnergyPlane1();
	esEnergyPlane2SCEle[index] = photon.superCluster()-> preshowerEnergyPlane2();

	//  energySCEle_corr[index] = photon.scEcalEnergy(); //but, I don't think this is the correct energy..

	// change in an electron properties please, EleNewEnergyProducer
	e3x3SCEle[index] = clustertools->e3x3(*photon.superCluster()->seed());
	e5x5SCEle[index] = clustertools->e5x5(*photon.superCluster()->seed());
	eSeedSCEle[index] = photon.superCluster()->seed()->energy();
	R9Ele[index] = e3x3SCEle[index] / photon.superCluster()->rawEnergy(); //original

	//   if(isMC){
	//     if(photon.isEB())
	//       R9Ele[index] = R9Ele[index]*1.0045+0.0010;
	//     else
	//       R9Ele[index] = R9Ele[index]*1.0086-0.0007;
	//   }

	// eleID[index] = ((bool) photon.photonID("fiducial")) << 0;
	// eleID[index] += ((bool) photon.photonID("loose")) << 1;
	// eleID[index] += ((bool) photon.photonID("medium")) << 2;
	// eleID[index] += ((bool) photon.photonID("tight")) << 3;
	// eleID[index] += ((bool) photon.photonID("loose25nsRun2")) << 4;
	// eleID[index] += ((bool) photon.photonID("medium25nsRun2")) << 5;
	// eleID[index] += ((bool) photon.photonID("tight25nsRun2")) << 6;

	return;
}


void ZNtupleDumper:: TreeSetDiElectronVar(const pat::Electron& electron1, const pat::Electron& electron2)
{

	TreeSetSingleElectronVar(electron1, 0);
	TreeSetSingleElectronVar(electron2, 1);

	double t1 = TMath::Exp(-etaEle[0]);
	double t2 = TMath::Exp(-etaEle[1]);
	double t1q = t1 * t1;
	double t2q = t2 * t2;

	double angle = 1 -
	               ( (1 - t1q) * (1 - t2q) + 4 * t1 * t2 * cos(phiEle[0] - phiEle[1])) / (
	                   (1 + t1q) * (1 + t2q)
	               );


	invMass = sqrt(2 * electron1.energy() * electron2.energy() * angle);
	invMass_e5x5   = sqrt(2 * electron1.e5x5() * electron2.e5x5() *
	                      angle);

	invMass_efull5x5   = sqrt(2 * electron1.full5x5_e5x5() * electron2.full5x5_e5x5() *
	                      angle);

	invMass_SC = sqrt(2 * energySCEle[0] * energySCEle[1] *
	                  angle);

	invMass_SC_must = sqrt(2 * energySCEle_must[0] * energySCEle_must[1] *
	                       angle);

	invMass_SC_must_regrCorr_ele = sqrt(2 * energySCEle_must_regrCorr_ele[0] * energySCEle_must_regrCorr_ele[1] *
	                                    angle);

	invMass_SC_pho_regrCorr = sqrt(2 * energySCEle_pho_regrCorr[0] * energySCEle_pho_regrCorr[1] *
	                               angle);

	invMass_rawSC = sqrt(2 * rawEnergySCEle[0] * rawEnergySCEle[1] *
	                     angle);

	invMass_rawSC_must = sqrt(2 * rawEnergySCEle_must[0] * rawEnergySCEle_must[1] *
	                          angle);


	invMass_rawSC_esSC = sqrt(2 * (rawEnergySCEle[0] + esEnergySCEle[0]) *
	                          (rawEnergySCEle[1] + esEnergySCEle[1]) *
	                          angle);

	invMass_SC_corr = sqrt(2 * energySCEle_corr[0] * energySCEle_corr[1] *
	                       angle);


	if(electron1.genLepton() != 0 && electron2.genLepton() != 0) {
		invMass_MC     = sqrt(2 * electron1.genLepton()->energy() * electron2.genLepton()->energy() *
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

void ZNtupleDumper::TreeSetDiElectronVar(const pat::Electron& electron1, const reco::SuperCluster& electron2)
{

	TreeSetSingleElectronVar(electron1, 0);
	TreeSetSingleElectronVar(electron2, 1);

	double t1 = TMath::Exp(-etaEle[0]);
	double t2 = TMath::Exp(-etaEle[1]);
	double t1q = t1 * t1;
	double t2q = t2 * t2;

	double angle = 1 - ( (1 - t1q) * (1 - t2q) + 4 * t1 * t2 * cos(phiEle[0] - phiEle[1])) / ( (1 + t1q) * (1 + t2q) );


	invMass = sqrt(2 * electron1.energy() * electron2.energy() * angle);
	invMass_e5x5   = sqrt(2 * electron1.e5x5() * (clustertools->e5x5(*electron2.seed())) * angle);

	invMass_SC = sqrt(2 * energySCEle[0] * energySCEle[1] *  angle);

	invMass_SC_must = sqrt(2 * energySCEle_must[0] * energySCEle_must[1] *  angle);
	invMass_SC_must_regrCorr_ele = sqrt(2 * energySCEle_must_regrCorr_ele[0] * energySCEle_must_regrCorr_ele[1] *  angle);
	invMass_SC_pho_regrCorr = sqrt(2 * energySCEle_pho_regrCorr[0] * energySCEle_pho_regrCorr[1] *  angle);


	invMass_rawSC = sqrt(2 * rawEnergySCEle[0] * rawEnergySCEle[1] * angle);

	invMass_rawSC_must = sqrt(2 * rawEnergySCEle_must[0] * rawEnergySCEle_must[1] * angle);


	invMass_rawSC_esSC = sqrt(2 * (rawEnergySCEle[0] + esEnergySCEle[0]) *
	                          (rawEnergySCEle[1] + esEnergySCEle[1]) *
	                          angle);

	invMass_SC_corr = sqrt(2 * energySCEle_corr[0] * energySCEle_corr[1] *
	                       angle);


	invMass_MC = -100; // temporary set it to be -100 for SC

	return;
}


void ZNtupleDumper:: TreeSetMuMuGammaVar(const pat::Photon& photon, const pat::Muon& muon1, const pat::Muon& muon2)
{

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

	Z->SetE(photon.e5x5() + muon1.energy() + muon2.energy());
	invMass_e5x5 = Z->M();

	Z->SetE(energySCEle[0] + muon1.energy() + muon2.energy());
	invMass_SC = Z->M();

	Z->SetE(energySCEle_must[0] + muon1.energy() + muon2.energy());
	invMass_SC_must = Z->M();

	Z->SetE(rawEnergySCEle[0] + muon1.energy() + muon2.energy());
	invMass_rawSC = Z->M();

	Z->SetE(rawEnergySCEle_must[0] + muon1.energy() + muon2.energy());
	invMass_rawSC_must = Z->M();

	Z->SetE(rawEnergySCEle[0] + esEnergySCEle[0] + muon1.energy() + muon2.energy());
	invMass_rawSC_esSC = Z->M();

	Z->SetE(energySCEle_corr[0] + muon1.energy() + muon2.energy());
	invMass_SC_corr = Z->M();

	if(photon.genPhoton() != 0 && muon1.genLepton() != 0 && muon2.genLepton() != 0) {
		Z->SetE(photon.genPhoton()->energy() + muon1.genLepton()->energy() + muon2.genLepton()->energy());
		invMass_MC = Z->M();
	} else invMass_MC = 0;

	double t1 = TMath::Exp(-etaEle[1]);
	double t2 = TMath::Exp(-etaEle[2]);
	double t1q = t1 * t1;
	double t2q = t2 * t2;

	double angle = 1 -
	               ( (1 - t1q) * (1 - t2q) + 4 * t1 * t2 * cos(phiEle[1] - phiEle[2])) / (
	                   (1 + t1q) * (1 + t2q)
	               );

	invMass_mumu     = sqrt(2 * muon1.energy() * muon2.energy() *
	                        angle);

	delete Z;
	delete ph;
	delete m1;
	delete m2;

	return;
}



// method to get the raw energy of one plane of ES summing the energy of only recHits associated to the electron SC
///\todo highly inefficient: instead of the loop over the recHits should use a ->find() method, it should return both energies of both planes
float ZNtupleDumper::GetESPlaneRawEnergy(const reco::SuperClusterRef& sc, unsigned int planeIndex)
{
	float RawenergyPlane = 0;
	float pfRawenergyPlane = 0;

	if(!ESRechitsHandle.isValid())
		return RawenergyPlane;
	if (!sc->preshowerClusters().isAvailable()) //protection for miniAOD
		return RawenergyPlane;

	for(auto iES = sc->preshowerClustersBegin(); iES != sc->preshowerClustersEnd(); ++iES) {
		const std::vector< std::pair<DetId, float> > hits = (*iES)->hitsAndFractions();
		for(std::vector<std::pair<DetId, float> >::const_iterator rh = hits.begin(); rh != hits.end(); ++rh) {
			//      std::cout << "print = " << (*iES)->printHitAndFraction(iCount);
			//      ++iCount;
			for(ESRecHitCollection::const_iterator esItr = ESRechitsHandle->begin(); esItr != ESRechitsHandle->end(); ++esItr) {
				ESDetId rhid = ESDetId(esItr->id());
				if(rhid == (*rh).first) {
					// std::cout << " ES energy = " << esItr->energy() << " pf energy = " << (*rh).second << std::endl;
					if((int) rhid.plane() == (int) planeIndex) {
						RawenergyPlane += esItr->energy();
						pfRawenergyPlane += rh->second;
					}
					break;
				}
			}
		}
	}

	if (pfRawenergyPlane) ; // avoid compilation error for unused var
	if (RawenergyPlane);
	//std::cout << "LC DEBUG RawenergyPlane "<< RawenergyPlane << ", pfRawenergyPlane " << pfRawenergyPlane << std::endl;
	return pfRawenergyPlane;
}

//////////////


//#============================== extra calib tree
void ZNtupleDumper::InitExtraCalibTree()
{

	//  tree = new TTree("selected",fChain->GetTitle());
	std::cout << "[STATUS] InitExtraCalibTree" << std::endl;
	if(extraCalibTree == NULL) return;

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
	extraCalibTree->Branch("ampliErrUncalibRecHitSCEle1", &(ampliErrUncalibRecHitSCEle[0]));
	extraCalibTree->Branch("ampliErrUncalibRecHitSCEle2", &(ampliErrUncalibRecHitSCEle[1]));
	extraCalibTree->Branch("ampliUncalibRecHitSCEle1", &(ampliUncalibRecHitSCEle[0]));
	extraCalibTree->Branch("ampliUncalibRecHitSCEle2", &(ampliUncalibRecHitSCEle[1]));
	extraCalibTree->Branch("chi2UncalibRecHitSCEle1", &(chi2UncalibRecHitSCEle[0]));
	extraCalibTree->Branch("chi2UncalibRecHitSCEle2", &(chi2UncalibRecHitSCEle[1]));
	extraCalibTree->Branch("flagsUncalibRecHitSCEle1", &(flagsUncalibRecHitSCEle[0]));
	extraCalibTree->Branch("flagsUncalibRecHitSCEle2", &(flagsUncalibRecHitSCEle[1]));
	extraCalibTree->Branch("jitterErrUncalibRecHitSCEle1", &(jitterErrUncalibRecHitSCEle[0]));
	extraCalibTree->Branch("jitterErrUncalibRecHitSCEle2", &(jitterErrUncalibRecHitSCEle[1]));
	extraCalibTree->Branch("jitterUncalibRecHitSCEle1", &(jitterUncalibRecHitSCEle[0]));
	extraCalibTree->Branch("jitterUncalibRecHitSCEle2", &(jitterUncalibRecHitSCEle[1]));
	extraCalibTree->Branch("ootAmplitudesUncalibRecHitSCEle1", &(ootAmplisUncalibRecHitSCEle[0]));
	extraCalibTree->Branch("ootAmplitudesUncalibRecHitSCEle2", &(ootAmplisUncalibRecHitSCEle[1]));
	extraCalibTree->Branch("pedUncalibRecHitSCEle1", &(pedEUncalibRecHitSCEle[0]));
	extraCalibTree->Branch("pedUncalibRecHitSCEle2", &(pedEUncalibRecHitSCEle[1]));

	return;
}

void ZNtupleDumper::TreeSetExtraCalibVar(const pat::Electron& electron1, const pat::Electron& electron2)
{

	TreeSetExtraCalibVar(electron1, 0);
	TreeSetExtraCalibVar(electron2, 1);
	return;
}

void ZNtupleDumper::TreeSetExtraCalibVar(const pat::Electron& electron1, const reco::SuperCluster& electron2)
{

	TreeSetExtraCalibVar(electron1, 0);
	TreeSetExtraCalibVar(electron2, 1);
	return;
}

void ZNtupleDumper::resetExtraVariables(int index)
{
	unsigned int aidx = abs(index);
	recoFlagRecHitSCEle[aidx].clear();
	rawIdRecHitSCEle[aidx].clear();
	XRecHitSCEle[aidx].clear();
	YRecHitSCEle[aidx].clear();
	ZRecHitSCEle[aidx].clear();
	energyRecHitSCEle[aidx].clear();
	ampliErrUncalibRecHitSCEle[aidx].clear();
	ampliUncalibRecHitSCEle[aidx].clear();
	chi2UncalibRecHitSCEle[aidx].clear();
	flagsUncalibRecHitSCEle[aidx].clear();
	jitterErrUncalibRecHitSCEle[aidx].clear();
	jitterUncalibRecHitSCEle[aidx].clear();
	ootAmplisUncalibRecHitSCEle[aidx].clear();
	pedEUncalibRecHitSCEle[aidx].clear();
	LCRecHitSCEle[aidx].clear();
	ICRecHitSCEle[aidx].clear();
	AlphaRecHitSCEle[aidx].clear();
}

void ZNtupleDumper::TreeSetExtraCalibVar(const std::vector<std::pair<DetId, float> > & hitsFracs, int index, bool isEB)
{
	const EcalRecHitCollection *recHits = isEB ? clustertools->getEcalEBRecHitCollection() : clustertools->getEcalEERecHitCollection();
	const EcalUncalibratedRecHitCollection * uncHits = isEB ? pEBUncRecHits.product() : pEEUncRecHits.product();
	const EcalIntercalibConstantMap& icalMap = clustertools->getEcalIntercalibConstants();
	const edm::ESHandle<EcalLaserDbService>& laserHandle_ = clustertools->getLaserHandle();
	for (std::vector<std::pair<DetId, float> >::const_iterator detitr = hitsFracs.begin();
	        detitr != hitsFracs.end(); detitr++ ) {
		//      EcalRecHitCollection::const_iterator theSeedHit = recHits->find (id); // trash this
		EcalRecHitCollection::const_iterator oneHit = recHits->find( (detitr -> first) ) ;
		if(oneHit == recHits->end()) {
			edm::LogError("ZNtupleDumper") << "No intercalib const found for xtal "  << (detitr->first).rawId()
			                               << " in subdetector " << (detitr->first).subdetId() << " bailing out";
			//assert(0);
			continue;
		}
		recoFlagRecHitSCEle[index].push_back(oneHit->recoFlag());
		rawIdRecHitSCEle[index].push_back(detitr->first.rawId());
		if(isEB) {
			EBDetId recHitId(detitr->first);
			XRecHitSCEle[index].push_back(recHitId.ieta());
			YRecHitSCEle[index].push_back(recHitId.iphi());
			ZRecHitSCEle[index].push_back(recHitId.zside());
		} else {
			EEDetId recHitId(detitr->first);
			XRecHitSCEle[index].push_back(recHitId.ix());
			YRecHitSCEle[index].push_back(recHitId.iy());
			ZRecHitSCEle[index].push_back(recHitId.zside());
		}
		energyRecHitSCEle[index].push_back(oneHit->energy());
		EcalUncalibratedRecHitCollection::const_iterator oneUHit = uncHits->find( (detitr -> first) ) ;
		if(oneUHit == uncHits->end()) {
			edm::LogError("ZNtupleDumper") << "No uncalibRecHit found for xtal "  << (detitr->first).rawId()
			                               << " in subdetector " << (detitr->first).subdetId() << " bailing out";
			//assert(0);
			continue;
		}
		// UncalibRecHit's information on OOT amplitudes
		float amplis[EcalDataFrame::MAXSAMPLES];
		for (int i = 0; i < EcalDataFrame::MAXSAMPLES; ++i) amplis[i] = oneUHit->outOfTimeAmplitude(i);
		ootAmplisUncalibRecHitSCEle[index].push_back(amplis);
		ampliUncalibRecHitSCEle[index].push_back(oneUHit->amplitude());
		ampliErrUncalibRecHitSCEle[index].push_back(oneUHit->amplitudeError());
		pedEUncalibRecHitSCEle[index].push_back(oneUHit->pedestal());
		jitterUncalibRecHitSCEle[index].push_back(oneUHit->jitter());
		jitterErrUncalibRecHitSCEle[index].push_back(oneUHit->jitterError());
		chi2UncalibRecHitSCEle[index].push_back(oneUHit->chi2());
		flagsUncalibRecHitSCEle[index].push_back(oneUHit->flags());
		// in order to get back the ADC counts from the recHit energy, three ingredients are necessary:
		// 1) get laser correction coefficient
		LCRecHitSCEle[index].push_back(laserHandle_->getLaserCorrection(detitr->first, runTime_));
		//laserHandle->
		// 2) get intercalibration
		EcalIntercalibConstantMap::const_iterator icalit = icalMap.find(detitr->first);
		EcalIntercalibConstant icalconst = 1.;
		if( icalit != icalMap.end() ) {
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

}

void ZNtupleDumper::TreeSetExtraCalibVar(const pat::Electron& electron1, int index)
{
	resetExtraVariables(index);
	if (index < 0) return;

	//  EcalIntercalibConstantMap icMap = icHandle->get()
	std::vector< std::pair<DetId, float> > hitsAndFractions_ele1 = electron1.superCluster()->hitsAndFractions();
	nHitsSCEle[index] = hitsAndFractions_ele1.size();
	TreeSetExtraCalibVar(hitsAndFractions_ele1, index, electron1.isEB());
	return;
}

void ZNtupleDumper::TreeSetExtraCalibVar(const reco::SuperCluster& electron1, int index)
{
	resetExtraVariables(index);
	if (index < 0) return;

	std::vector< std::pair<DetId, float> > hitsAndFractions_ele1 = electron1.hitsAndFractions();
	nHitsSCEle[index] = hitsAndFractions_ele1.size();
	TreeSetExtraCalibVar(hitsAndFractions_ele1, index, electron1.seed()->seed().subdetId() == EcalBarrel);
	return;
}


void ZNtupleDumper::TreeSetExtraCalibVar(const pat::Photon& photon, const pat::Muon& muon1, const pat::Muon& muon2)
{

	TreeSetExtraCalibVar(photon, 0);
	TreeSetExtraCalibVar(muon1, -1);
	TreeSetExtraCalibVar(muon2, -2);
	return;
}


void ZNtupleDumper::TreeSetExtraCalibVar(const pat::Photon& photon, int index)
{
	resetExtraVariables(index);
	if (index < 0) return;

	//  EcalIntercalibConstantMap icMap = icHandle->get()
	std::vector< std::pair<DetId, float> > hitsAndFractions_ele1 = photon.superCluster()->hitsAndFractions();
	nHitsSCEle[index] = hitsAndFractions_ele1.size();
	TreeSetExtraCalibVar(hitsAndFractions_ele1, index, photon.superCluster()->seed()->seed().subdetId() == EcalBarrel);
	return;
}

void ZNtupleDumper::TreeSetExtraCalibVar(const pat::Muon& muon1, int index)
{
	resetExtraVariables(index);
	if (index < 0) return;
	return;
}

//#============================== Ele ID tree
void ZNtupleDumper::InitEleIDTree()
{

	//  tree = new TTree("selected",fChain->GetTitle());
	std::cout << "[STATUS] InitEleIDTree" << std::endl;
	if(eleIDTree == NULL) {
		return;
	}

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


void ZNtupleDumper::TreeSetEleIDVar(const pat::Electron& electron1, const pat::Electron& electron2)
{

	TreeSetEleIDVar(electron1, 0);
	TreeSetEleIDVar(electron2, 1);
	return;
}

void ZNtupleDumper::TreeSetEleIDVar(const pat::Electron& electron1, int index)
{
	if(index < 0) {
		hOverE[-index] = -1;
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

	pfMVA[index]   = electron1.mva_e_pi();
	hasMatchedConversion[index] = ConversionTools::hasMatchedConversion(electron1, conversionsHandle, bsHandle->position());
	maxNumberOfExpectedMissingHits[index] = electron1.gsfTrack()->hitPattern().numberOfLostHits(reco::HitPattern::MISSING_INNER_HITS);


	//   if (primaryVertexHandle->size() > 0) {
	//     reco::VertexRef vtx(primaryVertexHandle, 0);
	//     d0vtx[index] = electron1.gsfTrack()->dxy(vtx->position());
	//     dzvtx[index] = electron1.gsfTrack()->dz(vtx->position());
	//   } else {
	//     d0vtx[index] = electron1.gsfTrack()->dxy();
	//     dzvtx[index] = electron1.gsfTrack()->dz();
	//   }

	// eleIDloose[index]  = electron1.electronID("loose50nsRun2");
	// eleIDmedium[index] = electron1.electronID("medium50nsRun2");
	// eleIDtight[index]  = electron1.electronID("tight50nsRun2");
	return;
}

void ZNtupleDumper::TreeSetEleIDVar(const pat::Photon& photon, const pat::Muon& muon1, const pat::Muon& muon2)
{

	TreeSetEleIDVar(photon, 0);
	TreeSetEleIDVar(muon1, -1);
	TreeSetEleIDVar(muon2, -2);
	return;
}

void ZNtupleDumper::TreeSetEleIDVar(const pat::Photon& photon, int index)
{
	if(index < 0) {
		return;
	}

	sigmaIEtaIEtaSCEle[index]  = photon.sigmaIetaIeta(); // alcarereco
	hOverE[index] = photon.hadronicOverEm();

	const reco::SuperCluster photonSC = *(photon.superCluster());
	hasMatchedConversion[index] = ConversionTools::hasMatchedConversion(photonSC, conversionsHandle, bsHandle->position());

	// eleIDloose[index]  = photon.photonID("loose");
	// eleIDmedium[index] = photon.photonID("medium");
	// eleIDtight[index]  = photon.photonID("tight");
	return;
}


void ZNtupleDumper::TreeSetEleIDVar(const pat::Muon& muon1, int index)
{
	if(index < 0) {
		return;
	}

	//  eleIDloose[index]  = muon1.muonID("loose");
	//  eleIDmedium[index] = muon1.muonID("medium");
	//  eleIDtight[index]  = muon1.muonID("tight");
	return;
}


//#============================== Ele ID tree
void ZNtupleDumper::InitPdfSystTree(void)
{

	//  tree = new TTree("selected",fChain->GetTitle());
	std::cout << "[STATUS] InitPdfSystTree" << std::endl;
	if(pdfSystTree == NULL) {
		std::cerr << "[ERROR] pdfSystTree==NULL" << std::endl;
		exit(1);
		return;
	}
	//   pdfSystTree->Branch("runNumber",     &runNumber,     "runNumber/I");
	//   pdfSystTree->Branch("eventNumber",   &eventNumber, "eventNumber/l");
	//   pdfSystTree->Branch("lumiBlock",     &lumiBlock,     "lumiBlock/I");
	//   pdfSystTree->Branch("runTime",       &runTime,         "runTime/i");

#ifdef PDFWEIGHTS
// this part is deprecated
	for(std::vector< edm::InputTag >::const_iterator pdfWeightTAGS_itr = pdfWeightTAGS.begin();
	        pdfWeightTAGS_itr != pdfWeightTAGS.end();
	        pdfWeightTAGS_itr++) {
		int i = pdfWeightTAGS_itr - pdfWeightTAGS.begin();
		std::string tagName = pdfWeightTAGS_itr->instance();
		//tagName.replace(0,pdfWeightTAGS_itr->label().size());
		//std::cout << i << "\t" << tagName << "\t" << pdfWeightTAGS_itr->label() << "\t" << pdfWeightTAGS_itr->encode() << std::endl;
		//pdfSystTree->Branch(pdfWeightTAGS_itr->encode().c_str(), &(pdfSystWeightNum[i]), "pdfSystWeightNum/I");
		pdfSystTree->Branch((pdfWeightTAGS_itr->label() + "_" + pdfWeightTAGS_itr->instance()).c_str(), &(pdfSystWeight[i]));
	}

	pdfSystTree->Branch("fsrWeight", &fsrWeight, "fsrWeight/F");
	pdfSystTree->Branch("weakWeight", &weakWeight, "weakWeight/F");
#endif
	return;
}

void ZNtupleDumper::TreeSetPdfSystVar(const edm::Event& iEvent)
{
#ifdef PDFWEIGHTS
	for(std::vector< edm::InputTag >::const_iterator pdfWeightTAGS_itr = pdfWeightTAGS.begin();
	        pdfWeightTAGS_itr != pdfWeightTAGS.end();
	        pdfWeightTAGS_itr++) {
		int i = pdfWeightTAGS_itr - pdfWeightTAGS.begin();
		iEvent.getByToken(*pdfWeightTAGS_itr, pdfWeightHandle);

		//pdfSystWeight[i] =
		std::vector<Double_t> weights = std::vector<Double_t>(*pdfWeightHandle);
		pdfSystWeight[i] = weights;
		//std::cout << "Event weight for central PDF " << pdfWeightTAGS_itr->encode() << ": " << pdfSystWeight[i][0] << std::endl;
		//unsigned int nmembers = weights.size();
		//pdfSystWeightNum[i]=pdfSystWeight.size();
		//    for (unsigned int j=1; j<nmembers; j+=2) {
		//      std::cout << "Event weight for PDF variation +" << (j+1)/2 << ": " << pdfSystWeight[i][j] << std::endl;
		//      std::cout << "Event weight for PDF variation -" << (j+1)/2 << ": " << pdfSystWeight[i][j+1] << std::endl;
		//    }
	}

	iEvent.getByToken(fsrWeightTAG, fsrWeightHandle);
	iEvent.getByToken(weakWeightTAG, weakWeightHandle);

	fsrWeight = (Float_t) * fsrWeightHandle;
	weakWeight = (Float_t) * weakWeightHandle;
#endif
	return ;
}

//define this as a plug-in
DEFINE_FWK_MODULE(ZNtupleDumper);

//  LocalWords:  pileupInfoTAG conversionsProducerTAG triggerResultsTAG
