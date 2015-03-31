import FWCore.ParameterSet.Config as cms

process = cms.Process("comparisonEEparameters")

process.Options = cms.PSet(

    inputFile = cms.string("/data1/rgerosa/L3_Weight/MC_WJets/EE_recoFlag/WJetsToLNu_7TeV-madgraph-tauola_Fall11_Z_R9_miscalib_EE.root"),
    fileMCTruth = cms.string("output/MCtruthIC_EE.root"),
    fileMCRecoIC = cms.string("output/MCRecoIC_EE.root"),
    fileStatPrecision = cms.string("output/StatPrec_MC_R9.root"),
    outputFile = cms.string("output/ResidualForSystematic_EE_MC.root")
            )

