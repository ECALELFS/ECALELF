import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'),
                               globaltag = cms.string('FT_R_53_V21::All'),
                                   toGet = cms.VPSet(
                               	cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
             				tag = cms.string("EcalLaserAPDPNRatios_20130606_527_p1_v0"),
            				connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_42X_ECAL_LAS") 
            				),   
   				cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
         				tag = cms.string("EcalIntercalibConstants_V20131117_2012BCD_GL_TEST"),
            				connect = cms.untracked.string("frontier://FrontierInt/CMS_COND_ECAL")
            				),
    				cms.PSet(record = cms.string("EcalADCToGeVConstantRcd"),
            				tag = cms.string("EcalADCToGeVConstant_Bon_2012firstIOV"),
            				connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL") 
            				), 
    				cms.PSet(record = cms.string("EcalLaserAlphasRcd"),
            				tag = cms.string("EcalLaserAlphas_EB_185_EE_sic1_btcp116"),
            				connect = cms.untracked.string("frontier://FrontierInt/CMS_COND_ECAL") 
            				) 
                                 ),
                               BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )
