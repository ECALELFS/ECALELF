import FWCore.ParameterSet.Config as cms

# ALCARECO collections
eleNewEnergiesProducer = cms.EDProducer('EleNewEnergiesProducer',
                                        recHitCollectionEB = cms.InputTag("alCaIsolatedElectrons", "alcaBarrelHits"),
                                        recHitCollectionEE = cms.InputTag("alCaIsolatedElectrons", "alcaEndcapHits"),
)
