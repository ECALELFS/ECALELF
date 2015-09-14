import FWCore.ParameterSet.Config as cms

process = cms.Process("FastCalibratorEEoptimization")

process.Options = cms.PSet(

        inputList            = cms.string("cfg/list.txt"),
#       inputFileDeadXtal    = cms.string(),
        inputTree            = cms.string("selected"),
        jsonFileName         = cms.string("json/Cert_190456-208686_8TeV_PromptReco_Collisions12_JSON.txt"),
        miscalibMap          = cms.string("scalibMapEE_eta_straweak.txt"),
        isMiscalib           = cms.bool(False),
        isSaveEPDistribution = cms.bool(False),
        isMCTruth            = cms.bool(False),
        isEPselection        = cms.bool(False),
        isPtCut              = cms.bool(False),
        PtMin                = cms.double(0.),
        isfbrem              = cms.bool(False),
        fbremMax             = cms.double(100.),
        isR9selection        = cms.bool(False),
        R9Min                = cms.double(-1.),
        miscalibMethod       = cms.int32(1),
        inputMomentumScale   = cms.string("output/MomentumCalibration2012"),
        typeEB               = cms.string("eta1"),
        typeEE               = cms.string("eta1"),
        outputPath           = cms.string("output/output_runD_EE"),
        outputFile           = cms.string("FastCalibrator_Oct22_Run2012ABC_Cal_Dic2012"),
        numberOfEvents       = cms.int32(-1),
        useZ                 = cms.int32(1),
        useW                 = cms.int32(1),
        splitStat            = cms.int32(0),
        nLoops               = cms.int32(20),
        isDeadTriggerTower   = cms.bool(False)
)

