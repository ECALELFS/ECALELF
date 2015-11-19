import FWCore.ParameterSet.Config as cms

zNtupleDumper = cms.EDAnalyzer('ZNtupleDumper',
                               jsonFileName = cms.string(""),
                               electronCollection = cms.InputTag('patElectrons'),
                               muonCollection = cms.InputTag('patMuons'),
                               photonCollection = cms.InputTag('patPhotons'),
                               #recHitCollectionEB = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEB"),
                               #recHitCollectionEE = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEE"),
                               recHitCollectionEB = cms.InputTag("alCaIsolatedElectrons", "alcaBarrelHits"),
                               recHitCollectionEE = cms.InputTag("alCaIsolatedElectrons", "alcaEndcapHits"),
															 recHitCollectionES = cms.InputTag("ecalPreshowerRecHit", "EcalRecHitsES"),
                               EESuperClusterCollection = cms.InputTag("correctedMulti5x5SuperClustersWithPreshower"),
                               rhoFastJet = cms.InputTag('kt6PFJetsForRhoCorrection',"rho"),
                               vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                               BeamSpotCollection = cms.InputTag('offlineBeamSpot'),
                               conversionCollection = cms.InputTag('allConversions'),
                               metCollection = cms.InputTag('pfMet'),
                               triggerResultsCollection = cms.InputTag('TriggerResults::HLT'),
                               foutName = cms.string("ZShervinNtuple.root"),
                               doStandardTree = cms.bool(True),
                               doExtraCalibTree = cms.bool(False),
                               doEleIDTree = cms.bool(False),
#                               pdfWeightCollections = cms.VInputTag(cms.InputTag('pdfWeights:cteq66'), cms.InputTag("pdfWeights:MRST2006nnlo"), cms.InputTag('pdfWeights:NNPDF10')),
                               pdfWeightCollections = cms.VInputTag(),
                               fsrWeightCollection = cms.InputTag("fsrWeight"),
                               weakWeightCollection = cms.InputTag("weakWeight"),
                               doHighEta_LowerEtaCut = cms.double(2.4),
                               isPartGun = cms.bool(False),
                               hltPaths = cms.vstring(),
#                               hltPaths = cms.vstring('HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v15',
#                                                      'HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v16',
#                                                      'HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v17',
#                                                      'HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v18',
#                                                      'HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v19'
#                                                      ),
                               #SelectEvents contains the list of ALCARECO paths you want to keep in your ntuple
                               # the path you want to keep has to be in the TriggerResults indicated by WZSkimResultsCollection
                               SelectEvents = cms.vstring(''),
                               ### This is the collection with the ALCARECO paths, identifying the skim selection
                               #WZSkimResultsCollection = cms.InputTag('TriggerResults::ALCARECO'),
                               #WZSkimResultsCollection = cms.InputTag('TriggerResults::ALCA'),
                               WZSkimResultsCollection = cms.InputTag('TriggerResults::RECO'),
                               #isMC = cms.bool(False),
                               #                      jsonFile = cms.string(options.json),
                               #puWeightFile = cms.string('')
                               #                      R9WeightFile = cms.string(options.R9WeightFile),
                                    #                      regrPhoFile = cms.string(regrPhoFile),
                                    #                      regrEleFile = cms.string(regrEleFile),
                                    #                      r9weightsFile = cms.string('./config/R9Weight.root'),
                                    #                      correctionFileName = cms.string(options.correctionFileName),
                                    #                      correctionType = cms.string(options.correctionType),
                                    #                      oddEventFilter = cms.bool(oddEventFilter)
                               eleID_loose = cms.string("loose25nsRun2"),
                               eleID_medium= cms.string("medium25nsRun2"),
                               eleID_tight = cms.string("tight25nsRun2"),
                               )
