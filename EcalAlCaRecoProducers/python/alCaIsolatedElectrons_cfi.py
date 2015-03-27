import FWCore.ParameterSet.Config as cms

alCaIsolatedElectrons = cms.EDProducer("AlCaECALRecHitReducer",
                                       srcLabels = cms.VInputTag( "gedGsfElectrons"),
                                       photonLabel = cms.InputTag("gedPhotons"),
                                       electronLabel = cms.InputTag("gedGsfElectrons"),
                                       EESuperClusterCollection = cms.InputTag("correctedMulti5x5SuperClustersWithPreshower"),
                                       minEta_highEtaSC = cms.double(2.4),
                                       ebRecHitsLabel = cms.InputTag("reducedEcalRecHitsEB"), #default
                                       eeRecHitsLabel = cms.InputTag("reducedEcalRecHitsEE"), #default
#                                       ebRecHitsLabel = cms.InputTag("ecalRecHit","EcalRecHitsEB"), #multifit 50ns
#                                       eeRecHitsLabel = cms.InputTag("ecalRecHit","EcalRecHitsEE"), #multifit 50ns
#                                       ebRecHitsLabel = cms.InputTag("ecalRecHitGlobal","EcalRecHitsGlobalEB"), #weights
#                                       eeRecHitsLabel = cms.InputTag("ecalRecHitGlobal","EcalRecHitsGlobalEE"), #weights
#                                       ebRecHitsLabel = cms.InputTag("ecalRecHitMultiFit25ns","EcalRecHitsMultiFit25nsEB"), #multifit 25ns
#                                       eeRecHitsLabel = cms.InputTag("ecalRecHitMultiFit25ns","EcalRecHitsMultiFit25nsEE"), #multifit 25ns
#                                       ebRecHitsLabel = cms.InputTag("ecalMultiFitUncalibRecHit","EcalUncalibRecHitsEB"),
#                                       eeRecHitsLabel = cms.InputTag("ecalMultiFitUncalibRecHit","EcalUncalibRecHitsEE"),
#                                       ebRecHitsLabel = cms.InputTag("ecalRecHit", "EcalRecHitsEB"),
#                                       eeRecHitsLabel = cms.InputTag("ecalRecHit", "EcalRecHitsEE"),
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


