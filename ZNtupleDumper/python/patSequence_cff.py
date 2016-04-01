import FWCore.ParameterSet.Config as cms
from TrackingTools.TransientTrack.TransientTrackBuilder_cfi import *
#from Configuration.StandardSequences.MagneticField_cff import *

#------------------------------ pattuple
from Calibration.ZNtupleDumper.elePat_cfi import *
from Calibration.ZNtupleDumper.muonPat_cfi import *
from Calibration.ZNtupleDumper.phoPat_cfi import *
#process.patElectrons.electronSource = cms.InputTag("gedGsfElectrons")
#process.patElectrons.addElectronID = cms.bool(False)
#process.patElectrons.addGenMatch = cms.bool(True)
#process.patElectrons.pvSrc = cms.InputTag("offlinePrimaryVerticesWithBS")
#print process.patElectrons.reducedBarrelRecHitCollection
    
#------------------------------ new energies
from Calibration.ZNtupleDumper.elenewenergiesproducer_cfi import *
#eleNewEnergiesProducer.regrPhoFile='src/Calibration/EleNewEnergiesProducer/data/gbrv3ph_52x.root'
#eleNewEnergiesProducer.regrEleFile='src/Calibration/EleNewEnergiesProducer/data/gbrv3ele_52x.root'
#eleNewEnergiesProducer.regrEleFile_fra='src/Calibration/EleNewEnergiesProducer/data/eleEnergyRegWeights_V1.root'
#eleNewEnergiesProducer.ptSplit=cms.bool(False)

#from EgammaAnalysis.ElectronTools.electronRegressionEnergyProducer_cfi import *
#eleRegressionEnergy.inputElectronsTag = cms.InputTag('gedGsfElectrons')
#eleRegressionEnergy.inputCollectionType = cms.uint32(0)
#eleRegressionEnergy.useRecHitCollections = cms.bool(True)
#eleRegressionEnergy.produceValueMaps = cms.bool(True)
#eleRegressionEnergy.rhoCollection = cms.InputTag('kt6PFJetsForRhoCorrection',"rho")
#eleRegressionEnergy.vertexCollection = cms.InputTag('offlinePrimaryVertices')

#------------------------------ electronID producer
from Calibration.ZNtupleDumper.eleselectionproducers_cfi import *
from Calibration.ZNtupleDumper.phoselectionproducers_cfi import *
from Calibration.ZNtupleDumper.muonselectionproducers_cfi import *
# process.EleSelectionProducers

#============================== Adding new energies to patElectrons
# adding new float variables to the patElectron
# this variables are produced with a specific producer: eleNewEnergiesProducer
# the name of the userFloat is equal to the InputTag passed here
# to access the float:
# electron.userFloat("eleNewEnergiesProducer:energySCEleJoshEle")
# electron.userFloat("eleNewEnergiesProducer:energySCEleJoshEle:MVAntuplizer")

patElectrons.userData.userFloats.src = [
    cms.InputTag("eleNewEnergiesProducer", "energySCEleMust"),
    cms.InputTag("eleNewEnergiesProducer", "energySCEleMustVar"),
    ]


#============================== Adding electron ID to patElectrons
patElectrons.addElectronID=cms.bool(True)
patElectrons.electronIDSources =  cms.PSet(
    # configure many IDs as InputTag <someName> = <someTag> you
    # can comment out those you don't want to save some disk space
    fiducial = cms.InputTag("eleSelectionProducers", "fiducial"),
    WP70PU      = cms.InputTag("eleSelectionProducers", "WP70PU"),
    WP80PU      = cms.InputTag("eleSelectionProducers", "WP80PU"),
    WP90PU      = cms.InputTag("eleSelectionProducers", "WP90PU"),
    loose       = cms.InputTag("eleSelectionProducers", "loose"),
    medium      = cms.InputTag("eleSelectionProducers", "medium"),
    tight      = cms.InputTag("eleSelectionProducers", "tight"),
    loose25nsRun2       = cms.InputTag("eleSelectionProducers", "loose25nsRun2"),
    medium25nsRun2       = cms.InputTag("eleSelectionProducers", "medium25nsRun2"),
    tight25nsRun2       = cms.InputTag("eleSelectionProducers", "tight25nsRun2"),
    loose50nsRun2       = cms.InputTag("eleSelectionProducers", "loose50nsRun2"),
    medium50nsRun2       = cms.InputTag("eleSelectionProducers", "medium50nsRun2"),
    tight50nsRun2       = cms.InputTag("eleSelectionProducers", "tight50nsRun2")
    )

electronMatch.src=cms.InputTag('gedGsfElectrons')

#============================== Adding photon ID to patPhotons
patPhotons.addPhotonID=cms.bool(True)
patPhotons.photonIDSources =  cms.PSet(
    # configure many IDs as InputTag <someName> = <someTag> you
    # can comment out those you don't want to save some disk space
    fiducial = cms.InputTag("phoSelectionProducers", "fiducial"),
    loose       = cms.InputTag("phoSelectionProducers", "loose"),
    medium      = cms.InputTag("phoSelectionProducers", "medium"),
    tight      = cms.InputTag("phoSelectionProducers", "tight"),
    loose25nsRun2       = cms.InputTag("phoSelectionProducers", "loose25nsRun2"),
    medium25nsRun2      = cms.InputTag("phoSelectionProducers", "medium25nsRun2"),
    tight25nsRun2      = cms.InputTag("phoSelectionProducers", "tight25nsRun2"),
    )

photonMatch.src=cms.InputTag('gedPhotons')
muonMatch.src=cms.InputTag('muons')

#process.trackerDrivenRemoverSeq: sequence to remove events with trackerDriven electrons
#process.eleSelectionProducers: produces value maps of floats that says if the electron passes the given selection
#process.eleNewEnergiesProducer: produces value maps of floats with the new calculated electron energy
#process.electronMatch: assosiation map of gsfelectron and genparticle
#process.patElectrons: producer of patElectron
#process.zNtupleDumper: dumper of flat tree for MVA energy training (Francesco Micheli)
prePatSequence = cms.Sequence((eleSelectionProducers + eleNewEnergiesProducer))
patTriggerMatchSeq = cms.Sequence( patTrigger * PatElectronTriggerMatchHLTEle_Ele20SC4Mass50v7 * PatElectronsTriggerMatch * patTriggerEvent ) 
patSequence=cms.Sequence( prePatSequence * patElectrons )
patSequenceMC=cms.Sequence( electronMatch * prePatSequence * patElectrons )


eleNewEnergiesProducer.recHitCollectionEB = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEB")
eleNewEnergiesProducer.recHitCollectionEE = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEE")
eleNewEnergiesProducer.recHitCollectionEB = cms.InputTag("alCaIsolatedElectrons", "alcaBarrelHits")
eleNewEnergiesProducer.recHitCollectionEE = cms.InputTag("alCaIsolatedElectrons", "alcaEndcapHits")

#eleRegressionEnergy.recHitCollectionEB = eleNewEnergiesProducer.recHitCollectionEB.value()
#eleRegressionEnergy.recHitCollectionEE = eleNewEnergiesProducer.recHitCollectionEE.value()
patElectrons.reducedBarrelRecHitCollection = eleNewEnergiesProducer.recHitCollectionEB.value()
patElectrons.reducedEndcapRecHitCollection = eleNewEnergiesProducer.recHitCollectionEE.value()

