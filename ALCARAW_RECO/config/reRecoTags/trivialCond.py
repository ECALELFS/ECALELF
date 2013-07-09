import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'),
                               #     globaltag = cms.string('UNSPECIFIED::All'),
                               globaltag = cms.string('::All'),
                               toGet = cms.VPSet(
    cms.PSet(record = cms.string('EcalIntercalibConstantsRcd'),
             tag = cms.string('EcalIntercalibConstants_unit'),
             connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
             )
                               #  ,cms.PSet(record = cms.string("EcalADCToGeVConstantRcd"),
                               #          tag = cms.string("EcalADCToGeVConstant_v10_offline"),
                               #          connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
                               #         )
    ,cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
              tag = cms.string('EcalLaserAPDPNRatios_p1p2p3_v2_mc'),
              connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
              )
                               # , cms.PSet(
                               #     record = cms.string('EcalLaserAlphasRcd'),
                               #     tag = cms.string('EcalLaserAlphas_test_prompt'),
                               #     connect = cms.untracked.string('frontier://FrontierProd/CMS_COND_31X_ECAL')
                               #    )
    ),
                               BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )
