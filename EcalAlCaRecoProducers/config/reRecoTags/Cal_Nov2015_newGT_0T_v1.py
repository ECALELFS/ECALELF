import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *

#candidate used for 0T rereco without updating ECAL conditions

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
                               globaltag = cms.string('74X_dataRun2_2015EOY_Candidate_2016_01_17_21_50_54'),
                               # toGet = cms.VPSet(
                               #                   cms.PSet(record = cms.string("ESIntercalibConstantsRcd"),
                               #                            tag = cms.string("ESIntercalibConstants_V03_offline"),
                               #                            connect = cms.untracked.string("frontier://FrontierPrep/CMS_CONDITIONS"),
                               #                            ),
                               #                   )
                               )
