import FWCore.ParameterSet.Config as cms

alCaIsolatedElectrons = cms.EDProducer("AlCaECALRecHitReducer",
                                       srcLabels = cms.VInputTag( "gedGsfElectrons"),
                                       photonLabel = cms.InputTag("gedPhotons"),
                                       electronLabel = cms.InputTag("gedGsfElectrons"),
                                       EESuperClusterCollection = cms.InputTag("correctedMulti5x5SuperClustersWithPreshower"),
                                       minEta_highEtaSC = cms.double(2.4),
                                       ebRecHitsLabel = cms.InputTag("reducedEcalRecHitsEB"), #default
                                       eeRecHitsLabel = cms.InputTag("reducedEcalRecHitsEE"), #default
#                                       esRecHitsLabel = cms.InputTag("ecalPreshowerRecHit","EcalRecHitsES"),
                                       esRecHitsLabel = cms.InputTag("reducedEcalRecHitsES"), #default
                                       uncalibRecHitCollectionEB = cms.InputTag(""),
                                       uncalibRecHitCollectionEE = cms.InputTag(""),
#                                       uncalibRecHitCollectionEB = cms.InputTag("ecalMultiFitUncalibRecHit","EcalUncalibRecHitsEB"),
#                                       uncalibRecHitCollectionEE = cms.InputTag("ecalMultiFitUncalibRecHit","EcalUncalibRecHitsEE"),

                                       alcaCaloClusterCollection = cms.string('alcaCaloCluster'),
#                                       isUncalib = cms.bool(False),
                                       phiSize = cms.int32(61),
                                       etaSize = cms.int32(15),
                                       alcaBarrelHitCollection = cms.string('alcaBarrelHits'),
                                       alcaEndcapHitCollection = cms.string('alcaEndcapHits'),
                                       alcaBarrelUncalibHitCollection = cms.string('alcaBarrelUncalibHits'),
                                       alcaEndcapUncalibHitCollection = cms.string('alcaEndcapUncalibHits'),
                                       alcaPreshowerHitCollection = cms.string('alcaPreshowerHits'),
#                                       esNstrips = cms.int32(20),
#                                       esNcolumns = cms.int32(1)                                       
                                       )


