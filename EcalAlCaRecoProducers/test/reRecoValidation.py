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

allRecHits=False
eventListPrint=False
lumi=-1
eventNumbers=[]
eventMin=-1

# for now look for events in two files with a given lumi section
maxEvents=-1
event_counter=0


file='EcalRecalElectron.root'

print file
events = Events (file)

handleElectrons = Handle('std::vector<reco::GsfElectron>')
handleRecHitsEB = Handle('edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit> >')
handleRecHitsEE = Handle('edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit> >')
handleRecHitsES = Handle('edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit> >')
handleRhoFastJet = Handle('double')

electronTAG = 'gedGsfElectrons'
electronTAG = 'electronRecalibSCAssociator'


EErecHitmap_ele1 = TH2F("EErecHitmap_ele1", "EErecHitmap_ele1",
                   100,0,100,
                   100,0,100)

EBrecHitmap_ele1 = TH2F("EBrecHitmap_ele1", "EBrecHitmap_ele1",
                   171,-85,85,
                   360,0,360)

EErecHitmap_ele2 = TH2F("EErecHitmap_ele2", "EErecHitmap_ele2",
                   100,0,100,
                   100,0,100)

EBrecHitmap_ele2 = TH2F("EBrecHitmap_ele2", "EBrecHitmap_ele2",
                   171,-85,85,
                   360,0,360)

print file_format, file, electronTAG, processName, maxEvents

print "run    lumi  event     isEB    energy     eSC  rawESC    e5x5    E_ES, etaEle, phiEle, etaSC, phiSC, clustersSize, nRecHits"
for event in events:

#    if(event_counter % 100):
#        print "[STATUS] event ", event_counter
        
    if(maxEvents > 0 and event_counter > maxEvents):
        break
    if(eventListPrint==True):
        print event.eventAuxiliary().run(), event.eventAuxiliary().luminosityBlock(), event.eventAuxiliary().event()
        continue
    #if(event.eventAuxiliary.run()== 145351895):
    if lumi > 0 and int(event.eventAuxiliary().luminosityBlock()) != lumi :
            continue

    if(len(eventNumbers)>0 and not (event.eventAuxiliary().event() in eventNumbers)):
       continue


        #    event.getByLabel(electronTAG, "", processName, handleElectrons)
    event.getByLabel(electronTAG, handleElectrons)
    #    print file_format, file, electronTAG        
    electrons = handleElectrons.product()


#        event.getByLabel("reducedEcalRecHitsEB", "", processName, handleRecHitsEB)
#        event.getByLabel("reducedEcalRecHitsEE", "", processName, handleRecHitsEE)
#        event.getByLabel("reducedEcalRecHitsES", "", processName, handleRecHitsES)
#        print "##############", 
        #        rhoTAG=edm.InputTag()
        #        rhoTAG=("kt6PFJets","rho","RECO")
#        event.getByLabel("kt6PFJets","rho","RECO",handleRhoFastJet)

    
#    print "Num of electrons: ",len(electrons)
    if(len(electrons)>=2):
     ele_counter=0
     for electron in electrons:
        if(not electron.ecalDrivenSeed()): 
            print "trackerDriven",
#            sys.exit(0)
        electron.superCluster().energy()

         
        #        ESrecHits = handleRecHitsES.product()
        #        if(abs(electron.eta()) > 1.566):
        #            for ESrecHit in ESrecHits:
        #                if(eventNumber >0):
        #                    esrecHit = ESDetId(ESrecHit.id().rawId())
        #                    print ESrecHit.id()(), esrecHit.strip(), esrecHit.six(), esrecHit.siy(), esrecHit.plane()
        print "------------------------------"
        if(electron.isEB()):
            recHits = handleRecHitsEB.product()
        else:
            recHits = handleRecHitsEE.product()
        nRecHits=0
        for recHit in recHits:
             nRecHits=nRecHits+1
             if(len(eventNumbers)==1):
                 if(electron.isEB()):
                     EBrecHit = EBDetId(recHit.id().rawId())
                     if(allRecHits):
                         if(ele_counter==0):
                             EBrecHitmap_ele1.Fill(EBrecHit.ieta(), EBrecHit.iphi(), recHit.energy());
                         elif(ele_counter==1):
                             EBrecHitmap_ele2.Fill(EBrecHit.ieta(), EBrecHit.iphi(), recHit.energy());
                         
                     print recHit.id()(), EBrecHit.ieta(), EBrecHit.iphi(), recHit.energy(), recHit.checkFlag(0)
                 else:
                     EErecHit = EEDetId(recHit.id().rawId())
                     if(allRecHits):
                         if(ele_counter==0):
                             EErecHitmap_ele1.Fill(EErecHit.ix(), EErecHit.iy(), recHit.energy());
                         elif(ele_counter==1):
                             EErecHitmap_ele2.Fill(EErecHit.ix(), EErecHit.iy(), recHit.energy());
                     print recHit.id()(), EErecHit.ix(), EErecHit.iy(), recHit.energy()
 
        hits = electron.superCluster().hitsAndFractions() 
        nRecHitsSC=0
        for hit in hits:
             nRecHitsSC=nRecHitsSC+1
             if(len(eventNumbers)==1):
                 if(electron.isEB()):
                     EBrecHit = EBDetId(hit.first.rawId())
                     if(not allRecHits):
                         if(ele_counter==0):
                             EBrecHitmap_ele1.Fill(EBrecHit.ieta(), EBrecHit.iphi(), hit.second*electron.superCluster().energy());
                         elif(ele_counter==1):
                             EBrecHitmap_ele2.Fill(EBrecHit.ieta(), EBrecHit.iphi(), hit.second*electron.superCluster().energy());
                     print "SC", (hit.first).rawId(), (hit.first)(), EBrecHit.ieta(), EBrecHit.iphi(),  EBrecHit.tower().iTT(), EBrecHit.ism(), EBrecHit.im()
                 else:
                     EErecHit = EEDetId(hit.first.rawId())
                     if(not allRecHits):
                         if(ele_counter==0):
                             EErecHitmap_ele1.Fill(EErecHit.ix(), EErecHit.iy(), recHit.energy());
                         elif(ele_counter==1):
                             EErecHitmap_ele2.Fill(EErecHit.ix(), EErecHit.iy(), recHit.energy());
 
 #                print "SC ", (hit.first)()
            
        print event.eventAuxiliary().run(), event.eventAuxiliary().luminosityBlock(), event.eventAuxiliary().event(),
        print "isEB=",electron.isEB(),
        print '{0:7.3f} {1:7.3f} {2:7.3f} {3:7.3f} {4:7.3f}'.format(electron.energy(), electron.superCluster().energy(), electron.superCluster().rawEnergy(), electron.e5x5(), electron.superCluster().preshowerEnergy()),
        print '{0:6.3f} {1:6.3f} {2:6.3f} {3:6.3f}'.format(electron.eta(), electron.phi(), electron.superCluster().eta(), electron.superCluster().phi()),
        print electron.superCluster().clustersSize(), nRecHits, nRecHitsSC
        ele_counter+=1
        
        
    event_counter+=1

print event_counter

# setting maps to -999
gStyle.SetPaintTextFormat("1.1f")
EBrecHitmap_ele1.SaveAs("EBrecHitmap_ele1.root")
EErecHitmap_ele1.SaveAs("EErecHitmap_ele1.root")
EBrecHitmap_ele2.SaveAs("EBrecHitmap_ele2.root")
EErecHitmap_ele2.SaveAs("EErecHitmap_ele2.root")



