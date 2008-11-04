import FWCore.ParameterSet.Config as cms

# output block for alcastream HCAL Isotrk
# output module 
#  module alcastreamHcalIsotrkOutput = PoolOutputModule
OutALCARECOHcalCalIsoTrk = cms.PSet(
    # use this in case of filter available
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOHcalCalIsoTrk')
    ),
    outputCommands = cms.untracked.vstring('drop *', 
        'keep *_IsoProd_*_*',
	'keep triggerTriggerEvent_*_*_*'
)
)

