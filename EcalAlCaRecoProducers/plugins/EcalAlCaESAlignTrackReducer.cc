// -*- C++ -*-
//
// Package:    EcalAlCaESAlignTrackReducer
// Class:      EcalAlCaESAlignTrackReducer
//
/**\class EcalAlCaESAlignTrackReducer EcalAlCaESAlignTrackReducer.cc
   Calibration/EcalAlCaRecoProducers/plugins/EcalAlCaESAlignTrackReducer.cc

   Description: Selects tracks in the ES (preshower) coverage and save
   them in a separate collection

   Implementation:
   Selects tracks with eta>1.5, clone the tracks, the extras and trackHits
*/
//
// Author: Jui-Fa and Shervin Nourbakhsh
//

#include "Calibration/EcalAlCaRecoProducers/plugins/EcalAlCaESAlignTrackReducer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/TrackExtra.h"

//
// constructors and destructor
//
EcalAlCaESAlignTrackReducer::EcalAlCaESAlignTrackReducer (const edm::ParameterSet & iConfig):
     newGeneralTracksCollection_ (iConfig.getParameter< std::string >("newGeneralTracksCollection")),
     newGeneralTracksExtraCollection_ (iConfig.getParameter< std::string >("newGeneralTracksExtraCollection")),
     newTrackingRecHitCollection_ (iConfig.getParameter< std::string >("newTrackingRecHitCollection"))
{
     // input collections
     generalTracksToken_ = consumes < reco::TrackCollection > (iConfig.getParameter <edm::InputTag >("generalTracksLabel"));
     generalTracksExtraToken_ =	consumes < reco::TrackExtraCollection > (iConfig.getParameter <edm::InputTag >	 ("generalTracksExtraLabel"));
     trackingRecHitToken_ = consumes < TrackingRecHitCollection > (iConfig.getParameter <					       edm::InputTag >					       ("trackingRecHitLabel"));

    // label of output collections
     newGeneralTracksCollection_ =
	iConfig.getParameter < std::string > ("newGeneralTracksCollection");
    newGeneralTracksExtraCollection_ =
	iConfig.getParameter < std::string >
	("newGeneralTracksExtraCollection");
    newTrackingRecHitCollection_ =
	iConfig.getParameter < std::string > ("newTrackingRecHitCollection");

    produces < reco::TrackCollection > (newGeneralTracksCollection_);
    produces < reco::TrackExtraCollection >
	(newGeneralTracksExtraCollection_);
    produces < reco::TrackExtraCollection > ();
    produces < TrackingRecHitCollection > (newTrackingRecHitCollection_);
}

EcalAlCaESAlignTrackReducer::~EcalAlCaESAlignTrackReducer ()
{
}

// ------------ additional functions  ------------
bool EcalAlCaESAlignTrackReducer::TrackSelection (reco::Track track)
{
    if (fabs (track.eta ()) < 1.5)
	return false;		// reject tracks in the barrel
    return true;
}

// ------------ method called to for each event  ------------
void
EcalAlCaESAlignTrackReducer::produce (edm::Event & iEvent,
				      const edm::EventSetup & iSetup)
{
    edm::Handle < reco::TrackCollection > TracksHandle;
    iEvent.getByToken (generalTracksToken_, TracksHandle);
    edm::Handle < reco::TrackExtraCollection > TrackExtraHandle;
    iEvent.getByToken (generalTracksExtraToken_, TrackExtraHandle);
    edm::Handle < TrackingRecHitCollection > TrackingRecHitHandle;
    iEvent.getByToken (trackingRecHitToken_, TrackingRecHitHandle);

    // Create empty collections
    std::auto_ptr < reco::TrackCollection >
	newGeneralTracksCollection (new reco::TrackCollection);
    std::auto_ptr < reco::TrackExtraCollection >
	newGeneralTracksExtraCollection (new reco::TrackExtraCollection);
    std::auto_ptr < TrackingRecHitCollection >
	newTrackingRecHitCollection (new TrackingRecHitCollection);

    reco::TrackExtraRefProd trackExtraRefProd =	iEvent.getRefBeforePut < reco::TrackExtraCollection >	(newGeneralTracksExtraCollection_);
    edm::Ref < reco::TrackExtraCollection >::key_type iTrackExtraRefProd = 0;

    TrackingRecHitRefProd trackingRecHitRefProd =iEvent.getRefBeforePut < TrackingRecHitCollection >(newTrackingRecHitCollection_);

    for (reco::TrackCollection::const_iterator itTrack =
	 TracksHandle->begin (); itTrack != TracksHandle->end (); ++itTrack)
      {
	  if (!TrackSelection (*itTrack))
	    {
		continue;
	    }
	  // clone old track and store to new collection
	  newGeneralTracksCollection->push_back (reco::Track (*itTrack));
	  reco::Track & newTrack = newGeneralTracksCollection->back ();

	  newGeneralTracksExtraCollection->push_back (*newTrack.extra ());	// implicit copy constructor
	  reco::TrackExtra & newTrackExtra =  newGeneralTracksExtraCollection->back ();

	  //auto const firstHitIndex = newTrackingRecHitCollection->size ();	// new hits will be appended

	  for (trackingRecHit_iterator itHit = itTrack->recHitsBegin ();
	       itHit != itTrack->recHitsEnd (); ++itHit)
	    {
		// add recHit to collection
		newTrackingRecHitCollection->push_back (*(*itHit)->clone ());
	    }


	  // setting track hits in trackExtra \todo fix error 
	  // Inconsistency RefCore::pushBackItem: Ref or Ptr is inconsistent with RefVector (PtrVector)id = (3:1) should be (2:257)
	  //newTrackExtra.setHits (trackingRecHitRefProd, 0, 1); //firstHitIndex,		 itTrack->recHitsSize ());
	  
	  // reference to the new trackExtra
	  reco::TrackExtraRef newTrackExtraRef (trackExtraRefProd,iTrackExtraRefProd++);
	  newTrack.setExtra (newTrackExtraRef);
      }

    // add the collections to the event
    iEvent.put( newGeneralTracksCollection,  newGeneralTracksCollection_ );
    iEvent.put( newGeneralTracksExtraCollection,newGeneralTracksExtraCollection_);
    iEvent.put( newTrackingRecHitCollection, newTrackingRecHitCollection_);
}

// ------------ method called once each job just before starting event loop
// ------------
void
EcalAlCaESAlignTrackReducer::beginJob ()
{
}

// ------------ method called once each job just after ending the event loop
// ------------
void
EcalAlCaESAlignTrackReducer::endJob ()
{
}

// define this as a plug-in
DEFINE_FWK_MODULE (EcalAlCaESAlignTrackReducer);
