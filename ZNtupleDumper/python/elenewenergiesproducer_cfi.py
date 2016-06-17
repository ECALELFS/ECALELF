

import FWCore.ParameterSet.Config as cms

# ALCARECO collections
eleNewEnergiesProducer = cms.EDProducer('EleNewEnergiesProducer',
                                        scEnergyCorrectorSemiParm = cms.PSet(
        ecalRecHitsEB = cms.InputTag("alCaIsolatedElectrons", "alcaBarrelHits"),
        ecalRecHitsEE = cms.InputTag("alCaIsolatedElectrons", "alcaEndcapHits"),
        vertexCollection   = cms.InputTag("offlinePrimaryVertices"),
        isHLT = cms.bool(False),
        regressionKeyEB = cms.string('pfscecal_EBCorrection_offline_v2'),
        uncertaintyKeyEB = cms.string('pfscecal_EBUncertainty_offline_v2'),
        regressionKeyEE = cms.string('pfscecal_EECorrection_offline_v2'),
        uncertaintyKeyEE = cms.string('pfscecal_EEUncertainty_offline_v2'),
        ),
                                        electronCollection = cms.InputTag("patElectrons"),
                                        photonCollection = cms.InputTag("photons"),
                                        
                                        )
