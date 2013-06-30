// -*- C++ -*-
//
// Package:    EleNewEnergiesProducer
// Class:      EleNewEnergiesProducer
// 
/**\class EleNewEnergiesProducer EleNewEnergiesProducer.cc calibration/EleNewEnergiesProducer/src/EleNewEnergiesProducer.cc

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

//
// class declaration
//

class EleNewEnergiesProducer : public edm::EDProducer {

  typedef edm::ValueMap<float> NewEnergyMap;

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
  std::string regrEleFile; //weights
  std::string regrPhoFile; 
  std::string regrEleFile_fra; 

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

   std::vector<float>  energySCEleJoshEle;
   std::vector<float>  energySCEleJoshEleVar;
   std::vector<float>  energySCEleJoshPho;
   std::vector<float>  energySCEleJoshPhoVar;
   std::vector<float>  energyEleFra;
   std::vector<float>  energyEleFraVar;

   std::auto_ptr<NewEnergyMap> energySCEleJoshEleMap(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap> energySCEleJoshEleVarMap(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap> energySCEleJoshPhoMap(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap> energySCEleJoshPhoVarMap(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap> energyEleFraMap(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap> energyEleFraVarMap(new NewEnergyMap());

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

  //fill and insert valuemapv
  energySCEleJoshEle_filler.insert(electronsHandle,energySCEleJoshEle.begin(),energySCEleJoshEle.end());
  energySCEleJoshEleVar_filler.insert(electronsHandle,energySCEleJoshEleVar.begin(),energySCEleJoshEleVar.end());//fra
  energySCEleJoshPho_filler.insert(electronsHandle,energySCEleJoshPho.begin(),energySCEleJoshPho.end());
  energySCEleJoshPhoVar_filler.insert(electronsHandle,energySCEleJoshPhoVar.begin(),energySCEleJoshPhoVar.end());
  energyEleFra_filler.insert(electronsHandle,energyEleFra.begin(),energyEleFra.end());//fra anche qui
  energyEleFraVar_filler.insert(electronsHandle,energyEleFraVar.begin(),energyEleFraVar.end());//fra anche qui
  energySCEleJoshEle_filler.fill();
  energySCEleJoshEleVar_filler.fill();//fra
  energySCEleJoshPho_filler.fill();
  energySCEleJoshPhoVar_filler.fill();
  energyEleFra_filler.fill();
  energyEleFraVar_filler.fill();//fra

  //------------------------------
  // put the ValueMap in the event
  iEvent.put(energySCEleJoshPhoMap, "energySCEleJoshPho");
  iEvent.put(energySCEleJoshPhoVarMap, "energySCEleJoshPhoVar");//fra
  iEvent.put(energySCEleJoshEleMap, "energySCEleJoshEle");
  iEvent.put(energySCEleJoshEleVarMap, "energySCEleJoshEleVar");//fra
  iEvent.put(energyEleFraMap, "energyEleFra");
  iEvent.put(energyEleFraVarMap, "energyEleFraVar");//fra

}

// ------------ method called once each job just before starting event loop  ------------
void 
EleNewEnergiesProducer::beginJob()
{
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
