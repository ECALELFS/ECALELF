import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
                               #     globaltag = cms.string('UNSPECIFIED::All'),
                               globaltag = cms.string('74X_dataRun2_Prompt_v0'),
#                               globaltag = cms.string('START72_V1::All'),
                               #toGet = cms.VPSet( ),   # hook to override or add single payloads
                               #    toGet = cms.VPSet(
                               # cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
                               #          tag = cms.string("EcalIntercalibConstants_v10_offline"),
                               #          connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
                               #         )
                               #  ,cms.PSet(record = cms.string("EcalADCToGeVConstantRcd"),
                               #          tag = cms.string("EcalADCToGeVConstant_v10_offline"),
                               #          connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
                               #         )
                               # ,cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
                               #           tag = cms.string("EcalLaserAPDPNRatios_2011fit_noVPT_nolim_online"),
                               #           tag = cms.string("EcalLaserAPDPNRatios_test_20110625"),
                               #           tag = cms.string("EcalLaserAPDPNRatios_2011V3_online"),
                               #           tag = cms.string("EcalLaserAPDPNRatios_2011mixfit_online"),
                               #          connect = cms.untracked.string("frontier://FrontierPrep/CMS_COND_ECAL")
                               #          )
                               # , cms.PSet(
                               #     record = cms.string('EcalLaserAlphasRcd'),
                               #     tag = cms.string('EcalLaserAlphas_test_prompt'),
                               #     connect = cms.untracked.string('frontier://FrontierProd/CMS_COND_31X_ECAL')
                               #    )
                               #  ),
                               #BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )
