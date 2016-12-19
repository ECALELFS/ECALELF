import FWCore.ParameterSet.Config as cms
# Official GT for september re-reco


from CondCore.ESSources.CondDBESSource_cfi import * 
#CondDBConnection.connect = cms.string( 'frontier://FrontierProd/CMS_CONDITIONS' )
RerecoGlobalTag = GlobalTag.clone(
    globaltag = cms.string('80X_dataRun2_2016SeptRepro_v4'),
    toGet = cms.VPSet(
        cms.PSet(record = cms.string("EcalTimeCalibConstantsRcd"),
                 tag = cms.string("EcalIntercalibConstants_Cal_Nov2016_v3"),
                 connect = cms.string("frontier://FrontierPrep/CMS_CONDITIONS"),
                 ),
        ),
    )
