import math
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


file='EcalRecalElectron.root'
file='root://eoscms//eos/cms/store/data/Run2015D/DoubleEG/MINIAOD/PromptReco-v4/000/259/862/00000/389D079E-EA7B-E511-9506-02163E0141D8.root'
file='root://eoscms//eos/cms//store/mc/RunIISpring15DR74/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/Asympt25ns_MCRUN2_74_V9-v3/10000/72F7A0B6-8E14-E511-8EBA-6C3BE5B58198.root'
print file
events = Events (file)

#handleElectrons = Handle('std::vector<reco::GsfElectron>')
handleElectrons  = Handle('std::vector<pat::Electron>')
handleRecHitsEB = Handle('edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit> >')
handleRecHitsEE = Handle('edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit> >')
handleRecHitsES = Handle('edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit> >')
handleRhoFastJet = Handle('double')
handleVertices = Handle('vector<reco::Vertex>')
electronTAG = 'gedGsfElectrons'
#electronTAG = 'electronRecalibSCAssociator'
electronTAG = 'slimmedElectrons'



print "run    lumi  event     isEB    energy     eSC  rawESC    e5x5    E_ES, etaEle, phiEle, etaSC, phiSC, clustersSize, nRecHits"
for event in events:
    event.getByLabel('offlineSlimmedPrimaryVertices', handleVertices)
    vertices=handleVertices.product()

    event.getByLabel(electronTAG, handleElectrons)
    electrons = handleElectrons.product()
    
    print vertices.size()

    for electron in electrons:
        print electron.eta(), electron.phi(), electron.energy()
        ids=electron.electronIDs()
        for id_ in ids:
            print id_.first, id_.second

    break
