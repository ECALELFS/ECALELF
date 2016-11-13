import FWCore.ParameterSet.Config as cms

from CondCore.CondDB.CondDB_cfi import *

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDB,
                               #connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
                               globaltag = cms.string('80X_prototype'),
                               )
