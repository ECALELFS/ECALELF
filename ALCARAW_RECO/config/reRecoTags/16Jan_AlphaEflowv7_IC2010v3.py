import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'),
                               #     globaltag = cms.string('UNSPECIFIED::All'),
                               globaltag = cms.string('GR_R_42_V24::All'),
                               #toGet = cms.VPSet( ),   # hook to override or add single payloads
                               toGet = cms.VPSet(
    cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
             tag = cms.string("EcalIntercalibConstants_2010_V3"),
             connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
             ),
    #  ,cms.PSet(record = cms.string("EcalADCToGeVConstantRcd"),
    #          tag = cms.string("EcalADCToGeVConstant_v10_offline"),
    #          connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
    #         )
    # ,
    cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
             tag = cms.string("EcalLaserAPDPNRatios_data_20120131_158851_183320"),
             connect = cms.untracked.string("frontier://FrontierInt/CMS_COND_ECAL")
             ),
    #
    cms.PSet(record = cms.string('EcalLaserAlphasRcd'),
             tag = cms.string('EcalLaserAlphas_eflow_EBpermod_p01_EEpereta'),
             connect = cms.untracked.string('frontier://FrontierInt/CMS_COND_ECAL')
             )
    ),
                               BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )
