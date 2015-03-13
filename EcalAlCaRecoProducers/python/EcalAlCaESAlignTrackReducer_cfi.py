import FWCore.ParameterSet.Config as cms

ecalAlCaESAlignTrackReducer = cms.EDProducer("EcalAlCaESAlignTrackReducer",
                                             generalTracksLabel = cms.InputTag("generalTracks"),
                                             generalTracksExtraLabel = cms.InputTag("generalTracksExtra"),
                                             trackingRecHitLabel = cms.InputTag("generalTracks"),
                                             newGeneralTracksCollection = cms.string("generalTracks"),
                                             newGeneralTracksExtraCollection = cms.string("generalTracksExtra"),
                                             newTrackingRecHitCollection = cms.string("trackingRecHits"),
                                             )
