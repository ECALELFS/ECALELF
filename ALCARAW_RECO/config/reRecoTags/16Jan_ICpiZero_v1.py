import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *
###
### Comments
#Ciao Shervin e Luca,
#ho fatto le due tag di pi0 che andrebbero validate sul 2011. Sono entrambe su int e si chiamano:
# a. EcalIntercalibConstants_V20121110_2011pizLP_Global
# b. EcalIntercalibConstants_V20121110_2011pizLP_Jan16EtaScale
# La b e' fatta applicando la scala della Jan16
# (EcalIntercalibConstants_V20120109_Electrons_etaScale/IOV2) alle
# costanti di Luca (IC_Ecal_mol.txt ), assegnando ai cristalli non
# calibrati (-1 999) le stesse IC da cui ricavo la scala e rimettendo
# a 1 la scala globale di EB ed EE separatamente.
# La a e' fatta partendo dalle IC in IC_Ecal_mol_global.txt,
# riempiendo i buchi con le stesse IC di cui parlo al punto precedente
# e rimettendo a 1 la scala globale di EB ed EE separatamente.
# Da quello che so io (Luca correggi se sbaglio) le IC sono state
# derivate sul periodo 174809-178049 con la GT GR_R_42_V24
# Potremmo iniziare a runnare la validazione sul questo
# periodo. Shervin, puoi fare tu i python specificando solo la GT e la
# tag di IC?
# Visto che in EE non vengono calibrati 3 ring vicini ad EB, potremmo
# anche guardare la risoluzione restringendoci a |eta|>1.6? 
### It is based on 16Jan rereco (GT)
###
###
RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'),
                               globaltag = cms.string('GR_R_42_V24::All'),
                               toGet = cms.VPSet(
    cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
             tag = cms.string('EcalIntercalibConstants_V20121110_2011pizLP_Global'),
             connect = cms.untracked.string("frontier://FrontierInt/CMS_COND_ECAL")
             ),
    ),
                               BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )
