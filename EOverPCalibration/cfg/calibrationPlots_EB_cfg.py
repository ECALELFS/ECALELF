import FWCore.ParameterSet.Config as cms

process = cms.Process("calibrationPlotsEBparameters")

process.Options = cms.PSet(

	inFileName     = cms.string("output/output_runDFastCalibrator_Oct22_Run2012ABC_Cal_Dic2012_WZ_noEP_EB_even.root"),
        inFileNameEven = cms.string("output/output_runDFastCalibrator_Oct22_Run2012ABC_Cal_Dic2012_WZ_noEP_EB_even.root"),
        inFileNameOdd  = cms.string("output/output_runDFastCalibrator_Oct22_Run2012ABC_Cal_Dic2012_WZ_noEP_EB_odd.root"),
        nEtaBinsEB     = cms.int32(1),
        nEtaBinsEE     = cms.int32(1),
        is2012Calib    = cms.bool(True),
        isEB           = cms.bool(True),
        evalStat       = cms.int32(1),
        outputFolder   = cms.string("output/"),
        outFileName    = cms.string("calibrationEB_Run2012ABC_22JanuaryRereco_WZ_Fbrem.root"),
        outputTxt      = cms.string("IC_Run2012ABC_22JanuaryRereco_WZ_Fbrem_EB"),
        fileType       = cms.string("cxx")

)


