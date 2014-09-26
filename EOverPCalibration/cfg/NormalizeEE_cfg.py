import FWCore.ParameterSet.Config as cms

process = cms.Process("FastCalibratorEBparameters")

process.Options = cms.PSet(

    Inputfile1 = cms.string("/data1/rgerosa/L3_Weight/PromptSkim_Single_Double_Electron_recoFlag/EE_regression/WZAnalysis_PromptSkim_W-DoubleElectron_FT_R_42_V21B_regression_Z_R9_EE.root"),
    Inputfile2 = cms.string("/data1/rgerosa/L3_Weight/PromptSkim_Single_Double_Electron_recoFlag/EE_regression/Odd_WZAnalysis_PromptSkim_W-DoubleElectron_FT_R_42_V21B_regression_Z_R9_EE.root"),
    Inputfile3 = cms.string("/data1/rgerosa/L3_Weight/PromptSkim_Single_Double_Electron_recoFlag/EE_regression/Even_WZAnalysis_PromptSkim_W-DoubleElectron_FT_R_42_V21B_regression_Z_R9_EE.root"),
    evalStat = cms.int32(1),
    isMC = cms.bool(false),
    fileType = cms.string("png"),
    dirName = cms.string("."),
    printPlots = cms.bool(false)

)
