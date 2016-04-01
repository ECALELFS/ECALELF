import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *
# MC production for 0T 
RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
                               globaltag = cms.string('74X_mcRun2_0T_v0'),
                               )
