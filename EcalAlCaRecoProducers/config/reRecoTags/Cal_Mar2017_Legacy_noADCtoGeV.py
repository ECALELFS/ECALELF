import FWCore.ParameterSet.Config as cms
# Candidate GT with most updated conditions for legacy rereco of 2016 data
# it does not include the ADCtoGeV (this rereco is done to derive the ADCtoGeV)

from CondCore.ESSources.CondDBESSource_cfi import * 
#CondDBConnection.connect = cms.string( 'frontier://FrontierProd/CMS_CONDITIONS' )
RerecoGlobalTag = GlobalTag.clone(
#    globaltag = cms.string('80X_dataRun2_2016LegacyRepro_Candidate_v2'),
    globaltag = cms.string('80X_dataRun2_2016Repro_Candidate_2017_04_05_22_30_05'),
    toGet = cms.VPSet(
        cms.PSet(record = cms.string("EcalPedestalsRcd"),
                 tag = cms.string("EcalPedestals_Legacy2016_time_v1"), # in queue but not submitted by me, so not included in candidate gt
                 connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
                 ),
        cms.PSet(record = cms.string("EcalPulseShapesRcd"),
                 tag = cms.string("EcalPulseShapes_Legacy2016_v2"), # in DB, not yet in queue
                 connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
                 ),
        cms.PSet(record = cms.string("ESIntercalibConstantsRcd"),
                 tag = cms.string("ESIntercalibConstants_Run1_Run2_V07_offline"),
                 connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
                 ),
        cms.PSet(record = cms.string("ESEEIntercalibConstantsRcd"),
                 tag = cms.string("ESEEIntercalibConstants_Legacy2016_v3"),
                 connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
                 ),
        ),
    )
