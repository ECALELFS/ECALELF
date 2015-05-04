import FWCore.ParameterSet.Config as cms
from CondCore.DBCommon.CondDBSetup_cfi import *

#### Please fill with comments
# Basic tag combination for 2012 end year seminar conditions
# Laser, alpha tags are fixed: no time to improve them
# A set of IC are derived on top of them and will be tested:
# Cal_Nov2012_ICpiZero_v1:
# description
#

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'),
                               globaltag = cms.string('FT_R_53_V21::All'),
                               toGet = cms.VPSet(
    cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
             tag = cms.string('EcalLaserAPDPNRatios_20130130_447_p1_v2'),
             connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_42X_ECAL_LAS")
             )
    ,cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
              tag = cms.string('EcalIntercalibConstants_V20131118_2012C_Pi0_Eta_Comb_Calt_ScaleHR9ABC'),
              connect = cms.untracked.string("frontier://FrontierInt/CMS_COND_ECAL")
              )
    ),
                               BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )

