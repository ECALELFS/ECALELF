import FWCore.ParameterSet.Config as cms
from TrackingTools.TransientTrack.TransientTrackBuilder_cfi import *
from Configuration.StandardSequences.MagneticField_cff import *

#------------------------------ pattuple
from calibration.ZNtupleDumper.elePat_cfi import *
#process.patElectrons.electronSource = cms.InputTag("gsfElectrons")
#process.patElectrons.addElectronID = cms.bool(False)
#process.patElectrons.addGenMatch = cms.bool(True)
#process.patElectrons.pvSrc = cms.InputTag("offlinePrimaryVerticesWithBS")
#print process.patElectrons.reducedBarrelRecHitCollection
    
#------------------------------ new energies
from calibration.EleNewEnergiesProducer.elenewenergiesproducer_cfi import *
eleNewEnergiesProducer.regrPhoFile='src/Calibration/EleNewEnergiesProducer/data/gbrv3ph_52x.root'
eleNewEnergiesProducer.regrEleFile='src/Calibration/EleNewEnergiesProducer/data/gbrv3ele_52x.root'
eleNewEnergiesProducer.regrEleFile_fra='src/Calibration/EleNewEnergiesProducer/data/eleEnergyRegWeights_V1.root'
eleNewEnergiesProducer.ptSplit=cms.bool(False)

from EgammaAnalysis.ElectronTools.electronRegressionEnergyProducer_cfi import *
eleRegressionEnergy.inputElectronsTag = cms.InputTag('gsfElectrons')
eleRegressionEnergy.inputCollectionType = cms.uint32(0)
eleRegressionEnergy.useRecHitCollections = cms.bool(True)
eleRegressionEnergy.produceValueMaps = cms.bool(True)
eleRegressionEnergy.rhoCollection = cms.InputTag('kt6PFJetsForRhoCorrection',"rho")
eleRegressionEnergy.vertexCollection = cms.InputTag('offlinePrimaryVertices')

#------------------------------ electronID producer
from calibration.EleSelectionProducers.eleselectionproducers_cfi import *
# process.EleSelectionProducers

#============================== Adding new energies to patElectrons
# adding new float variables to the patElectron
# this variables are produced with a specific producer: eleNewEnergiesProducer
# the name of the userFloat is equal to the InputTag passed here
# to access the float:
# electron.userFloat("eleNewEnergiesProducer:energySCEleJoshEle")
# electron.userFloat("eleNewEnergiesProducer:energySCEleJoshEle:MVAntuplizer")
patElectrons.userData.userFloats.src = [
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEle"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleVar"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPho"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoVar"),
    cms.InputTag("eleNewEnergiesProducer",    "energyEleFra" ),
    cms.InputTag("eleNewEnergiesProducer",    "energyEleFraVar" ),
    cms.InputTag("eleRegressionEnergy:eneRegForGsfEle"),
    cms.InputTag("eleRegressionEnergy", "eneErrorRegForGsfEle")
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
    tight      = cms.InputTag("eleSelectionProducers", "tight")
    )


#process.trackerDrivenRemoverSeq: sequence to remove events with trackerDriven electrons
#process.eleSelectionProducers: produces value maps of floats that says if the electron passes the given selection
#process.eleNewEnergiesProducer: produces value maps of floats with the new calculated electron energy
#process.electronMatch: assosiation map of gsfelectron and genparticle
#process.patElectrons: producer of patElectron
#process.zNtupleDumper: dumper of flat tree for MVA energy training (Francesco Micheli)

patTriggerMatchSeq = cms.Sequence( patTrigger * PatElectronTriggerMatchHLTEle_Ele20SC4Mass50v7 * PatElectronsTriggerMatch * patTriggerEvent ) 
patSequence=cms.Sequence( (eleSelectionProducers + eleNewEnergiesProducer +eleRegressionEnergy ) * patElectrons )
patSequenceMC=cms.Sequence( electronMatch * (eleSelectionProducers + eleNewEnergiesProducer + eleRegressionEnergy ) * patElectrons )


eleNewEnergiesProducer.recHitCollectionEB = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEB")
eleNewEnergiesProducer.recHitCollectionEE = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEE")
eleNewEnergiesProducer.recHitCollectionEB = cms.InputTag("alCaIsolatedElectrons", "alcaBarrelHits")
eleNewEnergiesProducer.recHitCollectionEE = cms.InputTag("alCaIsolatedElectrons", "alcaEndcapHits")

eleRegressionEnergy.recHitCollectionEB = eleNewEnergiesProducer.recHitCollectionEB.value()
eleRegressionEnergy.recHitCollectionEE = eleNewEnergiesProducer.recHitCollectionEE.value()
patElectrons.reducedBarrelRecHitCollection = eleNewEnergiesProducer.recHitCollectionEB.value()
patElectrons.reducedEndcapRecHitCollection = eleNewEnergiesProducer.recHitCollectionEE.value()

