import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *

# Winter13 conditions wo laser
RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'),
                               globaltag = cms.string('GR_R_53_V18::All'),
                               toGet = cms.VPSet(
#     cms.PSet(record = cms.string('EcalIntercalibConstantsRcd'),
#              tag = cms.string('EcalIntercalibConstants_unit'),
#              connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
#              )
    cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
             tag = cms.string('EcalLaserAPDPNRatios_p1p2p3_v2_mc'),
             connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
             )
    ),
                               BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )
