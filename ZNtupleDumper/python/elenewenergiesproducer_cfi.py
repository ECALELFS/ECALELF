

import FWCore.ParameterSet.Config as cms

# ALCARECO collections
eleNewEnergiesProducer = cms.EDProducer('EleNewEnergiesProducer',
                                        scEnergyCorrectorSemiParm = cms.PSet(
                                            ecalRecHitsEB = cms.InputTag("alCaIsolatedElectrons", "alcaBarrelHits"),
                                            ecalRecHitsEE = cms.InputTag("alCaIsolatedElectrons", "alcaEndcapHits"),
                                            vertexCollection   = cms.InputTag("offlinePrimaryVertices"),
                                            isHLT = cms.bool(False),
                                            ),
                                            electronCollection = cms.InputTag("patElectrons"),
                                            photonCollection = cms.InputTag("photons"),
                                        
)
