// -*- C++ -*-
//
// Package:    EleNewEnergiesProducer
// Class:      EleNewEnergiesProducer
// 
/**\class EleNewEnergiesProducer EleNewEnergiesProducer.cc Calibration/EleNewEnergiesProducer/src/EleNewEnergiesProducer.cc

 Description: [one line class summary]

 Implementation:
 TODO:
     - add the error on the  regression energy
     - define the name of the collection
     - implement a switch to select which energies to produce
     - remove unuseful variables


     [Notes on implementation]
*/
//
// Original Author:  Shervin Nourbakhsh,40 1-B24,+41227671643,
//         Created:  Thu Jul  5 20:17:56 CEST 2012
// $Id$
//
//
//#define REGRESSION 3

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#ifdef CMSSW_7_2_X
   #include "FWCore/Framework/interface/ESHandle.h"
   #include "FWCore/Utilities/interface/EDGetToken.h"
#else
   #include "FWCore/Framework/interface/ESHandle.h"
#endif

// for the output
#include "DataFormats/Common/interface/ValueMap.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
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


// Josh's regression
#include "../interface/EGEnergyCorrector_fra.h" 
#include "RecoEgamma/EgammaTools/interface/EGEnergyCorrector.h"

//#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "HiggsAnalysis/GBRLikelihoodEGTools/interface/EGEnergyCorrectorSemiParm.h"
//#include "DataFormats/EgammaCandidates/interface/Photon.h"

//
// class declaration
//

class EleNewEnergiesProducer : public edm::EDProducer {
  
  typedef float v_t;
  typedef edm::ValueMap<v_t> NewEnergyMap;

   public:
      explicit EleNewEnergiesProducer(const edm::ParameterSet&);
      ~EleNewEnergiesProducer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() ;
      virtual void produce(edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      
      virtual void beginRun(edm::Run&, edm::EventSetup const&);
      virtual void endRun(edm::Run&, edm::EventSetup const&);
      virtual void beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);
      virtual void endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);

      // ----------member data ---------------------------

  // ----------member data ---------------------------

private:

  bool isMC;
  // input tag for primary vertex
  edm::InputTag vtxCollectionTAG;
  edm::InputTag BeamSpotTAG;
  
  /// input tag for electrons
  edm::InputTag electronsTAG;

#ifdef CMSSW_7_2_X
  edm::EDGetTokenT <EcalRecHitCollection> recHitCollectionEBTAG;
  edm::EDGetTokenT <EcalRecHitCollection> recHitCollectionEETAG;
#else
  edm::InputTag recHitCollectionEBTAG;
  edm::InputTag recHitCollectionEETAG;
#endif

  /// input rho
  edm::InputTag rhoTAG;

  edm::InputTag conversionsProducerTAG;

  std::string foutName;
  std::string puWeightFile;

private:
  // Handle to the electron collection
  edm::Handle<std::vector<reco::GsfElectron> > electronsHandle;
  edm::Handle<edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit> > > recHitCollectionEBHandle;
  edm::Handle<edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit> > > recHitCollectionEEHandle;
  edm::Handle<reco::BeamSpot> bsHandle;
  edm::Handle<reco::VertexCollection> primaryVertexHandle;
  edm::ESHandle<CaloTopology> topologyHandle;
  edm::Handle<double> rhoHandle;
  edm::Handle<std::vector< PileupSummaryInfo > >  PupInfo;
  edm::Handle<reco::ConversionCollection> conversionsHandle;

  //------------------------------ Josh's regression (Hgg)
  EGEnergyCorrector       josh_Ele;
  EGEnergyCorrector       josh_Pho;
  EGEnergyCorrector_fra    fra_Ele;


  std::string regrEleFile; //weights
  std::string regrPhoFile; 
  std::string regrEleFile_fra; 

  EGEnergyCorrectorSemiParm corV4_ele;
  EGEnergyCorrectorSemiParm corV4_pho;
  std::string regrEleJoshV4_SemiParamFile;
  std::string regrPhoJoshV4_SemiParamFile;

  EGEnergyCorrectorSemiParm corV5_ele;
  EGEnergyCorrectorSemiParm corV5_pho;
  std::string regrEleJoshV5_SemiParamFile;
  std::string regrPhoJoshV5_SemiParamFile;

  EGEnergyCorrectorSemiParm corV6_ele;
  EGEnergyCorrectorSemiParm corV6_pho;
  std::string regrEleJoshV6_SemiParamFile;
  std::string regrPhoJoshV6_SemiParamFile;

  EGEnergyCorrectorSemiParm corV7_ele;
  EGEnergyCorrectorSemiParm corV7_pho;
  std::string regrEleJoshV7_SemiParamFile;
  std::string regrPhoJoshV7_SemiParamFile;

  EGEnergyCorrectorSemiParm corV8_ele;
  EGEnergyCorrectorSemiParm corV8_pho;
  std::string regrEleJoshV8_SemiParamFile;
  std::string regrPhoJoshV8_SemiParamFile;

  EGEnergyCorrectorSemiParm cor7TeVtrainV6_ele;
  EGEnergyCorrectorSemiParm cor7TeVtrainV6_pho;
  std::string regrEleJoshV6_SemiParam7TeVtrainFile;
  std::string regrPhoJoshV6_SemiParam7TeVtrainFile;

  EGEnergyCorrectorSemiParm cor7TeVtrainV7_ele;
  EGEnergyCorrectorSemiParm cor7TeVtrainV7_pho;
  std::string regrEleJoshV7_SemiParam7TeVtrainFile;
  std::string regrPhoJoshV7_SemiParam7TeVtrainFile;

  EGEnergyCorrectorSemiParm cor7TeVtrainV8_ele;
  EGEnergyCorrectorSemiParm cor7TeVtrainV8_pho;
  std::string regrEleJoshV8_SemiParam7TeVtrainFile;
  std::string regrPhoJoshV8_SemiParam7TeVtrainFile;

  bool ptSplit;
};

//
// constants, enums and typedefs
//


//
// static data member definitions
//

//
// constructors and destructor
//
EleNewEnergiesProducer::EleNewEnergiesProducer(const edm::ParameterSet& iConfig):
  isMC(iConfig.getParameter<bool>("isMC")),
  vtxCollectionTAG(iConfig.getParameter<edm::InputTag>("vertexCollection")),
  BeamSpotTAG(iConfig.getParameter<edm::InputTag>("BeamSpotCollection")),
  electronsTAG(iConfig.getParameter<edm::InputTag>("electronCollection")),

#ifdef CMSSW_7_2_X
  recHitCollectionEBTAG(consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>( "recHitCollectionEB" ))),
  recHitCollectionEETAG(consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>( "recHitCollectionEE" ))),
#else
  recHitCollectionEBTAG(iConfig.getParameter<edm::InputTag>("recHitCollectionEB")),
  recHitCollectionEETAG(iConfig.getParameter<edm::InputTag>("recHitCollectionEE")),
#endif

  rhoTAG(iConfig.getParameter<edm::InputTag>("rhoFastJet")),
  conversionsProducerTAG(iConfig.getParameter<edm::InputTag>("conversionCollection")),
  //  foutName(iConfig.getParameter<std::string>("foutName")),
  regrEleFile(iConfig.getParameter<std::string>("regrEleFile")),
  regrPhoFile(iConfig.getParameter<std::string>("regrPhoFile")),
  regrEleFile_fra(iConfig.getParameter<std::string>("regrEleFile_fra")),
  regrEleJoshV4_SemiParamFile(iConfig.getParameter<std::string>("regrEleJoshV4_SemiParamFile")),
  regrPhoJoshV4_SemiParamFile(iConfig.getParameter<std::string>("regrPhoJoshV4_SemiParamFile")),
  regrEleJoshV5_SemiParamFile(iConfig.getParameter<std::string>("regrEleJoshV5_SemiParamFile")),
  regrPhoJoshV5_SemiParamFile(iConfig.getParameter<std::string>("regrPhoJoshV5_SemiParamFile")),
  regrEleJoshV6_SemiParamFile(iConfig.getParameter<std::string>("regrEleJoshV6_SemiParamFile")),
  regrPhoJoshV6_SemiParamFile(iConfig.getParameter<std::string>("regrPhoJoshV6_SemiParamFile")),
  regrEleJoshV7_SemiParamFile(iConfig.getParameter<std::string>("regrEleJoshV7_SemiParamFile")),
  regrPhoJoshV7_SemiParamFile(iConfig.getParameter<std::string>("regrPhoJoshV7_SemiParamFile")),
  regrEleJoshV8_SemiParamFile(iConfig.getParameter<std::string>("regrEleJoshV8_SemiParamFile")),
  regrPhoJoshV8_SemiParamFile(iConfig.getParameter<std::string>("regrPhoJoshV8_SemiParamFile")),
  regrEleJoshV6_SemiParam7TeVtrainFile(iConfig.getParameter<std::string>("regrEleJoshV6_SemiParam7TeVtrainFile")),
  regrPhoJoshV6_SemiParam7TeVtrainFile(iConfig.getParameter<std::string>("regrPhoJoshV6_SemiParam7TeVtrainFile")),
  regrEleJoshV7_SemiParam7TeVtrainFile(iConfig.getParameter<std::string>("regrEleJoshV7_SemiParam7TeVtrainFile")),
  regrPhoJoshV7_SemiParam7TeVtrainFile(iConfig.getParameter<std::string>("regrPhoJoshV7_SemiParam7TeVtrainFile")),
  regrEleJoshV8_SemiParam7TeVtrainFile(iConfig.getParameter<std::string>("regrEleJoshV8_SemiParam7TeVtrainFile")),
  regrPhoJoshV8_SemiParam7TeVtrainFile(iConfig.getParameter<std::string>("regrPhoJoshV8_SemiParam7TeVtrainFile")),

  ptSplit(iConfig.getParameter<bool>("ptSplit"))
{
   //register your products
/* Examples
   produces<ExampleData2>();

   //if do put with a label
   produces<ExampleData2>("label");
 
   //if you want to put into the Run
   produces<ExampleData2,InRun>();
*/
  
//   produces< std::vector<float> >("energySCEleJoshEle");
//   produces< std::vector<float> >("energySCEleJoshPho");

  // this name are hard coded, should be put in the cfi
  produces< NewEnergyMap >("energySCEleJoshEle");
  produces< NewEnergyMap >("energySCEleJoshEleVar");  
  produces< NewEnergyMap >("energySCEleJoshPho");
  produces< NewEnergyMap >("energySCEleJoshPhoVar");
  produces< NewEnergyMap >("energyEleFra");
  produces< NewEnergyMap >("energyEleFraVar");

  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV4ecorr");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV4sigma");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV4alpha1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV4gamma1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV4alpha2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV4gamma2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV4pdfval");

  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV4ecorr");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV4sigma");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV4alpha1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV4gamma1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV4alpha2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV4gamma2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV4pdfval");
  //------------------------------
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV5ecorr");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV5sigma");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV5alpha1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV5gamma1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV5alpha2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV5gamma2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV5pdfval");
					   
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV5ecorr");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV5sigma");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV5alpha1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV5gamma1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV5alpha2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV5gamma2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV5pdfval");
  //------------------------------

  //------------------------------
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV6ecorr");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV6sigmaEoverE");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV6cbmean");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV6sigma");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV6alpha1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV6gamma1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV6alpha2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV6gamma2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV6pdfval");

  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV6ecorr");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV6sigmaEoverE");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV6cbmean");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV6sigma");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV6alpha1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV6gamma1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV6alpha2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV6gamma2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV6pdfval");
  //------------------------------
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV7ecorr");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV7sigmaEoverE");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV7cbmean");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV7sigma");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV7alpha1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV7gamma1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV7alpha2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV7gamma2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV7pdfval");

  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV7ecorr");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV7sigmaEoverE");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV7cbmean");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV7sigma");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV7alpha1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV7gamma1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV7alpha2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV7gamma2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV7pdfval");
  //------------------------------
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV8ecorr");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV8sigmaEoverE");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV8cbmean");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV8sigma");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV8alpha1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV8gamma1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV8alpha2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV8gamma2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV8pdfval");

  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV8ecorr");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV8sigmaEoverE");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV8cbmean");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV8sigma");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV8alpha1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV8gamma1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV8alpha2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV8gamma2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV8pdfval");


  //------------------------------
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV6ecorr");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV6sigmaEoverE");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV6cbmean");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV6sigma");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV6alpha1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV6gamma1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV6alpha2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV6gamma2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV6pdfval");
						       
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV6ecorr");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV6sigmaEoverE");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV6cbmean");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV6sigma");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV6alpha1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV6gamma1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV6alpha2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV6gamma2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV6pdfval");
  //------------------------------		       7TeVtrain
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV7ecorr");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV7sigmaEoverE");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV7cbmean");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV7sigma");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV7alpha1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV7gamma1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV7alpha2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV7gamma2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV7pdfval");
						       
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV7ecorr");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV7sigmaEoverE");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV7cbmean");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV7sigma");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV7alpha1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV7gamma1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV7alpha2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV7gamma2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV7pdfval");
  //------------------------------		       
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV8ecorr");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV8sigmaEoverE");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV8cbmean");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV8sigma");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV8alpha1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV8gamma1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV8alpha2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV8gamma2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParam7TeVtrainV8pdfval");
						       
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV8ecorr");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV8sigmaEoverE");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV8cbmean");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV8sigma");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV8alpha1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV8gamma1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV8alpha2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV8gamma2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParam7TeVtrainV8pdfval");

 //now do what ever other initialization is needed
  
}


EleNewEnergiesProducer::~EleNewEnergiesProducer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
EleNewEnergiesProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
/* This is an event example
   //Read 'ExampleData' from the Event
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);

   //Use the ExampleData to create an ExampleData2 which 
   // is put into the Event
   std::auto_ptr<ExampleData2> pOut(new ExampleData2(*pIn));
   iEvent.put(pOut);
*/


