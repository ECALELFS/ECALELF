import FWCore.ParameterSet.Config as cms
# Official GT for september re-reco
# pedestal tags produced by Jean for pedestal studies:
# 6a9a2818932fce79d8222768ba4f2ad3f60f894c payload is used (first Bon pedestal run of 2016, Apr)


from CondCore.ESSources.CondDBESSource_cfi import * 
#CondDBConnection.connect = cms.string( 'frontier://FrontierProd/CMS_CONDITIONS' )
RerecoGlobalTag = GlobalTag.clone(
    globaltag = cms.string('80X_dataRun2_2016LegacyRepro_Candidate_v2'),
    toGet = cms.VPSet(
        cms.PSet(record = cms.string("ESIntercalibConstantsRcd"),
                 tag = cms.string("ESIntercalibConstants_Run1_Run2_V07_offline"),
                 connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
                 ),
        ),
    )
