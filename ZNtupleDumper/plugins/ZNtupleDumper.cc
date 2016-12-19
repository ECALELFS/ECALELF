// -*- C++ -*-
//
// Package:    ZNtupleDumper
// Class:      ZNtupleDumper
//
/// Zee and E/p ntuple dumper from patElectrons
/**\class ZNtupleDumper ZNtupleDumper.cc Calibration/ZNtupleDumper/src/ZNtupleDumper.cc
 * \author Shervin Nourbakhsh
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

// HLT trigger
#include "FWCore/Framework/interface/TriggerNamesService.h"
#include <FWCore/Common/interface/TriggerNames.h>
#include <DataFormats/Common/interface/TriggerResults.h>

// alcaSkimPaths
#include "DataFormats/Provenance/interface/ParameterSetID.h"

#include "Calibration/ZNtupleDumper/interface/eleIDMap.h"
#include "DataFormats/Common/interface/ValueMap.h"

// number of electrons in each branch (max nEle)
#define NELE 3
#define initFloat     {-999.,-999.,-999.}
#define initInt       {0,0,0}
#define initIntCharge {-100,-100,-100}
//#define DEBUG


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
	const edm::Event *pEvent;      // to make the event content available in private methods
	const edm::EventSetup *pSetup; // to make the event setup available in private methods

	bool isMC;
	bool isPartGun;
	double doHighEta_LowerEtaCut;

	//Handles and inputTags
private:
	//------------------------------ Handles
	edm::ESHandle<CaloTopology> topologyHandle;
	//--------------- for main tree
	edm::Handle<std::vector<pat::Electron> > electronsHandle;
	edm::Handle<std::vector<pat::Muon> > muonsHandle;
	edm::Handle<std::vector<pat::Photon> > photonsHandle;
	edm::Handle<std::vector<reco::SuperCluster>> EESuperClustersHandle; //used only for high-eta
	edm::Handle<reco::VertexCollection> primaryVertexHandle; // for nPV
	edm::Handle<double> rhoHandle;
	edm::Handle<std::vector< PileupSummaryInfo > >  PupInfo;
	edm::Handle< GenEventInfoProduct >  GenEventInfoHandle;
	edm::Handle< reco::PFMETCollection > metHandle;
	edm::Handle<edm::TriggerResults> triggerResultsHandle;
	edm::Handle<edm::TriggerResults> WZSkimResultsHandle;
	edm::Handle<EcalRecHitCollection> ESRechitsHandle;

	//--------------- for eleIDtree
	edm::Handle<reco::BeamSpot> bsHandle;
	edm::Handle<reco::ConversionCollection> conversionsHandle;

	//--------------- for extraCalibTree
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
	edm::EDGetTokenT<edm::TriggerResults> triggerResultsToken_;
	edm::EDGetTokenT<edm::TriggerResults> WZSkimResultsToken_;
	edm::InputTag triggerResultsTAG, WZSkimResultsTAG;
	std::vector< std::string> hltPaths, SelectEvents;
private:
	std::string ntupleFileName;

	bool doExtraCalibTree; ///< bool to activate the dump of the extra calib tree for E/p ICs
	bool doExtraStudyTree; ///< bool to activate the dump of the extra tree for study with values for single recHits
	bool doEleIDTree;      ///< bool to activate the dump of the electronID variables in a separate tree

	TTree *tree;                   //< output file for standard ntuple

	// ntuple members, private to make them visible in doxygen
private:
	/**
		\addtogroup BRANCHES
		@{
	 */
	UInt_t     	runNumber;		///< run number
	UShort_t      lumiBlock;		///< lumi section
	Long64_t    eventNumber;	///< event number
	UInt_t		eventTime;		///< unix time of the event
	UShort_t		nBX;			///< bunch crossing

	Float_t       mcGenWeight; ///< weight in generator for MC

	std::vector< std::string > HLTNames[1];		///< List of HLT names
	std::vector<Bool_t>			HLTResults[1];	///< 0 = fail, 1=fire
	std::map<std::string, bool> HLTBits;
	Bool_t						HLTfire;		///< true if pass the triggers indicated by hltPaths in cfg

	//pileup
	Float_t		rho;            ///< rho fast jet
	UChar_t		nPV;            ///< nVtx
	UChar_t		nPU;		    ///< number of PU (filled only for MC)

	// selection
	UInt_t eleID[NELE] = initInt;      ///< bit mask for eleID: 1=fiducial, 2=loose, 6=medium, 14=tight, 16=WP90PU, 48=WP80PU, 112=WP70PU, 128=loose25nsRun2, 384=medium25nsRun2, 896=tight25nsRun2, 1024=loose50nsRun2, 3072=medium50nsRun2, 7168=tight50nsRun2. Selection from https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaCutBasedIdentification#Electron_ID_Working_Points

	Short_t  chargeEle[NELE]			= initIntCharge;	///< -100: no electron, 0: SC or photon, -1 or +1:electron or muon //Char_t is interpreted as char and not as integer
	UChar_t recoFlagsEle[NELE]		= initInt;	///< 1=trackerDriven, 2=ecalDriven only, 3=tracker and ecal driven
	Float_t   etaEle[NELE]			= initFloat, phiEle[NELE]  = initFloat;		///< phi of the electron (electron object)
	Float_t R9Ele[NELE] = initFloat;    ///< e3x3/rawEnergySCEle
	Float_t fbremEle[NELE]			= initFloat;

	Bool_t isMustacheSC[NELE] = initInt;
	Float_t etaSCEle[NELE]			= initFloat, phiSCEle[NELE] = initFloat;	///< phi of the SC

	// seed of the SC
	Short_t xSeedSC[NELE]			 = initInt;		///< ieta(ix) of the SC seed in EB(EE)
	Short_t ySeedSC[NELE]			 = initInt;		///< iphi(iy) of the SC seed in EB(EE)
	UChar_t gainSeedSC[NELE]		 = initInt;	///< Gain switch 0==gain12, 1==gain6, 2==gain1; gain status of the seed of the SC
	Float_t energySeedSC[NELE]		 = initFloat;		///< energy of the rechit seeding the SC
	Float_t timeSeedSC[NELE]         = initFloat;       ///< time of the rechit seeding the SC
	Float_t laserSeedSC[NELE]		 = initFloat;		///< laser correction of the SC seed crystal
	Float_t avgLCSC[NELE]			 = initFloat;
	Float_t alphaSeedSC[NELE]		 = initFloat;		///<alpha of the seed
	Float_t slewRateDeltaESeed[NELE] = initFloat;		///< slew rate correction for seed crystal energy

	Float_t energyEle[NELE]					 = initFloat;		///< electron.energy(), not changed by rereco
	Float_t rawEnergySCEle[NELE]			 = initFloat;		///< SC energy without cluster corrections
	Float_t energy_ECAL_ele[NELE]			 = initFloat;		///< ele-tuned regression energy: mustache for rereco and correctedEcalEnergy for official reco
	Float_t energy_ECAL_pho[NELE]			 = initFloat;		///< pho-tuned regression energy: mustache for rereco and correctedEcalEnergy for official reco
	Float_t energyUncertainty_ECAL_ele[NELE] = initFloat;		///< ele-tuned regression energy: mustache for rereco and correctedEcalEnergy for official reco
	Float_t energyUncertainty_ECAL_pho[NELE] = initFloat;		///< pho-tuned regression energy: mustache for rereco and correctedEcalEnergy for official reco

	Float_t esEnergySCEle[NELE]			 = initFloat;	///< pre-shower energy associated to the electron
	Float_t esEnergyPlane1SCEle[NELE]	 = initFloat;	///< energy associate to the electron in the first plane of ES
	Float_t esEnergyPlane2SCEle[NELE]	 = initFloat;	///< energy associate to the electron in the second plane of ES
	Float_t rawESEnergyPlane1SCEle[NELE] = initFloat;	///< pre-shower rechit energy sum of Plane 1 associated to the electron
	Float_t rawESEnergyPlane2SCEle[NELE] = initFloat;	///< pre-shower recHit energy sum of Plane 2 associated to the electron

	Float_t energy_3x3SC[NELE]				= initFloat;	//< sum of the recHit energy in 3x3 matrix centered at the seed of the SC
	Float_t energy_5x5SC[NELE]				= initFloat;	///< sum of the recHit energy in 5x5 matrix centered at the seed of the SC
	Float_t eBCseedEle[NELE]			= initFloat;	///< energy of the basic cluster seeding the SC
	Float_t pModeGsfEle[NELE]			= initFloat;	///< track momentum from Gsf Track (mode)
	Float_t pAtVtxGsfEle[NELE]			= initFloat;	///< momentum estimated at the vertex
	Float_t trackMomentumErrorEle[NELE] = initFloat;	///< track momentum error from standard electron method
	Float_t pNormalizedChi2Ele[NELE]	= initFloat;	///< track normalized chi2 of the fit (GSF)

	Float_t invMass;
	Float_t invMass_rawSC;
	Float_t invMass_rawSC_esSC;
	Float_t invMass_ECAL_ele;   ///< invariant mass using ECAL energy, this is mustache ele-tuned regression if rereco, and correctedEcalEnergy if official reco
	Float_t invMass_ECAL_pho;   ///< invariant mass using ECAL energy, this is mustache pho-tuned regression if rereco, and correctedEcalEnergy if official reco
	//   Float_t invMass_e3x3;
	Float_t invMass_5x5SC;

	Float_t invMass_mumu;

	Float_t   etaMCEle[NELE]  = initFloat, phiMCEle[NELE] = initFloat;
	Float_t energyMCEle[NELE] = initFloat;		///< Electron MC true energy
	Float_t		invMass_MC;


	//============================== ExtraCalibTree (for E/p)
	TFile *extraCalibTreeFile;
	TTree *extraCalibTree;
	edm::Timestamp eventTime_;
	Int_t nRecHitsEle[3];
	Int_t nHitsSCEle[3] = initInt;
	std::vector<unsigned int>  rawIdRecHitSCEle[3];
	std::vector<int>           XRecHitSCEle[3];
	std::vector<int>           YRecHitSCEle[3];
	std::vector<int>           ZRecHitSCEle[3];
	std::vector<float>         energyRecHitSCEle[3];
	std::vector<float>         fracRecHitSCEle[3];
	std::vector<int>           recoFlagRecHitSCEle[3];
	//==============================

	//============================== ExtraStudyTree
	TFile *extraStudyTreeFile;
	TTree *extraStudyTree;
	std::vector<float>               LCRecHitSCEle[3];
	std::vector<float>               AlphaRecHitSCEle[3];
	std::vector<float>               ICRecHitSCEle[3];
	std::vector<std::vector<float> > ootAmplisUncalibRecHitSCEle[3];
	std::vector<float>               ampliUncalibRecHitSCEle[3];
	std::vector<float>               ampliErrUncalibRecHitSCEle[3];
	std::vector<float>               pedEUncalibRecHitSCEle[3];
	std::vector<float>               jitterUncalibRecHitSCEle[3];
	std::vector<float>               jitterErrUncalibRecHitSCEle[3];
	std::vector<float>               chi2UncalibRecHitSCEle[3];
	std::vector<uint32_t>            flagsUncalibRecHitSCEle[3];

	//============================== check ele-id and iso
	TFile *eleIDTreeFile;
	TTree *eleIDTree;
	Float_t sigmaIEtaIEtaSCEle[3] = initFloat;
	Float_t sigmaIPhiIPhiSCEle[3] = initFloat;
	Float_t hOverE[3] = initFloat, hOverEBC[3] = initFloat;
	Float_t dr03TkSumPt[3] = initFloat;
	Float_t dr03EcalRecHitSumEt[3] = initFloat;
	Float_t dr03HcalTowerSumEt[3] = initFloat;
	Float_t deltaPhiSuperClusterTrackAtVtx[3] = initFloat;
	Float_t deltaEtaSuperClusterTrackAtVtx[3] = initFloat;
	Float_t E1x5[3] = initFloat;
	Float_t E1x3[3] = initFloat;
	Float_t E2x2[3] = initFloat;
	Float_t E2x5Max[3] = initFloat;
	Float_t S4[3] = initFloat;
	Float_t etaWidth[3] = initFloat;
	Float_t phiWidth[3] = initFloat;
	Bool_t hasMatchedConversion[3] = initInt;
	Int_t maxNumberOfExpectedMissingHits[3] = initInt;
	Float_t pfMVA[3] = initFloat;
	Float_t eleIDloose[3] = initFloat, eleIDmedium[3] = initFloat, eleIDtight[3] = initFloat;
	//==============================

	//==============================

	/**
	 @}
	*/
	//==============================
