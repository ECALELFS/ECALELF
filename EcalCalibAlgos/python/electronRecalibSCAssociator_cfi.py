import FWCore.ParameterSet.Config as cms

electronRecalibSCAssociator = cms.EDProducer("ElectronRecalibSuperClusterAssociator",
#    superClusterCollectionEB = cms.InputTag('correctedHybridSuperClusters'),    
#    superClusterCollectionEE = cms.InputTag('correctedMulti5x5SuperClustersWithPreshower'),
                                             superClusterCollectionEB = cms.InputTag("particleFlowSuperClusterECAL:particleFlowSuperClusterECALBarrel"),
                                             superClusterCollectionEE = cms.InputTag("particleFlowSuperClusterECAL:particleFlowSuperClusterECALEndcapWithPreshower"),
                                             recHitCollectionEB = cms.InputTag("ecalRecHit", "EcalRecHitsEB"),
                                             recHitCollectionEE = cms.InputTag("ecalRecHit", "EcalRecHitsEE"),
                                             electronSrc = cms.InputTag('gedGsfElectrons'),
                                             outputLabel = cms.string('recalibSC')
)


