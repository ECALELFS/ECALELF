// -*- C++ -*-
//
// Package:    EleSelectionProducers
// Class:      EleSelectionProducers
// 
/**\class EleSelectionProducers EleSelectionProducers.cc Calibration/EleSelectionProducers/src/EleSelectionProducers.cc

 */
//
// Original Author:  Shervin Nourbakhsh,40 1-B24,+41227671643,
//         Created:  Tue Jul 17 20:57:01 CEST 2012
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

#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "Calibration/ZNtupleDumper/interface/SimpleCutBasedElectronIDSelectionFunctor.h"

#include "DataFormats/RecoCandidate/interface/IsoDeposit.h"

//
// class declaration
//

class EleSelectionProducers : public edm::EDProducer {

  //  typedef pat::strbitset SelectionValue_t;
  typedef float SelectionValue_t;
  typedef edm::ValueMap<SelectionValue_t> SelectionMap;
  typedef std::vector< edm::Handle< edm::ValueMap<double> > > IsoDepositValsHandles_t;

public:
  explicit EleSelectionProducers(const edm::ParameterSet&);
  ~EleSelectionProducers();

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
  edm::Handle<std::vector<reco::GsfElectron> > electronsHandle;
  edm::Handle<reco::ConversionCollection> conversionsHandle;
  edm::Handle<reco::BeamSpot> bsHandle;
  edm::Handle<reco::VertexCollection> vertexHandle;
  edm::Handle< edm::ValueMap<double> > chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle;
  edm::Handle<double> rhoHandle;


  /// input tag for electrons
  edm::InputTag electronsTAG;
  // conversions
  edm::InputTag conversionsProducerTAG;
  edm::InputTag BeamSpotTAG;
  edm::InputTag VertexTAG;
  // isolation
  edm::InputTag chIsoValsTAG, emIsoValsTAG, nhIsoValsTAG;
  /// input rho
  edm::InputTag rhoTAG;


  SimpleCutBasedElectronIDSelectionFunctor fiducial_selector;
  SimpleCutBasedElectronIDSelectionFunctor WP70_PU_selector;
  SimpleCutBasedElectronIDSelectionFunctor WP80_PU_selector;
  SimpleCutBasedElectronIDSelectionFunctor WP90_PU_selector;
  SimpleCutBasedElectronIDSelectionFunctor loose_selector;
  SimpleCutBasedElectronIDSelectionFunctor medium_selector;
  SimpleCutBasedElectronIDSelectionFunctor tight_selector;
  SimpleCutBasedElectronIDSelectionFunctor loose25nsRun2_selector;
  SimpleCutBasedElectronIDSelectionFunctor medium25nsRun2_selector;
  SimpleCutBasedElectronIDSelectionFunctor tight25nsRun2_selector;
  SimpleCutBasedElectronIDSelectionFunctor loose50nsRun2_selector;
  SimpleCutBasedElectronIDSelectionFunctor medium50nsRun2_selector;
  SimpleCutBasedElectronIDSelectionFunctor tight50nsRun2_selector;
  SimpleCutBasedElectronIDSelectionFunctor tightElectronStream_selector;

};


EleSelectionProducers::EleSelectionProducers(const edm::ParameterSet& iConfig):
  electronsTAG(iConfig.getParameter<edm::InputTag>("electronCollection")),
  conversionsProducerTAG(iConfig.getParameter<edm::InputTag>("conversionCollection")),
  BeamSpotTAG(iConfig.getParameter<edm::InputTag>("BeamSpotCollection")),
  VertexTAG(iConfig.getParameter<edm::InputTag>("vertexCollection")),
  chIsoValsTAG(iConfig.getParameter<edm::InputTag>("chIsoVals")),
  emIsoValsTAG(iConfig.getParameter<edm::InputTag>("emIsoVals")),
  nhIsoValsTAG(iConfig.getParameter<edm::InputTag>("nhIsoVals")),
  rhoTAG(iConfig.getParameter<edm::InputTag>("rhoFastJet")),

  fiducial_selector("fiducial", electronsHandle, conversionsHandle, bsHandle, vertexHandle,
		    chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
  WP70_PU_selector("WP70PU", electronsHandle, conversionsHandle, bsHandle, vertexHandle,
		   chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
  WP80_PU_selector("WP80PU", electronsHandle, conversionsHandle, bsHandle, vertexHandle,
		   chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
  WP90_PU_selector("WP90PU", electronsHandle, conversionsHandle, bsHandle, vertexHandle,
		   chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
  loose_selector("loose", electronsHandle, conversionsHandle, bsHandle, vertexHandle,
		 chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
  medium_selector("medium", electronsHandle, conversionsHandle, bsHandle, vertexHandle,
		  chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
  tight_selector("tight", electronsHandle, conversionsHandle, bsHandle, vertexHandle,
		 chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
  loose25nsRun2_selector("loose25nsRun2", electronsHandle, conversionsHandle, bsHandle, vertexHandle,
		 chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
  medium25nsRun2_selector("medium25nsRun2", electronsHandle, conversionsHandle, bsHandle, vertexHandle,
		  chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
  tight25nsRun2_selector("tight25nsRun2", electronsHandle, conversionsHandle, bsHandle, vertexHandle,
		 chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
  loose50nsRun2_selector("loose50nsRun2", electronsHandle, conversionsHandle, bsHandle, vertexHandle,
		 chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
  medium50nsRun2_selector("medium50nsRun2", electronsHandle, conversionsHandle, bsHandle, vertexHandle,
		  chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
  tight50nsRun2_selector("tight50nsRun2", electronsHandle, conversionsHandle, bsHandle, vertexHandle,
			 chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
  tightElectronStream_selector("tightElectronStream", electronsHandle, conversionsHandle, bsHandle, vertexHandle,
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
  produces< SelectionMap >("WP70PU");
  produces< SelectionMap >("WP80PU");
  produces< SelectionMap >("WP90PU");
  produces< SelectionMap >("loose");
  produces< SelectionMap >("medium");
  produces< SelectionMap >("tight");
  produces< SelectionMap >("loose25nsRun2");
  produces< SelectionMap >("medium25nsRun2");
  produces< SelectionMap >("tight25nsRun2");
  produces< SelectionMap >("loose50nsRun2");
  produces< SelectionMap >("medium50nsRun2");
  produces< SelectionMap >("tight50nsRun2");
  produces< SelectionMap >("tightElectronStream");

  //now do what ever other initialization is needed
  
}


EleSelectionProducers::~EleSelectionProducers()
{
 
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void EleSelectionProducers::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  std::vector<SelectionValue_t>  fiducial_vec;
  std::vector<SelectionValue_t>  WP70_PU_vec;
  std::vector<SelectionValue_t>  WP80_PU_vec;
  std::vector<SelectionValue_t>  WP90_PU_vec;
  std::auto_ptr<SelectionMap> fiducialMap(new SelectionMap());
  std::auto_ptr<SelectionMap> WP70_PUMap(new SelectionMap());
  std::auto_ptr<SelectionMap> WP80_PUMap(new SelectionMap());
  std::auto_ptr<SelectionMap> WP90_PUMap(new SelectionMap());
  std::vector<SelectionValue_t>  loose_vec;
  std::vector<SelectionValue_t>  medium_vec;
  std::vector<SelectionValue_t>  tight_vec;
  std::auto_ptr<SelectionMap> looseMap(new SelectionMap());
  std::auto_ptr<SelectionMap> mediumMap(new SelectionMap());
  std::auto_ptr<SelectionMap> tightMap(new SelectionMap());
  std::vector<SelectionValue_t>  loose25nsRun2_vec;
  std::vector<SelectionValue_t>  medium25nsRun2_vec;
  std::vector<SelectionValue_t>  tight25nsRun2_vec;
  std::auto_ptr<SelectionMap> looseMap_run2_25ns(new SelectionMap());
  std::auto_ptr<SelectionMap> mediumMap_run2_25ns(new SelectionMap());
  std::auto_ptr<SelectionMap> tightMap_run2_25ns(new SelectionMap());
  std::vector<SelectionValue_t>  loose50nsRun2_vec;
  std::vector<SelectionValue_t>  medium50nsRun2_vec;
  std::vector<SelectionValue_t>  tight50nsRun2_vec;
  std::auto_ptr<SelectionMap> looseMap50nsRun2(new SelectionMap());
  std::auto_ptr<SelectionMap> mediumMap50nsRun2(new SelectionMap());
  std::auto_ptr<SelectionMap> tightMap50nsRun2(new SelectionMap());
  std::vector<SelectionValue_t>  tightElectronStream_vec;
  std::auto_ptr<SelectionMap> tightElectronStreamMap(new SelectionMap());


  //------------------------------ ELECTRON
  iEvent.getByLabel(electronsTAG, electronsHandle);
  //if(!electronsHandle.isValid()){
  //  std::cerr << "[ERROR] electron collection not found" << std::endl;
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
  std::cout << "[DEBUG] Starting loop over electrons" << std::endl;
#endif
  for(reco::GsfElectronCollection::const_iterator ele_itr = (electronsHandle)->begin(); 
      ele_itr != (electronsHandle)->end(); ele_itr++){
    const reco::GsfElectronRef eleRef(electronsHandle, ele_itr-electronsHandle->begin());

    // the new tree has one event per each electron
    pat::strbitset fiducial_ret;
    pat::strbitset WP70_PU_ret;
    pat::strbitset WP80_PU_ret;
    pat::strbitset WP90_PU_ret;
    pat::strbitset loose_ret;
    pat::strbitset medium_ret;
    pat::strbitset tight_ret;
    pat::strbitset loose25nsRun2_ret;
    pat::strbitset medium25nsRun2_ret;
    pat::strbitset tight25nsRun2_ret;
    pat::strbitset loose50nsRun2_ret;
    pat::strbitset medium50nsRun2_ret;
    pat::strbitset tight50nsRun2_ret;
    pat::strbitset tightElectronStream_ret;


    fiducial_selector(eleRef, fiducial_ret);
    fiducial_vec.push_back(fiducial_selector.result());

    WP70_PU_selector(eleRef, WP70_PU_ret);
    WP80_PU_selector(eleRef, WP80_PU_ret);
    WP90_PU_selector(eleRef, WP90_PU_ret);
     
    WP70_PU_vec.push_back(WP70_PU_selector.result()); // result gives a float
    WP80_PU_vec.push_back(WP80_PU_selector.result()); // result gives a float
    WP90_PU_vec.push_back(WP90_PU_selector.result()); // result gives a float

    WP70_PU_selector(eleRef, WP70_PU_ret);
    WP80_PU_selector(eleRef, WP80_PU_ret);
    WP90_PU_selector(eleRef, WP90_PU_ret);

    loose_selector(eleRef, loose_ret);
    loose_vec.push_back(loose_selector.result());
    medium_selector(eleRef, medium_ret);
    medium_vec.push_back(medium_selector.result());
    tight_selector(eleRef, tight_ret);
    tight_vec.push_back(tight_selector.result());

    loose25nsRun2_selector(eleRef, loose25nsRun2_ret);
    loose25nsRun2_vec.push_back(loose25nsRun2_selector.result());
    medium25nsRun2_selector(eleRef, medium25nsRun2_ret);
    medium25nsRun2_vec.push_back(medium25nsRun2_selector.result());
    tight25nsRun2_selector(eleRef, tight25nsRun2_ret);
    tight25nsRun2_vec.push_back(tight25nsRun2_selector.result());

    loose50nsRun2_selector(eleRef, loose50nsRun2_ret);
    loose50nsRun2_vec.push_back(loose50nsRun2_selector.result());
    medium50nsRun2_selector(eleRef, medium50nsRun2_ret);
    medium50nsRun2_vec.push_back(medium50nsRun2_selector.result());
    tight50nsRun2_selector(eleRef, tight50nsRun2_ret);
    tight50nsRun2_vec.push_back(tight50nsRun2_selector.result());

    tightElectronStream_selector(eleRef, tightElectronStream_ret);
    tightElectronStream_vec.push_back(tightElectronStream_selector.result());

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

    if(((bool)tight25nsRun2_selector.result())){
      if(!(bool) medium25nsRun2_selector.result() || !(bool) loose25nsRun2_selector.result()){
	edm::LogError("Incoherent selection") << "passing tight but not medium or loose for run2: 25ns";
	exit (1);
      }
    }

    if(((bool)medium25nsRun2_selector.result())){
      if( !(bool) loose25nsRun2_selector.result()){
	edm::LogError("Incoherent selection") << "passing medium but not loose for run2: 25ns";
	exit (1);
      }
    }
    /*
    if(((bool)tight50nsRun2_selector.result())){
      if(!(bool) medium50nsRun2_selector.result() || !(bool) loose50nsRun2_selector.result()){
	edm::LogError("Incoherent selection") << "passing tight but not medium or loose for run2: 50ns";
	exit (1);
      }
    }

    if(((bool)medium50nsRun2_selector.result())){
      if( !(bool) loose50nsRun2_selector.result()){
	edm::LogError("Incoherent selection") << "passing medium but not loose for run2: 50ns";
	exit (1);
      }
    }
    */    
    //     WP80_PU_vec.push_back((SelectionValue_t)WP80_PU_selector.bitMask());
    //     WP90_PU_vec.push_back((SelectionValue_t)WP90_PU_selector.bitMask());
#ifdef DEBUG
    std::cout << "[DEBUG] WP80 ret=" << WP80_PU_selector.bitMask() << std::endl;
    std::cout << "[DEBUG] WP80 ret= (float)" << (SelectionValue_t) WP80_PU_selector.bitMask() << std::endl;
    std::cout << "[DEBUG] loose ret=" << loose_selector.bitMask() << std::endl;
    std::cout << "[DEBUG] loose ret= (float)" << (SelectionValue_t) loose_selector.bitMask() << std::endl;

#endif
  }

  //prepare product 
  // declare the filler of the ValueMap
  SelectionMap::Filler fiducial_filler(*fiducialMap);
  SelectionMap::Filler WP70_PU_filler(*WP70_PUMap);
  SelectionMap::Filler WP80_PU_filler(*WP80_PUMap);
  SelectionMap::Filler WP90_PU_filler(*WP90_PUMap);
  SelectionMap::Filler loose_filler(*looseMap);
  SelectionMap::Filler medium_filler(*mediumMap);
  SelectionMap::Filler tight_filler(*tightMap);
  SelectionMap::Filler loose25nsRun2_filler(*looseMap_run2_25ns);
  SelectionMap::Filler medium25nsRun2_filler(*mediumMap_run2_25ns);
  SelectionMap::Filler tight25nsRun2_filler(*tightMap_run2_25ns);
  SelectionMap::Filler loose50nsRun2_filler(*looseMap50nsRun2);
  SelectionMap::Filler medium50nsRun2_filler(*mediumMap50nsRun2);
  SelectionMap::Filler tight50nsRun2_filler(*tightMap50nsRun2);
  SelectionMap::Filler tightElectronStream_filler(*tightElectronStreamMap);

  //fill and insert valuemap
  fiducial_filler.insert(electronsHandle,fiducial_vec.begin(),fiducial_vec.end());
  WP70_PU_filler.insert(electronsHandle,WP70_PU_vec.begin(),WP70_PU_vec.end());
  WP80_PU_filler.insert(electronsHandle,WP80_PU_vec.begin(),WP80_PU_vec.end());
  WP90_PU_filler.insert(electronsHandle,WP90_PU_vec.begin(),WP90_PU_vec.end());
  loose_filler.insert(electronsHandle,loose_vec.begin(),loose_vec.end());
  medium_filler.insert(electronsHandle,medium_vec.begin(),medium_vec.end());
  tight_filler.insert(electronsHandle,tight_vec.begin(),tight_vec.end());
  loose25nsRun2_filler.insert(electronsHandle,loose25nsRun2_vec.begin(),loose25nsRun2_vec.end());
  medium25nsRun2_filler.insert(electronsHandle,medium25nsRun2_vec.begin(),medium25nsRun2_vec.end());
  tight25nsRun2_filler.insert(electronsHandle,tight25nsRun2_vec.begin(),tight25nsRun2_vec.end());
  loose50nsRun2_filler.insert(electronsHandle,loose50nsRun2_vec.begin(),loose50nsRun2_vec.end());
  medium50nsRun2_filler.insert(electronsHandle,medium50nsRun2_vec.begin(),medium50nsRun2_vec.end());
  tight50nsRun2_filler.insert(electronsHandle,tight50nsRun2_vec.begin(),tight50nsRun2_vec.end());
  tightElectronStream_filler.insert(electronsHandle,tightElectronStream_vec.begin(),tightElectronStream_vec.end());

  
  fiducial_filler.fill();
  WP70_PU_filler.fill();
  WP80_PU_filler.fill();
  WP90_PU_filler.fill();
  loose_filler.fill();
  medium_filler.fill();
  tight_filler.fill();
  loose25nsRun2_filler.fill();
  medium25nsRun2_filler.fill();
  tight25nsRun2_filler.fill();
  loose50nsRun2_filler.fill();
  medium50nsRun2_filler.fill();
  tight50nsRun2_filler.fill();
  tightElectronStream_filler.fill();
    

  //------------------------------
  // put the ValueMap in the event
  iEvent.put(fiducialMap, "fiducial");
  iEvent.put(WP70_PUMap, "WP70PU");
  iEvent.put(WP80_PUMap, "WP80PU");
  iEvent.put(WP90_PUMap, "WP90PU");
  iEvent.put(looseMap, "loose");
  iEvent.put(mediumMap, "medium");
  iEvent.put(tightMap, "tight");
  iEvent.put(looseMap_run2_25ns, "loose25nsRun2");
  iEvent.put(mediumMap_run2_25ns, "medium25nsRun2");
  iEvent.put(tightMap_run2_25ns, "tight25nsRun2");
  iEvent.put(looseMap50nsRun2, "loose50nsRun2");
  iEvent.put(mediumMap50nsRun2, "medium50nsRun2");
  iEvent.put(tightMap50nsRun2, "tight50nsRun2");
  iEvent.put(tightElectronStreamMap, "tightElectronStream");
 
}

// ------------ method called once each job just before starting event loop  ------------
void 
EleSelectionProducers::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
EleSelectionProducers::endJob() {
}

// ------------ method called when starting to processes a run  ------------
void 
EleSelectionProducers::beginRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
EleSelectionProducers::endRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
EleSelectionProducers::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
EleSelectionProducers::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
EleSelectionProducers::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  //   desc.add<edm::InputTag>("electronCollection","gsfElectrons");
  //   desc.add<edm::InputTag>("rhoFastJet","kt6PFJetsForRhoCorrection:rho");
  //   desc.add<edm::InputTag>("vertexCollection","offlinePrimaryVertices");
  //   desc.add<edm::InputTag>("conversionCollection","allConversions");
  //   desc.add<edm::InputTag>("BeamSpotCollection","offlineBeamSpot");
  //   desc.add<edm::InputTag>("chIsoVals","elPFIsoValueCharged03PFIdPFIso");
  //   desc.add<edm::InputTag>("emIsoVals","elPFIsoValueGamma03PFIdPFIso");
  //   desc.add<edm::InputTag>("nhIsoVals","elPFIsoValueNeutral03PFIdPFIso");
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(EleSelectionProducers);
