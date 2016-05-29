import FWCore.ParameterSet.Config as cms

from CondCore.CondDB.CondDB_cfi import *
# global tag for 80X MC production
RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDB,
                               connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
                               globaltag = cms.string('80X_mcRun2_asymptotic_2016_miniAODv2'),
                               )
