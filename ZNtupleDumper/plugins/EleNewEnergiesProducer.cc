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
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"

class EleNewEnergiesProducer : public edm::EDProducer {

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

#ifdef CMSSW_7_2_X
  edm::EDGetTokenT <EcalRecHitCollection> recHitCollectionEBTAG;
  edm::EDGetTokenT <EcalRecHitCollection> recHitCollectionEETAG;
#else
  edm::InputTag recHitCollectionEBTAG;
  edm::InputTag recHitCollectionEETAG;
#endif

private:

  edm::Handle<edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit> > > recHitCollectionEBHandle;
  edm::Handle<edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit> > > recHitCollectionEEHandle;
};

EleNewEnergiesProducer::EleNewEnergiesProducer(const edm::ParameterSet& iConfig):
#ifdef CMSSW_7_2_X
    recHitCollectionEBTAG(consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>( "recHitCollectionEB" ))),
    recHitCollectionEETAG(consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>( "recHitCollectionEE" )))
#else
    recHitCollectionEBTAG(iConfig.getParameter<edm::InputTag>("recHitCollectionEB")),
    recHitCollectionEETAG(iConfig.getParameter<edm::InputTag>("recHitCollectionEE"))
#endif
  {
  }


EleNewEnergiesProducer::~EleNewEnergiesProducer()
  {
    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)
  }

void
EleNewEnergiesProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
  {
    using namespace edm;

    //------------------------------ RECHIT
    // iEvent.getByLabel(recHitCollectionEBTAG, recHitCollectionEBHandle);
    // iEvent.getByLabel(recHitCollectionEETAG, recHitCollectionEEHandle);

    EcalClusterLazyTools lazyTools(iEvent, iSetup,
				   recHitCollectionEBTAG,
				   recHitCollectionEETAG); 
  }

    // ------------ method called once each job just before starting event loop ------------
void
  EleNewEnergiesProducer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop ------------
void
EleNewEnergiesProducer::endJob() 
{
}

void
  EleNewEnergiesProducer::beginRun(edm::Run&, edm::EventSetup const& iSetup)
{
}

// ------------ method called when ending the processing of a run ------------
void
  EleNewEnergiesProducer::endRun(edm::Run&, edm::EventSetup const&)
{
}
// ------------ method called when starting to processes a luminosity block ------------
void
  EleNewEnergiesProducer::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}
// ------------ method called when ending the processing of a luminosity block ------------
void
  EleNewEnergiesProducer::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}
// ------------ method fills 'descriptions' with the allowed parameters for the module ------------
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
