import FWCore.ParameterSet.Config as cms

electronRecalibSCAssociator = cms.EDProducer("ElectronRecalibSuperClusterAssociatorSH",
    electronCollection = cms.string(''),
    scIslandCollection = cms.string('endcapRecalibSC'),
    scIslandProducer = cms.string('correctedMulti5x5SuperClustersWithPreshower'),
    scProducer = cms.string('correctedHybridSuperClusters'),
    electronProducer = cms.string('gedGsfElectrons'),
    scCollection = cms.string('recalibSC')
)


