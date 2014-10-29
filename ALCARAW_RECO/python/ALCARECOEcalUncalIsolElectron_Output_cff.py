import FWCore.ParameterSet.Config as cms
from Calibration.ALCARAW_RECO.ALCARECOEcalCalIsolElectron_Output_cff import *
import copy

OutALCARECOEcalUncalElectron_noDrop=copy.deepcopy(OutALCARECOEcalCalElectron_noDrop_)
OutALCARECOEcalUncalElectron_noDrop.outputCommands+=cms.untracked.vstring(
    'keep *EcalTriggerPrimitiveDigi*_ecalDigis_*_*',
    'keep *_ecalGlobalUncalibRecHit_*_*',
    'keep *_ecalMultiFitUncalibRecHit_*_*', 
    'keep *_ecalPreshowerDigis_*_*',
    'keep *_ecalDetIdToBeRecovered_*_*',
    'keep reco*Clusters_pfElectronTranslator_*_*'
    )

OutALCARECOEcalUncalElectron=copy.deepcopy(OutALCARECOEcalUncalElectron_noDrop)
OutALCARECOEcalUncalElectron.outputCommands.insert(0, "drop *")
OutALCARECOEcalUncalElectron.outputCommands += cms.untracked.vstring(
    'drop recoCaloClusters_*_*_*',
    'drop recoSuperClusters_*_*_*',
    'drop recoPreshowerCluster*_*_*_*',
    'drop *EcalRecHit*_reducedEcalRecHitsES*_*_*',
    'keep reco*Clusters_pfElectronTranslator_*_*'
    )

OutALCARECOEcalUncalElectron.SelectEvents = cms.untracked.PSet(
    SelectEvents = cms.vstring('pathALCARECOEcalUncalZElectron', 'pathALCARECOEcalUncalZSCElectron', 'pathALCARECOEcalUncalWElectron')
    #SelectEvents = cms.vstring('pathALCARECOEcalUncalZElectron', 'pathALCARECOEcalUncalWElectron')
    )



# OutALCARECOEcalUncalElectron = copy.deepcopy(OutALCARECOEcalCalElectron)
# OutALCARECOEcalUncalElectron.outputCommands +=OutALCARECOEcalUncalElectronOutputCommands

# OutALCARECOEcalUncalElectronOutputCommands = cms.untracked.vstring( [
# #        'drop *_TriggerResults_*_RECO',
# #        'keep *_TriggerResults_*_*',
# #        'keep recoGsfElectrons_gsfElectrons_*_RECO',
# #        'keep recoGsfElectronCores_gsfElectronCores_*_RECO',
#         'keep *EcalTriggerPrimitiveDigi*_ecalDigis_*_*',
#         'keep *_ecalGlobalUncalibRecHit_*_*',
#         'keep *_ecalPreshowerDigis_*_*',
#         'keep *_ecalDetIdToBeRecovered_*_*',
#         # these are not recreated
#         'keep reco*Clusters_pfElectronTranslator_*_*',

# # this are recreated, so they are not needed at this step
#         'drop recoCaloClusters_*_*_*',
#         'drop recoSuperClusters_*_*_*',
#         'drop recoPreshowerCluster*_*_*_*',
#         'drop *EcalRecHit*_reducedEcalRecHitsES*_*_*',
#         # keep the new alca track collections
#      ] )
