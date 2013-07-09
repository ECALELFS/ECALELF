import FWCore.ParameterSet.Config as cms
from CondCore.DBCommon.CondDBSetup_cfi import *

#### Please fill with comments
# Basic tag combination for 2012 end year seminar conditions
# Conditions used by Jarry to determine Alpha




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
              tag = cms.string('EcalIntercalibConstants_V1_express'),
              connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
              )
    ,cms.PSet(record = cms.string("EcalADCToGeVConstantRcd"),
              tag = cms.string('EcalADCToGeVConstant_V1_express'),
              connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
              )
    ),
                               BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )

