import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'),
                               globaltag = cms.string('FT_R_53_V6::All'),
                               toGet = cms.VPSet(
				cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
					tag = cms.string("EcalIntercalibConstants_V20121130_EleABC_2012HR9EtaScaleABC"),
					connect = cms.untracked.string("frontier://FrontierInt/CMS_COND_ECAL")
					)
				,cms.PSet(record = cms.string("EcalADCToGeVConstantRcd"),
					tag = cms.string("EcalADCToGeVConstant_Bon_V20111129"),
					connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
					)
				,cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
					tag = cms.string("EcalLaserAPDPNRatios_20121020_447_p1_v2"),
					connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_42X_ECAL_LAS")
					)
				,cms.PSet(record = cms.string('EcalLaserAlphasRcd'),
					tag = cms.string('EcalLaserAlphas_EB_sic1_btcp152_EE_sic1_btcp116'),
					connect = cms.untracked.string('frontier://FrontierInt/CMS_COND_ECAL')
					)
				),
				BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
				)
