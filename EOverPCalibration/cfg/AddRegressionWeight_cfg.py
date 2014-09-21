import FWCore.ParameterSet.Config as cms

process = cms.Process("AddRegressionWeight")

process.Options = cms.PSet(

  	inputFile = cms.string("../WZAnalysis_DATA.root"),
	treeNameDATA = cms.string("simpleNtupleEoverP/ntu"),  
	UseMethodFlag = cms.string("BDTG"),
	FilemethodXMLEBE = cms.string("weight_EB_BDTG_E_W/TMVARegression_BDTG.weights.xml"),
	FilemethodXMLEBP = cms.string("weight_EB_BDTG_P_W/TMVARegression_BDTG.weights.xml"),
	FilemethodXMLEEE = cms.string("weight_EE_BDTG_E_W/TMVARegression_BDTG.weights.xml"),
	FilemethodXMLEEP = cms.string("weight_EE_BDTG_P_W/TMVARegression_BDTG.weights.xml"),
	RegionOfTraining = cms.string("EE"),
	useW = cms.bool(True),
	isMC = cms.bool(False)
)