/* this is an EventSetup example
   //Read SetupData from the SetupRecord in the EventSetup
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
*/

   std::vector<v_t>  energySCEleJoshEle;
   std::vector<v_t>  energySCEleJoshEleVar;
   std::vector<v_t>  energySCEleJoshPho;
   std::vector<v_t>  energySCEleJoshPhoVar;
   std::vector<v_t>  energyEleFra;
   std::vector<v_t>  energyEleFraVar;
   std::vector<v_t>  energySCEleJoshEleSemiParamV4_ecorr, energySCEleJoshEleSemiParamV4_sigma, 
     energySCEleJoshEleSemiParamV4_alpha1, energySCEleJoshEleSemiParamV4_gamma1, energySCEleJoshEleSemiParamV4_alpha2, 
     energySCEleJoshEleSemiParamV4_gamma2, energySCEleJoshEleSemiParamV4_pdfval;
   std::vector<v_t>  energySCEleJoshPhoSemiParamV4_ecorr, energySCEleJoshPhoSemiParamV4_sigma, 
     energySCEleJoshPhoSemiParamV4_alpha1, energySCEleJoshPhoSemiParamV4_gamma1, energySCEleJoshPhoSemiParamV4_alpha2, 
     energySCEleJoshPhoSemiParamV4_gamma2, energySCEleJoshPhoSemiParamV4_pdfval;
   std::vector<v_t>  energySCEleJoshEleSemiParamV5_ecorr, energySCEleJoshEleSemiParamV5_sigma, 
     energySCEleJoshEleSemiParamV5_alpha1, energySCEleJoshEleSemiParamV5_gamma1, energySCEleJoshEleSemiParamV5_alpha2, 
     energySCEleJoshEleSemiParamV5_gamma2, energySCEleJoshEleSemiParamV5_pdfval;
   std::vector<v_t>  energySCEleJoshPhoSemiParamV5_ecorr, energySCEleJoshPhoSemiParamV5_sigma, 
     energySCEleJoshPhoSemiParamV5_alpha1, energySCEleJoshPhoSemiParamV5_gamma1, energySCEleJoshPhoSemiParamV5_alpha2, 
     energySCEleJoshPhoSemiParamV5_gamma2, energySCEleJoshPhoSemiParamV5_pdfval;

   std::vector<v_t>  energySCEleJoshEleSemiParamV6_ecorr, energySCEleJoshEleSemiParamV6_sigmaEoverE, 
     energySCEleJoshEleSemiParamV6_cbmean,      energySCEleJoshEleSemiParamV6_sigma, 
     energySCEleJoshEleSemiParamV6_alpha1, energySCEleJoshEleSemiParamV6_gamma1, energySCEleJoshEleSemiParamV6_alpha2, 
     energySCEleJoshEleSemiParamV6_gamma2, energySCEleJoshEleSemiParamV6_pdfval;
   std::vector<v_t>  energySCEleJoshPhoSemiParamV6_ecorr, energySCEleJoshPhoSemiParamV6_sigmaEoverE, 
     energySCEleJoshPhoSemiParamV6_cbmean,      energySCEleJoshPhoSemiParamV6_sigma, 
     energySCEleJoshPhoSemiParamV6_alpha1, energySCEleJoshPhoSemiParamV6_gamma1, energySCEleJoshPhoSemiParamV6_alpha2, 
     energySCEleJoshPhoSemiParamV6_gamma2, energySCEleJoshPhoSemiParamV6_pdfval;

   std::vector<v_t>  energySCEleJoshEleSemiParamV7_ecorr, energySCEleJoshEleSemiParamV7_sigmaEoverE, 
     energySCEleJoshEleSemiParamV7_cbmean,      energySCEleJoshEleSemiParamV7_sigma, 
     energySCEleJoshEleSemiParamV7_alpha1, energySCEleJoshEleSemiParamV7_gamma1, energySCEleJoshEleSemiParamV7_alpha2, 
     energySCEleJoshEleSemiParamV7_gamma2, energySCEleJoshEleSemiParamV7_pdfval;
   std::vector<v_t>  energySCEleJoshPhoSemiParamV7_ecorr, energySCEleJoshPhoSemiParamV7_sigmaEoverE, 
     energySCEleJoshPhoSemiParamV7_cbmean,      energySCEleJoshPhoSemiParamV7_sigma, 
     energySCEleJoshPhoSemiParamV7_alpha1, energySCEleJoshPhoSemiParamV7_gamma1, energySCEleJoshPhoSemiParamV7_alpha2, 
     energySCEleJoshPhoSemiParamV7_gamma2, energySCEleJoshPhoSemiParamV7_pdfval;

   std::vector<v_t>  energySCEleJoshEleSemiParamV8_ecorr, energySCEleJoshEleSemiParamV8_sigmaEoverE, 
     energySCEleJoshEleSemiParamV8_cbmean,      energySCEleJoshEleSemiParamV8_sigma, 
     energySCEleJoshEleSemiParamV8_alpha1, energySCEleJoshEleSemiParamV8_gamma1, energySCEleJoshEleSemiParamV8_alpha2, 
     energySCEleJoshEleSemiParamV8_gamma2, energySCEleJoshEleSemiParamV8_pdfval;
   std::vector<v_t>  energySCEleJoshPhoSemiParamV8_ecorr, energySCEleJoshPhoSemiParamV8_sigmaEoverE, 
     energySCEleJoshPhoSemiParamV8_cbmean,      energySCEleJoshPhoSemiParamV8_sigma, 
     energySCEleJoshPhoSemiParamV8_alpha1, energySCEleJoshPhoSemiParamV8_gamma1, energySCEleJoshPhoSemiParamV8_alpha2, 
     energySCEleJoshPhoSemiParamV8_gamma2, energySCEleJoshPhoSemiParamV8_pdfval;


   std::vector<v_t>  energySCEleJoshEleSemiParam7TeVtrainV6_ecorr, energySCEleJoshEleSemiParam7TeVtrainV6_sigmaEoverE, 
     energySCEleJoshEleSemiParam7TeVtrainV6_cbmean,      energySCEleJoshEleSemiParam7TeVtrainV6_sigma, 
     energySCEleJoshEleSemiParam7TeVtrainV6_alpha1, energySCEleJoshEleSemiParam7TeVtrainV6_gamma1, energySCEleJoshEleSemiParam7TeVtrainV6_alpha2, 
     energySCEleJoshEleSemiParam7TeVtrainV6_gamma2, energySCEleJoshEleSemiParam7TeVtrainV6_pdfval;
   std::vector<v_t>  energySCEleJoshPhoSemiParam7TeVtrainV6_ecorr, energySCEleJoshPhoSemiParam7TeVtrainV6_sigmaEoverE, 
     energySCEleJoshPhoSemiParam7TeVtrainV6_cbmean,      energySCEleJoshPhoSemiParam7TeVtrainV6_sigma, 
     energySCEleJoshPhoSemiParam7TeVtrainV6_alpha1, energySCEleJoshPhoSemiParam7TeVtrainV6_gamma1, energySCEleJoshPhoSemiParam7TeVtrainV6_alpha2, 
     energySCEleJoshPhoSemiParam7TeVtrainV6_gamma2, energySCEleJoshPhoSemiParam7TeVtrainV6_pdfval;

   std::vector<v_t>  energySCEleJoshEleSemiParam7TeVtrainV7_ecorr, energySCEleJoshEleSemiParam7TeVtrainV7_sigmaEoverE, 
     energySCEleJoshEleSemiParam7TeVtrainV7_cbmean,      energySCEleJoshEleSemiParam7TeVtrainV7_sigma, 
     energySCEleJoshEleSemiParam7TeVtrainV7_alpha1, energySCEleJoshEleSemiParam7TeVtrainV7_gamma1, energySCEleJoshEleSemiParam7TeVtrainV7_alpha2, 
     energySCEleJoshEleSemiParam7TeVtrainV7_gamma2, energySCEleJoshEleSemiParam7TeVtrainV7_pdfval;
   std::vector<v_t>  energySCEleJoshPhoSemiParam7TeVtrainV7_ecorr, energySCEleJoshPhoSemiParam7TeVtrainV7_sigmaEoverE, 
     energySCEleJoshPhoSemiParam7TeVtrainV7_cbmean,      energySCEleJoshPhoSemiParam7TeVtrainV7_sigma, 
     energySCEleJoshPhoSemiParam7TeVtrainV7_alpha1, energySCEleJoshPhoSemiParam7TeVtrainV7_gamma1, energySCEleJoshPhoSemiParam7TeVtrainV7_alpha2, 
     energySCEleJoshPhoSemiParam7TeVtrainV7_gamma2, energySCEleJoshPhoSemiParam7TeVtrainV7_pdfval;

   std::vector<v_t>  energySCEleJoshEleSemiParam7TeVtrainV8_ecorr, energySCEleJoshEleSemiParam7TeVtrainV8_sigmaEoverE, 
     energySCEleJoshEleSemiParam7TeVtrainV8_cbmean,      energySCEleJoshEleSemiParam7TeVtrainV8_sigma, 
     energySCEleJoshEleSemiParam7TeVtrainV8_alpha1, energySCEleJoshEleSemiParam7TeVtrainV8_gamma1, energySCEleJoshEleSemiParam7TeVtrainV8_alpha2, 
     energySCEleJoshEleSemiParam7TeVtrainV8_gamma2, energySCEleJoshEleSemiParam7TeVtrainV8_pdfval;
   std::vector<v_t>  energySCEleJoshPhoSemiParam7TeVtrainV8_ecorr, energySCEleJoshPhoSemiParam7TeVtrainV8_sigmaEoverE, 
     energySCEleJoshPhoSemiParam7TeVtrainV8_cbmean,      energySCEleJoshPhoSemiParam7TeVtrainV8_sigma, 
     energySCEleJoshPhoSemiParam7TeVtrainV8_alpha1, energySCEleJoshPhoSemiParam7TeVtrainV8_gamma1, energySCEleJoshPhoSemiParam7TeVtrainV8_alpha2, 
     energySCEleJoshPhoSemiParam7TeVtrainV8_gamma2, energySCEleJoshPhoSemiParam7TeVtrainV8_pdfval;

   std::auto_ptr<NewEnergyMap> energySCEleJoshEleMap(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap> energySCEleJoshEleVarMap(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap> energySCEleJoshPhoMap(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap> energySCEleJoshPhoVarMap(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap> energyEleFraMap(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap> energyEleFraVarMap(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV4_ecorr_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV4_sigma_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV4_alpha1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV4_gamma1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV4_alpha2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV4_gamma2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV4_pdfval_Map(new NewEnergyMap());

   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV4_ecorr_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV4_sigma_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV4_alpha1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV4_gamma1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV4_alpha2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV4_gamma2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV4_pdfval_Map(new NewEnergyMap());

   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV5_ecorr_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV5_sigma_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV5_alpha1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV5_gamma1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV5_alpha2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV5_gamma2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV5_pdfval_Map(new NewEnergyMap());

   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV5_ecorr_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV5_sigma_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV5_alpha1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV5_gamma1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV5_alpha2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV5_gamma2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV5_pdfval_Map(new NewEnergyMap());


   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV6_ecorr_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV6_sigmaEoverE_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV6_cbmean_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV6_sigma_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV6_alpha1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV6_gamma1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV6_alpha2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV6_gamma2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV6_pdfval_Map(new NewEnergyMap());

   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV6_ecorr_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV6_sigmaEoverE_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV6_cbmean_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV6_sigma_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV6_alpha1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV6_gamma1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV6_alpha2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV6_gamma2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV6_pdfval_Map(new NewEnergyMap());
   //------------------------------
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV7_ecorr_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV7_sigmaEoverE_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV7_cbmean_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV7_sigma_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV7_alpha1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV7_gamma1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV7_alpha2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV7_gamma2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV7_pdfval_Map(new NewEnergyMap());

   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV7_ecorr_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV7_sigmaEoverE_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV7_cbmean_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV7_sigma_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV7_alpha1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV7_gamma1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV7_alpha2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV7_gamma2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV7_pdfval_Map(new NewEnergyMap());
   //------------------------------
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV8_ecorr_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV8_sigmaEoverE_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV8_cbmean_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV8_sigma_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV8_alpha1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV8_gamma1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV8_alpha2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV8_gamma2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParamV8_pdfval_Map(new NewEnergyMap());

   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV8_ecorr_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV8_sigmaEoverE_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV8_cbmean_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV8_sigma_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV8_alpha1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV8_gamma1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV8_alpha2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV8_gamma2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParamV8_pdfval_Map(new NewEnergyMap());

   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV6_ecorr_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV6_sigmaEoverE_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV6_cbmean_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV6_sigma_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV6_alpha1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV6_gamma1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV6_alpha2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV6_gamma2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV6_pdfval_Map(new NewEnergyMap());

   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV6_ecorr_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV6_sigmaEoverE_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV6_cbmean_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV6_sigma_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV6_alpha1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV6_gamma1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV6_alpha2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV6_gamma2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV6_pdfval_Map(new NewEnergyMap());
   //------------------------------
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV7_ecorr_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV7_sigmaEoverE_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV7_cbmean_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV7_sigma_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV7_alpha1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV7_gamma1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV7_alpha2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV7_gamma2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV7_pdfval_Map(new NewEnergyMap());

   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV7_ecorr_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV7_sigmaEoverE_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV7_cbmean_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV7_sigma_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV7_alpha1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV7_gamma1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV7_alpha2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV7_gamma2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV7_pdfval_Map(new NewEnergyMap());
   //------------------------------
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV8_ecorr_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV8_sigmaEoverE_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV8_cbmean_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV8_sigma_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV8_alpha1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV8_gamma1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV8_alpha2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV8_gamma2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshEleSemiParam7TeVtrainV8_pdfval_Map(new NewEnergyMap());

   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV8_ecorr_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV8_sigmaEoverE_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV8_cbmean_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV8_sigma_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV8_alpha1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV8_gamma1_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV8_alpha2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV8_gamma2_Map(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap>  energySCEleJoshPhoSemiParam7TeVtrainV8_pdfval_Map(new NewEnergyMap());

   //------------------------------ ELECTRON
   iEvent.getByLabel(electronsTAG, electronsHandle);
   // altrimenti non tira eccezione
   //if(!electronsHandle.isValid()){
   //  std::cerr << "[ERROR] electron collection not found" << std::endl;
   //  return;
   //}
   
   //------------------------------  VERTEX
   iEvent.getByLabel(vtxCollectionTAG, primaryVertexHandle); 
   // for conversions with full vertex fit
   iEvent.getByLabel(BeamSpotTAG, bsHandle);
   //  bs = bsHandle.product();
   
   //------------------------------ RHO
   iEvent.getByLabel(rhoTAG,rhoHandle);
   
   //------------------------------ RECHIT
   //   iEvent.getByLabel(recHitCollectionEBTAG, recHitCollectionEBHandle);
   //   iEvent.getByLabel(recHitCollectionEETAG, recHitCollectionEEHandle);
   
   //------------------------------ CONVERSIONS
   iEvent.getByLabel(conversionsProducerTAG, conversionsHandle);
   
   
   iSetup.get<CaloTopologyRecord>().get(topologyHandle);
   //  if (! pTopology.isValid() ) return;
   //  topology = pTopology.product();
   


  EcalClusterLazyTools lazyTools(iEvent, iSetup, 
				 recHitCollectionEBTAG, 
                                 recHitCollectionEETAG);  




  for(reco::GsfElectronCollection::const_iterator ele_itr = (electronsHandle)->begin(); 
      ele_itr != (electronsHandle)->end(); ele_itr++){
    // the new tree has one event per each electron
    // reject trackerDriven only electrons because I haven't saved the PFClusters                                                                                                 
    //    if(ele_itr->trackerDrivenSeed() && !ele_itr->ecalDrivenSeed()) continue;
  
    //energyScaleSmearer scaler;
    //float scaledEnergy = scaler.scale(*ele_itr);
    //float smearedEnergy = scaler.smear(*ele_itr);

    std::pair<double,double> corEle,corPho,corEle_fra;

#if REGRESSION == 4
#elif REGRESSION == 3
    corEle = josh_Ele.CorrectedEnergyWithErrorV3(*ele_itr,*primaryVertexHandle, *rhoHandle, lazyTools,iSetup);
    corPho = josh_Pho.CorrectedEnergyWithErrorV3(*ele_itr,*primaryVertexHandle, *rhoHandle, lazyTools,iSetup);
#elif REGRESSION == 2  
  corEle = josh_Ele.CorrectedEnergyWithErrorV2(*ele_itr,*primaryVertexHandle,  lazyTools,iSetup);
  corPho = josh_Pho.CorrectedEnergyWithErrorV2(*ele_itr,*primaryVertexHandle,  lazyTools,iSetup);
#elif REGRESSION == 1
  corEle = josh_Ele.CorrectedEnergyWithError(*ele_itr,*primaryVertexHandle,  lazyTools,iSetup);
  corPho = josh_Pho.CorrectedEnergyWithError(*ele_itr,*primaryVertexHandle,  lazyTools,iSetup);
#endif

#if REGRESSION == 4
#else
  corEle_fra = fra_Ele.CorrectedEnergyWithErrorTracker(*ele_itr,*primaryVertexHandle, *rhoHandle, lazyTools,iSetup,ptSplit);
#endif


  double ecor, sigmaEoverE, cbmean, sigma, alpha1, n1, alpha2, n2, pdfval;

    
  corV4_ele.CorrectedEnergyWithErrorV4(*ele_itr, *primaryVertexHandle, *rhoHandle, lazyTools, iSetup,ecor, sigma, alpha1, n1, alpha2, n2, pdfval);
  energySCEleJoshEleSemiParamV4_ecorr.push_back(ecor);
  energySCEleJoshEleSemiParamV4_sigma.push_back(sigma);
  energySCEleJoshEleSemiParamV4_alpha1.push_back(alpha1);
  energySCEleJoshEleSemiParamV4_gamma1.push_back(n1);
  energySCEleJoshEleSemiParamV4_alpha2.push_back(alpha2);
  energySCEleJoshEleSemiParamV4_gamma2.push_back(n2);
  energySCEleJoshEleSemiParamV4_pdfval.push_back(pdfval);
  
#if DEBUG == 2
    printf("V4: sceta = %5f, default = %5f, corrected = %5f, sigma = %5f, alpha1 = %5f, n1 = %5f, alpha2 = %5f, n2 = %5f, pdfval = %5f\n", it->superCluster()->eta(), it->energy(),ecor,sigma,alpha1,n1,alpha2,n2,pdfval);
#endif

    corV5_ele.CorrectedEnergyWithErrorV5(*ele_itr, *primaryVertexHandle, *rhoHandle, lazyTools, iSetup,ecor, sigma, alpha1, n1, alpha2, n2, pdfval);
    energySCEleJoshEleSemiParamV5_ecorr.push_back(ecor);
    energySCEleJoshEleSemiParamV5_sigma.push_back(sigma);
    energySCEleJoshEleSemiParamV5_alpha1.push_back(alpha1);
    energySCEleJoshEleSemiParamV5_gamma1.push_back(n1);
    energySCEleJoshEleSemiParamV5_alpha2.push_back(alpha2);
    energySCEleJoshEleSemiParamV5_gamma2.push_back(n2);
    energySCEleJoshEleSemiParamV5_pdfval.push_back(pdfval);

#if DEBUG == 2
    printf("V5: sceta = %5f, default = %5f, corrected = %5f, sigma = %5f, alpha1 = %5f, n1 = %5f, alpha2 = %5f, n2 = %5f, pdfval = %5f\n", it->superCluster()->eta(), it->energy(),ecor,sigma,alpha1,n1,alpha2,n2,pdfval);
#endif



    corV4_pho.CorrectedEnergyWithErrorV4(*ele_itr, *primaryVertexHandle, *rhoHandle, lazyTools, iSetup,ecor, sigma, alpha1, n1, alpha2, n2, pdfval);
    energySCEleJoshPhoSemiParamV4_ecorr.push_back(ecor);
    energySCEleJoshPhoSemiParamV4_sigma.push_back(sigma);
    energySCEleJoshPhoSemiParamV4_alpha1.push_back(alpha1);
    energySCEleJoshPhoSemiParamV4_gamma1.push_back(n1);
    energySCEleJoshPhoSemiParamV4_alpha2.push_back(alpha2);
    energySCEleJoshPhoSemiParamV4_gamma2.push_back(n2);
    energySCEleJoshPhoSemiParamV4_pdfval.push_back(pdfval);

#if DEBUG == 2
    printf("V4: sceta = %5f, default = %5f, corrected = %5f, sigma = %5f, alpha1 = %5f, n1 = %5f, alpha2 = %5f, n2 = %5f, pdfval = %5f\n", it->superCluster()->eta(), it->energy(),ecor,sigma,alpha1,n1,alpha2,n2,pdfval);
#endif

    corV5_pho.CorrectedEnergyWithErrorV5(*ele_itr, *primaryVertexHandle, *rhoHandle, lazyTools, iSetup,ecor, sigma, alpha1, n1, alpha2, n2, pdfval);
    energySCEleJoshPhoSemiParamV5_ecorr.push_back(ecor);
    energySCEleJoshPhoSemiParamV5_sigma.push_back(sigma);
    energySCEleJoshPhoSemiParamV5_alpha1.push_back(alpha1);
    energySCEleJoshPhoSemiParamV5_gamma1.push_back(n1);
    energySCEleJoshPhoSemiParamV5_alpha2.push_back(alpha2);
    energySCEleJoshPhoSemiParamV5_gamma2.push_back(n2);
    energySCEleJoshPhoSemiParamV5_pdfval.push_back(pdfval);

#if DEBUG == 2
    printf("V5: sceta = %5f, default = %5f, corrected = %5f, sigma = %5f, alpha1 = %5f, n1 = %5f, alpha2 = %5f, n2 = %5f, pdfval = %5f\n", it->superCluster()->eta(), it->energy(),ecor,sigma,alpha1,n1,alpha2,n2,pdfval);
#endif



    //------------------------------
    corV6_ele.CorrectedEnergyWithErrorV6(*ele_itr, *primaryVertexHandle, *rhoHandle, lazyTools, iSetup,ecor, sigmaEoverE, cbmean, sigma, alpha1, n1, alpha2, n2, pdfval);
    energySCEleJoshEleSemiParamV6_ecorr.push_back(ecor);
    energySCEleJoshEleSemiParamV6_sigmaEoverE.push_back(sigmaEoverE);
    energySCEleJoshEleSemiParamV6_cbmean.push_back(cbmean);
    energySCEleJoshEleSemiParamV6_sigma.push_back(sigma);
    energySCEleJoshEleSemiParamV6_alpha1.push_back(alpha1);
    energySCEleJoshEleSemiParamV6_gamma1.push_back(n1);
    energySCEleJoshEleSemiParamV6_alpha2.push_back(alpha2);
    energySCEleJoshEleSemiParamV6_gamma2.push_back(n2);
    energySCEleJoshEleSemiParamV6_pdfval.push_back(pdfval);

    corV6_pho.CorrectedEnergyWithErrorV6(*ele_itr, *primaryVertexHandle, *rhoHandle, lazyTools, iSetup,ecor, sigmaEoverE, cbmean, sigma, alpha1, n1, alpha2, n2, pdfval);
    energySCEleJoshPhoSemiParamV6_ecorr.push_back(ecor);
    energySCEleJoshPhoSemiParamV6_sigmaEoverE.push_back(sigmaEoverE);
    energySCEleJoshPhoSemiParamV6_cbmean.push_back(cbmean);
    energySCEleJoshPhoSemiParamV6_sigma.push_back(sigma);
    energySCEleJoshPhoSemiParamV6_alpha1.push_back(alpha1);
    energySCEleJoshPhoSemiParamV6_gamma1.push_back(n1);
    energySCEleJoshPhoSemiParamV6_alpha2.push_back(alpha2);
    energySCEleJoshPhoSemiParamV6_gamma2.push_back(n2);
    energySCEleJoshPhoSemiParamV6_pdfval.push_back(pdfval);
    //------------------------------
    corV7_ele.CorrectedEnergyWithErrorV7(*ele_itr, *primaryVertexHandle, *rhoHandle, lazyTools, iSetup,ecor, sigmaEoverE, cbmean, sigma, alpha1, n1, alpha2, n2, pdfval);
    energySCEleJoshEleSemiParamV7_ecorr.push_back(ecor);
    energySCEleJoshEleSemiParamV7_sigmaEoverE.push_back(sigmaEoverE);
    energySCEleJoshEleSemiParamV7_cbmean.push_back(cbmean);
    energySCEleJoshEleSemiParamV7_sigma.push_back(sigma);
    energySCEleJoshEleSemiParamV7_alpha1.push_back(alpha1);
    energySCEleJoshEleSemiParamV7_gamma1.push_back(n1);
    energySCEleJoshEleSemiParamV7_alpha2.push_back(alpha2);
    energySCEleJoshEleSemiParamV7_gamma2.push_back(n2);
    energySCEleJoshEleSemiParamV7_pdfval.push_back(pdfval);

    corV7_pho.CorrectedEnergyWithErrorV7(*ele_itr, *primaryVertexHandle, *rhoHandle, lazyTools, iSetup,ecor, sigmaEoverE, cbmean, sigma, alpha1, n1, alpha2, n2, pdfval);
    energySCEleJoshPhoSemiParamV7_ecorr.push_back(ecor);
    energySCEleJoshPhoSemiParamV7_sigmaEoverE.push_back(sigmaEoverE);
    energySCEleJoshPhoSemiParamV7_cbmean.push_back(cbmean);
    energySCEleJoshPhoSemiParamV7_sigma.push_back(sigma);
    energySCEleJoshPhoSemiParamV7_alpha1.push_back(alpha1);
    energySCEleJoshPhoSemiParamV7_gamma1.push_back(n1);
    energySCEleJoshPhoSemiParamV7_alpha2.push_back(alpha2);
    energySCEleJoshPhoSemiParamV7_gamma2.push_back(n2);
    energySCEleJoshPhoSemiParamV7_pdfval.push_back(pdfval);
    //------------------------------
    corV8_ele.CorrectedEnergyWithErrorV8(*ele_itr, *primaryVertexHandle, *rhoHandle, lazyTools, iSetup,ecor, sigmaEoverE, cbmean, sigma, alpha1, n1, alpha2, n2, pdfval);
    energySCEleJoshEleSemiParamV8_ecorr.push_back(ecor);
    energySCEleJoshEleSemiParamV8_sigmaEoverE.push_back(sigmaEoverE);
    energySCEleJoshEleSemiParamV8_cbmean.push_back(cbmean);
    energySCEleJoshEleSemiParamV8_sigma.push_back(sigma);
    energySCEleJoshEleSemiParamV8_alpha1.push_back(alpha1);
    energySCEleJoshEleSemiParamV8_gamma1.push_back(n1);
    energySCEleJoshEleSemiParamV8_alpha2.push_back(alpha2);
    energySCEleJoshEleSemiParamV8_gamma2.push_back(n2);
    energySCEleJoshEleSemiParamV8_pdfval.push_back(pdfval);

    corV8_pho.CorrectedEnergyWithErrorV8(*ele_itr, *primaryVertexHandle, *rhoHandle, lazyTools, iSetup,ecor, sigmaEoverE, cbmean, sigma, alpha1, n1, alpha2, n2, pdfval);
    energySCEleJoshPhoSemiParamV8_ecorr.push_back(ecor);
    energySCEleJoshPhoSemiParamV8_sigmaEoverE.push_back(sigmaEoverE);
    energySCEleJoshPhoSemiParamV8_cbmean.push_back(cbmean);
    energySCEleJoshPhoSemiParamV8_sigma.push_back(sigma);
    energySCEleJoshPhoSemiParamV8_alpha1.push_back(alpha1);
    energySCEleJoshPhoSemiParamV8_gamma1.push_back(n1);
    energySCEleJoshPhoSemiParamV8_alpha2.push_back(alpha2);
    energySCEleJoshPhoSemiParamV8_gamma2.push_back(n2);
    energySCEleJoshPhoSemiParamV8_pdfval.push_back(pdfval);



    //------------------------------
    cor7TeVtrainV6_ele.CorrectedEnergyWithErrorV6(*ele_itr, *primaryVertexHandle, *rhoHandle, lazyTools, iSetup,ecor, sigmaEoverE, cbmean, sigma, alpha1, n1, alpha2, n2, pdfval);
    energySCEleJoshEleSemiParam7TeVtrainV6_ecorr.push_back(ecor);
    energySCEleJoshEleSemiParam7TeVtrainV6_sigmaEoverE.push_back(sigmaEoverE);
    energySCEleJoshEleSemiParam7TeVtrainV6_cbmean.push_back(cbmean);
    energySCEleJoshEleSemiParam7TeVtrainV6_sigma.push_back(sigma);
    energySCEleJoshEleSemiParam7TeVtrainV6_alpha1.push_back(alpha1);
    energySCEleJoshEleSemiParam7TeVtrainV6_gamma1.push_back(n1);
    energySCEleJoshEleSemiParam7TeVtrainV6_alpha2.push_back(alpha2);
    energySCEleJoshEleSemiParam7TeVtrainV6_gamma2.push_back(n2);
    energySCEleJoshEleSemiParam7TeVtrainV6_pdfval.push_back(pdfval);

    cor7TeVtrainV6_pho.CorrectedEnergyWithErrorV6(*ele_itr, *primaryVertexHandle, *rhoHandle, lazyTools, iSetup,ecor, sigmaEoverE, cbmean, sigma, alpha1, n1, alpha2, n2, pdfval);
    energySCEleJoshPhoSemiParam7TeVtrainV6_ecorr.push_back(ecor);
    energySCEleJoshPhoSemiParam7TeVtrainV6_sigmaEoverE.push_back(sigmaEoverE);
    energySCEleJoshPhoSemiParam7TeVtrainV6_cbmean.push_back(cbmean);
    energySCEleJoshPhoSemiParam7TeVtrainV6_sigma.push_back(sigma);
    energySCEleJoshPhoSemiParam7TeVtrainV6_alpha1.push_back(alpha1);
    energySCEleJoshPhoSemiParam7TeVtrainV6_gamma1.push_back(n1);
    energySCEleJoshPhoSemiParam7TeVtrainV6_alpha2.push_back(alpha2);
    energySCEleJoshPhoSemiParam7TeVtrainV6_gamma2.push_back(n2);
    energySCEleJoshPhoSemiParam7TeVtrainV6_pdfval.push_back(pdfval);
    //------------------------------
    cor7TeVtrainV7_ele.CorrectedEnergyWithErrorV7(*ele_itr, *primaryVertexHandle, *rhoHandle, lazyTools, iSetup,ecor, sigmaEoverE, cbmean, sigma, alpha1, n1, alpha2, n2, pdfval);
    energySCEleJoshEleSemiParam7TeVtrainV7_ecorr.push_back(ecor);
    energySCEleJoshEleSemiParam7TeVtrainV7_sigmaEoverE.push_back(sigmaEoverE);
    energySCEleJoshEleSemiParam7TeVtrainV7_cbmean.push_back(cbmean);
    energySCEleJoshEleSemiParam7TeVtrainV7_sigma.push_back(sigma);
    energySCEleJoshEleSemiParam7TeVtrainV7_alpha1.push_back(alpha1);
    energySCEleJoshEleSemiParam7TeVtrainV7_gamma1.push_back(n1);
    energySCEleJoshEleSemiParam7TeVtrainV7_alpha2.push_back(alpha2);
    energySCEleJoshEleSemiParam7TeVtrainV7_gamma2.push_back(n2);
    energySCEleJoshEleSemiParam7TeVtrainV7_pdfval.push_back(pdfval);

    cor7TeVtrainV7_pho.CorrectedEnergyWithErrorV7(*ele_itr, *primaryVertexHandle, *rhoHandle, lazyTools, iSetup,ecor, sigmaEoverE, cbmean, sigma, alpha1, n1, alpha2, n2, pdfval);
    energySCEleJoshPhoSemiParam7TeVtrainV7_ecorr.push_back(ecor);
    energySCEleJoshPhoSemiParam7TeVtrainV7_sigmaEoverE.push_back(sigmaEoverE);
    energySCEleJoshPhoSemiParam7TeVtrainV7_cbmean.push_back(cbmean);
    energySCEleJoshPhoSemiParam7TeVtrainV7_sigma.push_back(sigma);
    energySCEleJoshPhoSemiParam7TeVtrainV7_alpha1.push_back(alpha1);
    energySCEleJoshPhoSemiParam7TeVtrainV7_gamma1.push_back(n1);
    energySCEleJoshPhoSemiParam7TeVtrainV7_alpha2.push_back(alpha2);
    energySCEleJoshPhoSemiParam7TeVtrainV7_gamma2.push_back(n2);
    energySCEleJoshPhoSemiParam7TeVtrainV7_pdfval.push_back(pdfval);
    //------------------------------
    cor7TeVtrainV8_ele.CorrectedEnergyWithErrorV8(*ele_itr, *primaryVertexHandle, *rhoHandle, lazyTools, iSetup,ecor, sigmaEoverE, cbmean, sigma, alpha1, n1, alpha2, n2, pdfval);
    energySCEleJoshEleSemiParam7TeVtrainV8_ecorr.push_back(ecor);
    energySCEleJoshEleSemiParam7TeVtrainV8_sigmaEoverE.push_back(sigmaEoverE);
    energySCEleJoshEleSemiParam7TeVtrainV8_cbmean.push_back(cbmean);
    energySCEleJoshEleSemiParam7TeVtrainV8_sigma.push_back(sigma);
    energySCEleJoshEleSemiParam7TeVtrainV8_alpha1.push_back(alpha1);
    energySCEleJoshEleSemiParam7TeVtrainV8_gamma1.push_back(n1);
    energySCEleJoshEleSemiParam7TeVtrainV8_alpha2.push_back(alpha2);
    energySCEleJoshEleSemiParam7TeVtrainV8_gamma2.push_back(n2);
    energySCEleJoshEleSemiParam7TeVtrainV8_pdfval.push_back(pdfval);

    cor7TeVtrainV8_pho.CorrectedEnergyWithErrorV8(*ele_itr, *primaryVertexHandle, *rhoHandle, lazyTools, iSetup,ecor, sigmaEoverE, cbmean, sigma, alpha1, n1, alpha2, n2, pdfval);
    energySCEleJoshPhoSemiParam7TeVtrainV8_ecorr.push_back(ecor);
    energySCEleJoshPhoSemiParam7TeVtrainV8_sigmaEoverE.push_back(sigmaEoverE);
    energySCEleJoshPhoSemiParam7TeVtrainV8_cbmean.push_back(cbmean);
    energySCEleJoshPhoSemiParam7TeVtrainV8_sigma.push_back(sigma);
    energySCEleJoshPhoSemiParam7TeVtrainV8_alpha1.push_back(alpha1);
    energySCEleJoshPhoSemiParam7TeVtrainV8_gamma1.push_back(n1);
    energySCEleJoshPhoSemiParam7TeVtrainV8_alpha2.push_back(alpha2);
    energySCEleJoshPhoSemiParam7TeVtrainV8_gamma2.push_back(n2);
    energySCEleJoshPhoSemiParam7TeVtrainV8_pdfval.push_back(pdfval);


  //  std::cout<<corEle_fra.second<<std::endl;

  //fill the vector with the energies
  energySCEleJoshEle.push_back(corEle.first);
  energySCEleJoshEleVar.push_back(corEle.second);
  energySCEleJoshPho.push_back(corPho.first);
  energySCEleJoshPhoVar.push_back(corPho.second);
  energyEleFra.push_back(corEle_fra.first);
  energyEleFraVar.push_back(corEle_fra.second);
  }

  //prepare product 
  // declare the filler of the ValueMap
  NewEnergyMap::Filler energySCEleJoshEle_filler(*energySCEleJoshEleMap);
  NewEnergyMap::Filler energySCEleJoshEleVar_filler(*energySCEleJoshEleVarMap);//fra
  NewEnergyMap::Filler energySCEleJoshPho_filler(*energySCEleJoshPhoMap);
  NewEnergyMap::Filler energySCEleJoshPhoVar_filler(*energySCEleJoshPhoVarMap);
  NewEnergyMap::Filler energyEleFra_filler(*energyEleFraMap);
  NewEnergyMap::Filler energyEleFraVar_filler(*energyEleFraVarMap);//fra

  NewEnergyMap::Filler energySCEleJoshEleSemiParamV4_ecorr_filler(*energySCEleJoshEleSemiParamV4_ecorr_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV4_sigma_filler(*energySCEleJoshEleSemiParamV4_sigma_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV4_alpha1_filler(*energySCEleJoshEleSemiParamV4_alpha1_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV4_gamma1_filler(*energySCEleJoshEleSemiParamV4_gamma1_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV4_alpha2_filler(*energySCEleJoshEleSemiParamV4_alpha2_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV4_gamma2_filler(*energySCEleJoshEleSemiParamV4_gamma2_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV4_pdfval_filler(*energySCEleJoshEleSemiParamV4_pdfval_Map);
  
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV5_ecorr_filler(*energySCEleJoshEleSemiParamV5_ecorr_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV5_sigma_filler(*energySCEleJoshEleSemiParamV5_sigma_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV5_alpha1_filler(*energySCEleJoshEleSemiParamV5_alpha1_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV5_gamma1_filler(*energySCEleJoshEleSemiParamV5_gamma1_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV5_alpha2_filler(*energySCEleJoshEleSemiParamV5_alpha2_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV5_gamma2_filler(*energySCEleJoshEleSemiParamV5_gamma2_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV5_pdfval_filler(*energySCEleJoshEleSemiParamV5_pdfval_Map);

  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV4_ecorr_filler (*energySCEleJoshPhoSemiParamV4_ecorr_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV4_sigma_filler (*energySCEleJoshPhoSemiParamV4_sigma_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV4_alpha1_filler(*energySCEleJoshPhoSemiParamV4_alpha1_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV4_gamma1_filler(*energySCEleJoshPhoSemiParamV4_gamma1_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV4_alpha2_filler(*energySCEleJoshPhoSemiParamV4_alpha2_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV4_gamma2_filler(*energySCEleJoshPhoSemiParamV4_gamma2_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV4_pdfval_filler(*energySCEleJoshPhoSemiParamV4_pdfval_Map);
  				      						   
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV5_ecorr_filler (*energySCEleJoshPhoSemiParamV5_ecorr_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV5_sigma_filler (*energySCEleJoshPhoSemiParamV5_sigma_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV5_alpha1_filler(*energySCEleJoshPhoSemiParamV5_alpha1_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV5_gamma1_filler(*energySCEleJoshPhoSemiParamV5_gamma1_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV5_alpha2_filler(*energySCEleJoshPhoSemiParamV5_alpha2_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV5_gamma2_filler(*energySCEleJoshPhoSemiParamV5_gamma2_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV5_pdfval_filler(*energySCEleJoshPhoSemiParamV5_pdfval_Map);

  //------------------------------
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV6_ecorr_filler(*energySCEleJoshEleSemiParamV6_ecorr_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV6_sigmaEoverE_filler(*energySCEleJoshEleSemiParamV6_sigmaEoverE_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV6_cbmean_filler(*energySCEleJoshEleSemiParamV6_cbmean_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV6_sigma_filler(*energySCEleJoshEleSemiParamV6_sigma_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV6_alpha1_filler(*energySCEleJoshEleSemiParamV6_alpha1_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV6_gamma1_filler(*energySCEleJoshEleSemiParamV6_gamma1_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV6_alpha2_filler(*energySCEleJoshEleSemiParamV6_alpha2_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV6_gamma2_filler(*energySCEleJoshEleSemiParamV6_gamma2_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV6_pdfval_filler(*energySCEleJoshEleSemiParamV6_pdfval_Map);

  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV6_ecorr_filler (*energySCEleJoshPhoSemiParamV6_ecorr_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV6_sigmaEoverE_filler (*energySCEleJoshPhoSemiParamV6_sigmaEoverE_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV6_cbmean_filler (*energySCEleJoshPhoSemiParamV6_cbmean_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV6_sigma_filler (*energySCEleJoshPhoSemiParamV6_sigma_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV6_alpha1_filler(*energySCEleJoshPhoSemiParamV6_alpha1_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV6_gamma1_filler(*energySCEleJoshPhoSemiParamV6_gamma1_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV6_alpha2_filler(*energySCEleJoshPhoSemiParamV6_alpha2_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV6_gamma2_filler(*energySCEleJoshPhoSemiParamV6_gamma2_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV6_pdfval_filler(*energySCEleJoshPhoSemiParamV6_pdfval_Map);
  				      						   
  //------------------------------
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV7_ecorr_filler(*energySCEleJoshEleSemiParamV7_ecorr_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV7_sigmaEoverE_filler(*energySCEleJoshEleSemiParamV7_sigmaEoverE_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV7_cbmean_filler(*energySCEleJoshEleSemiParamV7_cbmean_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV7_sigma_filler(*energySCEleJoshEleSemiParamV7_sigma_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV7_alpha1_filler(*energySCEleJoshEleSemiParamV7_alpha1_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV7_gamma1_filler(*energySCEleJoshEleSemiParamV7_gamma1_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV7_alpha2_filler(*energySCEleJoshEleSemiParamV7_alpha2_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV7_gamma2_filler(*energySCEleJoshEleSemiParamV7_gamma2_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV7_pdfval_filler(*energySCEleJoshEleSemiParamV7_pdfval_Map);

  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV7_ecorr_filler (*energySCEleJoshPhoSemiParamV7_ecorr_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV7_sigmaEoverE_filler (*energySCEleJoshPhoSemiParamV7_sigmaEoverE_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV7_cbmean_filler (*energySCEleJoshPhoSemiParamV7_cbmean_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV7_sigma_filler (*energySCEleJoshPhoSemiParamV7_sigma_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV7_alpha1_filler(*energySCEleJoshPhoSemiParamV7_alpha1_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV7_gamma1_filler(*energySCEleJoshPhoSemiParamV7_gamma1_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV7_alpha2_filler(*energySCEleJoshPhoSemiParamV7_alpha2_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV7_gamma2_filler(*energySCEleJoshPhoSemiParamV7_gamma2_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV7_pdfval_filler(*energySCEleJoshPhoSemiParamV7_pdfval_Map);
  				      						   
  //------------------------------
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV8_ecorr_filler(*energySCEleJoshEleSemiParamV8_ecorr_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV8_sigmaEoverE_filler(*energySCEleJoshEleSemiParamV8_sigmaEoverE_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV8_cbmean_filler(*energySCEleJoshEleSemiParamV8_cbmean_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV8_sigma_filler(*energySCEleJoshEleSemiParamV8_sigma_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV8_alpha1_filler(*energySCEleJoshEleSemiParamV8_alpha1_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV8_gamma1_filler(*energySCEleJoshEleSemiParamV8_gamma1_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV8_alpha2_filler(*energySCEleJoshEleSemiParamV8_alpha2_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV8_gamma2_filler(*energySCEleJoshEleSemiParamV8_gamma2_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParamV8_pdfval_filler(*energySCEleJoshEleSemiParamV8_pdfval_Map);

  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV8_ecorr_filler (*energySCEleJoshPhoSemiParamV8_ecorr_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV8_sigmaEoverE_filler (*energySCEleJoshPhoSemiParamV8_sigmaEoverE_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV8_cbmean_filler (*energySCEleJoshPhoSemiParamV8_cbmean_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV8_sigma_filler (*energySCEleJoshPhoSemiParamV8_sigma_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV8_alpha1_filler(*energySCEleJoshPhoSemiParamV8_alpha1_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV8_gamma1_filler(*energySCEleJoshPhoSemiParamV8_gamma1_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV8_alpha2_filler(*energySCEleJoshPhoSemiParamV8_alpha2_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV8_gamma2_filler(*energySCEleJoshPhoSemiParamV8_gamma2_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParamV8_pdfval_filler(*energySCEleJoshPhoSemiParamV8_pdfval_Map);
  				      						   
  //------------------------------
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV6_ecorr_filler(*energySCEleJoshEleSemiParam7TeVtrainV6_ecorr_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV6_sigmaEoverE_filler(*energySCEleJoshEleSemiParam7TeVtrainV6_sigmaEoverE_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV6_cbmean_filler(*energySCEleJoshEleSemiParam7TeVtrainV6_cbmean_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV6_sigma_filler(*energySCEleJoshEleSemiParam7TeVtrainV6_sigma_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV6_alpha1_filler(*energySCEleJoshEleSemiParam7TeVtrainV6_alpha1_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV6_gamma1_filler(*energySCEleJoshEleSemiParam7TeVtrainV6_gamma1_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV6_alpha2_filler(*energySCEleJoshEleSemiParam7TeVtrainV6_alpha2_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV6_gamma2_filler(*energySCEleJoshEleSemiParam7TeVtrainV6_gamma2_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV6_pdfval_filler(*energySCEleJoshEleSemiParam7TeVtrainV6_pdfval_Map);

  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV6_ecorr_filler (*energySCEleJoshPhoSemiParam7TeVtrainV6_ecorr_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV6_sigmaEoverE_filler (*energySCEleJoshPhoSemiParam7TeVtrainV6_sigmaEoverE_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV6_cbmean_filler (*energySCEleJoshPhoSemiParam7TeVtrainV6_cbmean_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV6_sigma_filler (*energySCEleJoshPhoSemiParam7TeVtrainV6_sigma_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV6_alpha1_filler(*energySCEleJoshPhoSemiParam7TeVtrainV6_alpha1_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV6_gamma1_filler(*energySCEleJoshPhoSemiParam7TeVtrainV6_gamma1_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV6_alpha2_filler(*energySCEleJoshPhoSemiParam7TeVtrainV6_alpha2_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV6_gamma2_filler(*energySCEleJoshPhoSemiParam7TeVtrainV6_gamma2_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV6_pdfval_filler(*energySCEleJoshPhoSemiParam7TeVtrainV6_pdfval_Map);
  				      						   
  //------------------------------
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV7_ecorr_filler(*energySCEleJoshEleSemiParam7TeVtrainV7_ecorr_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV7_sigmaEoverE_filler(*energySCEleJoshEleSemiParam7TeVtrainV7_sigmaEoverE_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV7_cbmean_filler(*energySCEleJoshEleSemiParam7TeVtrainV7_cbmean_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV7_sigma_filler(*energySCEleJoshEleSemiParam7TeVtrainV7_sigma_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV7_alpha1_filler(*energySCEleJoshEleSemiParam7TeVtrainV7_alpha1_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV7_gamma1_filler(*energySCEleJoshEleSemiParam7TeVtrainV7_gamma1_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV7_alpha2_filler(*energySCEleJoshEleSemiParam7TeVtrainV7_alpha2_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV7_gamma2_filler(*energySCEleJoshEleSemiParam7TeVtrainV7_gamma2_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV7_pdfval_filler(*energySCEleJoshEleSemiParam7TeVtrainV7_pdfval_Map);

  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV7_ecorr_filler (*energySCEleJoshPhoSemiParam7TeVtrainV7_ecorr_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV7_sigmaEoverE_filler (*energySCEleJoshPhoSemiParam7TeVtrainV7_sigmaEoverE_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV7_cbmean_filler (*energySCEleJoshPhoSemiParam7TeVtrainV7_cbmean_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV7_sigma_filler (*energySCEleJoshPhoSemiParam7TeVtrainV7_sigma_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV7_alpha1_filler(*energySCEleJoshPhoSemiParam7TeVtrainV7_alpha1_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV7_gamma1_filler(*energySCEleJoshPhoSemiParam7TeVtrainV7_gamma1_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV7_alpha2_filler(*energySCEleJoshPhoSemiParam7TeVtrainV7_alpha2_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV7_gamma2_filler(*energySCEleJoshPhoSemiParam7TeVtrainV7_gamma2_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV7_pdfval_filler(*energySCEleJoshPhoSemiParam7TeVtrainV7_pdfval_Map);
  				      						   
  //------------------------------
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV8_ecorr_filler(*energySCEleJoshEleSemiParam7TeVtrainV8_ecorr_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV8_sigmaEoverE_filler(*energySCEleJoshEleSemiParam7TeVtrainV8_sigmaEoverE_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV8_cbmean_filler(*energySCEleJoshEleSemiParam7TeVtrainV8_cbmean_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV8_sigma_filler(*energySCEleJoshEleSemiParam7TeVtrainV8_sigma_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV8_alpha1_filler(*energySCEleJoshEleSemiParam7TeVtrainV8_alpha1_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV8_gamma1_filler(*energySCEleJoshEleSemiParam7TeVtrainV8_gamma1_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV8_alpha2_filler(*energySCEleJoshEleSemiParam7TeVtrainV8_alpha2_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV8_gamma2_filler(*energySCEleJoshEleSemiParam7TeVtrainV8_gamma2_Map);
  NewEnergyMap::Filler energySCEleJoshEleSemiParam7TeVtrainV8_pdfval_filler(*energySCEleJoshEleSemiParam7TeVtrainV8_pdfval_Map);

  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV8_ecorr_filler (*energySCEleJoshPhoSemiParam7TeVtrainV8_ecorr_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV8_sigmaEoverE_filler (*energySCEleJoshPhoSemiParam7TeVtrainV8_sigmaEoverE_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV8_cbmean_filler (*energySCEleJoshPhoSemiParam7TeVtrainV8_cbmean_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV8_sigma_filler (*energySCEleJoshPhoSemiParam7TeVtrainV8_sigma_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV8_alpha1_filler(*energySCEleJoshPhoSemiParam7TeVtrainV8_alpha1_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV8_gamma1_filler(*energySCEleJoshPhoSemiParam7TeVtrainV8_gamma1_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV8_alpha2_filler(*energySCEleJoshPhoSemiParam7TeVtrainV8_alpha2_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV8_gamma2_filler(*energySCEleJoshPhoSemiParam7TeVtrainV8_gamma2_Map);
  NewEnergyMap::Filler energySCEleJoshPhoSemiParam7TeVtrainV8_pdfval_filler(*energySCEleJoshPhoSemiParam7TeVtrainV8_pdfval_Map);

  //fill and insert valuemapv
  energySCEleJoshEle_filler.insert(electronsHandle,energySCEleJoshEle.begin(),energySCEleJoshEle.end());
  energySCEleJoshEleVar_filler.insert(electronsHandle,energySCEleJoshEleVar.begin(),energySCEleJoshEleVar.end());//fra
  energySCEleJoshPho_filler.insert(electronsHandle,energySCEleJoshPho.begin(),energySCEleJoshPho.end());
  energySCEleJoshPhoVar_filler.insert(electronsHandle,energySCEleJoshPhoVar.begin(),energySCEleJoshPhoVar.end());
  energyEleFra_filler.insert(electronsHandle,energyEleFra.begin(),energyEleFra.end());//fra anche qui
  energyEleFraVar_filler.insert(electronsHandle,energyEleFraVar.begin(),energyEleFraVar.end());//fra anche qui

  energySCEleJoshEleSemiParamV4_ecorr_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV4_ecorr.begin(), energySCEleJoshEleSemiParamV4_ecorr.end());
  energySCEleJoshEleSemiParamV4_sigma_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV4_sigma.begin(), energySCEleJoshEleSemiParamV4_sigma.end()); 
  energySCEleJoshEleSemiParamV4_alpha1_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV4_alpha1.begin(),energySCEleJoshEleSemiParamV4_alpha1.end());
  energySCEleJoshEleSemiParamV4_gamma1_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV4_gamma1.begin(),energySCEleJoshEleSemiParamV4_gamma1.end());
  energySCEleJoshEleSemiParamV4_alpha2_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV4_alpha2.begin(),energySCEleJoshEleSemiParamV4_alpha2.end());
  energySCEleJoshEleSemiParamV4_gamma2_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV4_gamma2.begin(),energySCEleJoshEleSemiParamV4_gamma2.end());
  energySCEleJoshEleSemiParamV4_pdfval_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV4_pdfval.begin(),energySCEleJoshEleSemiParamV4_pdfval.end());

  energySCEleJoshEleSemiParamV5_ecorr_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV5_ecorr.begin(), energySCEleJoshEleSemiParamV5_ecorr.end());
  energySCEleJoshEleSemiParamV5_sigma_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV5_sigma.begin(), energySCEleJoshEleSemiParamV5_sigma.end()); 
  energySCEleJoshEleSemiParamV5_alpha1_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV5_alpha1.begin(),energySCEleJoshEleSemiParamV5_alpha1.end());
  energySCEleJoshEleSemiParamV5_gamma1_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV5_gamma1.begin(),energySCEleJoshEleSemiParamV5_gamma1.end());
  energySCEleJoshEleSemiParamV5_alpha2_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV5_alpha2.begin(),energySCEleJoshEleSemiParamV5_alpha2.end());
  energySCEleJoshEleSemiParamV5_gamma2_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV5_gamma2.begin(),energySCEleJoshEleSemiParamV5_gamma2.end());
  energySCEleJoshEleSemiParamV5_pdfval_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV5_pdfval.begin(),energySCEleJoshEleSemiParamV5_pdfval.end());

  energySCEleJoshPhoSemiParamV4_ecorr_filler.insert(electronsHandle,  energySCEleJoshPhoSemiParamV4_ecorr.begin(), energySCEleJoshPhoSemiParamV4_ecorr.end());
  energySCEleJoshPhoSemiParamV4_sigma_filler.insert(electronsHandle,  energySCEleJoshPhoSemiParamV4_sigma.begin(), energySCEleJoshPhoSemiParamV4_sigma.end()); 
  energySCEleJoshPhoSemiParamV4_alpha1_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV4_alpha1.begin(),energySCEleJoshPhoSemiParamV4_alpha1.end());
  energySCEleJoshPhoSemiParamV4_gamma1_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV4_gamma1.begin(),energySCEleJoshPhoSemiParamV4_gamma1.end());
  energySCEleJoshPhoSemiParamV4_alpha2_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV4_alpha2.begin(),energySCEleJoshPhoSemiParamV4_alpha2.end());
  energySCEleJoshPhoSemiParamV4_gamma2_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV4_gamma2.begin(),energySCEleJoshPhoSemiParamV4_gamma2.end());
  energySCEleJoshPhoSemiParamV4_pdfval_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV4_pdfval.begin(),energySCEleJoshPhoSemiParamV4_pdfval.end());

  energySCEleJoshPhoSemiParamV5_ecorr_filler.insert(electronsHandle,  energySCEleJoshPhoSemiParamV5_ecorr.begin(), energySCEleJoshPhoSemiParamV5_ecorr.end());
  energySCEleJoshPhoSemiParamV5_sigma_filler.insert(electronsHandle,  energySCEleJoshPhoSemiParamV5_sigma.begin(), energySCEleJoshPhoSemiParamV5_sigma.end()); 
  energySCEleJoshPhoSemiParamV5_alpha1_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV5_alpha1.begin(),energySCEleJoshPhoSemiParamV5_alpha1.end());
  energySCEleJoshPhoSemiParamV5_gamma1_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV5_gamma1.begin(),energySCEleJoshPhoSemiParamV5_gamma1.end());
  energySCEleJoshPhoSemiParamV5_alpha2_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV5_alpha2.begin(),energySCEleJoshPhoSemiParamV5_alpha2.end());
  energySCEleJoshPhoSemiParamV5_gamma2_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV5_gamma2.begin(),energySCEleJoshPhoSemiParamV5_gamma2.end());
  energySCEleJoshPhoSemiParamV5_pdfval_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV5_pdfval.begin(),energySCEleJoshPhoSemiParamV5_pdfval.end());

  //------------------------------
  energySCEleJoshEleSemiParamV6_ecorr_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV6_ecorr.begin(), energySCEleJoshEleSemiParamV6_ecorr.end());
  energySCEleJoshEleSemiParamV6_sigmaEoverE_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV6_sigmaEoverE.begin(), energySCEleJoshEleSemiParamV6_sigmaEoverE.end()); 
  energySCEleJoshEleSemiParamV6_cbmean_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV6_cbmean.begin(), energySCEleJoshEleSemiParamV6_cbmean.end()); 
  energySCEleJoshEleSemiParamV6_sigma_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV6_sigma.begin(), energySCEleJoshEleSemiParamV6_sigma.end()); 
  energySCEleJoshEleSemiParamV6_alpha1_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV6_alpha1.begin(),energySCEleJoshEleSemiParamV6_alpha1.end());
  energySCEleJoshEleSemiParamV6_gamma1_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV6_gamma1.begin(),energySCEleJoshEleSemiParamV6_gamma1.end());
  energySCEleJoshEleSemiParamV6_alpha2_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV6_alpha2.begin(),energySCEleJoshEleSemiParamV6_alpha2.end());
  energySCEleJoshEleSemiParamV6_gamma2_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV6_gamma2.begin(),energySCEleJoshEleSemiParamV6_gamma2.end());
  energySCEleJoshEleSemiParamV6_pdfval_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV6_pdfval.begin(),energySCEleJoshEleSemiParamV6_pdfval.end());

  energySCEleJoshPhoSemiParamV6_ecorr_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV6_ecorr.begin(), energySCEleJoshPhoSemiParamV6_ecorr.end());
  energySCEleJoshPhoSemiParamV6_sigmaEoverE_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV6_sigmaEoverE.begin(), energySCEleJoshPhoSemiParamV6_sigmaEoverE.end()); 
  energySCEleJoshPhoSemiParamV6_cbmean_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV6_cbmean.begin(), energySCEleJoshPhoSemiParamV6_cbmean.end()); 
  energySCEleJoshPhoSemiParamV6_sigma_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV6_sigma.begin(), energySCEleJoshPhoSemiParamV6_sigma.end()); 
  energySCEleJoshPhoSemiParamV6_alpha1_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV6_alpha1.begin(),energySCEleJoshPhoSemiParamV6_alpha1.end());
  energySCEleJoshPhoSemiParamV6_gamma1_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV6_gamma1.begin(),energySCEleJoshPhoSemiParamV6_gamma1.end());
  energySCEleJoshPhoSemiParamV6_alpha2_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV6_alpha2.begin(),energySCEleJoshPhoSemiParamV6_alpha2.end());
  energySCEleJoshPhoSemiParamV6_gamma2_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV6_gamma2.begin(),energySCEleJoshPhoSemiParamV6_gamma2.end());
  energySCEleJoshPhoSemiParamV6_pdfval_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV6_pdfval.begin(),energySCEleJoshPhoSemiParamV6_pdfval.end());

  //------------------------------
  energySCEleJoshEleSemiParamV7_ecorr_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV7_ecorr.begin(), energySCEleJoshEleSemiParamV7_ecorr.end());
  energySCEleJoshEleSemiParamV7_sigmaEoverE_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV7_sigmaEoverE.begin(), energySCEleJoshEleSemiParamV7_sigmaEoverE.end()); 
  energySCEleJoshEleSemiParamV7_cbmean_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV7_cbmean.begin(), energySCEleJoshEleSemiParamV7_cbmean.end()); 
  energySCEleJoshEleSemiParamV7_sigma_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV7_sigma.begin(), energySCEleJoshEleSemiParamV7_sigma.end()); 
  energySCEleJoshEleSemiParamV7_alpha1_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV7_alpha1.begin(),energySCEleJoshEleSemiParamV7_alpha1.end());
  energySCEleJoshEleSemiParamV7_gamma1_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV7_gamma1.begin(),energySCEleJoshEleSemiParamV7_gamma1.end());
  energySCEleJoshEleSemiParamV7_alpha2_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV7_alpha2.begin(),energySCEleJoshEleSemiParamV7_alpha2.end());
  energySCEleJoshEleSemiParamV7_gamma2_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV7_gamma2.begin(),energySCEleJoshEleSemiParamV7_gamma2.end());
  energySCEleJoshEleSemiParamV7_pdfval_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV7_pdfval.begin(),energySCEleJoshEleSemiParamV7_pdfval.end());

  energySCEleJoshPhoSemiParamV7_ecorr_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV7_ecorr.begin(), energySCEleJoshPhoSemiParamV7_ecorr.end());
  energySCEleJoshPhoSemiParamV7_sigmaEoverE_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV7_sigmaEoverE.begin(), energySCEleJoshPhoSemiParamV7_sigmaEoverE.end()); 
  energySCEleJoshPhoSemiParamV7_cbmean_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV7_cbmean.begin(), energySCEleJoshPhoSemiParamV7_cbmean.end()); 
  energySCEleJoshPhoSemiParamV7_sigma_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV7_sigma.begin(), energySCEleJoshPhoSemiParamV7_sigma.end()); 
  energySCEleJoshPhoSemiParamV7_alpha1_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV7_alpha1.begin(),energySCEleJoshPhoSemiParamV7_alpha1.end());
  energySCEleJoshPhoSemiParamV7_gamma1_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV7_gamma1.begin(),energySCEleJoshPhoSemiParamV7_gamma1.end());
  energySCEleJoshPhoSemiParamV7_alpha2_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV7_alpha2.begin(),energySCEleJoshPhoSemiParamV7_alpha2.end());
  energySCEleJoshPhoSemiParamV7_gamma2_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV7_gamma2.begin(),energySCEleJoshPhoSemiParamV7_gamma2.end());
  energySCEleJoshPhoSemiParamV7_pdfval_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV7_pdfval.begin(),energySCEleJoshPhoSemiParamV7_pdfval.end());

  //------------------------------
  energySCEleJoshEleSemiParamV8_ecorr_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV8_ecorr.begin(), energySCEleJoshEleSemiParamV8_ecorr.end());
  energySCEleJoshEleSemiParamV8_sigmaEoverE_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV8_sigmaEoverE.begin(), energySCEleJoshEleSemiParamV8_sigmaEoverE.end()); 
  energySCEleJoshEleSemiParamV8_cbmean_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV8_cbmean.begin(), energySCEleJoshEleSemiParamV8_cbmean.end()); 
  energySCEleJoshEleSemiParamV8_sigma_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV8_sigma.begin(), energySCEleJoshEleSemiParamV8_sigma.end()); 
  energySCEleJoshEleSemiParamV8_alpha1_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV8_alpha1.begin(),energySCEleJoshEleSemiParamV8_alpha1.end());
  energySCEleJoshEleSemiParamV8_gamma1_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV8_gamma1.begin(),energySCEleJoshEleSemiParamV8_gamma1.end());
  energySCEleJoshEleSemiParamV8_alpha2_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV8_alpha2.begin(),energySCEleJoshEleSemiParamV8_alpha2.end());
  energySCEleJoshEleSemiParamV8_gamma2_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV8_gamma2.begin(),energySCEleJoshEleSemiParamV8_gamma2.end());
  energySCEleJoshEleSemiParamV8_pdfval_filler.insert(electronsHandle, energySCEleJoshEleSemiParamV8_pdfval.begin(),energySCEleJoshEleSemiParamV8_pdfval.end());

  energySCEleJoshPhoSemiParamV8_ecorr_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV8_ecorr.begin(), energySCEleJoshPhoSemiParamV8_ecorr.end());
  energySCEleJoshPhoSemiParamV8_sigmaEoverE_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV8_sigmaEoverE.begin(), energySCEleJoshPhoSemiParamV8_sigmaEoverE.end()); 
  energySCEleJoshPhoSemiParamV8_cbmean_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV8_cbmean.begin(), energySCEleJoshPhoSemiParamV8_cbmean.end()); 
  energySCEleJoshPhoSemiParamV8_sigma_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV8_sigma.begin(), energySCEleJoshPhoSemiParamV8_sigma.end()); 
  energySCEleJoshPhoSemiParamV8_alpha1_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV8_alpha1.begin(),energySCEleJoshPhoSemiParamV8_alpha1.end());
  energySCEleJoshPhoSemiParamV8_gamma1_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV8_gamma1.begin(),energySCEleJoshPhoSemiParamV8_gamma1.end());
  energySCEleJoshPhoSemiParamV8_alpha2_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV8_alpha2.begin(),energySCEleJoshPhoSemiParamV8_alpha2.end());
  energySCEleJoshPhoSemiParamV8_gamma2_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV8_gamma2.begin(),energySCEleJoshPhoSemiParamV8_gamma2.end());
  energySCEleJoshPhoSemiParamV8_pdfval_filler.insert(electronsHandle, energySCEleJoshPhoSemiParamV8_pdfval.begin(),energySCEleJoshPhoSemiParamV8_pdfval.end());

  //------------------------------
  energySCEleJoshEleSemiParam7TeVtrainV6_ecorr_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV6_ecorr.begin(), energySCEleJoshEleSemiParam7TeVtrainV6_ecorr.end());
  energySCEleJoshEleSemiParam7TeVtrainV6_sigmaEoverE_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV6_sigmaEoverE.begin(), energySCEleJoshEleSemiParam7TeVtrainV6_sigmaEoverE.end()); 
  energySCEleJoshEleSemiParam7TeVtrainV6_cbmean_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV6_cbmean.begin(), energySCEleJoshEleSemiParam7TeVtrainV6_cbmean.end()); 
  energySCEleJoshEleSemiParam7TeVtrainV6_sigma_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV6_sigma.begin(), energySCEleJoshEleSemiParam7TeVtrainV6_sigma.end()); 
  energySCEleJoshEleSemiParam7TeVtrainV6_alpha1_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV6_alpha1.begin(),energySCEleJoshEleSemiParam7TeVtrainV6_alpha1.end());
  energySCEleJoshEleSemiParam7TeVtrainV6_gamma1_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV6_gamma1.begin(),energySCEleJoshEleSemiParam7TeVtrainV6_gamma1.end());
  energySCEleJoshEleSemiParam7TeVtrainV6_alpha2_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV6_alpha2.begin(),energySCEleJoshEleSemiParam7TeVtrainV6_alpha2.end());
  energySCEleJoshEleSemiParam7TeVtrainV6_gamma2_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV6_gamma2.begin(),energySCEleJoshEleSemiParam7TeVtrainV6_gamma2.end());
  energySCEleJoshEleSemiParam7TeVtrainV6_pdfval_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV6_pdfval.begin(),energySCEleJoshEleSemiParam7TeVtrainV6_pdfval.end());

  energySCEleJoshPhoSemiParam7TeVtrainV6_ecorr_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV6_ecorr.begin(), energySCEleJoshPhoSemiParam7TeVtrainV6_ecorr.end());
  energySCEleJoshPhoSemiParam7TeVtrainV6_sigmaEoverE_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV6_sigmaEoverE.begin(), energySCEleJoshPhoSemiParam7TeVtrainV6_sigmaEoverE.end()); 
  energySCEleJoshPhoSemiParam7TeVtrainV6_cbmean_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV6_cbmean.begin(), energySCEleJoshPhoSemiParam7TeVtrainV6_cbmean.end()); 
  energySCEleJoshPhoSemiParam7TeVtrainV6_sigma_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV6_sigma.begin(), energySCEleJoshPhoSemiParam7TeVtrainV6_sigma.end()); 
  energySCEleJoshPhoSemiParam7TeVtrainV6_alpha1_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV6_alpha1.begin(),energySCEleJoshPhoSemiParam7TeVtrainV6_alpha1.end());
  energySCEleJoshPhoSemiParam7TeVtrainV6_gamma1_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV6_gamma1.begin(),energySCEleJoshPhoSemiParam7TeVtrainV6_gamma1.end());
  energySCEleJoshPhoSemiParam7TeVtrainV6_alpha2_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV6_alpha2.begin(),energySCEleJoshPhoSemiParam7TeVtrainV6_alpha2.end());
  energySCEleJoshPhoSemiParam7TeVtrainV6_gamma2_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV6_gamma2.begin(),energySCEleJoshPhoSemiParam7TeVtrainV6_gamma2.end());
  energySCEleJoshPhoSemiParam7TeVtrainV6_pdfval_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV6_pdfval.begin(),energySCEleJoshPhoSemiParam7TeVtrainV6_pdfval.end());

  //------------------------------
  energySCEleJoshEleSemiParam7TeVtrainV7_ecorr_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV7_ecorr.begin(), energySCEleJoshEleSemiParam7TeVtrainV7_ecorr.end());
  energySCEleJoshEleSemiParam7TeVtrainV7_sigmaEoverE_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV7_sigmaEoverE.begin(), energySCEleJoshEleSemiParam7TeVtrainV7_sigmaEoverE.end()); 
  energySCEleJoshEleSemiParam7TeVtrainV7_cbmean_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV7_cbmean.begin(), energySCEleJoshEleSemiParam7TeVtrainV7_cbmean.end()); 
  energySCEleJoshEleSemiParam7TeVtrainV7_sigma_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV7_sigma.begin(), energySCEleJoshEleSemiParam7TeVtrainV7_sigma.end()); 
  energySCEleJoshEleSemiParam7TeVtrainV7_alpha1_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV7_alpha1.begin(),energySCEleJoshEleSemiParam7TeVtrainV7_alpha1.end());
  energySCEleJoshEleSemiParam7TeVtrainV7_gamma1_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV7_gamma1.begin(),energySCEleJoshEleSemiParam7TeVtrainV7_gamma1.end());
  energySCEleJoshEleSemiParam7TeVtrainV7_alpha2_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV7_alpha2.begin(),energySCEleJoshEleSemiParam7TeVtrainV7_alpha2.end());
  energySCEleJoshEleSemiParam7TeVtrainV7_gamma2_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV7_gamma2.begin(),energySCEleJoshEleSemiParam7TeVtrainV7_gamma2.end());
  energySCEleJoshEleSemiParam7TeVtrainV7_pdfval_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV7_pdfval.begin(),energySCEleJoshEleSemiParam7TeVtrainV7_pdfval.end());

  energySCEleJoshPhoSemiParam7TeVtrainV7_ecorr_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV7_ecorr.begin(), energySCEleJoshPhoSemiParam7TeVtrainV7_ecorr.end());
  energySCEleJoshPhoSemiParam7TeVtrainV7_sigmaEoverE_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV7_sigmaEoverE.begin(), energySCEleJoshPhoSemiParam7TeVtrainV7_sigmaEoverE.end()); 
  energySCEleJoshPhoSemiParam7TeVtrainV7_cbmean_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV7_cbmean.begin(), energySCEleJoshPhoSemiParam7TeVtrainV7_cbmean.end()); 
  energySCEleJoshPhoSemiParam7TeVtrainV7_sigma_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV7_sigma.begin(), energySCEleJoshPhoSemiParam7TeVtrainV7_sigma.end()); 
  energySCEleJoshPhoSemiParam7TeVtrainV7_alpha1_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV7_alpha1.begin(),energySCEleJoshPhoSemiParam7TeVtrainV7_alpha1.end());
  energySCEleJoshPhoSemiParam7TeVtrainV7_gamma1_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV7_gamma1.begin(),energySCEleJoshPhoSemiParam7TeVtrainV7_gamma1.end());
  energySCEleJoshPhoSemiParam7TeVtrainV7_alpha2_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV7_alpha2.begin(),energySCEleJoshPhoSemiParam7TeVtrainV7_alpha2.end());
  energySCEleJoshPhoSemiParam7TeVtrainV7_gamma2_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV7_gamma2.begin(),energySCEleJoshPhoSemiParam7TeVtrainV7_gamma2.end());
  energySCEleJoshPhoSemiParam7TeVtrainV7_pdfval_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV7_pdfval.begin(),energySCEleJoshPhoSemiParam7TeVtrainV7_pdfval.end());

  //------------------------------
  energySCEleJoshEleSemiParam7TeVtrainV8_ecorr_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV8_ecorr.begin(), energySCEleJoshEleSemiParam7TeVtrainV8_ecorr.end());
  energySCEleJoshEleSemiParam7TeVtrainV8_sigmaEoverE_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV8_sigmaEoverE.begin(), energySCEleJoshEleSemiParam7TeVtrainV8_sigmaEoverE.end()); 
  energySCEleJoshEleSemiParam7TeVtrainV8_cbmean_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV8_cbmean.begin(), energySCEleJoshEleSemiParam7TeVtrainV8_cbmean.end()); 
  energySCEleJoshEleSemiParam7TeVtrainV8_sigma_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV8_sigma.begin(), energySCEleJoshEleSemiParam7TeVtrainV8_sigma.end()); 
  energySCEleJoshEleSemiParam7TeVtrainV8_alpha1_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV8_alpha1.begin(),energySCEleJoshEleSemiParam7TeVtrainV8_alpha1.end());
  energySCEleJoshEleSemiParam7TeVtrainV8_gamma1_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV8_gamma1.begin(),energySCEleJoshEleSemiParam7TeVtrainV8_gamma1.end());
  energySCEleJoshEleSemiParam7TeVtrainV8_alpha2_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV8_alpha2.begin(),energySCEleJoshEleSemiParam7TeVtrainV8_alpha2.end());
  energySCEleJoshEleSemiParam7TeVtrainV8_gamma2_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV8_gamma2.begin(),energySCEleJoshEleSemiParam7TeVtrainV8_gamma2.end());
  energySCEleJoshEleSemiParam7TeVtrainV8_pdfval_filler.insert(electronsHandle, energySCEleJoshEleSemiParam7TeVtrainV8_pdfval.begin(),energySCEleJoshEleSemiParam7TeVtrainV8_pdfval.end());

  energySCEleJoshPhoSemiParam7TeVtrainV8_ecorr_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV8_ecorr.begin(), energySCEleJoshPhoSemiParam7TeVtrainV8_ecorr.end());
  energySCEleJoshPhoSemiParam7TeVtrainV8_sigmaEoverE_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV8_sigmaEoverE.begin(), energySCEleJoshPhoSemiParam7TeVtrainV8_sigmaEoverE.end()); 
  energySCEleJoshPhoSemiParam7TeVtrainV8_cbmean_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV8_cbmean.begin(), energySCEleJoshPhoSemiParam7TeVtrainV8_cbmean.end()); 
  energySCEleJoshPhoSemiParam7TeVtrainV8_sigma_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV8_sigma.begin(), energySCEleJoshPhoSemiParam7TeVtrainV8_sigma.end()); 
  energySCEleJoshPhoSemiParam7TeVtrainV8_alpha1_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV8_alpha1.begin(),energySCEleJoshPhoSemiParam7TeVtrainV8_alpha1.end());
  energySCEleJoshPhoSemiParam7TeVtrainV8_gamma1_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV8_gamma1.begin(),energySCEleJoshPhoSemiParam7TeVtrainV8_gamma1.end());
  energySCEleJoshPhoSemiParam7TeVtrainV8_alpha2_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV8_alpha2.begin(),energySCEleJoshPhoSemiParam7TeVtrainV8_alpha2.end());
  energySCEleJoshPhoSemiParam7TeVtrainV8_gamma2_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV8_gamma2.begin(),energySCEleJoshPhoSemiParam7TeVtrainV8_gamma2.end());
  energySCEleJoshPhoSemiParam7TeVtrainV8_pdfval_filler.insert(electronsHandle, energySCEleJoshPhoSemiParam7TeVtrainV8_pdfval.begin(),energySCEleJoshPhoSemiParam7TeVtrainV8_pdfval.end());

  energySCEleJoshEle_filler.fill();
  energySCEleJoshEleVar_filler.fill();//fra
  energySCEleJoshPho_filler.fill();
  energySCEleJoshPhoVar_filler.fill();
  energyEleFra_filler.fill();
  energyEleFraVar_filler.fill();//fra

  energySCEleJoshEleSemiParamV4_ecorr_filler.fill();
  energySCEleJoshEleSemiParamV4_sigma_filler.fill();
  energySCEleJoshEleSemiParamV4_alpha1_filler.fill();
  energySCEleJoshEleSemiParamV4_gamma1_filler.fill();
  energySCEleJoshEleSemiParamV4_alpha2_filler.fill();
  energySCEleJoshEleSemiParamV4_gamma2_filler.fill();
  energySCEleJoshEleSemiParamV4_pdfval_filler.fill();

  energySCEleJoshEleSemiParamV5_ecorr_filler.fill();
  energySCEleJoshEleSemiParamV5_sigma_filler.fill();
  energySCEleJoshEleSemiParamV5_alpha1_filler.fill();
  energySCEleJoshEleSemiParamV5_gamma1_filler.fill();
  energySCEleJoshEleSemiParamV5_alpha2_filler.fill();
  energySCEleJoshEleSemiParamV5_gamma2_filler.fill();
  energySCEleJoshEleSemiParamV5_pdfval_filler.fill();

  energySCEleJoshPhoSemiParamV4_ecorr_filler.fill();
  energySCEleJoshPhoSemiParamV4_sigma_filler.fill();
  energySCEleJoshPhoSemiParamV4_alpha1_filler.fill();
  energySCEleJoshPhoSemiParamV4_gamma1_filler.fill();
  energySCEleJoshPhoSemiParamV4_alpha2_filler.fill();
  energySCEleJoshPhoSemiParamV4_gamma2_filler.fill();
  energySCEleJoshPhoSemiParamV4_pdfval_filler.fill();
		 
  energySCEleJoshPhoSemiParamV5_ecorr_filler.fill();
  energySCEleJoshPhoSemiParamV5_sigma_filler.fill();
  energySCEleJoshPhoSemiParamV5_alpha1_filler.fill();
  energySCEleJoshPhoSemiParamV5_gamma1_filler.fill();
  energySCEleJoshPhoSemiParamV5_alpha2_filler.fill();
  energySCEleJoshPhoSemiParamV5_gamma2_filler.fill();
  energySCEleJoshPhoSemiParamV5_pdfval_filler.fill();

  //------------------------------
  energySCEleJoshEleSemiParamV6_ecorr_filler.fill();
  energySCEleJoshEleSemiParamV6_sigmaEoverE_filler.fill();
  energySCEleJoshEleSemiParamV6_cbmean_filler.fill();
  energySCEleJoshEleSemiParamV6_sigma_filler.fill();
  energySCEleJoshEleSemiParamV6_alpha1_filler.fill();
  energySCEleJoshEleSemiParamV6_gamma1_filler.fill();
  energySCEleJoshEleSemiParamV6_alpha2_filler.fill();
  energySCEleJoshEleSemiParamV6_gamma2_filler.fill();
  energySCEleJoshEleSemiParamV6_pdfval_filler.fill();

  energySCEleJoshPhoSemiParamV6_ecorr_filler.fill();
  energySCEleJoshPhoSemiParamV6_sigmaEoverE_filler.fill();
  energySCEleJoshPhoSemiParamV6_cbmean_filler.fill();
  energySCEleJoshPhoSemiParamV6_sigma_filler.fill();
  energySCEleJoshPhoSemiParamV6_alpha1_filler.fill();
  energySCEleJoshPhoSemiParamV6_gamma1_filler.fill();
  energySCEleJoshPhoSemiParamV6_alpha2_filler.fill();
  energySCEleJoshPhoSemiParamV6_gamma2_filler.fill();
  energySCEleJoshPhoSemiParamV6_pdfval_filler.fill();

  //------------------------------
  energySCEleJoshEleSemiParamV7_ecorr_filler.fill();
  energySCEleJoshEleSemiParamV7_sigmaEoverE_filler.fill();
  energySCEleJoshEleSemiParamV7_cbmean_filler.fill();
  energySCEleJoshEleSemiParamV7_sigma_filler.fill();
  energySCEleJoshEleSemiParamV7_alpha1_filler.fill();
  energySCEleJoshEleSemiParamV7_gamma1_filler.fill();
  energySCEleJoshEleSemiParamV7_alpha2_filler.fill();
  energySCEleJoshEleSemiParamV7_gamma2_filler.fill();
  energySCEleJoshEleSemiParamV7_pdfval_filler.fill();

  energySCEleJoshPhoSemiParamV7_ecorr_filler.fill();
  energySCEleJoshPhoSemiParamV7_sigmaEoverE_filler.fill();
  energySCEleJoshPhoSemiParamV7_cbmean_filler.fill();
  energySCEleJoshPhoSemiParamV7_sigma_filler.fill();
  energySCEleJoshPhoSemiParamV7_alpha1_filler.fill();
  energySCEleJoshPhoSemiParamV7_gamma1_filler.fill();
  energySCEleJoshPhoSemiParamV7_alpha2_filler.fill();
  energySCEleJoshPhoSemiParamV7_gamma2_filler.fill();
  energySCEleJoshPhoSemiParamV7_pdfval_filler.fill();

  //------------------------------
  energySCEleJoshEleSemiParamV8_ecorr_filler.fill();
  energySCEleJoshEleSemiParamV8_sigmaEoverE_filler.fill();
  energySCEleJoshEleSemiParamV8_cbmean_filler.fill();
  energySCEleJoshEleSemiParamV8_sigma_filler.fill();
  energySCEleJoshEleSemiParamV8_alpha1_filler.fill();
  energySCEleJoshEleSemiParamV8_gamma1_filler.fill();
  energySCEleJoshEleSemiParamV8_alpha2_filler.fill();
  energySCEleJoshEleSemiParamV8_gamma2_filler.fill();
  energySCEleJoshEleSemiParamV8_pdfval_filler.fill();

  energySCEleJoshPhoSemiParamV8_ecorr_filler.fill();
  energySCEleJoshPhoSemiParamV8_sigmaEoverE_filler.fill();
  energySCEleJoshPhoSemiParamV8_cbmean_filler.fill();
  energySCEleJoshPhoSemiParamV8_sigma_filler.fill();
  energySCEleJoshPhoSemiParamV8_alpha1_filler.fill();
  energySCEleJoshPhoSemiParamV8_gamma1_filler.fill();
  energySCEleJoshPhoSemiParamV8_alpha2_filler.fill();
  energySCEleJoshPhoSemiParamV8_gamma2_filler.fill();
  energySCEleJoshPhoSemiParamV8_pdfval_filler.fill();

  //------------------------------
  energySCEleJoshEleSemiParam7TeVtrainV6_ecorr_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV6_sigmaEoverE_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV6_cbmean_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV6_sigma_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV6_alpha1_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV6_gamma1_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV6_alpha2_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV6_gamma2_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV6_pdfval_filler.fill();

  energySCEleJoshPhoSemiParam7TeVtrainV6_ecorr_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV6_sigmaEoverE_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV6_cbmean_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV6_sigma_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV6_alpha1_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV6_gamma1_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV6_alpha2_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV6_gamma2_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV6_pdfval_filler.fill();

  //------------------------------
  energySCEleJoshEleSemiParam7TeVtrainV7_ecorr_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV7_sigmaEoverE_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV7_cbmean_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV7_sigma_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV7_alpha1_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV7_gamma1_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV7_alpha2_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV7_gamma2_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV7_pdfval_filler.fill();

  energySCEleJoshPhoSemiParam7TeVtrainV7_ecorr_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV7_sigmaEoverE_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV7_cbmean_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV7_sigma_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV7_alpha1_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV7_gamma1_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV7_alpha2_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV7_gamma2_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV7_pdfval_filler.fill();

  //------------------------------
  energySCEleJoshEleSemiParam7TeVtrainV8_ecorr_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV8_sigmaEoverE_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV8_cbmean_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV8_sigma_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV8_alpha1_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV8_gamma1_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV8_alpha2_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV8_gamma2_filler.fill();
  energySCEleJoshEleSemiParam7TeVtrainV8_pdfval_filler.fill();

  energySCEleJoshPhoSemiParam7TeVtrainV8_ecorr_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV8_sigmaEoverE_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV8_cbmean_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV8_sigma_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV8_alpha1_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV8_gamma1_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV8_alpha2_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV8_gamma2_filler.fill();
  energySCEleJoshPhoSemiParam7TeVtrainV8_pdfval_filler.fill();

  //------------------------------
  // put the ValueMap in the event
  iEvent.put(energySCEleJoshPhoMap, "energySCEleJoshPho");
  iEvent.put(energySCEleJoshPhoVarMap, "energySCEleJoshPhoVar");//fra
  iEvent.put(energySCEleJoshEleMap, "energySCEleJoshEle");
  iEvent.put(energySCEleJoshEleVarMap, "energySCEleJoshEleVar");//fra
  iEvent.put(energyEleFraMap, "energyEleFra");
  iEvent.put(energyEleFraVarMap, "energyEleFraVar");//fra

  // electron
  iEvent.put(energySCEleJoshEleSemiParamV4_ecorr_Map,  "energySCEleJoshEleSemiParamV4ecorr");
  iEvent.put(energySCEleJoshEleSemiParamV4_sigma_Map,  "energySCEleJoshEleSemiParamV4sigma");
  iEvent.put(energySCEleJoshEleSemiParamV4_alpha1_Map, "energySCEleJoshEleSemiParamV4alpha1");
  iEvent.put(energySCEleJoshEleSemiParamV4_gamma1_Map, "energySCEleJoshEleSemiParamV4gamma1");
  iEvent.put(energySCEleJoshEleSemiParamV4_alpha2_Map, "energySCEleJoshEleSemiParamV4alpha2");
  iEvent.put(energySCEleJoshEleSemiParamV4_gamma2_Map, "energySCEleJoshEleSemiParamV4gamma2");
  iEvent.put(energySCEleJoshEleSemiParamV4_pdfval_Map, "energySCEleJoshEleSemiParamV4pdfval");
  
  iEvent.put(energySCEleJoshEleSemiParamV5_ecorr_Map,  "energySCEleJoshEleSemiParamV5ecorr");
  iEvent.put(energySCEleJoshEleSemiParamV5_sigma_Map,  "energySCEleJoshEleSemiParamV5sigma");
  iEvent.put(energySCEleJoshEleSemiParamV5_alpha1_Map, "energySCEleJoshEleSemiParamV5alpha1");
  iEvent.put(energySCEleJoshEleSemiParamV5_gamma1_Map, "energySCEleJoshEleSemiParamV5gamma1");
  iEvent.put(energySCEleJoshEleSemiParamV5_alpha2_Map, "energySCEleJoshEleSemiParamV5alpha2");
  iEvent.put(energySCEleJoshEleSemiParamV5_gamma2_Map, "energySCEleJoshEleSemiParamV5gamma2");
  iEvent.put(energySCEleJoshEleSemiParamV5_pdfval_Map, "energySCEleJoshEleSemiParamV5pdfval");
  // photon 
  iEvent.put(energySCEleJoshPhoSemiParamV4_ecorr_Map,  "energySCEleJoshPhoSemiParamV4ecorr");
  iEvent.put(energySCEleJoshPhoSemiParamV4_sigma_Map,  "energySCEleJoshPhoSemiParamV4sigma");
  iEvent.put(energySCEleJoshPhoSemiParamV4_alpha1_Map, "energySCEleJoshPhoSemiParamV4alpha1");
  iEvent.put(energySCEleJoshPhoSemiParamV4_gamma1_Map, "energySCEleJoshPhoSemiParamV4gamma1");
  iEvent.put(energySCEleJoshPhoSemiParamV4_alpha2_Map, "energySCEleJoshPhoSemiParamV4alpha2");
  iEvent.put(energySCEleJoshPhoSemiParamV4_gamma2_Map, "energySCEleJoshPhoSemiParamV4gamma2");
  iEvent.put(energySCEleJoshPhoSemiParamV4_pdfval_Map, "energySCEleJoshPhoSemiParamV4pdfval");

  iEvent.put(energySCEleJoshPhoSemiParamV5_ecorr_Map,  "energySCEleJoshPhoSemiParamV5ecorr");
  iEvent.put(energySCEleJoshPhoSemiParamV5_sigma_Map,  "energySCEleJoshPhoSemiParamV5sigma");
  iEvent.put(energySCEleJoshPhoSemiParamV5_alpha1_Map, "energySCEleJoshPhoSemiParamV5alpha1");
  iEvent.put(energySCEleJoshPhoSemiParamV5_gamma1_Map, "energySCEleJoshPhoSemiParamV5gamma1");
  iEvent.put(energySCEleJoshPhoSemiParamV5_alpha2_Map, "energySCEleJoshPhoSemiParamV5alpha2");
  iEvent.put(energySCEleJoshPhoSemiParamV5_gamma2_Map, "energySCEleJoshPhoSemiParamV5gamma2");
  iEvent.put(energySCEleJoshPhoSemiParamV5_pdfval_Map, "energySCEleJoshPhoSemiParamV5pdfval");

  //------------------------------
  iEvent.put(energySCEleJoshEleSemiParamV6_ecorr_Map,       "energySCEleJoshEleSemiParamV6ecorr");
  iEvent.put(energySCEleJoshEleSemiParamV6_sigmaEoverE_Map, "energySCEleJoshEleSemiParamV6sigmaEoverE");
  iEvent.put(energySCEleJoshEleSemiParamV6_cbmean_Map,      "energySCEleJoshEleSemiParamV6cbmean");
  iEvent.put(energySCEleJoshEleSemiParamV6_sigma_Map,       "energySCEleJoshEleSemiParamV6sigma");
  iEvent.put(energySCEleJoshEleSemiParamV6_alpha1_Map,      "energySCEleJoshEleSemiParamV6alpha1");
  iEvent.put(energySCEleJoshEleSemiParamV6_gamma1_Map,      "energySCEleJoshEleSemiParamV6gamma1");
  iEvent.put(energySCEleJoshEleSemiParamV6_alpha2_Map,      "energySCEleJoshEleSemiParamV6alpha2");
  iEvent.put(energySCEleJoshEleSemiParamV6_gamma2_Map,      "energySCEleJoshEleSemiParamV6gamma2");
  iEvent.put(energySCEleJoshEleSemiParamV6_pdfval_Map,      "energySCEleJoshEleSemiParamV6pdfval");

  iEvent.put(energySCEleJoshPhoSemiParamV6_ecorr_Map,       "energySCEleJoshPhoSemiParamV6ecorr");
  iEvent.put(energySCEleJoshPhoSemiParamV6_sigmaEoverE_Map, "energySCEleJoshPhoSemiParamV6sigmaEoverE");
  iEvent.put(energySCEleJoshPhoSemiParamV6_cbmean_Map,      "energySCEleJoshPhoSemiParamV6cbmean");
  iEvent.put(energySCEleJoshPhoSemiParamV6_sigma_Map,       "energySCEleJoshPhoSemiParamV6sigma");
  iEvent.put(energySCEleJoshPhoSemiParamV6_alpha1_Map,      "energySCEleJoshPhoSemiParamV6alpha1");
  iEvent.put(energySCEleJoshPhoSemiParamV6_gamma1_Map,      "energySCEleJoshPhoSemiParamV6gamma1");
  iEvent.put(energySCEleJoshPhoSemiParamV6_alpha2_Map,      "energySCEleJoshPhoSemiParamV6alpha2");
  iEvent.put(energySCEleJoshPhoSemiParamV6_gamma2_Map,      "energySCEleJoshPhoSemiParamV6gamma2");
  iEvent.put(energySCEleJoshPhoSemiParamV6_pdfval_Map,      "energySCEleJoshPhoSemiParamV6pdfval");
  
//------------------------------
  iEvent.put(energySCEleJoshEleSemiParamV7_ecorr_Map,       "energySCEleJoshEleSemiParamV7ecorr");
  iEvent.put(energySCEleJoshEleSemiParamV7_sigmaEoverE_Map, "energySCEleJoshEleSemiParamV7sigmaEoverE");
  iEvent.put(energySCEleJoshEleSemiParamV7_cbmean_Map,      "energySCEleJoshEleSemiParamV7cbmean");
  iEvent.put(energySCEleJoshEleSemiParamV7_sigma_Map,       "energySCEleJoshEleSemiParamV7sigma");
  iEvent.put(energySCEleJoshEleSemiParamV7_alpha1_Map,      "energySCEleJoshEleSemiParamV7alpha1");
  iEvent.put(energySCEleJoshEleSemiParamV7_gamma1_Map,      "energySCEleJoshEleSemiParamV7gamma1");
  iEvent.put(energySCEleJoshEleSemiParamV7_alpha2_Map,      "energySCEleJoshEleSemiParamV7alpha2");
  iEvent.put(energySCEleJoshEleSemiParamV7_gamma2_Map,      "energySCEleJoshEleSemiParamV7gamma2");
  iEvent.put(energySCEleJoshEleSemiParamV7_pdfval_Map,      "energySCEleJoshEleSemiParamV7pdfval");

  iEvent.put(energySCEleJoshPhoSemiParamV7_ecorr_Map,       "energySCEleJoshPhoSemiParamV7ecorr");
  iEvent.put(energySCEleJoshPhoSemiParamV7_sigmaEoverE_Map, "energySCEleJoshPhoSemiParamV7sigmaEoverE");
  iEvent.put(energySCEleJoshPhoSemiParamV7_cbmean_Map,      "energySCEleJoshPhoSemiParamV7cbmean");
  iEvent.put(energySCEleJoshPhoSemiParamV7_sigma_Map,       "energySCEleJoshPhoSemiParamV7sigma");
  iEvent.put(energySCEleJoshPhoSemiParamV7_alpha1_Map,      "energySCEleJoshPhoSemiParamV7alpha1");
  iEvent.put(energySCEleJoshPhoSemiParamV7_gamma1_Map,      "energySCEleJoshPhoSemiParamV7gamma1");
  iEvent.put(energySCEleJoshPhoSemiParamV7_alpha2_Map,      "energySCEleJoshPhoSemiParamV7alpha2");
  iEvent.put(energySCEleJoshPhoSemiParamV7_gamma2_Map,      "energySCEleJoshPhoSemiParamV7gamma2");
  iEvent.put(energySCEleJoshPhoSemiParamV7_pdfval_Map,      "energySCEleJoshPhoSemiParamV7pdfval");
  
//------------------------------
  iEvent.put(energySCEleJoshEleSemiParamV8_ecorr_Map,       "energySCEleJoshEleSemiParamV8ecorr");
  iEvent.put(energySCEleJoshEleSemiParamV8_sigmaEoverE_Map, "energySCEleJoshEleSemiParamV8sigmaEoverE");
  iEvent.put(energySCEleJoshEleSemiParamV8_cbmean_Map,      "energySCEleJoshEleSemiParamV8cbmean");
  iEvent.put(energySCEleJoshEleSemiParamV8_sigma_Map,       "energySCEleJoshEleSemiParamV8sigma");
  iEvent.put(energySCEleJoshEleSemiParamV8_alpha1_Map,      "energySCEleJoshEleSemiParamV8alpha1");
  iEvent.put(energySCEleJoshEleSemiParamV8_gamma1_Map,      "energySCEleJoshEleSemiParamV8gamma1");
  iEvent.put(energySCEleJoshEleSemiParamV8_alpha2_Map,      "energySCEleJoshEleSemiParamV8alpha2");
  iEvent.put(energySCEleJoshEleSemiParamV8_gamma2_Map,      "energySCEleJoshEleSemiParamV8gamma2");
  iEvent.put(energySCEleJoshEleSemiParamV8_pdfval_Map,      "energySCEleJoshEleSemiParamV8pdfval");

  iEvent.put(energySCEleJoshPhoSemiParamV8_ecorr_Map,       "energySCEleJoshPhoSemiParamV8ecorr");
  iEvent.put(energySCEleJoshPhoSemiParamV8_sigmaEoverE_Map, "energySCEleJoshPhoSemiParamV8sigmaEoverE");
  iEvent.put(energySCEleJoshPhoSemiParamV8_cbmean_Map,      "energySCEleJoshPhoSemiParamV8cbmean");
  iEvent.put(energySCEleJoshPhoSemiParamV8_sigma_Map,       "energySCEleJoshPhoSemiParamV8sigma");
  iEvent.put(energySCEleJoshPhoSemiParamV8_alpha1_Map,      "energySCEleJoshPhoSemiParamV8alpha1");
  iEvent.put(energySCEleJoshPhoSemiParamV8_gamma1_Map,      "energySCEleJoshPhoSemiParamV8gamma1");
  iEvent.put(energySCEleJoshPhoSemiParamV8_alpha2_Map,      "energySCEleJoshPhoSemiParamV8alpha2");
  iEvent.put(energySCEleJoshPhoSemiParamV8_gamma2_Map,      "energySCEleJoshPhoSemiParamV8gamma2");
  iEvent.put(energySCEleJoshPhoSemiParamV8_pdfval_Map,      "energySCEleJoshPhoSemiParamV8pdfval");

  //------------------------------
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV6_ecorr_Map,       "energySCEleJoshEleSemiParam7TeVtrainV6ecorr");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV6_sigmaEoverE_Map, "energySCEleJoshEleSemiParam7TeVtrainV6sigmaEoverE");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV6_cbmean_Map,      "energySCEleJoshEleSemiParam7TeVtrainV6cbmean");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV6_sigma_Map,       "energySCEleJoshEleSemiParam7TeVtrainV6sigma");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV6_alpha1_Map,      "energySCEleJoshEleSemiParam7TeVtrainV6alpha1");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV6_gamma1_Map,      "energySCEleJoshEleSemiParam7TeVtrainV6gamma1");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV6_alpha2_Map,      "energySCEleJoshEleSemiParam7TeVtrainV6alpha2");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV6_gamma2_Map,      "energySCEleJoshEleSemiParam7TeVtrainV6gamma2");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV6_pdfval_Map,      "energySCEleJoshEleSemiParam7TeVtrainV6pdfval");

  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV6_ecorr_Map,       "energySCEleJoshPhoSemiParam7TeVtrainV6ecorr");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV6_sigmaEoverE_Map, "energySCEleJoshPhoSemiParam7TeVtrainV6sigmaEoverE");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV6_cbmean_Map,      "energySCEleJoshPhoSemiParam7TeVtrainV6cbmean");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV6_sigma_Map,       "energySCEleJoshPhoSemiParam7TeVtrainV6sigma");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV6_alpha1_Map,      "energySCEleJoshPhoSemiParam7TeVtrainV6alpha1");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV6_gamma1_Map,      "energySCEleJoshPhoSemiParam7TeVtrainV6gamma1");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV6_alpha2_Map,      "energySCEleJoshPhoSemiParam7TeVtrainV6alpha2");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV6_gamma2_Map,      "energySCEleJoshPhoSemiParam7TeVtrainV6gamma2");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV6_pdfval_Map,      "energySCEleJoshPhoSemiParam7TeVtrainV6pdfval");
  
//------------------------------
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV7_ecorr_Map,       "energySCEleJoshEleSemiParam7TeVtrainV7ecorr");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV7_sigmaEoverE_Map, "energySCEleJoshEleSemiParam7TeVtrainV7sigmaEoverE");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV7_cbmean_Map,      "energySCEleJoshEleSemiParam7TeVtrainV7cbmean");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV7_sigma_Map,       "energySCEleJoshEleSemiParam7TeVtrainV7sigma");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV7_alpha1_Map,      "energySCEleJoshEleSemiParam7TeVtrainV7alpha1");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV7_gamma1_Map,      "energySCEleJoshEleSemiParam7TeVtrainV7gamma1");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV7_alpha2_Map,      "energySCEleJoshEleSemiParam7TeVtrainV7alpha2");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV7_gamma2_Map,      "energySCEleJoshEleSemiParam7TeVtrainV7gamma2");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV7_pdfval_Map,      "energySCEleJoshEleSemiParam7TeVtrainV7pdfval");

  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV7_ecorr_Map,       "energySCEleJoshPhoSemiParam7TeVtrainV7ecorr");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV7_sigmaEoverE_Map, "energySCEleJoshPhoSemiParam7TeVtrainV7sigmaEoverE");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV7_cbmean_Map,      "energySCEleJoshPhoSemiParam7TeVtrainV7cbmean");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV7_sigma_Map,       "energySCEleJoshPhoSemiParam7TeVtrainV7sigma");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV7_alpha1_Map,      "energySCEleJoshPhoSemiParam7TeVtrainV7alpha1");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV7_gamma1_Map,      "energySCEleJoshPhoSemiParam7TeVtrainV7gamma1");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV7_alpha2_Map,      "energySCEleJoshPhoSemiParam7TeVtrainV7alpha2");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV7_gamma2_Map,      "energySCEleJoshPhoSemiParam7TeVtrainV7gamma2");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV7_pdfval_Map,      "energySCEleJoshPhoSemiParam7TeVtrainV7pdfval");
  
