import ROOT
from ROOT import *
from DataFormats.FWLite import Events, Handle
from PhysicsTools.PythonAnalysis import *
#import print_options
#print_options.set_float_precision(4)
gSystem.Load("libFWCoreFWLite.so")
AutoLibraryLoader.enable()
#import EcalDetId
#from DataFormats.EcalDetId import *
#
import sys,os

#file='EXO-Phys14DR-00009.root'
#events = Events (file)
#
#print file
#handleElectrons = Handle('std::vector<pat::Electron>')
#electronTAG = "slimmedElectrons"
#
#
#print "numEvents = ", events.size()
#n=0
#for event in events:
#    print n
#    event.getByLabel(electronTAG, handleElectrons)
#    electrons = handleElectrons.product()
#    print electrons.size()
#    eleIndex=0
#    for electron in electrons:
#        
#        print eleIndex, electron.pt(), electron.eta(), electron.phi()
#        eleIndex+=1
#    
#    n+=1

file='root://eoscms//eos/cms/store/mc/RunIISummer16MiniAODv2/DYJetsToEE_M-50_LTbinned_75To80_5f_LO_13TeV-madgraph_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/76ED9216-7CCF-E611-8F47-842B2B766849.root'
#,/store/mc/RunIISummer16MiniAODv2/DYJetsToEE_M-50_LTbinned_75To80_5f_LO_13TeV-madgraph_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/14E688B5-B0CE-E611-9A36-1CC1DE18D04C.root,/store/mc/RunIISummer16MiniAODv2/DYJetsToEE_M-50_LTbinned_75To80_5f_LO_13TeV-madgraph_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/54E2E312-8ACE-E611-856E-D4AE526A0DAE.root'

handleGen = Handle('std::vector<reco::GenParticle>')
genTAG = "genParticles"
genTAG = "prunedGenParticles"

handleElectron = Handle('std::vector<pat::Electron>')

events = Events(file)

stopIndex = 30
index=0
for event in events:

    event.getByLabel(genTAG,handleGen)
    genParts = handleGen.product()
    
    event.getByLabel("slimmedElectrons", handleElectron)
    electrons = handleElectron.product()
    
    if(electrons.size()<20):
	for gen in genParts:
		# if(abs(gen.pdgId())!=9900024 and gen.mass() > 1200):
		# 	print gen.pdgId(), gen.mass(), gen.status(), gen.charge(), gen.numberOfDaughters()
# 		if(abs(gen.pdgId())==9900024):
#			print gen.pdgId(), gen.mass(), gen.status(), gen.charge(), gen.numberOfDaughters() #, gen.daughter(0).pt(), gen.daughter(1).pt()
#                if(abs(gen.pdgId())==9900024 and gen.status()==62):
#                        print gen.pdgId(), gen.mass(), gen.status(), gen.charge(), gen.numberOfDaughters()
#                        print "  - ", gen.daughter(0).pt(), gen.daughter(0).eta()
#                        print "  - ", gen.daughter(1).pt(), gen.daughter(1).eta()
                if(abs(gen.pdgId()) == 11): # and gen.status()==1 and gen.pt()>10 and abs(gen.eta())<2.5 and (abs(gen.eta())<1.4442 or abs(gen.eta())>1.566)):
                         print gen.pdgId(), gen.mass(), gen.status(), gen.charge(), gen.numberOfDaughters(), gen.pt(), gen.eta(), gen.mother().pdgId()
#        for electron in electrons:
#                print electron.pt(), electron.eta()
        print "------------------------------"

	index+=1
	if(index>=stopIndex):
		break

