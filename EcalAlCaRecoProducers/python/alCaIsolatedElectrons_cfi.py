import FWCore.ParameterSet.Config as cms

alCaIsolatedElectrons = cms.EDProducer("AlCaECALRecHitReducer",
                                       electronLabel = cms.InputTag("gedGsfElectrons"),
                                       photonLabel = cms.InputTag(""),
                                       EESuperClusterCollection = cms.InputTag("correctedMulti5x5SuperClustersWithPreshower"),
                                       minEta_highEtaSC = cms.double(2.4),
                                       ebRecHitsLabel = cms.InputTag("reducedEcalRecHitsEB"),
                                       eeRecHitsLabel = cms.InputTag("reducedEcalRecHitsEE"),

                                       alcaCaloClusterCollection = cms.string('alcaCaloCluster'),
#                                       isUncalib = cms.bool(False),
                                       phiSize = cms.int32(61),
                                       etaSize = cms.int32(15),
                                       alcaBarrelHitCollection = cms.string('alcaBarrelHits'),
                                       alcaEndcapHitCollection = cms.string('alcaEndcapHits'),
#                                       alcaEndcapHitCollection = cms.string('alCaRecHitsEE'),
# in sandboxRereco
#                                       ebRecHitsLabel = cms.InputTag("ecalRecHit","EcalRecHitsEB")
#                                       eeRecHitsLabel = cms.InputTag("ecalRecHit","EcalRecHitsEE")

#                                       esRecHitsLabel = cms.InputTag("ecalPreshowerRecHit","EcalRecHitsES"),
#                                       alcaPreshowerHitCollection = cms.string('alcaPreshowerHits'),
#                                       esNstrips = cms.int32(20),
#                                       esNcolumns = cms.int32(1)                                       
                                       )


