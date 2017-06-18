import FWCore.ParameterSet.Config as cms
# Official GT for september re-reco
# pedestal tags produced by Jean for pedestal studies:
# 6a9a2818932fce79d8222768ba4f2ad3f60f894c payload is used (first Bon pedestal run of 2016, Apr)


from CondCore.ESSources.CondDBESSource_cfi import * 
#CondDBConnection.connect = cms.string( 'frontier://FrontierProd/CMS_CONDITIONS' )
RerecoGlobalTag = GlobalTag.clone(
    globaltag = cms.string('80X_dataRun2_2016SeptRepro_v3'),
    toGet = cms.VPSet(
        cms.PSet(record = cms.string("EcalPedestalsRcd"),
                 tag = cms.string("EcalPedestals_test_0_0"),
                 connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
                 ),
        cms.PSet(
            record = cms.string('EcalLaserAlphasRcd'),
            tag = cms.string('EcalLaserAlphas_resolution_v1'),
            connect = cms.string('frontier://FrontierPrep/CMS_CONDITIONS')
            ),
        ),
    )
