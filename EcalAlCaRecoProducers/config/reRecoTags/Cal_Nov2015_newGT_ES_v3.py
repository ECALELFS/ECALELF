import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *

#candidate used for final 2015 rereco + ES intercalib without data/MC corrections
RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
                               globaltag = cms.string('74X_dataRun2_Candidate_2015_12_04_11_16_06'),
                               toGet = cms.VPSet(
                                                 cms.PSet(record = cms.string("ESIntercalibConstantsRcd"),
                                                          tag = cms.string("ESIntercalibConstants_V03_offline"),
                                                          connect = cms.untracked.string("frontier://FrontierProd/CMS_CONDITIONS"),
                                                          ),
                                                 cms.PSet(record = cms.string("ESEEIntercalibConstantsRcd"),
                                                          tag = cms.string("ESEEIntercalibConstants_LG_offline_data_test_V1"),
                                                          connect = cms.untracked.string("frontier://FrontierProd/CMS_CONDITIONS"),
                                                          ),
                                                 )
                               )