private:
	TFile *tree_file;
	void InitNewTree(void);

	void TreeSetSingleSCVar(const reco::SuperCluster& sc, int index);

	void TreeSetSingleElectronVar(const pat::Electron& electron1, int index);
	void TreeSetSingleElectronVar(const reco::SuperCluster& sc, int index);
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

	void InitExtraStudyTree(void); // the extra study tree uses the method of the extracalibtree

	void InitEleIDTree(void);
	void TreeSetEleIDVar(const pat::Electron& electron1, int index);
	void TreeSetEleIDVar(const pat::Electron& electron1, const pat::Electron& electron2);
	void TreeSetEleIDVar(const pat::Photon& photon, const pat::Muon& muon1, const pat::Muon& muon2);
	void TreeSetEleIDVar(const pat::Photon& photon, int index);
	void TreeSetEleIDVar(const pat::Muon& muon1, int index);

	//  void Tree_output(TString filename);
	void TreeSetEventSummaryVar(const edm::Event& iEvent);
	void TreeSetPileupVar(void);
	float GetESPlaneRawEnergy(const reco::SuperCluster& sc, unsigned int planeIndex);

	bool elePreselection(const pat::Electron& electron);
	//puWeights_class puWeights;

private:

	// --------------- selection cuts
private:


	//------------------------------
	// cluster tools
	noZS::EcalClusterLazyTools *clustertools;
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
		UNKNOWN,
		SINGLEELE, //no skim, no preselection and no selection are applied
		DEBUG_T
	} eventType_t;


	eventType_t eventType;
	unsigned int eventTypeCounter[DEBUG_T] = {0};

	bool _isMINIAOD;
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
	ebURHToken_(mayConsume<EcalUncalibratedRecHitCollection>(iConfig.getParameter<edm::InputTag>( "uncalibRecHitCollectionEB" ))),
	eeURHToken_(mayConsume<EcalUncalibratedRecHitCollection>(iConfig.getParameter<edm::InputTag>( "uncalibRecHitCollectionEE" ))),
	EESuperClustersToken_(consumes<reco::SuperClusterCollection>(iConfig.getParameter< edm::InputTag>("EESuperClusterCollection"))),
	rhoToken_(consumes<double>(iConfig.getParameter<edm::InputTag>("rhoFastJet"))),
	pileupInfoToken_(consumes<std::vector<PileupSummaryInfo>>(iConfig.getParameter<edm::InputTag>("pileupInfo"))),
	conversionsProducerToken_(consumes<reco::ConversionCollection>(iConfig.getParameter<edm::InputTag>("conversionCollection"))),
	metToken_(consumes<reco::PFMETCollection>(iConfig.getParameter<edm::InputTag>("metCollection"))),
	triggerResultsToken_(consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("triggerResultsCollection"))),
	WZSkimResultsToken_(consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("WZSkimResultsCollection"))),
	triggerResultsTAG(iConfig.getParameter<edm::InputTag>("triggerResultsCollection")),
	WZSkimResultsTAG(iConfig.getParameter<edm::InputTag>("WZSkimResultsCollection")),
	hltPaths(iConfig.getParameter< std::vector<std::string> >("hltPaths")),
	SelectEvents(iConfig.getParameter<std::vector<std::string> >("SelectEvents")),
	ntupleFileName(iConfig.getParameter<std::string>("foutName")),
	doExtraCalibTree(iConfig.getParameter<bool>("doExtraCalibTree")),
	doExtraStudyTree(iConfig.getParameter<bool>("doExtraStudyTree")),
	doEleIDTree(iConfig.getParameter<bool>("doEleIDTree")),
	preselID_(iConfig.getParameter<bool>("useIDforPresel")),
	// used for preselection and event type determination
	eleID_loose(iConfig.getParameter< std::string>("eleID_loose")),
	eleID_medium(iConfig.getParameter< std::string>("eleID_medium")),
	eleID_tight(iConfig.getParameter< std::string>("eleID_tight")),
	eventType(ZEE)
{
	_isMINIAOD = !(iConfig.getParameter<edm::InputTag>("recHitCollectionEB") == edm::InputTag("alCaIsolatedElectrons", "alcaBarrelHits"));
	if(_isMINIAOD) std::cout << "[INFO ZntupleDumper] running on MINIAOD" << std::endl;
	//  current_dir.cd();


}

