import FWCore.ParameterSet.Config as cms
# Official GT for september re-reco
# IC from from piZero derived on 4fb-1 of 2016

from CondCore.ESSources.CondDBESSource_cfi import * 
#CondDBConnection.connect = cms.string( 'frontier://FrontierProd/CMS_CONDITIONS' )
RerecoGlobalTag = GlobalTag.clone(
    globaltag = cms.string('80X_dataRun2_2016SeptRepro_v3'),
    toGet = cms.VPSet(
        cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
                 tag = cms.string("EcalIntercalibConstants_2016_v1"),
                 connect = cms.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/RunII-IC/Cal_Oct2016/pizero/tags/sqlite/EcalIntercalibConstants_2016_v1.db"),
                 )
        ),
    )
