import os, re
import FWCore.ParameterSet.Config as cms

#restarting from ECAL RAW to reconstruct amplitudes and energies
# create uncalib recHit collections
from Calibration.ALCARAW_RECO.ALCARECOEcalCalIsolElectron_cff import *
from Configuration.StandardSequences.RawToDigi_Data_cff import *
from RecoLocalCalo.Configuration.RecoLocalCalo_cff import *

CMSSW_VERSION=os.getenv("CMSSW_VERSION")

if (re.match("CMSSW_7_2_X",CMSSW_VERSION)):
    from RecoLocalCalo.EcalRecProducers.ecalMultiFitUncalibRecHit_cfi import *
    ecalUncalibRecHitSequence = cms.Sequence(ecalMultiFitUncalibRecHit * ecalDetIdToBeRecovered)
else:
    ecalUncalibRecHitSequence = cms.Sequence(ecalGlobalUncalibRecHit * ecalDetIdToBeRecovered)
        
uncalibRecHitSeq = cms.Sequence( (ecalDigis + ecalPreshowerDigis) * ecalUncalibRecHitSequence)

seqALCARECOEcalUncalElectron = cms.Sequence( uncalibRecHitSeq )

############################################### FINAL SEQUENCES
#seqALCARECOEcalUncalZElectron   = cms.Sequence( ZSkimSeq   *
#                                                (seqALCARECOEcalUncalElectron + ALCARECOEcalCalElectronPreSeq)
#                                                )
#seqALCARECOEcalUncalZSCElectron = cms.Sequence( ZSCSkimSeq *
#                                                (seqALCARECOEcalUncalElectron + ALCARECOEcalCalElectronPreSeq)
#                                                )
#seqALCARECOEcalUncalWElectron   = cms.Sequence( WSkimSeq   *
#                                                (seqALCARECOEcalUncalElectron + ALCARECOEcalCalElectronPreSeq)
#                                                )


