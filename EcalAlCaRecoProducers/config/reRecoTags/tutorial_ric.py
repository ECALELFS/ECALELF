import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'),
                               #     globaltag = cms.string('UNSPECIFIED::All'),
                               globaltag = cms.string('FT_R_53_V6::All'),
                               #toGet = cms.VPSet( ),   # hook to override or add single payloads
                               #    toGet = cms.VPSet(
                                cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
                                         tag = cms.string("EcalIntercalibConstants_V20121028_2012ABCpizphiele_2012LR9EtaScale"),
                                         connect = cms.untracked.string("frontier://FrontierInt/CMS_COND_ECAL")
                                        )
                                 ,cms.PSet(record = cms.string("EcalADCToGeVConstantRcd"),
                                         tag = cms.string("EcalADCToGeVConstant_Bon_V20111129"),
                                         connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
                                        )
                                ,cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
                                          tag = cms.string("EcalLaserAPDPNRatios_data_20120814_2011-2012_v3_upd20120919"),
                               #           tag = cms.string("EcalLaserAPDPNRatios_test_20110625"),
                               #           tag = cms.string("EcalLaserAPDPNRatios_2011V3_online"),
                               #           tag = cms.string("EcalLaserAPDPNRatios_2011mixfit_online"),
                                         connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_42X_ECAL_LAS")
                                         )
                                , cms.PSet(
                                    record = cms.string('EcalLaserAlphasRcd'),
                                    tag = cms.string('EcalLaserAlphas_EB_sic1_btcp152_EE_sic1_btcp116'),
                                    connect = cms.untracked.string('frontier://FrontierInt/CMS_COND_ECAL')
                                   )
                                 ),
                               BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )
