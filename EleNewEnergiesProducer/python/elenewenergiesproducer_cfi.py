## 
#This module is a producer of ValueMaps that associate to each electron
#in the GsfElectron collection, one float that is a new type of energy
#associated to the electron. 
#The core of the module is the plugin EleNewEnergiesProducer.cc whose
#cfi.py file is elenewenergiesproducer_cfi.py
#
#The plugin just create the new collection, loop over the gsfElectron
#collection and calculate the new energy according to a separate class
#that should called by the plugin. The implementation of the
#calculation of the new energy should not be done in the plugin but in
#a separate class in order to keep it more general and flexible.
#The separate class can be added in the src directory or already
#present in some other CMSSW package (add the required lines in the
#BuildFile.xml).
#
#
#You should copy the files indicated in data/copy.url in the data/ directory
##
import FWCore.ParameterSet.Config as cms

# ALCARECO collections
eleNewEnergiesProducer = cms.EDProducer('EleNewEnergiesProducer',
                                        electronCollection = cms.InputTag('gedGsfElectrons'),
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
                                        ptSplit = cms.bool(True),
                                        regrEleJoshV4_SemiParamFile = cms.string('root://eoscms.cern.ch//eos/cms/store/caf/user/shervin/GBRLikelihoodEGToolsData/regweights_v4_forest_ele.root'),
                                        regrPhoJoshV4_SemiParamFile = cms.string('root://eoscms.cern.ch//eos/cms/store/caf/user/shervin/GBRLikelihoodEGToolsData/regweights_v4_forest_ph.root'),
                                        regrEleJoshV5_SemiParamFile = cms.string('root://eoscms.cern.ch//eos/cms/store/caf/user/shervin/GBRLikelihoodEGToolsData/regweights_v5_forest_ele.root'),
                                        regrPhoJoshV5_SemiParamFile = cms.string('root://eoscms.cern.ch//eos/cms/store/caf/user/shervin/GBRLikelihoodEGToolsData/regweights_v5_forest_ph.root'),
                                        regrEleJoshV6_SemiParamFile = cms.string('root://eoscms.cern.ch//eos/cms/store/caf/user/shervin/GBRLikelihoodEGToolsData/regweights_v6_8TeV_forest_ele.root'),
                                        regrPhoJoshV6_SemiParamFile = cms.string('root://eoscms.cern.ch//eos/cms/store/caf/user/shervin/GBRLikelihoodEGToolsData/regweights_v6_8TeV_forest_ph.root'),
                                        regrEleJoshV7_SemiParamFile = cms.string('root://eoscms.cern.ch//eos/cms/store/caf/user/shervin/GBRLikelihoodEGToolsData/regweights_v7_8TeV_forest_ele.root'),
                                        regrPhoJoshV7_SemiParamFile = cms.string('root://eoscms.cern.ch//eos/cms/store/caf/user/shervin/GBRLikelihoodEGToolsData/regweights_v7_8TeV_forest_ph.root'),
                                        regrEleJoshV8_SemiParamFile = cms.string('root://eoscms.cern.ch//eos/cms/store/caf/user/shervin/GBRLikelihoodEGToolsData/regweights_v8_8TeV_forest_ele.root'),
                                        regrPhoJoshV8_SemiParamFile = cms.string('root://eoscms.cern.ch//eos/cms/store/caf/user/shervin/GBRLikelihoodEGToolsData/regweights_v8_8TeV_forest_ph.root'),
                                        regrEleJoshV6_SemiParam7TeVtrainFile = cms.string('root://eoscms.cern.ch//eos/cms/store/caf/user/shervin/GBRLikelihoodEGToolsData/regweights_v6_7TeV_forest_ele.root'),
                                        regrPhoJoshV6_SemiParam7TeVtrainFile = cms.string('root://eoscms.cern.ch//eos/cms/store/caf/user/shervin/GBRLikelihoodEGToolsData/regweights_v6_7TeV_forest_ph.root'),
                                        regrEleJoshV7_SemiParam7TeVtrainFile = cms.string('root://eoscms.cern.ch//eos/cms/store/caf/user/shervin/GBRLikelihoodEGToolsData/regweights_v7_7TeV_forest_ele.root'),
                                        regrPhoJoshV7_SemiParam7TeVtrainFile = cms.string('root://eoscms.cern.ch//eos/cms/store/caf/user/shervin/GBRLikelihoodEGToolsData/regweights_v7_7TeV_forest_ph.root'),
                                        regrEleJoshV8_SemiParam7TeVtrainFile = cms.string('root://eoscms.cern.ch//eos/cms/store/caf/user/shervin/GBRLikelihoodEGToolsData/regweights_v8_7TeV_forest_ele.root'),
                                        regrPhoJoshV8_SemiParam7TeVtrainFile = cms.string('root://eoscms.cern.ch//eos/cms/store/caf/user/shervin/GBRLikelihoodEGToolsData/regweights_v8_7TeV_forest_ph.root'),

                                        )
