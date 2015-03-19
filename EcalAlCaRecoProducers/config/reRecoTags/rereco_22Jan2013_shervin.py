import FWCore.ParameterSet.Config as cms
from CondCore.DBCommon.CondDBSetup_cfi import *

# 22Jan rereco GT:
#http://cms-conddb.cern.ch/gtlist/?GlobalTag=FT_R_53_V21

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'),
                               globaltag = cms.string('FT_R_53_V21::All'), 
                               BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )
