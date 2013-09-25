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
#include <TString.h>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
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
  edm::InputTag recHitCollectionEBTAG;
  edm::InputTag recHitCollectionEETAG;

  /// input rho
  edm::InputTag rhoTAG;

  edm::InputTag conversionsProducerTAG;

  std::string foutName;
  TString r9weightsFilename;
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
  EGEnergyCorrectorSemiParm corV4_ele;
  EGEnergyCorrectorSemiParm corV5_ele;
  EGEnergyCorrectorSemiParm corV4_pho;
  EGEnergyCorrectorSemiParm corV5_pho;


  std::string regrEleFile; //weights
  std::string regrPhoFile; 
  std::string regrEleFile_fra; 
  std::string regrEleJoshV4_SemiParamFile;
  std::string regrEleJoshV5_SemiParamFile;
  std::string regrPhoJoshV4_SemiParamFile;
  std::string regrPhoJoshV5_SemiParamFile;

  EGEnergyCorrectorSemiParm corV6_ele;
  EGEnergyCorrectorSemiParm corV6_pho;
  std::string regrEleJoshV6_SemiParamFile;
  std::string regrPhoJoshV6_SemiParamFile;
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
  recHitCollectionEBTAG(iConfig.getParameter<edm::InputTag>("recHitCollectionEB")),
  recHitCollectionEETAG(iConfig.getParameter<edm::InputTag>("recHitCollectionEE")),
  rhoTAG(iConfig.getParameter<edm::InputTag>("rhoFastJet")),
  conversionsProducerTAG(iConfig.getParameter<edm::InputTag>("conversionCollection")),
  //  foutName(iConfig.getParameter<std::string>("foutName")),
  regrEleFile(iConfig.getParameter<std::string>("regrEleFile")),
  regrPhoFile(iConfig.getParameter<std::string>("regrPhoFile")),
  regrEleFile_fra(iConfig.getParameter<std::string>("regrEleFile_fra")),
  regrEleJoshV4_SemiParamFile(iConfig.getParameter<std::string>("regrEleJoshV4_SemiParamFile")),
  regrEleJoshV5_SemiParamFile(iConfig.getParameter<std::string>("regrEleJoshV5_SemiParamFile")),
  regrPhoJoshV4_SemiParamFile(iConfig.getParameter<std::string>("regrPhoJoshV4_SemiParamFile")),
  regrPhoJoshV5_SemiParamFile(iConfig.getParameter<std::string>("regrPhoJoshV5_SemiParamFile")),

  regrEleJoshV6_SemiParamFile(iConfig.getParameter<std::string>("regrEleJoshV6_SemiParamFile")),
  regrPhoJoshV6_SemiParamFile(iConfig.getParameter<std::string>("regrPhoJoshV6_SemiParamFile")),

  ptSplit(iConfig.getParameter<bool>("ptSplit"))
//     r9weightsFilename(iConfig.getParameter<std::string>("r9weightsFile")),
//     puWeightFile(iConfig.getParameter<std::string>("puWeightFile")),
//     puWeights(),
//     oddEventFilter(iConfig.getParameter<bool>("oddEventFilter")),
//     scaleCorrections_(iConfig.getParameter<std::string>("correctionFileName"),iConfig.getParameter<std::string>("correctionType"))
  
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

  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV5ecorr");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV5sigma");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV5alpha1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV5gamma1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV5alpha2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV5gamma2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV5pdfval");

  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV4ecorr");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV4sigma");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV4alpha1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV4gamma1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV4alpha2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV4gamma2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV4pdfval");
					   
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV5ecorr");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV5sigma");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV5alpha1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV5gamma1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV5alpha2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV5gamma2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamV5pdfval");

  //------------------------------
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV6ecorr");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV6sigma");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV6alpha1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV6gamma1");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV6alpha2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV6gamma2");
  produces< NewEnergyMap >("energySCEleJoshEleSemiParamV6pdfval");

  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamVecorr");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamVsigma");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamValpha1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamVgamma1");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamValpha2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamVgamma2");
  produces< NewEnergyMap >("energySCEleJoshPhoSemiParamVpdfval");

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
   
#if REGRESSION == 3
   corEle = josh_Ele.CorrectedEnergyWithErrorV3(*ele_itr,*primaryVertexHandle, *rhoHandle, lazyTools,iSetup);
   corPho = josh_Pho.CorrectedEnergyWithErrorV3(*ele_itr,*primaryVertexHandle, *rhoHandle, lazyTools,iSetup);
#elif REGRESSION == 2  
  corEle = josh_Ele.CorrectedEnergyWithErrorV2(*ele_itr,*primaryVertexHandle,  lazyTools,iSetup);
  corPho = josh_Pho.CorrectedEnergyWithErrorV2(*ele_itr,*primaryVertexHandle,  lazyTools,iSetup);
#elif REGRESSION == 1
  corEle = josh_Ele.CorrectedEnergyWithError(*ele_itr,*primaryVertexHandle,  lazyTools,iSetup);
  corPho = josh_Pho.CorrectedEnergyWithError(*ele_itr,*primaryVertexHandle,  lazyTools,iSetup);
#endif
  corEle_fra = fra_Ele.CorrectedEnergyWithErrorTracker(*ele_itr,*primaryVertexHandle, *rhoHandle, lazyTools,iSetup,ptSplit);


    double ecor, sigma, alpha1, n1, alpha2, n2, pdfval;

    
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
  
}

// ------------ method called once each job just before starting event loop  ------------
void 
EleNewEnergiesProducer::beginJob()
{
  if (!corV4_ele.IsInitialized()) {
    std::cout << "[STATUS] Initializing V4 regrEle: " << regrEleJoshV4_SemiParamFile << std::endl;
    corV4_ele.Initialize(regrEleJoshV4_SemiParamFile); //"/afs/cern.ch/user/b/bendavid/CMSSWhgg/CMSSW_5_3_11_patch5/src/HiggsAnalysis/GBRLikelihoodEGTools/data/regweights_v4_forest_ph.root");
  }
  
  if (!corV5_ele.IsInitialized()) {
    std::cout << "[STATUS] Initializing V5 regrEle: " << regrEleJoshV5_SemiParamFile <<std::endl;
    corV5_ele.Initialize(regrEleJoshV5_SemiParamFile); //"/afs/cern.ch/user/b/bendavid/CMSSWhgg/CMSSW_5_3_11_patch5/src/HiggsAnalysis/GBRLikelihoodEGTools/data/regweights_v5_forest_ph.root");
  }

  if (!corV4_pho.IsInitialized()) {
    std::cout << "[STATUS] Initializing V4 regrPho: " << regrPhoJoshV4_SemiParamFile <<std::endl;
    corV4_pho.Initialize(regrPhoJoshV4_SemiParamFile); //"/afs/cern.ch/user/b/bendavid/CMSSWhgg/CMSSW_5_3_11_patch5/src/HiggsAnalysis/GBRLikelihoodEGTools/data/regweights_v4_forest_ph.root");
  }
  
  if (!corV5_pho.IsInitialized()) {
    std::cout << "[STATUS] Initializing V5 regrPho: " << regrPhoJoshV5_SemiParamFile <<std::endl;
    corV5_pho.Initialize(regrPhoJoshV5_SemiParamFile); //"/afs/cern.ch/user/b/bendavid/CMSSWhgg/CMSSW_5_3_11_patch5/src/HiggsAnalysis/GBRLikelihoodEGTools/data/regweights_v5_forest_ph.root");
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
