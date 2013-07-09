import FWCore.ParameterSet.Config as cms
from CondCore.DBCommon.CondDBSetup_cfi import *

#### Please fill with comments
# Basic tag combination for 2012 end year seminar conditions
# Laser, alpha tags are fixed: no time to improve them
# A set of IC are derived on top of them and will be tested:
# Cal_Nov2012_ICEle_v2:
# description
#

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'),
                               globaltag = cms.string('GR_R_53_V13::All'),
                               toGet = cms.VPSet(
    cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
             tag = cms.string('EcalLaserAPDPNRatios_data_20120814_2011-2012_v3_upd20120919'),
             connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_42X_ECAL_LAS")
             )
    ,cms.PSet(record = cms.string('EcalLaserAlphasRcd'),
              tag = cms.string('EcalLaserAlphas_EB_sic1_btcp152_EE_sic1_btcp116'),
              connect = cms.untracked.string('frontier://FrontierInt/CMS_COND_ECAL')
              )
    ,cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
              tag = cms.string('EcalIntercalibConstants_V20120915_Ele29082012_2012IncEtaScale8TeV'),
              connect = cms.untracked.string("frontier://FrontierInt/CMS_COND_ECAL")
              )
    ),
                               BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )

