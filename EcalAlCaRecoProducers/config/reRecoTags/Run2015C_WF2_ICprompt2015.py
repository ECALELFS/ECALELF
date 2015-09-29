import FWCore.ParameterSet.Config as cms
from CondCore.DBCommon.CondDBSetup_cfi import *

# rereco of cycle Run2015B_WF2
# other infos

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
                               globaltag = cms.string('74X_dataRun2_Prompt_v0'),
                               )
