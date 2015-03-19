import FWCore.ParameterSet.Config as cms

process = cms.Process("comparisonEBparameters")

process.Options = cms.PSet(

	inputFile = cms.string("/data1/rgerosa/L3_Weight/MC_WJets/noEP_Z/WZAnalysis_SingleEle_WJetsToLNu_Z_noEP_miscalib.root"),
	fileMCTruth = cms.string("output/MCtruthIC.root"),
	fileMCRecoIC = cms.string("output/MCRecoIC.root"),
	fileStatPrecision = cms.string("output/StatPrec.root")
)

