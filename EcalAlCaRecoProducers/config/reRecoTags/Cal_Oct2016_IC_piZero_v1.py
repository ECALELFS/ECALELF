import FWCore.ParameterSet.Config as cms
# Official GT for september re-reco
# IC from piZero tested in August

from CondCore.ESSources.CondDBESSource_cfi import * 
#CondDBConnection.connect = cms.string( 'frontier://FrontierProd/CMS_CONDITIONS' )
RerecoGlobalTag = GlobalTag.clone(
    globaltag = cms.string('80X_dataRun2_2016SeptRepro_v3'),
    toGet = cms.VPSet(
        cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
                 tag = cms.string("IC_fromECALpro_Absolute_EtaRing1"),
                 connect = cms.string("sqlite_file:/afs/cern.ch/user/l/lbrianza/work/public/EoP_aug2016/IC_fromECALpro_Absolute_EtaRing1.db"),
                 )
        ),
    )
