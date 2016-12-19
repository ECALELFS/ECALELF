import FWCore.ParameterSet.Config as cms
# Official GT for september re-reco
# pedestal tags produced by Jean for pedestal studies:
# 6a9a2818932fce79d8222768ba4f2ad3f60f894c payload is used (first Bon pedestal run of 2016, Apr)
# then variations of ADC values are performed (all channels up or down)
# here's the complete list of tags: 
# EcalPedestals_test_-0.1_-0.1
# EcalPedestals_test_-0.3_-0.5
# EcalPedestals_test_-0.5_-1
# EcalPedestals_test_-1.5_-5
# EcalPedestals_test_-1_-3
# EcalPedestals_test_-2_-10
# EcalPedestals_test_0.1_0.1
# EcalPedestals_test_0.3_0.5
# EcalPedestals_test_0.5_1
# EcalPedestals_test_1.5_5
# EcalPedestals_test_1_3
# EcalPedestals_test_2_10


from CondCore.ESSources.CondDBESSource_cfi import * 
#CondDBConnection.connect = cms.string( 'frontier://FrontierProd/CMS_CONDITIONS' )
RerecoGlobalTag = GlobalTag.clone(
    globaltag = cms.string('80X_dataRun2_2016SeptRepro_v3'),
    toGet = cms.VPSet(
        cms.PSet(record = cms.string("EcalPedestalsRcd"),
                 tag = cms.string("EcalPedestals_test_0_0"),
                 connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
                 ),
        ),
    )
