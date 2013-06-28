import FWCore.ParameterSet.Config as cms

eleSelectionProducers = cms.EDProducer('EleSelectionProducers',
                                       electronCollection = cms.InputTag('gsfElectrons'),
                                       #recHitCollectionEB = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEB", "ALCARECO"),
                                       #recHitCollectionEE = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEE", "ALCARECO"),
                                       rhoFastJet = cms.InputTag('kt6PFJetsForRhoCorrection',"rho"),
                                       #vertexCollection = cms.InputTag('offlinePrimaryVerticesWithBS'),
                                       conversionCollection = cms.InputTag('allConversions'),
                                       BeamSpotCollection = cms.InputTag('offlineBeamSpot')
                                       

)

