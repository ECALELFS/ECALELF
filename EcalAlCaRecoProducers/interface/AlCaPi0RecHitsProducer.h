/**

 Description: Producer for EcalRecHits to be used for pi0 ECAL calibration. ECAL Barrel RecHits only.

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Vladimir Litvine


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

//#include "TrackingTools/TrackAssociator/interface/TimerStack.h"
#include "Utilities/Timing/interface/TimerStack.h"

//
// class declaration
//

typedef std::map<DetId, EcalRecHit> RecHitsMap;

// Less than operator for sorting EcalRecHits according to energy.
class ecalRecHitLess : public std::binary_function<EcalRecHit, EcalRecHit, bool> 
{
 public:
  bool operator()(EcalRecHit x, EcalRecHit y) 
    { 
      return (x.energy() > y.energy()); 
    }
};



class AlCaPi0RecHitsProducer : public edm::EDProducer {
   public:
      explicit AlCaPi0RecHitsProducer(const edm::ParameterSet&);
      ~AlCaPi0RecHitsProducer();


      virtual void produce(edm::Event &, const edm::EventSetup&);
   private:
      // ----------member data ---------------------------

 std::string ecalHitsProducer_;
 std::string barrelHits_;
 std::string pi0BarrelHits_;

 int gammaCandEtaSize_;
 int gammaCandPhiSize_;

 double clusSeedThr_;
 int clusEtaSize_;
 int clusPhiSize_;

 double selePtGammaOne_;
 double selePtGammaTwo_;
 double selePtPi0_;
 double seleMinvMaxPi0_;
 double seleMinvMinPi0_;
 double seleXtalMinEnergy_;
 int seleNRHMax_;

 std::map<DetId, EcalRecHit> *recHitsEB_map;


};
