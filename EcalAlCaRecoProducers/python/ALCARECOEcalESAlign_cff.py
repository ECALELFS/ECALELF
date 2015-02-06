import FWCore.ParameterSet.Config as cms

# this imports the module that produces a reduced collections for ES alignment
#from Calibration.EcalAlCaRecoProducers.EcalAlCaESAlignTrackReducer import *

# this imports the filter that skims the events requiring a min number of selected tracks

EcalESAlignTracksSkimSeq = cms.Sequence()
#EcalESAlignTracksSkimSeq = cms.Sequence( EcalAlCaESAlignTrackReducer * EcalAlCaESAlignSkimFilter )


seqEcalESAlign = cms.Sequence(EcalESAlignTracksSkimSeq)

