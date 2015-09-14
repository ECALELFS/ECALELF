import FWCore.ParameterSet.Config as cms

electronRecalibSCAssociator = cms.EDProducer("ElectronRecalibSuperClusterAssociatorSH",
    scIslandCollection = cms.string('endcapRecalibSC'),
    scIslandProducer = cms.string('correctedMulti5x5SuperClustersWithPreshower'),
    scProducer = cms.string('correctedHybridSuperClusters'),
    electronSrc = cms.InputTag('gedGsfElectrons'),
    scCollection = cms.string('recalibSC')
)


