import FWCore.ParameterSet.Config as cms

# ALCARECO collections
eleNewEnergiesProducer = cms.EDProducer('EleNewEnergiesProducer',
                                        electronCollection = cms.InputTag('gsfElectrons'),
                                        recHitCollectionEB = cms.InputTag("alCaIsolatedElectrons", "alcaBarrelHits"),
                                        recHitCollectionEE = cms.InputTag("alCaIsolatedElectrons", "alcaEndcapHits"),
                                        rhoFastJet = cms.InputTag('kt6PFJetsForRhoCorrection',"rho"),
                                        vertexCollection = cms.InputTag('offlinePrimaryVerticesWithBS'),
                                        BeamSpotCollection = cms.InputTag('offlineBeamSpot'),
                                        conversionCollection = cms.InputTag('allConversions'),
                                        isMC = cms.bool(False),
                                        regrPhoFile = cms.string('./data/gbrv3ph_52x.root'),
                                        regrEleFile = cms.string('./data/gbrv3ele_52x.root'),
                                        regrEleFile_fra = cms.string('nocorrection.root'),
                                        correctionFileName = cms.string(''),
                                        correctionType = cms.string(''),
                                        ptSplit = cms.bool(True)
                                        
                                        )
