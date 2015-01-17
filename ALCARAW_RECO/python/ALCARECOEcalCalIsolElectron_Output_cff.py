import FWCore.ParameterSet.Config as cms



# output block for alcastream Electron
OutALCARECOEcalCalElectron_specific_ = cms.untracked.vstring(
    'drop reco*Clusters_hfEMClusters_*_*',
    'drop reco*Clusters_pfPhotonTranslator_*_*',
    'drop *EcalRecHit*_ecalRecHit_*_*',
    'drop *EcalrecHit*_*ecalPreshowerRecHit*_*EcalRecHitsES*_*',
    'drop *EcalRecHit*_reducedEcalRecHitsE*_*_*',
    'keep *EcalRecHit*_alCaIsolatedElectrons_*_*',
    'keep *EcalRecHit*_reducedEcalRecHitsES_alCaRecHitsES_*',
    'drop *_*Unclean*_*_*',
    'drop *_*unclean*_*_*',
    'drop *_*_*Unclean*_*',
    'drop *_*_*unclean*_*',
    'drop *CaloCluster*_*particleFlowEGamma*_*EBEEClusters*_*',
    'drop *CaloCluster*_*particleFlowEGamma*_*ESClusters*_*',
    'keep *CaloCluster*_*alCaIsolatedElectrons*_*alcaCaloCluster*_*',
    'drop recoMuon*_muon*_*cosmicsVeto*_*',
    'drop recoMuon*_muon*_*muCorrData*_*',
    'drop recoMuon*_muon*_*muonShowerInformation*_*',
    'drop recoMuon*_muon*_*combined*_*',
    'drop recoMuon*_muon*_*csc*_*',
    'drop recoMuon*_muon*_*dt*_*'
)

OutALCARECOEcalCalElectron_noDrop_ = cms.PSet(
    # put this if you have a filter
    SelectEvents = cms.untracked.PSet(
#    SelectEvents = cms.vstring('pathALCARECOEcalCalZElectron', 'pathALCARECOEcalCalWElectron')
    SelectEvents = cms.vstring('pathALCARECOEcalUncalZElectron', 'pathALCARECOEcalUncalZSCElectron', 'pathALCARECOEcalUncalWElectron')
    ),
    outputCommands = cms.untracked.vstring( 
    'keep *_pfMet_*_*',
    'keep *_kt6PFJetsForRhoCorrection_rho_*',
    'keep *_kt6PFJets_rho_*',
    'keep *_offlinePrimaryVertices*_*_*',
    'keep *BeamSpot_offlineBeamSpot_*_*',
    'keep *_allConversions_*_*',
    'keep *_conversions_*_*',
    'keep *GsfTrack*_*_*_*',
    'keep *_generator_*_*',
    'keep *_addPileupInfo_*_*',
    'keep *_genParticles_*_*',
    'keep recoGsfElectron*_gsfElectron*_*_*',
    'keep recoGsfElectron*_gedGsfElectron*_*_*',
    'keep recoCaloClusters_*_*_*',
    'keep recoSuperClusters_*_*_*',
    'keep recoPreshowerCluster*_*_*_*',
    'keep *_pfElectronTranslator_*_*',
    #'keep *_*_*_HLT',
    #'keep *_generalTracks_*_*',
    #'keep reco*Track*Extra*_generalTracks_*_*',
    'keep *_alcaElectronTracksReducer_*_*',
    # for the trigger matching
    'keep *_l1extraParticles_*_*',
    'keep L1GlobalTriggerReadoutRecord_gtDigis_*_*',
    'keep *_l1L1GtObjectMap_*_*',
    'keep edmConditionsInEventBlock_conditionsInEdm_*_*',
    'keep edmConditionsInLumiBlock_conditionsInEdm_*_*',
    'keep edmConditionsInRunBlock_conditionsInEdm_*_*',
    'keep *_TriggerResults_*_*',
    'keep *_hltTriggerSummaryAOD_*_HLT',
    # pfisolation
    'keep *_elPFIsoValueCharged03PFId*_*_*',
    'keep *_elPFIsoValueGamma03PFId*_*_*',
    'keep *_elPFIsoValueNeutral03PFId*_*_*',
    'keep recoPhoton*_gedPhoton*_*_*',
    'keep recoMuon*_muon*_*_*'
    )
)

import copy
OutALCARECOEcalCalElectron_=copy.deepcopy(OutALCARECOEcalCalElectron_noDrop_)
OutALCARECOEcalCalElectron_.outputCommands.insert(0, "drop *")
OutALCARECOEcalCalElectron_.outputCommands+=OutALCARECOEcalCalElectron_specific_


