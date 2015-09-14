import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'),
                               globaltag = cms.string('FT_R_53_V21::All'),
                                   toGet = cms.VPSet(
                               	cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
         				tag = cms.string("EcalIntercalibConstants_V20131117_2012BCD_ESCorr_BL_TESTREF"),
            				connect = cms.untracked.string("frontier://FrontierInt/CMS_COND_ECAL")
            				),
    				cms.PSet(record = cms.string("EcalADCToGeVConstantRcd"),
            				tag = cms.string("EcalADCToGeVConstant_Bon_2012firstIOV"),
            				connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL") 
            				)
                                 ),
                               BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )
