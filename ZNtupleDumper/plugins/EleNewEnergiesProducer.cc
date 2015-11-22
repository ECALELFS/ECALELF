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

// system include files
#include <memory>
#include <cassert>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

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


#include "RecoEgamma/EgammaTools/interface/SCEnergyCorrectorSemiParm.h"


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
	  virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

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

	SCEnergyCorrectorSemiParm mustache_regr_; 
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
  electronsTAG(iConfig.getParameter<edm::InputTag>("electronCollection"))
{
	std::cout << electronsTAG << std::endl;
  // this name are hard coded, should be put in the cfi
  produces< NewEnergyMap >("energySCEleMust");
  produces< NewEnergyMap >("energySCEleMustVar");  

 //now do what ever other initialization is needed
  edm::ConsumesCollector iC = consumesCollector();
  mustache_regr_.setTokens(iC, iConfig.getParameterSet("scEnergyCorrectorSemiParm"));
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
   mustache_regr_.setEvent(iEvent);

   std::vector<v_t>  energySC_must;
   std::vector<v_t>  energySC_mustVar;

   std::auto_ptr<NewEnergyMap> energySC_mustMap(new NewEnergyMap());
   std::auto_ptr<NewEnergyMap> energySC_mustVarMap(new NewEnergyMap());

   //------------------------------ ELECTRON
   iEvent.getByLabel(electronsTAG, electronsHandle);
   
   //iSetup.get<CaloTopologyRecord>().get(topologyHandle);


   for(reco::GsfElectronCollection::const_iterator ele_itr = (electronsHandle)->begin(); 
	   ele_itr != (electronsHandle)->end(); ele_itr++){

	   std::pair<double,double> corEle = std::make_pair<double,double>(-1,-1);
	   if(ele_itr->ecalDriven()){
		   //assert(ele_itr->parentSuperCluster().isNonnull() && ele_itr->parentSuperCluster()->seed().isNonnull()); //failing for 1/14000 events in MC!
		   if(ele_itr->parentSuperCluster().isNonnull())
			   corEle = mustache_regr_.GetCorrections(*(ele_itr->parentSuperCluster()));
		   else edm::LogWarning("energyProducer") << "EcalDriven electron withouth parentSC";
	   }
	   
	   //fill the vector with the energies
	   energySC_must.push_back(corEle.first);
	   energySC_mustVar.push_back(corEle.second);
   }

  //prepare product 
  // declare the filler of the ValueMap
  NewEnergyMap::Filler energySC_must_filler(   *energySC_mustMap);
  NewEnergyMap::Filler energySC_mustVar_filler(*energySC_mustVarMap);

  //fill and insert valuemapv
  energySC_must_filler.insert(electronsHandle,energySC_must.begin(),energySC_must.end());
  energySC_mustVar_filler.insert(electronsHandle,energySC_mustVar.begin(),energySC_mustVar.end());//fra

  energySC_must_filler.fill();
  energySC_mustVar_filler.fill();//fra

  //------------------------------
  // put the ValueMap in the event
  iEvent.put(energySC_mustMap, "energySCEleMust");
  iEvent.put(energySC_mustVarMap, "energySCEleMustVar");
  
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
}

// ------------ method called when ending the processing of a run  ------------
void 
EleNewEnergiesProducer::endRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block ------------
void  EleNewEnergiesProducer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const& iSetup)
{
	mustache_regr_.setEventSetup(iSetup);
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
