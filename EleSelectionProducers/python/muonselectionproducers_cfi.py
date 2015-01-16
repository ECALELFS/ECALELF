import FWCore.ParameterSet.Config as cms

muonSelectionProducers = cms.EDProducer('MuonSelectionProducers',
                                       muonCollection = cms.InputTag('muons'),
                                       #recHitCollectionEB = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEB", "ALCARECO"),
                                       #recHitCollectionEE = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEE", "ALCARECO"),
                                       rhoFastJet = cms.InputTag('kt6PFJetsForRhoCorrection',"rho"),
                                       vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                                       conversionCollection = cms.InputTag('allConversions'),
                                       BeamSpotCollection = cms.InputTag('offlineBeamSpot'),
                                       chIsoVals = cms.InputTag('elPFIsoValueCharged03PFIdPFIso'), #useless for muons
                                       emIsoVals = cms.InputTag('elPFIsoValueGamma03PFIdPFIso'),   #useless for muons
                                       nhIsoVals = cms.InputTag('elPFIsoValueNeutral03PFIdPFIso')  #useless for muons
                                       #                                        chIsoVals = cms.InputTag('elPFIsoValueCharged03PFIdGsf'),
                                       #                                        emIsoVals = cms.InputTag('elPFIsoValueGamma03PFIdGsf'),
                                       #                                        nhIsoVals = cms.InputTag('elPFIsoValueNeutral03PFIdGsf')
                                       )

