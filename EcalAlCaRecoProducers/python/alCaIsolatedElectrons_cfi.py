import FWCore.ParameterSet.Config as cms

alCaIsolatedElectrons = cms.EDProducer("AlCaElectronsProducer",
    electronLabel = cms.InputTag("gsfElectrons"),
    alcaEndcapHitCollection = cms.string('alcaEndcapHits'),
    phiSize = cms.int32(11),
    etaSize = cms.int32(5),
    ebRecHitsLabel = cms.InputTag("ecalRecHit","EcalRecHitsEB"),
    alcaBarrelHitCollection = cms.string('alcaBarrelHits'),
    eventWeight = cms.double(1.0),
    eeRecHitsLabel = cms.InputTag("ecalRecHit","EcalRecHitsEE")
)


