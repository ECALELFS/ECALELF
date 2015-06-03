import FWCore.ParameterSet.Config as cms

phoSelectionProducers = cms.EDProducer('PhoSelectionProducers',
                                       photonCollection = cms.InputTag('gedPhotons'),
                                       #recHitCollectionEB = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEB", "ALCARECO"),
                                       #recHitCollectionEE = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEE", "ALCARECO"),
                                       rhoFastJet = cms.InputTag('kt6PFJetsForRhoCorrection',"rho"),
                                       vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                                       conversionCollection = cms.InputTag('allConversions'),
                                       BeamSpotCollection = cms.InputTag('offlineBeamSpot'),
                                       chIsoVals = cms.InputTag('phPFIsoValueCharged03PFIdPFIso'),
                                       emIsoVals = cms.InputTag('phPFIsoValueGamma03PFIdPFIso'),
                                       nhIsoVals = cms.InputTag('phPFIsoValueNeutral03PFIdPFIso')
                                       #                                        chIsoVals = cms.InputTag('elPFIsoValueCharged03PFIdGsf'),
                                       #                                        emIsoVals = cms.InputTag('elPFIsoValueGamma03PFIdGsf'),
                                       #                                        nhIsoVals = cms.InputTag('elPFIsoValueNeutral03PFIdGsf')
                                       )

