import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'),
                               globaltag = cms.string('FT_R_42_V24::All'),
                               toGet = cms.VPSet( ),   # hook to override or add single payloads
                               BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )
