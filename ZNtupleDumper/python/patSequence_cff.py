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
'''
patElectrons.userData.userFloats.src = [
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEle"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleVar"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPho"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoVar"),
    cms.InputTag("eleNewEnergiesProducer",    "energyEleFra" ),
    cms.InputTag("eleNewEnergiesProducer",    "energyEleFraVar" ),
    cms.InputTag("eleNewEnergiesProducer",    "energyEleFra" ),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV4ecorr"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV4sigma"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV4alpha1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV4gamma1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV4alpha2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV4gamma2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV4pdfval"),
                                                                            
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV5ecorr"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV5sigma"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV5alpha1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV5gamma1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV5alpha2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV5gamma2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV5pdfval"),
                                                                            
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV4ecorr"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV4sigma"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV4alpha1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV4gamma1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV4alpha2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV4gamma2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV4pdfval"),
                                                                            
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV5ecorr"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV5sigma"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV5alpha1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV5gamma1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV5alpha2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV5gamma2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV5pdfval"),
    ###############
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV6ecorr"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV6sigmaEoverE"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV6cbmean"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV6sigma"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV6alpha1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV6gamma1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV6alpha2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV6gamma2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV6pdfval"),

    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV6ecorr"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV6sigmaEoverE"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV6cbmean"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV6sigma"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV6alpha1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV6gamma1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV6alpha2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV6gamma2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV6pdfval"),
    ###############
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV7ecorr"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV7sigmaEoverE"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV7cbmean"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV7sigma"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV7alpha1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV7gamma1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV7alpha2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV7gamma2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV7pdfval"),

    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV7ecorr"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV7sigmaEoverE"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV7cbmean"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV7sigma"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV7alpha1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV7gamma1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV7alpha2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV7gamma2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV7pdfval"),
    ###############
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV8ecorr"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV8sigmaEoverE"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV8cbmean"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV8sigma"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV8alpha1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV8gamma1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV8alpha2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV8gamma2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParamV8pdfval"),

    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV8ecorr"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV8sigmaEoverE"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV8cbmean"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV8sigma"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV8alpha1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV8gamma1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV8alpha2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV8gamma2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParamV8pdfval"),

    ###############
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV6ecorr"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV6sigmaEoverE"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV6cbmean"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV6sigma"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV6alpha1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV6gamma1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV6alpha2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV6gamma2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV6pdfval"),

    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV6ecorr"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV6sigmaEoverE"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV6cbmean"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV6sigma"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV6alpha1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV6gamma1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV6alpha2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV6gamma2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV6pdfval"),
    ###############
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV7ecorr"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV7sigmaEoverE"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV7cbmean"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV7sigma"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV7alpha1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV7gamma1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV7alpha2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV7gamma2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV7pdfval"),

    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV7ecorr"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV7sigmaEoverE"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV7cbmean"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV7sigma"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV7alpha1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV7gamma1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV7alpha2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV7gamma2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV7pdfval"),
    ###############
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV8ecorr"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV8sigmaEoverE"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV8cbmean"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV8sigma"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV8alpha1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV8gamma1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV8alpha2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV8gamma2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshEleSemiParam7TeVtrainV8pdfval"),

    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV8ecorr"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV8sigmaEoverE"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV8cbmean"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV8sigma"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV8alpha1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV8gamma1"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV8alpha2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV8gamma2"),
    cms.InputTag("eleNewEnergiesProducer",    "energySCEleJoshPhoSemiParam7TeVtrainV8pdfval"),

    cms.InputTag("eleRegressionEnergy:eneRegForGsfEle"),
    cms.InputTag("eleRegressionEnergy", "eneErrorRegForGsfEle")
    ]
'''

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

patTriggerMatchSeq = cms.Sequence( patTrigger * PatElectronTriggerMatchHLTEle_Ele20SC4Mass50v7 * PatElectronsTriggerMatch * patTriggerEvent ) 
patSequence=cms.Sequence( (eleSelectionProducers + eleNewEnergiesProducer) * patElectrons )
patSequenceMC=cms.Sequence( electronMatch * (eleSelectionProducers + eleNewEnergiesProducer) * patElectrons )


eleNewEnergiesProducer.recHitCollectionEB = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEB")
eleNewEnergiesProducer.recHitCollectionEE = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEE")
eleNewEnergiesProducer.recHitCollectionEB = cms.InputTag("alCaIsolatedElectrons", "alcaBarrelHits")
eleNewEnergiesProducer.recHitCollectionEE = cms.InputTag("alCaIsolatedElectrons", "alcaEndcapHits")

#eleRegressionEnergy.recHitCollectionEB = eleNewEnergiesProducer.recHitCollectionEB.value()
#eleRegressionEnergy.recHitCollectionEE = eleNewEnergiesProducer.recHitCollectionEE.value()
patElectrons.reducedBarrelRecHitCollection = eleNewEnergiesProducer.recHitCollectionEB.value()
patElectrons.reducedEndcapRecHitCollection = eleNewEnergiesProducer.recHitCollectionEE.value()

