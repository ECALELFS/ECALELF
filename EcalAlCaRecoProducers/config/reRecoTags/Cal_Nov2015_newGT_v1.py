import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *

#candidate with adc2gev fixed... maybe
RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
                               globaltag = cms.string('74X_dataRun2_Candidate_2015_12_04_11_16_06'),
                               )
