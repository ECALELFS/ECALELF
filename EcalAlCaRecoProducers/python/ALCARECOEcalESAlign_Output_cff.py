import FWCore.ParameterSet.Config as cms


OutALCARECOEcalESAlign_noDrop = cms.PSet(
    # put this if you have a filter
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOEcalESAlign')
    ),
    outputCommands = cms.untracked.vstring(
        'keep *_ecalPreshowerRecHit_*_*',
        'keep *_generalTracks_*_*',
        'keep *_offlineBeamSpot_*_*',
        'keep *_siPixelClusters_*_*',
        'keep *_siStripClusters_*_*',
        'keep *_siStripDigis_*_*'
        )
)


import copy
OutALCARECOEcalESAlign=copy.deepcopy(OutALCARECOEcalESAlign_noDrop)
OutALCARECOEcalESAlign.outputCommands.insert(0,"drop *")
