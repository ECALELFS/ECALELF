import FWCore.ParameterSet.Config as cms
from CondCore.DBCommon.CondDBSetup_cfi import *

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'),
                               globaltag = cms.string('FT_R_53_V21::All'),
                               toGet = cms.VPSet(
    cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
             tag = cms.string('EcalIntercalibConstants_V20130808_2012ABCD_offline_5x5Zee'),
             connect = cms.untracked.string("frontier://FrontierInt/CMS_COND_ECAL")
             )
    ),
                               BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )

