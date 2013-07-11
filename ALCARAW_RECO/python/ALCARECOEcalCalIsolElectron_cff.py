import FWCore.ParameterSet.Config as cms

from Calibration.ALCARAW_RECO.alCaIsolatedElectrons_cfi import *
from Calibration.ALCARAW_RECO.AlCaElectronTracksReducer_cfi import *

alcarecoEcalRecHitReducerSeq = cms.Sequence(alCaIsolatedElectrons)
alcarecoElectronTracksReducerSeq = cms.Sequence(alcaElectronTracksReducer)
seqALCARECOEcalCalElectronRECO = cms.Sequence(alCaIsolatedElectrons + alcaElectronTracksReducer)