ZNtupleDumper::~ZNtupleDumper()
{

	// do anything here that needs to be done at desctruction time
	// (e.g. close files, deallocate resources etc.)

	std::cout << "[STATUS] Calling the destructor" << std::endl;
}

// ------------ method called once each job just before starting event loop  ------------
void ZNtupleDumper::beginJob()
{
#ifdef DEBUG
	std::cout << "[DEBUG] Starting creation of ntuples" << std::endl;
#endif

	tree_file = new TFile(ntupleFileName.c_str(), "recreate");
	if(tree_file->IsZombie()) {
		throw cms::Exception("OutputError") <<  "Output tree not created (Zombie): " << ntupleFileName;
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
			throw cms::Exception("OutputError") <<  "Output tree for extra calib not created (Zombie): " << ntupleFileName;
			return;
		}
		extraCalibTreeFile->cd();

		extraCalibTree = new TTree("extraCalibTree", "extraCalibTree");
		extraCalibTree->SetDirectory(extraCalibTreeFile);
		InitExtraCalibTree();
	}

	if(doExtraStudyTree) {
		extraStudyTreeFile = new TFile("extraStudyTree.root", "recreate");
		if(extraStudyTreeFile->IsZombie()) {
			throw cms::Exception("OutputError") <<  "Output tree for extra study not created (Zombie): " << ntupleFileName;
			return;
		}
		extraStudyTreeFile->cd();

		extraStudyTree = new TTree("extraStudyTree", "extraStudyTree");
		extraStudyTree->SetDirectory(extraStudyTreeFile);
		InitExtraStudyTree();
	}


	if(doEleIDTree) {
		eleIDTreeFile = new TFile("eleIDTree.root", "recreate");
		if(eleIDTreeFile->IsZombie()) {
			throw cms::Exception("OutputError") <<  "Output tree for extra calib not created (Zombie): " << ntupleFileName;
			return;
		}
		eleIDTreeFile->cd();
		eleIDTree = new TTree("eleIDTree", "eleIDTree");
		eleIDTree->SetDirectory(eleIDTreeFile);
		//eleIDTree = fs->make<TTree>("eleIDTree","");
		InitEleIDTree();
	}

	//for(int i = ZEE; i <= DEBUG_T && i <= 7; i++) {
	//	eventTypeCounter[i] = 0;
	//}

#ifdef DEBUG
	std::cout << "[DEBUG] End creation of ntuples" << std::endl;
