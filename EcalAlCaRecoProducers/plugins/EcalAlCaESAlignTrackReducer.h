#ifndef EcalAlCaESAlignTrackReducer_h
#define EcalAlCaESAlignTrackReducer_h

// system include files
#include <memory>
#include <fstream>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
//#include "FWCore/Framework/interface/MakerMacros.h"
//#include "FWCore/ParameterSet/interface/ParameterSet.h"
//#include "FWCore/Utilities/interface/InputTag.h"
//#include "FWCore/ServiceRegistry/interface/Service.h"
//#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/TrackExtraFwd.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHitFwd.h"

class EcalAlCaESAlignTrackReducer : public edm::EDProducer
{
public:
  explicit EcalAlCaESAlignTrackReducer(const edm::ParameterSet&);
  virtual ~EcalAlCaESAlignTrackReducer();

protected:
  virtual void beginJob();
  virtual void produce(edm::Event &, const edm::EventSetup & );
  virtual void endJob();
  bool TrackSelection( reco::Track track ); 

  // input collections
  edm::EDGetTokenT<reco::TrackCollection> generalTracksToken_;
  edm::EDGetTokenT<reco::TrackExtraCollection> generalTracksExtraToken_;
  edm::EDGetTokenT<TrackingRecHitCollection> trackingRecHitToken_;

  // label of output collections
  std::string newGeneralTracksCollection_; 
  std::string newGeneralTracksExtraCollection_; 
  std::string newTrackingRecHitCollection_; 
 
};

#endif
