#include <vector>
#include <memory>
#include <algorithm>

// Class header file
#include "Calibration/HcalIsolatedTrackReco/interface/IsolatedPixelTrackCandidateProducer.h"
#include "DataFormats/HcalIsolatedTrack/interface/IsolatedPixelTrackCandidate.h"
#include "DataFormats/HcalIsolatedTrack/interface/IsolatedPixelTrackCandidateFwd.h"
// Framework
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"
//
#include "DataFormats/Common/interface/TriggerResults.h"
// L1Extra
#include "DataFormats/L1Trigger/interface/L1EmParticle.h"
#include "DataFormats/L1Trigger/interface/L1JetParticle.h"
#include "DataFormats/L1Trigger/interface/L1JetParticleFwd.h"
///
#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"

//#include "DataFormats/HLTReco/interface/HLTFilterObject.h"

#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetupFwd.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerObjectMapRecord.h"
//#include "DataFormats/L1GlobalTrigger/interface/L1GtLogicParser.h"

#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerObjectMapFwd.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerObjectMap.h"


// Math
#include "Math/GenVector/VectorUtil.h"
#include "Math/GenVector/PxPyPzE4D.h"


IsolatedPixelTrackCandidateProducer::IsolatedPixelTrackCandidateProducer(const edm::ParameterSet& config){
   
  l1eTauJetsSource_=config.getUntrackedParameter<edm::InputTag>("L1eTauJetsSource");
  tauAssocCone_=config.getParameter<double>("tauAssociationCone"); 
  pixelTracksSource_=config.getUntrackedParameter<edm::InputTag>("PixelTracksSource");
  pixelIsolationConeSize_=config.getParameter<double>("PixelIsolationConeSize");
  hltGTseedlabel_=config.getUntrackedParameter<edm::InputTag>("L1GTSeedLabel");
  l1GtObjectMapSource_ = config.getUntrackedParameter<edm::InputTag> ("L1GtObjectMapSource");

  // Register the product
  produces< reco::IsolatedPixelTrackCandidateCollection >();

}

IsolatedPixelTrackCandidateProducer::~IsolatedPixelTrackCandidateProducer() {

}


void IsolatedPixelTrackCandidateProducer::produce(edm::Event& theEvent, const edm::EventSetup& theEventSetup) {

  using namespace edm;
   using namespace l1extra;

  reco::IsolatedPixelTrackCandidateCollection * trackCollection=new reco::IsolatedPixelTrackCandidateCollection;

  edm::Handle<reco::TrackCollection> pixelTracks;
  theEvent.getByLabel(pixelTracksSource_,pixelTracks);

  edm::Handle<l1extra::L1JetParticleCollection> l1eTauJets;
  theEvent.getByLabel(l1eTauJetsSource_,l1eTauJets);

  Handle<trigger::TriggerFilterObjectWithRefs> l1trigobj;
  theEvent.getByLabel(hltGTseedlabel_, l1trigobj);

  std::vector< edm::Ref<l1extra::L1JetParticleCollection> > l1tauobjref;
  std::vector< edm::Ref<l1extra::L1JetParticleCollection> > l1jetobjref;

  l1trigobj->getObjects(trigger::TriggerL1TauJet, l1tauobjref);
  l1trigobj->getObjects(trigger::TriggerL1CenJet, l1jetobjref);

//  edm::Handle<reco::HLTFilterObjectWithRefs> l1trigobj;
//  theEvent.getByLabel(hltGTseedlabel_, l1trigobj);

  double ptTriggered=-10;
  double etaTriggered=-100;
  double phiTriggered=-100;

  bool restj=false;
  bool resj=false;

  for (unsigned int p=0; p<l1tauobjref.size(); p++)
	{
	if (l1tauobjref[p]->pt()>ptTriggered)
		{
		ptTriggered=l1tauobjref[p]->pt(); 
		phiTriggered=l1tauobjref[p]->phi();
		etaTriggered=l1tauobjref[p]->eta();
		}
	}
  for (unsigned int p=0; p<l1jetobjref.size(); p++)
        {
        if (l1jetobjref[p]->pt()>ptTriggered)
                {
                ptTriggered=l1jetobjref[p]->pt();
                phiTriggered=l1jetobjref[p]->phi();
                etaTriggered=l1jetobjref[p]->eta();
                }
        }


  double minPtTrack_=5;
  double drMaxL1Track_=tauAssocCone_;

  int ntr=0;
  
  //loop to select isolated tracks
  for (reco::TrackCollection::const_iterator track=pixelTracks->begin(); 
       track!=pixelTracks->end(); track++) {

    if(track->pt()<minPtTrack_) continue;

    //selected tracks should match L1 taus

    bool tmatch=false;

    for (l1extra::L1JetParticleCollection::const_iterator tj=l1eTauJets->begin(); tj!=l1eTauJets->end(); tj++) {

       //select taus not matched to triggered L1 jet
      double dPhi=fabs(tj->phi()-phiTriggered);
      if (dPhi>3.1415926535) dPhi=2*3.1415926535-dPhi;
      double R=sqrt(dPhi*dPhi+pow(tj->eta()-etaTriggered,2));
      if (R<1) continue;
	      
      //select tracks matched to tau
      if(ROOT::Math::VectorUtil::DeltaR(track->momentum(),tj->momentum()) > drMaxL1Track_) continue;
	
      tmatch=true;

    } //loop over L1 tau

    //calculate isolation
    double maxPt=0;
    double sumPt=0;
    for (reco::TrackCollection::const_iterator track2=pixelTracks->begin(); 
	 track2!=pixelTracks->end(); track2++) {
      if(track2!=track &&
	 ROOT::Math::VectorUtil::DeltaR(track->momentum(),track2->momentum())
	 <pixelIsolationConeSize_){
	sumPt+=track2->pt();
	if(track2->pt()>maxPt) maxPt=track2->pt();
      }
    }

    if (tmatch&&maxPt<5)
      {
	reco::IsolatedPixelTrackCandidate 
	newCandidate(reco::TrackRef(pixelTracks,track-pixelTracks->begin()),restj, resj, maxPt,sumPt);
        trackCollection->push_back(newCandidate);
        ntr++;
      }

  }//loop over pixel tracks

  // put the product in the event
  std::auto_ptr< reco::IsolatedPixelTrackCandidateCollection > outCollection(trackCollection);
  theEvent.put(outCollection);


}
