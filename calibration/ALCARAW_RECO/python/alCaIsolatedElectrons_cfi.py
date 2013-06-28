import FWCore.ParameterSet.Config as cms

alCaIsolatedElectrons = cms.EDProducer("AlCaECALRecHitReducer",
                                       electronLabel = cms.InputTag("gsfElectrons"),
                                       ebRecHitsLabel = cms.InputTag("reducedEcalRecHitsEB"),
                                       eeRecHitsLabel = cms.InputTag("reducedEcalRecHitsEE"),
                                       isUncalib = cms.bool(False),
                                       phiSize = cms.int32(61),
                                       etaSize = cms.int32(15),
                                       alcaBarrelHitCollection = cms.string('alcaBarrelHits'),
                                       alcaEndcapHitCollection = cms.string('alcaEndcapHits'),
#                                       alcaEndcapHitCollection = cms.string('alCaRecHitsEE'),
                                       eventWeight = cms.double(1.0),
# in sandboxRereco
#                                       ebRecHitsLabel = cms.InputTag("ecalRecHit","EcalRecHitsEB")
#                                       eeRecHitsLabel = cms.InputTag("ecalRecHit","EcalRecHitsEE")

                                       esRecHitsLabel = cms.InputTag("ecalPreshowerRecHit","EcalRecHitsES"),
                                       alcaPreshowerHitCollection = cms.string('alcaPreshowerHits'),
                                       esNstrips = cms.int32(20),
                                       esNcolumns = cms.int32(1)                                       
                                       )


