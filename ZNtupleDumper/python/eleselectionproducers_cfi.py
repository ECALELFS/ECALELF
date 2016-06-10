import FWCore.ParameterSet.Config as cms

eleSelectionProducers = cms.EDProducer('EleSelectionProducers',
                                       electronCollection = cms.InputTag('patElectrons'),
                                       rhoFastJet = cms.InputTag('kt6PFJetsForRhoCorrection',"rho"),
                                       vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                                       conversionCollection = cms.InputTag('allConversions'),
                                       BeamSpotCollection = cms.InputTag('offlineBeamSpot'),
                                       chIsoVals = cms.InputTag('elPFIsoValueCharged03PFIdPFIso'),
                                       emIsoVals = cms.InputTag('elPFIsoValueGamma03PFIdPFIso'),
                                       nhIsoVals = cms.InputTag('elPFIsoValueNeutral03PFIdPFIso')
                                       )