//------------------------------
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV8_ecorr_Map,       "energySCEleJoshEleSemiParam7TeVtrainV8ecorr");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV8_sigmaEoverE_Map, "energySCEleJoshEleSemiParam7TeVtrainV8sigmaEoverE");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV8_cbmean_Map,      "energySCEleJoshEleSemiParam7TeVtrainV8cbmean");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV8_sigma_Map,       "energySCEleJoshEleSemiParam7TeVtrainV8sigma");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV8_alpha1_Map,      "energySCEleJoshEleSemiParam7TeVtrainV8alpha1");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV8_gamma1_Map,      "energySCEleJoshEleSemiParam7TeVtrainV8gamma1");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV8_alpha2_Map,      "energySCEleJoshEleSemiParam7TeVtrainV8alpha2");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV8_gamma2_Map,      "energySCEleJoshEleSemiParam7TeVtrainV8gamma2");
  iEvent.put(energySCEleJoshEleSemiParam7TeVtrainV8_pdfval_Map,      "energySCEleJoshEleSemiParam7TeVtrainV8pdfval");

  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV8_ecorr_Map,       "energySCEleJoshPhoSemiParam7TeVtrainV8ecorr");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV8_sigmaEoverE_Map, "energySCEleJoshPhoSemiParam7TeVtrainV8sigmaEoverE");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV8_cbmean_Map,      "energySCEleJoshPhoSemiParam7TeVtrainV8cbmean");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV8_sigma_Map,       "energySCEleJoshPhoSemiParam7TeVtrainV8sigma");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV8_alpha1_Map,      "energySCEleJoshPhoSemiParam7TeVtrainV8alpha1");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV8_gamma1_Map,      "energySCEleJoshPhoSemiParam7TeVtrainV8gamma1");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV8_alpha2_Map,      "energySCEleJoshPhoSemiParam7TeVtrainV8alpha2");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV8_gamma2_Map,      "energySCEleJoshPhoSemiParam7TeVtrainV8gamma2");
  iEvent.put(energySCEleJoshPhoSemiParam7TeVtrainV8_pdfval_Map,      "energySCEleJoshPhoSemiParam7TeVtrainV8pdfval");
  
}

