// -*- C++ -*-
//
// Package:    EleSelectionProducers
// Class:      PhoSelectionProducers
// 
/**\class PhoSelectionProducers PhoSelectionProducers.cc Calibration/EleSelectionProducers/src/PhoSelectionProducers.cc

 */
//
// Original Author:  Luca Brianza
//       
// $Id$
//
//

//#define DEBUG
// system include files

#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "PhysicsTools/SelectorUtils/interface/strbitset.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "Calibration/EleSelectionProducers/interface/SimpleCutBasedPhotonIDSelectionFunctor.h"

#include "DataFormats/RecoCandidate/interface/IsoDeposit.h"

//
// class declaration
//

class PhoSelectionProducers : public edm::EDProducer {

  //  typedef pat::strbitset SelectionValue_t;
  typedef float SelectionValue_t;
  typedef edm::ValueMap<SelectionValue_t> SelectionMap;
  typedef std::vector< edm::Handle< edm::ValueMap<double> > > IsoDepositValsHandles_t;

public:
  explicit PhoSelectionProducers(const edm::ParameterSet&);
  ~PhoSelectionProducers();

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
private:
  edm::Handle<std::vector<reco::Photon> > photonsHandle;
  edm::Handle<reco::ConversionCollection> conversionsHandle;
  edm::Handle<reco::BeamSpot> bsHandle;
  edm::Handle<reco::VertexCollection> vertexHandle;
  edm::Handle< edm::ValueMap<double> > chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle;
  edm::Handle<double> rhoHandle;


  /// input tag for photons
  edm::InputTag photonsTAG;
  // conversions
  edm::InputTag conversionsProducerTAG;
  edm::InputTag BeamSpotTAG;
  edm::InputTag VertexTAG;
  // isolation
  edm::InputTag chIsoValsTAG, emIsoValsTAG, nhIsoValsTAG;
  /// input rho
  edm::InputTag rhoTAG;


  SimpleCutBasedPhotonIDSelectionFunctor fiducial_selector;
  SimpleCutBasedPhotonIDSelectionFunctor loose_selector;
  SimpleCutBasedPhotonIDSelectionFunctor medium_selector;
  SimpleCutBasedPhotonIDSelectionFunctor tight_selector;

};


PhoSelectionProducers::PhoSelectionProducers(const edm::ParameterSet& iConfig):
  photonsTAG(iConfig.getParameter<edm::InputTag>("photonCollection")),
  conversionsProducerTAG(iConfig.getParameter<edm::InputTag>("conversionCollection")),
  BeamSpotTAG(iConfig.getParameter<edm::InputTag>("BeamSpotCollection")),
  VertexTAG(iConfig.getParameter<edm::InputTag>("vertexCollection")),
  chIsoValsTAG(iConfig.getParameter<edm::InputTag>("chIsoVals")),
  emIsoValsTAG(iConfig.getParameter<edm::InputTag>("emIsoVals")),
  nhIsoValsTAG(iConfig.getParameter<edm::InputTag>("nhIsoVals")),
  rhoTAG(iConfig.getParameter<edm::InputTag>("rhoFastJet")),

  fiducial_selector("fiducial", photonsHandle, conversionsHandle, bsHandle, vertexHandle,
		    chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
  loose_selector("loose", photonsHandle, conversionsHandle, bsHandle, vertexHandle,
		 chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
  medium_selector("medium", photonsHandle, conversionsHandle, bsHandle, vertexHandle,
		  chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
  tight_selector("tight", photonsHandle, conversionsHandle, bsHandle, vertexHandle,
		 chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle)
{
  //register your products
  /* Examples
     produces<ExampleData2>();

     //if do put with a label
     produces<ExampleData2>("label");
 
     //if you want to put into the Run
     produces<ExampleData2,InRun>();
  */
  produces< SelectionMap >("fiducial");
  produces< SelectionMap >("loose");
  produces< SelectionMap >("medium");
  produces< SelectionMap >("tight");

  //now do what ever other initialization is needed
  
}


PhoSelectionProducers::~PhoSelectionProducers()
{
 
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void PhoSelectionProducers::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  std::vector<SelectionValue_t>  fiducial_vec;
  std::auto_ptr<SelectionMap> fiducialMap(new SelectionMap());
  std::vector<SelectionValue_t>  loose_vec;
  std::vector<SelectionValue_t>  medium_vec;
  std::vector<SelectionValue_t>  tight_vec;
  std::auto_ptr<SelectionMap> looseMap(new SelectionMap());
  std::auto_ptr<SelectionMap> mediumMap(new SelectionMap());
  std::auto_ptr<SelectionMap> tightMap(new SelectionMap());

  //------------------------------ PHOTON
  iEvent.getByLabel(photonsTAG, photonsHandle);
  //if(!photonsHandle.isValid()){
  //  std::cerr << "[ERROR] photon collection not found" << std::endl;
  //  return;
  //}
   
  //------------------------------ CONVERSIONS
  iEvent.getByLabel(conversionsProducerTAG, conversionsHandle);
  //   std::cout << conversionsHandle.isValid() << std::endl;
  iEvent.getByLabel(BeamSpotTAG, bsHandle);
  iEvent.getByLabel(VertexTAG, vertexHandle);
  //------------------------------ RHO
  iEvent.getByLabel(rhoTAG,rhoHandle);

  //------------------------------ ISO DEPOSITS
#ifdef CMSSW_7_2_X
#else
  iEvent.getByLabel(chIsoValsTAG, chIsoValsHandle);
  if(!chIsoValsHandle.isValid()){
    chIsoValsTAG=edm::InputTag(chIsoValsTAG.label().substr(0,chIsoValsTAG.label().find("PFIso",chIsoValsTAG.label().size()-6))+"Gsf", chIsoValsTAG.instance(), chIsoValsTAG.process());
    emIsoValsTAG=edm::InputTag(emIsoValsTAG.label().substr(0,emIsoValsTAG.label().find("PFIso",emIsoValsTAG.label().size()-6))+"Gsf", emIsoValsTAG.instance(), emIsoValsTAG.process());
    nhIsoValsTAG=edm::InputTag(nhIsoValsTAG.label().substr(0,nhIsoValsTAG.label().find("PFIso",nhIsoValsTAG.label().size()-6))+"Gsf", nhIsoValsTAG.instance(), nhIsoValsTAG.process());

    iEvent.getByLabel(chIsoValsTAG, chIsoValsHandle);
  }
  iEvent.getByLabel(emIsoValsTAG, emIsoValsHandle);  
  iEvent.getByLabel(nhIsoValsTAG, nhIsoValsHandle);
#endif  
   
#ifdef DEBUG
  std::cout << "[DEBUG] Starting loop over photons" << std::endl;
#endif
  for(reco::PhotonCollection::const_iterator ele_itr = (photonsHandle)->begin(); 
      ele_itr != (photonsHandle)->end(); ele_itr++){
    const reco::PhotonRef eleRef(photonsHandle, ele_itr-photonsHandle->begin());

    // the new tree has one event per each photon
    pat::strbitset fiducial_ret;
    pat::strbitset loose_ret;
    pat::strbitset medium_ret;
    pat::strbitset tight_ret;


    fiducial_selector(eleRef, fiducial_ret);
    fiducial_vec.push_back(fiducial_selector.result());


    loose_selector(eleRef, loose_ret);
    loose_vec.push_back(loose_selector.result());
    medium_selector(eleRef, medium_ret);
    medium_vec.push_back(medium_selector.result());
    tight_selector(eleRef, tight_ret);
    tight_vec.push_back(tight_selector.result());


    if(((bool)tight_selector.result())){
      if(!(bool) medium_selector.result() || !(bool) loose_selector.result()){
	edm::LogError("Incoherent selection") << "passing tight but not medium or loose";
	exit (1);
      }
    }

    if(((bool)medium_selector.result())){
      if( !(bool) loose_selector.result()){
	edm::LogError("Incoherent selection") << "passing medium but not loose";
	exit (1);
      }
    }
    
    
  }

  //prepare product 
  // declare the filler of the ValueMap
  SelectionMap::Filler fiducial_filler(*fiducialMap);
  SelectionMap::Filler loose_filler(*looseMap);
  SelectionMap::Filler medium_filler(*mediumMap);
  SelectionMap::Filler tight_filler(*tightMap);

  //fill and insert valuemap
  fiducial_filler.insert(photonsHandle,fiducial_vec.begin(),fiducial_vec.end());
  loose_filler.insert(photonsHandle,loose_vec.begin(),loose_vec.end());
  medium_filler.insert(photonsHandle,medium_vec.begin(),medium_vec.end());
  tight_filler.insert(photonsHandle,tight_vec.begin(),tight_vec.end());

  
  fiducial_filler.fill();
  loose_filler.fill();
  medium_filler.fill();
  tight_filler.fill();

  //------------------------------
  // put the ValueMap in the event
  iEvent.put(fiducialMap, "fiducial");
  iEvent.put(looseMap, "loose");
  iEvent.put(mediumMap, "medium");
  iEvent.put(tightMap, "tight");
 
}

// ------------ method called once each job just before starting event loop  ------------
void 
PhoSelectionProducers::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
PhoSelectionProducers::endJob() {
}

// ------------ method called when starting to processes a run  ------------
void 
PhoSelectionProducers::beginRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
PhoSelectionProducers::endRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
PhoSelectionProducers::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
PhoSelectionProducers::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
PhoSelectionProducers::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  //   desc.add<edm::InputTag>("photonCollection","Photons");
  //   desc.add<edm::InputTag>("rhoFastJet","kt6PFJetsForRhoCorrection:rho");
  //   desc.add<edm::InputTag>("vertexCollection","offlinePrimaryVertices");
  //   desc.add<edm::InputTag>("conversionCollection","allConversions");
  //   desc.add<edm::InputTag>("BeamSpotCollection","offlineBeamSpot");
  //   desc.add<edm::InputTag>("chIsoVals","phPFIsoValueCharged03PFIdPFIso");
  //   desc.add<edm::InputTag>("emIsoVals","phPFIsoValueGamma03PFIdPFIso");
  //   desc.add<edm::InputTag>("nhIsoVals","phPFIsoValueNeutral03PFIdPFIso");
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(PhoSelectionProducers);
