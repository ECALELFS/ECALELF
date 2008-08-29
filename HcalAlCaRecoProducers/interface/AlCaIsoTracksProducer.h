// -*- C++ -*-


// system include files
#include <memory>
#include <string>
// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/DetId/interface/DetId.h"
//#include "DataFormats/CaloTowers/interface/CaloTowerDetId.h"

#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "RecoTracker/TrackProducer/interface/TrackProducerBase.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"

#include "TrackPropagation/SteppingHelixPropagator/interface/SteppingHelixPropagator.h"
#include "TrackingTools/TrackAssociator/interface/TrackDetectorAssociator.h"
#include "TrackingTools/TrackAssociator/interface/TrackAssociatorParameters.h"
#include "TrackingTools/TrackAssociator/interface/TrackDetMatchInfo.h"

#include "Calibration/Tools/interface/TimerStack.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h" 
#include "Geometry/CaloGeometry/interface/CaloGeometry.h" 

#include "TH1F.h"
class TFile;

//
// class declaration
//

class AlCaIsoTracksProducer : public edm::EDProducer {
public:
  explicit AlCaIsoTracksProducer(const edm::ParameterSet&);
  ~AlCaIsoTracksProducer();
  
  virtual void produce(edm::Event &, const edm::EventSetup&);
  void endJob(void);

private:
  
  TrackDetectorAssociator trackAssociator_;
  TrackAssociatorParameters parameters_;
  
  
  const CaloGeometry* geo;
  edm::InputTag hoLabel_;
  edm::InputTag hbheLabel_;
  std::vector<edm::InputTag> ecalLabels_;

  edm::InputTag m_inputTrackLabel_;
  
  double m_dvCut;
  double m_ddirCut;
  double m_pCut;
  double m_ptCut;
  double m_ecalCut;

  double taECALCone_;
  double taHCALCone_;

  bool skipNeutrals_;
  bool checkHLTMatch_;
  edm::InputTag hltObjTag_;
  //temporary:
  double hltPcut_;
  double hltMaxNearCut_;
  bool hltFilterE_;

  double isolE_;
  double etaMax_;
  double cluRad_;
  double ringRad_;

};