// ------------ method called once each job just before starting event loop  ------------
void 
EleNewEnergiesProducer::beginJob()
{
  if (!corV4_ele.IsInitialized()) {
    std::cout << "[STATUS] Initializing V4 regrEle: " << regrEleJoshV4_SemiParamFile << std::endl;
    corV4_ele.Initialize(regrEleJoshV4_SemiParamFile, 4); //"/afs/cern.ch/user/b/bendavid/CMSSWhgg/CMSSW_5_3_11_patch5/src/HiggsAnalysis/GBRLikelihoodEGTools/data/regweights_v4_forest_ph.root");
  }
  
  if (!corV5_ele.IsInitialized()) {
    std::cout << "[STATUS] Initializing V5 regrEle: " << regrEleJoshV5_SemiParamFile <<std::endl;
    corV5_ele.Initialize(regrEleJoshV5_SemiParamFile, 5); //"/afs/cern.ch/user/b/bendavid/CMSSWhgg/CMSSW_5_3_11_patch5/src/HiggsAnalysis/GBRLikelihoodEGTools/data/regweights_v5_forest_ph.root");
  }

  if (!corV4_pho.IsInitialized()) {
    std::cout << "[STATUS] Initializing V4 regrPho: " << regrPhoJoshV4_SemiParamFile <<std::endl;
    corV4_pho.Initialize(regrPhoJoshV4_SemiParamFile, 4); //"/afs/cern.ch/user/b/bendavid/CMSSWhgg/CMSSW_5_3_11_patch5/src/HiggsAnalysis/GBRLikelihoodEGTools/data/regweights_v4_forest_ph.root");
  }
  
  if (!corV5_pho.IsInitialized()) {
    std::cout << "[STATUS] Initializing V5 regrPho: " << regrPhoJoshV5_SemiParamFile <<std::endl;
    corV5_pho.Initialize(regrPhoJoshV5_SemiParamFile, 5); //"/afs/cern.ch/user/b/bendavid/CMSSWhgg/CMSSW_5_3_11_patch5/src/HiggsAnalysis/GBRLikelihoodEGTools/data/regweights_v5_forest_ph.root");
  }

  if (!corV6_ele.IsInitialized()) {
    std::cout << "[STATUS] Initializing V6 regrEle: " << regrEleJoshV6_SemiParamFile << std::endl;
    corV6_ele.Initialize(regrEleJoshV6_SemiParamFile, 6); 
  }
  if (!corV6_pho.IsInitialized()) {
    std::cout << "[STATUS] Initializing V6 regrPho: " << regrPhoJoshV6_SemiParamFile << std::endl;
    corV6_pho.Initialize(regrPhoJoshV6_SemiParamFile, 6); 
  }

  if (!corV7_ele.IsInitialized()) {
    std::cout << "[STATUS] Initializing V7 regrEle: " << regrEleJoshV7_SemiParamFile << std::endl;
    corV7_ele.Initialize(regrEleJoshV7_SemiParamFile, 7); 
  }
  if (!corV7_pho.IsInitialized()) {
    std::cout << "[STATUS] Initializing V7 regrPho: " << regrPhoJoshV7_SemiParamFile << std::endl;
    corV7_pho.Initialize(regrPhoJoshV7_SemiParamFile, 7); 
  }

  if (!corV8_ele.IsInitialized()) {
    std::cout << "[STATUS] Initializing V8 regrEle: " << regrEleJoshV8_SemiParamFile << std::endl;
    corV8_ele.Initialize(regrEleJoshV8_SemiParamFile, 8); 
  }
  if (!corV8_pho.IsInitialized()) {
    std::cout << "[STATUS] Initializing V8 regrPho: " << regrPhoJoshV8_SemiParamFile << std::endl;
    corV8_pho.Initialize(regrPhoJoshV8_SemiParamFile, 8); 
  }


  if (!cor7TeVtrainV6_ele.IsInitialized()) {
    std::cout << "[STATUS] Initializing V6 regrEle: " << regrEleJoshV6_SemiParam7TeVtrainFile << std::endl;
    cor7TeVtrainV6_ele.Initialize(regrEleJoshV6_SemiParam7TeVtrainFile, 6); 
  }
  if (!cor7TeVtrainV6_pho.IsInitialized()) {
    std::cout << "[STATUS] Initializing V6 regrPho: " << regrPhoJoshV6_SemiParam7TeVtrainFile << std::endl;
    cor7TeVtrainV6_pho.Initialize(regrPhoJoshV6_SemiParam7TeVtrainFile, 6); 
  }

  if (!cor7TeVtrainV7_ele.IsInitialized()) {
    std::cout << "[STATUS] Initializing V7 regrEle: " << regrEleJoshV7_SemiParam7TeVtrainFile << std::endl;
    cor7TeVtrainV7_ele.Initialize(regrEleJoshV7_SemiParam7TeVtrainFile, 7); 
  }
  if (!cor7TeVtrainV7_pho.IsInitialized()) {
    std::cout << "[STATUS] Initializing V7 regrPho: " << regrPhoJoshV7_SemiParam7TeVtrainFile << std::endl;
    cor7TeVtrainV7_pho.Initialize(regrPhoJoshV7_SemiParam7TeVtrainFile, 7); 
  }

  if (!cor7TeVtrainV8_ele.IsInitialized()) {
    std::cout << "[STATUS] Initializing V8 regrEle: " << regrEleJoshV8_SemiParam7TeVtrainFile << std::endl;
    cor7TeVtrainV8_ele.Initialize(regrEleJoshV8_SemiParam7TeVtrainFile, 8); 
  }
  if (!cor7TeVtrainV8_pho.IsInitialized()) {
    std::cout << "[STATUS] Initializing V8 regrPho: " << regrPhoJoshV8_SemiParam7TeVtrainFile << std::endl;
    cor7TeVtrainV8_pho.Initialize(regrPhoJoshV8_SemiParam7TeVtrainFile, 8); 
  }

}

// ------------ method called once each job just after ending the event loop  ------------
void 
EleNewEnergiesProducer::endJob() {
}

// ------------ method called when starting to processes a run  ------------
void 
EleNewEnergiesProducer::beginRun(edm::Run&, edm::EventSetup const& iSetup)
{
  bool useDB=false;
  // Initialization of Josh regression: electron-tuned and photon-tuned regression
  //std::cout << "[DEBUG] Initialyze Josh's ele regression: " << regrEleFile <<  std::endl;
  josh_Ele.Initialize(iSetup, regrEleFile);
  //std::cout << "[DEBUG] Initialyze Josh's pho regression" << regrPhoFile <<  std::endl;
  josh_Pho.Initialize(iSetup, regrPhoFile);
  //std::cout << "[DEBUG] Initialyze Emanuele's ele regression" << std::endl;
  fra_Ele.Initialize(iSetup, regrEleFile_fra,useDB,ptSplit);


}

// ------------ method called when ending the processing of a run  ------------
void 
EleNewEnergiesProducer::endRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
EleNewEnergiesProducer::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
EleNewEnergiesProducer::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
EleNewEnergiesProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(EleNewEnergiesProducer);
