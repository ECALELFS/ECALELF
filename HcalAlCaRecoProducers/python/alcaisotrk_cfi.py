import FWCore.ParameterSet.Config as cms

# producer for alcaisotrk (HCAL isolated tracks)
from TrackingTools.TrackAssociator.default_cfi import *
from TrackingTools.TrackAssociator.DetIdAssociatorESProducer_cff import *
IsoProd = cms.EDProducer("AlCaIsoTracksProducer",
    TrackAssociatorParameterBlock,
    HBHEInput = cms.InputTag("hbhereco"),
    InputTracksLabel = cms.InputTag("generalTracks"),
    vtxCut = cms.double(10.0),
    MinTrackPt = cms.double(0.0),
    ECALOuterRingRadius = cms.double(18.0),
    ECALClusterRadius = cms.double(9.0),
    HOInput = cms.InputTag("horeco"),
    MaxNearbyTrackEnergy = cms.double(2.0),
    RIsolAtECALSurface = cms.double(40.0),
    ECALInputs = cms.VInputTag(cms.InputTag("ecalRecHit","EcalRecHitsEB"), cms.InputTag("ecalRecHit","EcalRecHitsEE")),
    MaxTrackEta = cms.double(1.9),
    SkipNeutralIsoCheck = cms.untracked.bool(False),
    MinTrackP = cms.double(15.0),
    CheckHLTMatch=cms.bool(True)
)