#endif
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


	energyEle[0] = -99;
	energyEle[1] = -99;

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
					if(hltName_str.find("WElectron") != std::string::npos)
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
	clustertools = new noZS::EcalClusterLazyTools(iEvent, iSetup, recHitCollectionEBToken_,
	        recHitCollectionEEToken_);

	if(doExtraCalibTree || doExtraStudyTree) {
		iEvent.getByToken(ebURHToken_, pEBUncRecHits);
		iEvent.getByToken(eeURHToken_, pEEUncRecHits);
	}
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

	iEvent.getByToken(metToken_, metHandle);
	iEvent.getByToken(recHitCollectionESToken_, ESRechitsHandle);
	//if(metHandle.isValid()==false) iEvent.getByType(metHandle);
	reco::PFMET met = metHandle.isValid() ? ((*metHandle))[0] : reco::PFMET(); /// \todo use corrected phi distribution


	//Here the HLTBits are filled. TriggerResults
	TreeSetEventSummaryVar(iEvent);
	TreeSetPileupVar(); // this can be filled once per event


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

	//if (eventType!=ZMMG) { // count the number of electrons passing the different IDs for preselection and event type determination
	if (eventType != UNKNOWN) { // count the number of electrons passing the different IDs for preselection and event type determination
		for( pat::ElectronCollection::const_iterator eleIter1 = electronsHandle->begin();
		        eleIter1 != electronsHandle->end();
		        eleIter1++) {
			if( eleIter1->electronID(eleID_tight) )          ++nTight;
			else if( eleIter1->electronID(eleID_medium) ) ++nMedium;
			else if( eleIter1->electronID(eleID_loose) )  ++nLoose;
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
		if(doExtraCalibTree || doExtraStudyTree) {
			TreeSetExtraCalibVar(*eleIter1, *eleIter2);
		}
		if(doEleIDTree) {
			TreeSetEleIDVar(*eleIter1, *eleIter2);
		}
	} else if(eventType == ZEE || eventType == WENU || eventType == UNKNOWN) {
#ifdef DEBUG
		std::cout << "[DEBUG] Electrons in the event: " << electronsHandle->size() << std::endl;
#endif

		for( pat::ElectronCollection::const_iterator eleIter1 = electronsHandle->begin();
		        eleIter1 != electronsHandle->end();
		        eleIter1++) {

			if(! elePreselection(*eleIter1)) continue;
			if(!eleIter1->ecalDrivenSeed()) continue; // skip tracker-driven only electrons
			if(eventType == WENU) {
				if(! (eleIter1->electronID(eleID_tight)) ) continue;
				if( nTight != 1 || nLoose > 0 ) continue; //to be a Wenu event request only 1 ele WP70 in the event

				// MET/MT selection
				if(  met.et() < 25. ) continue;
				if( sqrt( 2.*eleIter1->et()*met.et() * (1 - cos(eleIter1->phi() - met.phi()))) < 50. ) continue;
				if( eleIter1->et() < 30) continue;

				doFill = true;
				if(eventType == UNKNOWN) eventType = WENU;
				TreeSetSingleElectronVar(*eleIter1, 0);  //fill first electron
				TreeSetSingleElectronVar(*eleIter1, -1); // fill fake second electron

				if(doExtraCalibTree || doExtraStudyTree) {
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

					if(! elePreselection(*eleIter2)) continue;
					if(!eleIter2->ecalDrivenSeed()) continue; // skip tracker-driven only electrons
#ifdef DEBUG
					std::cout << "[DEBUG] Electron passing preselection" << std::endl;
#endif
					//	  float mass=(eleIter1->p4()+eleIter2->p4()).mass();

					//calculate the invariant mass
					double t1 = TMath::Exp(-eleIter1->eta());
					double t1q = t1 * t1;
					double t2 = TMath::Exp(-eleIter2->eta());
					double t2q = t2 * t2;

//					if(!eleIter2->parentSuperCluster().isNonnull()) continue;
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
					if(eleID[0] < 2 || (abs(chargeEle[1]) == 1 && eleID[1] < 2)) {
						// this event is not passing any eleID, skip it
						doFill = false;
						continue;
					}
					if(doExtraCalibTree || doExtraStudyTree) {
						TreeSetExtraCalibVar(*eleIter1, *eleIter2);
					}
					if(doEleIDTree) {
						TreeSetEleIDVar(*eleIter1, *eleIter2);
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

					if(doExtraCalibTree || doExtraStudyTree) {
						TreeSetExtraCalibVar(*phoIter1, *muIter1, *muIter2);
					}
					if(doEleIDTree) {
						TreeSetEleIDVar(*phoIter1, *muIter1, *muIter2);
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
			if(!PatEle1->ecalDrivenSeed()) continue;

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
			if(doExtraCalibTree || doExtraStudyTree) {
				TreeSetExtraCalibVar(*PatEle1, *HighEtaSC1);
			}
		}
	}

	if(doFill) {
		tree->Fill();
		if(doExtraCalibTree) extraCalibTree->Fill();
		if(doEleIDTree)      eleIDTree->Fill();
		if(doExtraStudyTree) extraStudyTree->Fill();
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



// ------------ method called once each job just after ending the event loop  ------------
void ZNtupleDumper::endJob()
{

	if(tree->GetEntries() > 0) {
		tree->BuildIndex("runNumber", "eventNumber");
		if(doEleIDTree)       eleIDTree->BuildIndex("runNumber", "eventNumber");
		if(doExtraCalibTree) extraCalibTree->BuildIndex("runNumber", "eventNumber");
		if(doExtraStudyTree) extraStudyTree->BuildIndex("runNumber", "eventNumber");
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
	if(doExtraStudyTree) {
		extraStudyTreeFile->cd();
		extraStudyTree->Write();
		extraStudyTreeFile->Close();
	}
	if(doEleIDTree) {
		eleIDTreeFile->cd();
		eleIDTree->Write();
		eleIDTreeFile->Close();
	}

	std::cout << DEBUG_T << std::endl;
//	for(size_t i = 0; i < 8; ++i) {
//		std::cout << "[NTUPLEDUMPER] EventTypeCounter[" << i << "]\t" << eventTypeCounter[i] << std::endl;
//	}
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
	tree->Branch("runNumber",     &runNumber,   "runNumber/i");
	tree->Branch("lumiBlock",     &lumiBlock,   "lumiBlock/s");
	tree->Branch("eventNumber",   &eventNumber, "eventNumber/l");
	tree->Branch("eventTime",       &eventTime,     "eventTime/i");
	tree->Branch("nBX",           &nBX,         "nBX/s");

	tree->Branch("mcGenWeight",   &mcGenWeight, "mcGenWeight/F");

	tree->Branch("HLTfire", &HLTfire, "HLTfire/B");
	//tree->Branch("HLTNames",&(HLTNames[0]));
	//tree->Branch("HLTResults",&(HLTResults[0]));

	// pu
	tree->Branch("rho", &rho, "rho/F");
	tree->Branch("nPV", &nPV, "nPV/b");
	tree->Branch("nPU", &nPU, "nPU/b");

	// ele
	tree->Branch("eleID", eleID, "eleID[3]/i");
	tree->Branch("chargeEle",   chargeEle,    "chargeEle[3]/S");
	tree->Branch("recoFlagsEle", recoFlagsEle, "recoFlagsEle[3]/b");
	tree->Branch("etaEle",      etaEle,       "etaEle[3]/F");
	tree->Branch("phiEle",      phiEle,       "phiEle[3]/F");
	tree->Branch("fbremEle",    fbremEle,     "fbremEle[3]/F");
	tree->Branch("R9Ele", R9Ele, "R9Ele[3]/F");


	// SC
	tree->Branch("etaSCEle",      etaSCEle,       "etaSCEle[3]/F");
	tree->Branch("phiSCEle",      phiSCEle,       "phiSCEle[3]/F");
	tree->Branch("nHitsSCEle", nHitsSCEle, "nHitsSCEle[3]/I");
	tree->Branch("avgLCSC",  avgLCSC,       "avgLCSC[3]/F");
//	tree->Branch("isMustacheSC", isMustacheSC, "isMustacheSC[3]/b");
	tree->Branch("rawEnergySCEle", rawEnergySCEle, "rawEnergySCEle[3]/F");
	tree->Branch("energy_ECAL_ele", energy_ECAL_ele, "energy_ECAL_ele[3]/F"); ///< correctedEcalEnergy from MINIAOD or mustache regression if rereco
	tree->Branch("energy_ECAL_pho", energy_ECAL_pho, "energy_ECAL_pho[3]/F");
	tree->Branch("energyUncertainty_ECAL_ele", energyUncertainty_ECAL_ele, "energyUncertainty_ECAL_ele[3]/F"); ///< correctedEcalEnergy from MINIAOD or mustache regression if rereco
	tree->Branch("energyUncertainty_ECAL_pho", energyUncertainty_ECAL_pho, "energyUncertainty_ECAL_pho[3]/F");

	tree->Branch("energy_5x5SC", energy_5x5SC, "energy_5x5SC[3]/F");
	tree->Branch("pModeGsfEle", pModeGsfEle, "pModeGsfEle[3]/F");
	tree->Branch("pAtVtxGsfEle", pAtVtxGsfEle, "pAtVtxGsfEle[3]/F");
	tree->Branch("pNormalizedChi2Ele", pNormalizedChi2Ele, "pNormalizedChi2Ele[3]/F");
	tree->Branch("trackMomentumErrorEle", trackMomentumErrorEle, "trackMomentumErrorEle[3]/F");

	// seed recHit
	tree->Branch("xSeedSC",            xSeedSC,            "xSeedSC[3]/S");
	tree->Branch("ySeedSC",            ySeedSC,            "ySeedSC[3]/S");
	tree->Branch("gainSeedSC", gainSeedSC, "gainSeedSC[3]/b");
	tree->Branch("energySeedSC",       energySeedSC,       "energySeedSC[3]/F");
	tree->Branch("timeSeedSC",       timeSeedSC,       "timeSeedSC[3]/F");
	tree->Branch("laserSeedSC",        laserSeedSC,        "laserSeedSC[3]/F");
	tree->Branch("alphaSeedSC",        alphaSeedSC,        "alphaSeedSC[3]/F");
	tree->Branch("slewRateDeltaESeed", slewRateDeltaESeed, "slewRateDeltaESeed[3]/F");

	// ES
	tree->Branch("esEnergySCEle", esEnergySCEle, "esEnergySCEle[3]/F");
	tree->Branch("esEnergyPlane2SCEle", esEnergyPlane2SCEle, "esEnergyPlane2SCEle[3]/F");
	tree->Branch("esEnergyPlane1SCEle", esEnergyPlane1SCEle, "esEnergyPlane1SCEle[3]/F");
	tree->Branch("rawESEnergyPlane2SCEle", rawESEnergyPlane2SCEle, "rawESEnergyPlane2SCEle[3]/F");
	tree->Branch("rawESEnergyPlane1SCEle", rawESEnergyPlane1SCEle, "rawESEnergyPlane1SCEle[3]/F");

	// MC truth
	tree->Branch("etaMCEle",      etaMCEle,       "etaMCEle[3]/F");	//[nEle]
	tree->Branch("phiMCEle",      phiMCEle,       "phiMCEle[3]/F");	//[nEle]
	tree->Branch("invMass_MC", &invMass_MC, "invMass_MC/F");

	// invariant mass
	tree->Branch("invMass",    &invMass,      "invMass/F");
	tree->Branch("invMass_ECAL_ele", &invMass_ECAL_ele, "invMass_ECAL_ele/F"); ///< using correctedEcalEnergy or using mustache SC dedicated regression
	tree->Branch("invMass_ECAL_pho", &invMass_ECAL_pho,   "invMass_ECAL_pho/F");
	//   tree->Branch("invMass_e3x3",    &invMass_e3x3,      "invMass_e3x3/F");
	tree->Branch("invMass_5x5SC",    &invMass_5x5SC,      "invMass_5x5SC/F");
	//tree->Branch("invMass_efull5x5",    &invMass_efull5x5,      "invMass_efull5x5/F");
	tree->Branch("invMass_rawSC", &invMass_rawSC,   "invMass_rawSC/F");
	tree->Branch("invMass_rawSC_esSC", &invMass_rawSC_esSC,   "invMass_rawSC_esSC/F");

	return;
}



void ZNtupleDumper::TreeSetEventSummaryVar(const edm::Event& iEvent)
{

	eventTime_   =  iEvent.eventAuxiliary().time();
	eventTime = (UInt_t) eventTime_.unixTime();
	runNumber = (UInt_t) iEvent.run();
	eventNumber = (Long64_t) iEvent.id().event();
//	nBX = (UShort_t) (iEvent.isRealData()) ? iEvent.bunchCrossing() : 0;
	nBX = (UShort_t)  iEvent.bunchCrossing();

	lumiBlock = (UShort_t) iEvent.luminosityBlock();

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
	nPV = 255;
	nPU = 255;
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
				nPU = PVI->getTrueNumInteractions();
			}
		}

		if(!GenEventInfoHandle->weights().empty()) {
			mcGenWeight = (GenEventInfoHandle->weights())[0];
		}
	}
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
void ZNtupleDumper::TreeSetSingleElectronVar(const pat::Electron& electron, int index)
{

	if(index < 0) {
		chargeEle[-index] = -100;
		etaEle[-index]    = 0;
		phiEle[-index]    = 0;
		recoFlagsEle[-index] = -1;
		return;
	}
	assert(electron.ecalDrivenSeed());

	energyEle[index] = electron.energy();
	chargeEle[index] = (Char_t)electron.charge();
	etaEle[index]    = electron.eta(); // degli elettroni
	phiEle[index]    = electron.phi();
	fbremEle[index]  = electron.fbrem(); // fatta con pIn-pOut

	if(electron.ecalDrivenSeed()) {
		if(electron.trackerDrivenSeed()) recoFlagsEle[index] = 3;
		else recoFlagsEle[index] = 2;
	} else recoFlagsEle[index] = 1;

	isMustacheSC[index] = !electron.superCluster().isNonnull();
	assert(isMustacheSC);
	const reco::SuperClusterRef& sc = isMustacheSC[index] ?  electron.parentSuperCluster() : electron.superCluster();
	assert(sc.isNonnull()); // at least one of the SCs has to be defined!

	TreeSetSingleSCVar(*sc, index);

	energy_ECAL_ele[index]			  = (_isMINIAOD) ? electron.correctedEcalEnergy()     : electron.userFloat("energySCEleMust");
	energyUncertainty_ECAL_ele[index] = (_isMINIAOD) ? electron.correctedEcalEnergyError() : electron.userFloat("energySCEleMustVar");

	energy_ECAL_pho[index]			  = electron.userFloat("energySCElePho");
	energyUncertainty_ECAL_pho[index] = electron.userFloat("energySCElePhoVar");

	slewRateDeltaESeed[index] = electron.hasUserFloat("slewRateDeltaESeed") ? electron.userFloat("slewRateDeltaESeed") : 0;

	// change in an electron properties please, EleNewEnergyProducer
	energy_3x3SC[index] = clustertools->e3x3(*sc->seed());
	energy_5x5SC[index] = electron.full5x5_e5x5();

	Float_t efull5x5SCEle = clustertools->e5x5(*sc->seed()); // this is the official one, in rerecoes it is the one of the prompt reco, not updated by the rereco
	if(fabs(efull5x5SCEle - energy_5x5SC[index]) > 1e-6) {
		std::cout << "[WARNING] " << efull5x5SCEle << "\t" << energy_5x5SC[index] << "\t" << efull5x5SCEle - energy_5x5SC[index] << "\t" << clustertools->n5x5(*sc->seed()) << "\t" << etaEle[index] << "\t" << runNumber << "\t" << eventNumber << std::endl;
	}
	//if(efull5x5SCEle>15) assert(fabs(efull5x5SCEle - energy_5x5SC[index])< 1e-6);

	pModeGsfEle[index] = electron.gsfTrack()->pMode();
	trackMomentumErrorEle[index] = electron.trackMomentumError();
	pNormalizedChi2Ele[index] = electron.gsfTrack()->normalizedChi2();
	pAtVtxGsfEle[index] = electron.trackMomentumAtVtx().R();

	//R9Ele[index] = e3x3SCEle[index] / sc->rawEnergy(); //already commented
	R9Ele[index] = electron.full5x5_r9();
	if(fabs(R9Ele[index] - energy_3x3SC[index] / sc->rawEnergy()) > 1e-4) {
		std::cerr << "[WARNING] R9 different: " << runNumber << "\t" << eventNumber << "\t" << etaEle[index] << "\t" << R9Ele[index] << "\t" << energy_3x3SC[index] / sc->rawEnergy() << std::endl;
	}

	eleIDMap eleID_map;

	eleID[index] = 0;
	for (std::map<std::string, UInt_t>::iterator it = eleID_map.eleIDmap.begin(); it != eleID_map.eleIDmap.end(); ++it) {
#ifdef check_newID
		if((it->first).compare("loose25nsRun2V2016") == 0) {
			std::cout << "eleID is " << it->first << "; isAvailable is " << electron.isElectronIDAvailable(it->first) << std::endl;
		}
#endif
		if(electron.isElectronIDAvailable(it->first)) { //
			if ((bool) electron.electronID(it->first))  eleID[index] |= it->second;//
		}//
	}


//	return;//To be fixed
	//  sigmaIEtaIEtaSCEle[index] = sqrt(clustertools->localCovariances(*(electron.superCluster()->seed()))[index]);
	//  sigmaIEtaIEtaSCEle[1] = sqrt(clustertools->localCovariances(*(electron2.superCluster()->seed()))[index]);
	//  sigmaIPhiIPhiBC = sqrt(clustertools->localCovariances(*b)[3]);
	//  sigmaIEtaIPhiBC = clustertools->localCovariances(*b)[1];

	if(electron.genLepton() != 0) {
		energyMCEle[index]  = electron.genLepton()->energy();
		etaMCEle[index]  = electron.genLepton()->eta();
		phiMCEle[index]  = electron.genLepton()->phi();
	} else {
		energyMCEle[index] = 0;
		etaMCEle[index] = 0;
		phiMCEle[index] = 0;
	}

	return;
}

void ZNtupleDumper::TreeSetSingleSCVar(const reco::SuperCluster& sc, int index)
{
	etaSCEle[index] = sc.eta();
	phiSCEle[index] = sc.phi();

	nHitsSCEle[index] = sc.size();

	rawEnergySCEle[index]	   = sc.rawEnergy();
	eBCseedEle[index] = sc.seed()->energy();

	esEnergySCEle[index]	   = sc.preshowerEnergy();
	esEnergyPlane1SCEle[index] = sc. preshowerEnergyPlane1();
	esEnergyPlane2SCEle[index] = sc. preshowerEnergyPlane2();
	rawESEnergyPlane1SCEle[index] = GetESPlaneRawEnergy(sc, 1);
	rawESEnergyPlane2SCEle[index] = GetESPlaneRawEnergy(sc, 2);


	DetId seedDetId = sc.seed()->seed();
	if(seedDetId.subdetId() == EcalBarrel) {
		EBDetId seedDetIdEcal = seedDetId;
		xSeedSC[index] = seedDetIdEcal.ieta();
		ySeedSC[index] = seedDetIdEcal.iphi();
	} else {
		EEDetId seedDetIdEcal = seedDetId;
		xSeedSC[index] = seedDetIdEcal.ix();
		ySeedSC[index] = seedDetIdEcal.iy();
	}

	const EcalRecHitCollection *recHits = (seedDetId.subdetId() == EcalBarrel) ?  clustertools->getEcalEBRecHitCollection() : clustertools->getEcalEERecHitCollection();
	EcalRecHitCollection::const_iterator seedRecHit = recHits->find(seedDetId) ;
	assert(seedRecHit != recHits->end());
	energySeedSC[index] = seedRecHit->energy();
	timeSeedSC[index]   = seedRecHit->time();

	const edm::ESHandle<EcalLaserDbService>& laserHandle_ = clustertools->getLaserHandle();
	laserSeedSC[index] = laserHandle_->getLaserCorrection(seedDetId, eventTime_);

	edm::ESHandle<EcalLaserDbService> aSetup; //ALPHA PART
	pSetup->get<EcalLaserDbRecord>().get( aSetup );
	const EcalLaserAlphas* myalpha =  aSetup->getAlphas();
	const EcalLaserAlphaMap& laserAlphaMap =  myalpha->getMap();
	alphaSeedSC[index] = *(laserAlphaMap.find( seedDetId ));

	float sumLC_E = 0.;
	float sumE = 0.;

	std::vector< std::pair<DetId, float> > hitsAndFractions = sc.hitsAndFractions();
	for (const auto&  detitr : hitsAndFractions) {
		EcalRecHitCollection::const_iterator oneHit = recHits->find( (detitr.first) ) ;
#ifdef DEBUG
		assert(oneHit != recHits->end()); // DEBUG
#endif
		sumLC_E += laserHandle_->getLaserCorrection(detitr.first, eventTime_) * oneHit->energy();
		sumE    += oneHit->energy();
	}
	avgLCSC[index] = sumLC_E / sumE;

	if(seedRecHit->checkFlag(EcalRecHit::kHasSwitchToGain6)) gainSeedSC[index] |= 0x01;
	if(seedRecHit->checkFlag(EcalRecHit::kHasSwitchToGain1)) gainSeedSC[index] |= 0x02;

	return;
}


// a negative index means that the corresponding electron does not exist, fill with 0
void ZNtupleDumper::TreeSetSingleElectronVar(const reco::SuperCluster& electron1, int index)
{

	if(index < 0) {
		chargeEle[-index] = -100;
		etaEle[-index]    = 0;
		phiEle[-index]    = 0;
		return;
	}

	//checks

	chargeEle[index] = 0; // dont know the charge for SC
	etaEle[index]    = electron1.eta(); // eta = etaSC
	phiEle[index]    = electron1.phi();

	recoFlagsEle[index] = -1; // define -1 as a SC

	fbremEle[index] = -1; // no bremstrahlung for SC

	etaSCEle[index] = electron1.eta(); // itself is a SC
	phiSCEle[index] = electron1.phi();

	// no MC matching has been considered yet for SCV
	energyMCEle[index] = -100;
	etaMCEle[index] = -100;
	phiMCEle[index] = -100;


	pModeGsfEle[index] = -1; // no track, though ..
	trackMomentumErrorEle[index] = -1;
	pNormalizedChi2Ele[index] = -1;
	pAtVtxGsfEle[index] = -1;


	// temporary ignor the id and classification
	eleID[index] = -100;

	TreeSetSingleSCVar(electron1, index);
	return;
}

// a negative index means that the corresponding muon does not exist, fill with 0
void ZNtupleDumper::TreeSetSingleMuonVar(const pat::Muon& muon1, int index)
{

	if(index < 0) {
		chargeEle[-index] = -100;
		etaEle[-index]    = 0;
		phiEle[-index]    = 0;
		return;
	}

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


	return;
}


// a negative index means that the corresponding electron does not exist, fill with 0
void ZNtupleDumper::TreeSetSinglePhotonVar(const pat::Photon& photon, int index)
{

	if(index < 0) {
		chargeEle[-index] = -100;
		etaEle[-index]    = 0;
		phiEle[-index]    = 0;
		return;
	}

	chargeEle[index] = (Char_t) photon.charge();
	etaEle[index]    = photon.eta();
	phiEle[index]    = photon.phi();

	etaSCEle[index] = photon.superCluster()->eta();
	phiSCEle[index] = photon.superCluster()->phi();

	TreeSetSingleSCVar(*(photon.superCluster()), index);

	if(photon.genParticle() != 0) {
		energyMCEle[index]  = photon.genParticle()->energy();
		etaMCEle[index]  = photon.genParticle()->eta();
		phiMCEle[index]  = photon.genParticle()->phi();
	} else {
		energyMCEle[index] = 0;
		etaMCEle[index] = 0;
		phiMCEle[index] = 0;
	}


	rawEnergySCEle[index]  = photon.superCluster()->rawEnergy();
	esEnergySCEle[index] = photon.superCluster()->preshowerEnergy();
	esEnergyPlane1SCEle[index] = photon.superCluster()-> preshowerEnergyPlane1();
	esEnergyPlane2SCEle[index] = photon.superCluster()-> preshowerEnergyPlane2();

	//  energySCEle_corr[index] = photon.scEcalEnergy(); //but, I don't think this is the correct energy..

	// change in an electron properties please, EleNewEnergyProducer
	energy_3x3SC[index] = clustertools->e3x3(*photon.superCluster()->seed());
	energy_5x5SC[index] = clustertools->e5x5(*photon.superCluster()->seed());
	eBCseedEle[index] = photon.superCluster()->seed()->energy();
	R9Ele[index] = energy_3x3SC[index] / photon.superCluster()->rawEnergy(); //original

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


	invMass		= sqrt(2 * energyEle[0] * energyEle[1] * angle);
	invMass_5x5SC = sqrt(2 * energy_5x5SC[0] * energy_5x5SC[1] * angle);

	invMass_ECAL_ele = sqrt(2 * energy_ECAL_ele[0] * energy_ECAL_ele[1] * angle);
	invMass_ECAL_pho = sqrt(2 * energy_ECAL_pho[0] * energy_ECAL_pho[1] * angle);

	invMass_rawSC = sqrt(2 * rawEnergySCEle[0] * rawEnergySCEle[1] * angle);

	invMass_rawSC_esSC = sqrt(2 * (rawEnergySCEle[0] + esEnergySCEle[0]) *
	                          (rawEnergySCEle[1] + esEnergySCEle[1]) *
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
	invMass_5x5SC   = sqrt(2 * energy_5x5SC[0] * energy_5x5SC[1] * /// full 5x5
	                       angle);

	invMass_ECAL_ele = sqrt(2 * energy_ECAL_ele[0] * energy_ECAL_ele[1] * angle);
	invMass_ECAL_pho = sqrt(2 * energy_ECAL_pho[0] * energy_ECAL_pho[1] * angle);

	invMass_rawSC = sqrt(2 * rawEnergySCEle[0] * rawEnergySCEle[1] *
	                     angle);

	invMass_rawSC_esSC = sqrt(2 * (rawEnergySCEle[0] + esEnergySCEle[0]) *
	                          (rawEnergySCEle[1] + esEnergySCEle[1]) *
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

//	ph->SetPtEtaPhiE(PtEle[0], etaEle[0], phiEle[0], photon.energy());
//	m1->SetPtEtaPhiE(PtEle[1], etaEle[1], phiEle[1], muon1.energy());
//	m2->SetPtEtaPhiE(PtEle[2], etaEle[2], phiEle[2], muon2.energy());

	*Z = *ph + *m1 + *m2;
	invMass = Z->M();

	Z->SetE(photon.e5x5() + muon1.energy() + muon2.energy());
	invMass_5x5SC = Z->M();

	Z->SetE(rawEnergySCEle[0] + muon1.energy() + muon2.energy());
	invMass_rawSC = Z->M();

	Z->SetE(rawEnergySCEle[0] + esEnergySCEle[0] + muon1.energy() + muon2.energy());
	invMass_rawSC_esSC = Z->M();

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

	invMass     = sqrt(2 * muon1.energy() * muon2.energy() *
	                   angle);

	delete Z;
	delete ph;
	delete m1;
	delete m2;

	return;
}



// method to get the raw energy of one plane of ES summing the energy of only recHits associated to the electron SC
///\todo highly inefficient: instead of the loop over the recHits should use a ->find() method, it should return both energies of both planes
float ZNtupleDumper::GetESPlaneRawEnergy(const reco::SuperCluster& sc, unsigned int planeIndex)
{
	double RawenergyPlane = 0.;
	double pfRawenergyPlane = 0.;

//	if(!ESRechitsHandle.isValid())
//		return RawenergyPlane;
	if (!sc.preshowerClusters().isAvailable()) //protection for miniAOD
		return -999;

	for(auto iES = sc.preshowerClustersBegin(); iES != sc.preshowerClustersEnd(); ++iES) {//loop over preshower clusters
		const std::vector< std::pair<DetId, float> > hits = (*iES)->hitsAndFractions();
		for(std::vector<std::pair<DetId, float> >::const_iterator rh = hits.begin(); rh != hits.end(); ++rh) { // loop over recHits of the cluster
			//      std::cout << "print = " << (*iES)->printHitAndFraction(iCount);
			//      ++iCount;
			for(ESRecHitCollection::const_iterator esItr = ESRechitsHandle->begin(); esItr != ESRechitsHandle->end(); ++esItr) {//loop over ES rechits to find the one in the cluster
				ESDetId rhid = ESDetId(esItr->id());
				if(rhid == (*rh).first) { // found ESrechit
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
	return RawenergyPlane;
}

//////////////


//#============================== extra calib tree
void ZNtupleDumper::InitExtraCalibTree()
{

	//  tree = new TTree("selected",fChain->GetTitle());
	std::cout << "[STATUS] InitExtraCalibTree" << std::endl;
	if(extraCalibTree == NULL) return;

	extraCalibTree->Branch("runNumber",     &runNumber,   "runNumber/i");
	extraCalibTree->Branch("lumiBlock",     &lumiBlock,   "lumiBlock/s");
	extraCalibTree->Branch("eventNumber",   &eventNumber, "eventNumber/l");
	extraCalibTree->Branch("eventTime",       &eventTime,     "eventTime/i");

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

	extraCalibTree->Branch("fracRecHitSCEle1", &(fracRecHitSCEle[0]));
	extraCalibTree->Branch("fracRecHitSCEle2", &(fracRecHitSCEle[1]));

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
	fracRecHitSCEle[aidx].clear();
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
		fracRecHitSCEle[index].push_back(detitr->second);
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
		ootAmplisUncalibRecHitSCEle[index].push_back(std::vector<float>(amplis, amplis + EcalDataFrame::MAXSAMPLES));
		ampliUncalibRecHitSCEle[index].push_back(oneUHit->amplitude());
		ampliErrUncalibRecHitSCEle[index].push_back(oneUHit->amplitudeError());
		pedEUncalibRecHitSCEle[index].push_back(oneUHit->pedestal());
		jitterUncalibRecHitSCEle[index].push_back(oneUHit->jitter());
		jitterErrUncalibRecHitSCEle[index].push_back(oneUHit->jitterError());
		chi2UncalibRecHitSCEle[index].push_back(oneUHit->chi2());
		flagsUncalibRecHitSCEle[index].push_back(oneUHit->flags());
		// in order to get back the ADC counts from the recHit energy, three ingredients are necessary:
		// 1) get laser correction coefficient
		LCRecHitSCEle[index].push_back(laserHandle_->getLaserCorrection(detitr->first, eventTime_));
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


//#============================== extra study tree
void ZNtupleDumper::InitExtraStudyTree()
{

	//  tree = new TTree("selected",fChain->GetTitle());
	std::cout << "[STATUS] InitExtraStudyTree" << std::endl;
	if(extraStudyTree == NULL) return;

	extraStudyTree->Branch("runNumber",     &runNumber,   "runNumber/i");
	extraStudyTree->Branch("lumiBlock",     &lumiBlock,   "lumiBlock/s");
	extraStudyTree->Branch("eventNumber",   &eventNumber, "eventNumber/l");
	extraStudyTree->Branch("eventTime",       &eventTime,     "eventTime/i");

	extraStudyTree->Branch("LCRecHitSCEle1", &(LCRecHitSCEle[0]));
	extraStudyTree->Branch("LCRecHitSCEle2", &(LCRecHitSCEle[1]));
	extraStudyTree->Branch("AlphaRecHitSCEle1", &(AlphaRecHitSCEle[0]));
	extraStudyTree->Branch("AlphaRecHitSCEle2", &(AlphaRecHitSCEle[1]));
	extraStudyTree->Branch("ICRecHitSCEle1", &(ICRecHitSCEle[0]));
	extraStudyTree->Branch("ICRecHitSCEle2", &(ICRecHitSCEle[1]));
	extraStudyTree->Branch("ampliErrUncalibRecHitSCEle1", &(ampliErrUncalibRecHitSCEle[0]));
	extraStudyTree->Branch("ampliErrUncalibRecHitSCEle2", &(ampliErrUncalibRecHitSCEle[1]));
	extraStudyTree->Branch("ampliUncalibRecHitSCEle1", &(ampliUncalibRecHitSCEle[0]));
	extraStudyTree->Branch("ampliUncalibRecHitSCEle2", &(ampliUncalibRecHitSCEle[1]));
	extraStudyTree->Branch("chi2UncalibRecHitSCEle1", &(chi2UncalibRecHitSCEle[0]));
	extraStudyTree->Branch("chi2UncalibRecHitSCEle2", &(chi2UncalibRecHitSCEle[1]));
	extraStudyTree->Branch("flagsUncalibRecHitSCEle1", &(flagsUncalibRecHitSCEle[0]));
	extraStudyTree->Branch("flagsUncalibRecHitSCEle2", &(flagsUncalibRecHitSCEle[1]));
	extraStudyTree->Branch("jitterErrUncalibRecHitSCEle1", &(jitterErrUncalibRecHitSCEle[0]));
	extraStudyTree->Branch("jitterErrUncalibRecHitSCEle2", &(jitterErrUncalibRecHitSCEle[1]));
	extraStudyTree->Branch("jitterUncalibRecHitSCEle1", &(jitterUncalibRecHitSCEle[0]));
	extraStudyTree->Branch("jitterUncalibRecHitSCEle2", &(jitterUncalibRecHitSCEle[1]));
	extraStudyTree->Branch("ootAmplitudesUncalibRecHitSCEle1", &(ootAmplisUncalibRecHitSCEle[0]));
	extraStudyTree->Branch("ootAmplitudesUncalibRecHitSCEle2", &(ootAmplisUncalibRecHitSCEle[1]));
	extraStudyTree->Branch("pedUncalibRecHitSCEle1", &(pedEUncalibRecHitSCEle[0]));
	extraStudyTree->Branch("pedUncalibRecHitSCEle2", &(pedEUncalibRecHitSCEle[1]));

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

	eleIDTree->Branch("runNumber",     &runNumber,   "runNumber/i");
	eleIDTree->Branch("lumiBlock",     &lumiBlock,   "lumiBlock/s");
	eleIDTree->Branch("eventNumber",   &eventNumber, "eventNumber/l");
	eleIDTree->Branch("eventTime",       &eventTime,     "eventTime/i");

	eleIDTree->Branch("dr03TkSumPt", dr03TkSumPt, "dr03TkSumPt[3]/F");
	eleIDTree->Branch("dr03EcalRecHitSumEt", dr03EcalRecHitSumEt, "dr03EcalRecHitSumEt[3]/F");
	eleIDTree->Branch("dr03HcalTowerSumEt", dr03HcalTowerSumEt, "dr03HcalTowerSumEt[3]/F");
	eleIDTree->Branch("sigmaIEtaIEtaSCEle", sigmaIEtaIEtaSCEle, "sigmaIEtaIEtaSCEle[3]/F");

	eleIDTree->Branch("E1x5",    E1x5,    "E1x5[3]/F");
	eleIDTree->Branch("E2x5Max", E2x5Max, "E2x5Max[3]/F");
	eleIDTree->Branch("E1x3",    E1x3,    "E1x3[3]/F");
	eleIDTree->Branch("E2x2",    E2x2,    "E2x2[3]/F");
	eleIDTree->Branch("S4", S4, "S4[3]/F");
	eleIDTree->Branch("etaWidth", etaWidth, "etaWidth[3]/F");
	eleIDTree->Branch("phiWidth", phiWidth, "phiWidth[3]/F");
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

void ZNtupleDumper::TreeSetEleIDVar(const pat::Electron& ele, int index)
{
	if(index < 0) {
		hOverE[-index] = -1;
		return;
	}

	dr03TkSumPt[index]					  = ele.dr03TkSumPt();
	dr03EcalRecHitSumEt[index]			  = ele.dr03EcalRecHitSumEt();
	dr03HcalTowerSumEt[index]			  = ele.dr03HcalTowerSumEt();
	sigmaIEtaIEtaSCEle[index]			  = ele.full5x5_sigmaIetaIeta();
	sigmaIPhiIPhiSCEle[index]			  = ele.full5x5_sigmaIphiIphi();
	E1x5[index]							  = ele.full5x5_e1x5();
	E2x5Max[index]						  = ele.full5x5_e2x5Max();
	deltaPhiSuperClusterTrackAtVtx[index] = ele.deltaPhiSuperClusterTrackAtVtx();
	deltaEtaSuperClusterTrackAtVtx[index] = ele.deltaEtaSuperClusterTrackAtVtx();
	hOverE[index]						  = ele.full5x5_hcalOverEcal();
	hOverEBC[index]                       = ele.full5x5_hcalOverEcalBc();

	pfMVA[index]   = ele.mva_e_pi();
	hasMatchedConversion[index] = ConversionTools::hasMatchedConversion(ele, conversionsHandle, bsHandle->position());
	maxNumberOfExpectedMissingHits[index] = ele.gsfTrack()->hitPattern().numberOfLostHits(reco::HitPattern::MISSING_INNER_HITS);

	const reco::CaloClusterPtr seed_clu = ele.superCluster()->seed();
	E1x3[index] = clustertools->e1x3( *seed_clu );
	E2x2[index] = clustertools->e2x2( *seed_clu );
	S4[index]   = clustertools->e2x2( *seed_clu ) / clustertools->e5x5( *seed_clu );
	etaWidth[index] = ele.superCluster()->etaWidth();
	phiWidth[index] = ele.superCluster()->phiWidth();

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



//define this as a plug-in
DEFINE_FWK_MODULE(ZNtupleDumper);

//  LocalWords:  pileupInfoTAG conversionsProducerTAG triggerResultsTAG
