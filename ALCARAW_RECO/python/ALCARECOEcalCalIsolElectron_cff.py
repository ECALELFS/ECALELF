import FWCore.ParameterSet.Config as cms

from Calibration.ALCARAW_RECO.alCaIsolatedElectrons_cfi import *
from Calibration.ALCARAW_RECO.AlCaElectronTracksReducer_cfi import *
from Calibration.ALCARAW_RECO.eleIsoSequence_cff import *

from RecoJets.Configuration.RecoPFJets_cff import kt6PFJets
kt6PFJetsForRhoCorrection = kt6PFJets.clone(doRhoFastjet = True)
kt6PFJetsForRhoCorrection.Rho_EtaMax = cms.double(2.5)

alcarecoEcalRecHitReducerSeq = cms.Sequence(alCaIsolatedElectrons)
alcarecoElectronTracksReducerSeq = cms.Sequence(alcaElectronTracksReducer)

seqALCARECOEcalCalElectronRECO = cms.Sequence( alCaIsolatedElectrons)

ALCARECOEcalCalElectronPreSeq = cms.Sequence( kt6PFJetsForRhoCorrection +
                                              alcaElectronTracksReducer +
                                              pfiso )

seqALCARECOEcalCalElectron = cms.Sequence( ALCARECOEcalCalElectronPreSeq +
                                           seqALCARECOEcalCalElectronRECO
                                           )


############################################### FINAL SEQUENCES
#seqALCARECOEcalCalZElectron = cms.Sequence( ZSkimSeq * seqALCARECOEcalCalElectron)
#seqALCARECOEcalCalWElectron = cms.Sequence( WSkimSeq  * seqALCARECOEcalCalElectron)
#seqALCARECOEcalCalZSCElectron = cms.Sequence( ZSCSkimSeq  * seqALCARECOEcalCalElectron)



