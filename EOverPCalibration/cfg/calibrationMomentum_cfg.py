import FWCore.ParameterSet.Config as cms

process = cms.Process("CalibrationMomentum")

process.Options = cms.PSet(

    TreeName = cms.string("selected"),
    infileDATA = cms.string("cfg/listDATA_momentumCalibration.txt"),
    infileMC = cms.string("cfg/listDATA_momentumCalibration.txt"),
    WeightforMC = cms.string("pileupDataRun2016B_71p3mb.root"),
    usePUweights = cms.bool(False),
    nPhiBinsEB = cms.int32(360),
    nPhiBinsEE = cms.int32(360),
    nEtaBinsEB = cms.int32(1),
    nEtaBinsEE = cms.int32(1),
    nPhiBinsTempEB = cms.int32(1),
    nPhiBinsTempEE = cms.int32(1),
    rebinEB = cms.int32(10),
    rebinEE = cms.int32(20),
    outputFile = cms.string("output/MomentumCalibration2016.root"),
    typeEB = cms.string("eta1"),
    typeEE = cms.string("eta1")

)

