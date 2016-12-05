import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
import os,sys
import ROOT
from array import array

process = cms.Process("pileup")

options = VarParsing.VarParsing('standard')

options.register('PileupCFI',
                 'mix_2016_25ns_SpringMC_PUScenarioV1_PoissonOOTPU_cfi',
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 '_cfi file setup with MC PU values')

options.parseArguments()

string = "SimGeneral.MixingModule." + options.PileupCFI 
print string
process.load(string)

pileupbins   =  array('d',  process.mix.input.nbPileupEvents.probFunctionVariable)
pileupvalue  =  array('d',  process.mix.input.nbPileupEvents.probValue)

pileupbins += array('d', [pileupbins[-1] + pileupbins[1] - pileupbins[0]] )

PUFile = ROOT.TFile(options.output, "recreate")
PUHist = ROOT.TH1F("pileup","pileup",len(pileupbins)-1, array('d',pileupbins))

for x,y in zip(xrange(len(pileupvalue)), pileupvalue):
	PUHist.SetBinContent(x+1,y)
PUFile.Write()
